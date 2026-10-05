/* Copyright (c) Real Time Logic LLC, 2026. All rights reserved.
 * Private interface for runtime-dispatched x86 cipher backends.
 * Uses the same license as SharkSslCrypto.c. Not an application API.
 */
#ifndef SHARKSSL_CRYPTO_ASM_H
#define SHARKSSL_CRYPTO_ASM_H

#if (defined(_MSC_VER) && !defined(_M_ARM64EC) && (defined(_M_X64) || defined(_M_IX86))) || \
    ((defined(__GNUC__) || defined(__clang__)) && (defined(__linux__) || defined(__QNXNTO__)) && defined(__ELF__) && defined(__x86_64__) && defined(__LP64__) && !defined(__ILP32__))
#define SHARKSSL_X86_ASM_PLATFORM 1
#else
#define SHARKSSL_X86_ASM_PLATFORM 0
#endif
#if SHARKSSL_X86_ASM_PLATFORM && !defined(_M_IX86)
#define SHARKSSL_X64_ASM_PLATFORM 1
#else
#define SHARKSSL_X64_ASM_PLATFORM 0
#endif

#if SHARKSSL_X86_ASM_PLATFORM && (SHARKSSL_OPTIMIZED_CHACHA_ASM || SHARKSSL_OPTIMIZED_POLY1305_ASM || (SHARKSSL_OPTIMIZED_GCM_ASM && SHARKSSL_OPTIMIZED_GHASH_ASM))
#if defined(_MSC_VER)
#include <intrin.h>
#define SHARKSSL_ASM_LOAD(p) ((U8)_InterlockedCompareExchange8((volatile char*)(p), (char)-1, (char)-1))
#define SHARKSSL_ASM_INIT(p,v) ((void)_InterlockedCompareExchange8((volatile char*)(p), (char)(v), (char)-1))
#else
#define SHARKSSL_ASM_LOAD(p) __atomic_load_n((p), __ATOMIC_RELAXED)
/* All initializers compute the same CPU/OS result; publication carries no data. */
#define SHARKSSL_ASM_INIT(p,v) __atomic_store_n((p), (v), __ATOMIC_RELAXED)
#endif

/* Keep these tiny helpers inline in the caller, without an indirect hot call. */
#define SHARKSSL_ASM_AVAILABILITY(name,probe) \
static U8 name(void) \
{ \
   static volatile U8 cached = 0xFF; \
   if (0xFF == SHARKSSL_ASM_LOAD(&cached)) \
   { \
      U8 available = (probe) ? 1 : 0; \
      SHARKSSL_ASM_INIT(&cached, available); \
   } \
   return SHARKSSL_ASM_LOAD(&cached); \
}

#if SHARKSSL_OPTIMIZED_CHACHA_ASM || SHARKSSL_OPTIMIZED_POLY1305_ASM
#if defined(_M_IX86)
int sharkssl_x86_avx2_available(void);
SHARKSSL_ASM_AVAILABILITY(sharkssl_x86_avx2Available, sharkssl_x86_avx2_available())
#else
int sharkssl_x86_64_avx2_available(void);
SHARKSSL_ASM_AVAILABILITY(sharkssl_x86_avx2Available, sharkssl_x86_64_avx2_available())
#endif
#endif

#if SHARKSSL_OPTIMIZED_GCM_ASM && SHARKSSL_OPTIMIZED_GHASH_ASM
#if defined(_M_IX86)
int sharkssl_x86_gcm_available(void);
SHARKSSL_ASM_AVAILABILITY(sharkssl_gcm_asmAvailable, sharkssl_x86_gcm_available())
#else
int sharkssl_x86_64_gcm_available(void);
SHARKSSL_ASM_AVAILABILITY(sharkssl_gcm_asmAvailable, sharkssl_x86_64_gcm_available())
#endif
#if SHARKSSL_X64_ASM_PLATFORM && SHARKSSL_OPTIMIZED_GCM_VAES_ASM
int sharkssl_x86_64_vaes_available(void);
SHARKSSL_ASM_AVAILABILITY(sharkssl_gcm_vaesAvailable, sharkssl_x86_64_vaes_available())
#endif
#endif
#undef SHARKSSL_ASM_AVAILABILITY
#undef SHARKSSL_ASM_LOAD
#undef SHARKSSL_ASM_INIT
#endif
#endif
