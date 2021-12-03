/*
 *     ____             _________                __                _
 *    / __ \___  ____ _/ /_  __(_)___ ___  ___  / /   ____  ____ _(_)____
 *   / /_/ / _ \/ __ `/ / / / / / __ `__ \/ _ \/ /   / __ \/ __ `/ / ___/
 *  / _, _/  __/ /_/ / / / / / / / / / / /  __/ /___/ /_/ / /_/ / / /__
 * /_/ |_|\___/\__,_/_/ /_/ /_/_/ /_/ /_/\___/_____/\____/\__, /_/\___/
 *                                                       /____/
 *
 *                  Barracuda Application Server
 ****************************************************************************
 *            PROGRAM MODULE
 *
 *   $Id: SoDisp.c 4914 2021-12-01 18:24:30Z wini $
 *
 *   COPYRIGHT:  Real Time Logic, 2015 - 2020
 *
 *   This software is copyrighted by and is the sole property of Real
 *   Time Logic LLC.  All rights, title, ownership, or other interests in
 *   the software remain the property of Real Time Logic LLC.  This
 *   software may only be used in accordance with the terms and
 *   conditions stipulated in the corresponding license agreement under
 *   which the software has been supplied.  Any unauthorized use,
 *   duplication, transmission, distribution, or disclosure of this
 *   software is expressly forbidden.
 *                                                                        
 *   This Copyright notice may not be removed or modified without prior
 *   written consent of Real Time Logic LLC.
 *                                                                         
 *   Real Time Logic LLC. reserves the right to modify this software
 *   without notice.
 *
 *               http://www.realtimelogic.com
 ****************************************************************************
 *
 *
 * Platform: LwIP : netconn.
 *
 * Since the Barracuda App Server (BAS) is event driven, the design
 * chosen for the lwIP porting layer was to interface to the netconn
 * API (in event driven mode) instead of using the less efficient lwIP
 * BSD porting layer.
 *
 * The event driven API consists of two parts, an lwIP event callback
 * function (HttpSocket_netconnCB) and the socket dispatcher (SoDisp),
 * which must be powered by a dedicated thread. The SoDisp function
 * (SoDisp_run) waits for socket events signaled by the lwIP event
 * callback and dispatches the events as they trickle in where the
 * events are dispatched to the event driven BAS.
 */

#ifdef BA_LWIP

#include <HttpServer.h>
#include <BaServerLib.h>
#include <HttpTrace.h>
#include <lwip/tcp.h>
#include <lwip/init.h>
#if LWIP_VERSION_MAJOR < 2
#include <lwip/api_msg.h>
#else
#define LWIP_PROVIDE_ERRNO 
#include <lwip/errno.h>
#include <lwip/priv/api_msg.h>
#endif

#include <lwip/sockets.h>


#if LWIP_SO_RCVTIMEO != 1
#error LWIP_SO_RCVTIMEO must be set
#endif
#if LWIP_DNS != 1
#error LWIP_DNS must be set
#endif

/* See builds/FreeRTOS/README.txt for details on construction below:
 */
#ifdef USE_CALLBACKDATA
#define setCallBackData(ncon, httpSock) (ncon)->callbackData=httpSock
#define getCallBackData(ncon) (HttpSocket*)(ncon)->callbackData
#elif LWIP_SOCKET != 0
/* Cast the application's socket int member to/from HttpSocket* */
#define USING_INT_AS_VOID_PTR
#define setCallBackData(ncon, httpSock) (ncon)->socket=(int)httpSock
#define getCallBackData(ncon)  /* -1: closed */                 \
   (HttpSocket*)((ncon)->socket == -1 ? 0 : (ncon)->socket)
#else
#error Must set callBackData pointer in api.h or define LWIP_SOCKET=1
#endif

static SoDisp* disp; /* Set in the SoDisp_constructor. We can have only one instance! */

/* Enable in lwipopt.h (optional: not required) */
#if SO_REUSE
#else
#if USE_DBGMON
#error You may remove this error check but restarting server will result in using other server ports
#endif
#endif


/**************************************************************************
 *                              Error handler
 **************************************************************************
 */

extern UserDefinedErrHandler barracudaUserDefinedErrHandler;

