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
 *   $Id: BaConfigTypes.h 6657 2026-10-28 18:19:39Z wini $
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
 * ABI settings shared by the port configuration and primitive types.
 */
#ifndef _BaConfigTypes_h
#define _BaConfigTypes_h
#include <BaConfigDefs.h>
#include <stdint.h>

#if UINTPTR_MAX == UINT64_MAX
#ifndef BA_ARCH_64BIT
#define BA_ARCH_64BIT 1
#endif
#if BA_ARCH_64BIT != 1
#error BA_ARCH_64BIT disagrees with the compiler pointer size
#endif
#elif UINTPTR_MAX == UINT32_MAX
#ifndef BA_ARCH_64BIT
#define BA_ARCH_64BIT 0
#endif
#if BA_ARCH_64BIT != 0
#error BA_ARCH_64BIT disagrees with the compiler pointer size
#endif
#else
#error BAS requires 32-bit or 64-bit pointers
#endif

/* A port may choose its file ABI before including this header. */
#ifndef BA_FILE_OFFSET_BITS
#define BA_FILE_OFFSET_BITS 32
#endif
#if BA_FILE_OFFSET_BITS != 32 && BA_FILE_OFFSET_BITS != 64
#error BA_FILE_OFFSET_BITS must be 32 or 64
#endif
#endif
