/*
 *     ____             _________                __                _     
 *    / __ \___  ____ _/ /_  __(_)___ ___  ___  / /   ____  ____ _(_)____
 *   / /_/ / _ \/ __ `/ / / / / / __ `__ \/ _ \/ /   / __ \/ __ `/ / ___/
 *  / _, _/  __/ /_/ / / / / / / / / / / /  __/ /___/ /_/ / /_/ / / /__  
 * /_/ |_|\___/\__,_/_/ /_/ /_/_/ /_/ /_/\___/_____/\____/\__, /_/\___/  
 *                                                       /____/          
 *
 *                  Barracuda Embedded Web-Server 
 ****************************************************************************
 *            PROGRAM MODULE
 *
 *   $Id: SoDisp.c 5554 2024-08-21 07:06:58Z wini $
 *
 *   COPYRIGHT:  Real Time Logic, 2009 - 2024
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
 ******************* *********************************************************
 *
 */

#include <HttpServer.h>
#include <BaServerLib.h>
#include <HttpTrace.h>
#include <stddef.h>

#ifdef BA_NX_DNS
#ifdef FEATURE_NX_IPV6
#include <nxd_dns.h>
#else
#include <nx_dns.h>
#endif
#endif



#if __NETXDUO_MAJOR_VERSION__ < 5
#define BA_OLD_NX_API
#elif __NETXDUO_MAJOR_VERSION__ == 5 && __NETXDUO_MINOR_VERSION__ < 6
#define BA_OLD_NX_API
#endif


/* Initialized with SoDisp_nxInit
*/
static  NX_IP** nxIpList;
static  NX_PACKET_POOL* nxPacketPool;
static AllocatorIntf* nxTcpSockAlloc; /* sizeof(NX_TCP_SOCKET) allocator */
ULONG nxWindowSize;

#ifdef BA_NX_DNS
static NX_DNS* nxdns;
#endif

/**************************************************************************
 *                              Error handler
 **************************************************************************
 */

extern UserDefinedErrHandler barracudaUserDefinedErrHandler;

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
 *                              HttpSocket
 **************************************************************************
 */

/* The HttpSocket object is an aggregate of the SoDispCon
 * object. We can therefore calculate the offset position to the
 * SoDispCon object from the HttpSocket object.
 */ 

/*
  Warning: The macro httpSocket2Con assumes all HttpSocket_xx
  interaction is via a SoDispCon object. The NetX interface will fail
  if the HttpSocket is used directly -- i.e. if not an aggregate of
  SoDispCon.
*/
#define httpSocket2Con(httpSock) \
 ((SoDispCon*)(((U8*)httpSock)-offsetof(SoDispCon,httpSocket)))

/* Convert a HttpSocket to a SoDispCon and fetch the Dispatcher
 * object.
*/  
#define HttpSocket_getDispatcher(o) \
  SoDispCon_getDispatcher(httpSocket2Con(o))

static void
HttpSocket_sendSockEv2Disp(NX_TCP_SOCKET *nxSock, BaBool close)
{
   HttpSocket* o;
   SoDisp* disp;
   SoDispCon* con;
   BaBool sendSig;
   TX_INTERRUPT_SAVE_AREA;

   TX_DISABLE; /* Protecting asynch TX callback -- i.e. this function. */
   o = (HttpSocket*)nxSock->nx_tcp_socket_reserved_ptr;
   con = httpSocket2Con(o);
   disp = SoDispCon_getDispatcher(con);
   if( (close || SoDispCon_recEvActive(con)) && ! DoubleLink_isLinked(o) )
   {
      DoubleList_insertLast(&disp->soDispQueue, o);
      sendSig = TRUE;
   }
   else
      sendSig = FALSE;
   TX_RESTORE;
   if(sendSig)
      TST_OK(tx_semaphore_put(&disp->queueSem));
}


static void
HttpSocket_disconnectCB(NX_TCP_SOCKET *nxSock)
{
   HttpSocket_sendSockEv2Disp(nxSock, TRUE);
}


static void
HttpSocket_receiveCB(NX_TCP_SOCKET *nxSock)
{
   HttpSocket_sendSockEv2Disp(nxSock, FALSE);
}


