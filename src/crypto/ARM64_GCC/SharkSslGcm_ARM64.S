/*
 **
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
 *   $Id: SharkSslGcm_ARM64.S 6818 2026-11-08 21:18:02Z gianluca $
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
 *
 */

#include "../../../inc/SharkSSL_cfg.h"
#if !defined(__aarch64__) || defined(__AARCH64EB__) || !defined(__LP64__)
#error "Requires little-endian AArch64 LP64"
#endif
.text
.arch armv8-a+crypto
#if SHARKSSL_ENABLE_AES_GCM && SHARKSSL_OPTIMIZED_GCM_ASM
#ifndef SHARKSSL_GCM_M0
#error "Pass the AES context M0 offset (180, 212, or 244)"
#endif
#define JOIN_(a,b) a##b
#define JOIN(a,b) JOIN_(a,b)
.global JOIN(sharkssl_arm64_gcm_layout_,SHARKSSL_GCM_M0)
.hidden JOIN(sharkssl_arm64_gcm_layout_,SHARKSSL_GCM_M0)
.type JOIN(sharkssl_arm64_gcm_layout_,SHARKSSL_GCM_M0), %function
JOIN(sharkssl_arm64_gcm_layout_,SHARKSSL_GCM_M0):
    ret

/* w10 = AES rounds - 1. C stores rounds-1, or rounds/2-1 in the unrolled
 * configuration. The count is public. Clobbers w10. */
