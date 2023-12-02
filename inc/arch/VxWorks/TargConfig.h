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
 *   $Id: TargConfig.h 5496 2023-11-23 18:51:15Z wini $
 *
 *   COPYRIGHT:  Real Time Logic, 2007 - 2023
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

#define ENABLE_REUSE_ADDR 1

#include <assert.h>
#include <BaErrorCodes.h>
#ifdef NDEBUG
#define baAssert(exp)
#else
#define baAssert(exp) assert(exp)
#endif

#if !defined(B_BIG_ENDIAN) && !defined(B_LITTLE_ENDIAN)
  /* Works with clang/gcc */
  #if defined(__BYTE_ORDER) && __BYTE_ORDER == __BIG_ENDIAN || \
      defined(__BIG_ENDIAN__) || \
      defined(__ARMEB__) || \
      defined(__THUMBEB__) || \
      defined(__AARCH64EB__) || \
      defined(_MIBSEB) || defined(__MIBSEB) || defined(__MIBSEB__)
        #define B_BIG_ENDIAN 
  #elif defined(__BYTE_ORDER) && __BYTE_ORDER == __LITTLE_ENDIAN || \
      defined(__LITTLE_ENDIAN__) || \
      defined(__ARMEL__) || \
      defined(__THUMBEL__) || \
      defined(__AARCH64EL__) || \
      defined(_MIPSEL) || defined(__MIPSEL) || defined(__MIPSEL__)
        #define B_LITTLE_ENDIAN
  #else
    #error Define one of B_BIG_ENDIAN or B_LITTLE_ENDIAN
  #endif
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

#define NO_IPV6_MEMBERSHIP

#include <BaSharkSslOpt.h>

#if defined(__LP64__) && !defined(BA_64BIT)
#define BA_64BIT
#ifndef BA_STACKSZ
#define BA_STACKSZ 30000
#endif
#endif

/* For SharkSSL's buf. align. */
#ifdef BA_64BIT
#define SHARKSSL_64BIT
#endif 
 
#ifndef BA_STACKSZ
#define BA_STACKSZ 15000
#endif

#include <stddef.h>

/***********************************************************************
 *  Memory allocation
 ***********************************************************************/

#ifdef USE_DLMALLOC
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

