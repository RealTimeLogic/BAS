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
 *   $Id: SharkSslCrypto_ARM64_Gcm.S 6629 2026-10-26 19:43:26Z gianluca $
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

/* Public lengths are multiples of 16. Counter is big-endian inc32.
 * Encrypt four independent counters per round and fold their ciphertext
 * into one GHASH reduction. Decrypt stages ciphertext before an in-place
 * store overwrites it. Short input uses the original single-block path.
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
    stp x29, x30, [sp, #-320]!
.cfi_def_cfa_offset 320
.cfi_offset 29, -320
.cfi_offset 30, -312
    mov x29, sp
    stp x19, x20, [sp, #16]
.cfi_offset 19, -304
.cfi_offset 20, -296
    stp x21, x22, [sp, #32]
.cfi_offset 21, -288
.cfi_offset 22, -280
    stp x23, x24, [sp, #48]
.cfi_offset 23, -272
.cfi_offset 24, -264
    stp x25, x26, [sp, #64]
.cfi_offset 25, -256
.cfi_offset 26, -248
    stp x27, x28, [sp, #80]
.cfi_offset 27, -240
.cfi_offset 28, -232

    mov x19, x0
    mov x20, x1
    mov x21, x2
    mov x22, x3
    mov x23, x4
    mov w24, w5
    mov w25, w6
    mov w26, w7
    cmp w26, #1
    b.ne .Lgcm_loop
    cmp w24, #128
    b.lo .Lgcm_loop
    /* M0[8] is H with each 32-bit word byte-swapped. Powers are kept in
     * ordinary tag byte order so the PMULL helper can reverse their bits. */
    add x9, x19, #SHARKSSL_GCM_M0
    ldr q0, [x9, #128]
    rev32 v0.16b, v0.16b
    str q0, [sp, #272]
    str q0, [sp, #256]
    add x0, x19, #SHARKSSL_GCM_M0
    add x1, sp, #256
    bl sharkssl_arm64_gmult_pmull
    ldr q0, [sp, #256]
    str q0, [sp, #240]
    add x0, x19, #SHARKSSL_GCM_M0
    add x1, sp, #240
    bl sharkssl_arm64_gmult_pmull
    ldr q0, [sp, #240]
    str q0, [sp, #224]
    add x0, x19, #SHARKSSL_GCM_M0
    add x1, sp, #224
    bl sharkssl_arm64_gmult_pmull
    /* Mode 5 records that all four powers are ready on this stack frame. */
    orr w26, w26, #4
.Lgcm_loop:
    mov w27, #1
    cmp w26, #5
    b.ne .Lgcm_not_bulk
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
    b .Lgcm_xor4_start
.Lgcm_not_bulk:
    cmp w26, #1
    b.eq .Lgcm_aes1
    b .Lgcm_software
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
    b .Lgcm_done
.Lgcm_xor4_start:
    add x28, sp, #96
    add x8, sp, #160
.Lgcm_xor4:
    ldp x9, x10, [x22], #16
    ldp x11, x12, [x28], #16
    eor x11, x11, x9
    eor x12, x12, x10
    cmp w25, #1
    csel x9, x11, x9, eq
    csel x10, x12, x10, eq
    stp x9, x10, [x8], #16
    stp x11, x12, [x23], #16
    subs w27, w27, #1
    b.ne .Lgcm_xor4
    add x0, sp, #224
    mov x1, x21
    add x2, sp, #160
    bl sharkssl_arm64_gmult4_pmull
    sub w24, w24, #64
    cbnz w24, .Lgcm_loop
    b .Lgcm_done
.Lgcm_done:
    movi v0.16b, #0
    tbz w26, #2, .Lgcm_wipe_short
    stp q0, q0, [sp, #96]
    stp q0, q0, [sp, #128]
    stp q0, q0, [sp, #160]
    stp q0, q0, [sp, #192]
    stp q0, q0, [sp, #224]
    stp q0, q0, [sp, #256]
    b .Lgcm_wiped
.Lgcm_wipe_short:
    str q0, [sp, #96]
.Lgcm_wiped:
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
    ldp x29, x30, [sp], #320
.cfi_restore 29
.cfi_restore 30
.cfi_def_cfa_offset 0
.Lgcm_empty:
    ret
.cfi_endproc
.size .Lgcm_enter, .-.Lgcm_enter
#endif
.section .note.GNU-stack,"",%progbits

