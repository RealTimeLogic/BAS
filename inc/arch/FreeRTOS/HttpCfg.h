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
 *   $Id: HttpCfg.h 4914 2021-12-01 18:24:30Z wini $
 *   COPYRIGHT:  Real Time Logic, 2019
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
 * Platform: FreeRTOS+TCP.
 */

#ifndef _HttpConfig_h
#define _HttpConfig_h

#include <FreeRTOS.h>
#include <task.h>
#include <FreeRTOS_IP.h>
#include <FreeRTOS_sockets.h>

#if socketsconfigBYTE_ORDER == pdLITTLE_ENDIAN
#define B_LITTLE_ENDIAN
#elif socketsconfigBYTE_ORDER == pdBIG_ENDIAN
#define B_BIG_ENDIAN
#else
#error socketsconfigBYTE_ORDER not defined
#endif

#include <TargConfig.h>
#include <NetConv.h>

#define BA_FREERTOS_PLUS_TCP

#ifdef __cplusplus
extern "C" {
#endif

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

struct HttpSocket;

typedef struct HttpSocket
{
   Socket_t xSocket;
   U16 port;
   BaBool isNonBlock;
   U8 selectBit; /* bit 1: set read, bit 2: set */
} HttpSocket;


void HttpSocket_constructor(HttpSocket* o);

//PATCH
#define HttpSocket_invalidate(o) (o)->xSocket = FREERTOS_INVALID_SOCKET
#define HttpSocket_shutdown(o) HttpSocket_close(o)
#define HttpSocket_hardClose(o) HttpSocket_close(o)

#define HttpSocket_getId(o) (int)(o)->xSocket
#define HttpSocket_errno(o, status, ecode) (*(ecode) = -status)

int _HttpSocket_setNonblockingOnOff(HttpSocket* o, int set);

#define HttpSocket_setBlocking(o, status) \
   *(status) = _HttpSocket_setNonblockingOnOff(o, FALSE)
#define HttpSocket_setNonblocking(o, status) \
   *(status) = _HttpSocket_setNonblockingOnOff(o, TRUE)


#define HttpSocket_wouldBlock(o, status) \
   *(status) == *(status) == pdFREERTOS_ERRNO_EWOULDBLOCK ? TRUE : FALSE


#define HttpSocket_isValid(o) ((o)->xSocket != FREERTOS_INVALID_SOCKET)

void HttpSocket_setTCPNoDelay(HttpSocket* o, int enable, int* status);


#define HttpSocket_soReuseaddr(o, status) (*(status)=0)

void HttpSocket_sockStream(HttpSocket*,const char*,BaBool,int*);
void HttpSocket_bind(HttpSocket*,HttpSockaddr*,U16,int*);
void HttpSocket_listen(HttpSocket*,HttpSockaddr*,int,int*);
void HttpSocket_connect(HttpSocket*,HttpSockaddr*,U16,int*);
void HttpSocket_accept(HttpSocket*,HttpSocket*,int*);
void HttpSocket_close(HttpSocket*);

void HttpSocket_getAddr(HttpSocket*,HttpSockaddr*,U16*,BaBool,int*, int);
#define HttpSocket_getSockName(o, sockaddr, port, ip6, status) \
   HttpSocket_getAddr(o, sockaddr, port, ip6, status, TRUE)
#define HttpSocket_getPeerName(o, sockaddr, port, ip6, status) \
   HttpSocket_getAddr(o, sockaddr, port, ip6, status, FALSE)

void HttpSocket_move(HttpSocket*,HttpSocket* newS);
#define HttpSocket_recv(o, data, len, retlen) \
   *(retlen)=FreeRTOS_recv((o)->xSocket,data,(size_t)len,0);

void HttpSocket_send(HttpSocket*, struct ThreadMutex* m,
                     BaBool* isTerminated, const void*,int,int*);

#if 0
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
 
*/
#define CONNECTION_DISPATCHER_OBJ DoubleLink dispatcherLink;
 

/***********************************************************************
 *  SoDisp
 ***********************************************************************/
#include <DoubleList.h>
#define DISPATCHER_DATA                         \
   SocketSet_t xFDSet;                          \
   DoubleList conList;                          \
   DoubleLink* curL;                            \
   DoubleListEnumerator iter;                   \
   int defaultPollDelay;                        \
   int pollDelay;                               \
   BaBool selectSet

#ifdef __cplusplus
}
#endif 

#endif
