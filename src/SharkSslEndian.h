/**
 *     ____             _________                __                _
 *    / __ \___  ____ _/ /_  __(_)___ ___  ___  / /   ____  ____ _(_)____
 *   / /_/ / _ \/ __ `/ / / / / / __ `__ \/ _ \/ /   / __ \/ __ `/ / ___/
 *  / _, _/  __/ /_/ / / / / / / / / / / /  __/ /___/ /_/ / /_/ / / /__
 * /_/ |_|\___/\__,_/_/ /_/ /_/_/ /_/ /_/\___/_____/\____/\__, /_/\___/
 *                                                       /____/
 *
 *                 SharkSSL Embedded SSL/TLS Stack
 ****************************************************************************
 *   PROGRAM MODULE
 *
 *   $Id: SharkSslEndian.h 6818 2026-11-08 21:18:02Z gianluca $
 *
 *   COPYRIGHT:  Real Time Logic LLC, 2010 - 2026
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
 *               http://www.sharkssl.com
 ****************************************************************************
 *
 */
#ifndef _SharkSslEndian_h
#define _SharkSslEndian_h

#include "TargConfig.h"
#include "SharkSSL_cfg.h"

/**
 * the following macros return the A|B|C|D bytes of a 32-bit word
 * A: bits [31..24]
 * B: bits [23..16]
 * C: bits [15..8]
 * D: bits [7..0]
 */
#if (BA_BYTE_ORDER == BA_BYTE_ORDER_LITTLE_ENDIAN)
#define GET_U8_A(w) (*(U8*)((U8*)(&(w)) + 3))
#define GET_U8_B(w) (*(U8*)((U8*)(&(w)) + 2))
#define GET_U8_C(w) (*(U8*)((U8*)(&(w)) + 1))
#define GET_U8_D(w) (*(U8*)((U8*)(&(w)) + 0))

#else
#define GET_U8_A(w) (*(U8*)((U8*)(&(w)) + 0))
#define GET_U8_B(w) (*(U8*)((U8*)(&(w)) + 1))
#define GET_U8_C(w) (*(U8*)((U8*)(&(w)) + 2))
#define GET_U8_D(w) (*(U8*)((U8*)(&(w)) + 3))

#endif


#if   (__COLDFIRE__)  /* ColdFire */
static inline asm U32 __declspec(register_abi) sharkssl_byteswap (U32 d) { byterev.l d0 }
#define SHARKSSL_BYTESWAP_U32  sharkssl_byteswap

#elif (__ICCARM__ && __ARM_PROFILE_M__)   /* ARM Cortex-M, IAR EWARM */
#include <intrinsics.h>
#define SHARKSSL_BYTESWAP_U32  __REV
#define __sharkssl_packed      __packed  /* force 1-byte alignment */

#elif (__CC_ARM && __TARGET_PROFILE_M)   /* ARM Cortex-M, Keil MDK */
#define SHARKSSL_BYTESWAP_U32  __rev
#define __sharkssl_packed      __packed  /* force 1-byte alignment */

#elif (__ICCRX__)  /* Renesas RX */
static volatile inline U32 sharkssl_byteswap(U32 value) { asm ("REVL %0, %0" : "+r"(value)); return value; }
#define SHARKSSL_BYTESWAP_U32  sharkssl_byteswap

#elif (__GNUC__)  /* GCC */
#if !defined(_OSX_) && GCC_VERSION >= 402
#ifdef __bswap_32
#define SHARKSSL_BYTESWAP_U32  (U32)__bswap_32
#else
#include <byteswap.h>
#define SHARKSSL_BYTESWAP_U32  (U32)__builtin_bswap32
#endif
#endif
/**
 * A plain U32 pointer lets GCC assume 4-byte alignment and merge accesses,
 * e.g. into LDRD/LDM/STRD on Cortex-M, which fault on unaligned addresses.
 */
typedef U16 __attribute__((aligned(1), may_alias)) SharkSslUnalignedU16;
typedef U32 __attribute__((aligned(1), may_alias)) SharkSslUnalignedU32;
typedef U64 __attribute__((aligned(1), may_alias)) SharkSslUnalignedU64;
#define SHARKSSL_UNALIGNED_U16 SharkSslUnalignedU16
#define SHARKSSL_UNALIGNED_U32 SharkSslUnalignedU32
#define SHARKSSL_UNALIGNED_U64 SharkSslUnalignedU64
#endif


