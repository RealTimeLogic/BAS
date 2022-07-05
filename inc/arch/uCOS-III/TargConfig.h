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
 *   $Id$
 *
 *   COPYRIGHT:  Real Time Logic, 2022
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
 *
 */
#ifndef _TargConfig_h
#define _TargConfig_h

#ifndef BA_API
#define BA_API
#endif

#ifdef __ICCARM__
#pragma diag_suppress=Pa091
#pragma diag_suppress=Pa109
#pragma diag_suppress=Pe193 
#pragma diag_suppress=Pe068
#pragma diag_suppress=Pe1348
#pragma diag_suppress=Pe2349
#pragma diag_suppress=Pe1873
#pragma diag_suppress=Go030
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

/* Allocator code below hard-coded for dlmalloc */
#ifndef USE_DLMALLOC
#define USE_DLMALLOC
#endif

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
#define  baMalloc(size) dlmalloc(size)
#define baRealloc(ptr, size) dlrealloc(ptr, size)
#define baFree(ptr) dlfree(ptr)


/***********************************************************************
 *  Barracuda time
 ***********************************************************************/

/* See ThreadLib.c */
#ifdef __cplusplus
extern "C" {
#endif
void baSetUnixTime(BaTime time);
BaTime baGetUnixTime(void);
BaTime baGetMsClock(void);
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
#define SERVER_SOFTWARE_NAME "BarracudaServer.com (uCOS-III)"
#endif
#define HTTP_E_BASE 1 /* Error code base */

#if !defined(B_LITTLE_ENDIAN) &&  !defined(B_BIG_ENDIAN)
#ifdef __LITTLE_ENDIAN__
#define B_LITTLE_ENDIAN
#elif defined(__BIG_ENDIAN__)
#define B_BIG_ENDIAN
#else
#error endian needed 
#endif
#endif



#define BAI_FUNC	extern
#define BAI_DATA	extern

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

#endif
