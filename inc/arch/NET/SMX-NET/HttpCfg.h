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
 *  SMX
 */
#ifndef _HttpConfig_h
#define _HttpConfig_h


#include <TargConfig.h>

#include <OSENV.H>
#undef INLINE_SUPPORT
#define INLINE_SUPPORT 0
#include <rtip.h>
#include <socket.h>

#include <stddef.h>

#define socketSend(socket, buffer, buf_len, flags)  \
    sendto((socket), (buffer), (buf_len), (flags), (PSOCKADDR)0, 0)

#include <gBsdDspO.h>
#include <NetConv.h>

#undef send


/***********************************************************************
 *  The HttpSocket API
 ***********************************************************************/

/* Overload the following default data and functions in gBsdSock.h */

#define socketErrno(s) xn_getlasterror()
#define socketIoctl ioctlsocket
#define socketClose closesocket

#define socketWouldBlock socketWouldBlockSMX
#ifdef __cplusplus
extern "C" {
#endif
int socketWouldBlockSMX(int s);
#ifdef __cplusplus
}
#endif

/* the following macro is used by HttpConnection_setTCPNoDelay */
#define TCP_NODELAY SO_NAGLE

/* gethostbyname takes a char* and not a const char* as argument */
#define socketGethostbyname(n) gethostbyname((char*)n)

#define HttpSocket_accept(o, conSock, status) do { \
   struct sockaddr_in whatever; \
   int sin_len = sizeof(whatever); \
   (conSock)->hndl=socketAccept((o)->hndl, (PSOCKADDR)&whatever, &sin_len); \
   *(status) = (HttpSocket_isValid(conSock)) ? 0 : -1; \
}while(0)



/* Include the default HttpSocket functions */
#include <gBsdSock.h>


/***********************************************************************
 *  Incompatebility settings and various tricks needed to get
 *  SMXNET to compile
 ***********************************************************************/

#ifndef COMPILING_THREADLIB
int  xn_getlasterror(void);
#endif

#define HttpConfig_SMX

#endif