#ifndef __sharkssl_packed
#define __sharkssl_packed
#endif


/**
 * Lvalue types for the SHARKSSL_UNALIGNED_ACCESS paths: (SHARKSSL_UNALIGNED_U32*)p
 * may point to any address. A typedef is needed: IAR applies __packed at the
 * start of a declaration to the pointer, not to the type it points to.
 */
#ifndef SHARKSSL_UNALIGNED_U32
typedef __sharkssl_packed U16 SharkSslUnalignedU16;
typedef __sharkssl_packed U32 SharkSslUnalignedU32;
typedef __sharkssl_packed U64 SharkSslUnalignedU64;
#define SHARKSSL_UNALIGNED_U16 SharkSslUnalignedU16
#define SHARKSSL_UNALIGNED_U32 SharkSslUnalignedU32
#define SHARKSSL_UNALIGNED_U64 SharkSslUnalignedU64
#endif


#ifndef __sharkssl_noinline
#if defined(_MSC_VER)
#define __sharkssl_noinline __declspec(noinline)

#elif defined(__ICCARM__) || defined(__ICCRX__)
/* IAR has no __noinline keyword; the attribute needs -e (as __packed does) */
#define __sharkssl_noinline __attribute__((noinline))

#elif defined(__GNUC__) || defined(__clang__) || defined(__CC_ARM)
#define __sharkssl_noinline __attribute__((noinline))

#else
#define __sharkssl_noinline
#endif
#endif


#ifndef SHARKSSL_BYTESWAP_U32
#define SHARKSSL_BYTESWAP_U32(x) (((x) >> 24) | (((x) << 8) & 0x00FF0000) | (((x) >> 8) & 0x0000FF00) | ((x) << 24))
#endif


#if   ((BA_BYTE_ORDER == BA_BYTE_ORDER_LITTLE_ENDIAN) && SHARKSSL_UNALIGNED_ACCESS)
#define GET_U32_LE(w,a,i)  (w) = ((SHARKSSL_UNALIGNED_U32*)(a))[(i) >> 2]
#elif ((BA_BYTE_ORDER == BA_BYTE_ORDER_BIG_ENDIAN) && SHARKSSL_UNALIGNED_ACCESS)
#define GET_U32_LE(w,a,i)  (w) = SHARKSSL_BYTESWAP_U32(((SHARKSSL_UNALIGNED_U32*)(a))[(i) >> 2])
#else
#define GET_U32_LE(w,a,i)                 \
{                                         \
   (w) = ((U32)(a)[(i)])                  \
       | ((U32)(a)[(i) + 1] <<  8)        \
       | ((U32)(a)[(i) + 2] << 16)        \
       | ((U32)(a)[(i) + 3] << 24);       \
}
#endif


#if   ((BA_BYTE_ORDER == BA_BYTE_ORDER_LITTLE_ENDIAN) && SHARKSSL_UNALIGNED_ACCESS)
#define PUT_U32_LE(w,a,i)  ((SHARKSSL_UNALIGNED_U32*)(a))[(i) >> 2] = (w)
#elif ((BA_BYTE_ORDER == BA_BYTE_ORDER_BIG_ENDIAN) && SHARKSSL_UNALIGNED_ACCESS)
#define PUT_U32_LE(w,a,i)  ((SHARKSSL_UNALIGNED_U32*)(a))[(i) >> 2] = SHARKSSL_BYTESWAP_U32(w)
#else
#define PUT_U32_LE(w,a,i)                 \
{                                         \
   (a)[(i)]     = (U8)((w));              \
   (a)[(i) + 1] = (U8)((w) >>  8);        \
   (a)[(i) + 2] = (U8)((w) >> 16);        \
   (a)[(i) + 3] = (U8)((w) >> 24);        \
}
#endif


