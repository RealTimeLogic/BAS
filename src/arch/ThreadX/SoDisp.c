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
 *   $Id: SoDisp.c 6761M 2026-11-06 23:54:32Z (local) $
 *
 *   COPYRIGHT:  Real Time Logic, 2009 - 2026
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
#include <nx_tcp.h>
#if BA_ENABLE_MDNS
#include <BamDNS.h>
#include <nx_ipv6.h>
#if !BA_ENABLE_UDP
#error BA_ENABLE_MDNS requires BA_ENABLE_UDP
#endif
#if BA_ENABLE_IPV6 && !defined(NX_ENABLE_IPV6_MULTICAST)
#error BA_ENABLE_MDNS with IPv6 requires NX_ENABLE_IPV6_MULTICAST
#endif
#endif

#ifdef BA_NX_DNS
#include <nxd_dns.h>
#endif

/* Initialized with SoDisp_nxInit
*/
static  NX_IP** nxIpList;
static  NX_PACKET_POOL* nxPacketPool;
ULONG nxWindowSize;

/* The BAS owner may disappear while a native call is suspended. Keep its
   allocation alive independently until the last in-flight call returns. */
typedef struct HttpSocketNxRef
{
   NX_TCP_SOCKET* socket;
   unsigned int users;
   volatile BaBool closed;
   struct HttpSocketNxRef* nextClose;
   U32 closeStarted;
#ifdef NX_ENABLE_TCP_KEEPALIVE
   DoubleLink keepLink;
   U32 keepIdle, keepInterval, keepStarted;
#endif
#if BA_HAVE_SOCKET_WRITE_EVENTS
   BaBool connecting; /* Fits reference padding; completion is not send readiness. */
#endif
} HttpSocketNxRef;

#if BA_ENABLE_UDP
/* This extension follows the reference only for UDP. */
typedef struct HttpSocketNxUdp
{
   NX_UDP_SOCKET socket;
   HttpSockaddr peer;
   U16 peerPort;
   BaBool ip6;
   BaBool broadcast;
#if BA_ENABLE_MDNS
   /* Logical channels share one native dual-stack socket per NX_IP. The
      transport retains its allocation until the last channel is closed. */
   struct HttpSocketNxUdp* mdnsTransport;
   struct HttpSocketNxUdp* mdnsNext;
   struct HttpSocketNxUdp* mdnsMembers;
   struct HttpSocketNxUdp* mdnsNextTransport;
   HttpSocket* mdnsOwner;
   NX_PACKET* mdnsHead;
   NX_PACKET* mdnsTail;
   UINT mdnsInterface;
   U8 mdnsQueued;
#endif
} HttpSocketNxUdp;

#define HttpSocket_nxUdp(ref) ((HttpSocketNxUdp*)((ref) + 1))
#if BA_ENABLE_MDNS
static UINT mdnsNxReceive(HttpSocketNxUdp* udp, NX_PACKET** packet);
static void mdnsNxClose(HttpSocketNxUdp* udp);
#endif
#endif

static HttpSocketNxRef*
HttpSocket_nxAlloc(void)
{
   HttpSocketNxRef* ref = (HttpSocketNxRef*)baMalloc(sizeof(*ref));
   if(ref)
   {
      memset(ref, 0, sizeof(*ref));
      ref->socket = (NX_TCP_SOCKET*)baMalloc(sizeof(NX_TCP_SOCKET));
      if(!ref->socket)
      {
         baFree(ref);
         return 0;
      }
      memset(ref->socket, 0, sizeof(NX_TCP_SOCKET));
      ref->users = 1; /* The BAS owner. */
      ref->closed = FALSE;
#if BA_HAVE_SOCKET_WRITE_EVENTS
      ref->connecting = FALSE;
#endif
   }
   return ref;
}

static HttpSocketNxRef*
HttpSocket_nxRetain(HttpSocket* socket)
{
   HttpSocketNxRef* ref = socket->nxRef;
   TX_INTERRUPT_SAVE_AREA;
   TX_DISABLE;
   if(ref)
   {
      ref->users++;
   }
   TX_RESTORE;
   return ref;
}

static void
HttpSocket_nxRelease(HttpSocketNxRef* ref)
{
   unsigned int users;
   TX_INTERRUPT_SAVE_AREA;
   TX_DISABLE;
   users = --ref->users;
   TX_RESTORE;
   if(!users)
   {
      if(ref->socket)
         baFree(ref->socket);
      baFree(ref);
   }
}

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

/* Normal close must outlive the BAS object until queued data and FIN are
   acknowledged. Only this native worker waits; it never calls Lua. */
static void
HttpSocket_closeWorker(ULONG argument)
{
   SoDisp* disp = (SoDisp*)argument;
   for(;;)
   {
      HttpSocketNxRef* ref;
      NX_TCP_SOCKET* socket;
      NX_IP* ip;
      BaBool finished;
      TX_INTERRUPT_SAVE_AREA;
      TST_OK(tx_semaphore_get(&disp->nxCloseSem, TX_WAIT_FOREVER));
      TX_DISABLE;
      ref = disp->nxCloseHead;
      disp->nxCloseHead = ref->nextClose;
      if(!disp->nxCloseHead)
      {
         disp->nxCloseTail = 0;
      }
      TX_RESTORE;
      socket = ref->socket;
      ip = socket->nx_tcp_socket_ip_ptr;
      TST_OK(tx_mutex_get(&ip->nx_ip_protection, TX_WAIT_FOREVER));
      if((U32)((U32)baGetMsClock() - ref->closeStarted) >= 5000)
      {
         _nx_tcp_socket_connection_reset(socket);
      }
      else if(socket->nx_tcp_socket_state == NX_TCP_ESTABLISHED ||
              socket->nx_tcp_socket_state == NX_TCP_CLOSE_WAIT)
      {
         if(!socket->nx_tcp_socket_transmit_sent_head)
         {
            /* NetX disconnect(NO_WAIT) resets and its suspended variant
               flushes on return. Start the same native FIN transition only
               after all data is acknowledged; NetX owns retransmission and
               the remaining state transitions. No BAS thread is suspended. */
            socket->nx_tcp_socket_state =
               socket->nx_tcp_socket_state == NX_TCP_CLOSE_WAIT ?
               NX_TCP_LAST_ACK : NX_TCP_FIN_WAIT_1;
            socket->nx_tcp_socket_timeout = socket->nx_tcp_socket_timeout_rate;
            socket->nx_tcp_socket_timeout_retries = 0;
            socket->nx_tcp_socket_tx_sequence++;
            _nx_tcp_packet_send_fin(socket, socket->nx_tcp_socket_tx_sequence - 1);
         }
      }
      else if(socket->nx_tcp_socket_state == NX_TCP_SYN_SENT ||
              socket->nx_tcp_socket_state == NX_TCP_SYN_RECEIVED)
      {
         nx_tcp_socket_disconnect(socket, NX_NO_WAIT);
      }
      finished = socket->nx_tcp_socket_state == NX_TCP_CLOSED ||
         socket->nx_tcp_socket_state == NX_TCP_LISTEN_STATE ||
         socket->nx_tcp_socket_state == NX_TCP_TIMED_WAIT;
      TST_OK(tx_mutex_put(&ip->nx_ip_protection));
      if(!finished)
      {
         TX_DISABLE;
         ref->nextClose = 0;
         if(disp->nxCloseTail)
         {
            disp->nxCloseTail->nextClose = ref;
         }
         else
         {
            disp->nxCloseHead = ref;
         }
         disp->nxCloseTail = ref;
         TX_RESTORE;
         TST_OK(tx_semaphore_put(&disp->nxCloseSem));
         tx_thread_sleep(1);
         continue;
      }
      if(socket->nx_tcp_socket_client_type)
      {
         nx_tcp_client_socket_unbind(socket);
      }
      else
      {
         nx_tcp_server_socket_unaccept(socket);
      }
      TST_OK(nx_tcp_socket_delete(socket));
      /* Allocators and outstanding BAS readers retain their usual mutex. */
      SoDisp_mutexSet(disp);
      HttpSocket_nxRelease(ref);
      SoDisp_mutexRelease(disp);
   }
}

#ifdef NX_ENABLE_TCP_KEEPALIVE
void
HttpSocket_nxKeepAlive(HttpSocket* o, int enable, int idle, int interval,
                       int* status)
{
   HttpSocketNxRef* ref = o->nxRef;
   SoDisp* disp = HttpSocket_getDispatcher(o);
   NX_TCP_SOCKET* socket = o->nxSock;
   *status = E_INVALID_PARAM;
   if(!ref || !socket || !disp || ref->closed)
   {
      return;
   }
   if(!idle && !interval)
   {
      idle = NX_TCP_KEEPALIVE_INITIAL;
      interval = NX_TCP_KEEPALIVE_RETRY;
   }
   if(enable && (idle <= 0 || interval <= 0 ||
                 idle > 0x7FFFFFFF / 1000 || interval > 0x7FFFFFFF / 1000))
   {
      return;
   }
   TST_OK(tx_mutex_get(&socket->nx_tcp_socket_ip_ptr->nx_ip_protection,
                      TX_WAIT_FOREVER));
   socket->nx_tcp_socket_keepalive_enabled = enable != 0;
   socket->nx_tcp_socket_keepalive_retries = 0;
   /* ACK processing resets this marker to NetX's initial timeout. Keep
      NetX's fixed global timer dormant while we apply per-socket timings. */
   socket->nx_tcp_socket_keepalive_timeout = 0xFFFFFFFFUL;
   TST_OK(tx_mutex_put(&socket->nx_tcp_socket_ip_ptr->nx_ip_protection));
   if(DoubleLink_isLinked(&ref->keepLink))
   {
      DoubleLink_unlink(&ref->keepLink);
   }
   if(enable)
   {
      ref->keepIdle = (U32)idle * 1000;
      ref->keepInterval = (U32)interval * 1000;
      ref->keepStarted = (U32)baGetMsClock();
      DoubleList_insertLast(&disp->nxKeepAlive, &ref->keepLink);
      TST_OK(tx_semaphore_put(&disp->queueSem));
   }
   *status = 0;
}

