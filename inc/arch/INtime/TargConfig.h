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
 *   $Id: TargConfig.h 5375 2023-02-02 21:43:05Z wini $
 *
 *   COPYRIGHT:  2020 - 2023
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

#include <stdio.h>
#include <rt.h>



/*  The default choice for Posix - change by using another arch
 *  or override on compile
 */
#if !defined(B_LITTLE_ENDIAN) && !defined(B_BIG_ENDIAN)
#define B_LITTLE_ENDIAN 1
#endif

/***********************************************************************
 *  Misc settings
 ***********************************************************************/

#define BA_INTIME
#define NO_LOG2

#if _M_X64 == 100
#define BA_64BIT
#endif
#define BA_64BIT //PATCH test

#define BA_FILESIZE64

#define ENABLE_REUSE_ADDR 1

/* https://realtimelogic.com/ba/doc/?url=auxlua.html#reverseproxy
 */
#ifndef USE_REDIRECTOR
//#define USE_REDIRECTOR 1
#endif

#ifndef USE_UBJSON
#define USE_UBJSON 1
#endif

#ifndef USE_REVCON
#define USE_REVCON 1
#endif

#define HTTP_E_BASE 1 /* Error code base */
#ifndef SERVER_SOFTWARE_NAME
#define SERVER_SOFTWARE_NAME "BarracudaServer.com (INtime)"
#endif

#ifndef BA_API
#define BA_API
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
#define BAI_FUNC	extern
#define BAI_DATA	extern

#ifndef SHARKSSL_API
#define SHARKSSL_API BA_API
#endif

#ifndef BALUA_API
#define BALUA_API BA_API
#endif

#ifndef SHARKSSL_USE_SHA_512
#define SHARKSSL_USE_SHA_512 1
#endif

#if 1
#ifndef SHARKSSL_ENABLE_ASN1_KEY_CREATION
#define SHARKSSL_ENABLE_ASN1_KEY_CREATION                1
#endif
#ifndef SHARKSSL_ENABLE_RSAKEY_CREATE
#define SHARKSSL_ENABLE_RSAKEY_CREATE                    1
#endif
#ifndef SHARKSSL_ENABLE_ECCKEY_CREATE
#define SHARKSSL_ENABLE_ECCKEY_CREATE                    1
#endif
#ifndef SHARKSSL_ENABLE_CSR_CREATION
#define SHARKSSL_ENABLE_CSR_CREATION                     1
#endif
#ifndef SHARKSSL_ENABLE_CSR_SIGNING
#define SHARKSSL_ENABLE_CSR_SIGNING                      1
#endif
#endif



/* For SharkSSL */
#ifdef BA_64BIT
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
#ifdef USE_DLMALLOC
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
#define baGetUnixTime() time(0)

#ifdef __cplusplus
extern "C" {
#endif
void _baClckGettime(BaTimeEx* spec);
#define baClckGettime _baClckGettime
BA_API U32 baGetMsClock(void);
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