#if ((BA_BYTE_ORDER == BA_BYTE_ORDER_BIG_ENDIAN) && SHARKSSL_UNALIGNED_ACCESS)
#define GET_U32_BE(w,a,i)  (w) = ((SHARKSSL_UNALIGNED_U32*)(a))[(i) >> 2]
#elif ((BA_BYTE_ORDER == BA_BYTE_ORDER_LITTLE_ENDIAN) && SHARKSSL_UNALIGNED_ACCESS)
#define GET_U32_BE(w,a,i)  (w) = SHARKSSL_BYTESWAP_U32(((SHARKSSL_UNALIGNED_U32*)(a))[(i) >> 2])
#else
#define GET_U32_BE(w,a,i)                 \
{                                         \
   (w) = ((U32)(a)[(i)] << 24)            \
       | ((U32)(a)[(i) + 1] << 16)        \
       | ((U32)(a)[(i) + 2] <<  8)        \
       | ((U32)(a)[(i) + 3]);             \
}
#endif


#if ((BA_BYTE_ORDER == BA_BYTE_ORDER_BIG_ENDIAN) && SHARKSSL_UNALIGNED_ACCESS)
#define PUT_U32_BE(w,a,i)  ((SHARKSSL_UNALIGNED_U32*)(a))[(i) >> 2] = (w)
#elif ((BA_BYTE_ORDER == BA_BYTE_ORDER_LITTLE_ENDIAN) && SHARKSSL_UNALIGNED_ACCESS)
#define PUT_U32_BE(w,a,i)  ((SHARKSSL_UNALIGNED_U32*)(a))[(i) >> 2] = SHARKSSL_BYTESWAP_U32(w)
#else
#define PUT_U32_BE(w,a,i)                 \
{                                         \
   (a)[(i)]     = (U8)((w) >> 24);        \
   (a)[(i) + 1] = (U8)((w) >> 16);        \
   (a)[(i) + 2] = (U8)((w) >>  8);        \
   (a)[(i) + 3] = (U8)((w));              \
}
#endif


#if ((BA_BYTE_ORDER == BA_BYTE_ORDER_BIG_ENDIAN) && SHARKSSL_UNALIGNED_ACCESS)
#define GET_U64_BE(w,a,i)  (w) = ((SHARKSSL_UNALIGNED_U64*)(a))[(i) >> 3]
#elif ((BA_BYTE_ORDER == BA_BYTE_ORDER_LITTLE_ENDIAN) && SHARKSSL_UNALIGNED_ACCESS)
#define GET_U64_BE(w,a,i)  (w) = ((U64)(SHARKSSL_BYTESWAP_U32(((SHARKSSL_UNALIGNED_U32*)(a))[(i) >> 2])) << 32) + \
                                 (SHARKSSL_BYTESWAP_U32(((SHARKSSL_UNALIGNED_U32*)(a))[((i) >> 2) + 1]))
#else
#define GET_U64_BE(w,a,i)                 \
{                                         \
   (w) = ((U64)(a)[(i)]     << 56)        \
       | ((U64)(a)[(i) + 1] << 48)        \
       | ((U64)(a)[(i) + 2] << 40)        \
       | ((U64)(a)[(i) + 3] << 32)        \
       | ((U64)(a)[(i) + 4] << 24)        \
       | ((U64)(a)[(i) + 5] << 16)        \
       | ((U64)(a)[(i) + 6] <<  8)        \
       | ((U64)(a)[(i) + 7]);             \
}
#endif


#if ((BA_BYTE_ORDER == BA_BYTE_ORDER_BIG_ENDIAN) && SHARKSSL_UNALIGNED_ACCESS)
#define PUT_U64_BE(w,a,i)  ((SHARKSSL_UNALIGNED_U64*)(a))[(i) >> 3] = (w)
#elif ((BA_BYTE_ORDER == BA_BYTE_ORDER_LITTLE_ENDIAN) && SHARKSSL_UNALIGNED_ACCESS)
#define PUT_U64_BE(w,a,i)  ((SHARKSSL_UNALIGNED_U32*)(a))[((i) >> 2) + 1] = SHARKSSL_BYTESWAP_U32(*(SHARKSSL_UNALIGNED_U32*)&(w));  \
                           ((SHARKSSL_UNALIGNED_U32*)(a))[(i) >> 2] = SHARKSSL_BYTESWAP_U32(*(SHARKSSL_UNALIGNED_U32*)((SHARKSSL_UNALIGNED_U32*)&(w) + 1))