static void
HttpSocket_listenCB(NX_TCP_SOCKET *nxSock, UINT port)
{
   HttpSocket* o=(HttpSocket*)nxSock->nx_tcp_socket_reserved_ptr;
   /* REF-LS.  
      Must perform "accept" immediately just in case we get a SYN-RST
      before SoDisp runs. We use the nxPacket pointer to store the
      return value, which is later used in HttpSocket_accept. This
      pointer is not used by listening sockets. Note that we make sure
      the value cannot be zero by adding one. This is necessary in order to
      prevent function SoDisp_run from attempting to read data on the
      socket.
   */
   if(o->nxPacket == 0) /* if not in use */
   {
      o->nxPacket=
         (NX_PACKET*)(nx_tcp_server_socket_accept(nxSock, NX_NO_WAIT)+1);
      HttpSocket_sendSockEv2Disp(nxSock, FALSE);
   }
}



void
HttpSocket_sockStream(HttpSocket* o,const char* host,BaBool ip6,int* status)
{
   HttpSockaddr addr;
   baAssert( ! o->nxSock );
   HttpSockaddr_gethostbyname(&addr, host, ip6, status);
   if( ! *status )
   {
      size_t s = sizeof(NX_TCP_SOCKET);
      o->nxSock = (NX_TCP_SOCKET*)nxTcpSockAlloc->mallocCB(nxTcpSockAlloc, &s);
      if(o->nxSock)
      {
         memset(o->nxSock, 0, sizeof(NX_TCP_SOCKET));
         *status = nx_tcp_socket_create(addr.ip,
                                        o->nxSock,
                                        "BA",
                                        NX_IP_NORMAL,
                                        NX_FRAGMENT_OKAY,
                                        NX_IP_TIME_TO_LIVE,
                                        nxWindowSize,
                                        NX_NULL,
                                        HttpSocket_disconnectCB);
         o->nxSock->nx_tcp_socket_reserved_ptr = o; /*Needed by NX callbacks*/
         if(*status)
         {
            nxTcpSockAlloc->freeCB(nxTcpSockAlloc, o->nxSock);
            o->nxSock=0;
         }
      }
      else
         *status = E_MALLOC;
   }
}


void
HttpSocket_bind(HttpSocket* o, HttpSockaddr* addr, UINT port, int* status)
{
   *status = nx_tcp_client_socket_bind(o->nxSock, port, NX_NO_WAIT);
}


void
HttpSocket_listen(HttpSocket* o, HttpSockaddr* addr,int queueSize, int* status)
{
   UINT port = o->nxSock->nx_tcp_socket_port;
   /*  Bind is used for client and server so listen does an unbind
       before listening.
   */
   nx_tcp_client_socket_unbind(o->nxSock);
   *status = nx_tcp_server_socket_listen(
      o->nxSock->nx_tcp_socket_ip_ptr,
      port,
      o->nxSock,
      queueSize,
      HttpSocket_listenCB);
}


void
HttpSocket_connect(HttpSocket* o, HttpSockaddr* addr, UINT port, int* status)
{
   U32 ipAddr = *((U32*)addr->addr);
   /* Most network stacks use Network endian (big endian), but not NetX.
      Convert back to local endian on little endian machines
   */
   NX_CHANGE_ULONG_ENDIAN(ipAddr);
   if(o->nxSock)
   {
      /* If not bound */
      if( ! o->nxSock->nx_tcp_socket_bound_next )
      {
         *status = nx_tcp_client_socket_bind(o->nxSock,NX_ANY_PORT,NX_NO_WAIT);
         if(*status)
            return;
      }
      *status = nx_tcp_client_socket_connect(o->nxSock, ipAddr, port, 500);
      if( ! *status )
      {
         nx_tcp_socket_receive_notify(o->nxSock,HttpSocket_receiveCB);
      }
   }
   else
      *status = -1;
}


