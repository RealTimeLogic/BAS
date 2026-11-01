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
 *   $Id: BaConfigEndian.h 6694 2026-10-30 22:03:51Z gianluca $
 *
 *   COPYRIGHT:  Real Time Logic LLC, 2023 - 2026
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
 * Resolve and validate native byte order after the port has established its
 * facts. This macro-only header also supports BAS assembly and build probes.
 */
#ifndef _BaConfigEndian_h
#define _BaConfigEndian_h
#include <BaConfigDefs.h>

#ifndef BA_INTERNAL_CONFIG_OPTS_LOADED
#define BA_INTERNAL_CONFIG_OPTS_LOADED 1
#include <BaConfigOpts.h>
#endif

/* Application options may select byte order, but cannot change its constants. */
#if BA_BYTE_ORDER_LITTLE_ENDIAN != 1234
#error BA_BYTE_ORDER_LITTLE_ENDIAN must be 1234
#endif
#if BA_BYTE_ORDER_BIG_ENDIAN != 4321
#error BA_BYTE_ORDER_BIG_ENDIAN must be 4321
#endif

#if defined(B_LITTLE_ENDIAN) || defined(B_BIG_ENDIAN)
#error Replace legacy endian flags with BA_BYTE_ORDER
#endif

/* Compiler facts are independent of an explicit application selection. */
#if defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__) && defined(__ORDER_BIG_ENDIAN__)
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define BA_INTERNAL_COMPILER_BYTE_ORDER 1234
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define BA_INTERNAL_COMPILER_BYTE_ORDER 4321
#else
#error Unsupported compiler byte order
#endif
#elif defined(_MSC_VER) && (defined(_M_IX86) || defined(_M_X64) || defined(_M_ARM) || defined(_M_ARM64))
#define BA_INTERNAL_COMPILER_BYTE_ORDER 1234
/* IAR defines __LITTLE_ENDIAN__ as 1 or 0 for either byte order. */
#elif defined(__IAR_SYSTEMS_ICC__) || defined(__IAR_SYSTEMS_ASM__)
#if __LITTLE_ENDIAN__
#define BA_INTERNAL_COMPILER_BYTE_ORDER 1234
#else
#define BA_INTERNAL_COMPILER_BYTE_ORDER 4321
#endif
#elif defined(__LITTLE_ENDIAN__) && !defined(__BIG_ENDIAN__)
#define BA_INTERNAL_COMPILER_BYTE_ORDER 1234
#elif defined(__BIG_ENDIAN__) && !defined(__LITTLE_ENDIAN__)
#define BA_INTERNAL_COMPILER_BYTE_ORDER 4321
#elif defined(__CC_ARM) && defined(__ARM_BIG_ENDIAN)
#define BA_INTERNAL_COMPILER_BYTE_ORDER 4321
#elif defined(__CC_ARM)
#define BA_INTERNAL_COMPILER_BYTE_ORDER 1234
#endif

#ifndef BA_BYTE_ORDER
#if defined(BA_INTERNAL_COMPILER_BYTE_ORDER) && (BA_INTERNAL_COMPILER_BYTE_ORDER == 1234)
#define BA_BYTE_ORDER BA_BYTE_ORDER_LITTLE_ENDIAN
#elif defined(BA_INTERNAL_COMPILER_BYTE_ORDER) && (BA_INTERNAL_COMPILER_BYTE_ORDER == 4321)
#define BA_BYTE_ORDER BA_BYTE_ORDER_BIG_ENDIAN
#else
#error The port must define BA_BYTE_ORDER
#endif
#endif

#if (BA_BYTE_ORDER != BA_BYTE_ORDER_LITTLE_ENDIAN) && (BA_BYTE_ORDER != BA_BYTE_ORDER_BIG_ENDIAN)
#error BA_BYTE_ORDER must select LITTLE_ENDIAN or BIG_ENDIAN
#endif
#if defined(BA_INTERNAL_COMPILER_BYTE_ORDER) && (BA_BYTE_ORDER != BA_INTERNAL_COMPILER_BYTE_ORDER)
#error BA_BYTE_ORDER disagrees with the compiler
#endif
#undef BA_INTERNAL_COMPILER_BYTE_ORDER

#endif /* _BaConfigEndian_h */
