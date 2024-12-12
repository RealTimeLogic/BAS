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
 *   $Id: TargConfig.h 5594 2024-12-12 16:18:11Z wini $
 *
 *   COPYRIGHT:  Real Time Logic, 2023
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
#ifndef _TargConfig_h
#define _TargConfig_h

/***********************************************************************
 *  Misc settings
 ***********************************************************************/

#define HTTP_E_BASE 1 /* Error code base */
#ifndef SERVER_SOFTWARE_NAME
#define SERVER_SOFTWARE_NAME "BarracudaServer.com (Quadros)"
#endif

#ifndef BA_API
#define BA_API
#endif

#ifndef SHARKSSL_API
#define SHARKSSL_API BA_API
#endif

#include <BaErrorCodes.h>
#include <BaSharkSslOpt.h>

#ifdef NDEBUG
#define baAssert(exp)
#else
#define baAssert(exp) ( (exp) ? (void)0 : baFatalEf(FE_ASSERT, 0, __FILE__, __LINE__) )
#endif

#if defined(SHARKSSL_LIB)
#if !defined(BA_LIB)
#define BA_LIB SHARKSSL_LIB
#endif
#endif

#if defined(BALUA_LIB)
#if !defined(BA_LIB)
#define BA_LIB BALUA_LIB
#endif
#endif



#include <stddef.h>
#include <errno.h>

/***********************************************************************
 *  Memory allocation
 ***********************************************************************/
#ifdef USE_DLMALLOC
#include <dlmalloc.h>
#else
#define baMalloc(size) malloc(size)
#define baRealloc(ptr, size) realloc(ptr, size);
#define baFree(ptr) free(ptr)
#endif



/***********************************************************************
 *  Time functions
 ***********************************************************************/

#ifdef __cplusplus
extern "C" {
#endif
/* Must be provided by the application */
unsigned long baGetUnixTime(void);
#ifdef __cplusplus
}
#endif

#include "rtxcapi.h" /* RTXC Kernel Service prototypes */
#include "kcounter.h" /* COUNTER1 */

#define baGetMsClock() ((U32)(KS_GetCounterAcc(TIMEBASE) * CLKTICK))


/***********************************************************************
 *  All string manipulation functions in Barracuda
 ***********************************************************************/
#define bStrncmp strncmp
#define bStrchr strchr
#define bStrstr strstr
#define bStrrchr strrchr
#define bStrncpy strncpy
#define bAtoi atoi
#define bTolower tolower
#define bIsspace isspace
#define bIsxdigit isxdigit

#include <GenPrimT.h>

#endif

