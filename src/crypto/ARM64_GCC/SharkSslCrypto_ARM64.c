/* Copyright (c) Real Time Logic LLC, 2026. All rights reserved.
 * AArch64 Linux/QNX dispatch for the existing SharkSSL assembly hooks.
 * Same license as SharkSslCrypto.c. Compile for baseline armv8-a.
 */
#include "SharkSslCrypto.h"
#include <stddef.h>
#if defined(__QNXNTO__)
#include <sys/syspage.h>
#else
#include <sys/auxv.h>
#include <asm/hwcap.h>
#endif

#if !defined(__aarch64__) || (!defined(__linux__) && !defined(__QNXNTO__)) || defined(__ANDROID__) || !defined(__ELF__) || \
    !defined(__LP64__) || defined(__ILP32__) || defined(__AARCH64EB__) || \
    !defined(B_LITTLE_ENDIAN) || defined(B_BIG_ENDIAN)
#error "ARM64_GCC requires little-endian AArch64 Linux/QNX LP64/ELF"
#endif

/* The shared small-footprint AES rotates by zero with a shift-by-32 C
 * expression. Clang fails independent AES vectors even without assembly.
 * Do not accept that configuration until the common implementation is fixed. */
#if SHARKSSL_ENABLE_AES_GCM && SHARKSSL_AES_SMALL_FOOTPRINT && \
    (SHARKSSL_OPTIMIZED_GCM_ASM || SHARKSSL_OPTIMIZED_GHASH_ASM)
#error "ARM64_GCC AES-GCM requires SHARKSSL_AES_SMALL_FOOTPRINT=0"
#endif

/* No key data is published: relaxed atomics suffice for this immutable mask. */
unsigned sharkssl_arm64_capabilities(void)
{
   static unsigned cached;
   unsigned caps = __atomic_load_n(&cached, __ATOMIC_RELAXED);
   if (!caps)
   {
#if defined(__QNXNTO__)
      unsigned cpu;
      unsigned hw = 0;
      /* Intersect all CPUs: callers may migrate after this result is cached.
       * Use the runtime array stride, not this SDK's cpuinfo structure size. */
      for (cpu = 0; cpu < _syspage_ptr->num_cpu; ++cpu)
      {
         unsigned flags = SYSPAGE_ARRAY_IDX(cpuinfo, cpu)->flags;
         hw = cpu ? (hw & flags) : flags;
      }
      caps = 1;
      if (hw & AARCH64_CPU_FLAG_SIMD)
         caps |= 2;
#if defined(AARCH64_CPU_FLAG_AES) && defined(AARCH64_CPU_FLAG_PMULL)
      if ((hw & (AARCH64_CPU_FLAG_SIMD | AARCH64_CPU_FLAG_AES | AARCH64_CPU_FLAG_PMULL)) ==
                (AARCH64_CPU_FLAG_SIMD | AARCH64_CPU_FLAG_AES | AARCH64_CPU_FLAG_PMULL))
         caps |= 4;
#endif
#else
      unsigned long hw = getauxval(AT_HWCAP);
      caps = 1; /* initialized, even when no accelerated instructions exist */
      if (hw & HWCAP_ASIMD)
         caps |= 2;
      if ((hw & (HWCAP_ASIMD | HWCAP_AES | HWCAP_PMULL)) ==
                (HWCAP_ASIMD | HWCAP_AES | HWCAP_PMULL))
         caps |= 4;
#endif
      __atomic_store_n(&cached, caps, __ATOMIC_RELAXED);
   }
   return caps;
}

#if SHARKSSL_USE_CHACHA20 && SHARKSSL_OPTIMIZED_CHACHA_ASM
typedef char arm64_chacha_layout[(offsetof(SharkSslChaChaCtx, state) == 0) ? 1 : -1];
void sharkssl_arm64_chacha_neon(SharkSslChaChaCtx*, const U8*, U8*, U32);
void sharkssl_arm64_chacha_scalar(SharkSslChaChaCtx*, const U8*, U8*, U32);
SHARKSSL_API void SharkSslChaChaCtx_crypt(SharkSslChaChaCtx *ctx,
                                       const U8 *in, U8 *out, U32 len)
{
   if (sharkssl_arm64_capabilities() & 2)
      sharkssl_arm64_chacha_neon(ctx, in, out, len);
   else
      sharkssl_arm64_chacha_scalar(ctx, in, out, len);
}
#endif