static void
HttpSocket_keepAlivePoll(SoDisp* disp)
{
   DoubleListEnumerator iter;
   DoubleLink* link;
   U32 now = (U32)baGetMsClock();
   DoubleListEnumerator_constructor(&iter, &disp->nxKeepAlive);
   for(link = DoubleListEnumerator_getElement(&iter); link;
       link = DoubleListEnumerator_nextElement(&iter))
   {
      HttpSocketNxRef* ref = (HttpSocketNxRef*)((U8*)link -
                                    offsetof(HttpSocketNxRef, keepLink));
      NX_TCP_SOCKET* socket = ref->socket;
      NX_IP* ip = socket->nx_tcp_socket_ip_ptr;
      TST_OK(tx_mutex_get(&ip->nx_ip_protection, TX_WAIT_FOREVER));
      if(socket->nx_tcp_socket_state == NX_TCP_ESTABLISHED)
      {
         if(socket->nx_tcp_socket_keepalive_timeout < 0x80000000UL)
         {
            ref->keepStarted = now;
            socket->nx_tcp_socket_keepalive_retries = 0;
            socket->nx_tcp_socket_keepalive_timeout = 0xFFFFFFFFUL;
         }
         if((U32)(now - ref->keepStarted) >=
            (socket->nx_tcp_socket_keepalive_retries ? ref->keepInterval : ref->keepIdle))
         {
            ref->keepStarted = now;
            if(++socket->nx_tcp_socket_keepalive_retries > NX_TCP_KEEPALIVE_RETRIES)
            {
               _nx_tcp_socket_connection_reset(socket);
            }
            else
            {
               _nx_tcp_packet_send_ack(socket, socket->nx_tcp_socket_tx_sequence - 1);
            }
         }
      }
      TST_OK(tx_mutex_put(&ip->nx_ip_protection));
   }
}
#endif

static void
HttpSocket_queueEvent(HttpSocket* o, BaBool close)
{
   SoDisp* disp;
   SoDispCon* con;
   BaBool sendSig;
   TX_INTERRUPT_SAVE_AREA;

   TX_DISABLE; /* Protecting asynch TX callback -- i.e. this function. */
   if(!o)
   {
      TX_RESTORE;
      return;
   }
   con = httpSocket2Con(o);
   disp = SoDispCon_getDispatcher(con);
   if(disp && (close || SoDispCon_recEvActive(con) || SoDispCon_sendEvActive(con)) &&
      !DoubleLink_isLinked(o))
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
   HttpSocket_queueEvent((HttpSocket*)nxSock->nx_tcp_socket_reserved_ptr, TRUE);
}


static void
HttpSocket_receiveCB(NX_TCP_SOCKET *nxSock)
{
   HttpSocket_queueEvent((HttpSocket*)nxSock->nx_tcp_socket_reserved_ptr, FALSE);
}


#if BA_HAVE_SOCKET_WRITE_EVENTS
static void
HttpSocket_writeCB(NX_TCP_SOCKET* socket)
{
   /* Called under the IP mutex: queue only, never enter BAS/Lua here. */
   HttpSocket_queueEvent((HttpSocket*)socket->nx_tcp_socket_reserved_ptr, TRUE);
}

static void
HttpSocket_writeNotify(NX_TCP_SOCKET* socket)
{
   TST_OK(nx_tcp_socket_establish_notify(socket, HttpSocket_writeCB));
   TST_OK(nx_tcp_socket_disconnect_complete_notify(socket, HttpSocket_writeCB));
   TST_OK(nx_tcp_socket_queue_depth_notify_set(socket, HttpSocket_writeCB));
}

/* Called with the IP mutex held. Use a conservative usable window; the
   native send remains authoritative, including congestion recovery. */
static ULONG
HttpSocket_writeWindow(NX_TCP_SOCKET* socket)
{
   ULONG window = socket->nx_tcp_socket_tx_window_advertised;
   if(window > socket->nx_tcp_socket_tx_window_congestion)
   {
      window = socket->nx_tcp_socket_tx_window_congestion;
   }
   return window > socket->nx_tcp_socket_tx_outstanding_bytes ?
      window - socket->nx_tcp_socket_tx_outstanding_bytes : 0;
}

static BaBool
HttpSocket_writeReady(HttpSocket* socket)
{
   NX_TCP_SOCKET* tcp = socket->nxSock;
   NX_IP* ip;
   NX_PACKET_POOL* pool;
   BaBool ready;
   if(!socket->nxRef || socket->nxRef->closed)
   {
      return TRUE; /* Deliver terminal errors as well as writable state. */
   }
#if BA_ENABLE_UDP
   if(socket->nxDgram)
   {
      ip = HttpSocket_nxUdp(socket->nxRef)->socket.nx_udp_socket_ip_ptr;
      pool = nxPacketPool ? nxPacketPool : ip->nx_ip_default_packet_pool;
      return pool->nx_packet_pool_available != 0;
   }
#endif
   ip = tcp->nx_tcp_socket_ip_ptr;
   pool = nxPacketPool ? nxPacketPool : ip->nx_ip_default_packet_pool;
   TST_OK(tx_mutex_get(&ip->nx_ip_protection, TX_WAIT_FOREVER));
   if(tcp->nx_tcp_socket_state == NX_TCP_SYN_SENT ||
      tcp->nx_tcp_socket_state == NX_TCP_SYN_RECEIVED)
   {
      ready = FALSE;
   }
   else if(socket->nxRef->connecting ||
           (tcp->nx_tcp_socket_state != NX_TCP_ESTABLISHED &&
            tcp->nx_tcp_socket_state != NX_TCP_CLOSE_WAIT))
   {
      ready = TRUE;
   }
   else
   {
      ready = pool->nx_packet_pool_available && HttpSocket_writeWindow(tcp) &&
         tcp->nx_tcp_socket_transmit_sent_count < tcp->nx_tcp_socket_transmit_queue_maximum;
   }
   TST_OK(tx_mutex_put(&ip->nx_ip_protection));
   return ready;
}
#endif


static void
HttpSocket_listenCB(NX_TCP_SOCKET *nxSock, UINT port)
{
   HttpSocket* o=(HttpSocket*)nxSock->nx_tcp_socket_reserved_ptr;
   (void)port;
   /* Accept promptly, but leave dispatch and BAS object access to SoDisp. */
   if(o && !o->nxAcceptStatus)
   {
      o->nxAcceptStatus = (int)nx_tcp_server_socket_accept(nxSock, NX_NO_WAIT) + 1;
      HttpSocket_queueEvent(o, FALSE);
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
      o->nxRef = HttpSocket_nxAlloc();
      o->nxSock = o->nxRef ? o->nxRef->socket : 0;
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
#if BA_HAVE_SOCKET_WRITE_EVENTS
         if(!*status)
         {
            HttpSocket_writeNotify(o->nxSock);
         }
#endif
         if(*status)
         {
            HttpSocket_nxRelease(o->nxRef);
            o->nxRef=0;
            o->nxSock=0;
         }
      }
      else
         *status = E_MALLOC;
   }
}


#if BA_ENABLE_UDP
static void
HttpSocket_udpReceiveCB(NX_UDP_SOCKET* socket)
{
   HttpSocket_queueEvent((HttpSocket*)socket->nx_udp_socket_reserved_ptr, FALSE);
}

static UINT
HttpSocket_udpBind(HttpSocketNxUdp* udp)
{
   return udp->socket.nx_udp_socket_bound_next ? NX_SUCCESS :
      nx_udp_socket_bind(&udp->socket, NX_ANY_PORT, NX_NO_WAIT);
}

static void
HttpSockaddr_toNx(const HttpSockaddr* source, NXD_ADDRESS* target)
{
   unsigned i, words = source->isIp6 ? 4 : 1;
   U32 word;
   memset(target, 0, sizeof(*target));
   target->nxd_ip_version = source->isIp6 ? NX_IP_VERSION_V6 : NX_IP_VERSION_V4;
   for(i = 0; i < words; i++)
   {
      memcpy(&word, source->addr + 4 * i, 4);
#ifdef FEATURE_NX_IPV6
      if(source->isIp6)
         target->nxd_ip_address.v6[i] = baHtonl(word);
      else
#endif
         target->nxd_ip_address.v4 = baHtonl(word);
   }
}

static void
HttpSockaddr_fromNx(HttpSockaddr* target, const NXD_ADDRESS* source, NX_IP* ip)
{
   unsigned i, words;
   U32 word;
   memset(target, 0, sizeof(*target));
   target->ip = ip;
   target->isIp6 = source->nxd_ip_version == NX_IP_VERSION_V6;
   words = target->isIp6 ? 4 : 1;
   for(i = 0; i < words; i++)
   {
#ifdef FEATURE_NX_IPV6
      if(target->isIp6)
         word = baHtonl(source->nxd_ip_address.v6[i]);
      else
#endif
         word = baHtonl(source->nxd_ip_address.v4);
      memcpy(target->addr + 4 * i, &word, 4);
   }
}

void
HttpSocket_sockUdp(HttpSocket* o, const char* host, BaBool ip6, int* status)
{
   HttpSockaddr address;
   HttpSocketNxRef* ref;
   HttpSocketNxUdp* udp;
   baAssert(!o->nxRef);
   HttpSockaddr_gethostbyname(&address, host, ip6, status);
   if(*status)
      return;
   ref = (HttpSocketNxRef*)baMalloc(sizeof(*ref) + sizeof(*udp));
   if(!ref)
   {
      *status = E_MALLOC;
      return;
   }
   memset(ref, 0, sizeof(*ref) + sizeof(*udp));
   ref->users = 1;
   udp = HttpSocket_nxUdp(ref);
   udp->ip6 = ip6;
   *status = nx_udp_socket_create(address.ip, &udp->socket, "BA UDP",
                                  NX_IP_NORMAL, NX_FRAGMENT_OKAY,
                                  NX_IP_TIME_TO_LIVE, 16);
   if(*status)
   {
      baFree(ref);
      return;
   }
   o->nxRef = ref;
   o->nxDgram = TRUE;
   udp->socket.nx_udp_socket_reserved_ptr = o;
   TST_OK(nx_udp_socket_receive_notify(&udp->socket, HttpSocket_udpReceiveCB));
}

