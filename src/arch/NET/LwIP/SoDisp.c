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
 *   $Id: SoDisp.c 6747 2026-11-04 20:29:24Z wini $
 *
 *   COPYRIGHT:  Real Time Logic, 2015 - 2026
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
#include <lwip/netif.h>
#include <lwip/init.h>
#if LWIP_VERSION_MAJOR < 2
#include <lwip/api_msg.h>
#else
#ifndef LWIP_PROVIDE_ERRNO
#define LWIP_PROVIDE_ERRNO
#endif
#include <lwip/errno.h>
#include <lwip/priv/api_msg.h>
#include <lwip/priv/tcpip_priv.h>
#endif

#include <lwip/sockets.h>
#include <stdint.h>


#if LWIP_SO_RCVTIMEO != 1
#error LWIP_SO_RCVTIMEO must be set
#endif
#if LWIP_DNS != 1
#error LWIP_DNS must be set
#endif
#if LWIP_NETCONN_FULLDUPLEX != 1
#error BAS lwIP requires LWIP_NETCONN_FULLDUPLEX=1
#endif
#if BA_ENABLE_IPV6 && (LWIP_VERSION_MAJOR < 2 || \
    (LWIP_VERSION_MAJOR == 2 && LWIP_VERSION_MINOR < 1))
#error BAS IPv6 requires lwIP 2.1 or later
#endif
#if BA_ENABLE_IPV6
#define HttpSocket_isIp6(o) NETCONNTYPE_ISIPV6(netconn_type((o)->ncon))
#else
#define HttpSocket_isIp6(o) FALSE
#endif

/* See builds/FreeRTOS/README.txt for details on construction below:
 */
#if BA_LWIP_USE_NETCONN_CALLBACK_DATA
#define getRawCallbackData(ncon) ((uintptr_t)(ncon)->callbackData)
#define setRawCallbackData(ncon, value) (ncon)->callbackData=(void*)(value)
#elif LWIP_SOCKET != 0
#if LWIP_VERSION_MAJOR < 2 || (LWIP_VERSION_MAJOR == 2 && LWIP_VERSION_MINOR < 2)
/* Cast the application's socket int member to/from HttpSocket* */
#define USING_INT_AS_VOID_PTR
#define getRawCallbackData(ncon) ((uintptr_t)(unsigned int)(ncon)->socket)
#define setRawCallbackData(ncon, value) (ncon)->socket=(int)(value)
#else
#define getRawCallbackData(ncon) ((uintptr_t)(ncon)->callback_arg.ptr)
#define setRawCallbackData(ncon, value) (ncon)->callback_arg.ptr=(void*)(value)
#endif     
#else
#error Must set callBackData pointer in api.h or define LWIP_SOCKET=1
#endif

/* Accepted netconns have no HttpSocket until netconn_accept returns. Store
   their pending receive count in the otherwise unused callback slot. The low
   two bits tag pending counts (1), detached I/O counts (3), or pointers (0).
   HttpSocket is at least four-byte aligned. LwIP uses
   socket == -1 for a newly accepted netconn in its socket-enabled builds. */
static HttpSocket*
getCallBackData(struct netconn* ncon)
{
   uintptr_t value=getRawCallbackData(ncon);
   return value && !(value & 3) ? (HttpSocket*)value : 0;
}

static void
setCallBackData(struct netconn* ncon, HttpSocket* sock)
{
   baAssert(!((uintptr_t)sock & 3));
   setRawCallbackData(ncon,(uintptr_t)sock);
}

static int
getPendingRecEvents(struct netconn* ncon)
{
   uintptr_t value=getRawCallbackData(ncon);
   return !value || (unsigned int)value == (unsigned int)-1 ? 0 :
      (int)(value >> 2);
}

static void
setPendingRecEvents(struct netconn* ncon, int count)
{
   baAssert(count >= 0 && count < 0x40000000);
   setRawCallbackData(ncon,((uintptr_t)count << 2) | 1);
}

#define setClosedIOCount(ncon,count) \
   setRawCallbackData(ncon,((uintptr_t)(count) << 2) | 3)

 /* Set in the SoDisp_constructor. We can have only one instance! */
