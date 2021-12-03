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
 *   $Id: TargConfig.h 2195 2010-09-16 12:26:20Z wini $
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

#include <RTOS.h>


/***********************************************************************
 *  Misc settings
 ***********************************************************************/

#define HTTP_E_BASE 1 /* Error code base */
#ifndef SERVER_SOFTWARE_NAME
#define SERVER_SOFTWARE_NAME "BarracudaServer.com (embOS)"
#endif

/* not used */
#ifndef BA_API
#define BA_API
#define SHARKSSL_API
#endif
#define BAI_FUNC

#if OS_BIG_ENDIAN
  #ifdef B_LITTLE_ENDIAN
    #error ENDIANESS MISMATCH!
  #endif
  #ifndef B_BIG_ENDIAN
    #define B_BIG_ENDIAN
  #endif
#else
  #ifdef B_BIG_ENDIAN
    #error ENDIANESS MISMATCH!
  #endif
  #ifndef B_LITTLE_ENDIAN
    #define B_LITTLE_ENDIAN
  #endif
#endif


#include <BaErrorCodes.h>
#ifdef NDEBUG
#define baAssert(exp)
#else
#define baAssert(exp) ( (exp) ? (void)0 : baFatalEf(FE_ASSERT, 0, __FILE__, __LINE__) )
#endif

/***********************************************************************
 *  embOS types
 ***********************************************************************/

#include <Global.h>
typedef OS_I16 S16;
typedef OS_I32 S32;
typedef OS_I8  S8;

/* U64 will be redefined in GenPrimT.h */
#define B_OVERLOAD_BASIC_TYPES
#include <GenPrimT.h>


/***********************************************************************
 *  Memory allocation
 ***********************************************************************/
#include <stddef.h>

#ifndef USE_DLMALLOC
#define USE_DLMALLOC
#endif


#ifdef __cplusplus
extern "C" {
#endif
void init_dlmalloc(char* heapstart, char* heapend);
void* dlmalloc(size_t bytes);
void* dlrealloc(void* oldmem, size_t bytes);
void  dlfree(void* mem);

#define baMalloc(size) dlmalloc(size)
#define baRealloc(ptr, size) dlrealloc(ptr, size);
#define baFree(ptr) dlfree(ptr)
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
