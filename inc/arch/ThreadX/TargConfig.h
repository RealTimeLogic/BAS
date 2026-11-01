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
 *   $Id: TargConfig.h 6686 2026-10-29 23:59:57Z wini $
 *
 *   COPYRIGHT:  Real Time Logic, 2004 - 2026
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
 * Platform: ThreadX.
 *
 */
#ifndef _TargConfig_h
#define _TargConfig_h

#include <BaConfigDefs.h>

#ifndef BA_INTERNAL_CONFIG_OPTS_LOADED
#define BA_INTERNAL_CONFIG_OPTS_LOADED 1
#include <BaConfigOpts.h>
#endif

/* Select the allocator before any port hooks are installed. */
#ifndef BA_ALLOCATOR
#define BA_ALLOCATOR BA_ALLOCATOR_DLMALLOC
#endif
#if BA_ALLOCATOR == BA_ALLOCATOR_MQX
#error BA_ALLOCATOR_MQX requires the MQX port
#endif


/* C runtime and Lua library capabilities for this port. */
#ifndef BA_HAVE_STDIO
#define BA_HAVE_STDIO 0
#endif
#ifndef BA_LUA_OS_LIBRARY_MODE
#define BA_LUA_OS_LIBRARY_MODE BA_LUA_OS_LIBRARY_MINIMAL
#endif
#ifndef BA_ENABLE_LUA_STDIO
#define BA_ENABLE_LUA_STDIO 0
#endif


#ifdef __cplusplus
#include <string.h>
#include <stdlib.h>
#endif

#include <tx_api.h>

/* Fetch endian from nx_port.h */
#include <nx_port.h>
#ifdef NX_LITTLE_ENDIAN
#ifndef BA_BYTE_ORDER
#define BA_BYTE_ORDER BA_BYTE_ORDER_LITTLE_ENDIAN
#elif BA_BYTE_ORDER != BA_BYTE_ORDER_LITTLE_ENDIAN
#error BA_BYTE_ORDER disagrees with NetX
#endif
#else
#ifndef BA_BYTE_ORDER
#define BA_BYTE_ORDER BA_BYTE_ORDER_BIG_ENDIAN
#elif BA_BYTE_ORDER != BA_BYTE_ORDER_BIG_ENDIAN
#error BA_BYTE_ORDER disagrees with NetX
#endif
#endif



#ifndef BA_API
#define BA_API
#endif

/***********************************************************************
 *  Barracuda types
 ***********************************************************************/


#ifndef _GenPrimT_h
#include <GenPrimT.h>
#endif

/***********************************************************************
 *  Memory allocation
 ***********************************************************************/
#include <stddef.h>

#ifdef BA_LEAK_CHECK
#include <LeakDetect.h>
#else
#include <dlmalloc.h>
#if BA_ALLOCATOR != BA_ALLOCATOR_DLMALLOC && (!defined(baMalloc) || !defined(baRealloc) || !defined(baFree))
#error This port requires BA_ALLOCATOR_DLMALLOC or all three custom allocation hooks
#endif
#endif



/***********************************************************************
 *  Barracuda time
 ***********************************************************************/

/* See ThreadLib.c */
#ifdef __cplusplus
extern "C" {
#endif
void baSetUnixTime(U32 time);
U32 baGetUnixTime(void);
U32 baGetMsClock(void);
U32 baMsTime2TxTicks(U32 msec);
#ifdef __cplusplus
}
#endif

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


/***********************************************************************
 *  Misc settings
 ***********************************************************************/

#ifndef SERVER_SOFTWARE_NAME
#define SERVER_SOFTWARE_NAME "BarracudaServer.com (ThreadX)"
#endif
#define HTTP_E_BASE 1 /* Error code base */


#if defined(SHARKSSL_LIB)
#if !defined(BA_LIB)
#define BA_LIB SHARKSSL_LIB
#endif
#endif


/* BAI_FUNC/DATA is a mark for all extern functions that are not to be
   exported to outside modules. Use when compiled as a shared library.
*/

#if defined(__GNUC__) && ((__GNUC__*100 + __GNUC_MINOR__) >= 302) && \
      defined(__ELF__)
#define BAI_FUNC	__attribute__((visibility("hidden"))) extern
#define BAI_DATA	BAI_FUNC

#else
#define BAI_FUNC	extern
#define BAI_DATA	extern
#endif

#ifndef SHARKSSL_API
#define SHARKSSL_API BA_API
#endif


#include <BaErrorCodes.h>
#include <BaConfig.h>

#ifdef NDEBUG
#define baAssert(exp)
#else
#define baAssert(exp) ( (exp) ? (void)0 : baFatalEf(FE_ASSERT, 0, __FILE__, __LINE__) )
#endif

#ifdef NDEBUG
#define TST_OK(x) x
#else
#define TST_OK(x) baAssert(0 == x)
#endif

#endif