void
HttpSocket_nxBroadcast(HttpSocket* o, int enable, int* status)
{
   *status = -1;
   if(o->nxRef && o->nxDgram && !HttpSocket_nxUdp(o->nxRef)->ip6)
   {
      HttpSocket_nxUdp(o->nxRef)->broadcast = (BaBool)(enable != 0);
      *status = 0;
   }
}

void
HttpSocket_sendto(HttpSocket* o, const void* data, int len,
                  HttpSockaddr* address, U16 port, int* status)
{
   HttpSocketNxUdp* udp;
   NX_PACKET_POOL* pool;
   NX_PACKET* packet;
   NXD_ADDRESS target;
   UINT result;
   *status = -1;
#if BA_HAVE_SOCKET_WRITE_EVENTS
   o->nxWouldBlock = FALSE;
#endif
   if(!o->nxRef || !o->nxDgram || o->nxRef->closed || !port || len < 0)
      return;
   udp = HttpSocket_nxUdp(o->nxRef);
   if(address->isIp6 != udp->ip6 || len > (udp->ip6 ? 65527 : 65507))
      return;
   HttpSockaddr_toNx(address, &target);
   if(!udp->ip6 && !udp->broadcast)
   {
      ULONG mask = udp->socket.nx_udp_socket_ip_ptr->nx_ip_interface[0].nx_interface_ip_network_mask;
      if(target.nxd_ip_address.v4 == 0xffffffffUL ||
         (mask != 0xffffffffUL && (target.nxd_ip_address.v4 | mask) == 0xffffffffUL))
         return;
   }
   pool = nxPacketPool ? nxPacketPool : udp->socket.nx_udp_socket_ip_ptr->nx_ip_default_packet_pool;
   if(HttpSocket_udpBind(udp))
   {
      return;
   }
   result = nx_packet_allocate(pool, &packet, NX_UDP_PACKET, NX_NO_WAIT);
   if(result)
   {
#if BA_HAVE_SOCKET_WRITE_EVENTS
      o->nxWouldBlock = result == NX_NO_PACKET;
#endif
      return;
   }
   result = len ? nx_packet_data_append(packet, (void*)data, (ULONG)len, pool, NX_NO_WAIT) : NX_SUCCESS;
   if(!result)
   {
      result = nxd_udp_socket_send(&udp->socket, packet, &target, port);
   }
   if(!result)
   {
      *status = len;
   }
   else
   {
      nx_packet_release(packet);
#if BA_HAVE_SOCKET_WRITE_EVENTS
      o->nxWouldBlock = result == NX_NO_PACKET;
#endif
   }
}

static UINT
HttpSocket_udpReceive(HttpSocketNxRef* ref, NX_PACKET** packet, ULONG wait)
{
   HttpSocketNxUdp* udp = HttpSocket_nxUdp(ref);
   ULONG start = tx_time_get();
#if BA_ENABLE_MDNS
   if(udp->mdnsTransport)
   {
      baAssert(wait == NX_NO_WAIT);
      return mdnsNxReceive(udp, packet);
   }
#endif
   UINT status = HttpSocket_udpBind(udp);
   unsigned filtered = 0;
   if(status)
      return status;
   for(;;)
   {
      NXD_ADDRESS source = {0};
      HttpSockaddr address;
      UINT port;
      ULONG remaining = wait;
      if(ref->closed)
         return NX_SOCKET_UNBOUND;
      if(wait != NX_WAIT_FOREVER && wait)
      {
         ULONG elapsed = tx_time_get() - start;
         if(elapsed >= wait)
            return NX_NO_PACKET;
         remaining -= elapsed;
      }
      status = nx_udp_socket_receive(&udp->socket, packet, remaining);
      if(status)
         return status;
      status = nxd_udp_source_extract(*packet, &source, &port);
      HttpSockaddr_fromNx(&address, &source, udp->socket.nx_udp_socket_ip_ptr);
      if(!status && address.isIp6 == udp->ip6 &&
         (!udp->peerPort || (port == udp->peerPort &&
          !memcmp(address.addr, udp->peer.addr, udp->ip6 ? 16 : 4))))
         return NX_SUCCESS;
      nx_packet_release(*packet);
      *packet = 0;
      /* Bound work even if a foreign sender continuously fills the queue. */
      if(++filtered == 32 && wait == NX_NO_WAIT)
         return NX_NO_PACKET;
   }
}

int
SoDispCon_nxPeekDgram(SoDispCon* con, ThreadMutex* mutex,
                     BaBool* terminated, void* data, int len)
{
   HttpSocket* socket = &con->httpSocket;
   (void)data;
   (void)len;
   if(!socket->nxRef || !socket->nxDgram)
      return E_SOCKET_READ_FAILED;
   if(!socket->nxPacket)
   {
      HttpSocketNxRef* ref = HttpSocket_nxRetain(socket);
      NX_PACKET* packet = 0;
      UINT status;
      ULONG wait = SoDispCon_isNonBlocking(con) ? NX_NO_WAIT :
         (con->rtmo ? baMsTime2TxTicks((U32)con->rtmo * 50) : NX_WAIT_FOREVER);
      BaBool release = wait && mutex && ThreadMutex_isOwner(mutex);
      if(release)
         ThreadMutex_release(mutex);
      status = HttpSocket_udpReceive(ref, &packet, wait);
      if(release)
         ThreadMutex_set(mutex);
      if((terminated && *terminated) || ref->closed)
      {
         if(!status)
            nx_packet_release(packet);
         HttpSocket_nxRelease(ref);
         return E_SOCKET_CLOSED;
      }
      HttpSocket_nxRelease(ref);
      if(status)
         return status == NX_NO_PACKET ? (wait ? E_TIMEOUT : 0) : E_SOCKET_READ_FAILED;
      socket->nxFirstPacket = socket->nxPacket = packet;
   }
   return (int)socket->nxPacket->nx_packet_length + 1;
}

int
SoDispCon_nxRecvfrom(SoDispCon* con, void* data, int len,
                    HttpSockaddr* address, U16* port)
{
   HttpSocket* socket = &con->httpSocket;
   NXD_ADDRESS source;
   UINT sourcePort;
   ULONG count = 0;
   int status;
   if(!socket->nxRef || !socket->nxDgram || !socket->nxPacket)
      return E_SOCKET_READ_FAILED;
   if(len < 0 || (ULONG)len < socket->nxPacket->nx_packet_length)
      return E_TOO_MUCH_DATA;
   status = nxd_udp_source_extract(socket->nxPacket, &source, &sourcePort);
   if(!status)
      status = nx_packet_data_retrieve(socket->nxPacket, data, &count);
   if(!status && address)
      HttpSockaddr_fromNx(address, &source, HttpSocket_nxUdp(socket->nxRef)->socket.nx_udp_socket_ip_ptr);
   if(!status && port)
      *port = (U16)sourcePort;
   nx_packet_release(socket->nxFirstPacket);
   socket->nxPacket = socket->nxFirstPacket = 0;
   SoDispCon_clearSocketHasNonBlockData(con);
   return status ? E_SOCKET_READ_FAILED : (int)count;
}
#endif

static UINT
HttpSocket_receive(HttpSocketNxRef* ref, NX_PACKET** packet, ULONG wait)
{
#if BA_ENABLE_UDP
   if(!ref->socket)
      return HttpSocket_udpReceive(ref, packet, wait);
#endif
   return nx_tcp_socket_receive(ref->socket, packet, wait);
}


void
HttpSocket_bind(HttpSocket* o, HttpSockaddr* addr, UINT port, int* status)
{
   (void)addr;
#if BA_ENABLE_UDP
   if(o->nxDgram)
   {
      *status = nx_udp_socket_bind(&HttpSocket_nxUdp(o->nxRef)->socket, port, NX_NO_WAIT);
      return;
   }
#endif
   /* Client binding cannot reserve a server port that still has accepted
      connections. Defer to the correct native operation at listen/connect. */
   o->nxBindPort = port;
   *status = 0;
}


void
HttpSocket_listen(HttpSocket* o, HttpSockaddr* addr,int queueSize, int* status)
{
   UINT port = o->nxBindPort;
   if(port == NX_ANY_PORT)
   {
      *status = nx_tcp_client_socket_bind(o->nxSock, NX_ANY_PORT, NX_NO_WAIT);
      if(*status)
         return;
      port = o->nxSock->nx_tcp_socket_port;
      nx_tcp_client_socket_unbind(o->nxSock);
   }
   (void)addr;
   o->nxListener = TRUE;
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
#if BA_ENABLE_UDP
   if(o->nxDgram)
   {
      HttpSocketNxUdp* udp = HttpSocket_nxUdp(o->nxRef);
      *status = HttpSocket_udpBind(udp);
      if(!*status)
      {
         udp->peer = *addr;
         udp->peerPort = (U16)port;
      }
      return;
   }
#endif
   NXD_ADDRESS target;
   U32 word;
   memset(&target, 0, sizeof(target));
   target.nxd_ip_version = addr->isIp6 ? NX_IP_VERSION_V6 : NX_IP_VERSION_V4;
#ifdef FEATURE_NX_IPV6
   if(addr->isIp6)
   {
      unsigned int i;
      for(i = 0; i < 4; i++)
      {
         memcpy(&word, addr->addr + i * 4, 4);
         target.nxd_ip_address.v6[i] = baHtonl(word);
      }
   }
   else
#endif
   {
      memcpy(&word, addr->addr, 4);
      target.nxd_ip_address.v4 = baHtonl(word);
   }
   if(o->nxSock)
   {
      ULONG wait = 5 * NX_IP_PERIODIC_RATE;
#if BA_HAVE_SOCKET_WRITE_EVENTS
      o->nxWouldBlock = FALSE;
      if(o->nxNonblocking)
      {
         wait = NX_NO_WAIT;
      }
#endif
      TST_OK(nx_tcp_socket_receive_notify(o->nxSock, HttpSocket_receiveCB));
      /* If not bound */
      if( ! o->nxSock->nx_tcp_socket_bound_next )
      {
         *status = nx_tcp_client_socket_bind(o->nxSock,o->nxBindPort,NX_NO_WAIT);
         if(*status)
            return;
      }
      *status = nxd_tcp_client_socket_connect(o->nxSock, &target, port,
                                             wait);
#if BA_HAVE_SOCKET_WRITE_EVENTS
      o->nxWouldBlock = o->nxNonblocking && *status == NX_IN_PROGRESS;
      o->nxRef->connecting = o->nxWouldBlock;
#endif
   }
   else
      *status = -1;
}