static SoDisp* disp;

/* Enable in lwipopt.h (optional: not required) */
#if SO_REUSE
#else
#if BA_ENABLE_LUA_DEBUGGER
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


static void
HttpSockaddr_fromLwip(HttpSockaddr* o, const ip_addr_t* addr)
{
#if BA_ENABLE_IPV6
   o->zone=0;
   if(IP_IS_V6(addr))
   {
      memcpy(o->addr,ip_2_ip6(addr)->addr,16);
      o->isIp6=TRUE;
      o->zone=ip6_addr_zone(ip_2_ip6(addr));
      return;
   }
#endif
   o->isIp6=FALSE;
#if LWIP_IPV6
   memcpy(o->addr,ip_2_ip4(addr),4);
#else
   memcpy(o->addr,&addr->addr,4);
#endif
}

static void
HttpSockaddr_toLwip(const HttpSockaddr* o, ip_addr_t* addr)
{
#if BA_ENABLE_IPV6
   if(o->isIp6)
   {
      memcpy(ip_2_ip6(addr)->addr,o->addr,16);
      IP_SET_TYPE_VAL(*addr,IPADDR_TYPE_V6);
      ip6_addr_set_zone(ip_2_ip6(addr),o->zone);
      return;
   }
#endif
#if LWIP_IPV6
   memcpy(ip_2_ip4(addr),o->addr,4);
   IP_SET_TYPE_VAL(*addr,IPADDR_TYPE_V4);
#else
   memcpy(&addr->addr,o->addr,4);
#endif
}

#if BA_ENABLE_IPV6
typedef struct
{
   struct tcpip_api_call_data call;
   const char* name;
   U8 zone;
} HttpSockaddrZone;

static err_t
HttpSockaddr_findZone(struct tcpip_api_call_data* call)
{
   HttpSockaddrZone* zone=(HttpSockaddrZone*)call;
   struct netif* netif=netif_find(zone->name);
   zone->zone=netif ? netif_get_index(netif) : 0;
   return ERR_OK;
}

void
_HttpSockaddr_inetAddr(HttpSockaddr* o,const char* host,BaBool ip6,int* status)
{
   ip_addr_t addr;
   char literal[46];
   const char* scope=strchr(host,'%');
   size_t len=scope ? (size_t)(scope-host) : strlen(host);
   *status=-1;
   if(!len || len >= sizeof(literal) ||
      strspn(host,"0123456789abcdefABCDEF:.") != len)
      return;
   memcpy(literal,host,len);
   literal[len]=0;
   if(!ipaddr_aton(literal,&addr) || (IP_IS_V6(&addr) != (ip6 != 0)))
      return;
   HttpSockaddr_fromLwip(o,&addr);
   if(scope)
   {
      unsigned long index;
      char* end;
      if(!ip6 || !ip6_addr_has_scope(ip_2_ip6(&addr),IP6_UNKNOWN))
         return;
      ++scope;
      if(*scope >= '0' && *scope <= '9')
      {
         index=strtoul(scope,&end,10);
         if(*end || !index || index > 255)
            return;
         o->zone=(U8)index;
      }
      else
      {
         HttpSockaddrZone zone;
         if(!scope[0] || !scope[1] || scope[2] < '0' || scope[2] > '9')
            return;
         index=strtoul(scope+2,&end,10);
         if(*end || index > 254)
            return;
         zone.name=scope;
         zone.zone=0;
         if(tcpip_api_call(HttpSockaddr_findZone,&zone.call) != ERR_OK ||
            !zone.zone)
            return;
         o->zone=zone.zone;
      }
   }
   *status=0;
}

void
_HttpSockaddr_addr2String(HttpSockaddr* o,char* buf,int len,int* status)
{
   ip_addr_t addr;
   HttpSockaddr_toLwip(o,&addr);
   *status=ipaddr_ntoa_r(&addr,buf,len) ? 0 : -1;
   if(!*status && o->zone)
   {
      int used=(int)strlen(buf);
      int n=basnprintf(buf+used,len-used,"%%%u",(unsigned)o->zone);
      if(n < 0 || n >= len-used)
         *status=-1;
   }
}
#endif