/*
  Default BAS fatal error handler for lwIP
*/
void
baFatalEf(BaFatalErrorCodes ecode1, unsigned int ecode2,
            const char* file, int line)
{
#ifdef HTTP_TRACE
   static int recursiveCall=0;
   if( ! recursiveCall )
   {
      recursiveCall=1;
      HttpTrace_printf(0,"Fatal error detected in Barracuda.\n"
                       "E1 = %d, E2=%d\n"
                       "%s, line %d",
                       ecode1, ecode2,
                       file, line);
      HttpTrace_flush();
   }
#endif
   if(barracudaUserDefinedErrHandler)
      (*barracudaUserDefinedErrHandler)(ecode1, ecode2, file, line);
   for(;;) Thread_sleep(100000);
}


/**************************************************************************
 *                              HttpSockaddr
 **************************************************************************
 */


void
HttpSockaddr_gethostbyname(
   HttpSockaddr* o, const char* host, BaBool ip6, int* status)
{
   ip_addr_t addr;
   memset(o, 0, sizeof(HttpSockaddr));
   if(ip6)
   {
      *status = -1;
   }
   else if(host)
   {
      *status = netconn_gethostbyname(host, &addr);
   }
   if( ! *status )
   {
#if LWIP_IPV6==0
      memcpy(o->addr,&addr.addr,4);
#else
      memcpy(o->addr,&addr.u_addr.ip4,4);
#endif
   }
}


/**************************************************************************
 *                              HttpSocket
 **************************************************************************
 */

/* The HttpSocket object is an aggregate of the SoDispCon
   object. We can therefore calculate the offset position to the
   SoDispCon object from the HttpSocket object.

   Warning: The macro httpSocket2Con assumes all HttpSocket_xx
   interaction is via a SoDispCon object. The interface will fail
   if the HttpSocket is used directly -- i.e. if not an aggregate of
   SoDispCon.
*/
#define httpSocket2Con(httpSock) \
 ((SoDispCon*)(((U8*)httpSock)-offsetof(SoDispCon,httpSocket)))


/* lwIP race condition management used by function HttpSocket_accept
 * and HttpSocket_close. See comment in HttpSocket_netconnCB for
 * details.
 */
static SoDispAcceptQNode*
SoDispAcceptQ_findNode(SoDispAcceptQNode* ptr, struct netconn* ncon)
{
   int i;
   for(i=0 ; i < DISP_ACCEPT_Q_LEN ; i++,ptr++)
   {
      if(ncon == ptr->ncon) return ptr;
   }
   return 0;
}



/*
  All connections (including listening sockets) are event enabled by
  calling netconn_new_with_callback. The following function is called
  by lwIP for any new network activity for all connections used by
  BAS. The function records the event and signals SoDisp, which is
  responsible for dispatching network events.

  Each pending receive event is stored in variable 'recEvent' by
  increasing the count value. Send events, which is used by non
  blocking sockets, are simply stored as a boolean value since we can
  only have one pending send event with BAS. Error events are also
  stored as a boolean. If a valid event is detected, variable 
  'doEvent' is set, which in turn makes the callback signal SoDisp by
  setting the 'queueSem' semaphore.

  Some design elements such as the 'recEvent' are based on similar
  design found in the lwIP BSD implementation, which is also using the
  netconn API.
*/

