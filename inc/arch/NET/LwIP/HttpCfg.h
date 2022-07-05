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
 *   $Id: HttpCfg.h 5186 2022-06-08 21:13:22Z wini $
 *   COPYRIGHT:  Real Time Logic, 2015 - 2022
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
 */

#ifndef _HttpConfig_h
#define _HttpConfig_h

#ifndef BA_LWIP
#define BA_LWIP
#endif

#ifdef ESP_PLATFORM
#include <lwip/errno.h>
#endif
#include <lwip/opt.h>
#include <lwip/arch.h>
#include <lwip/api.h>

#include <TargConfig.h>
#include <NetConv.h>
#include <DoubleList.h>


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
   U8 addr[16]; /* Must be long aligned */

   BaBool isIp6;
} HttpSockaddr;


void HttpSockaddr_gethostbyname(HttpSockaddr*, const char*, BaBool, int*);


/***********************************************************************
 *  HttpSocket
 ***********************************************************************/

#define USE_DGRAM

typedef struct HttpSocket
{
   DoubleLink super; /*Used when in SoDisp queue. */
   struct netconn* ncon;
   void* nbuf; /* pbuf when TCP, netbuf when UDP */
   int pbOffs;
   int recEvent;
   U8 sendEvent;
   U8 errEvent;
   U8 isDgram;
} HttpSocket;


#define HttpSocket_constructor(o) memset(o, 0, sizeof(HttpSocket))


#define HttpSocket_invalidate(o) HttpSocket_close(o)
#define HttpSocket_shutdown(o) HttpSocket_close(o)
#define HttpSocket_hardClose(o) HttpSocket_close(o)


#define HttpSocket_errno(o, status, ecode) (*(ecode) = status)

void HttpSocket_setBlocking(HttpSocket* o, int* status);

void HttpSocket_setNonblocking(HttpSocket* o, int* status);

void HttpSocket_wouldBlock(HttpSocket* o, int* status);

#define HttpSocket_isValid(o) ((o)->ncon ? TRUE : FALSE)

void HttpSocket_setTCPNoDelay(HttpSocket* o, int enable, int* status);


#define HttpSocket_soReuseaddr(o, status) \
  (*(status)=0)

#define HttpSocket_setmembership(o,enable,ipv6,multiAddrName,intfName) -1
#define HttpSocket_sockUdp(o, host, ip6, status) \
   _HttpSocket_newSock(o, host, NETCONN_UDP, ip6, status)
#define HttpSocket_sockStream(o, host, ip6, status) \
   _HttpSocket_newSock(o, host, NETCONN_TCP, ip6, status)
void _HttpSocket_newSock(HttpSocket* o, const char* host, enum netconn_type t,
                         BaBool ip6, int* status);

void HttpSocket_bind(HttpSocket*,HttpSockaddr*,U16,int*);
void HttpSocket_listen(HttpSocket*,HttpSockaddr*,int,int*);
void HttpSocket_connect(HttpSocket*,HttpSockaddr*,U16,int*);
void HttpSocket_accept(HttpSocket*,HttpSocket*,int*);
void HttpSocket_close(HttpSocket*);
U32 HttpSocket_getId(HttpSocket*);

void HttpSocket_getAddr(HttpSocket*,HttpSockaddr*,U16*,BaBool,int*, int);
#define HttpSocket_getSockName(o, sockaddr, port, ip6, status) \
   HttpSocket_getAddr(o, sockaddr, port, ip6, status, TRUE)
#define HttpSocket_getPeerName(o, sockaddr, port, ip6, status) \
   HttpSocket_getAddr(o, sockaddr, port, ip6, status, FALSE)

void HttpSocket_move(HttpSocket*,HttpSocket* newS);
void HttpSocket_send(HttpSocket*, struct ThreadMutex* m,
                     BaBool* isTerminated, const void*,int,int*);

void HttpSocket_sendto(
   HttpSocket*,const void*,int,struct HttpSockaddr*,U16,int*);
#define HttpSocket_soDontroute(o, enableFlag, status) (*(status))=-1
#define HttpSocket_soBroadcast(o, enableFlag, status) (*(status))=-1

#if LWIP_TCP_KEEPALIVE == 1
void _HttpSocket_getKeepAlive(HttpSocket* o, int* enablePtr, int* statusPtr);
void _HttpSocket_setKeepAlive(HttpSocket* o, int enable, int* statusPtr);
void _HttpSocket_getKeepAliveEx(
   HttpSocket* o,int* enablePtr,int* timePtr,int* intervalPtr,int* statusPtr);
void _HttpSocket_setKeepAliveEx(
   HttpSocket* o,int enable,int time, int interval,int* statusPtr);
#define HttpSocket_getKeepAlive _HttpSocket_getKeepAlive
#define HttpSocket_setKeepAlive _HttpSocket_setKeepAlive
#define HttpSocket_getKeepAliveEx _HttpSocket_getKeepAliveEx
#define HttpSocket_setKeepAliveEx _HttpSocket_setKeepAliveEx
#endif

void HttpSockaddr_inetAddr(
   HttpSockaddr* o, const char* host, BaBool useIp6, int* status);

int HttpSocket_makeClientCon(
   HttpSocket* o,struct HttpSockaddr* addr,
   const char* interfaceName,U16 port, BaBool nonBlocking);

void HttpSockaddr_addr2String(
   HttpSockaddr* o, char* buf, int bufLen, int* status);



/***********************************************************************
 *  SoDispCon
 ***********************************************************************/

/*
   Object declared in SoDispCon and used by SoDisp.
   Not used by the LwIP port.
*/
#define CONNECTION_DISPATCHER_OBJ

#define SoDispCon_newConnectionIsReady(x)
struct SoDispCon;
#define USE_SoDispCon_recvfrom
#define SoDispCon_recvfrom(o,data,len,addr,port) \
   SoDispCon_lwipRec(o,0,0,data,len,addr,port)
#define SoDispCon_platReadData(o,m,isTerminated, data, len) \
   SoDispCon_lwipRec(o,m,isTerminated, data, len,0,0)
int SoDispCon_lwipRec(
   struct SoDispCon* o, ThreadMutexBase* m, BaBool* isTerminated,
   void* data, int len, HttpSockaddr* addr, U16* port);


/***********************************************************************
 *  SoDisp
 ***********************************************************************/


typedef struct
{
   struct netconn* ncon;
   int recEvent;
} SoDispAcceptQNode;

#define DISP_ACCEPT_Q_LEN 12

#define DISPATCHER_DATA \
  DoubleList soDispQueue;\
  SoDispAcceptQNode acceptQ[DISP_ACCEPT_Q_LEN];\
  sys_sem_t queueSem;\
  HttpSocket* curSock


#ifdef __cplusplus
}
#endif 

#endif