void
HttpSockaddr_gethostbyname(
   HttpSockaddr* o, const char* host, BaBool ip6, int* status)
{
   ip_addr_t addr;
   memset(o,0,sizeof(*o));
   *status=-1;
#if !BA_ENABLE_IPV6
   if(ip6)
      return;
#endif
   o->isIp6=ip6;
   if(!host)
   {
      *status=0;
      return;
   }
#if BA_ENABLE_IPV6
   _HttpSockaddr_inetAddr(o,host,ip6,status);
   if(!*status || strchr(host,'%'))
      return;
#endif
#if LWIP_IPV6 && LWIP_VERSION_MAJOR >= 2
   *status=netconn_gethostbyname_addrtype(
      host,&addr,ip6 ? NETCONN_DNS_IPV6 : NETCONN_DNS_IPV4);
#else
   *status=netconn_gethostbyname(host,&addr);
#endif
   if(!*status)
      HttpSockaddr_fromLwip(o,&addr);
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

   /* Do not change ncon->callback: LwIP's API_EVENT reads it twice without
      this lock. Detached connections ignore events; -1 means new accept. */
   if((getRawCallbackData(ncon) & 3) == 3 &&
      (unsigned int)getRawCallbackData(ncon) != (unsigned int)-1)
   {
      SYS_ARCH_UNPROTECT(lev);
      return;
   }

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
            /* A receiver can consume the posted pbuf before RCVPLUS runs.
               Keep the temporary negative count so both events balance. */
            o->recEvent--;
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
      /* Data may arrive before accept can attach its HttpSocket. There is
         one count per netconn, so unrelated listeners cannot exhaust it. */
      int count=getPendingRecEvents(ncon);
      if(evt == NETCONN_EVT_RCVPLUS || evt == NETCONN_EVT_ERROR)
      {
         if(count < 0x3FFFFFFF)
            count++;
      }
      else if(evt == NETCONN_EVT_RCVMINUS && count)
         count--;
      setPendingRecEvents(ncon,count);
   }
   SYS_ARCH_UNPROTECT(lev);
}


/* Activate socket after a blocking call if any of the event flags are set.
   ref-BT
*/
static void
HttpSocket_checkEvents(HttpSocket* o)
{
   if(o->recEvent > 0 || o->sendEvent || o->errEvent)
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
   *status = o->ncon && (ERR_WOULDBLOCK == *status ||
                         ERR_INPROGRESS == *status);
}


enum HttpSocketTcpOptionType
{
   HttpSocketTcpOption_NoDelay,
#if LWIP_TCP_KEEPALIVE == 1
   HttpSocketTcpOption_GetKeepAlive,
   HttpSocketTcpOption_SetKeepAlive,
   HttpSocketTcpOption_GetKeepAliveEx,
   HttpSocketTcpOption_SetKeepAliveEx
#endif
};

typedef struct
{
#if LWIP_VERSION_MAJOR >= 2
   struct tcpip_api_call_data call;
#endif
   struct netconn* ncon;
   enum HttpSocketTcpOptionType type;
   int enable;
   int time;
   int interval;
   int status;
} HttpSocketTcpOption;

static void
HttpSocket_applyTcpOption(HttpSocketTcpOption* option)
{
   struct netconn* ncon=option->ncon;
   if(!ncon || NETCONNTYPE_GROUP(netconn_type(ncon)) != NETCONN_TCP ||
      ncon->state == NETCONN_LISTEN || !ncon->pcb.tcp)
   {
      option->status=E_SOCKET_CLOSED;
      return;
   }
   option->status=0;
   switch(option->type)
   {
      case HttpSocketTcpOption_NoDelay:
         if(option->enable)
            tcp_nagle_disable(ncon->pcb.tcp);
         else
            tcp_nagle_enable(ncon->pcb.tcp);
         break;
#if LWIP_TCP_KEEPALIVE == 1
      case HttpSocketTcpOption_GetKeepAlive:
         option->enable=ip_get_option(ncon->pcb.ip,SOF_KEEPALIVE);
         break;
      case HttpSocketTcpOption_SetKeepAlive:
         if(option->enable)
            ip_set_option(ncon->pcb.ip,SOF_KEEPALIVE);
         else
            ip_reset_option(ncon->pcb.ip,SOF_KEEPALIVE);
         break;
      case HttpSocketTcpOption_GetKeepAliveEx:
         option->enable=ip_get_option(ncon->pcb.ip,SOF_KEEPALIVE);
         option->time=(int)(ncon->pcb.tcp->keep_idle/1000);
         option->interval=(int)(ncon->pcb.tcp->keep_intvl/1000);
         break;
      case HttpSocketTcpOption_SetKeepAliveEx:
         if(option->enable)
         {
            ip_set_option(ncon->pcb.ip,SOF_KEEPALIVE);
            ncon->pcb.tcp->keep_idle=option->time*1000;
            ncon->pcb.tcp->keep_intvl=option->interval*1000;
         }
         else
            ip_reset_option(ncon->pcb.ip,SOF_KEEPALIVE);
         break;
#endif
   }
}