static void
HttpSocket_netconnCB(struct netconn* ncon, enum netconn_evt evt, u16_t len)
{
   HttpSocket* o;
   int doEvent;
   SYS_ARCH_DECL_PROTECT(lev);
   (void)len;
   doEvent=FALSE;

   SYS_ARCH_PROTECT(lev);

   /* ON COMPILE ERROR -> Add to struct netconn: void* callbackData;
    * The BAS LwIP porting layer requires a handle (pointer) for the
    * HttpSocket.
    */
   o = getCallBackData(ncon);
   if(o)
   {
      switch(evt)
      {
         case NETCONN_EVT_ERROR:
            o->errEvent = TRUE;
            doEvent=TRUE;
            break;

         case NETCONN_EVT_RCVPLUS:
            o->recEvent++;
            doEvent=TRUE;
            break;

         case NETCONN_EVT_RCVMINUS:
            if(o->recEvent > 0) o->recEvent--;
            break;

         case NETCONN_EVT_SENDPLUS:
            o->sendEvent = TRUE;
            doEvent=TRUE;
            break;

         case NETCONN_EVT_SENDMINUS:
            o->sendEvent = FALSE;
            break;

         default: baAssert(0);
      }

      /* If: doEvent and object not already in 'soDispQueue'.
         else if already in queue:
         We may get multiple events before the SoDisp thread gets a
         chance to run, however, no event is lost since we use a
         counter for each recEvent. SoDisp will later keep running
         until all flags and the 'recEvent' counter is zero for this
         connection object.

       */
      if(doEvent && ! DoubleLink_isLinked(o) )
      {
         DoubleList_insertLast(&disp->soDispQueue, o);
         SYS_ARCH_UNPROTECT(lev);
         sys_sem_signal(&disp->queueSem);
         return;
      }
   }
   else
   {
      /* We get to this part in the code if we get an event for a
         netconn object with no HttpSocket handle.  This happens when
         the peer sends data immediately after connecting and the lwIP
         thread has a higher priority than the SoDisp thread. This
         race condition is managed in the code below by counting the
         events. This information is then used in HttpSocket_accept
         when the SoDisp thread starts running.
      */
      int i;
      SoDispAcceptQNode* ptr=disp->acceptQ;
      for(i=0 ; i < DISP_ACCEPT_Q_LEN ; i++,ptr++)
      {
         if(ncon == ptr->ncon) break;
      }
      if(i == DISP_ACCEPT_Q_LEN) /* if not found */
      {
         ptr=disp->acceptQ;
         for(i=0 ; i < DISP_ACCEPT_Q_LEN ; i++,ptr++)
         {
            if( ! ptr->ncon )
            {
               ptr->ncon=ncon;
               break;
            }
         }
      }
      if(ptr->ncon == ncon)
      {
         switch(evt)
         {
            case NETCONN_EVT_ERROR:
            case NETCONN_EVT_RCVPLUS:
               ptr->recEvent++;
               break;

            case NETCONN_EVT_RCVMINUS:
               ptr->recEvent--;
               if(ptr->recEvent == 0)
                  ptr->ncon=0;
               break;

            default:
               break;
         }
      }
   }
   SYS_ARCH_UNPROTECT(lev);
}


/* Activate socket after a blocking call if any of the event flags are set.
   ref-BT
*/
static void
HttpSocket_checkEvents(HttpSocket* o)
{
   if(o->recEvent || o->sendEvent || o->errEvent)
   {
      /*
        Insert socket into soDispQueue if not already in the queue and
        signal SoDisp
      */

      SYS_ARCH_DECL_PROTECT(lev);
      SYS_ARCH_PROTECT(lev);
      if( ! DoubleLink_isLinked(o) )
      {
         DoubleList_insertLast(&disp->soDispQueue, o);
         SYS_ARCH_UNPROTECT(lev);
         sys_sem_signal(&disp->queueSem);
      }
      else if( ! DoubleList_isEmpty(&disp->soDispQueue) )
      {
         SYS_ARCH_UNPROTECT(lev);
         sys_sem_signal(&disp->queueSem);
      }
      else
      {
         SYS_ARCH_UNPROTECT(lev);
      }
   }
}


void
HttpSocket_setBlocking(HttpSocket* o, int* status)
{
   if(!o->ncon)
      *status = E_SOCKET_CLOSED;
   else
   {
      netconn_set_nonblocking(o->ncon, FALSE);
      *status = 0;
   }
}


void
HttpSocket_setNonblocking(HttpSocket* o, int* status)
{
   if(!o->ncon)
      *status = E_SOCKET_CLOSED;
   else
   {
      netconn_set_nonblocking(o->ncon, TRUE);
      *status = 0;
   }
}


void
HttpSocket_wouldBlock(HttpSocket* o, int* status)
{
   *status = o->ncon && *status == EWOULDBLOCK;
}


void
HttpSocket_setTCPNoDelay(HttpSocket* o, int enable, int* status)
{
   if(o->ncon && o->ncon->pcb.tcp)
   {
      *status = 0;
      if(enable)
         tcp_nagle_enable(o->ncon->pcb.tcp); 
      else
         tcp_nagle_disable(o->ncon->pcb.tcp); 
   }
   else
   {
      *status = E_SOCKET_CLOSED;
   }
}


