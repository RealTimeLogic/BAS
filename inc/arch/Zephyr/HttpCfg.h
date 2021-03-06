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
 *   $Id: HttpCfg.h 5134 2022-04-27 22:40:57Z wini $
 *
 *   COPYRIGHT:  Real Time Logic, 2022
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
 *               https://realtimelogic.com
 ****************************************************************************
 *
 *
 *  Platform: Zephyr
 */
#ifndef _HttpConfig_h
#define _HttpConfig_h

#include <fcntl.h>
#include <net/socket.h>
#include <TargConfig.h>

/***********************************************************************
 *  The HttpSocket API
 ***********************************************************************/
#if defined(CONFIG_NET_IPV6) && ! defined(USE_IPV6)
#define USE_IPV6
#endif

#define SocketHandle int
#define socketAccept zsock_accept
#define socketBind zsock_bind
#define socketClose zsock_close
#define socketShutdown zsock_shutdown
#define socketErrno(s) errno
#define socketGetPeerName zsock_getpeername
#define socketGetSockName zsock_getsockname
#define socketListen zsock_listen
#define socketRecv zsock_recv
#define socketSelect zsock_select
#define socketSend zsock_send
#define socketSendto zsock_sendto
#define socketRecvfrom zsock_recvfrom
#define socketGetsockopt zsock_getsockopt
#define socketSetsockopt zsock_setsockopt
#define socketSocket zsock_socket
#define socketInetNtoa zsock_inet_ntoa
#define socketWouldBlock(s) (EWOULDBLOCK == socketErrno(s))
#define socketInProgress(s) (EINPROGRESS == socketErrno(s))
#define socketConnect zsock_connect


#define HttpSocket_setBlocking(o, status) do {                  \
      int flags=zsock_fcntl((o)->hndl, F_GETFL, 0);             \
      zsock_fcntl((o)->hndl,F_SETFL,flags | O_NONBLOCK);        \
      *(status)=0;                                              \
   } while(0)

#define HttpSocket_setNonblocking(o, status) do {               \
      int flags=zsock_fcntl((o)->hndl, F_GETFL, 0);             \
      zsock_fcntl((o)->hndl,F_SETFL,flags & ~O_NONBLOCK);       \
      *(status)=0;                                              \
   } while(0)

#define HttpSocket_soReuseaddr(o, status) do {                          \
      int enableFlag = 1;                                               \
      *(status) = zsock_setsockopt((o)->hndl, SOL_SOCKET, SO_REUSEADDR, \
                                   &enableFlag, sizeof(int));           \
   }while(0)

#define HttpSocket_hardClose HttpSocket_close

#define HttpSockaddr_gethostbyname HttpSockaddr_gethostbynameF
struct HttpSockaddr;
void HttpSockaddr_gethostbynameF(
   struct HttpSockaddr* o, const char* host, BaBool useIp6, int* status);

#include <TargConfig.h>
#include <NetConv.h>

/* Include the default HttpSocket functions */
#include <gBsdDspO.h>
#include <gBsdSock.h>

#define BA_FD_SET ZSOCK_FD_SET
#define BA_FD_ZERO ZSOCK_FD_ZERO
#define BA_FD_ISSET ZSOCK_FD_ISSET
#define baFdSet zsock_fd_set


#endif
