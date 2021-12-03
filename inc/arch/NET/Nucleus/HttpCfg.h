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
 *   COPYRIGHT:  Real Time Logic, 2014
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
 *               http://realtimelogic.com
 ****************************************************************************
 *
 *
 *  Nucleus
 */
#ifndef _HttpConfig_h
#define _HttpConfig_h

#include <networking/nu_networking.h>

#include <TargConfig.h>
#include <gBsdDspO.h>
#include <NetConv.h>

#define SocketHandle INT
#define socketClose NU_Close_Socket
#define socketListen NU_Listen

#define HttpSocket_accept(o, conSock, status) do {                      \
      struct addr_struct peer;                                          \
      if( ((conSock)->hndl=NU_Accept((o)->hndl, &peer, 0)) >= 0)        \
         *(status) = 0;                                                 \
      else                                                              \
         *(status) = (conSock)->hndl;                                   \
   }while(0)

#define HttpSocket_bind(o, sa, portMA, status) do {                     \
   struct addr_struct  servaddr;                                        \
   servaddr.family = (sa)->isIp6 ? NU_FAMILY_IP6 : NU_FAMILY_IP;        \
   servaddr.port = portMA;                                              \
   memcpy(servaddr.id.is_ip_addrs,(sa)->addr,(sa)->isIp6 ? 16 : 4);     \
   } while(0)

#define HttpSocket_hardClose(o) NU_Abort((o)->hndl),HttpSocket_invalidate(o)

#define HttpSocket_shutdown HttpSocket_close

#define HttpSocket_errno(o, errorno, ecode) *(ecode) = errorno

#define HttpSocket_getPeerName(o, sa, port, setIsIp6, status) do { \
   struct sockaddr_struct peer;                                        \
   INT16 len; \
   if( (*(status) = NU_Get_Peer_Name((o)->hndl, &peer, &len)) == NU_SUCCESS) { \
      memcpy((sa)->addr, &peer.ip_num.is_ip_addrs, len);          \
      (sa)->isIp6 = peer.family == NU_FAMILY_IP6;                 \
      if((sa)->isIp6 != setIsIp6) *(status) = -1;                       \
   }                                                                    \
   }while(0)

#define HttpSocket_getSockName(o, sa, port, setIsIp6, status) do { \
   struct sockaddr_struct peer;                                        \
   INT16 len; \
   if( (*(status) = NU_Get_Sock_Name((o)->hndl, &peer, &len)) == NU_SUCCESS) { \
      memcpy((sa)->addr, &peer.ip_num.is_ip_addrs, len);          \
      (sa)->isIp6 = peer.family == NU_FAMILY_IP6;                 \
      if((sa)->isIp6 != setIsIp6) *(status) = -1;                       \
   }                                                                    \
} while(0)


#define HttpSocket_recv(o, data, len, retLen) do {                      \
   *(retLen)=NU_Recv((o)->hndl,data,(UINT16)len,0);                     \
   if( *(retLen) == NU_WOULD_BLOCK ) *(retLen)=0;  /* No data */        \
} while(0)


#define HttpSocket_send(o, m, isTerminated, data, len, retLen) do {     \
      if(m && ThreadMutex_isOwner(m)) {                                 \
         ThreadMutex_release(m);                                        \
         *(retLen)=NU_Send((o)->hndl,(CHAR*)data,(UINT16)len,0);        \
         ThreadMutex_set(m);                                            \
      }                                                                 \
      else                                                              \
         *(retLen)=NU_Send((o)->hndl,(CHAR*)data,(UINT16)len,0);        \
      if( *(retLen) == NU_WOULD_BLOCK ) *(retLen)=0;  /*No data sent*/  \
} while(0)



#define HttpSocket_setBlocking(o, status)               \
   *(status)=NU_Fcntl((o)->hndl,NU_SETFLAG,NU_BLOCK)

#define HttpSocket_setNonblocking(o, status)            \
   *(status)=NU_Fcntl((o)->hndl,NU_SETFLAG,NU_NO_BLOCK)

#define HttpSocket_wouldBlock(o, status)  \
   *(status) = *(status) == NU_WOULD_BLOCK

#define HttpSocket_setTCPNoDelay(o, enable, status)             \
   *(status)=NU_Setsockopt_TCP_NODELAY((o)->hndl, enable ? 1 : 0)

                          
#define HttpSocket_setKeepAlive(o,enable,status)                        \
   *(status)=NU_Setsockopt_SO_KEEPALIVE((o)->hndl, enable ? 1 : 0)


#define HttpSocket_soReuseaddr(o, status)               \
   *(status)=NU_Setsockopt_SO_REUSEADDR((o)->hndl, 1)


#define HttpSocket_sockStream(o, na, useIp6, status) do {               \
   (o)->hndl=NU_Socket(useIp6 ? NU_FAMILY_IP6:NU_FAMILY_IP,NU_TYPE_STREAM,0); \
   *(status)= (o)->hndl < 0 ? (o)->hndl : 0;                            \
} while(0)


#define HttpSocket_connect(o, sa, portMA, status) do {                  \
   struct addr_struct  servaddr;                                        \
   servaddr.family = (sa)->isIp6 ? NU_FAMILY_IP6 : NU_FAMILY_IP;        \
   servaddr.port = portMA;                                              \
   memcpy(servaddr.id.is_ip_addrs,(sa)->addr,(sa)->isIp6 ? 16 : 4);     \
   *(status)=NU_Connect((o)->hndl, &servaddr, 0);                        \
} while(0)

#define HttpSockaddr_gethostbyname
#include <gBsdSock.h>
#undef HttpSockaddr_gethostbyname

void HttpSockaddr_gethostbyname(
   HttpSockaddr* o, const char* host, BaBool ip6, int* status);



#endif
