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
 *   $Id: HttpCmdThreadPoolIntf.h 4915 2021-12-01 18:26:55Z wini $
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

#ifndef _HttpCmdThreadPoolIntf_h
#define _HttpCmdThreadPoolIntf_h

/* Provides late binding between HttpServer and HttpCmdThreadPool
 */
 
#include <HttpServer.h>

struct HttpCmdThreadPoolIntf;

typedef int(*HttpCmdThreadPoolIntf_DoPage)(
   struct HttpCmdThreadPoolIntf* o, HttpCommand* cmd,HttpPage* page);
typedef int(*HttpCmdThreadPoolIntf_DoDir)(
   struct HttpCmdThreadPoolIntf* o,HttpCommand* cmd,
   HttpDir* dir);

typedef struct HttpCmdThreadPoolIntf
{
      HttpCmdThreadPoolIntf_DoPage doPage;
      HttpCmdThreadPoolIntf_DoDir doDir;
}  HttpCmdThreadPoolIntf;

#define HttpCmdThreadPoolIntf_doDir(o, cmd, dir) \
   (o)->doDir(o, cmd, dir)
#define HttpCmdThreadPoolIntf_doPage(o, cmd, page) (o)->doPage(o, cmd, page)

#endif