void
HttpSocket_sockStream(HttpSocket* o,const char* host,BaBool ip6,int* status)
{
   (void)host;
   *status = -1;
   if(!o->ncon && !ip6)
   {
      o->recEvent = 0;
      o->sendEvent = o->errEvent = FALSE;
      o->ncon = netconn_new_with_callback(NETCONN_TCP, HttpSocket_netconnCB);
      if(o->ncon)
      {
         setCallBackData(o->ncon, o);
         *status = 0;
      }
   }
}


void
HttpSocket_bind(HttpSocket* o,HttpSockaddr* addr,U16 port,int* status)
{
   (void)addr; /* Not implemented. Note LwIP uses pointers for comp. */
   if(o->ncon)
   {
      ip_set_option(o->ncon->pcb.ip, SO_REUSEADDR);
   }
   *status = (!o->ncon || netconn_bind(o->ncon,IP_ADDR_ANY,port)) ? -1 : 0;
}


void
HttpSocket_listen(HttpSocket* o, HttpSockaddr* addr, int len, int* status)
{
   (void)addr;
   *status = (!o->ncon || netconn_listen_with_backlog(o->ncon, len)) ? -1 : 0;
}


void
HttpSocket_connect(HttpSocket* o,HttpSockaddr* addr,U16 port,int* status)
{
   ip_addr_t ia;
   if(!o->ncon || addr->isIp6) *status = -1;
#if LWIP_IPV6==0
   memcpy(&ia.addr, addr->addr, 4);
#else
   memcpy(&ia.u_addr.ip4,addr->addr, 4);
   IP_SET_TYPE_VAL(ia, IPADDR_TYPE_V4);
#endif
   *status=netconn_connect(o->ncon, &ia, port);
}


void
HttpSocket_accept(HttpSocket* o,HttpSocket* newSock,int* status)
{
   if(o->ncon && ! netconn_accept(o->ncon, &newSock->ncon))
   {
      SoDispAcceptQNode* acn = SoDispAcceptQ_findNode(
         disp->acceptQ, newSock->ncon);
      setCallBackData(newSock->ncon, newSock);
      *status=0;
      if(acn) /* if pending recv events */
      {
         newSock->recEvent = acn->recEvent;
         acn->ncon=0;
         acn->recEvent=0;
      }
   }
   else
      *status=-1;
}


void
HttpSocket_close(HttpSocket* o)
{
   
   struct netconn* ncon;
   SYS_ARCH_DECL_PROTECT(lev);
   SYS_ARCH_PROTECT(lev);
   ncon = o->ncon;
   if(ncon)
   {
      SoDispAcceptQNode* acn = SoDispAcceptQ_findNode(disp->acceptQ, ncon);
      if(acn)
      {
         acn->ncon=0;
         acn->recEvent=0;
      }
      setCallBackData(ncon,0);
      ncon->callback = 0;
      o->ncon = 0;
   }
   if(DoubleLink_isLinked(o))
      DoubleLink_unlink((DoubleLink*)o);
   SYS_ARCH_UNPROTECT(lev);
   if(ncon)
   {
      /* Dispatcher loop needs to know if the active socket terminated
       */
      if(o == disp->curSock)
         disp->curSock=0;
      netconn_close(ncon);
      netconn_delete(ncon);
   }
   if(o->pb)
   {
      pbuf_free(o->pb);
      o->pb=0;
   }
   o->recEvent = 0;
   o->errEvent = o->sendEvent = FALSE;
}

U32
HttpSocket_getId(HttpSocket* o)
{
   return (U32) o->ncon;
}


void
HttpSocket_getAddr(HttpSocket* o, HttpSockaddr* addr, U16* port,
                   BaBool ip6, int* status, int local)
{
   ip_addr_t ia;
   U16 portBuf;
   if(!port) port=&portBuf;
   if(o->ncon && !ip6 && ERR_OK == netconn_getaddr(o->ncon,&ia,port,local))
   {
#if LWIP_IPV6==0
      memcpy(addr->addr,&ia.addr,4);
#else
      memcpy(addr->addr,&ia.u_addr.ip4,4);
#endif
      *status = 0;
      addr->isIp6=FALSE;
   }
   else
      *status = -1;
}