.macro AES_ROUNDS1 ctx
    ldrh w10, [\ctx, #(SHARKSSL_GCM_M0-4)]
    add w10, w10, #1
#if !SHARKSSL_AES_SMALL_FOOTPRINT && SHARKSSL_AES_CIPHER_LOOP_UNROLL
    lsl w10, w10, #1
#endif
    sub w10, w10, #1
.endm

/* One AES block on a key schedule of big-endian words. Registers hold the
 * round keys and the block only; this replaces the bitsliced block in C when
 * the CPU has AES instructions. x0 = ctx, x1 = input, x2 = output. */
.p2align 4
.global sharkssl_arm64_aes_block
.hidden sharkssl_arm64_aes_block
.type sharkssl_arm64_aes_block, %function
sharkssl_arm64_aes_block:
.cfi_startproc
    ldr q0, [x1]
    AES_ROUNDS1 x0
.Laes_block_round:
    ldr q1, [x0], #16
    rev32 v1.16b, v1.16b
    aese v0.16b, v1.16b
    aesmc v0.16b, v0.16b
    subs w10, w10, #1
    b.ne .Laes_block_round
    ldp q1, q2, [x0]
    rev32 v1.16b, v1.16b
    rev32 v2.16b, v2.16b
    aese v0.16b, v1.16b
    eor v0.16b, v0.16b, v2.16b
    str q0, [x2]
    movi v0.16b, #0
    movi v1.16b, #0
    movi v2.16b, #0
    ret
.cfi_endproc
.size sharkssl_arm64_aes_block, .-sharkssl_arm64_aes_block

.macro AES4 k
    aese v0.16b, \k\().16b
    aesmc v0.16b, v0.16b
    aese v1.16b, \k\().16b
    aesmc v1.16b, v1.16b
    aese v2.16b, \k\().16b
    aesmc v2.16b, v2.16b
    aese v3.16b, \k\().16b
    aesmc v3.16b, v3.16b
.endm

.macro AES1 k
    aese v0.16b, \k\().16b
    aesmc v0.16b, v0.16b
.endm

/* out = a*b in the twisted field; am and bm hold hi^lo of a and b in their low
 * lanes. Clobbers v4-v7. */
.macro GF_MUL out, a, b, am, bm
    pmull v4.1q, \a\().1d, \b\().1d
    pmull2 v5.1q, \a\().2d, \b\().2d
    pmull v6.1q, \am\().1d, \bm\().1d
    GF_REDUCE \out
.endm

/* v4 = lo, v5 = hi, v6 = Karatsuba middle; reduces modulo the reflected GCM
 * polynomial, with v14 = 0xc2 << 56. Clobbers v4-v7. */
.macro GF_REDUCE out
    ext v7.16b, v4.16b, v5.16b, #8
    eor v6.16b, v6.16b, v7.16b
    eor v7.16b, v4.16b, v5.16b
    eor v6.16b, v6.16b, v7.16b
    pmull v7.1q, v4.1d, v14.1d
    ins v5.d[0], v6.d[1]
    ins v6.d[1], v4.d[0]
    eor v4.16b, v6.16b, v7.16b
    ext v7.16b, v4.16b, v4.16b, #8
    pmull v4.1q, v4.1d, v14.1d
    eor v7.16b, v7.16b, v5.16b
    eor \out\().16b, v4.16b, v7.16b
.endm

/* Hash one more ciphertext block held in \x: multiply by the power \h with
 * middle term in \lane of \hm, accumulating in v4-v6. Clobbers \x, v7, v15. */
.macro GH_NEXT x, h, mull, hm, lane
    rev64 \x\().16b, \x\().16b
    ext v15.16b, \x\().16b, \x\().16b, #8
    eor \x\().16b, \x\().16b, v15.16b
    pmull v7.1q, v15.1d, \h\().1d
    eor v4.16b, v4.16b, v7.16b
    pmull2 v7.1q, v15.2d, \h\().2d
    eor v5.16b, v5.16b, v7.16b
    \mull v7.1q, \x\().\lane, \hm\().\lane
    eor v6.16b, v6.16b, v7.16b
.endm

/* First block of a group: XOR in Xi (v31) and start the accumulators. */
.macro GH_FIRST x, h, mull, hm, lane
    rev64 \x\().16b, \x\().16b
    ext v15.16b, \x\().16b, \x\().16b, #8
    eor v15.16b, v15.16b, v31.16b
    ext \x\().16b, v15.16b, v15.16b, #8
    eor \x\().16b, \x\().16b, v15.16b
    pmull v4.1q, v15.1d, \h\().1d
    pmull2 v5.1q, v15.2d, \h\().2d
    \mull v6.1q, \x\().\lane, \hm\().\lane
.endm

/* AES-GCM over whole blocks with AES and PMULL instructions. Four counters per
 * pass, one GHASH reduction per pass. Round keys and the powers of H stay in
 * registers for the whole call, and are recomputed from M0[8] on every call, so
 * the context layout does not change and no key material reaches the stack.
 * Branches depend on the public length, direction and round count only.
 *   x0 ctx, x1 counter, x2 tag, x3 input, x4 output, w5 len (multiple of 16),
 *   w6 type; the input may be the output.
 *   v8..v11 = H, H^2, H^3, H^4 in the twisted form; v12 = hi^lo of H, of H^2;
 *   v13 = hi^lo of H^3, of H^4; v14 = 0xc2 << 56; v16..v30 = round keys;
 *   v31 = Xi, byte reversed. */
.p2align 4
.global sharkssl_arm64_gcm_crypto
.hidden sharkssl_arm64_gcm_crypto
.type sharkssl_arm64_gcm_crypto, %function
sharkssl_arm64_gcm_crypto:
.cfi_startproc
    cbz w5, .Lgcm_crypto_empty
    stp d8, d9, [sp, #-64]!
.cfi_def_cfa_offset 64
.cfi_offset 72, -64
.cfi_offset 73, -56
    stp d10, d11, [sp, #16]
.cfi_offset 74, -48
.cfi_offset 75, -40
    stp d12, d13, [sp, #32]
.cfi_offset 76, -32
.cfi_offset 77, -24
    stp d14, d15, [sp, #48]
.cfi_offset 78, -16
.cfi_offset 79, -8
    /* Round keys: keys 0..8 always, 9..10 for AES-192, 11..12 for AES-256;
     * the last two keys of the schedule go to v29 and v30. */
    AES_ROUNDS1 x0
    mov x9, x0
    ldp q16, q17, [x9], #32
    ldp q18, q19, [x9], #32
    ldp q20, q21, [x9], #32
    ldp q22, q23, [x9], #32
    ldr q24, [x9], #16
    rev32 v16.16b, v16.16b
    rev32 v17.16b, v17.16b
    rev32 v18.16b, v18.16b
    rev32 v19.16b, v19.16b
    rev32 v20.16b, v20.16b
    rev32 v21.16b, v21.16b
    rev32 v22.16b, v22.16b
    rev32 v23.16b, v23.16b
    rev32 v24.16b, v24.16b
    cmp w10, #9
    b.eq .Lgcm_keys_last
    ldp q25, q26, [x9], #32
    rev32 v25.16b, v25.16b
    rev32 v26.16b, v26.16b
    cmp w10, #11
    b.eq .Lgcm_keys_last
    ldp q27, q28, [x9], #32
    rev32 v27.16b, v27.16b
    rev32 v28.16b, v28.16b
.Lgcm_keys_last:
    ldp q29, q30, [x9]
    rev32 v29.16b, v29.16b
    rev32 v30.16b, v30.16b

    /* H from M0[8], words byte-swapped by C, then H << 1 mod the reflected
     * polynomial. */
    movi v14.16b, #0xe1
    shl v14.2d, v14.2d, #57
    add x9, x0, #(SHARKSSL_GCM_M0+128)
    ldr q8, [x9]
    rev32 v8.16b, v8.16b
    rev64 v8.16b, v8.16b
    ext v8.16b, v8.16b, v8.16b, #8
    ushr v7.2d, v14.2d, #63
    ext v5.16b, v7.16b, v14.16b, #8
    dup v6.4s, v8.s[3]
    ushr v7.2d, v8.2d, #63
    sshr v6.4s, v6.4s, #31
    and v7.16b, v7.16b, v5.16b
    shl v8.2d, v8.2d, #1
    ext v7.16b, v7.16b, v7.16b, #8
    and v5.16b, v5.16b, v6.16b
    orr v8.16b, v8.16b, v7.16b
    eor v8.16b, v8.16b, v5.16b
    ext v12.16b, v8.16b, v8.16b, #8
    eor v12.16b, v12.16b, v8.16b
    cmp w5, #64
    b.lo .Lgcm_powers_done
    GF_MUL v9, v8, v8, v12, v12
    ext v13.16b, v9.16b, v9.16b, #8
    eor v13.16b, v13.16b, v9.16b
    ins v12.d[1], v13.d[0]
    GF_MUL v10, v9, v8, v13, v12
    GF_MUL v11, v9, v9, v13, v13
    ext v13.16b, v10.16b, v10.16b, #8
    eor v13.16b, v13.16b, v10.16b
    ext v15.16b, v11.16b, v11.16b, #8
    eor v15.16b, v15.16b, v11.16b
    ins v13.d[1], v15.d[0]
.Lgcm_powers_done:
    ldr q31, [x2]
    rev64 v31.16b, v31.16b
    ext v31.16b, v31.16b, v31.16b, #8

    /* The counter is public: x11 = its first eight bytes, w12 = word 2, w13 =
     * the 32-bit big-endian counter in native order. */
    ldr x11, [x1]
    ldr w12, [x1, #8]
    ldr w13, [x1, #12]
    rev w13, w13
    cmp w5, #64
    b.lo .Lgcm_one_loop

.Lgcm_four_loop:
    add w13, w13, #1
    rev w14, w13
    orr x14, x12, x14, lsl #32
    fmov d0, x11
    ins v0.d[1], x14
    add w13, w13, #1
    rev w14, w13
    orr x14, x12, x14, lsl #32
    fmov d1, x11
    ins v1.d[1], x14
    add w13, w13, #1
    rev w14, w13
    orr x14, x12, x14, lsl #32
    fmov d2, x11
    ins v2.d[1], x14
    add w13, w13, #1
    rev w14, w13
    orr x14, x12, x14, lsl #32
    fmov d3, x11
    ins v3.d[1], x14
    AES4 v16
    AES4 v17
    AES4 v18
    AES4 v19
    AES4 v20
    AES4 v21
    AES4 v22
    AES4 v23
    AES4 v24
    cmp w10, #9
    b.eq .Lgcm_four_last
    AES4 v25
    AES4 v26
    cmp w10, #11
    b.eq .Lgcm_four_last
    AES4 v27
    AES4 v28
.Lgcm_four_last:
    aese v0.16b, v29.16b
    aese v1.16b, v29.16b
    aese v2.16b, v29.16b
    aese v3.16b, v29.16b
    ldp q4, q5, [x3], #32
    ldp q6, q7, [x3], #32
    eor v0.16b, v0.16b, v30.16b
    eor v1.16b, v1.16b, v30.16b
    eor v2.16b, v2.16b, v30.16b
    eor v3.16b, v3.16b, v30.16b
    eor v0.16b, v0.16b, v4.16b
    eor v1.16b, v1.16b, v5.16b
    eor v2.16b, v2.16b, v6.16b
    eor v3.16b, v3.16b, v7.16b
    stp q0, q1, [x4], #32
    stp q2, q3, [x4], #32
    cmp w6, #1
    b.eq .Lgcm_four_hash
    mov v0.16b, v4.16b
    mov v1.16b, v5.16b
    mov v2.16b, v6.16b
    mov v3.16b, v7.16b
.Lgcm_four_hash:
    GH_FIRST v0, v11, pmull2, v13, 2d
    GH_NEXT v1, v10, pmull, v13, 1d
    GH_NEXT v2, v9, pmull2, v12, 2d
    GH_NEXT v3, v8, pmull, v12, 1d
    GF_REDUCE v31
    sub w5, w5, #64
    cmp w5, #64
    b.hs .Lgcm_four_loop
    cbz w5, .Lgcm_store

.Lgcm_one_loop:
    add w13, w13, #1
    rev w14, w13
    orr x14, x12, x14, lsl #32
    fmov d0, x11
    ins v0.d[1], x14
    AES1 v16
    AES1 v17
    AES1 v18
    AES1 v19
    AES1 v20
    AES1 v21
    AES1 v22
    AES1 v23
    AES1 v24
    cmp w10, #9
    b.eq .Lgcm_one_last
    AES1 v25
    AES1 v26
    cmp w10, #11
    b.eq .Lgcm_one_last
    AES1 v27
    AES1 v28
.Lgcm_one_last:
    aese v0.16b, v29.16b
    ldr q4, [x3], #16
    eor v0.16b, v0.16b, v30.16b
    eor v0.16b, v0.16b, v4.16b
    str q0, [x4], #16
    cmp w6, #1
    b.eq .Lgcm_one_hash
    mov v0.16b, v4.16b
.Lgcm_one_hash:
    GH_FIRST v0, v8, pmull, v12, 1d
    GF_REDUCE v31
    subs w5, w5, #16
    b.ne .Lgcm_one_loop

.Lgcm_store:
    rev w14, w13
    str w14, [x1, #12]
    rev64 v31.16b, v31.16b
    ext v31.16b, v31.16b, v31.16b, #8
    str q31, [x2]
    movi v0.16b, #0
    movi v1.16b, #0
    movi v2.16b, #0
    movi v3.16b, #0
    movi v4.16b, #0
    movi v5.16b, #0
    movi v6.16b, #0
    movi v7.16b, #0
    movi v8.16b, #0
    movi v9.16b, #0
    movi v10.16b, #0
    movi v11.16b, #0
    movi v12.16b, #0
    movi v13.16b, #0
    movi v14.16b, #0
    movi v15.16b, #0
    movi v16.16b, #0
    movi v17.16b, #0
    movi v18.16b, #0
    movi v19.16b, #0
    movi v20.16b, #0
    movi v21.16b, #0
    movi v22.16b, #0
    movi v23.16b, #0
    movi v24.16b, #0
    movi v25.16b, #0
    movi v26.16b, #0
    movi v27.16b, #0
    movi v28.16b, #0
    movi v29.16b, #0
    movi v30.16b, #0
    movi v31.16b, #0
    ldp d10, d11, [sp, #16]
.cfi_restore 74
.cfi_restore 75
    ldp d12, d13, [sp, #32]
.cfi_restore 76
.cfi_restore 77
    ldp d14, d15, [sp, #48]
.cfi_restore 78
.cfi_restore 79
    ldp d8, d9, [sp], #64
.cfi_restore 72
.cfi_restore 73
.cfi_def_cfa_offset 0
.Lgcm_crypto_empty:
    ret
.cfi_endproc
.size sharkssl_arm64_gcm_crypto, .-sharkssl_arm64_gcm_crypto

#endif
.section .note.GNU-stack,"",%progbits
