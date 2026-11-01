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
 *   $Id: SharkSslCrypto_ARM64_Sha256.c 6686 2026-10-29 23:59:57Z wini $
 *
 *   COPYRIGHT:  Real Time Logic LLC, 2026
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

/* AArch64 SHA-256 block compression using the optional SHA2 instructions.
 * Compile with -march=armv8-a+crypto. GCC 11 groups SHA intrinsics under +crypto.
 */
#include "SharkSslCrypto.h"
#include <arm_neon.h>

#if !defined(__aarch64__) || defined(__AARCH64EB__) || !defined(__LP64__)
#error Requires little-endian AArch64 LP64
#endif

#if (SHARKSSL_USE_SHA_256 && SHARKSSL_ARM64_SHA256_INTRINSICS && (!SHARKSSL_PSA_CONSUMER))
/**
 * Process four SHA-256 rounds using one vector of message words.
 */
#define SHARKSSL_SHA256_ROUND(schedule, k)                                                         \
   do                                                                                              \
   {                                                                                               \
      old_abcd = abcd;                                                                             \
      round_input = vaddq_u32((schedule), vld1q_u32(constants + (k)));                             \
      abcd = vsha256hq_u32(old_abcd, efgh, round_input);                                           \
      efgh = vsha256h2q_u32(efgh, old_abcd, round_input);                                          \
   } while (0)

void SharkSslSha256Ctx_process_arm64(SharkSslSha256Ctx *ctx, const U8 data[64], const U32 *constants)
{
   uint32x4_t w0, w1, w2, w3;
   uint32x4_t abcd, efgh, old_abcd, round_input;
   unsigned i;

   w0 = vreinterpretq_u32_u8(vrev32q_u8(vld1q_u8(data)));
   w1 = vreinterpretq_u32_u8(vrev32q_u8(vld1q_u8(data + 16)));
   w2 = vreinterpretq_u32_u8(vrev32q_u8(vld1q_u8(data + 32)));
   w3 = vreinterpretq_u32_u8(vrev32q_u8(vld1q_u8(data + 48)));
   abcd = vld1q_u32(ctx->state);
   efgh = vld1q_u32(ctx->state + 4);
   for (i = 0; i < 4; ++i)
   {
      SHARKSSL_SHA256_ROUND(w0, 16 * i);
      SHARKSSL_SHA256_ROUND(w1, 16 * i + 4);
      SHARKSSL_SHA256_ROUND(w2, 16 * i + 8);
      SHARKSSL_SHA256_ROUND(w3, 16 * i + 12);
      if (i != 3)
      {
         w0 = vsha256su1q_u32(vsha256su0q_u32(w0, w1), w2, w3);
         w1 = vsha256su1q_u32(vsha256su0q_u32(w1, w2), w3, w0);
         w2 = vsha256su1q_u32(vsha256su0q_u32(w2, w3), w0, w1);
         w3 = vsha256su1q_u32(vsha256su0q_u32(w3, w0), w1, w2);
      }
   }
   vst1q_u32(ctx->state, vaddq_u32(vld1q_u32(ctx->state), abcd));
   vst1q_u32(ctx->state + 4, vaddq_u32(vld1q_u32(ctx->state + 4), efgh));
}
#undef SHARKSSL_SHA256_ROUND
#endif