void
HttpSocket_move(HttpSocket* o,HttpSocket* newS)
{
   SYS_ARCH_DECL_PROTECT(lev);
   baAssert( ! newS->ncon );
   baAssert( o->ncon );
   baAssert(o == getCallBackData(o->ncon));

   SYS_ARCH_PROTECT(lev);
   newS->ncon=o->ncon;
   o->ncon=0;
   newS->pb = o->pb;
   newS->pbOffs = o->pbOffs;
   newS->recEvent = o->recEvent;
   newS->sendEvent = o->sendEvent;
   newS->errEvent = o->errEvent;
   setCallBackData(newS->ncon, newS);
   if(DoubleLink_isLinked(o))
      DoubleLink_unlink((DoubleLink*)o);
   SYS_ARCH_UNPROTECT(lev);
   if(o == disp->curSock)
      disp->curSock=0;
   o->pb=0;
   o->pbOffs=0;
   o->recEvent=0;
   o->sendEvent = o->errEvent = FALSE;
}


void HttpSocket_send(HttpSocket* o, struct ThreadMutex* m, BaBool* isTerminated,
                const void* data, int len, int* retLen)
{
   err_t err=ERR_ARG;
   if(o->errEvent)
   {
      *retLen=E_SOCKET_CLOSED;
      return;
   }
   if(!netconn_is_nonblocking(o->ncon))
   {
      if(m && ThreadMutex_isOwner(m))
      {
         ThreadMutex_release(m);
         err=netconn_write(o->ncon, data, len, NETCONN_COPY);
         ThreadMutex_set(m);
      }
      else
      {
         err=netconn_write(o->ncon, data, len, NETCONN_COPY);
      }
      if(*isTerminated)
      {
         *retLen=E_SOCKET_CLOSED;
         return;
      }

      HttpSocket_checkEvents(o);
   }
   else
   {
      int tslen=0; /* total sent len */
      while(tslen < len)
      {
         size_t slen=0;
         err=netconn_write_partly(
            o->ncon, (U8*)data+tslen, len-tslen, NETCONN_COPY, &slen);
         if(err)
         {
            if(ERR_WOULDBLOCK == err)
            {
               *retLen = tslen+(int)slen;
               return;
            }
            break;
         }
         tslen += (int)slen;
      }
      baAssert(err || len == (int)tslen);
   }
   if(ERR_OK != err)
      *retLen = E_SOCKET_WRITE_FAILED;
   else
      *retLen = len;
}


#if LWIP_TCP_KEEPALIVE == 1
void
_HttpSocket_getKeepAlive(HttpSocket* o, int* enable, int* status)
{
   if(o->ncon)
   {
      *enable = ip_get_option(o->ncon->pcb.ip, SOF_KEEPALIVE);
      *status=0;
   }
   else
      *status=E_SOCKET_CLOSED;
}


void
_HttpSocket_setKeepAlive(HttpSocket* o, int enable, int* status)
{
   if(o->ncon)
   {
      if(enable)
         ip_set_option(o->ncon->pcb.ip, SOF_KEEPALIVE);
      else
         ip_reset_option(o->ncon->pcb.ip, SOF_KEEPALIVE);
      *status=0;
   }
   else
      *status=E_SOCKET_CLOSED;
}


void
_HttpSocket_getKeepAliveEx(
   HttpSocket* o,int* enable,int* time,int* interval,int* status)
{
   if(o->ncon)
   {
      *enable = ip_get_option(o->ncon->pcb.ip,SOF_KEEPALIVE);
      *time = (int)(o->ncon->pcb.tcp->keep_idle/1000);
      *interval = (int)(o->ncon->pcb.tcp->keep_intvl/1000);
      *status=0;
   }
   else
      *status=E_SOCKET_CLOSED;
}


void
_HttpSocket_setKeepAliveEx(
   HttpSocket* o,int enable,int time, int interval,int* status)
{
   if(o->ncon)
   {
      if(enable)
      {
         ip_set_option(o->ncon->pcb.ip, SOF_KEEPALIVE);
         o->ncon->pcb.tcp->keep_idle = time*1000;
         o->ncon->pcb.tcp->keep_intvl = interval*1000;
      }
      else
         ip_reset_option(o->ncon->pcb.ip, SOF_KEEPALIVE);
      *status=0;
   }
   else
      *status=E_SOCKET_CLOSED;
}
#endif