#else
#define PUT_U64_BE(w,a,i)                 \
{                                         \
   (a)[(i)]     = (U8)((w) >> 56);        \
   (a)[(i) + 1] = (U8)((w) >> 48);        \
   (a)[(i) + 2] = (U8)((w) >> 40);        \
   (a)[(i) + 3] = (U8)((w) >> 32);        \
   (a)[(i) + 4] = (U8)((w) >> 24);        \
   (a)[(i) + 5] = (U8)((w) >> 16);        \
   (a)[(i) + 6] = (U8)((w) >>  8);        \
   (a)[(i) + 7] = (U8)((w));              \
}
#endif


/**
 * alignment macros
 */
#if (defined(__LP64__) || defined(_WIN64)) && !defined(SHARKSSL_64BIT)
#define SHARKSSL_64BIT
#endif
#ifdef SHARKSSL_64BIT
#ifndef UPTR
#define UPTR                                       U64
#endif
#ifndef SHARKSSL_ALIGNMENT
#define SHARKSSL_ALIGNMENT                         8
#endif
#endif
#ifndef UPTR
#define UPTR                                       U32
#endif
#ifndef SHARKSSL_ALIGNMENT
#define SHARKSSL_ALIGNMENT                         4   /* sizeof(UPTR) */
#endif
#ifndef SHARKSSL_UNALIGNED_MALLOC
#define SHARKSSL_UNALIGNED_MALLOC                  0
#endif
#if ((SHARKSSL_UNALIGNED_MALLOC != 0) && (SHARKSSL_UNALIGNED_MALLOC != 1))
#error SHARKSSL_UNALIGNED_MALLOC must be 0 or 1
#endif
#if ((SHARKSSL_ALIGNMENT < 1) || ((SHARKSSL_ALIGNMENT & (SHARKSSL_ALIGNMENT - 1)) != 0))
#error SHARKSSL_ALIGNMENT must be a positive power of two
#endif
typedef char sharkssl_uptr_must_hold_pointer[(sizeof(UPTR) >= sizeof(void*)) ? 1 : -1];
#define SHARKSSL_ALIGN_SIZE(s)                     (((s) + (SHARKSSL_ALIGNMENT - 1)) & ((U32)-SHARKSSL_ALIGNMENT))
#define SHARKSSL_ALIGNED_POINTER(p)                (U8*)(((UPTR)((UPTR)(p) + SHARKSSL_ALIGNMENT - 1)) & ((UPTR)-SHARKSSL_ALIGNMENT))
#define SHARKSSL_POINTER_IS_ALIGNED(p)             (0 == ((unsigned int)(UPTR)(p) & (SHARKSSL_ALIGNMENT - 1)))
#define SHARKSSL_BIGINT_WORDSIZE_ALIGN             ((U32)3)  /* 32-bit words */

/**
 * SHARKSSL_ALIGNMENT must satisfy the strictest alignment required by any
 * SharkSSL type enabled by the target configuration. SHARKSSL_MALLOC_ALIGN
 * adds allocation headroom; SHARKSSL_PNTR_ALIGNMENT returns the aligned
 * working pointer. Preserve the raw pointer for baFree. baRealloc is safe
 * only for raw byte buffers; aligned typed objects require allocate-copy-free.
 */
#if SHARKSSL_UNALIGNED_MALLOC
#define SHARKSSL_MALLOC_ALIGN(s)                   ((s) + SHARKSSL_ALIGNMENT)
#define SHARKSSL_PNTR_ALIGNMENT(p)                 SHARKSSL_ALIGNED_POINTER(p)
#else
#define SHARKSSL_MALLOC_ALIGN(s)                   (s)
#define SHARKSSL_PNTR_ALIGNMENT(p)                 (U8*)(p)
#endif

/* EACH HEX4/2 BYTE MUST BE TWO DIGITS 00..FF */
#define HEX4_TO_WORDSIZE(a,b,c,d) 0x##a##b##c##d
#define HEX2_TO_WORDSIZE(a,b)     0x##a##b

#if (BA_BYTE_ORDER == BA_BYTE_ORDER_BIG_ENDIAN)
#define memmove_endianness memmove

#else
/**
 * Little-endian targets
 *
 * d(est) and s(rc) must not overlap,
 * unless they point to the very same address
 */
void memmove_endianness(U8 *d, const U8 *s, U16 len);
#endif

#endif /* _SharkSslEndian_h */
