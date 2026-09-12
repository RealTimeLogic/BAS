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
 *                            HEADER
 *
 *   $Id: GenPrimT.h 5978 2026-09-11 16:13:48Z wini $
 *
 *   COPYRIGHT:  Real Time Logic, 2004 - 2022
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
 *              Do not directly include this file!!!
 *  This header file contains Generic Primitive Type definitions.
 *  The header file is included by the architecture dependent header file
 *  TargConfig.h for most of the supported platforms.
 */

/** @file GenPrimT.h */

#ifndef _TargConfig_h
#include <TargConfig.h>
#endif

#ifndef _GenPrimT_h
#define _GenPrimT_h
#include <stdint.h>


#define SHARKSSL_USE_SHA1       1
#define SHARKSSL_USE_MD5        1
#define SHARKSSL_ENABLE_AES_CCM 1
#define SHARKSSL_HMAC_API       1
#define SHARKSSL_ENABLE_AES_CBC 1
#define SHARKSSL_ENABLE_ENCRYPTED_PKCS8_SUPPORT 1

#ifdef __ICCARM__
#pragma diag_suppress=Pa084
#pragma diag_suppress=Pa089
#pragma diag_suppress=Pa181
#pragma diag_suppress=Pe188
#pragma diag_suppress=Pe546
#pragma diag_suppress=Pe550
#endif

/** @addtogroup UtilityFunctions
    @{
 */


/***********************************************************************
 *  Barracuda types
 ***********************************************************************/

/* Define Barracuda's legacy types */
/** Signed 8-bit integer. */
typedef int8_t  S8;
/** Signed 16-bit integer. */
typedef int16_t S16;
/** Signed 32-bit integer. */
typedef int32_t S32;
/** Signed 64-bit integer. */
typedef int64_t S64;
/** Unsigned 8-bit integer. */
typedef uint8_t  U8;
/** Unsigned 16-bit integer. */
typedef uint16_t U16;
/** Unsigned 32-bit integer. */
typedef uint32_t U32;
/** Unsigned 64-bit integer. */
typedef uint64_t U64;

/** An arithmetic type representing calendar time
    with epoch of 1970-01-01 00:00:00 UTC, that is,
    +/- number of seconds since the epoch of 1970-01-01
    \sa baTime2tm
    \sa baTm2Time
*/
typedef S64 BaTime;


/** UTC timestamp with a fractional second and an explicit timezone offset.
 */
typedef struct {
   /** Signed seconds since 1970-01-01 00:00:00 UTC, independent of offset. */
   BaTime sec;
   /** Nanoseconds, valid range: 0 to 999999999 */
   S32 nsec;
   /** Minutes east of UTC, from -1439 to 1439; negative values are west. */
   S16 offset;
} BaTimeEx;

/** Boolean stored in an unsigned byte; FALSE is zero and TRUE is one. */
typedef U8 BaBool;
#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

#define S64_getMsw(o) ((S32)(0xFFFFFFFF & ((o) >> 32) ))

#ifdef BA_FILESIZE64
/** Unsigned file size or position in bytes; 64 bits with BA_FILESIZE64. */
typedef U64 BaFileSize;
/** Signed file-size value in bytes; 64 bits with BA_FILESIZE64. */
typedef S64 SBaFileSize;
/* Unsigned and Signed baprintf FileSize format flags */
#define BA_UFSF "llu"
#define BA_SFSF "lld"
#else
/** Unsigned file size or position in bytes; 32 bits without BA_FILESIZE64. */
typedef U32 BaFileSize;
/** Signed file-size value in bytes; 32 bits without BA_FILESIZE64. */
typedef S32 SBaFileSize;
#define BA_UFSF "lu"
#define BA_SFSF "ld"
#endif
/** @} */

#define iStrlen(s) ((int)strlen(s))

#include "BaAtoi.h"

#endif