/* Platform specific socket read.
  returns:
    > zero: returns data len
    zero: no data (would block)
    < zero: an error code
 */
int
SoDispCon_platReadData(
   SoDispCon* o, ThreadMutex* m, BaBool* isTerminated, void* data, int dlen)
{
   int offset = 0;
   HttpSocket* sock = &o->httpSocket;
   int releaseMutex =
      m && ThreadMutex_isOwner(m) && ! SoDispCon_isNonBlocking(o);
   if(sock->errEvent || !sock->ncon)
      return E_SOCKET_CLOSED;
   baAssert((netconn_is_nonblocking(sock->ncon) == 0) ==
            (SoDispCon_isNonBlocking(o) == 0));
   for(;;)
   {
      int rlen;
      if( ! sock->pb )
      {
         err_t err;
         sock->pbOffs = 0;
         if(releaseMutex)
         {
            ThreadMutex_release(m);
            /* rtmo 0 means wait forever, same concept as used by netconn */
            netconn_set_recvtimeout(sock->ncon, o->rtmo*50);
            err = netconn_recv_tcp_pbuf(sock->ncon, &sock->pb);
            ThreadMutex_set(m);
            if(*isTerminated)
               return E_SOCKET_READ_FAILED;
            HttpSocket_checkEvents(&o->httpSocket);
         }
         else if(sock->recEvent)
         {
            err = netconn_recv_tcp_pbuf(sock->ncon, &sock->pb);
            if(ERR_OK != err)
               sock->recEvent=0; /* Ref-spurious */
         }
         else if(! SoDispCon_isNonBlocking(o) )
         { /* Special case where mutex is not released, but we
            * block. Used by for example the debug monitor. */
            netconn_set_recvtimeout(sock->ncon, o->rtmo*50);
            err = netconn_recv_tcp_pbuf(sock->ncon, &sock->pb);
         }
         else
         {
            SoDispCon_clearSocketHasNonBlockData(o);
            return offset; /* zero means no data */
         }
         if(ERR_OK != err)
         {  
            /* printf("Sock rec err %d, %p\n",err,sock->pb); */
            sock->pb=0; /* this was set in an lwIP version on 'err' ?? */
            SoDispCon_clearSocketHasNonBlockData(o);
            if(offset)
            {
               sock->errEvent = TRUE;
               return offset; /* Error, but we have data */
            }
            switch(err)
            {
               case ERR_TIMEOUT:
                  return E_TIMEOUT;
               case ERR_WOULDBLOCK:
                  return 0; /* no data */
               case ERR_CLSD:
               case ERR_RST:
                  return E_SOCKET_CLOSED;
            }
            return E_SOCKET_READ_FAILED;
         }
      }
      rlen=(int)pbuf_copy_partial(sock->pb,(U8*)data+offset,dlen,sock->pbOffs);
      if(!rlen)
         return E_SOCKET_READ_FAILED;
      sock->pbOffs += rlen;
      offset += rlen;
      dlen -= rlen;
      baAssert(dlen >= 0 && sock->pbOffs <= sock->pb->tot_len);
      if(sock->pbOffs >= sock->pb->tot_len)
      {
         pbuf_free(sock->pb);
         sock->pb=0;
         if(sock->recEvent)
         {
            if(dlen > 0)
               continue;
         }
         else
            SoDispCon_clearSocketHasNonBlockData(o);
      }
      return offset;
   }
}



void
SoDisp_newCon(SoDisp* o, struct SoDispCon* con)
{
   (void)o;
   (void)con;
   baAssert( ! con->httpSocket.pb );
   DoubleLink_constructor(&con->httpSocket);

}


void
SoDisp_addConnection(SoDisp* o, SoDispCon* con)
{
   (void)o;
   (void)con;
   baAssert(!SoDispCon_dispatcherHasCon(con));
   SoDispCon_setDispatcherHasCon(con);
}


