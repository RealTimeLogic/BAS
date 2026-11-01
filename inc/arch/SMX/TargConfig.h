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
 *   COPYRIGHT:  Real Time Logic, 2023 - 2026
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
#define BA_ALLOCATOR BA_ALLOCATOR_DEFAULT
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


/***********************************************************************
 *  Misc settings
 ***********************************************************************/

#ifndef BA_API
#define BA_API
#endif
#define BAI_FUNC extern
#define BAI_DATA extern

#ifndef SHARKSSL_API
#define SHARKSSL_API BA_API
#endif

#include <smx.h>


#define HTTP_E_BASE 1 /* Error code base */
#ifndef SERVER_SOFTWARE_NAME
#define SERVER_SOFTWARE_NAME "BarracudaServer.com (SMX)"
#endif


#ifndef SB_CPU_LITTLE_ENDIAN
#error Missing SMX byte-order declaration
#elif SB_CPU_LITTLE_ENDIAN == 1
#ifndef BA_BYTE_ORDER
#define BA_BYTE_ORDER BA_BYTE_ORDER_LITTLE_ENDIAN
#elif BA_BYTE_ORDER != BA_BYTE_ORDER_LITTLE_ENDIAN
#error BA_BYTE_ORDER disagrees with SMX
#endif
#elif SB_CPU_LITTLE_ENDIAN == 0
#ifndef BA_BYTE_ORDER
#define BA_BYTE_ORDER BA_BYTE_ORDER_BIG_ENDIAN
#elif BA_BYTE_ORDER != BA_BYTE_ORDER_BIG_ENDIAN
#error BA_BYTE_ORDER disagrees with SMX
#endif
#else
#error Unsupported SMX byte order
#endif

#include <BaErrorCodes.h>
#include <BaConfig.h>

#ifdef NDEBUG
#define baAssert(exp)
#else
#define baAssert(exp) ( (exp) ? (void)0 : baFatalEf(FE_ASSERT, 0, __FILE__, __LINE__) )
#endif


/***********************************************************************
 *  Barracuda types
 ***********************************************************************/
#include <GenPrimT.h>


/***********************************************************************
 *  Memory allocation
 ***********************************************************************/

#if BA_ALLOCATOR == BA_ALLOCATOR_DLMALLOC
#include <dlmalloc.h>
#else
#define baRealloc(ptr, size) smx_HeapRealloc(ptr, size)
#define baMalloc(size) smx_HeapMalloc(size)
#define baFree(ptr) smx_HeapFree(ptr)
#endif

/***********************************************************************
 *  Time functions
 ***********************************************************************/

#ifdef __cplusplus
extern "C" {
#endif
void baSetUnixTime(u32 time);
u32 baGetUnixTime(void);
#ifdef __cplusplus
}
#endif

#define baGetMsClock() sb_OS_MSEC_GET()

 
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