void
HttpSocket_accept(HttpSocket* o, HttpSocket* conSock, int* status)
{
   int nxStatus;
   SoDisp* disp = HttpSocket_getDispatcher(o);
   baAssert( ! conSock->nxSock );
   baAssert( o->nxSock );
   baAssert( o->nxSock->nx_tcp_socket_reserved_ptr == (void*)o );
   baAssert(disp->curSock == o);
   baAssert(o->nxFirstPacket==0);
   baAssert(conSock->nxPacket==0);
   baAssert( ! DoubleLink_isLinked(conSock) );

   disp->curSock=0;

   /* The NetX porting layer is not designed for failing with a return
      code since this would make the caller call accept again. If any
      errors, the code returns OK, but later fails in socket read.
   */
   *status=0;

   /* nxPacket is status + 1 (See REF-LS), thus we got a double event if zero */
   if(o->nxPacket == 0)
   {
      return; /* Do nothing */
   }
   nxStatus = ((int)o->nxPacket) - 1; /* See REF-LS */

   /* Both successful accept and a pending accept are treated as
      "successful". The state should normally be NX_IN_PROGRESS since
      we use a zero timeout for accept in function HttpSocket_listenCB.
   */
   if(nxStatus == NX_SUCCESS || nxStatus == NX_IN_PROGRESS)
   {
      size_t s = sizeof(NX_TCP_SOCKET);
      NX_TCP_SOCKET* newNxListenSock = (NX_TCP_SOCKET*)
         nxTcpSockAlloc->mallocCB(nxTcpSockAlloc, &s);
      if(newNxListenSock)
      {
         nxStatus = nx_tcp_socket_create(
            o->nxSock->nx_tcp_socket_ip_ptr,
            newNxListenSock,
            "BA",
            NX_IP_NORMAL,
            NX_FRAGMENT_OKAY,
            NX_IP_TIME_TO_LIVE,
            nxWindowSize,
            NX_NULL,
            HttpSocket_disconnectCB);
         if(nxStatus == NX_SUCCESS)
         {
            newNxListenSock->nx_tcp_socket_reserved_ptr = o;
            conSock->nxSock=o->nxSock;
            conSock->nxSock->nx_tcp_socket_reserved_ptr = conSock;
            o->nxSock = newNxListenSock;
            /* Function nx_tcp_server_socket_relisten may call
               HttpSocket_listenCB if there are pending SYN packets.
               It's OK to call HttpSocket_listenCB at this point since
               we have set the nx_tcp_socket_reserved_ptr.
               See See REF-LS.
            */
            o->nxPacket=0; /* Enable HttpSocket_listenCB */
            nxStatus = nx_tcp_server_socket_relisten(
               conSock->nxSock->nx_tcp_socket_ip_ptr,
               conSock->nxSock->nx_tcp_socket_port,
               newNxListenSock);
            if(nxStatus == NX_SUCCESS || nxStatus == NX_CONNECTION_PENDING)
            {
               nx_tcp_socket_receive_notify(
                  conSock->nxSock,HttpSocket_receiveCB);
               nxStatus=0;
               return;
            }
            nx_tcp_socket_delete(newNxListenSock);
            o->nxSock=conSock->nxSock;
            o->nxSock->nx_tcp_socket_reserved_ptr = o;
            conSock->nxSock=0;
            nxStatus=0; /* Prevent caller from killing idle connections */
         }
         nxTcpSockAlloc->freeCB(nxTcpSockAlloc, newNxListenSock);
      }
   }

   o->nxPacket=0; /* Enable HttpSocket_listenCB. See See REF-LS */
   /* This ensures that the socket that fails to accept is being
    * cleaned up.
    */
   nx_tcp_socket_disconnect(o->nxSock, NX_NO_WAIT);
   nx_tcp_server_socket_unaccept(o->nxSock);
   if( (nxStatus=nx_tcp_server_socket_relisten(o->nxSock->nx_tcp_socket_ip_ptr,
                                              o->nxSock->nx_tcp_socket_port,
                                              o->nxSock)) != NX_SUCCESS)
   {
      baFatalE(FE_ACCEPT, nxStatus);
   }
}


