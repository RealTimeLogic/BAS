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
 *   COPYRIGHT:  Real Time Logic, 2002 - 2020
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
 *  Windows
 */
#ifndef _HttpConfig_h
#define _HttpConfig_h

#ifndef USE_IPV6
#define USE_IPV6
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif

#include <TargConfig.h>
#ifdef USE_IPV6
#ifndef NO_KEEPALIVEEX
#include <Mstcpip.h>
#endif
#include <WS2tcpip.h>
#define USE_ADDRINFO
#else
#ifndef NO_KEEPALIVEEX
#define NO_KEEPALIVEEX
#endif
#endif
#include <gBsdDspO.h>
#include <NetConv.h>

#define USE_DGRAM

#ifdef USE_DGRAM
/* No good implementation on Windows */
#define ba_nametoindex(name, resp,status) *(resp)=0
#endif


/***********************************************************************
 *  The HttpSocket API
 ***********************************************************************/

/* Overload the following default functions */
#define socketErrno(s) WSAGetLastError()
#define socketWouldBlock(s) (WSAEWOULDBLOCK == WSAGetLastError())
#define socketClose closesocket
#define socketIoctl ioctlsocket
/* #define HttpSocket_soReuseaddr(o,status) */

#if _M_X64 == 100
#define socketAccept (int)accept
#define socketSocket (int)socket
#define socketConnect(s, addr, addrlen) connect(s, addr, (int)addrlen)
#endif


#ifdef USE_IPV6
#define HttpSocket_getPeerName(o, addrMA, port, isIp6M, status) do {\
   struct sockaddr_storage winaddr;\
   int addrLen = sizeof (winaddr);\
   if( !(*(status) = getpeername((o)->hndl, (LPSOCKADDR)&winaddr, &addrLen)) )\
   {\
      if(isIp6M){\
         memcpy(&(addrMA)->addr, &((struct sockaddr_in6*)&winaddr)->sin6_addr, 16);\
         if(port) *(port)=baHtons(((struct sockaddr_in6*)&winaddr)->sin6_port);\
	  }\
      else {\
         *((U32*)(addrMA)->addr) = ((struct sockaddr_in*)&winaddr)->sin_addr.s_addr;\
         if(port) *(port)=baHtons(((struct sockaddr_in*)&winaddr)->sin_port);\
	  }\
      (addrMA)->isIp6=isIp6M; \
   }\
} while(0)


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


#ifndef NO_KEEPALIVEEX
#define HttpSocket_setKeepAliveEx(o,enable,time,interval,statusPtr) do { \
    struct tcp_keepalive x; \
    DWORD notUsed=0; \
    x.onoff = enable; \
    x.keepalivetime = time*1000; \
    x.keepaliveinterval = interval*1000; \
    *(statusPtr) = \
       WSAIoctl((o)->hndl,SIO_KEEPALIVE_VALS,&x,sizeof(x),0,0,&notUsed,0,0) ? \
       -1 : 0; \
} while(0)
#endif

#define baIoctlArg u_long

/* Include the default HttpSocket functions */
#include <gBsdSock.h>

#endif
