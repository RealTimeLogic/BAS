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
#define BA_ALLOCATOR BA_ALLOCATOR_DEFAULT
#endif
#if BA_ALLOCATOR == BA_ALLOCATOR_MQX
#error BA_ALLOCATOR_MQX requires the MQX port
#endif


/* C runtime and Lua library capabilities for this port. */
#ifdef _WIN32_WCE
#ifndef BA_HAVE_LOG2
#define BA_HAVE_LOG2 0
#endif
#ifndef BA_USE_C_RUNTIME_TIME
#define BA_USE_C_RUNTIME_TIME 0
#endif
#ifndef BA_HAVE_STDIO
#define BA_HAVE_STDIO 0
#endif
#ifndef BA_LUA_OS_LIBRARY_MODE
#define BA_LUA_OS_LIBRARY_MODE BA_LUA_OS_LIBRARY_MINIMAL
#endif
#endif
#ifndef BA_HAVE_STDIO
#define BA_HAVE_STDIO 1
#endif
#ifndef BA_LUA_OS_LIBRARY_MODE
#define BA_LUA_OS_LIBRARY_MODE BA_LUA_OS_LIBRARY_FULL
#endif
#ifndef BA_ENABLE_LUA_STDIO
#define BA_ENABLE_LUA_STDIO 1
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_WARNINGS
#endif

#pragma warning( disable : 4127 4706 4054)

#define BA_WINDOWS


#ifndef BA_BYTE_ORDER
#define BA_BYTE_ORDER BA_BYTE_ORDER_LITTLE_ENDIAN
#elif BA_BYTE_ORDER != BA_BYTE_ORDER_LITTLE_ENDIAN
#error BA_BYTE_ORDER disagrees with Windows
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

#ifndef BA_ENABLE_SOCKET_REUSE_ADDRESS
#define BA_ENABLE_SOCKET_REUSE_ADDRESS 1
#endif

/* https://realtimelogic.com/ba/doc/?url=auxlua.html#reverseproxy
 */
#ifndef BA_ENABLE_REVERSE_PROXY
#define BA_ENABLE_REVERSE_PROXY 1
#endif

#ifndef BA_ENABLE_UBJSON
#define BA_ENABLE_UBJSON 1
#endif

#ifndef BA_ENABLE_REVERSE_CONNECTION
#define BA_ENABLE_REVERSE_CONNECTION 1
#endif

#ifndef SERVER_SOFTWARE_NAME
#define SERVER_SOFTWARE_NAME "BarracudaServer.com (Windows)"
#endif
#define HTTP_E_BASE 1 /* Error code base */

/* Defaults to 64 bit file size on Windows if Lua type is not integer */
#if !defined(GCW_INT) && !defined(BA_FILE_OFFSET_BITS)
#define BA_FILE_OFFSET_BITS 64
#endif

#include <BaConfig.h>

/* For SharkSSL */
#if BA_ARCH_64BIT
#define SHARKSSL_64BIT
#endif 


/* One BAS DLL: define BA_DLLBUILD for both producer and consumers.
   Define BA_LIB for every source compiled into the DLL, before including
   any BAS header. Only public APIs carry BA_API; private linkage uses BAI_FUNC.
*/

#ifdef BA_DLLBUILD
#if defined(BA_LIB)
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





/***********************************************************************
 *  Memory allocation
 ***********************************************************************/
#ifdef BA_LEAK_CHECK
#include <LeakDetect.h>
#elif (BA_ALLOCATOR == BA_ALLOCATOR_DLMALLOC)
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
