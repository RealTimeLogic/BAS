/* Copyright (c) Real Time Logic LLC, 2026. All rights reserved.
 * Original AArch64 kernels for SharkSSL. Same license as SharkSslCrypto.c.
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

/* Public lengths are multiples of 16. Counter is big-endian inc32.
 * Encrypt four independent counters per round for bulk input. GHASH retains
 * the existing table layout; a later measured experiment can fold blocks.
 * Decrypt hashes ciphertext before an in-place store overwrites it.
 */
.p2align 4
.global sharkssl_arm64_gcm_crypto
.hidden sharkssl_arm64_gcm_crypto
.type sharkssl_arm64_gcm_crypto, %function
sharkssl_arm64_gcm_crypto:
.cfi_startproc
    mov w7, #1
    b .Lgcm_enter
.cfi_endproc
.size sharkssl_arm64_gcm_crypto, .-sharkssl_arm64_gcm_crypto
.p2align 4
.global sharkssl_arm64_gcm_neon
.hidden sharkssl_arm64_gcm_neon
.type sharkssl_arm64_gcm_neon, %function
sharkssl_arm64_gcm_neon:
.cfi_startproc
    mov w7, #2
    b .Lgcm_enter
.cfi_endproc
.size sharkssl_arm64_gcm_neon, .-sharkssl_arm64_gcm_neon
.p2align 4
.global sharkssl_arm64_gcm_scalar
.hidden sharkssl_arm64_gcm_scalar
.type sharkssl_arm64_gcm_scalar, %function
sharkssl_arm64_gcm_scalar:
.cfi_startproc
    mov w7, #0
.cfi_endproc
.size sharkssl_arm64_gcm_scalar, .-sharkssl_arm64_gcm_scalar

