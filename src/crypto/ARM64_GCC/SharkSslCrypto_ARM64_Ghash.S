/* Copyright (c) Real Time Logic LLC, 2026. All rights reserved.
 * Original AArch64 kernels for SharkSSL. Same license as SharkSslCrypto.c.
 */
#include "../../../inc/SharkSSL_cfg.h"
#if !defined(__aarch64__) || defined(__AARCH64EB__) || !defined(__LP64__)
#error "Requires little-endian AArch64 LP64"
#endif
.text
.arch armv8-a+crypto
#if SHARKSSL_ENABLE_AES_GCM && (SHARKSSL_OPTIMIZED_GHASH_ASM || SHARKSSL_OPTIMIZED_GCM_ASM)
/* M0[8] stores H as four native-endian words. Never modify the C table.
 * RBIT maps the GHASH bit convention to little-endian polynomial limbs.
 * Four products yield L + x^128 H. Fold H*(1+x+x^2+x^7), then its
 * at-most-seven-bit overflow, modulo x^128+x^7+x^2+x+1.
 */
.p2align 4
.global sharkssl_arm64_gmult_pmull
.hidden sharkssl_arm64_gmult_pmull
.type sharkssl_arm64_gmult_pmull, %function
sharkssl_arm64_gmult_pmull:
.cfi_startproc

    ldr q0, [x1]
    ldr q1, [x0, #128]
    rev32 v1.16b, v1.16b
    rbit v0.16b, v0.16b
    rbit v1.16b, v1.16b
    ext v2.16b, v1.16b, v1.16b, #8
    pmull v3.1q, v0.1d, v1.1d
    pmull2 v4.1q, v0.2d, v1.2d
    pmull v5.1q, v0.1d, v2.1d
    pmull2 v6.1q, v0.2d, v2.2d
    eor v5.16b, v5.16b, v6.16b
    umov x2, v3.d[0]
    umov x3, v3.d[1]
    umov x4, v4.d[0]
    umov x5, v4.d[1]
    umov x6, v5.d[0]
    umov x7, v5.d[1]
    eor x3, x3, x6
    eor x4, x4, x7
    eor x2, x2, x4
    eor x2, x2, x4, lsl #1
    eor x2, x2, x4, lsl #2
    eor x2, x2, x4, lsl #7
    eor x3, x3, x5
    eor x3, x3, x5, lsl #1
    eor x3, x3, x5, lsl #2
    eor x3, x3, x5, lsl #7
    eor x3, x3, x4, lsr #63
    eor x3, x3, x4, lsr #62
    eor x3, x3, x4, lsr #57
    lsr x6, x5, #63
    eor x6, x6, x5, lsr #62
    eor x6, x6, x5, lsr #57
    eor x2, x2, x6
    eor x2, x2, x6, lsl #1
    eor x2, x2, x6, lsl #2
    eor x2, x2, x6, lsl #7
    fmov d0, x2
    ins v0.d[1], x3
    rbit v0.16b, v0.16b
    str q0, [x1]
    ret
.cfi_endproc
.size sharkssl_arm64_gmult_pmull, .-sharkssl_arm64_gmult_pmull

/* ASIMD-only polynomial multiplication, with no crypto extension or lookup.
 * Split each byte into nibbles. PMUL returns the complete 4x4-bit product;
 * three such products reconstruct each 8x8-bit product. Sixteen fixed Horner
 * steps accumulate the 256-bit product, then use the same field reduction.
 * All temporary vectors are caller-saved. Memory addresses are key-independent.
 */
.p2align 4
.global sharkssl_arm64_gmult_neon
.hidden sharkssl_arm64_gmult_neon
.type sharkssl_arm64_gmult_neon, %function
sharkssl_arm64_gmult_neon:
.cfi_startproc
    ldr q0, [x0, #128]
    rev32 v0.16b, v0.16b
    rbit v0.16b, v0.16b
    movi v7.16b, #15
    ushr v1.16b, v0.16b, #4
    and v0.16b, v0.16b, v7.16b
    eor v2.16b, v0.16b, v1.16b
    ldr q3, [x1]
    rbit v3.16b, v3.16b
    movi v4.16b, #0
    movi v5.16b, #0
    movi v6.16b, #0
    mov w2, #16
.Lghash_bytes:
    dup v16.16b, v3.b[15]
    ext v3.16b, v6.16b, v3.16b, #15
    ushr v17.16b, v16.16b, #4
    and v16.16b, v16.16b, v7.16b
    eor v18.16b, v16.16b, v17.16b
    pmul v19.16b, v0.16b, v16.16b
    pmul v20.16b, v1.16b, v17.16b
    pmul v21.16b, v2.16b, v18.16b
    eor v21.16b, v21.16b, v19.16b
    eor v21.16b, v21.16b, v20.16b
    shl v22.16b, v21.16b, #4
    ushr v21.16b, v21.16b, #4
    eor v19.16b, v19.16b, v22.16b
    eor v20.16b, v20.16b, v21.16b
    ext v5.16b, v4.16b, v5.16b, #15
    ext v4.16b, v6.16b, v4.16b, #15
    ext v22.16b, v6.16b, v20.16b, #15
    ext v20.16b, v20.16b, v6.16b, #15
    eor v4.16b, v4.16b, v19.16b
    eor v4.16b, v4.16b, v22.16b
    eor v5.16b, v5.16b, v20.16b
    subs w2, w2, #1
    b.ne .Lghash_bytes
    umov x2, v4.d[0]
    umov x3, v4.d[1]
    umov x4, v5.d[0]
    umov x5, v5.d[1]
    eor x2, x2, x4
    eor x2, x2, x4, lsl #1
    eor x2, x2, x4, lsl #2
    eor x2, x2, x4, lsl #7
    eor x3, x3, x5
    eor x3, x3, x5, lsl #1
    eor x3, x3, x5, lsl #2
    eor x3, x3, x5, lsl #7
    eor x3, x3, x4, lsr #63
    eor x3, x3, x4, lsr #62
    eor x3, x3, x4, lsr #57
    lsr x6, x5, #63
    eor x6, x6, x5, lsr #62
    eor x6, x6, x5, lsr #57
    eor x2, x2, x6
    eor x2, x2, x6, lsl #1
    eor x2, x2, x6, lsl #2
    eor x2, x2, x6, lsl #7
    fmov d0, x2
    ins v0.d[1], x3
    rbit v0.16b, v0.16b
    str q0, [x1]
    ret
.cfi_endproc
.size sharkssl_arm64_gmult_neon, .-sharkssl_arm64_gmult_neon

/* Fixed 128 iterations; no secret-indexed table accesses or branches.
 * Used when Linux does not expose ASIMD.
 */
.p2align 4
.global sharkssl_arm64_gmult_scalar
.hidden sharkssl_arm64_gmult_scalar
.type sharkssl_arm64_gmult_scalar, %function
sharkssl_arm64_gmult_scalar:
.cfi_startproc

    ldp x2, x3, [x1]
    rev x2, x2
    rev x3, x3
    ldp w4, w5, [x0, #128]
    orr x4, x5, x4, lsl #32
    ldp w6, w7, [x0, #136]
    orr x6, x7, x6, lsl #32
    mov x8, #0
    mov x9, #0
    mov x12, #0xe100000000000000
    mov w13, #128
.Lghash_bits:
    asr x10, x2, #63
    and x11, x4, x10
    eor x8, x8, x11
    and x11, x6, x10
    eor x9, x9, x11
    and x10, x6, #1
    neg x10, x10
    and x10, x10, x12
    extr x6, x4, x6, #1
    lsr x4, x4, #1
    eor x4, x4, x10
    extr x2, x2, x3, #63
    lsl x3, x3, #1
    subs w13, w13, #1
    b.ne .Lghash_bits
    rev x8, x8
    rev x9, x9
    stp x8, x9, [x1]
    ret
.cfi_endproc
.size sharkssl_arm64_gmult_scalar, .-sharkssl_arm64_gmult_scalar
#endif
.section .note.GNU-stack,"",%progbits

