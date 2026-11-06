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
 *   $Id: TargConfig.h 6760 2026-11-06 00:03:47Z wini $
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
 *
 */
#ifndef _TargConfig_h
#define _TargConfig_h

#include <BaConfigDefs.h>

#ifndef BA_INTERNAL_CONFIG_OPTS_LOADED
#define BA_INTERNAL_CONFIG_OPTS_LOADED 1
#include <BaConfigOpts.h>
#endif

#include <BaConfigPosix.h>
/* Select the allocator before any port hooks are installed. */
#ifdef __NuttX__
#ifndef BA_ALLOCATOR
#define BA_ALLOCATOR BA_ALLOCATOR_DLMALLOC
#endif
#endif
#ifndef BA_ALLOCATOR
#define BA_ALLOCATOR BA_ALLOCATOR_DEFAULT
#endif
#if BA_ALLOCATOR == BA_ALLOCATOR_MQX
#error BA_ALLOCATOR_MQX requires the MQX port
#endif


/* C runtime and Lua library capabilities for this port. */
#ifndef BA_HAVE_STDIO
#define BA_HAVE_STDIO 1
#endif
#ifndef BA_LUA_OS_LIBRARY_MODE
#define BA_LUA_OS_LIBRARY_MODE BA_LUA_OS_LIBRARY_FULL
#endif
#ifndef BA_ENABLE_LUA_STDIO
#define BA_ENABLE_LUA_STDIO 1
#endif



#ifdef __QNX__
/* Function dirent in xrc/DiskIo/posix/BaFile.c, struct member 
   ep->d_name fails if the following are not included here.
*/
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#endif

/*  The default choice for Posix - change by using another arch
 *  or set at compile time.
 */
/* BaConfigEndian.h resolves compiler byte order or validates an explicit selection. */

/***********************************************************************
 *  Misc settings
 ***********************************************************************/

#define BA_POSIX

#ifndef BA_ENABLE_SOCKET_REUSE_ADDRESS
#define BA_ENABLE_SOCKET_REUSE_ADDRESS 1
#endif

#ifdef __NuttX__
#pragma GCC diagnostic ignored "-Wshadow"
#pragma GCC diagnostic ignored "-Wundef"
#pragma GCC diagnostic ignored "-Wstrict-prototypes"
#else
/* https://realtimelogic.com/ba/doc/?url=auxlua.html#reverseproxy
 */
#ifndef BA_ENABLE_REVERSE_PROXY
#define BA_ENABLE_REVERSE_PROXY 1
#endif
#ifndef BA_ENABLE_UBJSON
#define BA_ENABLE_UBJSON 1
#endif
#ifndef BA_ENABLE_REVERSE_CONNECTION
#define BA_ENABLE_REVERSE_CONNECTION 1
#endif
#ifndef BA_ENABLE_LUA_PIPE
#define BA_ENABLE_LUA_PIPE 1
#endif
/* https://realtimelogic.com/ba/doc/?url=auxlua.html#forkptylib
 */
#ifndef BA_ENABLE_LUA_FORKPTY
#define BA_ENABLE_LUA_FORKPTY 1
#endif
#endif

#if defined(GNUC) || defined(__GNU__) || defined(__GNUC_MINOR__)
#ifndef __GNUC__
#define __GNUC__
#endif
#endif

#define HTTP_E_BASE 1 /* Error code base */
#ifndef SERVER_SOFTWARE_NAME
#define SERVER_SOFTWARE_NAME "BarracudaServer.com (Posix)"
#endif

#ifndef BA_API
#define BA_API
#endif

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


#include <BaConfig.h>

/* For SharkSSL */
#if BA_ARCH_64BIT
#define SHARKSSL_64BIT
#endif 


#include <BaErrorCodes.h>
#ifndef baAssert
#ifdef NDEBUG
#define baAssert(exp)
#else
#define baAssert(exp) ( (exp) ? (void)0 : baFatalEf(FE_ASSERT, 0, __FILE__, __LINE__) )
#endif
#endif


/***********************************************************************
 *  Memory allocation
 ***********************************************************************/
#include <stdlib.h>

#ifdef BA_LEAK_CHECK
#include <LeakDetect.h>
#else
#if BA_ALLOCATOR == BA_ALLOCATOR_DLMALLOC
#include <dlmalloc.h>
#else
#define baMalloc(size) malloc(size)
#define baRealloc(ptr,size) realloc(ptr,size)
#define baFree(ptr) free(ptr)
#endif
#endif

/***********************************************************************
 *  Barracuda types
 ***********************************************************************/
#include <GenPrimT.h>

/***********************************************************************
 *  Time functions
 ***********************************************************************/

#include <time.h>
#define baGetUnixTime() ((BaTime)time(0))
void _baClckGettime(BaTimeEx* spec);
#define baClckGettime _baClckGettime


#ifdef __cplusplus
extern "C" {
#endif
   BA_API unsigned int baGetMsClock(void);
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


#endif
