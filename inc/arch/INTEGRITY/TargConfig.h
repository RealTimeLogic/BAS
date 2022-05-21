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
 *   $Id: TargConfig.h 5146 2022-05-10 17:15:54Z wini $
 *
 *   COPYRIGHT:  Real Time Logic, 2011 - 2020
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
 * Platform: INTEGRITY.
 *
 */
#ifndef _TargConfig_h
#define _TargConfig_h

#include <INTEGRITY.h>

#if defined(__LittleEndian) || defined(LittleEndian) || defined(TM_LITTLE_ENDIAN)
  #ifdef B_BIG_ENDIAN
    #error ENDIANESS MISMATCH!
  #endif
  #ifndef B_LITTLE_ENDIAN
    #define B_LITTLE_ENDIAN
  #endif
#elif defined(__BigEndian) || defined(BigEndian) || defined(TM_BIG_ENDIAN)
  #ifdef B_LITTLE_ENDIAN
    #error ENDIANESS MISMATCH!
  #endif
  #ifndef B_BIG_ENDIAN
    #define B_BIG_ENDIAN
  #endif
#else
#error Missing little/big endian declaration
#endif



#ifndef BA_API
#define BA_API
#endif

/***********************************************************************
 *  Barracuda types
 ***********************************************************************/

#define B_OVERLOAD_BASIC_TYPES
typedef signed char S8;
typedef unsigned char U8;
typedef signed short S16;
typedef unsigned short U16;
typedef signed long S32;
typedef unsigned long U32;



#ifndef _GenPrimT_h
#include <GenPrimT.h>
#endif

/***********************************************************************
 *  Memory allocation
 ***********************************************************************/
#include <stddef.h>

#ifdef USE_DLMALLOC
#ifdef __cplusplus
extern "C" {
#endif
void init_dlmalloc(char* heapstart, char* heapend);
void* dlmalloc(size_t bytes);
void* dlrealloc(void* oldmem, size_t bytes);
void dlfree(void* mem);
#define  baMalloc(size) dlmalloc(size)
#define baRealloc(ptr, size) dlrealloc(ptr, size);
#define baFree(ptr) dlfree(ptr)
#ifdef __cplusplus
}
#endif
#else
#include <stdlib.h>
#define baMalloc(size) malloc(size)
#define baRealloc(ptr,size) realloc(ptr,size)
#define baFree(ptr) free(ptr)
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
#define SERVER_SOFTWARE_NAME "BarracudaServer.com (INTEGRITY)"
#endif
#define HTTP_E_BASE 1 /* Error code base */

#define DL_UNLINK_INLINE 0

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

#ifndef SHARKSSL_API
#define SHARKSSL_API BA_API
#endif

#ifndef BALUA_API
#define BALUA_API BA_API
#endif



#include <BaErrorCodes.h>
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
