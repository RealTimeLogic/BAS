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
 *   $Id: HttpCfg.h 6188 2026-09-29 22:55:46Z wini $
 *
 *   COPYRIGHT:  Real Time Logic, 2007 - 2026
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
 *  VxWorks
 */
#ifndef _HttpConfig_h
#define _HttpConfig_h

#if _WRS_VXWORKS_MAJOR == 5
#include <net/inet.h>
#endif

#include <vxWorks.h> 
#include <sockLib.h>
#include <selectLib.h>
#include <hostLib.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <inetLib.h>
#include <net/if.h>  /* if_nametoindex */
#include <errno.h>
#include <fcntl.h>
#if _WRS_VXWORKS_MAJOR > 5
#include <netdb.h>
#endif


/* Include the line below if you get compile errors */
/* #include <net/inet.h> */



#include <TargConfig.h>
#include <gBsdDspO.h>
#include <NetConv.h>

#define USE_DGRAM
#if _WRS_VXWORKS_MAJOR > 5
#define USE_ADDRINFO
#endif


/***********************************************************************
 *  The HttpSocket API
 ***********************************************************************/

#define socketConnect _socketConnect
int _socketConnect(int s,  struct sockaddr* name,  int namelen);
#ifdef sodisp_c
int _socketConnect(int s,  struct sockaddr* name,  int namelen)
{
   struct timeval timeout;
   timeout.tv_sec  = 1;
   timeout.tv_usec = 0;
   return connectWithTimeout(s,name,namelen,&timeout);
}
#endif


/* Overload the following default functions */

#if _WRS_VXWORKS_MAJOR == 5
#define basocklen_t int

#define HttpSocket_shutdown(o) do {\
   int status;\
   HttpSocket_setBlocking(o,&status);\
   (void)status; \
   socketClose((o)->hndl);\
   HttpSocket_invalidate(o);\
 } while(0)

#endif

#define socketIoctl(a,b,c) ioctl(a,b,c)


#ifdef USE_IPV6
#define UseVxWorksGethostbyname
#define HttpSockaddr_gethostbyname HttpSockaddr_gethostbynameF
#else
#define HttpSockaddr_gethostbyname(o, host, useIp6, status)  do { \
   int ipAddr; \
   (o)->isIp6=FALSE; \
   *(status)=0; \
   if(host) \
   { \
      ipAddr = socketInetAddr((char*)host); /* is "host" an IP address ? */ \
      if((unsigned)ipAddr == INADDR_NONE) /* No, not an IP address. */ \
      { /* Is "host" a hostname ? */\
         ipAddr = hostGetByName((char*)host); \
         if(ipAddr == ERROR) \
            *(status)=-1; \
      } \
   } \
   else \
      ipAddr = baHtonl(INADDR_ANY); \
   memcpy((o)->addr,&ipAddr, 4); \
}while(0)
#endif



#if defined(FD_CLOEXEC) && defined(F_SETFD)
#define HttpSocket_setcloexec(o) (void)fcntl((o)->hndl, F_SETFD, FD_CLOEXEC)
#define HttpSocket_clearcloexec(o) (void)fcntl((o)->hndl, F_SETFD, 0)
#endif

/* VxWorks assigns different values to EAGAIN and EWOULDBLOCK. */
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
         if(e==EINTR)                                        \
            continue;                                                   \
         *(status) = e ? e : -1;                                        \
         break;                                                         \
      }                                                                 \
      else {                                                            \
         *(status)=0;                                                   \
         HttpSocket_setcloexec(conSock);                                \
         break;                                                         \
      }                                                                 \
   } while(1)

#define HttpSocket_recv(o, data, len, retLen) do { \
  *(retLen)=recv((o)->hndl,data,len,0); \
  if(*(retLen) == 0) {*(retLen) = -1;break;} /* graceful disconnect */ \
  if(*(retLen) < 0) { int e=errno; \
    if (e==EINTR) continue; \
    if (e==EAGAIN || e==EWOULDBLOCK) {*(retLen)=0;break;}  /* No data */ \
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
    if (e==EAGAIN || e==EWOULDBLOCK) {*(retLen)=0;}/* non blocking, no data sent */ \
  } \
  break; \
} while(1)

#endif /* defined EINTR EAGAIN */

#if !defined(NO_KEEPALIVEEX) && defined(TCP_KEEPIDLE) && defined(TCP_KEEPINTVL)
#define HttpSocket_getKeepAliveEx(o,enablePtr,timePtr,intervalPtr,statusPtr)\
do {\
   int _ZoptV=0,_Zidle=0,_Zintv=0;\
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

#ifdef UseVxWorksGethostbyname
void HttpSockaddr_gethostbynameF(
   HttpSockaddr* o, const char* host, BaBool useIp6, int* status);
#endif

#endif