void
HttpSocket_nxClose(HttpSocket* o, int hardClose)
{
   NX_IP* ip;
   NX_TCP_SOCKET* nxs;
   SoDisp* disp = HttpSocket_getDispatcher(o);
   TX_INTERRUPT_SAVE_AREA;

   /* Dispatcher loop needs to know if the active socket terminated
    */
   if(disp && o == disp->curSock)
      disp->curSock=0;

   TX_DISABLE; /* Protecting asynch TX callback */
   if(DoubleLink_isLinked(o))
      DoubleLink_unlink(o); /* If in SoDisp queueu */
   TX_RESTORE;

   if( !o->nxSock )
      return; /* Already closed */

   nxs = o->nxSock;

   ip = nxs->nx_tcp_socket_ip_ptr;

   /* First, make sure we stop all callbacks.
    */
   nx_tcp_socket_receive_notify(nxs, 0);

   /* Any saved packets from SoDispCon_platReadData */
   if(o->nxPacket)
   {
      TST_OK(nx_packet_release(o->nxFirstPacket));
      o->nxPacket=0;
   }

   if(nxs->nx_tcp_socket_client_type)
   {
      nx_tcp_socket_disconnect(nxs, NX_NO_WAIT);
      nx_tcp_client_socket_unbind(nxs);
   }
   else
   { /* Server sock */

       /* The following must be enabled for NetX versions older than
        * 6.2.1. In version 6.2.1, function
        * _nx_tcp_socket_block_cleanup sets nx_tcp_socket_state to
        * NX_TCP_LISTEN_STATE if the socket type is 'Server'. See also
        * NetX documentation example to services
        * nx_tcp_server_socket_listen and
        * nx_tcp_server_socket_relisten
        */
#if 0
      if(nxs->nx_tcp_socket_state ==  NX_TCP_LISTEN_STATE)
      {
         /* We do not know the state of the socket at this point. The socket
          * could be in any state. The first thing to do is to check if this
          * is a listen socket. It can only be a listen socket if the user
          * terminates an active HttpServCon object.
          */
         nx_tcp_server_socket_unlisten(ip,nxs->nx_tcp_socket_port);
      }
      else /* Attempt to close the socket */
#endif
      {
         /* Macro HttpSocket_hardClose sets hardClose to one and
            HttpSocket_close sets hardClose to zero. A hard close should
            make the TCP stack send RST to the peer and return
            immediately.
         */
         nx_tcp_socket_disconnect(nxs, hardClose ? NX_NO_WAIT : 100);
      }
      nx_tcp_server_socket_unaccept(nxs);
   }
   TST_OK(nx_tcp_socket_delete(nxs));

   nxTcpSockAlloc->freeCB(nxTcpSockAlloc, nxs);
   o->nxSock = 0; /* invalidate */
}


U32
HttpSocket_getId(HttpSocket* o)
{
   return (U32)o->nxSock;
}


void HttpSocket_getPeerName(
   HttpSocket* o, HttpSockaddr* sockaddr, U16* port, BaBool ip6, int* status)
{
   U32 ipAddr;
   if(o->nxSock)
   {
      *status=0;
      sockaddr->ip = o->nxSock->nx_tcp_socket_ip_ptr;
      if(port) *(port)=baHtons(o->nxSock->nx_tcp_socket_port);
#ifdef FEATURE_NX_IPV6
      if(o->nxSock->nx_tcp_socket_connect_ip.nxd_ip_version==NX_IP_VERSION_V6)
      {
         sockaddr->isIp6=TRUE;
         memcpy(sockaddr->addr, o->nxSock->nx_tcp_socket_connect_ip.nxd_ip_address.v6, 16);
      }
      else
      {
         sockaddr->isIp6=FALSE;
         ipAddr = o->nxSock->nx_tcp_socket_connect_ip.nxd_ip_address.v4;
         NX_CHANGE_ULONG_ENDIAN(ipAddr); /* See comment in HttpSocket_connect */
         *((U32*)sockaddr->addr) = ipAddr;
      }
#else
      sockaddr->isIp6=FALSE;
#ifdef BA_OLD_NX_API
      ipAddr = o->nxSock->nx_tcp_socket_connect_ip;
#else
      ipAddr = o->nxSock->nx_tcp_socket_connect_ip.nxd_ip_address.v4;
#endif
      NX_CHANGE_ULONG_ENDIAN(ipAddr); /* See comment in HttpSocket_connect */
      *((U32*)sockaddr->addr) = ipAddr;
#endif
      *status=0;
   }
   else
      *status=-1;
}


