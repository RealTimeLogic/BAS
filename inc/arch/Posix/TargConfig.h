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
 *   COPYRIGHT:  Real Time Logic, 2004 - 2023
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
#if !defined(B_LITTLE_ENDIAN) && !defined(B_BIG_ENDIAN)
#if defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define B_BIG_ENDIAN 1
#else
#define B_LITTLE_ENDIAN 1
#endif
#endif

/***********************************************************************
 *  Misc settings
 ***********************************************************************/

#define BA_POSIX

#define ENABLE_REUSE_ADDR 1

#ifdef __NuttX__
#define USE_DLMALLOC 1
#pragma GCC diagnostic ignored "-Wshadow"
#pragma GCC diagnostic ignored "-Wundef"
#pragma GCC diagnostic ignored "-Wstrict-prototypes"
#else
/* https://realtimelogic.com/ba/doc/?url=auxlua.html#reverseproxy
 */
#ifndef USE_REDIRECTOR
#define USE_REDIRECTOR 1
#endif
#ifndef USE_UBJSON
#define USE_UBJSON 1
#endif
#ifndef USE_REVCON
#define USE_REVCON 1
#endif
#ifndef USE_LPIPE
#define USE_LPIPE 1
#endif
/* https://realtimelogic.com/ba/doc/?url=auxlua.html#forkptylib
 */
#ifndef USE_FORKPTY
#define USE_FORKPTY 1
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


#ifndef SHARKSSL_ECC_USE_SECP256R1
#define SHARKSSL_ECC_USE_SECP256R1 1
#endif
#ifndef SHARKSSL_ECC_USE_SECP384R1
#define SHARKSSL_ECC_USE_SECP384R1 1
#endif
#ifndef SHARKSSL_ECC_USE_SECP521R1
#define SHARKSSL_ECC_USE_SECP521R1 1
#endif
#ifndef SHARKSSL_ECC_USE_BRAINPOOLP256R1
#define SHARKSSL_ECC_USE_BRAINPOOLP256R1 1
#endif
#ifndef SHARKSSL_ECC_USE_BRAINPOOLP384R1
#define SHARKSSL_ECC_USE_BRAINPOOLP384R1 1
#endif
#ifndef SHARKSSL_ECC_USE_BRAINPOOLP512R1
#define SHARKSSL_ECC_USE_BRAINPOOLP512R1 1
#endif


/* For SharkSSL */
#if defined(__LP64__) && !defined(BA_64BIT)
#define BA_64BIT
#endif
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