void
HttpSocket_accept(HttpSocket* o, HttpSocket* conSock, int* status)
{
   int nxStatus;
   TX_INTERRUPT_SAVE_AREA;
   SoDisp* disp = HttpSocket_getDispatcher(o);
   baAssert( ! conSock->nxSock );
   baAssert( o->nxSock );
   baAssert( o->nxSock->nx_tcp_socket_reserved_ptr == (void*)o );
   baAssert(disp->curSock == o);
   baAssert(o->nxFirstPacket==0);
   baAssert(conSock->nxPacket==0);
   baAssert( ! DoubleLink_isLinked(conSock) );

   /* Only report success after transferring a valid socket. Keep curSock
      on failure so the caller can retry after reclaiming an idle connection. */
   *status=-1;

   /* A stale queued notification must not accept the connection twice. */
   if(o->nxAcceptStatus == 0)
   {
      return; /* Do nothing */
   }
   nxStatus = o->nxAcceptStatus - 1;

   /* Both successful accept and a pending accept are treated as
      "successful". The state should normally be NX_IN_PROGRESS since
      we use a zero timeout for accept in function HttpSocket_listenCB.
   */
   if(nxStatus == NX_SUCCESS || nxStatus == NX_IN_PROGRESS)
   {
      HttpSocketNxRef* newRef = HttpSocket_nxAlloc();
      NX_TCP_SOCKET* newNxListenSock = newRef ? newRef->socket : 0;
      if(newNxListenSock)
      {
         memset(newNxListenSock, 0, sizeof(*newNxListenSock));
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
#if BA_HAVE_SOCKET_WRITE_EVENTS
            HttpSocket_writeNotify(newNxListenSock);
#endif
            TX_DISABLE;
            newNxListenSock->nx_tcp_socket_reserved_ptr = o;
            conSock->nxSock=o->nxSock;
            conSock->nxRef=o->nxRef;
            conSock->nxSock->nx_tcp_socket_reserved_ptr = conSock;
            o->nxSock = newNxListenSock;
            o->nxRef = newRef;
            TX_RESTORE;
            /* Function nx_tcp_server_socket_relisten may call
               HttpSocket_listenCB if there are pending SYN packets.
               It's OK to call HttpSocket_listenCB at this point since
               we have set the nx_tcp_socket_reserved_ptr.
            */
            o->nxAcceptStatus=0; /* Enable HttpSocket_listenCB */
            nxStatus = nx_tcp_server_socket_relisten(
               conSock->nxSock->nx_tcp_socket_ip_ptr,
               conSock->nxSock->nx_tcp_socket_port,
               newNxListenSock);
            if(nxStatus == NX_SUCCESS || nxStatus == NX_CONNECTION_PENDING)
            {
               nx_tcp_socket_receive_notify(
                  conSock->nxSock,HttpSocket_receiveCB);
               *status=0;
               disp->curSock=0;
               return;
            }
            nx_tcp_socket_delete(newNxListenSock);
            o->nxSock=conSock->nxSock;
            o->nxRef=conSock->nxRef;
            o->nxSock->nx_tcp_socket_reserved_ptr = o;
            conSock->nxSock=0;
            conSock->nxRef=0;
         }
         HttpSocket_nxRelease(newRef);
      }
   }

   o->nxAcceptStatus=0; /* Enable HttpSocket_listenCB. */
   /* This ensures that the socket that fails to accept is being
    * cleaned up.
    */
   nx_tcp_socket_disconnect(o->nxSock, NX_NO_WAIT);
   nx_tcp_server_socket_unaccept(o->nxSock);
   nxStatus=nx_tcp_server_socket_relisten(o->nxSock->nx_tcp_socket_ip_ptr,
                                         o->nxSock->nx_tcp_socket_port,
                                         o->nxSock);
   if(nxStatus != NX_SUCCESS && nxStatus != NX_CONNECTION_PENDING)
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

#if BA_HAVE_SOCKET_WRITE_EVENTS
   if(DoubleLink_isLinked(&o->nxWriteLink))
   {
      DoubleLink_unlink(&o->nxWriteLink);
   }
#endif
   if( !o->nxRef )
      return; /* Already closed */

#if BA_ENABLE_UDP
   if(o->nxDgram)
   {
      HttpSocketNxUdp* udp = HttpSocket_nxUdp(o->nxRef);
#if BA_ENABLE_MDNS
      if(udp->mdnsTransport)
      {
         o->nxRef->closed = TRUE;
         mdnsNxClose(udp);
         TX_DISABLE;
         if(DoubleLink_isLinked(o))
            DoubleLink_unlink(o);
         TX_RESTORE;
         if(o->nxFirstPacket)
            nx_packet_release(o->nxFirstPacket);
         HttpSocket_nxRelease(o->nxRef);
         o->nxRef = 0;
         o->nxFirstPacket = o->nxPacket = 0;
         o->nxPktOffs = 0;
         o->nxDgram = FALSE;
         return;
      }
#endif
      ip = udp->socket.nx_udp_socket_ip_ptr;
      o->nxRef->closed = TRUE;
      TST_OK(tx_mutex_get(&ip->nx_ip_protection, TX_WAIT_FOREVER));
      udp->socket.nx_udp_socket_reserved_ptr = 0;
      nx_udp_socket_receive_notify(&udp->socket, 0);
      TST_OK(tx_mutex_put(&ip->nx_ip_protection));
      TX_DISABLE;
      if(DoubleLink_isLinked(o))
         DoubleLink_unlink(o);
      TX_RESTORE;
      if(o->nxFirstPacket)
         nx_packet_release(o->nxFirstPacket);
      if(udp->socket.nx_udp_socket_id)
      {
         nx_udp_socket_unbind(&udp->socket); /* Wakes a blocked reader. */
         TST_OK(nx_udp_socket_delete(&udp->socket));
      }
      HttpSocket_nxRelease(o->nxRef);
      o->nxRef = 0;
      o->nxFirstPacket = o->nxPacket = 0;
      o->nxPktOffs = 0;
      o->nxDgram = FALSE;
      return;
   }
#endif

   nxs = o->nxSock;
   /* A concurrent close cancels a suspended read/write immediately. */
   if(o->nxRef->users > 1)
   {
      hardClose = TRUE;
   }
   o->nxRef->closed = TRUE;
#ifdef NX_ENABLE_TCP_KEEPALIVE
   if(DoubleLink_isLinked(&o->nxRef->keepLink))
   {
      DoubleLink_unlink(&o->nxRef->keepLink);
   }
#endif

   ip = nxs->nx_tcp_socket_ip_ptr;

   /* Notifications run with the IP mutex held. Detach the BAS owner before
      disconnecting, since disconnect itself may produce a notification. */
   TST_OK(tx_mutex_get(&ip->nx_ip_protection, TX_WAIT_FOREVER));
   nxs->nx_tcp_socket_reserved_ptr = 0;
   nx_tcp_socket_receive_notify(nxs, 0);
   TST_OK(tx_mutex_put(&ip->nx_ip_protection));
   TX_DISABLE;
   if(DoubleLink_isLinked(o))
      DoubleLink_unlink(o);
   TX_RESTORE;

   /* Any saved packets from SoDispCon_platReadData */
   if(o->nxFirstPacket)
   {
      TST_OK(nx_packet_release(o->nxFirstPacket));
      o->nxPacket=0;
      o->nxFirstPacket=0;
      o->nxPktOffs=0;
   }

   if(!hardClose && !o->nxListener && disp)
   {
      HttpSocketNxRef* ref = o->nxRef;
      ref->closeStarted = (U32)baGetMsClock();
      TX_DISABLE;
      ref->nextClose = 0;
      if(disp->nxCloseTail)
      {
         disp->nxCloseTail->nextClose = ref;
      }
      else
      {
         disp->nxCloseHead = ref;
      }
      disp->nxCloseTail = ref;
      TX_RESTORE;
      TST_OK(tx_semaphore_put(&disp->nxCloseSem));
   }
   else
   {
      nx_tcp_socket_disconnect(nxs, NX_NO_WAIT);
      if(nxs->nx_tcp_socket_client_type)
      {
         nx_tcp_client_socket_unbind(nxs);
      }
      else
      {
         UINT port = nxs->nx_tcp_socket_port;
         nx_tcp_server_socket_unaccept(nxs);
         if(o->nxListener)
         {
            nx_tcp_server_socket_unlisten(ip, port);
         }
      }
      TST_OK(nx_tcp_socket_delete(nxs));
      HttpSocket_nxRelease(o->nxRef);
   }
   o->nxRef = 0;
   o->nxSock = 0; /* invalidate */
   o->nxAcceptStatus = 0;
   o->nxBindPort = 0;
   o->nxListener = FALSE;
}


U32
HttpSocket_getId(HttpSocket* o)
{
#if BA_ENABLE_UDP
   if(o->nxDgram && o->nxRef)
      return (U32)&HttpSocket_nxUdp(o->nxRef)->socket;
#endif
   return (U32)o->nxSock;
}


