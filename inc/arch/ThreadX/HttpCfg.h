/*
 *     ____             _________                __                _     
 *    / __ \___  ____ _/ /_  __(_)___ ___  ___  / /   ____  ____ _(_)____
 *   / /_/ / _ \/ __ `/ / / / / / __ `__ \/ _ \/ /   / __ \/ __ `/ / ___/
 *  / _, _/  __/ /_/ / / / / / / / / / / /  __/ /___/ /_/ / /_/ / / /__  
 * /_/ |_|\___/\__,_/_/ /_/ /_/_/ /_/ /_/\___/_____/\____/\__, /_/\___/  
 *                                                       /____/          
 *
 *                  Barracuda Embedded Web-Server
 *
 ****************************************************************************
 *                            HEADER
 *
 *   $Id: HttpCfg.h 6761 2026-11-06 00:38:14Z wini $
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
 ****************************************************************************
 *
 *
 * Platform: ThreadX.
 */

#ifndef _HttpConfig_h
#define _HttpConfig_h

/* Load application input before selecting network defaults. */
#ifndef BA_INTERNAL_CONFIG_OPTS_LOADED
#define BA_INTERNAL_CONFIG_OPTS_LOADED 1
#include <BaConfigOpts.h>
#endif

/* Requires matching NetX Duo notification settings; define 0 for blocking. */
#ifndef BA_HAVE_SOCKET_WRITE_EVENTS
#define BA_HAVE_SOCKET_WRITE_EVENTS 1
#endif

#include <TargConfig.h>
#include <NetConv.h>
#include <DoubleList.h>
#include <nx_api.h>

#if !defined(__NETXDUO_MAJOR_VERSION__) || __NETXDUO_MAJOR_VERSION__ < 6
#error ThreadX port requires NetX Duo 6 or newer
#endif

/* The Windows simulator includes RPC headers through tx_port.h. */
#if defined(_WIN32) && defined(small)
#undef small
#endif

/* NetX loads nx_user.h through nx_port.h before these defaults are selected.
   NX_DISABLE_IPV6 is presence-tested, including when its value is zero. */
#ifndef BA_ENABLE_IPV6
#ifndef NX_DISABLE_IPV6
#define BA_ENABLE_IPV6 1
#else
#define BA_ENABLE_IPV6 0
#endif
#endif

#if BA_HAVE_SOCKET_WRITE_EVENTS
#ifndef NX_ENABLE_EXTENDED_NOTIFY_SUPPORT
#error BA_HAVE_SOCKET_WRITE_EVENTS requires NX_ENABLE_EXTENDED_NOTIFY_SUPPORT
#endif
#ifndef NX_ENABLE_TCP_QUEUE_DEPTH_UPDATE_NOTIFY
#error BA_HAVE_SOCKET_WRITE_EVENTS requires NX_ENABLE_TCP_QUEUE_DEPTH_UPDATE_NOTIFY
#endif
#endif

#if BA_ENABLE_IPV6 && !defined(FEATURE_NX_IPV6)
#error BA_ENABLE_IPV6 requires NetX Duo IPv6 support
#endif


