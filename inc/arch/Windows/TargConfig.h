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
 *   $Id: TargConfig.h 5394 2023-02-21 18:41:44Z wini $
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
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_WARNINGS
#endif

#pragma warning( disable : 4127 4706 4054)

#define BA_WINDOWS

#if _M_X64 == 100
#define BA_64BIT
#endif

#if !defined(B_LITTLE_ENDIAN)
#define B_LITTLE_ENDIAN
#endif

#ifdef _WIN32_WCE
#define errno  GetLastError()
#define BUFSIZ 512
#define basocklen_t int
#endif

#include <windows.h>
#ifndef BAS_RTOS_WINDOWS_SIM
#include <winsock2.h>
#endif

/***********************************************************************
 *  Misc settings
 ***********************************************************************/

#define ENABLE_REUSE_ADDR 1

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

#ifndef SERVER_SOFTWARE_NAME
#define SERVER_SOFTWARE_NAME "BarracudaServer.com (Windows)"
#endif
#define HTTP_E_BASE 1 /* Error code base */

/* Defaults to 64 bit file size on Windows if Lua type is not integer */
#if !defined(GCW_INT) && !defined(BA_FILESIZE64)
#define BA_FILESIZE64
#endif

#ifndef SHARKSSL_USE_SHA_512
#define SHARKSSL_USE_SHA_512                             1
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
#ifdef BA_64BIT
#define SHARKSSL_64BIT
#endif 


/* BA_DLLBUILD defines import/export depending on whether
   BA_LIB is defined. Modules that have BA_LIB defined will have
   their BA_API functions exported. This scheme is similar to the
   Lua scheme which uses LUA_BUILD_AS_DLL
*/

#ifdef BA_DLLBUILD
#if defined(BA_LIB)
#define LUA_BUILD_AS_DLL
#define LUA_LIB
#define BA_API __declspec(dllexport)
#else
#define BA_API __declspec(dllimport)
#endif
#else
#define BA_API
#endif

#include <BaErrorCodes.h>
#ifndef baAssert
#ifdef NDEBUG
#define baAssert(exp)
#else
#define baAssert(exp) \
   ( (exp) ? (void)0 : baFatalEf(FE_ASSERT, 0, __FILE__, __LINE__) )
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




/***********************************************************************
 *  Memory allocation
 ***********************************************************************/
#ifdef BA_LEAK_CHECK
#include <LeakDetect.h>
#elif defined(USE_DLMALLOC)
#include <dlmalloc.h>
#else
#include <malloc.h>
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
#ifdef _WIN32_WCE
BA_API unsigned baGetUnixTime(void);
#else
#define baGetUnixTime() ((BaTime)time(0))
BA_API void _baClckGettime(BaTimeEx* spec);
#define baClckGettime _baClckGettime
#endif
#define baGetClock() ((BaTime)clock()/CLOCKS_PER_SEC)
#define baGetMsClock() GetTickCount()

/***********************************************************************
 *  All string manipulation functions in Barracuda (but not Lua)
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