void
HttpSocket_getSockName(
HttpSocket* o, HttpSockaddr* sockaddr, U16* port, BaBool ip6, int* status) 
{
   U32 ipAddr;
   if(o->nxSock)
   {
      *status=0;
      sockaddr->ip = o->nxSock->nx_tcp_socket_ip_ptr;
      if(port) *(port)=baHtons(o->nxSock->nx_tcp_socket_port);
#ifdef FEATURE_NX_IPV6
      if(ip6)
      {
         sockaddr->isIp6=TRUE;
#ifdef BA_OLD_NX_API
         memcpy(sockaddr->addr, sockaddr->ip->nx_ipv6_global.nxd_ipv6_address, 16);
#else
         memcpy(sockaddr->addr, o->nxSock->nx_tcp_socket_ipv6_addr->nxd_ipv6_address, 16);
#endif
      }
      else
      {
         sockaddr->isIp6=FALSE;
#ifdef BA_OLD_NX_API
         ipAddr = sockaddr->ip->nx_ip_address;
#else
         ipAddr = sockaddr->ip->nx_ip_interface[0].nx_interface_ip_address;
#endif
         NX_CHANGE_ULONG_ENDIAN(ipAddr); /* See comment in HttpSocket_connect */
         *((U32*)sockaddr->addr) = ipAddr;
      }
#else
      sockaddr->isIp6=FALSE;
#ifdef BA_OLD_NX_API
      ipAddr = sockaddr->ip->nx_ip_address;
#else
      ipAddr = sockaddr->ip->nx_ip_interface[0].nx_interface_ip_address;
#endif
      NX_CHANGE_ULONG_ENDIAN(ipAddr); /* See comment in HttpSocket_connect */
      *((U32*)sockaddr->addr) = ipAddr;
#endif
      *status=0;
   }
   else
      *status=-1;
}


void
HttpSocket_move(HttpSocket* o, HttpSocket* newS)
{
   SoDisp* disp = HttpSocket_getDispatcher(o);
   TX_INTERRUPT_SAVE_AREA;
   baAssert( ! newS->nxSock );
   baAssert( o->nxSock );
   baAssert( o->nxSock->nx_tcp_socket_reserved_ptr == (void*)o );

   TX_DISABLE; /* Protecting asynch TX callback */
   if(DoubleLink_isLinked(o))
   {
      DoubleLink_unlink(o); /* If in SoDisp queueu */
      DoubleList_insertLast(&disp->soDispQueue, newS);
   }
   TX_RESTORE;

   newS->nxSock=o->nxSock;
   newS->nxFirstPacket=o->nxFirstPacket;
   newS->nxPacket=o->nxPacket;
   newS->nxPktOffs=o->nxPktOffs;
   if(o == disp->curSock)
      disp->curSock=0;
   o->nxPacket=0;
   o->nxSock=0;
   o->nxPktOffs=0;
   o->nxFirstPacket=0;
   newS->nxSock->nx_tcp_socket_reserved_ptr = newS;
}


/*
  This horrible hack is needed since "accept" is performed in the NetX
  callback thus "accept" requires a NX_NO_WAIT option. The no wait
  option makes "accept" return NX_IN_PROGRESS.
*/
static  UINT
nx_tcp_socket_send_wrapper(NX_TCP_SOCKET *sock, NX_PACKET *packet)
{
   int i;
   UINT status=0;
   for(i=0; i < 10; i++)
   {
      status = nx_tcp_socket_send(sock, packet, NX_WAIT_FOREVER);
      if(NX_NOT_CONNECTED == status &&
         sock->nx_tcp_socket_state == NX_TCP_SYN_RECEIVED)
      {
         Thread_sleep(100);
      }
      else
      {
         break;
      }
   }
   return status;
}

void
HttpSocket_send(HttpSocket* o, struct ThreadMutex* m, BaBool* isTerminated,
                const void* data, int len, int* retLen)
{
   if(o->nxSock)
   {
      NX_PACKET* nxPacket;
      NX_PACKET_POOL* pool = nxPacketPool ? nxPacketPool :
         o->nxSock->nx_tcp_socket_ip_ptr->nx_ip_default_packet_pool;
      if(m && ThreadMutex_isOwner(m))
      {
         ThreadMutex_release(m);
         if(!nx_packet_allocate(pool,&nxPacket,NX_TCP_PACKET,NX_WAIT_FOREVER))
         {
            if(!*isTerminated &&
               !nx_packet_data_append(
                  nxPacket,(void*)data,(ULONG)len,pool,NX_WAIT_FOREVER))
            {
               if(!*isTerminated &&
                  !nx_tcp_socket_send_wrapper(o->nxSock, nxPacket))
               {
                  ThreadMutex_set(m);
                  *retLen = len;
                  return;
               }
            }
            nx_packet_release(nxPacket);
         }
         ThreadMutex_set(m);
      }
      else
      {
         if(!nx_packet_allocate(pool,&nxPacket,NX_TCP_PACKET,NX_WAIT_FOREVER))
         {
            if(!nx_packet_data_append(
                  nxPacket,(void*)data,(ULONG)len,pool,NX_WAIT_FOREVER))
            {
               if(!nx_tcp_socket_send_wrapper(o->nxSock, nxPacket))
               {
                  *retLen = len;
                  return;
               }
            }
            nx_packet_release(nxPacket);
         }
      }
   }
   *retLen = -1;
}