#if SHARKSSL_USE_POLY1305 && SHARKSSL_OPTIMIZED_POLY1305_ASM
typedef char arm64_poly_r[(offsetof(SharkSslPoly1305Ctx, r) == 0) ? 1 : -1];
typedef char arm64_poly_key[(offsetof(SharkSslPoly1305Ctx, key) == 20) ? 1 : -1];
typedef char arm64_poly_flag[(offsetof(SharkSslPoly1305Ctx, flag) == 68) ? 1 : -1];
#endif

#if SHARKSSL_ENABLE_AES_GCM
#if SHARKSSL_OPTIMIZED_GCM_ASM
#ifndef SHARKSSL_GCM_M0
#if SHARKSSL_USE_AES_256 || SHARKSSL_NOPACK
#define SHARKSSL_GCM_M0 244
#elif SHARKSSL_USE_AES_192
#define SHARKSSL_GCM_M0 212
#else
#define SHARKSSL_GCM_M0 180
#endif
#endif
typedef char arm64_gcm_m0[(offsetof(SharkSslAesGcmCtx, M0) == SHARKSSL_GCM_M0) ? 1 : -1];
typedef char arm64_gcm_nr[(offsetof(SharkSslAesGcmCtx, super.nr) == SHARKSSL_GCM_M0-4) ? 1 : -1];
typedef char arm64_gcm_type[(SharkSslAesCtx_Encrypt == 1) ? 1 : -1];
#define ARM64_JOIN_(a,b) a##b
#define ARM64_JOIN(a,b) ARM64_JOIN_(a,b)
void ARM64_JOIN(sharkssl_arm64_gcm_layout_,SHARKSSL_GCM_M0)(void);
void sharkssl_arm64_gcm_crypto(SharkSslAesGcmCtx*, U8*, U8*, const U8*, U8*, U32, SharkSslAesCtx_Type);
void sharkssl_arm64_gcm_scalar(SharkSslAesGcmCtx*, U8*, U8*, const U8*, U8*, U32, SharkSslAesCtx_Type);
void sharkssl_arm64_gcm_neon(SharkSslAesGcmCtx*, U8*, U8*, const U8*, U8*, U32, SharkSslAesCtx_Type);
void sharkssl_gcm_crypt_blocks(SharkSslAesGcmCtx *ctx, U8 *ctr, U8 *tag,
                             const U8 *in, U8 *out, U32 len, SharkSslAesCtx_Type type)
{
   ARM64_JOIN(sharkssl_arm64_gcm_layout_,SHARKSSL_GCM_M0)();
   if (sharkssl_arm64_capabilities() & 4)
      sharkssl_arm64_gcm_crypto(ctx, ctr, tag, in, out, len, type);
   else if (sharkssl_arm64_capabilities() & 2)
      sharkssl_arm64_gcm_neon(ctx, ctr, tag, in, out, len, type);
   else
      sharkssl_arm64_gcm_scalar(ctx, ctr, tag, in, out, len, type);
}
#endif
#if SHARKSSL_OPTIMIZED_GHASH_ASM
void sharkssl_arm64_gmult_pmull(U8 (*)[16], U8*);
void sharkssl_arm64_gmult_scalar(U8 (*)[16], U8*);
void sharkssl_arm64_gmult_neon(U8 (*)[16], U8*);
void sharkssl_gcm_gmult32M0(U8 (*table)[16], U8 *tag)
{
   if (sharkssl_arm64_capabilities() & 4)
      sharkssl_arm64_gmult_pmull(table, tag);
   else if (sharkssl_arm64_capabilities() & 2)
      sharkssl_arm64_gmult_neon(table, tag);
   else
      sharkssl_arm64_gmult_scalar(table, tag);
}
#endif
#endif
