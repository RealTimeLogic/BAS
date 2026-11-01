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
 *   COPYRIGHT:  Real Time Logic, 2007 - 2026
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
#ifndef BA_HAVE_LOG2
#define BA_HAVE_LOG2 0
#endif
#ifndef BA_HAVE_STDIO
#define BA_HAVE_STDIO 0
#endif
#if BA_HAVE_STDIO
#ifndef BA_LUA_OS_LIBRARY_MODE
#define BA_LUA_OS_LIBRARY_MODE BA_LUA_OS_LIBRARY_FULL
#endif
#else
#ifndef BA_LUA_OS_LIBRARY_MODE
#define BA_LUA_OS_LIBRARY_MODE BA_LUA_OS_LIBRARY_MINIMAL
#endif
#endif
#ifndef BA_ENABLE_LUA_STDIO
#define BA_ENABLE_LUA_STDIO 1
#endif


/***********************************************************************
 *  Misc settings
 ***********************************************************************/

#define HTTP_E_BASE 1 /* Error code base */
#ifndef SERVER_SOFTWARE_NAME
#define SERVER_SOFTWARE_NAME "BarracudaServer.com (VxWorks)"
#endif

#ifndef BA_VXWORKS
#define BA_VXWORKS
#endif

#ifndef BA_API
#define BA_API
#endif

#ifndef SHARKSSL_API
#define SHARKSSL_API BA_API
#endif

#ifndef BA_ENABLE_SOCKET_REUSE_ADDRESS
#define BA_ENABLE_SOCKET_REUSE_ADDRESS 1
#endif

#include <assert.h>
#include <BaErrorCodes.h>
#ifdef NDEBUG
#define baAssert(exp)
#else
#define baAssert(exp) assert(exp)
#endif

/* VxWorks/compiler facts supplement the common GCC/Clang detection. */
#if (defined(__BYTE_ORDER) && defined(__BIG_ENDIAN) && __BYTE_ORDER == __BIG_ENDIAN) || \
    defined(__ARMEB__) || defined(__THUMBEB__) || defined(__AARCH64EB__) || \
    defined(_MIPSEB) || defined(__MIPSEB) || defined(__MIPSEB__)
#ifndef BA_BYTE_ORDER
#define BA_BYTE_ORDER BA_BYTE_ORDER_BIG_ENDIAN
#elif BA_BYTE_ORDER != BA_BYTE_ORDER_BIG_ENDIAN
#error BA_BYTE_ORDER disagrees with VxWorks
#endif
#elif (defined(__BYTE_ORDER) && defined(__LITTLE_ENDIAN) && __BYTE_ORDER == __LITTLE_ENDIAN) || \
      defined(__ARMEL__) || defined(__THUMBEL__) || defined(__AARCH64EL__) || \
      defined(_MIPSEL) || defined(__MIPSEL) || defined(__MIPSEL__)
#ifndef BA_BYTE_ORDER
#define BA_BYTE_ORDER BA_BYTE_ORDER_LITTLE_ENDIAN
#elif BA_BYTE_ORDER != BA_BYTE_ORDER_LITTLE_ENDIAN
#error BA_BYTE_ORDER disagrees with VxWorks
#endif
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

#define BA_HAVE_IPV6_MULTICAST_MEMBERSHIP 0

#include <BaConfigTypes.h>
#ifndef BA_THREAD_STACK_SIZE
#if BA_ARCH_64BIT
#define BA_THREAD_STACK_SIZE 30000
#else
#define BA_THREAD_STACK_SIZE 15000
#endif
#endif

#include <BaConfig.h>



/* For SharkSSL's buf. align. */
#if BA_ARCH_64BIT
#define SHARKSSL_64BIT
#endif 
 


#include <stddef.h>

/***********************************************************************
 *  Memory allocation
 ***********************************************************************/

#if BA_ALLOCATOR == BA_ALLOCATOR_DLMALLOC
#include <dlmalloc.h>
#else
#include <stdlib.h>
#define baMalloc(size) malloc(size)
#define baRealloc(ptr,size) realloc(ptr,size)
#define baFree(ptr) free(ptr)
#endif

/***********************************************************************
 *  Barracuda types
 ***********************************************************************/
#include <GenPrimT.h>

/***********************************************************************
 *  Time functions
 ***********************************************************************/
#include <time.h>
#include <sysLib.h>
#define baGetUnixTime() time(0)
#define baGetMsClock() (U32)(((U64)tickGet() * (U64)1000) / (U64)sysClkRateGet())

/***********************************************************************
 *  All string manipulation functions in Barracuda
 ***********************************************************************/
#define bStrncmp strncmp
#define bStrchr strchr
#define bStrstr strstr
#define bStrrchr strrchr
#define bStrncpy strncpy
#define bAtoi atoi
#define bTolower(x) tolower((U8)x)
#define bIsspace(x) isspace((U8)x)
#define bIsxdigit(x) isxdigit((U8)x)

/***********************************************
 * Version specific VxWorks stuff
 ***********************************************/
#if (_WRS_VXWORKS_MAJOR == 6)
# if (_WRS_VXWORKS_MINOR == 7)
#  define VXWORKS_VER 0x670
# elif (_WRS_VXWORKS_MINOR == 9)
#  define VXWORKS_VER 0x690
# endif
#elif (_WRS_VXWORKS_MAJOR == 7)
# define VXWORKS_VER 0x700
#endif

#if (VXWORKS_VER < 0x690)
# define VX_USR_ARG_T int
#else
# define VX_USR_ARG_T _Vx_usr_arg_t
#endif

#endif