void HttpSocket_getPeerName(
   HttpSocket* o, HttpSockaddr* sockaddr, U16* port, BaBool ip6, int* status)
{
#if BA_ENABLE_UDP
   if(o->nxDgram && o->nxRef)
   {
      HttpSocketNxUdp* udp = HttpSocket_nxUdp(o->nxRef);
      *status = udp->peerPort ? 0 : -1;
      if(!*status)
      {
         *sockaddr = udp->peer;
         if(port)
            *port = udp->peerPort;
      }
      return;
   }
#endif
   NXD_ADDRESS address = {0};
   ULONG peerPort = 0;
   (void)ip6;
#if BA_HAVE_SOCKET_WRITE_EVENTS
   if(o->nxSock)
   {
      NX_TCP_SOCKET* tcp = o->nxSock;
      NX_IP* ip = tcp->nx_tcp_socket_ip_ptr;
      TST_OK(tx_mutex_get(&ip->nx_ip_protection, TX_WAIT_FOREVER));
      /* Peer info in the pinned API accepts SYN states but rejects CLOSE_WAIT.
         A peer may send data and FIN before async connect is dispatched. */
      if(tcp->nx_tcp_socket_state == NX_TCP_ESTABLISHED ||
         tcp->nx_tcp_socket_state == NX_TCP_CLOSE_WAIT)
      {
         address = tcp->nx_tcp_socket_connect_ip;
         peerPort = tcp->nx_tcp_socket_connect_port;
         *status = 0;
      }
      else
      {
         *status = -1;
      }
      TST_OK(tx_mutex_put(&ip->nx_ip_protection));
   }
   else
   {
      *status = -1;
   }
#else
   *status = o->nxSock ? (int)nxd_tcp_socket_peer_info_get(o->nxSock, &address,
                                                          &peerPort) : -1;
#endif
   if(!*status)
   {
      U32 word;
      sockaddr->ip = o->nxSock->nx_tcp_socket_ip_ptr;
      sockaddr->isIp6 = address.nxd_ip_version == NX_IP_VERSION_V6;
      if(port)
         *port = (U16)peerPort;
#ifdef FEATURE_NX_IPV6
      if(sockaddr->isIp6)
      {
         unsigned int i;
         for(i = 0; i < 4; i++)
         {
            word = baHtonl(address.nxd_ip_address.v6[i]);
            memcpy(sockaddr->addr + i * 4, &word, 4);
         }
      }
      else
#endif
      {
         word = baHtonl(address.nxd_ip_address.v4);
         memcpy(sockaddr->addr, &word, 4);
      }
   }
}


