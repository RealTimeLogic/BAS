/*
 *     ____             _________                __                _     
 *    / __ \___  ____ _/ /_  __(_)___ ___  ___  / /   ____  ____ _(_)____
 *   / /_/ / _ \/ __ `/ / / / / / __ `__ \/ _ \/ /   / __ \/ __ `/ / ___/
 *  / _, _/  __/ /_/ / / / / / / / / / / /  __/ /___/ /_/ / /_/ / / /__  
 * /_/ |_|\___/\__,_/_/ /_/ /_/_/ /_/ /_/\___/_____/\____/\__, /_/\___/  
 *                                                       /____/          
 *
 *                  Barracuda Embedded Web-Server 
 ****************************************************************************
 *            HEADER
 *
 *   $Id: HttpCmdThreadPoolIntf.h 5978 2026-09-11 16:13:48Z wini $
 *
 *   COPYRIGHT:  Real Time Logic LLC, 2005-2012
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
 */

/** @file HttpCmdThreadPoolIntf.h */
#ifndef _HttpCmdThreadPoolIntf_h
#define _HttpCmdThreadPoolIntf_h

/* This API provides late bindings between HttpServer and HttpCmdThreadPool.
 */
 
#include <HttpServer.h>

struct HttpCmdThreadPoolIntf;

/** Attempt to dispatch HTTP work to a worker while the server mutex is held.
    @param o Required initialized pool interface.
    @param cmd Borrowed live command whose processing is transferred on success.
    @param dir Borrowed directory from which service starts; retain it through work.
    @return 0 if accepted for worker execution; nonzero declines the transfer and
    lets HttpServer process it on the current thread. This is not a response or
    completion status. Implementations must preserve the server's command/mutex
    lifecycle. Supplied implementations are HttpCmdThreadPool and LThreadMgr.
 */
typedef int(*HttpCmdThreadPoolIntf_DoDir)(
   struct HttpCmdThreadPoolIntf* o,HttpCommand* cmd,
   HttpDir* dir);

/** Worker-pool dispatch interface borrowed by HttpServer. */
typedef struct HttpCmdThreadPoolIntf
{
   HttpCmdThreadPoolIntf_DoDir doDir; /**< Required dispatch callback. */
}  HttpCmdThreadPoolIntf;

/** @copydoc HttpCmdThreadPoolIntf_DoDir */
#define HttpCmdThreadPoolIntf_doDir(o, cmd, dir) \
   (o)->doDir(o, cmd, dir)

#endif
