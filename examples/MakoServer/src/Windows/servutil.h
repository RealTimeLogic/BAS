/*
 *     ____             _________                __                _     
 *    / __ \___  ____ _/ /_  __(_)___ ___  ___  / /   ____  ____ _(_)____
 *   / /_/ / _ \/ __ `/ / / / / / __ `__ \/ _ \/ /   / __ \/ __ `/ / ___/
 *  / _, _/  __/ /_/ / / / / / / / / / / /  __/ /___/ /_/ / /_/ / / /__  
 * /_/ |_|\___/\__,_/_/ /_/ /_/_/ /_/ /_/\___/_____/\____/\__, /_/\___/  
 *                                                       /____/          
 *
 *
 ****************************************************************************
 *			      HEADER
 *
 *   $Id: servutil.h 4914 2021-12-01 18:24:30Z wini $
 *
 *   COPYRIGHT:  Real Time Logic, 2012
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
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#ifndef _CRT_NONSTDC_NO_WARNING
#define _CRT_NONSTDC_NO_WARNING
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif
#ifndef STRICT
#define STRICT 1
#endif
#include <windows.h>
#include <stdlib.h>


void servicePrintf(const char* fmt, ...);
void serviceErrPrintf(const char* fmt, ...);
void serviceVPrintf(int isErr, const char* fmt, va_list argList);

int serviceStart(const char* servname, int argc, const char ** argv);
int serviceStop(const char* servname);
int servicePause(const char* servname);
int serviceContinue(const char* servname);
int serviceUsercode(const char* servname, int action);
int serviceState(const char* servname);
const char* serviceStateDesc(int state);
int serviceGetStartup(const char* servname);
int serviceSetStartup(const char* servname, int startType);
int serviceInstall(
   const char* servicename, int autostart, int argc, const char ** argv);
int serviceRemove(const char* servicename);

