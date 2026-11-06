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
 *   $Id: SharkSslP256_ARM64.S 6737 2026-11-04 05:37:01Z gianluca $
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
.arch armv8-a
#if SHARKSSL_ECC_P256_ASM
/* P-256 field arithmetic for AArch64, with the contract of SharkSslP256_M4.S.
 *   void SharkSslP256_mulmod(const SharkSslBigInt *a, const SharkSslBigInt *b, SharkSslBigInt *r);
 *   void SharkSslP256_addmod(SharkSslBigInt *a, const SharkSslBigInt *b);
 *   void SharkSslP256_submod(SharkSslBigInt *a, const SharkSslBigInt *b);
 * Operands are eight 32-bit words, most significant first, below p; results are
 * fully reduced; r->beg = r->mem + 8, r->len = 8, and r must not be a or b. No
 * branch or address depends on a value.
 * Comba columns use 32x32-bit products: one cycle each on Cortex-A76, against seven
 * for a MUL and UMULH pair. Each output word of the reduction is the signed sum of
 * NIST's formula in a 64-bit register. The carry above 2^256 folds once by
 * 2^256 = 2^224 - 2^192 - 2^96 + 1, which leaves -p < v < 2p for the two masked
 * corrections. The stack frame holds the product only while the routine runs. */
