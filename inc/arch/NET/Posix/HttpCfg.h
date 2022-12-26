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
 *   $Id: HttpCfg.h 5364 2022-12-23 17:20:13Z wini $
 *
 *   COPYRIGHT:  Real Time Logic, 2002 - 2022
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
 *  Posix
 */
#ifndef _HttpConfig_h
#define _HttpConfig_h

#include <sys/types.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <errno.h>

#include <TargConfig.h>
#include <gBsdDspO.h>
#include <NetConv.h>

#define USE_ADDRINFO
#define USE_DGRAM

#ifdef USE_DGRAM
#include <net/if.h>  /* if_nametoindex */
#endif

/***********************************************************************
 *  The HttpSocket API
 ***********************************************************************/

#ifdef __NuttX__

#ifndef CONFIG_LIBC_NETDB
#error CONFIG_LIBC_NETDB must be defined
#endif
#define HttpSocket_shutdown(o) HttpSocket_close(o)
#define HttpSocket_hardClose(o) HttpSocket_close(o)
#define HttpSocket_soReuseaddr(o, status) *(status)=0
#define HttpSocket_soDontroute(o, enableFlag, status) (*(status))=-1
#define HttpSocket_soBroadcast(o, enableFlag, status) (*(status))=-1
//#define HttpSocket_setKeepAlive(o,enable,status) (*(status))=-1
#undef SO_KEEPALIVE
#define HttpSocket_setmembership(o,enable,ipv6,multiAddrName,intfName) -1
#define HttpSocket_setTCPNoDelay(o, enable, status) (*(status)=0)
#define NO_KEEPALIVEEX

#define HttpSockaddr_gethostbyname(o, host, useIp6, status) do {\
   *(status)=0;\
   if(host) {\
      struct addrinfo hints;\
      struct addrinfo * retAddrInfoPtr;\
      memset(&hints,0,sizeof(hints)); \
      hints.ai_flags = AI_ALL;\
      hints.ai_family = useIp6 ? AF_INET6 : AF_INET;\
      if(!getaddrinfo(host, 0, &hints, &retAddrInfoPtr))\
      {\
         struct addrinfo * addrInfoPtr;\
         addrInfoPtr = retAddrInfoPtr;\
         if (addrInfoPtr->ai_family == AF_INET && !useIp6)\
         {\
            struct sockaddr_in * tempSockInPtr =\
               (struct sockaddr_in*)addrInfoPtr->ai_addr;\
            *((U32*)(o)->addr) = tempSockInPtr->sin_addr.s_addr;\
            (o)->isIp6=FALSE;\
         }\
         else if(useIp6)\
         {\
            struct sockaddr_in6 * tempSockIn6Ptr =\
               (struct sockaddr_in6*)addrInfoPtr->ai_addr;\
            memcpy((o)->addr, &tempSockIn6Ptr->sin6_addr, 16);\
            (o)->isIp6=TRUE;\
         }\
         else\
            *(status)=-1;\
      }\
      else\
         *(status)=-1;\
   }\
   else if(useIp6) {\
      memset((o)->addr, 0, 16);\
      (o)->isIp6=TRUE;\
   }\
   else {\
      unsigned long ipAddr;\
      ipAddr = baHtonl(INADDR_ANY);\
      *((U32*)(o)->addr) = (U32)ipAddr;\
      (o)->isIp6=FALSE;\
   }\
} while(0)
#endif


#if defined(FD_CLOEXEC)

#undef HttpSocket_setcloexec
#undef HttpSocket_clearcloexec
#undef basocketpair

#define HttpSocket_setcloexec(o) (void)fcntl((o)->hndl, F_SETFD, FD_CLOEXEC)

#define HttpSocket_clearcloexec(o)  (void)fcntl((o)->hndl, F_SETFD, 0)

#define basocketpair(fd) socketpair(AF_UNIX,SOCK_STREAM,0,fd)

#else
#define HttpSocket_setcloexec(x)
#define HttpSocket_clearcloexec(x)
#define basocketpair(x)
#endif /* CLOEXEC */