#if LWIP_VERSION_MAJOR >= 2
static err_t
HttpSocket_applyTcpOptionCore(struct tcpip_api_call_data* call)
{
   HttpSocket_applyTcpOption((HttpSocketTcpOption*)call);
   return ERR_OK;
}
#endif

static int
HttpSocket_runTcpOption(HttpSocketTcpOption* option)
{
#if LWIP_VERSION_MAJOR >= 2
   if(tcpip_api_call(HttpSocket_applyTcpOptionCore,&option->call) != ERR_OK)
      return E_SOCKET_CLOSED;
#else
   HttpSocket_applyTcpOption(option);
#endif
   return option->status;
}

void
HttpSocket_setTCPNoDelay(HttpSocket* o, int enable, int* status)
{
   HttpSocketTcpOption option;
   option.ncon=o->ncon;
   option.type=HttpSocketTcpOption_NoDelay;
   option.enable=enable;
   *status=HttpSocket_runTcpOption(&option);
}


#if LWIP_VERSION_MAJOR >= 2
typedef struct
{
   struct tcpip_api_call_data call;
   struct netconn* ncon;
   ip_addr_t addr;
   BaBool hasAddress;
   BaBool found;
} HttpSocketLocalAddrCheck;

/* netif_list belongs to the TCP/IP thread, not the BAS dispatcher mutex. */
static err_t
HttpSocket_findLocalAddr(struct tcpip_api_call_data* call)
{
   HttpSocketLocalAddrCheck* check=(HttpSocketLocalAddrCheck*)call;
   struct netif* netif;
   if(!check->hasAddress || ip_addr_isany(&check->addr) ||
      ip_addr_isloopback(&check->addr))
      check->found=TRUE;
   for(netif=netif_list; netif && !check->found; netif=netif->next)
   {
#if BA_ENABLE_IPV6
      if(IP_IS_V6(&check->addr))
      {
         int i;
         for(i=0;i<LWIP_IPV6_NUM_ADDRESSES;i++)
         {
            if(ip6_addr_isvalid(netif_ip6_addr_state(netif,i)) &&
               ip6_addr_cmp_zoneless(netif_ip6_addr(netif,i),ip_2_ip6(&check->addr)) &&
               (!ip6_addr_has_zone(ip_2_ip6(&check->addr)) ||
                ip6_addr_test_zone(ip_2_ip6(&check->addr),netif)))
            {
               check->found=TRUE;
               break;
            }
         }
      }
      else
#endif
      if(ip4_addr_cmp(netif_ip4_addr(netif),ip_2_ip4(&check->addr)))
      {
         check->found=TRUE;
         break;
      }
   }
   if(check->found && check->ncon->pcb.ip)
      ip_set_option(check->ncon->pcb.ip,SO_REUSEADDR);
   return ERR_OK;
}

static BaBool
HttpSocket_prepareBind(struct netconn* ncon, const ip_addr_t* addr)
{
   HttpSocketLocalAddrCheck check;
   check.ncon=ncon;
   check.hasAddress=(BaBool)(addr != 0);
   if(addr)
      check.addr=*addr;
   check.found=FALSE;
   return tcpip_api_call(HttpSocket_findLocalAddr,&check.call) == ERR_OK &&
      check.found;
}
#endif