/**************************************************************************
 *                              HttpSockaddr
 **************************************************************************
 */

void
HttpSockaddr_gethostbyname(
   HttpSockaddr* o, const char* host, BaBool ip6, int* status)
{
   o->ip=0;
   o->isIp6=FALSE;
   if(host)
   {
      NX_IP** ipPtr = nxIpList; /* read only */
      while(*ipPtr)
      {
         if((*ipPtr)->nx_ip_name && !strcmp((*ipPtr)->nx_ip_name, host))
         {
            o->ip = *ipPtr;
            break;
         }
         ipPtr++;
      }
#ifdef BA_NX_DNS
      if(!o->ip && nxdns)
      {
#ifdef FEATURE_NX_IPV6
         NXD_ADDRESS nxAddr;
         if(!nxd_dns_host_by_name_get(nxdns,(UCHAR*)host,&nxAddr,4000,ip6?6:4))
         {
            if(ip6)
            {
#ifdef B_LITTLE_ENDIAN
               nxAddr.nxd_ip_address.v6[0]=baHtonl(nxAddr.nxd_ip_address.v6[0]);
               nxAddr.nxd_ip_address.v6[1]=baHtonl(nxAddr.nxd_ip_address.v6[1]);
               nxAddr.nxd_ip_address.v6[2]=baHtonl(nxAddr.nxd_ip_address.v6[2]);
               nxAddr.nxd_ip_address.v6[3]=baHtonl(nxAddr.nxd_ip_address.v6[3]);
#endif
               memcpy(o->addr,&nxAddr.nxd_ip_address,16);
               o->isIp6=TRUE;
            }
            else
            {
#ifdef B_LITTLE_ENDIAN
               nxAddr.nxd_ip_address.v4=baHtonl(nxAddr.nxd_ip_address.v4);
#endif
               memcpy(o->addr,&nxAddr.nxd_ip_address,4);
               o->isIp6=FALSE;
            }
            *status = 0;
            return;
         }
#else  /*  FEATURE_NX_IPV6 */
         ULONG addr;
         if(!nx_dns_host_by_name_get(nxdns,(UCHAR*)host,&addr,4000))
         {
#ifdef B_LITTLE_ENDIAN
            addr=baHtonl(addr);
#endif
            memcpy(o->addr,&addr,4);
            o->isIp6=FALSE;
            *status = 0;
            return;
         }
#endif /*  FEATURE_NX_IPV6 */
      }
#endif /* BA_NX_DNS */
   }
   else
      o->ip = nxIpList[0];
   if(o->ip)
   {
      /* Used for binding server sockets: NextX binds to all interfaces. */
      *status = 0;
   }
   else
   {
      /* not implemented */
      *status = -1;
   }
}




/**************************************************************************
 *                              SoDispCon
 **************************************************************************
 */

#ifdef FEATURE_NX_IPV6
int
SoDispCon_isIP6NetX(struct SoDispCon* o)
{
   NX_TCP_SOCKET* nxSock = o->httpSocket.nxSock;
   if(nxSock)
   {
      return nxSock->nx_tcp_socket_connect_ip.nxd_ip_version==NX_IP_VERSION_V6;
   }
   return FALSE;
}
#endif

