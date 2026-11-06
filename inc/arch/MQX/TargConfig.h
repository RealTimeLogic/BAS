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

/* Select the allocator before any port hooks are installed. */
#ifndef BA_ALLOCATOR
#define BA_ALLOCATOR BA_ALLOCATOR_DLMALLOC
#endif
#if BA_ALLOCATOR != BA_ALLOCATOR_DLMALLOC && BA_ALLOCATOR != BA_ALLOCATOR_MQX
#error The MQX port requires its DLMALLOC or MQX allocator
#endif


/* C runtime and Lua library capabilities for this port. */
#ifndef BA_USE_C_RUNTIME_TIME
#define BA_USE_C_RUNTIME_TIME 0
#endif
#ifndef BA_HAVE_STDIO
#define BA_HAVE_STDIO 0
#endif
#ifndef BA_LUA_OS_LIBRARY_MODE
#define BA_LUA_OS_LIBRARY_MODE BA_LUA_OS_LIBRARY_MINIMAL
#endif
#ifndef BA_ENABLE_LUA_STDIO
#define BA_ENABLE_LUA_STDIO 0
#endif


#include <mqx.h>


/***********************************************************************
 *  Misc settings
 ***********************************************************************/

#define HTTP_E_BASE 1 /* Error code base */
#ifndef SERVER_SOFTWARE_NAME
#define SERVER_SOFTWARE_NAME "BarracudaServer.com (MQX)"
#endif

/* not used */
#ifndef BA_API
#define BA_API
#define SHARKSSL_API
#endif
#define BAI_FUNC

#if !defined(PSP_ENDIAN) || !defined(MQX_BIG_ENDIAN) || !defined(MQX_LITTLE_ENDIAN)
#error Missing MQX byte-order declaration
#elif PSP_ENDIAN == MQX_BIG_ENDIAN
#ifndef BA_BYTE_ORDER
#define BA_BYTE_ORDER BA_BYTE_ORDER_BIG_ENDIAN
#elif BA_BYTE_ORDER != BA_BYTE_ORDER_BIG_ENDIAN
#error BA_BYTE_ORDER disagrees with MQX
#endif
#elif PSP_ENDIAN == MQX_LITTLE_ENDIAN
#ifndef BA_BYTE_ORDER
#define BA_BYTE_ORDER BA_BYTE_ORDER_LITTLE_ENDIAN
#elif BA_BYTE_ORDER != BA_BYTE_ORDER_LITTLE_ENDIAN
#error BA_BYTE_ORDER disagrees with MQX
#endif
#else
#error Unsupported MQX byte order
#endif

#if defined(SHARKSSL_LIB)
#if !defined(BA_LIB)
#define BA_LIB SHARKSSL_LIB
#endif
#endif


#include <BaConfig.h>

#include <BaErrorCodes.h>
#ifdef NDEBUG
#define baAssert(exp)
#else
#define baAssert(exp) \
         ( (exp) ? (void)0 : baFatalEf(FE_ASSERT, 0, __FILE__, __LINE__) )
#endif

/***********************************************************************
 *  Barracuda types
 ***********************************************************************/

#include <GenPrimT.h>


/***********************************************************************
 *  Memory allocation
 ***********************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#if BA_ALLOCATOR == BA_ALLOCATOR_MQX
/* Note: Do not use when using the Lua plugin */
#define baMalloc(size) _mem_alloc_system(size)
void* baRealloc(void* ptr, unsigned long size);
#define baFree(ptr) _mem_free(ptr)
#else
#include <stddef.h>
#include <dlmalloc.h>
#endif

#ifdef __cplusplus
}
#endif 


/***********************************************************************
 *  Time functions
 ***********************************************************************/


/* See ThreadLib.c */
#ifdef __cplusplus
extern "C" {
#endif
BaTime baGetUnixTime(void);
U32 baGetMsClock(void);
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
