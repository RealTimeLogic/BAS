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
 *			      HEADER
 *
 *   $Id: HttpCfg.h 4914 2021-12-01 18:24:30Z wini $
 *
 *   COPYRIGHT:  Real Time Logic, 2004
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
 *  MQX
 */

#ifndef _HttpConfig_h
#define _HttpConfig_h

#include <TargConfig.h>
#include <rtcs.h>
#include <BaServerLib.h>
#include <NetConv.h>

/* Global MQX macros that conflict with the C++ wrappers */
#ifdef __cplusplus
#undef send
#endif


/* Not implemented due to limitations in the TCP/IP stack
 */
#define NO_ASYNCH_RESP


/***********************************************************************
 *  The HttpSocket API
 ***********************************************************************/

/* Overload the following default data and functions in gBsdSock.h */

/* INADDR_NONE is used in HttpSockaddr_gethostbynameIp4 */
#define INADDR_NONE -1

#define SocketHandle uint_32 

#define socketBind(s, sIn, size) bind(s,(struct sockaddr_in*)(sIn),size)
#define socketClose(s) shutdown(s,FLAG_CLOSE_TX)
#define socketErrno(s) RTCS_geterror(s)
#define socketConnect(sock, addr, len) connect(sock, (sockaddr_in*)addr, len)


#define HttpSocket_getPeerName(o, sockAddr, port, setIsIp6, status) do { \
   struct sockaddr_in in; uint_16 size=sizeof(struct sockaddr_in);\
   *(status) = socketGetPeerName((o)->hndl, &in, &size); \
   *((U32*)(sockAddr)->addr) = in.sin_addr.s_addr; \
   baAssert(setIsIp6 == FALSE); \
   if(port) *(port)=baHtons(in.sin_port);\
   addr->isIp6=FALSE; \
} while(0)

#define HttpSocket_getSockName(o, sockAddr, port,setIsIp6, status) do { \
   struct sockaddr_in in; uint_16 size=sizeof(struct sockaddr_in);\
   *(status) = socketGetSockName((o)->hndl, &in, &size); \
   *((U32*)(sockAddr)->addr) = in.sin_addr.s_addr; \
   baAssert(setIsIp6 == FALSE); \
   if(port) *(port)=baHtons(in.sin_port);\
   addr->isIp6=FALSE; \
} while(0)

#define HttpSocket_invalidate(o) (o)->hndl = RTCS_SOCKET_ERROR
#define HttpSocket_isValid(o) ((o)->hndl != RTCS_SOCKET_ERROR)

/* MQX receive:
   n > 0: data received.
   n == 0: no data. Socket is non-blocking
   n < 0: Error condition including graceful and non-graceful disconnect.

   The above return values map directly to the return values used by Barracuda.
*/
#define HttpSocket_recv(o, data, len, retLen) do { \
  *(retLen)=recv((o)->hndl,data,len,0);\
} while(0)

/* MQX send:
    n >= 0: data received. A non blocking socket if n != send length.
    n < 0: Error condition including graceful and non-graceful disconnect.

   The above return values map directly to the return values used by Barracuda.
*/
#define HttpSocket_send(o, m, isTerminated, data, len, retLen) do { \
  if(m && ThreadMutex_isOwner(m)) { \
    ThreadMutex_release(m); \
    *(retLen)=socketSend((o)->hndl,(void*)(data),len,0); \
    ThreadMutex_set(m); \
  } \
  else \
    *(retLen)=socketSend((o)->hndl,(void*)(data),len,0); \
} while(0)


#define HttpSocket_setBlocking(o, status) do { \
   int disableFlag = 0; \
   *(status) = setsockopt( \
       (o)->hndl, SOL_TCP, OPT_RECEIVE_NOWAIT | OPT_SEND_NOWAIT, \
                          &disableFlag, sizeof(int)); \
} while(0)

#define HttpSocket_setNonblocking(o, status) \
do { \
   int enableFlag = 1; \
   *(status) = setsockopt( \
       (o)->hndl, SOL_TCP, OPT_RECEIVE_NOWAIT | OPT_SEND_NOWAIT, \
                          &enableFlag, sizeof(int)); \
} while(0)

#define HttpSocket_setTCPNoDelay(o, enable, status) do { \
   int enableFlag = enable ? 1 : 0; \
   *(status) = setsockopt((o)->hndl, SOL_TCP, OPT_NO_NAGLE_ALGORITHM, \
                          &enableFlag, sizeof(int)); \
} while(0)

/* Function wouldBlock is only used by the default HttpSocket_recv/send */
#define HttpSocket_wouldBlock(o, status) (*(status)=TRUE)

/* SO_REUSEADDR socket option is not available in RTCS 2.96 */
#define HttpSocket_soReuseaddr(o, status)

#define HttpSocket_hardClose(o) HttpSocket_close(o)

#if !RTCSCFG_ENABLE_DNS
#error Please recompile RTCS with RTCSCFG_ENABLE_DNS set to 1 in rtcscfg.h
#endif

/* Include the default HttpSocket functions */
#include <gBsdSock.h>


/***********************************************************************
 *  SoDisp object
 * Not using the default in gBsdDspO.h
 ***********************************************************************/

#define CONNECTION_DISPATCHER_OBJ DoubleLink dispatcherLink;

#include <DoubleList.h>
#define DISPATCHER_DATA \
  DoubleList conList; \
  DoubleList pendingList; \
  DoubleLink* curL; \
  DoubleListEnumerator iter; \
  SocketHandle* sockList; \
  int pollDelay; \
  int sockListLen

#endif
