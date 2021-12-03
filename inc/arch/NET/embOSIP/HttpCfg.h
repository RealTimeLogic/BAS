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
 *   $Id: HttpCfg.h 245 2006-10-16 14:07:04Z wini $
 *
 *   COPYRIGHT:  Real Time Logic, 2002 - 2004
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
 *  embOS/IP
 */
#ifndef _HttpConfig_h
#define _HttpConfig_h

#include <IP.h>
#include <TargConfig.h>
#include <gBsdDspO.h>
#include <NetConv.h>

/* Global embOS/IP macros that conflict with the C++ wrappers */
#ifdef __cplusplus
#undef send
#endif


/* Not implemented due to limitations in the TCP/IP stack
 */
#define NO_ASYNCH_RESP


/***********************************************************************
 *  The HttpSocket API
 ***********************************************************************/
#define SocketHandle SOCKTYPE 

typedef IP_fd_set fd_set;
#define FD_ZERO(set)      IP_FD_ZERO(set)
#define FD_ISSET(so,set)  IP_FD_ISSET(so,set)
#define FD_SET(so,set)    IP_FD_SET(so,set)
#define FD_CLR(so,set)    IP_FD_CLR(so,set)

#define socketSend(s,d,l,f)  send(s,(const char*)d,l,f)
#define socketErrno(s)       t_errno(s)   
#define socketClose          closesocket
#define HttpSocket_soReuseaddr(o,status)

#define HttpSocket_setBlocking(o, status) do { \
   int arg=0; \
  *(status)=setsockopt((o)->hndl, SOL_SOCKET, SO_NONBLOCK, &arg, sizeof(arg)); \
  (void)status; \
} while(0)

#define HttpSocket_setNonblocking(o, status) do { \
   int arg=1; \
  *(status)=setsockopt((o)->hndl, SOL_SOCKET, SO_NONBLOCK, &arg, sizeof(arg)); \
  (void)status; \
} while(0)

#define HttpSocket_wouldBlock(o, status) do { \
   int opt; \
   getsockopt((o)->hndl, SOL_SOCKET, SO_ERROR, &opt, sizeof(opt)); \
   *(status) = (opt == EWOULDBLOCK) ? TRUE : FALSE; \
} while(0)


#define HttpSockaddr_gethostbyname(o, host, useIp6, status) _HttpSockaddr_gethostbyname(o, host, useIp6, status)
#define HttpSockaddr_gethostbynameIp4(o, host, status) _HttpSockaddr_gethostbyname(o, host, FALSE, status)


/* Include the default HttpSocket functions */
#include <gBsdSock.h>


#ifdef __cplusplus
extern "C" {
#endif
void _HttpSockaddr_gethostbyname(HttpSockaddr *o, const char *host, BaBool useIp6, int *status);
#ifdef __cplusplus
}
#endif

#endif