void
HttpSocket_getSockName(
HttpSocket* o, HttpSockaddr* sockaddr, U16* port, BaBool ip6, int* status) 
{
   U32 ipAddr;
   (void)ip6;
#if BA_ENABLE_UDP
   if(o->nxDgram && o->nxRef)
   {
      HttpSocketNxUdp* udp = HttpSocket_nxUdp(o->nxRef);
      NXD_ADDRESS address = {0};
      *status = HttpSocket_udpBind(udp);
      if(*status)
         return;
      /* UDP is bound to the selected NX_IP, not an individual local address. */
      address.nxd_ip_version = udp->ip6 ? NX_IP_VERSION_V6 : NX_IP_VERSION_V4;
      HttpSockaddr_fromNx(sockaddr, &address, udp->socket.nx_udp_socket_ip_ptr);
      if(port)
         *port = (U16)udp->socket.nx_udp_socket_port;
      return;
   }
#endif
   if(o->nxSock)
   {
      *status=0;
      sockaddr->ip = o->nxSock->nx_tcp_socket_ip_ptr;
      if(port) *port=(U16)o->nxSock->nx_tcp_socket_port;
#ifdef FEATURE_NX_IPV6
      if(ip6)
      {
         sockaddr->isIp6=TRUE;
         unsigned int i;
         if(!o->nxSock->nx_tcp_socket_ipv6_addr)
         {
            *status = -1;
            return;
         }
         for(i = 0; i < 4; i++)
         {
            ipAddr = baHtonl(o->nxSock->nx_tcp_socket_ipv6_addr->nxd_ipv6_address[i]);
            memcpy(sockaddr->addr + i * 4, &ipAddr, 4);
         }
      }
      else
      {
         sockaddr->isIp6=FALSE;
         ipAddr = o->nxSock->nx_tcp_socket_connect_interface ?
            o->nxSock->nx_tcp_socket_connect_interface->nx_interface_ip_address :
            sockaddr->ip->nx_ip_interface[0].nx_interface_ip_address;
         NX_CHANGE_ULONG_ENDIAN(ipAddr); /* NetX uses host byte order. */
         *((U32*)sockaddr->addr) = ipAddr;
      }
#else
      sockaddr->isIp6=FALSE;
      ipAddr = o->nxSock->nx_tcp_socket_connect_interface ?
         o->nxSock->nx_tcp_socket_connect_interface->nx_interface_ip_address :
         sockaddr->ip->nx_ip_interface[0].nx_interface_ip_address;
      NX_CHANGE_ULONG_ENDIAN(ipAddr); /* NetX uses host byte order. */
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
   baAssert( ! newS->nxRef );
   baAssert( o->nxRef );

   TX_DISABLE; /* Protecting asynch TX callback */
   if(DoubleLink_isLinked(o))
   {
      DoubleLink_unlink(o); /* If in SoDisp queueu */
      DoubleList_insertLast(&disp->soDispQueue, newS);
   }
   newS->nxSock=o->nxSock;
   newS->nxRef=o->nxRef;
   newS->nxFirstPacket=o->nxFirstPacket;
   newS->nxPacket=o->nxPacket;
   newS->nxPktOffs=o->nxPktOffs;
   newS->nxAcceptStatus=o->nxAcceptStatus;
   newS->nxBindPort=o->nxBindPort;
   newS->nxListener=o->nxListener;
#if BA_HAVE_SOCKET_WRITE_EVENTS
   newS->nxNonblocking=o->nxNonblocking;
   newS->nxWouldBlock=o->nxWouldBlock;
   o->nxNonblocking=o->nxWouldBlock=FALSE;
   if(DoubleLink_isLinked(&o->nxWriteLink))
   {
      DoubleLink_insertBefore(&o->nxWriteLink, &newS->nxWriteLink);
      DoubleLink_unlink(&o->nxWriteLink);
   }
#endif
#if BA_ENABLE_UDP
   newS->nxDgram=o->nxDgram;
   o->nxDgram=FALSE;
#endif
   if(disp && o == disp->curSock)
      disp->curSock=0;
   o->nxPacket=0;
   o->nxSock=0;
   o->nxRef=0;
   o->nxPktOffs=0;
   o->nxFirstPacket=0;
   o->nxAcceptStatus=0;
   o->nxBindPort=0;
   o->nxListener=FALSE;
#if BA_ENABLE_UDP
   if(newS->nxDgram)
      HttpSocket_nxUdp(newS->nxRef)->socket.nx_udp_socket_reserved_ptr = newS;
   else
#endif
      newS->nxSock->nx_tcp_socket_reserved_ptr = newS;
   TX_RESTORE;
}


/*
  This horrible hack is needed since "accept" is performed in the NetX
  callback thus "accept" requires a NX_NO_WAIT option. The no wait
  option makes "accept" return NX_IN_PROGRESS.
*/
static  UINT
nx_tcp_socket_send_wrapper(HttpSocketNxRef* ref, NX_PACKET *packet)
{
   NX_TCP_SOCKET* sock = ref->socket;
   int i;
   UINT status=0;
   for(i=0; i < 10; i++)
   {
      if(ref->closed)
         return NX_NOT_CONNECTED;
      status = nx_tcp_socket_send(sock, packet, 5 * NX_IP_PERIODIC_RATE);
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

#if BA_HAVE_SOCKET_WRITE_EVENTS
static int
HttpSocket_nonblockingSend(HttpSocket* owner, const void* data, int len)
{
   NX_TCP_SOCKET* socket = owner->nxSock;
   NX_IP* ip;
   NX_PACKET_POOL* pool;
   NX_PACKET* packet = 0;
   UINT status;
   ULONG count, accepted = 0;
   int result = -1;
   owner->nxWouldBlock = FALSE;
   if(!socket || !owner->nxRef || owner->nxRef->closed || len < 0)
   {
      return -1;
   }
   if(!len)
   {
      return 0;
   }
   ip = socket->nx_tcp_socket_ip_ptr;
   pool = nxPacketPool ? nxPacketPool : ip->nx_ip_default_packet_pool;
   /* Keep the BAS mutex and native allocation stable. No call below suspends
      on packet/window/queue availability. The IP mutex serializes ACK state. */
   TST_OK(tx_mutex_get(&ip->nx_ip_protection, TX_WAIT_FOREVER));
   if(socket->nx_tcp_socket_state == NX_TCP_SYN_SENT ||
      socket->nx_tcp_socket_state == NX_TCP_SYN_RECEIVED)
   {
      owner->nxWouldBlock = TRUE;
      result = 0;
      goto L_done;
   }
   if(socket->nx_tcp_socket_state != NX_TCP_ESTABLISHED &&
      socket->nx_tcp_socket_state != NX_TCP_CLOSE_WAIT)
   {
      goto L_done; /* Pool exhaustion must not hide a terminal error. */
   }
   status = nx_packet_allocate(pool, &packet, NX_TCP_PACKET, NX_NO_WAIT);
   if(!status)
   {
      count = (ULONG)len;
      if(count > socket->nx_tcp_socket_connect_mss && socket->nx_tcp_socket_connect_mss)
      {
         count = socket->nx_tcp_socket_connect_mss;
      }
      if(count > (ULONG)(packet->nx_packet_data_end - packet->nx_packet_prepend_ptr))
      {
         count = (ULONG)(packet->nx_packet_data_end - packet->nx_packet_prepend_ptr);
      }
      status = nx_packet_data_append(packet, (void*)data, count, pool, NX_NO_WAIT);
      if(!status)
      {
         status = nx_tcp_socket_send(socket, packet, NX_NO_WAIT);
         if(!status)
         {
            accepted = count;
            packet = 0; /* Native stack owns it; never inspect after success. */
         }
         else if(packet->nx_packet_length <= count)
         {
            /* NetX can accept a prefix and return a trimmed unsent suffix. */
            accepted = count - packet->nx_packet_length;
         }
      }
   }
   if(packet)
   {
      TST_OK(nx_packet_release(packet));
   }
   owner->nxWouldBlock = status == NX_NO_PACKET || status == NX_WINDOW_OVERFLOW ||
      status == NX_TX_QUEUE_DEPTH || (status == NX_NOT_CONNECTED &&
      (socket->nx_tcp_socket_state == NX_TCP_SYN_RECEIVED ||
       socket->nx_tcp_socket_state == NX_TCP_SYN_SENT));
   if(accepted)
   {
      result = (int)accepted;
   }
   else if(owner->nxWouldBlock)
   {
      result = 0;
   }
L_done:
   TST_OK(tx_mutex_put(&ip->nx_ip_protection));
   return result;
}
#endif

void
HttpSocket_send(HttpSocket* o, struct ThreadMutex* m, BaBool* isTerminated,
                const void* data, int len, int* retLen)
{
#if BA_ENABLE_UDP
   if(o->nxDgram && o->nxRef)
   {
      HttpSocketNxUdp* udp = HttpSocket_nxUdp(o->nxRef);
      /* Native UDP has no wait parameter. Copy/send one whole datagram while
         holding the caller's mutex; do not enter the TCP partial-write path. */
      HttpSocket_sendto(o, data, len, &udp->peer, udp->peerPort, retLen);
#if BA_HAVE_SOCKET_WRITE_EVENTS
      if(*retLen < 0 && o->nxNonblocking && o->nxWouldBlock)
      {
         *retLen = 0;
      }
#endif
      return;
   }
#endif
#if BA_HAVE_SOCKET_WRITE_EVENTS
   if(o->nxNonblocking)
   {
      *retLen = HttpSocket_nonblockingSend(o, data, len);
      return;
   }
#endif
   HttpSocketNxRef* ref = HttpSocket_nxRetain(o);
   *retLen = -1;
   if(ref)
   {
      NX_PACKET* nxPacket = 0;
      BaBool releaseMutex = m && ThreadMutex_isOwner(m);
      NX_PACKET_POOL* pool = nxPacketPool ? nxPacketPool :
         ref->socket->nx_tcp_socket_ip_ptr->nx_ip_default_packet_pool;
      /* Copy while the caller still owns its payload. A pool shortage must
         fail without suspending: close can release an asynchronous buffer.
         Once copied, the native send owns an independent packet chain. */
      if(!ref->closed &&
         !nx_packet_allocate(pool, &nxPacket, NX_TCP_PACKET, NX_NO_WAIT))
      {
         if(!ref->closed &&
            !nx_packet_data_append(nxPacket, (void*)data, (ULONG)len, pool,
                                   NX_NO_WAIT))
         {
            if(releaseMutex)
               ThreadMutex_release(m);
            if(!nx_tcp_socket_send_wrapper(ref, nxPacket))
               *retLen = len;
            else
               nx_packet_release(nxPacket);
            if(releaseMutex)
               ThreadMutex_set(m);
         }
         else
            nx_packet_release(nxPacket);
      }
      if(ref->closed || (isTerminated && *isTerminated))
         *retLen = -1;
      HttpSocket_nxRelease(ref);
   }
}


/**************************************************************************
 *                              HttpSockaddr
 **************************************************************************
 */

void
HttpSockaddr_gethostbyname(
   HttpSockaddr* o, const char* host, BaBool ip6, int* status)
{
   (void)ip6;
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
         if(!nxd_dns_host_by_name_get(nxdns,(UCHAR*)host,&nxAddr,
                                    5 * NX_IP_PERIODIC_RATE,ip6?6:4))
         {
            if(ip6)
            {
#if BA_BYTE_ORDER == BA_BYTE_ORDER_LITTLE_ENDIAN
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
#if BA_BYTE_ORDER == BA_BYTE_ORDER_LITTLE_ENDIAN
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
         if(!nx_dns_host_by_name_get(nxdns,(UCHAR*)host,&addr,
                                   5 * NX_IP_PERIODIC_RATE))
         {
#if BA_BYTE_ORDER == BA_BYTE_ORDER_LITTLE_ENDIAN
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
#if BA_ENABLE_UDP
   if(o->httpSocket.nxDgram && o->httpSocket.nxRef)
      return HttpSocket_nxUdp(o->httpSocket.nxRef)->ip6;
#endif
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
         HttpSocketNxRef* ref = HttpSocket_nxRetain(so);
         BaBool restoreRec = SoDispCon_recEvActive(o);
         if(restoreRec)
         {
            SoDisp_deactivateRec(o->dispatcher, o);
         }
         ThreadMutex_release(m);
         status = nx_tcp_socket_receive(ref->socket, &nxPacket, waitOption);
         ThreadMutex_set(m);
         if(*isTerminated || ref->closed)
         {
            if(status == NX_SUCCESS)
               nx_packet_release(nxPacket);
            HttpSocket_nxRelease(ref);
            return E_SOCKET_READ_FAILED;
         }
         HttpSocket_nxRelease(ref);
         if(status == NX_SUCCESS)
         {
            so->nxPacket = so->nxFirstPacket = nxPacket;
            so->nxPktOffs = nxPacket->nx_packet_prepend_ptr;
         }
         if(restoreRec)
         {
            SoDisp_activateRec(o->dispatcher, o);
         }
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
         /* An HTTP handoff may retain rtmo after switching to nonblocking.
            Only an actual timed wait can expire; an empty poll is no data. */
         return waitOption != NX_NO_WAIT && o->rtmo ? E_TIMEOUT : 0;
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
   /* A nonblocking read retains readiness until the native queue reports
      NX_NO_PACKET. Clearing it at a packet boundary can strand buffered TLS
      records after the last native packet has already been consumed. */
   if(!SoDispCon_isNonBlocking(o))
   {
      SoDispCon_clearSocketHasNonBlockData(o);
   }
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
  Register the interfaces (NX_IP objects) that the server can use.
  The IP list and packet pool must outlive the server. Initialize the
  Barracuda heap before creating sockets; socket state uses baMalloc/baFree.
  static NX_IP* ipList[] = {ip1, ip2, 0};
  SoDisp_nxInit(ipList, sendPool, 65535);
 */
void
SoDisp_nxInit(NX_IP** ip, NX_PACKET_POOL* _nxPacketPool,
              ULONG _nxWindowSize)
{
   NX_IP** ipPtr;
   ipPtr = nxIpList = ip;
   nxPacketPool = _nxPacketPool;
   nxWindowSize = _nxWindowSize;
   while(*ipPtr)
   {
      NX_IP* instance = *ipPtr;
      if( ! instance->nx_ip_fragment_processing )
         nx_ip_fragment_enable(instance);
      if( ! instance->nx_ip_tcp_packet_receive )
         nx_tcp_enable(instance);
#if BA_ENABLE_UDP
      if(!instance->nx_ip_udp_packet_receive)
         nx_udp_enable(instance);
#endif
#if BA_ENABLE_MDNS
      if(!instance->nx_ip_igmp_packet_receive)
         nx_igmp_enable(instance);
#endif
      ipPtr++;
   }
}


void
SoDisp_setDNS(void* dns)
{
   (void)dns;
#ifdef BA_NX_DNS
   nxdns=(NX_DNS*)dns;
#else
   baAssert(0);
#endif
}

#if BA_ENABLE_MDNS
/* Native NetX mDNS transport. NetX binds UDP ports across both IP families,
   so share one socket and give each responder/channel a bounded packet queue.
   The IP mutex protects membership, queues and the native notification. The
   BAS mutex serializes setup/close and the transport list; callbacks never
   acquire it or invoke Lua. No TCP/IP stack or Ethernet driver changes. */
static HttpSocketNxUdp* mdnsNxTransports;

static void
mdnsNxGroup(NXD_ADDRESS* group, BaBool ipv6)
{
   memset(group, 0, sizeof(*group));
   group->nxd_ip_version = ipv6 ? NX_IP_VERSION_V6 : NX_IP_VERSION_V4;
#if BA_ENABLE_IPV6
   if(ipv6)
   {
      group->nxd_ip_address.v6[0] = 0xff020000UL;
      group->nxd_ip_address.v6[3] = 0xfb;
   }
   else
#endif
      group->nxd_ip_address.v4 = 0xe00000fbUL;
}

/* BAS reserves zero for an absent interface. Encode the IP instance and
   physical interface as a one-based token; never advertise loopback. */
static NX_IP*
mdnsNxInterface(U32 token, UINT* index)
{
   unsigned i;
   if(!token || !nxIpList)
      return 0;
   --token;
   *index = token % NX_MAX_PHYSICAL_INTERFACES;
   for(i = 0; nxIpList[i]; ++i)
      if(i == token / NX_MAX_PHYSICAL_INTERFACES)
         return nxIpList[i];
   return 0;
}

int
BamDNS_getNetInfo(BamDNS_NetInfo* net)
{
   unsigned n, i;
   if(!net)
      return E_INVALID_PARAM;
   memset(net, 0, sizeof(*net));
   if(!nxIpList)
      return E_CANNOT_RESOLVE;
   for(n = 0; nxIpList[n]; ++n)
   {
      NX_IP* ip = nxIpList[n];
      TST_OK(tx_mutex_get(&ip->nx_ip_protection, TX_WAIT_FOREVER));
      for(i = 0; i < NX_MAX_PHYSICAL_INTERFACES; ++i)
      {
         NX_INTERFACE* iface = &ip->nx_ip_interface[i];
         U32 token = n * NX_MAX_PHYSICAL_INTERFACES + i + 1;
         ULONG addr = iface->nx_interface_ip_address;
         ULONG mask = iface->nx_interface_ip_network_mask;
         NXD_ADDRESS native;
         HttpSockaddr address;
         if(!iface->nx_interface_valid || !iface->nx_interface_link_up)
            continue;
         if(addr && addr < 0xe0000000UL && (addr >> 24) != 127 && mask)
         {
            native.nxd_ip_version = NX_IP_VERSION_V4;
            native.nxd_ip_address.v4 = addr;
            HttpSockaddr_fromNx(&address, &native, ip);
            memcpy(net->addr4, address.addr, 4);
            net->ifIndex4 = token;
            while(mask & 0x80000000UL)
            {
               ++net->prefix4;
               mask <<= 1;
            }
         }
#if BA_ENABLE_IPV6
         {
            unsigned a;
            for(a = 0; a < NX_MAX_IPV6_ADDRESSES; ++a)
            {
               NXD_IPV6_ADDRESS* v6 = &ip->nx_ipv6_address[a];
               if(v6->nxd_ipv6_address_state != NX_IPV6_ADDR_STATE_VALID ||
                  v6->nxd_ipv6_address_attached != iface)
                  continue;
               native.nxd_ip_version = NX_IP_VERSION_V6;
               memcpy(native.nxd_ip_address.v6, v6->nxd_ipv6_address, 16);
               HttpSockaddr_fromNx(&address, &native, ip);
               if(!address.addr[0] || address.addr[0] == 255)
                  continue;
               memcpy(net->addr6, address.addr, 16);
               net->ifIndex6 = token;
               net->prefix6 = v6->nxd_ipv6_address_prefix_length;
               if(address.addr[0] == 0xfe && (address.addr[1] & 0xc0) == 0x80)
                  break;
            }
         }
#endif
         if(net->ifIndex4
#if BA_ENABLE_IPV6
            || net->ifIndex6
#endif
           )
         {
            TST_OK(tx_mutex_put(&ip->nx_ip_protection));
            return 0;
         }
      }
      TST_OK(tx_mutex_put(&ip->nx_ip_protection));
   }
   return E_CANNOT_RESOLVE;
}

static void
mdnsNxNotify(NX_UDP_SOCKET* socket)
{
   HttpSocketNxUdp* transport = (HttpSocketNxUdp*)socket;
   NX_PACKET* packet;
   /* Called by NetX with the IP mutex held. Copies preserve destination and
      receiving-interface metadata. Pool exhaustion drops datagrams without
      blocking the IP thread; normal mDNS retransmission handles loss. */
   while(!nx_udp_socket_receive(socket, &packet, NX_NO_WAIT))
   {
      HttpSocketNxUdp* member;
      UINT iface;
      nxd_udp_packet_info_extract(packet, 0, 0, 0, &iface);
      for(member = transport->mdnsMembers; member; member = member->mdnsNext)
      {
         NX_PACKET* copy;
         if(member->mdnsQueued == 8 || member->mdnsInterface != iface ||
            member->ip6 != (packet->nx_packet_ip_version == NX_IP_VERSION_V6))
            continue;
         if(nx_packet_copy(packet, &copy, packet->nx_packet_pool_owner, NX_NO_WAIT))
            continue;
         copy->nx_packet_ip_version = packet->nx_packet_ip_version;
         copy->nx_packet_queue_next = 0;
         if(member->mdnsTail)
            member->mdnsTail->nx_packet_queue_next = copy;
         else
            member->mdnsHead = copy;
         member->mdnsTail = copy;
         ++member->mdnsQueued;
         HttpSocket_queueEvent(member->mdnsOwner, FALSE);
      }
      nx_packet_release(packet);
   }
}

static UINT
mdnsNxReceive(HttpSocketNxUdp* udp, NX_PACKET** packet)
{
   NX_IP* ip = udp->mdnsTransport->socket.nx_udp_socket_ip_ptr;
   TST_OK(tx_mutex_get(&ip->nx_ip_protection, TX_WAIT_FOREVER));
   *packet = udp->mdnsHead;
   if(*packet)
   {
      udp->mdnsHead = (*packet)->nx_packet_queue_next;
      if(!udp->mdnsHead)
         udp->mdnsTail = 0;
      --udp->mdnsQueued;
      (*packet)->nx_packet_queue_next = 0;
   }
   TST_OK(tx_mutex_put(&ip->nx_ip_protection));
   return *packet ? NX_SUCCESS : NX_NO_PACKET;
}

int
BamDNS_socketSetup(BamDNS_Channel* c, const BamDNS_NetInfo* net)
{
   HttpSocket* owner = &c->con.httpSocket;
   HttpSocketNxUdp* udp = HttpSocket_nxUdp(owner->nxRef);
   HttpSocketNxUdp* transport;
   NXD_ADDRESS group;
   UINT iface, status;
   U32 token = net->ifIndex4;
   NX_IP* ip;
#if BA_ENABLE_IPV6
   if(c->ipv6)
      token = net->ifIndex6;
#endif
   ip = mdnsNxInterface(token, &iface);
   if(!ip)
      return -1;
   mdnsNxGroup(&group, c->ipv6);
   TST_OK(tx_mutex_get(&ip->nx_ip_protection, TX_WAIT_FOREVER));
   status = c->ipv6 ?
#if BA_ENABLE_IPV6
      nxd_ipv6_multicast_interface_join(ip, &group, iface) :
#else
      NX_NOT_SUPPORTED :
#endif
      nx_igmp_multicast_interface_join(ip, group.nxd_ip_address.v4, iface);
   if(status)
   {
      TST_OK(tx_mutex_put(&ip->nx_ip_protection));
      return -1;
   }
   for(transport = mdnsNxTransports; transport;
       transport = transport->mdnsNextTransport)
      if(transport->socket.nx_udp_socket_ip_ptr == ip)
         break;
   if(!transport)
   {
      /* sockUdp may have selected another NX_IP. Recreate on the discovered
         interface, preserving the ordinary UDP path when mDNS is disabled. */
      if(udp->socket.nx_udp_socket_ip_ptr != ip)
      {
         nx_udp_socket_delete(&udp->socket);
         status = nx_udp_socket_create(ip, &udp->socket, "BA mDNS",
                                        NX_IP_NORMAL, NX_FRAGMENT_OKAY, 255, 8);
      }
      else
         udp->socket.nx_udp_socket_time_to_live = 255;
      if(!status)
         status = nx_udp_socket_bind(&udp->socket, 5353, NX_NO_WAIT);
      if(!status)
      {
         transport = udp;
         HttpSocket_nxRetain(owner);
         transport->mdnsNextTransport = mdnsNxTransports;
         mdnsNxTransports = transport;
         nx_udp_socket_receive_notify(&transport->socket, mdnsNxNotify);
      }
   }
   else
      nx_udp_socket_delete(&udp->socket);
   if(!status)
   {
      udp->mdnsTransport = transport;
      udp->mdnsOwner = owner;
      udp->mdnsInterface = iface;
      udp->mdnsNext = transport->mdnsMembers;
      transport->mdnsMembers = udp;
   }
   else
   {
#if BA_ENABLE_IPV6
      if(c->ipv6)
         nxd_ipv6_multicast_interface_leave(ip, &group, iface);
      else
#endif
         nx_igmp_multicast_interface_leave(ip, group.nxd_ip_address.v4, iface);
   }
   TST_OK(tx_mutex_put(&ip->nx_ip_protection));
   return status == NX_PORT_UNAVAILABLE ? E_BIND : status ? -1 : 0;
}

static void
mdnsNxClose(HttpSocketNxUdp* udp)
{
   HttpSocketNxUdp* transport = udp->mdnsTransport;
   HttpSocketNxUdp** link;
   NX_IP* ip = transport->socket.nx_udp_socket_ip_ptr;
   NX_PACKET* packet;
   NXD_ADDRESS group;
   BaBool last;
   TST_OK(tx_mutex_get(&ip->nx_ip_protection, TX_WAIT_FOREVER));
   for(link = &transport->mdnsMembers; *link != udp; link = &(*link)->mdnsNext)
      ;
   *link = udp->mdnsNext;
   while((packet = udp->mdnsHead) != 0)
   {
      udp->mdnsHead = packet->nx_packet_queue_next;
      nx_packet_release(packet);
   }
   mdnsNxGroup(&group, udp->ip6);
#if BA_ENABLE_IPV6
   if(udp->ip6)
      nxd_ipv6_multicast_interface_leave(ip, &group, udp->mdnsInterface);
   else
#endif
      nx_igmp_multicast_interface_leave(ip, group.nxd_ip_address.v4, udp->mdnsInterface);
   udp->mdnsTransport = 0;
   last = transport->mdnsMembers == 0;
   if(last)
   {
      nx_udp_socket_receive_notify(&transport->socket, 0);
      nx_udp_socket_unbind(&transport->socket);
      TST_OK(nx_udp_socket_delete(&transport->socket));
   }
   TST_OK(tx_mutex_put(&ip->nx_ip_protection));
   if(last)
   {
      for(link = &mdnsNxTransports; *link != transport; link = &(*link)->mdnsNextTransport)
         ;
      *link = transport->mdnsNextTransport;
      HttpSocket_nxRelease(((HttpSocketNxRef*)transport) - 1);
   }
}

static BaBool
mdnsNxPrefix(const U8* a, const U8* b, unsigned bits)
{
   unsigned n = bits / 8;
   if(memcmp(a, b, n))
      return FALSE;
   bits %= 8;
   return !bits || !((a[n] ^ b[n]) & (0xff << (8 - bits)));
}

int
BamDNS_socketRecv(BamDNS_Channel* c, const BamDNS_NetInfo* net,
                  void* data, int size, BamDNS_Peer* peer)
{
   HttpSocket* s = &c->con.httpSocket;
   HttpSocketNxUdp* udp = HttpSocket_nxUdp(s->nxRef);
   NX_PACKET* packet = s->nxPacket;
   NXD_ADDRESS source, dest, group;
   HttpSockaddr destination;
   UINT port, iface;
   ULONG length = 0;
   BaBool accept;
   if(!packet && mdnsNxReceive(udp, &packet))
      return -2;
   s->nxPacket = s->nxFirstPacket = 0;
   s->nxPktOffs = 0;
   SoDispCon_clearSocketHasNonBlockData(&c->con);
   nxd_udp_packet_info_extract(packet, &source, 0, &port, &iface);
   dest.nxd_ip_version = source.nxd_ip_version;
#if BA_ENABLE_IPV6
   if(c->ipv6)
      memcpy(dest.nxd_ip_address.v6,
             ((NX_IPV6_HEADER*)packet->nx_packet_ip_header)->nx_ip_header_destination_ip, 16);
   else
#endif
      dest.nxd_ip_address.v4 =
         ((NX_IPV4_HEADER*)packet->nx_packet_ip_header)->nx_ip_header_destination_ip;
   HttpSockaddr_fromNx(&peer->addr, &source, udp->mdnsTransport->socket.nx_udp_socket_ip_ptr);
   HttpSockaddr_fromNx(&destination, &dest, peer->addr.ip);
   peer->port = (U16)port;
   mdnsNxGroup(&group, c->ipv6);
   peer->multicast = !memcmp(&dest.nxd_ip_address, &group.nxd_ip_address, c->ipv6 ? 16 : 4);
#if BA_ENABLE_IPV6
   if(c->ipv6)
      accept = peer->multicast || (!memcmp(destination.addr, net->addr6, 16) &&
         ((peer->addr.addr[0] == 0xfe && (peer->addr.addr[1] & 0xc0) == 0x80) ||
          mdnsNxPrefix(peer->addr.addr, net->addr6, net->prefix6)));
   else
#endif
      accept = peer->multicast || (!memcmp(destination.addr, net->addr4, 4) &&
         ((peer->addr.addr[0] == 169 && peer->addr.addr[1] == 254) ||
          mdnsNxPrefix(peer->addr.addr, net->addr4, net->prefix4)));
   if(accept && port && iface == udp->mdnsInterface &&
      packet->nx_packet_length <= (ULONG)size)
      nx_packet_data_retrieve(packet, data, &length);
   nx_packet_release(packet);
   return (int)length;
}

int
BamDNS_socketSend(BamDNS_Channel* c, const BamDNS_NetInfo* net,
                  const void* data, int size, const BamDNS_Peer* peer)
{
   HttpSocketNxUdp* udp = HttpSocket_nxUdp(c->con.httpSocket.nxRef);
   NX_UDP_SOCKET* socket = &udp->mdnsTransport->socket;
   NX_IP* ip = socket->nx_udp_socket_ip_ptr;
   NX_PACKET_POOL* pool = nxPacketPool ? nxPacketPool : ip->nx_ip_default_packet_pool;
   NX_PACKET* packet;
   NXD_ADDRESS dest;
   UINT index = udp->mdnsInterface, status;
   if(peer)
      HttpSockaddr_toNx(&peer->addr, &dest);
   else
      mdnsNxGroup(&dest, c->ipv6);
   TST_OK(tx_mutex_get(&ip->nx_ip_protection, TX_WAIT_FOREVER));
#if BA_ENABLE_IPV6
   if(c->ipv6)
   {
      HttpSockaddr local;
      NXD_ADDRESS native;
      memset(&local, 0, sizeof(local));
      local.isIp6 = TRUE;
      memcpy(local.addr, net->addr6, 16);
      HttpSockaddr_toNx(&local, &native);
      for(index = 0; index < NX_MAX_IPV6_ADDRESSES; ++index)
         if(ip->nx_ipv6_address[index].nxd_ipv6_address_state == NX_IPV6_ADDR_STATE_VALID &&
            ip->nx_ipv6_address[index].nxd_ipv6_address_attached == &ip->nx_ip_interface[udp->mdnsInterface] &&
            !memcmp(ip->nx_ipv6_address[index].nxd_ipv6_address, native.nxd_ip_address.v6, 16))
            break;
      if(index == NX_MAX_IPV6_ADDRESSES)
      {
         TST_OK(tx_mutex_put(&ip->nx_ip_protection));
         return -1;
      }
   }
#else
   (void)net;
#endif
   status = nx_packet_allocate(pool, &packet, NX_UDP_PACKET, NX_NO_WAIT);
   if(!status)
   {
      status = nx_packet_data_append(packet, (void*)data, (ULONG)size, pool, NX_NO_WAIT);
      if(!status)
         status = nxd_udp_socket_source_send(socket, packet, &dest, peer ? peer->port : 5353, index);
      if(status)
         nx_packet_release(packet);
   }
   TST_OK(tx_mutex_put(&ip->nx_ip_protection));
   return !status ? size : status == NX_NO_PACKET ? -2 : -1;
}
#endif /* BA_ENABLE_MDNS */


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
   if(!sock->nxListener)
   {
      if(sock->nxRef)
      {
         if( ! sock->nxPacket )
         {
            UINT status = HttpSocket_receive(
               sock->nxRef, &sock->nxPacket, NX_NO_WAIT);
            if(status)
            {
               sock->nxPacket=0;
               if(status == NX_NO_PACKET)
                  return; /* Nothing to do */
            }
            else
            {
               sock->nxFirstPacket=sock->nxPacket;
               sock->nxPktOffs=sock->nxPacket->nx_packet_prepend_ptr;
            }
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
   if(!SoDispCon_sendEvActive(con) && DoubleLink_isLinked(&con->httpSocket))
      DoubleLink_unlink(&con->httpSocket); /* If in SoDisp queueu */
   TX_RESTORE;
}

#if BA_HAVE_SOCKET_WRITE_EVENTS
void
SoDisp_activateSend(SoDisp* o, SoDispCon* con)
{
   baAssert(!SoDispCon_sendEvActive(con));
   baAssert(!DoubleLink_isLinked(&con->httpSocket.nxWriteLink));
   SoDispCon_setSendEvActive(con);
   DoubleList_insertLast(&o->nxWriters, &con->httpSocket.nxWriteLink);
   /* Interest is registered before waking/rechecking, so an earlier ACK or
      pool release cannot be lost. Retry only while the writer list is nonempty. */
   TST_OK(tx_semaphore_put(&o->queueSem));
}

void
SoDisp_deactivateSend(SoDisp* o, SoDispCon* con)
{
   TX_INTERRUPT_SAVE_AREA;
   (void)o;
   baAssert(SoDispCon_sendEvActive(con));
   SoDispCon_setSendEvInactive(con);
   if(DoubleLink_isLinked(&con->httpSocket.nxWriteLink))
   {
      DoubleLink_unlink(&con->httpSocket.nxWriteLink);
   }
   TX_DISABLE;
   if(!SoDispCon_recEvActive(con) && DoubleLink_isLinked(&con->httpSocket))
   {
      DoubleLink_unlink(&con->httpSocket);
   }
   TX_RESTORE;
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
   DoubleList_constructor(&o->nxKeepAlive);
#if BA_HAVE_SOCKET_WRITE_EVENTS
   DoubleList_constructor(&o->nxWriters);
#endif
   TST_OK(tx_semaphore_create(&o->queueSem,"SoDisp",0));
   TST_OK(tx_semaphore_create(&o->nxCloseSem,"NetX close",0));
   TST_OK(tx_thread_create(&o->nxCloseThread, "NetX close",
                          HttpSocket_closeWorker, (ULONG)o,
                          o->nxCloseStack, sizeof(o->nxCloseStack),
                          3, 3, TX_NO_TIME_SLICE, TX_AUTO_START));
}


void
SoDisp_run(SoDisp* o, S32 msecTimeout)
{
   ULONG waitOption = msecTimeout < 0 ?
      TX_WAIT_FOREVER : baMsTime2TxTicks((U32)msecTimeout);
   SoDisp_mutexSet(o);
   o->doExit = FALSE;
   while(!o->doExit)
   {
      HttpSocket* sock;
      ULONG wait = waitOption;
      TX_INTERRUPT_SAVE_AREA;
#ifdef NX_ENABLE_TCP_KEEPALIVE
      if(!DoubleList_isEmpty(&o->nxKeepAlive) &&
         wait > baMsTime2TxTicks(100))
      {
         wait = baMsTime2TxTicks(100);
      }
#endif
#if BA_HAVE_SOCKET_WRITE_EVENTS
      if(!DoubleList_isEmpty(&o->nxWriters) && wait != TX_NO_WAIT)
      {
         wait = 1; /* Tick retry covers pure window updates and pool release. */
      }
#endif
      SoDisp_mutexRelease(o);
      tx_semaphore_get(&o->queueSem, wait);
      SoDisp_mutexSet(o);
#ifdef NX_ENABLE_TCP_KEEPALIVE
      HttpSocket_keepAlivePoll(o);
#endif
#if BA_HAVE_SOCKET_WRITE_EVENTS
      {
         DoubleListEnumerator iter;
         DoubleLink* link;
         DoubleListEnumerator_constructor(&iter, &o->nxWriters);
         for(link = DoubleListEnumerator_getElement(&iter); link;
             link = DoubleListEnumerator_nextElement(&iter))
         {
            sock = (HttpSocket*)((U8*)link - offsetof(HttpSocket, nxWriteLink));
            if(HttpSocket_writeReady(sock))
            {
               HttpSocket_queueEvent(sock, TRUE);
            }
         }
      }
#endif
      TX_DISABLE;
      sock = (HttpSocket*)DoubleList_removeFirst(&o->soDispQueue);
      TX_RESTORE;
      if(sock)
      {
         SoDispCon* con = httpSocket2Con(sock);
         o->curSock = sock;
#if BA_HAVE_SOCKET_WRITE_EVENTS
         if(SoDispCon_sendEvActive(con) && HttpSocket_writeReady(sock))
         {
            if(sock->nxRef)
            {
               sock->nxRef->connecting = FALSE;
            }
            SoDispCon_dispSendEvent(con);
         }
#endif
         /* A send callback may close, move or free its connection. */
         while(sock == o->curSock && SoDispCon_recEvActive(con))
         {
            UINT status;
            /* Establish/disconnect/queue notifications may remain queued
               after a listener's accepted socket has been transferred.
               A listening socket has no readable stream to dispatch. */
            if(sock->nxListener && !sock->nxAcceptStatus)
            {
               break;
            }
            if(sock->nxRef)
            {
               if(!sock->nxPacket && !sock->nxAcceptStatus)
               {
                  status = HttpSocket_receive(sock->nxRef, &sock->nxPacket, NX_NO_WAIT);
                  if(status)
                  {
                     sock->nxPacket = 0;
                     if(status == NX_NO_PACKET)
                     {
                        break;
                     }
                  }
                  else
                  {
                     sock->nxFirstPacket = sock->nxPacket;
                     sock->nxPktOffs = sock->nxPacket->nx_packet_prepend_ptr;
                  }
               }
            }
            SoDispCon_setDispHasRecData(con);
            SoDispCon_dispRecEvent(con);
         }
         o->curSock = 0;
      }
   }
   SoDisp_mutexRelease(o);
}