void
_HttpSocket_newSock(HttpSocket* o, const char* host, enum netconn_type t,
                    BaBool ip6, int* status)
{
   (void)host;
   *status = -1;
#if !BA_ENABLE_IPV6
   if(ip6)
      return;
#else
   if(ip6)
      t=(enum netconn_type)(t | NETCONN_TYPE_IPV6);
#endif
   if(!o->ncon)
   {
      o->recEvent = 0;
      o->sendEvent = o->errEvent = FALSE;
      o->ncon = netconn_new_with_callback(t, HttpSocket_netconnCB);
      if(o->ncon)
      {
         o->isDgram = (U8)(NETCONNTYPE_GROUP(t) == NETCONN_UDP);
#if BA_ENABLE_IPV6
         if(ip6)
            netconn_set_ipv6only(o->ncon,1);
#endif
         setCallBackData(o->ncon, o);
         *status = 0;
      }
   }
}


void
HttpSocket_bind(HttpSocket* o,HttpSockaddr* addr,U16 port,int* status)
{
   ip_addr_t ia;
   const ip_addr_t* local=IP_ADDR_ANY;
   if(!o->ncon || (addr && (addr->isIp6 != HttpSocket_isIp6(o))))
   {
      *status=-1;
      return;
   }
   if(addr)
   {
      HttpSockaddr_toLwip(addr,&ia);
      local=&ia;
   }
#if BA_ENABLE_IPV6
   else if(NETCONNTYPE_ISIPV6(netconn_type(o->ncon)))
      local=IP6_ADDR_ANY;
#endif
#if LWIP_VERSION_MAJOR >= 2
   if(!HttpSocket_prepareBind(o->ncon,addr ? &ia : 0))
   {
      *status=-1;
      return;
   }
#else
   ip_set_option(o->ncon->pcb.ip, SO_REUSEADDR);
#endif
   *status = netconn_bind(o->ncon,local,port) == ERR_OK ? 0 : -1;
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
   if(!o->ncon || !addr || (addr->isIp6 != HttpSocket_isIp6(o)))
   {
      *status=-1;
      return;
   }
   HttpSockaddr_toLwip(addr,&ia);
   *status=netconn_connect(o->ncon, &ia, port);
}


