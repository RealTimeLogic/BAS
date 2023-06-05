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
 *   $Id: mako.h 4914 2021-12-01 18:24:30Z wini $
 *
 *   COPYRIGHT:  Real Time Logic, 2012 - 2020
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
 */

#include <stdio.h>

#include <barracuda.h>

#define APPNAME "mako"


void makoprintf(int isErr,const char* fmt, ...);
void makovprintf(int isErr,const char* fmt, va_list argList);
void setDispExit(void);
void sendFatalError(const char* eMsg,
                    BaFatalErrorCodes ecode1,
                    unsigned int ecode2,
                    const char* file,
                    int line);
void errQuit(const char* fmt, ...);
  

#ifdef _WIN32
#define SERVICENAME "mako"
void runMako(int isWinService,int argc, char* argv[], char* envp[]);
void mustBeAdmin(void);
void instalMakoEventSource(void);
int prtok(int ok, const char* okmsg);
#endif