#if defined(EINTR) && defined(EAGAIN)
 /* avoid unused macro */
#undef socketAccept
#define socketAccept
#undef socketSend
#define socketSend

#define HttpSocket_accept(o, conSock, status) do {                      \
      int e;                                                            \
      (conSock)->hndl=accept((o)->hndl, NULL, NULL);                    \
      if((conSock)->hndl < 0) {                                         \
         e=errno;                                                       \
         if(e==EINTR||e==EAGAIN)                                        \
            continue;                                                   \
         *(status) = e ? e : -1;                                        \
         break;                                                         \
      }                                                                 \
      else {                                                            \
         *(status)=0;                                                   \
         HttpSocket_setcloexec(o);                                      \
         break;                                                         \
      }                                                                 \
   } while(1)

#define HttpSocket_recv(o, data, len, retLen) do { \
  *(retLen)=recv((o)->hndl,data,len,0); \
  if(*(retLen) == 0) {*(retLen) = -1;break;} /* graceful disconnect */ \
  if(*(retLen) < 0) { int e=errno; \
    if (e==EINTR) continue; \
    if (e==EAGAIN) {*(retLen)=0;break;}  /* No data */ \
  } \
  break; \
} while(1)

#define HttpSocket_send(o, m, isTerminated, data, len, retLen) do { \
  if(m && ThreadMutex_isOwner(m)) { \
    ThreadMutex_release(m); \
    *(retLen)=send((o)->hndl,data,len,0); \
    ThreadMutex_set(m); \
  } \
  else \
    *(retLen)=send((o)->hndl,data,len,0); \
  if(*(retLen) < 0) { \
    int e=errno; \
    if (e==EINTR) continue; \
    if (e==EAGAIN) {*(retLen)=0;}/* non blocking, no data sent */ \
  } \
  break; \
} while(1)

#endif /* defined EINTR EAGAIN */

#if !defined(NO_KEEPALIVEEX) && defined(TCP_KEEPIDLE)
#define HttpSocket_getKeepAliveEx(o,enablePtr,timePtr,intervalPtr,statusPtr)\
do {\
   int _ZoptV,_Zidle,_Zintv;\
   int _Zs = (o)->hndl;\
   socklen_t _Zol = sizeof(_ZoptV);\
   *(statusPtr) =\
      getsockopt(_Zs, SOL_SOCKET, SO_KEEPALIVE, (char*)&_ZoptV, &_Zol) ||\
      getsockopt(_Zs, IPPROTO_TCP, TCP_KEEPIDLE, (char*)&_Zidle, &_Zol) ||\
      getsockopt(_Zs, IPPROTO_TCP, TCP_KEEPINTVL, (char*)&_Zintv, &_Zol) ?\
      -1 : 0;\
   *(enablePtr)=_ZoptV;\
   *(timePtr)=_Zidle;\
   *(intervalPtr)=_Zintv;\
} while(0)

#define HttpSocket_setKeepAliveEx(o,enable,time,interval,statusPtr) do {\
   int _Zs = (o)->hndl;\
   int _ZoptV=enable;\
   socklen_t _Zol = sizeof(_ZoptV);\
   if( ! setsockopt(_Zs, SOL_SOCKET, SO_KEEPALIVE, (char*)&_ZoptV, _Zol) ) {\
      if(_ZoptV && time && interval) {\
         int _Zidle=time;\
         int _Zintv=interval;\
         *(statusPtr) =\
            setsockopt(_Zs, IPPROTO_TCP, TCP_KEEPIDLE, (char*)&_Zidle, _Zol) ||\
            setsockopt(_Zs, IPPROTO_TCP, TCP_KEEPINTVL, (char*)&_Zintv, _Zol) ?\
            -1 : 0;\
      }\
      else\
         *(statusPtr)=0;\
   }\
   else\
      *(statusPtr)=1;\
} while(0)
#endif

/* Include the default HttpSocket functions */
#include <gBsdSock.h>


#endif