void
HttpSocket_accept(HttpSocket* o,HttpSocket* newSock,int* status)
{
   if(o->ncon && ! netconn_accept(o->ncon, &newSock->ncon))
   {
      SYS_ARCH_DECL_PROTECT(lev);
      SYS_ARCH_PROTECT(lev);
      newSock->recEvent=getPendingRecEvents(newSock->ncon);
      setCallBackData(newSock->ncon, newSock);
      SYS_ARCH_UNPROTECT(lev);
      *status=0;
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
      /* Detached connections retain only their outstanding I/O count. */
      setClosedIOCount(ncon,o->ioCount);
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
#if LWIP_NETCONN_FULLDUPLEX
      if(o->ioCount)
      {
         /* Cancel before the caller frees any send/TLS buffers. The last
            returning I/O call deletes the netconn, even if o is gone. */
         err_t err=netconn_prepare_delete(ncon);
         if(err != ERR_OK)
            baFatalE(FE_SOCKET,err);
      }
      else
#endif
      {
         netconn_close(ncon);
         netconn_delete(ncon);
      }
   }
   o->ioCount=0;
   if(o->nbuf)
   {
      if(o->isDgram)
        netbuf_delete((struct netbuf*)o->nbuf);
      else
         pbuf_free((struct pbuf*)o->nbuf);
      o->nbuf=0;
   }
   o->pbOffs=0;
   o->emptyDgram=FALSE;
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
   if(o->ncon && (ip6 == HttpSocket_isIp6(o)) &&
      ERR_OK == netconn_getaddr(o->ncon,&ia,port,local))
   {
      HttpSockaddr_fromLwip(addr,&ia);
      *status = 0;
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
   newS->nbuf = o->nbuf;
   newS->pbOffs = o->pbOffs;
   newS->ioCount = o->ioCount;
   newS->recEvent = o->recEvent;
   newS->sendEvent = o->sendEvent;
   newS->errEvent = o->errEvent;
   newS->isDgram = o->isDgram;
   newS->emptyDgram = o->emptyDgram;
   setCallBackData(newS->ncon, newS);
   if(DoubleLink_isLinked(o))
      DoubleLink_unlink((DoubleLink*)o);
   SYS_ARCH_UNPROTECT(lev);
   if(o == disp->curSock)
      disp->curSock=0;
   o->nbuf=0;
   o->pbOffs=0;
   o->ioCount=0;
   o->recEvent=0;
   o->sendEvent = o->errEvent = FALSE;
   o->isDgram = o->emptyDgram = FALSE;
}


#if LWIP_NETCONN_FULLDUPLEX
/* Called with the BAS mutex held. No allocation, search, or polling is
   needed: close transfers the count to the detached netconn callback slot. */
static HttpSocket*
HttpSocket_endIO(struct netconn* ncon, BaBool* isTerminated)
{
   HttpSocket* sock=getCallBackData(ncon);
   if(sock)
   {
      baAssert(sock->ioCount);
      --sock->ioCount;
      if(!*isTerminated)
         return sock;
   }
   else
   {
      int count=getPendingRecEvents(ncon)-1;
      baAssert(count >= 0);
      if(count)
      {
         SYS_ARCH_DECL_PROTECT(lev);
         SYS_ARCH_PROTECT(lev);
         setClosedIOCount(ncon,count);
         SYS_ARCH_UNPROTECT(lev);
      }
      else
         netconn_delete(ncon);
   }
   *isTerminated=TRUE;
   return 0;
}
#endif


static int
HttpSocket_dgramMaxLen(HttpSocket* o)
{
#if BA_ENABLE_IPV6
   /* lwIP's 16-bit pbuf length includes the IP and UDP headers. */
   if(NETCONNTYPE_ISIPV6(netconn_type(o->ncon)))
      return 65535-40-8;
#else
   (void)o;
#endif
   return 65507;
}

void HttpSocket_send(HttpSocket* o, struct ThreadMutex* m, BaBool* isTerminated,
                const void* data, int len, int* retLen)
{
   err_t err=ERR_ARG;
   if(o->errEvent || !o->ncon)
   {
      *retLen=E_SOCKET_CLOSED;
      return;
   }
   if(o->isDgram)
   {
      struct netbuf* nbuf;
      char* ndata;
      if(len < 0 || len > HttpSocket_dgramMaxLen(o))
      {
         *retLen=E_SOCKET_WRITE_FAILED;
         return;
      }
      if(0 != (nbuf = netbuf_new()))
      {
         if(0 != (ndata = netbuf_alloc(nbuf, len)))
         {
            memcpy(ndata, data, len);
            /* Keep the caller's mutex: FULLDUPLEX does not pin this netconn
               against deletion between unlocking and entering netconn_send. */
            err=netconn_send(o->ncon,nbuf);
            *retLen = err == ERR_OK ? len : E_SOCKET_WRITE_FAILED;
         }
         else
            *retLen = E_MALLOC;
         netbuf_delete(nbuf);
      }
      else
         *retLen = E_MALLOC;
      return;
   }
   if(!netconn_is_nonblocking(o->ncon))
   {
      if(m && ThreadMutex_isOwner(m))
      {
#if LWIP_NETCONN_FULLDUPLEX
         struct netconn* ncon=o->ncon;
         if(!isTerminated)
         {
            *retLen=E_INCORRECT_USE;
            return;
         }
         ++o->ioCount;
         ThreadMutex_release(m);
         err=netconn_write(ncon, data, len, NETCONN_COPY);
         ThreadMutex_set(m);
         o=HttpSocket_endIO(ncon,isTerminated);
         if(!o)
         {
            *retLen=E_SOCKET_CLOSED;
            return;
         }
#else
         *retLen=E_INCORRECT_USE;
         return;
#endif
      }
      else
      {
         err=netconn_write(o->ncon, data, len, NETCONN_COPY);
      }
      if(isTerminated && *isTerminated)
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

void
HttpSocket_sendto(HttpSocket* o, const void* data, int len,
                  struct HttpSockaddr* sa,U16 port, int* status)
{
   ip_addr_t addr;
   struct netbuf* nbuf;
   char* ndata;
   if(o->errEvent || !o->ncon)
   {
      *status=E_SOCKET_CLOSED;
      return;
   }
   if(!o->isDgram || !sa || (sa->isIp6 != HttpSocket_isIp6(o)) ||
      len < 0 || len > HttpSocket_dgramMaxLen(o))
   {
      *status=E_SOCKET_WRITE_FAILED;
      return;
   }
   HttpSockaddr_toLwip(sa,&addr);
   if(0 != (nbuf = netbuf_new()))
   {
      if(0 != (ndata = netbuf_alloc(nbuf, len)))
      {
         memcpy(ndata, data, len);
         *status = netconn_sendto(o->ncon,nbuf,&addr,port);
      }
      else
         *status=E_MALLOC;
      netbuf_delete(nbuf);
   }
   else
      *status=E_MALLOC;
}


#if LWIP_TCP_KEEPALIVE == 1
void
_HttpSocket_getKeepAlive(HttpSocket* o, int* enable, int* status)
{
   HttpSocketTcpOption option;
   option.ncon=o->ncon;
   option.type=HttpSocketTcpOption_GetKeepAlive;
   *status=HttpSocket_runTcpOption(&option);
   if(!*status)
      *enable=option.enable;
}


void
_HttpSocket_setKeepAlive(HttpSocket* o, int enable, int* status)
{
   HttpSocketTcpOption option;
   option.ncon=o->ncon;
   option.type=HttpSocketTcpOption_SetKeepAlive;
   option.enable=enable;
   *status=HttpSocket_runTcpOption(&option);
}


void
_HttpSocket_getKeepAliveEx(
   HttpSocket* o,int* enable,int* time,int* interval,int* status)
{
   HttpSocketTcpOption option;
   option.ncon=o->ncon;
   option.type=HttpSocketTcpOption_GetKeepAliveEx;
   *status=HttpSocket_runTcpOption(&option);
   if(!*status)
   {
      *enable=option.enable;
      *time=option.time;
      *interval=option.interval;
   }
}


void
_HttpSocket_setKeepAliveEx(
   HttpSocket* o,int enable,int time, int interval,int* status)
{
   if(enable && (time <= 0 || interval <= 0 ||
                 time > 0x7FFFFFFF/1000 || interval > 0x7FFFFFFF/1000))
   {
      *status=E_INVALID_PARAM;
      return;
   }
   {
      HttpSocketTcpOption option;
      option.ncon=o->ncon;
      option.type=HttpSocketTcpOption_SetKeepAliveEx;
      option.enable=enable;
      option.time=time;
      option.interval=interval;
      *status=HttpSocket_runTcpOption(&option);
   }
}
#endif


/* Platform specific socket read.
  returns:
    > zero: returns data len
    zero: no data (would block)
    < zero: an error code
 */
int
SoDispCon_lwipRec(struct SoDispCon* o,ThreadMutexBase* m,BaBool* isTerminated,
                  void* data, int dlen, HttpSockaddr* addr, U16* port)
{
   int offset = 0;
   int isTcp;
   struct pbuf* pb;
   HttpSocket* sock = &o->httpSocket;
   int releaseMutex =
      m && ThreadMutex_isOwner(m) && ! SoDispCon_isNonBlocking(o);
   sock->emptyDgram=FALSE;
   if(sock->errEvent || !sock->ncon)
      return E_SOCKET_CLOSED;
   isTcp=NETCONNTYPE_GROUP(netconn_type(sock->ncon)) == NETCONN_TCP;
   baAssert((netconn_is_nonblocking(sock->ncon) == 0) ==
            (SoDispCon_isNonBlocking(o) == 0));
   for(;;)
   {
      int rlen;
      if( ! sock->nbuf )
      {
         err_t err;
         sock->pbOffs = 0;
         if(releaseMutex)
         {
#if LWIP_NETCONN_FULLDUPLEX
            struct netconn* ncon=sock->ncon;
            void* nbuf=0;
            if(!isTerminated)
               return E_INCORRECT_USE;
            /* rtmo 0 means wait forever, same concept as used by netconn */
            netconn_set_recvtimeout(ncon, o->rtmo*50);
            ++sock->ioCount;
            ThreadMutex_release(m);
            err = isTcp ?
               netconn_recv_tcp_pbuf(ncon, (struct pbuf**)&nbuf) :
               netconn_recv(ncon, (struct netbuf**)&nbuf);
            ThreadMutex_set(m);
            sock=HttpSocket_endIO(ncon,isTerminated);
            if(!sock)
            {
               if(nbuf)
               {
                  if(isTcp)
                     pbuf_free((struct pbuf*)nbuf);
                  else
                     netbuf_delete((struct netbuf*)nbuf);
               }
               return E_SOCKET_READ_FAILED;
            }
            o=httpSocket2Con(sock);
            sock->nbuf=nbuf;
            HttpSocket_checkEvents(sock);
#else
            return E_INCORRECT_USE;
#endif
         }
         else if(sock->recEvent > 0)
         {
            err = isTcp ?
               netconn_recv_tcp_pbuf(sock->ncon, (struct pbuf**)&sock->nbuf) :
               netconn_recv(sock->ncon, (struct netbuf**)&sock->nbuf);
            if(ERR_OK != err)
               sock->recEvent=0; /* Ref-spurious */
         }
         else if(! SoDispCon_isNonBlocking(o) )
         { /* Special case where mutex is not released, but we
            * block. Used by for example the debug monitor. */
            netconn_set_recvtimeout(sock->ncon, o->rtmo*50);
            err = isTcp ?
               netconn_recv_tcp_pbuf(sock->ncon, (struct pbuf**)&sock->nbuf) :
               netconn_recv(sock->ncon, (struct netbuf**)&sock->nbuf);
         }
         else
         {
            SoDispCon_clearSocketHasNonBlockData(o);
            return offset; /* zero means no data */
         }
         if(ERR_OK != err)
         {
            /* printf("Sock rec err %d, %p\n",err,sock->pb); */
            sock->nbuf=0; /* Make sure not set. See close() */
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
      pb = isTcp ? (struct pbuf*)sock->nbuf : ((struct netbuf*)sock->nbuf)->p;
      if(!isTcp)
      {
         struct netbuf* nbuf=(struct netbuf*)sock->nbuf;
         /* Lua probes the actual packet length before allocating its string.
            Keep the packet socket-owned until a consuming read or close. */
         if(!data)
            return (int)pb->tot_len+1;
         rlen=(int)pbuf_copy_partial(pb,data,dlen,0);
         sock->emptyDgram=(U8)(pb->tot_len == 0);
         if(addr)
         {
            *port=nbuf->port;
            HttpSockaddr_fromLwip(addr,&nbuf->addr);
         }
         netbuf_delete(nbuf); /* Discard a truncated tail with its packet. */
         sock->nbuf=0;
         if(sock->recEvent <= 0)
            SoDispCon_clearSocketHasNonBlockData(o);
         return rlen;
      }
      rlen=(int)pbuf_copy_partial(pb,(U8*)data+offset,dlen,sock->pbOffs);
      if(!rlen)
         return E_SOCKET_READ_FAILED;
      sock->pbOffs += rlen;
      offset += rlen;
      dlen -= rlen;
      baAssert(dlen >= 0 && sock->pbOffs <= pb->tot_len);
      if(sock->pbOffs >= pb->tot_len)
      {
         pbuf_free(pb);
         sock->nbuf=0;
         if(sock->recEvent > 0)
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
   baAssert( ! con->httpSocket.nbuf );
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

   /* sys_arch_sem_wait: timeout in milliseconds; 0 = wait forever */
   if(msecTimeout< 0)
      msecTimeout=0;

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
                     (sock->recEvent > 0 || sock->errEvent) &&
                     SoDispCon_recEvActive(con))
                  {
                     SoDispCon_setDispHasRecData(con);
                     SoDispCon_dispRecEvent(con);
                  }
               }
               else if((sock->recEvent > 0 || sock->errEvent) &&
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
