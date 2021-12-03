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
 *   COPYRIGHT:  Real Time Logic, 2002 - 2017
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

#include <windows.h>
#include <winsock.h>
#ifdef USE_IPV6
#include <WS2tcpip.h>
#endif
#include <TargConfig.h>
#include <gBsdDspO.h>
#include <NetConv.h>

/* Settings for Lua: lstate.h: Atomic type
 */
#define l_signalT int


/***********************************************************************
 *  The HttpSocket API
 ***********************************************************************/

/* Overload the following default functions */
#define socketErrno(s) WSAGetLastError()
#define socketWouldBlock(s) (WSAEWOULDBLOCK == WSAGetLastError())
#define socketClose closesocket
#define socketIoctl ioctlsocket
#define HttpSocket_soReuseaddr(o,status)

/* Include the default HttpSocket functions */
#include <gBsdSock.h>


#endif