.type .Lgcm_enter, %function
.Lgcm_enter:
.cfi_startproc
    cbz w5, .Lgcm_empty
    stp x29, x30, [sp, #-192]!
.cfi_def_cfa_offset 192
.cfi_offset 29, -192
.cfi_offset 30, -184
    mov x29, sp
    stp x19, x20, [sp, #16]
.cfi_offset 19, -176
.cfi_offset 20, -168
    stp x21, x22, [sp, #32]
.cfi_offset 21, -160
.cfi_offset 22, -152
    stp x23, x24, [sp, #48]
.cfi_offset 23, -144
.cfi_offset 24, -136
    stp x25, x26, [sp, #64]
.cfi_offset 25, -128
.cfi_offset 26, -120
    stp x27, x28, [sp, #80]
.cfi_offset 27, -112
.cfi_offset 28, -104

    mov x19, x0
    mov x20, x1
    mov x21, x2
    mov x22, x3
    mov x23, x4
    mov w24, w5
    mov w25, w6
    mov w26, w7
.Lgcm_loop:
    mov w27, #1
    cmp w26, #1
    b.ne .Lgcm_software
    cmp w24, #64
    b.lo .Lgcm_aes1
    mov w27, #4
    ldr q0, [x20]
    mov v1.16b, v0.16b
    mov v2.16b, v0.16b
    mov v3.16b, v0.16b
    ldr w9, [x20, #12]
    rev w9, w9
    add w9, w9, #1
    rev w10, w9
    ins v0.s[3], w10
    add w9, w9, #1
    rev w10, w9
    ins v1.s[3], w10
    add w9, w9, #1
    rev w10, w9
    ins v2.s[3], w10
    add w9, w9, #1
    rev w10, w9
    ins v3.s[3], w10
    str w10, [x20, #12]
    mov x9, x19
    ldrh w10, [x19, #(SHARKSSL_GCM_M0-4)]
    /* C stores rounds-1, or rounds/2-1 in the unrolled configuration. */
    add w10, w10, #1
#if !SHARKSSL_AES_SMALL_FOOTPRINT && SHARKSSL_AES_CIPHER_LOOP_UNROLL
    lsl w10, w10, #1
#endif
    sub w10, w10, #1
.Lgcm_aes4_round:
    ldr q4, [x9], #16
    rev32 v4.16b, v4.16b
    aese v0.16b, v4.16b
    aesmc v0.16b, v0.16b
    aese v1.16b, v4.16b
    aesmc v1.16b, v1.16b
    aese v2.16b, v4.16b
    aesmc v2.16b, v2.16b
    aese v3.16b, v4.16b
    aesmc v3.16b, v3.16b
    subs w10, w10, #1
    b.ne .Lgcm_aes4_round
    ldp q4, q5, [x9]
    rev32 v4.16b, v4.16b
    rev32 v5.16b, v5.16b
    aese v0.16b, v4.16b
    eor v0.16b, v0.16b, v5.16b
    aese v1.16b, v4.16b
    eor v1.16b, v1.16b, v5.16b
    aese v2.16b, v4.16b
    eor v2.16b, v2.16b, v5.16b
    aese v3.16b, v4.16b
    eor v3.16b, v3.16b, v5.16b
    stp q0, q1, [sp, #96]
    stp q2, q3, [sp, #128]
    b .Lgcm_xor_start
.Lgcm_aes1:
    ldr q0, [x20]
    ldr w9, [x20, #12]
    rev w9, w9
    add w9, w9, #1
    rev w9, w9
    str w9, [x20, #12]
    ins v0.s[3], w9
    mov x9, x19
    ldrh w10, [x19, #(SHARKSSL_GCM_M0-4)]
    add w10, w10, #1
#if !SHARKSSL_AES_SMALL_FOOTPRINT && SHARKSSL_AES_CIPHER_LOOP_UNROLL
    lsl w10, w10, #1
#endif
    sub w10, w10, #1
.Lgcm_aes1_round:
    ldr q4, [x9], #16
    rev32 v4.16b, v4.16b
    aese v0.16b, v4.16b
    aesmc v0.16b, v0.16b
    subs w10, w10, #1
    b.ne .Lgcm_aes1_round
    ldp q4, q5, [x9]
    rev32 v4.16b, v4.16b
    rev32 v5.16b, v5.16b
    aese v0.16b, v4.16b
    eor v0.16b, v0.16b, v5.16b
    str q0, [sp, #96]
    b .Lgcm_xor_start
.Lgcm_software:
    ldr w9, [x20, #12]
    rev w9, w9
    add w9, w9, #1
    rev w9, w9
    str w9, [x20, #12]
    mov x0, x19
    mov x1, x20
    add x2, sp, #96
    bl SharkSslAesCtx_encrypt
.Lgcm_xor_start:
    add x28, sp, #96
.Lgcm_xor:
    ldp x9, x10, [x22], #16
    ldp x11, x12, [x28], #16
    eor x11, x11, x9
    eor x12, x12, x10
    cmp w25, #1
    csel x9, x11, x9, eq
    csel x10, x12, x10, eq
    stp x11, x12, [x23], #16
    ldp x11, x12, [x21]
    eor x11, x11, x9
    eor x12, x12, x10
    stp x11, x12, [x21]
    add x0, x19, #SHARKSSL_GCM_M0
    mov x1, x21
    cbz w26, .Lgcm_hash_scalar
    cmp w26, #2
    b.eq .Lgcm_hash_neon
    bl sharkssl_arm64_gmult_pmull
    b .Lgcm_hashed
.Lgcm_hash_neon:
    bl sharkssl_arm64_gmult_neon
    b .Lgcm_hashed
.Lgcm_hash_scalar:
    bl sharkssl_arm64_gmult_scalar
.Lgcm_hashed:
    sub w24, w24, #16
    subs w27, w27, #1
    b.ne .Lgcm_xor
    cbnz w24, .Lgcm_loop
    stp xzr, xzr, [sp, #96]
    stp xzr, xzr, [sp, #112]
    stp xzr, xzr, [sp, #128]
    stp xzr, xzr, [sp, #144]
    ldp x19, x20, [sp, #16]
.cfi_restore 19
.cfi_restore 20
    ldp x21, x22, [sp, #32]
.cfi_restore 21
.cfi_restore 22
    ldp x23, x24, [sp, #48]
.cfi_restore 23
.cfi_restore 24
    ldp x25, x26, [sp, #64]
.cfi_restore 25
.cfi_restore 26
    ldp x27, x28, [sp, #80]
.cfi_restore 27
.cfi_restore 28
    ldp x29, x30, [sp], #192
.cfi_restore 29
.cfi_restore 30
.cfi_def_cfa_offset 0
.Lgcm_empty:
    ret
.cfi_endproc
.size .Lgcm_enter, .-.Lgcm_enter
#endif
.section .note.GNU-stack,"",%progbits

