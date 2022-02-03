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
 *   $Id: GenPrimT.h 5069 2022-02-03 13:58:42Z wini $
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
typedef int8_t  S8;
typedef int16_t S16;
typedef int32_t S32;
typedef int64_t S64;
typedef uint8_t  U8;
typedef uint16_t U16;
typedef uint32_t U32;
typedef uint64_t U64;

/** An arithmetic type representing calendar time
    with epoch of 1970-01-01 00:00:10 GMT -- i.e.
    +/- number of seconds since the epoch of 1970-01-01
    \sa baTime2tm
    \sa baTm2Time
*/
typedef S64 BaTime;


/** Extended BaTime with nano-seconds and timezone offset in minutes.
 */
typedef struct {
   /** epoch */
   BaTime sec;
   /** Nanoseconds, valid range: 0 to 999999999 */
   S32 nsec;
   /** Offset from UTC in minutes, valid range: -1439 to 1439 */
   S16 offset;
} BaTimeEx;

typedef U8 BaBool;
#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

#define S64_getMsw(o) ((S32)(0xFFFFFFFF & ((o) >> 32) ))

#ifdef BA_FILESIZE64
typedef U64 BaFileSize;
typedef S64 SBaFileSize;
/* Unsigned and Signed baprintf FileSize format flags */
#define BA_UFSF "llu"
#define BA_SFSF "lld"
#else
typedef U32 BaFileSize;
typedef S32 SBaFileSize;
#define BA_UFSF "lu"
#define BA_SFSF "ld"
#endif
/** @} */

#define iStrlen(s) ((int)strlen(s))

#include "BaAtoi.h"

#endif