/* Returns 0 if no data, >0 if data and <0 on error.
   This function is called unprotected i.e. the SoDisp mutex is not set.
   isTerminated is a variable handled by the caller.
   Can be set to TRUE by a another thread.
   If TRUE, the object "o" is no longer valid.
*/
int
SoDispCon_platReadData(SoDispCon* o, ThreadMutex* m, BaBool* isTerminated,
                       void* data, int len)
{
   int retLen = 0;
   U8* dataPtr=(U8*)data;
   HttpSocket* so = &o->httpSocket;
   if(! so->nxSock )
      return E_SOCKET_READ_FAILED;
   if( ! so->nxPacket )
   {
      UINT status;
      ULONG waitOption;
      NX_PACKET* nxPacket;
      int releaseMutex=FALSE;
      if(SoDispCon_isNonBlocking(o))
      {
         waitOption = NX_NO_WAIT;
      }
      else
      {
         if(o->rtmo)
            waitOption=baMsTime2TxTicks((U32)o->rtmo*50);
         else
            waitOption = NX_WAIT_FOREVER;
         if(m && ThreadMutex_isOwner(m))
            releaseMutex=TRUE;
      }
      if(releaseMutex)
      {
         ThreadMutex_release(m);
         status =  nx_tcp_socket_receive(so->nxSock,&nxPacket,waitOption);
         ThreadMutex_set(m);
         if(*isTerminated)
            return E_SOCKET_READ_FAILED;
      }
      else
      {
         status =  nx_tcp_socket_receive(so->nxSock,&nxPacket,waitOption);
      }
      if(status != NX_SUCCESS)
      {
         if(status != NX_NO_PACKET)
         {
            SoDispCon_closeCon(o);
            return E_SOCKET_READ_FAILED;
         }
         SoDispCon_clearSocketHasNonBlockData(o);
         return o->rtmo ? E_TIMEOUT : 0; /* 0 = No data */
      }
      so->nxPacket = so->nxFirstPacket = nxPacket;
      so->nxPktOffs = nxPacket->nx_packet_prepend_ptr;
   }
   for(;;)
   {
      int pLen = so->nxPacket->nx_packet_append_ptr - so->nxPktOffs;
      baAssert(pLen > 0);
      if(pLen > len)
      {
         if(len)
         {
            memcpy(dataPtr, so->nxPktOffs, len);
            so->nxPktOffs += len;
            retLen = retLen + len;
         }
         return retLen;
      }
      memcpy(dataPtr, so->nxPktOffs, pLen);
      retLen = retLen + pLen;
      dataPtr += pLen;
      len -= pLen;
      so->nxPacket = so->nxPacket->nx_packet_next;
      if( ! so->nxPacket )
         break;
      so->nxPktOffs = so->nxPacket->nx_packet_prepend_ptr;
   }
   SoDispCon_clearSocketHasNonBlockData(o);
   TST_OK(nx_packet_release(so->nxFirstPacket));
   so->nxPacket=0;
   so->nxFirstPacket=0;
   return retLen;
}


/* 
 An interesting feature of NetX is that receive data is available
 before the accept callback is called. The receive callback is
 disabled because the web-server can obviously not accept any data
 before an accept.  This function simulates a receive callback
 notification, which would otherwise be lost.
 This function, which is not doing anything on most platforms except
 ThreadX, is used exclusively by HttpServCon.c, just after accept().

 In HttpCfg.h: macro SoDispCon_newConnectionIsReady ->
   function _SoDispCon_newConnectionIsReady
*/
void
_SoDispCon_newConnectionIsReady(struct SoDispCon* con)
{
   SoDispCon_setDispHasRecData(con);
   SoDispCon_dispRecEvent(con);
}



/**************************************************************************
 *                              SoDisp
 **************************************************************************
 */


/*
  All interfaces (NX_IP objects) that the server can use.
  Initialize as 
  static NX_IP* ipList[] = {ip1,ip2,0};
  static char buf[sizeof(NX_TCP_SOCKET)*20];
  static FixedSizeAllocator allocator(buf, sizeof(buf), sizeof(NX_TCP_SOCKET));
  SoDisp_nxInit(ipList, allocator);
 */
void
SoDisp_nxInit(NX_IP** ip, NX_PACKET_POOL* _nxPacketPool,
              AllocatorIntf* _nxTcpSockAlloc, ULONG _nxWindowSize)
{
   NX_IP** ipPtr;
   ipPtr = nxIpList = ip;
   nxTcpSockAlloc = _nxTcpSockAlloc?_nxTcpSockAlloc:AllocatorIntf_getDefault();
   nxPacketPool = _nxPacketPool;
   nxWindowSize = _nxWindowSize;
   while(*ipPtr)
   {
      NX_IP* ip = *ipPtr;
      if( ! ip->nx_ip_fragment_processing )
         nx_ip_fragment_enable(ip);
      if( ! ip->nx_ip_tcp_packet_receive )
         nx_tcp_enable(ip);
      ipPtr++;
   }
}


void
SoDisp_setDNS(void* dns)
{
#ifdef BA_NX_DNS
   nxdns=(NX_DNS*)dns;
#else
   baAssert(0);
#endif
}