.p2align 4
.global SharkSslP256_mulmod
.hidden SharkSslP256_mulmod
.type SharkSslP256_mulmod, %function
SharkSslP256_mulmod:
.cfi_startproc
    stp x29, x30, [sp, #-176]!
.cfi_def_cfa_offset 176
.cfi_offset 29, -176
.cfi_offset 30, -168
    mov x29, sp
    stp x19, x20, [sp, #16]
.cfi_offset 19, -160
.cfi_offset 20, -152
    stp x21, x22, [sp, #32]
.cfi_offset 21, -144
.cfi_offset 22, -136
    stp x23, x24, [sp, #48]
.cfi_offset 23, -128
.cfi_offset 24, -120
    stp x25, x26, [sp, #64]
.cfi_offset 25, -112
.cfi_offset 26, -104
    stp x27, x28, [sp, #80]
.cfi_offset 27, -96
.cfi_offset 28, -88
    str x2, [sp, #96]
    ldr x2, [x0, #8]
    ldr x0, [x1, #8]
    ldp w4, w3, [x2, #24]
    ldp w12, w11, [x0, #24]
    ldp w6, w5, [x2, #16]
    ldp w14, w13, [x0, #16]
    ldp w8, w7, [x2, #8]
    ldp w16, w15, [x0, #8]
    ldp w10, w9, [x2, #0]
    ldp w19, w17, [x0, #0]
    mov x20, xzr
    mov x1, xzr
    umull x2, w3, w11
    adds x20, x20, x2
    adc x1, x1, xzr
    str w20, [sp, #112]
    extr x20, x1, x20, #32
    lsr x1, x1, #32
    umull x2, w3, w12
    adds x20, x20, x2
    adc x1, x1, xzr
    umull x2, w4, w11
    adds x20, x20, x2
    adc x1, x1, xzr
    str w20, [sp, #116]
    extr x20, x1, x20, #32
    lsr x1, x1, #32
    umull x2, w3, w13
    adds x20, x20, x2
    adc x1, x1, xzr
    umull x2, w4, w12
    adds x20, x20, x2
    adc x1, x1, xzr
    umull x2, w5, w11
    adds x20, x20, x2
    adc x1, x1, xzr
    str w20, [sp, #120]
    extr x20, x1, x20, #32
    lsr x1, x1, #32
    umull x2, w3, w14
    adds x20, x20, x2
    adc x1, x1, xzr
    umull x2, w4, w13
    adds x20, x20, x2
    adc x1, x1, xzr
    umull x2, w5, w12
    adds x20, x20, x2
    adc x1, x1, xzr
    umull x2, w6, w11
    adds x20, x20, x2
    adc x1, x1, xzr
    str w20, [sp, #124]
    extr x20, x1, x20, #32
    lsr x1, x1, #32
    umull x2, w3, w15
    adds x20, x20, x2
    adc x1, x1, xzr
    umull x2, w4, w14
    adds x20, x20, x2
    adc x1, x1, xzr
    umull x2, w5, w13
    adds x20, x20, x2
    adc x1, x1, xzr
    umull x2, w6, w12
    adds x20, x20, x2
    adc x1, x1, xzr
    umull x2, w7, w11
    adds x20, x20, x2
    adc x1, x1, xzr
    str w20, [sp, #128]
    extr x20, x1, x20, #32
    lsr x1, x1, #32
    umull x2, w3, w16
    adds x20, x20, x2
    adc x1, x1, xzr
    umull x2, w4, w15
    adds x20, x20, x2
    adc x1, x1, xzr
    umull x2, w5, w14
    adds x20, x20, x2
    adc x1, x1, xzr
    umull x2, w6, w13
    adds x20, x20, x2
    adc x1, x1, xzr
    umull x2, w7, w12
    adds x20, x20, x2
    adc x1, x1, xzr
    umull x2, w8, w11
    adds x20, x20, x2
    adc x1, x1, xzr
    str w20, [sp, #132]
    extr x20, x1, x20, #32
    lsr x1, x1, #32
    umull x2, w3, w17
    adds x20, x20, x2
    adc x1, x1, xzr
    umull x2, w4, w16
    adds x20, x20, x2
    adc x1, x1, xzr
    umull x2, w5, w15
    adds x20, x20, x2
    adc x1, x1, xzr
    umull x2, w6, w14
    adds x20, x20, x2
    adc x1, x1, xzr
    umull x2, w7, w13
    adds x20, x20, x2
    adc x1, x1, xzr
    umull x2, w8, w12
    adds x20, x20, x2
    adc x1, x1, xzr
    umull x2, w9, w11
    adds x20, x20, x2
    adc x1, x1, xzr
    str w20, [sp, #136]
    extr x20, x1, x20, #32
    lsr x1, x1, #32
    umull x2, w3, w19
    adds x20, x20, x2
    adc x1, x1, xzr
    umull x2, w4, w17
    adds x20, x20, x2
    adc x1, x1, xzr
    umull x2, w5, w16
    adds x20, x20, x2
    adc x1, x1, xzr
    umull x2, w6, w15
    adds x20, x20, x2
    adc x1, x1, xzr
    umull x2, w7, w14
    adds x20, x20, x2
    adc x1, x1, xzr
    umull x2, w8, w13
    adds x20, x20, x2
    adc x1, x1, xzr
    umull x2, w9, w12
    adds x20, x20, x2
    adc x1, x1, xzr
    umull x2, w10, w11
    adds x20, x20, x2
    adc x1, x1, xzr
    str w20, [sp, #140]
    extr x20, x1, x20, #32
    lsr x1, x1, #32
    umull x2, w4, w19
    adds x20, x20, x2
    adc x1, x1, xzr
    umull x2, w5, w17
    adds x20, x20, x2
    adc x1, x1, xzr
    umull x2, w6, w16
    adds x20, x20, x2
    adc x1, x1, xzr
    umull x2, w7, w15
    adds x20, x20, x2
    adc x1, x1, xzr
    umull x2, w8, w14
    adds x20, x20, x2
    adc x1, x1, xzr
    umull x2, w9, w13
    adds x20, x20, x2
    adc x1, x1, xzr
    umull x2, w10, w12
    adds x20, x20, x2
    adc x1, x1, xzr
    str w20, [sp, #144]
    extr x20, x1, x20, #32
    lsr x1, x1, #32
    umull x2, w5, w19
    adds x20, x20, x2
    adc x1, x1, xzr
    umull x2, w6, w17
    adds x20, x20, x2
    adc x1, x1, xzr
    umull x2, w7, w16
    adds x20, x20, x2
    adc x1, x1, xzr
    umull x2, w8, w15
    adds x20, x20, x2
    adc x1, x1, xzr
    umull x2, w9, w14
    adds x20, x20, x2
    adc x1, x1, xzr
    umull x2, w10, w13
    adds x20, x20, x2
    adc x1, x1, xzr
    str w20, [sp, #148]
    extr x20, x1, x20, #32
    lsr x1, x1, #32
    umull x2, w6, w19
    adds x20, x20, x2
    adc x1, x1, xzr
    umull x2, w7, w17
    adds x20, x20, x2
    adc x1, x1, xzr
    umull x2, w8, w16
    adds x20, x20, x2
    adc x1, x1, xzr
    umull x2, w9, w15
    adds x20, x20, x2
    adc x1, x1, xzr
    umull x2, w10, w14
    adds x20, x20, x2
    adc x1, x1, xzr
    str w20, [sp, #152]
    extr x20, x1, x20, #32
    lsr x1, x1, #32
    umull x2, w7, w19
    adds x20, x20, x2
    adc x1, x1, xzr
    umull x2, w8, w17
    adds x20, x20, x2
    adc x1, x1, xzr
    umull x2, w9, w16
    adds x20, x20, x2
    adc x1, x1, xzr
    umull x2, w10, w15
    adds x20, x20, x2
    adc x1, x1, xzr
    str w20, [sp, #156]
    extr x20, x1, x20, #32
    lsr x1, x1, #32
    umull x2, w8, w19
    adds x20, x20, x2
    adc x1, x1, xzr
    umull x2, w9, w17
    adds x20, x20, x2
    adc x1, x1, xzr
    umull x2, w10, w16
    adds x20, x20, x2
    adc x1, x1, xzr
    str w20, [sp, #160]
    extr x20, x1, x20, #32
    lsr x1, x1, #32
    umull x2, w9, w19
    adds x20, x20, x2
    adc x1, x1, xzr
    umull x2, w10, w17
    adds x20, x20, x2
    adc x1, x1, xzr
    str w20, [sp, #164]
    extr x20, x1, x20, #32
    lsr x1, x1, #32
    umull x2, w10, w19
    adds x20, x20, x2
    adc x1, x1, xzr
    str w20, [sp, #168]
    extr x20, x1, x20, #32
    lsr x1, x1, #32
    str w20, [sp, #172]
    /* Fast reduction: signed column sums of the 32-bit words. */
    ldp w3, w4, [sp, #144]
    ldp w5, w6, [sp, #152]
    ldp w7, w8, [sp, #160]
    ldp w9, w10, [sp, #168]
    ldr w11, [sp, #112]
    ldr w12, [sp, #116]
    ldr w13, [sp, #120]
    ldr w14, [sp, #124]
    ldr w15, [sp, #128]
    ldr w16, [sp, #132]
    ldr w17, [sp, #136]
    ldr w19, [sp, #140]
    add x11, x11, w3, uxtw
    add x11, x11, w4, uxtw
    sub x11, x11, w6, uxtw
    sub x11, x11, w7, uxtw
    sub x11, x11, w8, uxtw
    sub x11, x11, w9, uxtw
    add x12, x12, w4, uxtw
    add x12, x12, w5, uxtw
    sub x12, x12, w7, uxtw
    sub x12, x12, w8, uxtw
    sub x12, x12, w9, uxtw
    sub x12, x12, w10, uxtw
    add x13, x13, w5, uxtw
    add x13, x13, w6, uxtw
    sub x13, x13, w8, uxtw
    sub x13, x13, w9, uxtw
    sub x13, x13, w10, uxtw
    add x14, x14, w6, uxtw
    add x14, x14, w6, uxtw
    add x14, x14, w7, uxtw
    add x14, x14, w7, uxtw
    add x14, x14, w8, uxtw
    sub x14, x14, w3, uxtw
    sub x14, x14, w4, uxtw
    sub x14, x14, w10, uxtw
    add x15, x15, w7, uxtw
    add x15, x15, w7, uxtw
    add x15, x15, w8, uxtw
    add x15, x15, w8, uxtw
    add x15, x15, w9, uxtw
    sub x15, x15, w4, uxtw
    sub x15, x15, w5, uxtw
    add x16, x16, w8, uxtw
    add x16, x16, w8, uxtw
    add x16, x16, w9, uxtw
    add x16, x16, w9, uxtw
    add x16, x16, w10, uxtw
    sub x16, x16, w5, uxtw
    sub x16, x16, w6, uxtw
    add x17, x17, w9, uxtw
    add x17, x17, w9, uxtw
    add x17, x17, w9, uxtw
    add x17, x17, w10, uxtw
    add x17, x17, w10, uxtw
    add x17, x17, w8, uxtw
    sub x17, x17, w3, uxtw
    sub x17, x17, w4, uxtw
    add x19, x19, w10, uxtw
    add x19, x19, w10, uxtw
    add x19, x19, w10, uxtw
    add x19, x19, w3, uxtw
    sub x19, x19, w5, uxtw
    sub x19, x19, w6, uxtw
    sub x19, x19, w7, uxtw
    sub x19, x19, w8, uxtw
    add x12, x12, x11, asr #32
    add x13, x13, x12, asr #32
    add x14, x14, x13, asr #32
    add x15, x15, x14, asr #32
    add x16, x16, x15, asr #32
    add x17, x17, x16, asr #32
    add x19, x19, x17, asr #32
    asr x1, x19, #32
    bfi x11, x12, #32, #32
    bfi x13, x14, #32, #32
    bfi x15, x16, #32, #32
    bfi x17, x19, #32, #32
    mov x0, x1
    asr x2, x0, #63
    mov x1, xzr
    /* Fold the signed carry above the prime length. */
    adds x11, x11, x0
    adcs x13, x13, x2
    adcs x15, x15, x2
    adcs x17, x17, x2
    adc x1, x1, x2
    lsl x20, x0, #32
    subs x13, x13, x20
    asr x20, x0, #32
    sbcs x15, x15, x20
    sbcs x17, x17, x2
    sbc x1, x1, x2
    subs x17, x17, x0
    sbc x1, x1, x2
    lsl x20, x0, #32
    adds x17, x17, x20
    adc x1, x1, x2
    /* Below zero: add p. Then subtract p unless that borrows. */
    asr x0, x1, #63
    mov x3, #0xffff
    movk x3, #0xffff, lsl #16
    movk x3, #0xffff, lsl #32
    movk x3, #0xffff, lsl #48
    mov x4, #0xffff
    movk x4, #0xffff, lsl #16
    mov x5, #0x0
    mov x6, #0x1
    movk x6, #0xffff, lsl #32
    movk x6, #0xffff, lsl #48
    and x2, x3, x0
    adds x11, x11, x2
    and x2, x4, x0
    adcs x13, x13, x2
    and x2, x5, x0
    adcs x15, x15, x2
    and x2, x6, x0
    adcs x17, x17, x2
    adc x1, x1, xzr
    subs x7, x11, x3
    sbcs x8, x13, x4
    sbcs x9, x15, x5
    sbcs x10, x17, x6
    sbcs x0, x1, xzr
    cmp x0, #0
    csel x11, x11, x7, lt
    csel x13, x13, x8, lt
    csel x15, x15, x9, lt
    csel x17, x17, x10, lt
    ldr x2, [sp, #96]
    ldr x0, [x2]
    add x0, x0, #32
    str x0, [x2, #8]
    mov w1, #8
    strh w1, [x2, #16]
    ror x11, x11, #32
    ror x13, x13, #32
    ror x15, x15, #32
    ror x17, x17, #32
    str x17, [x0, #0]
    str x15, [x0, #8]
    str x13, [x0, #16]
    str x11, [x0, #24]
    stp xzr, xzr, [sp, #112]
    stp xzr, xzr, [sp, #128]
    stp xzr, xzr, [sp, #144]
    stp xzr, xzr, [sp, #160]
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
    ldp x29, x30, [sp], #176
.cfi_restore 29
.cfi_restore 30
.cfi_def_cfa_offset 0
    ret
.cfi_endproc
.size SharkSslP256_mulmod, .-SharkSslP256_mulmod

.p2align 4
.global SharkSslP256_addmod
.hidden SharkSslP256_addmod
.type SharkSslP256_addmod, %function
SharkSslP256_addmod:
.cfi_startproc
    ldr x2, [x0, #8]
    ldr x1, [x1, #8]
    ldr x3, [x2, #24]
    ldr x7, [x1, #24]
    ror x3, x3, #32
    ror x7, x7, #32
    ldr x4, [x2, #16]
    ldr x8, [x1, #16]
    ror x4, x4, #32
    ror x8, x8, #32
    ldr x5, [x2, #8]
    ldr x9, [x1, #8]
    ror x5, x5, #32
    ror x9, x9, #32
    ldr x6, [x2, #0]
    ldr x10, [x1, #0]
    ror x6, x6, #32
    ror x10, x10, #32
    adds x3, x3, x7
    adcs x4, x4, x8
    adcs x5, x5, x9
    adcs x6, x6, x10
    adc x0, xzr, xzr
    mov x15, #0xffff
    movk x15, #0xffff, lsl #16
    movk x15, #0xffff, lsl #32
    movk x15, #0xffff, lsl #48
    subs x7, x3, x15
    mov x15, #0xffff
    movk x15, #0xffff, lsl #16
    sbcs x8, x4, x15
    mov x15, #0x0
    sbcs x9, x5, x15
    mov x15, #0x1
    movk x15, #0xffff, lsl #32
    movk x15, #0xffff, lsl #48
    sbcs x10, x6, x15
    sbcs x0, x0, xzr
    csel x3, x3, x7, lo
    csel x4, x4, x8, lo
    csel x5, x5, x9, lo
    csel x6, x6, x10, lo
    ror x3, x3, #32
    str x3, [x2, #24]
    ror x4, x4, #32
    str x4, [x2, #16]
    ror x5, x5, #32
    str x5, [x2, #8]
    ror x6, x6, #32
    str x6, [x2, #0]
    ret
.cfi_endproc
.size SharkSslP256_addmod, .-SharkSslP256_addmod

.p2align 4
.global SharkSslP256_submod
.hidden SharkSslP256_submod
.type SharkSslP256_submod, %function
SharkSslP256_submod:
.cfi_startproc
    ldr x2, [x0, #8]
    ldr x1, [x1, #8]
    ldr x3, [x2, #24]
    ldr x7, [x1, #24]
    ror x3, x3, #32
    ror x7, x7, #32
    ldr x4, [x2, #16]
    ldr x8, [x1, #16]
    ror x4, x4, #32
    ror x8, x8, #32
    ldr x5, [x2, #8]
    ldr x9, [x1, #8]
    ror x5, x5, #32
    ror x9, x9, #32
    ldr x6, [x2, #0]
    ldr x10, [x1, #0]
    ror x6, x6, #32
    ror x10, x10, #32
    subs x3, x3, x7
    sbcs x4, x4, x8
    sbcs x5, x5, x9
    sbcs x6, x6, x10
    csetm x0, lo
    mov x15, #0xffff
    movk x15, #0xffff, lsl #16
    movk x15, #0xffff, lsl #32
    movk x15, #0xffff, lsl #48
    and x15, x15, x0
    adds x3, x3, x15
    mov x15, #0xffff
    movk x15, #0xffff, lsl #16
    and x15, x15, x0
    adcs x4, x4, x15
    mov x15, #0x0
    and x15, x15, x0
    adcs x5, x5, x15
    mov x15, #0x1
    movk x15, #0xffff, lsl #32
    movk x15, #0xffff, lsl #48
    and x15, x15, x0
    adcs x6, x6, x15
    ror x3, x3, #32
    str x3, [x2, #24]
    ror x4, x4, #32
    str x4, [x2, #16]
    ror x5, x5, #32
    str x5, [x2, #8]
    ror x6, x6, #32
    str x6, [x2, #0]
    ret
.cfi_endproc
.size SharkSslP256_submod, .-SharkSslP256_submod
#endif
.section .note.GNU-stack,"",%progbits
