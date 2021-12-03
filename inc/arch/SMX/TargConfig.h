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
 *   $Id: TargConfig.h 4915 2021-12-01 18:26:55Z wini $
 *
 *   COPYRIGHT:  Real Time Logic, 2004
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

#ifndef BA_API
#define BA_API
#endif
#ifndef SHARKSSL_API
#define SHARKSSL_API BA_API
#endif

#include <smx.h>


#define HTTP_E_BASE 1 /* Error code base */
#ifndef SERVER_SOFTWARE_NAME
#define SERVER_SOFTWARE_NAME "BarracudaServer.com (SMX)"
#endif


#if !defined(B_BIG_ENDIAN) && !defined(B_LITTLE_ENDIAN)
#if SB_CPU_LITTLE_ENDIAN
#define B_LITTLE_ENDIAN
#else
#define B_BIG_ENDIAN
#endif
#endif

#include <BaErrorCodes.h>
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

#ifdef USE_DLMALLOC
#ifdef __cplusplus
extern "C" {
#endif
void init_dlmalloc(char* heapstart, char* heapend);
void* dlmalloc(size_t bytes);
void* dlrealloc(void* oldmem, size_t bytes);
void dlfree(void* mem);
#ifdef __cplusplus
}
#endif
#define baMalloc(size) dlmalloc(size)
#define baRealloc(ptr, size) dlrealloc(ptr, size);
#define baFree(ptr) dlfree(ptr)
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