void
SoDisp_newCon(SoDisp* o, struct SoDispCon* con)
{
   (void)o;
   (void)con;
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
   HttpSocket* sock = &con->httpSocket;
   TX_INTERRUPT_SAVE_AREA;
   (void)o;
   baAssert(!SoDispCon_recEvActive(con));
   SoDispCon_setRecEvActive(con);
   if(sock->nxSock->nx_tcp_socket_state != NX_TCP_LISTEN_STATE)
   {
      if(sock->nxSock)
      {
         if( ! sock->nxPacket )
         {
            UINT status = nx_tcp_socket_receive(
               sock->nxSock, &sock->nxPacket, NX_NO_WAIT);
            if(status)
            {
               sock->nxPacket=0;
               if(status == NX_NO_PACKET)
                  return; /* Nothing to do */
            }
            sock->nxFirstPacket=sock->nxPacket;
            sock->nxPktOffs=sock->nxPacket->nx_packet_prepend_ptr;
         }
      }
      /* Simulate event by adding to SoDisp queue */
      TX_DISABLE; /* Protecting asynch TX callback */
      if( ! DoubleLink_isLinked(sock) )
         DoubleList_insertLast(&o->soDispQueue, sock);
      TX_RESTORE;
      TST_OK(tx_semaphore_put(&o->queueSem));
   }
}


void
SoDisp_deactivateRec(SoDisp* o, SoDispCon* con)
{
   TX_INTERRUPT_SAVE_AREA;
   (void)o;
   baAssert(SoDispCon_recEvActive(con));
   SoDispCon_setRecEvInactive(con);
   TX_DISABLE; /* Protecting asynch TX callback */
   if(DoubleLink_isLinked(&con->httpSocket))
      DoubleLink_unlink(&con->httpSocket); /* If in SoDisp queueu */
   TX_RESTORE;
}

#ifndef NO_ASYNCH_RESP
#error not implemented. No support for asynchronous response messages in NetX.
void
SoDisp_activateSend(SoDisp* o, SoDispCon* con)
{
   (void)o;
   (void)con;
}

void
SoDisp_deactivateSend(SoDisp* o, SoDispCon* con)
{
   (void)o;
   (void)con;
}
#endif


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
   o->mutex = mutex;
   DoubleList_constructor(&o->soDispQueue);
   tx_semaphore_create(&o->queueSem,"SoDisp",0);
}


void
SoDisp_run(SoDisp* o, S32 msecTimeout)
{
   ULONG waitOption = msecTimeout < 0 ?
      TX_WAIT_FOREVER : baMsTime2TxTicks((U32)msecTimeout);
   o->doExit = FALSE;
   while( ! o->doExit )
   {
      UINT status = tx_semaphore_get(&o->queueSem, waitOption);
      if(status == TX_SUCCESS)
      {
         HttpSocket* sock;
         TX_INTERRUPT_SAVE_AREA;
         SoDisp_mutexSet(o); /* Dispatcher mutex: multithreaded server. */
         TX_DISABLE; /* Protecting asynch TX callback */
         sock = (HttpSocket*)DoubleList_removeFirst(&o->soDispQueue);
         TX_RESTORE;
         if(sock)
         {
            SoDispCon* con = httpSocket2Con(sock);
            o->curSock = sock;
            /* While not closed and receive state is active */
            while(sock == o->curSock && SoDispCon_recEvActive(con))
            {
               if(sock->nxSock)
               {
                  /* Note: can also be listening sock: See REF-LS */
                  if( ! sock->nxPacket )
                  {
                     UINT status = nx_tcp_socket_receive(
                        sock->nxSock, &sock->nxPacket, NX_NO_WAIT);
                     if(status)
                     {
                        sock->nxPacket=0;
                        if(status == NX_NO_PACKET)
                           break; /* Nothing to do */
                     }
                     else
                     {
                        sock->nxFirstPacket=sock->nxPacket;
                        sock->nxPktOffs=sock->nxPacket->nx_packet_prepend_ptr;
                     }
                  }
               }
               /* Else closed : will be handled in SoDispCon_dispRecEvent */
               SoDispCon_setDispHasRecData(con);
               SoDispCon_dispRecEvent(con);
            }
         }
         SoDisp_mutexRelease(o);
      }
   }
}