void
SoDisp_activateRec(SoDisp* o, SoDispCon* con)
{
   (void)o;
   baAssert(SoDispCon_isValid(con));
   baAssert(!SoDispCon_recEvActive(con));
   SoDispCon_setRecEvActive(con);
   HttpSocket_checkEvents(&con->httpSocket);
}


void
SoDisp_deactivateRec(SoDisp* o, SoDispCon* con)
{
   (void)o;
   baAssert(SoDispCon_recEvActive(con));
   SoDispCon_setRecEvInactive(con);
}


void
SoDisp_activateSend(SoDisp* o, SoDispCon* con)
{
   (void)o;
   baAssert(!SoDispCon_sendEvActive(con));
   SoDispCon_setSendEvActive(con);
   HttpSocket_checkEvents(&con->httpSocket);
}


void
SoDisp_deactivateSend(SoDisp* o, SoDispCon* con)
{
   (void)o;
   baAssert(SoDispCon_sendEvActive(con));
   SoDispCon_setSendEvInactive(con);
}


void
SoDisp_removeConnection(SoDisp* o, SoDispCon* con)
{
   (void)o;
   (void)con;
   baAssert(SoDispCon_dispatcherHasCon(con));
   baAssert(!SoDispCon_recEvActive(con));
   baAssert(!SoDispCon_sendEvActive(con));
   SoDispCon_clearDispatcherHasCon(con);
}


void
SoDisp_constructor(SoDisp* o, ThreadMutex* mutex)
{
   memset(o, 0, sizeof(SoDisp));
   DoubleList_constructor(&o->soDispQueue);
   sys_sem_new(&o->queueSem, 0);
   o->mutex=mutex;
   disp=o;
}


/* A dedicated thread must run this function. The function waits for
 * events signaled by HttpSocket_netconnCB. The events are dispatched
 * by calling the receive/send SoDispCon callback functions.
 */
void
SoDisp_run(SoDisp* o, S32 msecTimeout)
{
#ifdef USING_INT_AS_VOID_PTR
   baAssert((sizeof(int) >= sizeof(void*)));
#endif
   o->doExit = FALSE;
   if(disp != o)
   {
      /* Max one SoDisp instance */
      baFatalE(FE_INCORRECT_USE, __LINE__);      
   }
   while( ! o->doExit )
   {
      HttpSocket* sock;
      if(DoubleList_isEmpty(&o->soDispQueue) && 
         sys_arch_sem_wait(&o->queueSem, msecTimeout) == SYS_ARCH_TIMEOUT &&
         msecTimeout > 0)
      {
         return;
      }
      for(;;)
      {
         SoDispCon* con;
         SYS_ARCH_DECL_PROTECT(lev);
         SoDisp_mutexSet(o); /* Dispatcher mutex: multithreaded server. */
         SYS_ARCH_PROTECT(lev);
         sock = (HttpSocket*)DoubleList_removeFirst(&o->soDispQueue);
         SYS_ARCH_UNPROTECT(lev);
         if(!sock)
            break;
         con = httpSocket2Con(sock);
         /* SoDispCon::recTermPtr is set if another thread is in a
          * blocking read or write call. LwIP cannot handle multiple
          * threads. Ref-BT
          */
         if( ! con->recTermPtr )
         {
            o->curSock = sock;
            /* While not closed and receive state is active */
            while(sock == o->curSock)
            {
               /* Dispatch active events by calling the callback functions */
               if((sock->sendEvent || sock->errEvent) &&
                  SoDispCon_sendEvActive(con))
               {
                  SoDispCon_dispSendEvent(con);
                  if(sock == o->curSock &&
                     (sock->recEvent || sock->errEvent) &&
                     SoDispCon_recEvActive(con))
                  {
                     SoDispCon_setDispHasRecData(con);
                     SoDispCon_dispRecEvent(con);
                  }
               }
               else if((sock->recEvent || sock->errEvent) &&
                       SoDispCon_recEvActive(con))
               {
                  SoDispCon_setDispHasRecData(con);
                  SoDispCon_dispRecEvent(con);
               }
               else
               {
                  break;
               }
            }
         }
         SoDisp_mutexRelease(o);
      }
      SoDisp_mutexRelease(o); /* From 'break' above */
   }
}

#endif /* BA_LWIP */