#ifdef __cplusplus
extern "C" {
#endif


/***********************************************************************
 *  Forward decl.
 ***********************************************************************/

struct SoDisp;
struct HttpSockaddr;
struct ThreadMutex;

/***********************************************************************
 *  HttpSockaddr
 ***********************************************************************/

typedef struct HttpSockaddr
{
      NX_IP* ip;
      U8 addr[16]; /* Must be long aligned */
      BaBool isIp6;
} HttpSockaddr;


void HttpSockaddr_gethostbyname(HttpSockaddr*, const char*, BaBool, int*);


/***********************************************************************
 *  HttpSocket
 ***********************************************************************/

typedef struct HttpSocket
{
      DoubleLink super; /*Used when in SoDisp queue. Inserted by NX callback*/
      NX_TCP_SOCKET* nxSock;
      struct HttpSocketNxRef* nxRef;
      NX_PACKET* nxFirstPacket;
      NX_PACKET* nxPacket;
      UCHAR* nxPktOffs;
      int nxAcceptStatus; /* No packet-pointer encoding for pending accept. */
      UINT nxBindPort; /* Applied by the native client or server operation. */
      BaBool nxListener;
#if BA_ENABLE_UDP
      BaBool nxDgram; /* Native UDP state is allocated only for UDP sockets. */
#endif
#if BA_HAVE_SOCKET_WRITE_EVENTS
      BaBool nxNonblocking;
      BaBool nxWouldBlock;
      DoubleLink nxWriteLink; /* Active writers only; owned by the BAS mutex. */
#endif
} HttpSocket;


#define HttpSocket_constructor(o) memset(o, 0, sizeof(HttpSocket))

#define HttpSocket_invalidate(o) HttpSocket_nxClose(o, TRUE)
#define HttpSocket_shutdown(o) HttpSocket_nxClose(o, FALSE)
#define HttpSocket_hardClose(o) HttpSocket_nxClose(o, TRUE)
#define HttpSocket_close(o) HttpSocket_nxClose(o, FALSE)

#define HttpSocket_errno(o, status, ecode) (*(ecode) = status)

#if BA_HAVE_SOCKET_WRITE_EVENTS
#define HttpSocket_setBlocking(o, status) ((o)->nxNonblocking=FALSE, *(status)=0)
#define HttpSocket_setNonblocking(o, status) ((o)->nxNonblocking=TRUE, *(status)=0)
#define HttpSocket_wouldBlock(o, status) (*(status)=(o)->nxWouldBlock)
#else
#define HttpSocket_setBlocking(o, status) (*(status)=0)
#define HttpSocket_setNonblocking(o, status) (*(status)=0)
#define HttpSocket_wouldBlock(o, status) (*(status)=FALSE)
#endif

#define HttpSocket_isValid(o) ((o)->nxRef ? TRUE : FALSE)

#define HttpSocket_setTCPNoDelay(o, enable, status) \
  (*(status)=0)

#define HttpSocket_soReuseaddr(o, status) \
  (*(status)=0)

BA_API int HttpSocket_create(HttpSocket* o,
                             const char* bindIntfName,
                             U16 port,
                             BaBool dgram,
                             BaBool ipv6);
void HttpSocket_sockStream(HttpSocket*,const char*,BaBool,int*);
void HttpSocket_bind(HttpSocket*,HttpSockaddr*,UINT,int*);
void HttpSocket_listen(HttpSocket*,HttpSockaddr*,int,int*);
void HttpSocket_connect(HttpSocket*,HttpSockaddr*,UINT,int*);
void HttpSocket_accept(HttpSocket*,HttpSocket*,int*);
void HttpSocket_nxClose(HttpSocket* o, int hardClose);
#ifdef NX_ENABLE_TCP_KEEPALIVE
void HttpSocket_nxKeepAlive(HttpSocket*,int,int,int,int*);
#define HttpSocket_setKeepAlive(o,enable,status) \
   HttpSocket_nxKeepAlive(o,enable,0,0,status)
#define HttpSocket_setKeepAliveEx HttpSocket_nxKeepAlive
#endif
U32 HttpSocket_getId(HttpSocket*);
void HttpSocket_getPeerName(HttpSocket*,HttpSockaddr*,U16*,BaBool,int*);
void HttpSocket_getSockName(HttpSocket*,HttpSockaddr*,U16*,BaBool,int*);
void HttpSocket_move(HttpSocket*,HttpSocket* newS);
void HttpSocket_send(HttpSocket*, struct ThreadMutex* m,
                     BaBool* isTerminated, const void*,int,int*);
void HttpSockaddr_inetAddr(
   HttpSockaddr* o, const char* host, BaBool useIp6, int* status);

int HttpSocket_makeClientCon(
   HttpSocket* o,struct HttpSockaddr* addr,
   const char* interfaceName,U16 port, BaBool nonBlocking);

void HttpSockaddr_addr2String(
   HttpSockaddr* o, char* buf, int bufLen, int* status);

#if BA_ENABLE_UDP
void HttpSocket_sockUdp(HttpSocket*,const char*,BaBool,int*);
void HttpSocket_sendto(HttpSocket*,const void*,int,HttpSockaddr*,U16,int*);
void HttpSocket_nxBroadcast(HttpSocket*,int,int*);
#define HttpSocket_soBroadcast HttpSocket_nxBroadcast
#define HttpSocket_soDontroute(o,enable,status) (*(status) = -1)
#define HttpSocket_setmembership(o,enable,ipv6,group,intf) (-1)
#endif

/***********************************************************************
 *  SoDispCon
 ***********************************************************************/

/*
   Object declared in SoDispCon and used by SoDisp.
   Not used by the ThreadX port.
*/
#define CONNECTION_DISPATCHER_OBJ

struct SoDispCon; /* forward decl */

#if BA_ENABLE_UDP
#define BA_HAVE_SODISPCON_RECVFROM 1
#define SoDispCon_platPeekDgram SoDispCon_nxPeekDgram
#define SoDispCon_recvfrom SoDispCon_nxRecvfrom
int SoDispCon_nxPeekDgram(struct SoDispCon*,struct ThreadMutex*,BaBool*,void*,int);
int SoDispCon_nxRecvfrom(struct SoDispCon*,void*,int,HttpSockaddr*,U16*);
#endif

#define SoDispCon_newConnectionIsReady(x)

#ifdef FEATURE_NX_IPV6
int SoDispCon_isIP6NetX(struct SoDispCon*);
#define SoDispCon_isIP6(o) SoDispCon_isIP6NetX(o)
#endif

/***********************************************************************
 *  SoDisp
 ***********************************************************************/


#if BA_HAVE_SOCKET_WRITE_EVENTS
#define BA_NX_WRITE_DATA DoubleList nxWriters;
#else
#define BA_NX_WRITE_DATA
#endif

#define DISPATCHER_DATA \
  DoubleList soDispQueue;\
  TX_SEMAPHORE queueSem;\
  TX_SEMAPHORE nxCloseSem;\
  TX_THREAD nxCloseThread;\
  ULONG nxCloseStack[1024];\
  struct HttpSocketNxRef* nxCloseHead;\
  struct HttpSocketNxRef* nxCloseTail;\
  DoubleList nxKeepAlive;\
  BA_NX_WRITE_DATA \
  HttpSocket* curSock

void SoDisp_nxInit(NX_IP** ip, NX_PACKET_POOL* nxPacketPool,
                   ULONG nxWindowSize);
void SoDisp_setDNS(void* dns);


#ifdef __cplusplus
}
#endif 

#endif
