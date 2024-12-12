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
 *   $Id: TargConfig.h 5594 2024-12-12 16:18:11Z wini $
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

#include <mqx.h>


/***********************************************************************
 *  Misc settings
 ***********************************************************************/

#define HTTP_E_BASE 1 /* Error code base */
#ifndef SERVER_SOFTWARE_NAME
#define SERVER_SOFTWARE_NAME "BarracudaServer.com (MQX)"
#endif

/* not used */
#ifndef BA_API
#define BA_API
#define SHARKSSL_API
#endif
#define BAI_FUNC

#if PSP_ENDIAN != MQX_BIG_ENDIAN && PSP_ENDIAN != MQX_LITTLE_ENDIAN
#error something is wrong
#endif
#if PSP_ENDIAN == MQX_BIG_ENDIAN
#define B_BIG_ENDIAN
#else
#define B_LITTLE_ENDIAN
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

#include <BaSharkSslOpt.h>

#include <BaErrorCodes.h>
#ifdef NDEBUG
#define baAssert(exp)
#else
#define baAssert(exp) \
         ( (exp) ? (void)0 : baFatalEf(FE_ASSERT, 0, __FILE__, __LINE__) )
#endif

/***********************************************************************
 *  Barracuda types
 ***********************************************************************/

#include <GenPrimT.h>


/***********************************************************************
 *  Memory allocation
 ***********************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#ifdef USE_MQX_MALLOC
/* Note: Do not use when using the Lua plugin */
#define baMalloc(size) _mem_alloc_system(size)
void* baRealloc(void* ptr, unsigned long size);
#define baFree(ptr) _mem_free(ptr)
#else
#include <stddef.h>
#include <dlmalloc.h>
#endif

#ifdef __cplusplus
}
#endif 


/***********************************************************************
 *  Time functions
 ***********************************************************************/


/* See ThreadLib.c */
#ifdef __cplusplus
extern "C" {
#endif
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


#endif
