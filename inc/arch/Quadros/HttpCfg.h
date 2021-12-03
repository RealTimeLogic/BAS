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
 *   $Id: HttpCfg.h 4915 2021-12-01 18:26:55Z wini $
 *
 *   COPYRIGHT:  Real Time Logic, 2012 - 2018
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
 *  Treck TCP/IP
 */
#ifndef _HttpConfig_h
#define _HttpConfig_h

#ifndef USE_IPV6
#define USE_IPV6
#endif

 
#ifdef USE_IPV6
#ifndef TM_USE_IPV6
#define TM_USE_IPV6
#endif
#elif defined(TM_USE_IPV6)
#ifndef USE_IPV6
#define USE_IPV6
#endif
#endif

#include <TargConfig.h>
#include <trsocket.h>
#include <gBsdDspO.h>
#include <NetConv.h>

/***********************************************************************
 *  The HttpSocket API
 ***********************************************************************/

#define EWOULDBLOCK TM_EWOULDBLOCK
#define basocklen_t int

#define socketSend(s,buf,len,flags) send(s,(char*)buf,len,flags)

#define HttpSockaddr_gethostbyname(o, host, useIp6, status) do {\
   if(host) {\
      struct addrinfo hints;\
      struct addrinfo * retAddrInfoPtr;\
      memset(&hints,0,sizeof(hints)); \
      *(status)=0;\
      hints.ai_flags = AI_ALL;\
      hints.ai_family = useIp6 ? AF_INET6 : AF_INET;\
      if(getaddrinfo(host, 0, &hints, &retAddrInfoPtr) == TM_ENOERROR)\
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


#define socketClose(s) tfClose(s)

#define HttpSocket_setNonblocking(o, status) \
   *(status)=tfBlockingState((o)->hndl, TM_BLOCKING_OFF)

#define HttpSocket_setBlocking(o, status) \
   *(status)=tfBlockingState((o)->hndl, TM_BLOCKING_ON)

/* tfGetSocketError uses 's' as array index */
#define socketErrno(s) (s >= 0 ? tfGetSocketError(s) : -1)

#include <gBsdSock.h>

#endif
