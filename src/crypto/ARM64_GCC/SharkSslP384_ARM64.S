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
 *   $Id: SharkSslP384_ARM64.S 6737 2026-11-04 05:37:01Z gianluca $
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
#if SHARKSSL_ECC_P384_ASM
/* P-384 field arithmetic for AArch64, with the contract of SharkSslP384_M4.S on
 * twelve words; r->beg = r->mem + 12, r->len = 12.
 *   void SharkSslP384_mulmod(const SharkSslBigInt *a, const SharkSslBigInt *b, SharkSslBigInt *r);
 *   void SharkSslP384_addmod(SharkSslBigInt *a, const SharkSslBigInt *b);
 *   void SharkSslP384_submod(SharkSslBigInt *a, const SharkSslBigInt *b);
 * The method is that of SharkSslP256_ARM64.S, with 2^384 = 2^128 + 2^96 - 2^32 + 1. */
.p2align 4
.global SharkSslP384_mulmod
.hidden SharkSslP384_mulmod
.type SharkSslP384_mulmod, %function
SharkSslP384_mulmod:
.cfi_startproc
    stp x29, x30, [sp, #-208]!
.cfi_def_cfa_offset 208
.cfi_offset 29, -208
.cfi_offset 30, -200
    mov x29, sp
    stp x19, x20, [sp, #16]
.cfi_offset 19, -192
.cfi_offset 20, -184
    stp x21, x22, [sp, #32]
.cfi_offset 21, -176
.cfi_offset 22, -168
    stp x23, x24, [sp, #48]
.cfi_offset 23, -160
.cfi_offset 24, -152
    stp x25, x26, [sp, #64]
.cfi_offset 25, -144
.cfi_offset 26, -136
    stp x27, x28, [sp, #80]
.cfi_offset 27, -128
.cfi_offset 28, -120
    str x2, [sp, #96]
    ldr x2, [x0, #8]
    ldr x0, [x1, #8]
    ldp w4, w3, [x2, #40]
    ldp w16, w15, [x0, #40]
    ldp w6, w5, [x2, #32]
    ldp w19, w17, [x0, #32]
    ldp w8, w7, [x2, #24]
    ldp w21, w20, [x0, #24]
    ldp w10, w9, [x2, #16]
    ldp w23, w22, [x0, #16]
    ldp w12, w11, [x2, #8]
    ldp w25, w24, [x0, #8]
    ldp w14, w13, [x2, #0]
    ldp w27, w26, [x0, #0]
    mov x28, xzr
    mov x1, xzr
    umull x2, w3, w15
    adds x28, x28, x2
    adc x1, x1, xzr
    str w28, [sp, #112]
    extr x28, x1, x28, #32
    lsr x1, x1, #32
    umull x2, w3, w16
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w4, w15
    adds x28, x28, x2
    adc x1, x1, xzr
    str w28, [sp, #116]
    extr x28, x1, x28, #32
    lsr x1, x1, #32
    umull x2, w3, w17
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w4, w16
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w5, w15
    adds x28, x28, x2
    adc x1, x1, xzr
    str w28, [sp, #120]
    extr x28, x1, x28, #32
    lsr x1, x1, #32
    umull x2, w3, w19
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w4, w17
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w5, w16
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w6, w15
    adds x28, x28, x2
    adc x1, x1, xzr
    str w28, [sp, #124]
    extr x28, x1, x28, #32
    lsr x1, x1, #32
    umull x2, w3, w20
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w4, w19
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w5, w17
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w6, w16
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w7, w15
    adds x28, x28, x2
    adc x1, x1, xzr
    str w28, [sp, #128]
    extr x28, x1, x28, #32
    lsr x1, x1, #32
    umull x2, w3, w21
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w4, w20
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w5, w19
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w6, w17
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w7, w16
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w8, w15
    adds x28, x28, x2
    adc x1, x1, xzr
    str w28, [sp, #132]
    extr x28, x1, x28, #32
    lsr x1, x1, #32
    umull x2, w3, w22
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w4, w21
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w5, w20
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w6, w19
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w7, w17
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w8, w16
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w9, w15
    adds x28, x28, x2
    adc x1, x1, xzr
    str w28, [sp, #136]
    extr x28, x1, x28, #32
    lsr x1, x1, #32
    umull x2, w3, w23
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w4, w22
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w5, w21
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w6, w20
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w7, w19
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w8, w17
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w9, w16
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w10, w15
    adds x28, x28, x2
    adc x1, x1, xzr
    str w28, [sp, #140]
    extr x28, x1, x28, #32
    lsr x1, x1, #32
    umull x2, w3, w24
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w4, w23
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w5, w22
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w6, w21
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w7, w20
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w8, w19
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w9, w17
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w10, w16
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w11, w15
    adds x28, x28, x2
    adc x1, x1, xzr
    str w28, [sp, #144]
    extr x28, x1, x28, #32
    lsr x1, x1, #32
    umull x2, w3, w25
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w4, w24
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w5, w23
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w6, w22
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w7, w21
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w8, w20
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w9, w19
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w10, w17
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w11, w16
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w12, w15
    adds x28, x28, x2
    adc x1, x1, xzr
    str w28, [sp, #148]
    extr x28, x1, x28, #32
    lsr x1, x1, #32
    umull x2, w3, w26
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w4, w25
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w5, w24
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w6, w23
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w7, w22
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w8, w21
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w9, w20
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w10, w19
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w11, w17
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w12, w16
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w13, w15
    adds x28, x28, x2
    adc x1, x1, xzr
    str w28, [sp, #152]
    extr x28, x1, x28, #32
    lsr x1, x1, #32
    umull x2, w3, w27
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w4, w26
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w5, w25
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w6, w24
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w7, w23
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w8, w22
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w9, w21
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w10, w20
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w11, w19
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w12, w17
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w13, w16
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w14, w15
    adds x28, x28, x2
    adc x1, x1, xzr
    str w28, [sp, #156]
    extr x28, x1, x28, #32
    lsr x1, x1, #32
    umull x2, w4, w27
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w5, w26
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w6, w25
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w7, w24
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w8, w23
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w9, w22
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w10, w21
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w11, w20
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w12, w19
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w13, w17
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w14, w16
    adds x28, x28, x2
    adc x1, x1, xzr
    str w28, [sp, #160]
    extr x28, x1, x28, #32
    lsr x1, x1, #32
    umull x2, w5, w27
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w6, w26
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w7, w25
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w8, w24
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w9, w23
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w10, w22
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w11, w21
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w12, w20
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w13, w19
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w14, w17
    adds x28, x28, x2
    adc x1, x1, xzr
    str w28, [sp, #164]
    extr x28, x1, x28, #32
    lsr x1, x1, #32
    umull x2, w6, w27
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w7, w26
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w8, w25
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w9, w24
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w10, w23
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w11, w22
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w12, w21
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w13, w20
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w14, w19
    adds x28, x28, x2
    adc x1, x1, xzr
    str w28, [sp, #168]
    extr x28, x1, x28, #32
    lsr x1, x1, #32
    umull x2, w7, w27
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w8, w26
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w9, w25
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w10, w24
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w11, w23
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w12, w22
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w13, w21
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w14, w20
    adds x28, x28, x2
    adc x1, x1, xzr
    str w28, [sp, #172]
    extr x28, x1, x28, #32
    lsr x1, x1, #32
    umull x2, w8, w27
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w9, w26
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w10, w25
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w11, w24
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w12, w23
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w13, w22
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w14, w21
    adds x28, x28, x2
    adc x1, x1, xzr
    str w28, [sp, #176]
    extr x28, x1, x28, #32
    lsr x1, x1, #32
    umull x2, w9, w27
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w10, w26
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w11, w25
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w12, w24
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w13, w23
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w14, w22
    adds x28, x28, x2
    adc x1, x1, xzr
    str w28, [sp, #180]
    extr x28, x1, x28, #32
    lsr x1, x1, #32
    umull x2, w10, w27
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w11, w26
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w12, w25
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w13, w24
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w14, w23
    adds x28, x28, x2
    adc x1, x1, xzr
    str w28, [sp, #184]
    extr x28, x1, x28, #32
    lsr x1, x1, #32
    umull x2, w11, w27
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w12, w26
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w13, w25
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w14, w24
    adds x28, x28, x2
    adc x1, x1, xzr
    str w28, [sp, #188]
    extr x28, x1, x28, #32
    lsr x1, x1, #32
    umull x2, w12, w27
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w13, w26
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w14, w25
    adds x28, x28, x2
    adc x1, x1, xzr
    str w28, [sp, #192]
    extr x28, x1, x28, #32
    lsr x1, x1, #32
    umull x2, w13, w27
    adds x28, x28, x2
    adc x1, x1, xzr
    umull x2, w14, w26
    adds x28, x28, x2
    adc x1, x1, xzr
    str w28, [sp, #196]
    extr x28, x1, x28, #32
    lsr x1, x1, #32
    umull x2, w14, w27
    adds x28, x28, x2
    adc x1, x1, xzr
    str w28, [sp, #200]
    extr x28, x1, x28, #32
    lsr x1, x1, #32
    str w28, [sp, #204]
    /* Fast reduction: signed column sums of the 32-bit words. */
    ldp w3, w4, [sp, #160]
    ldp w5, w6, [sp, #168]
    ldp w7, w8, [sp, #176]
    ldp w9, w10, [sp, #184]
    ldp w11, w12, [sp, #192]
    ldp w13, w14, [sp, #200]
    ldr w15, [sp, #112]
    ldr w16, [sp, #116]
    ldr w17, [sp, #120]
    ldr w19, [sp, #124]
    ldr w20, [sp, #128]
    ldr w21, [sp, #132]
    ldr w22, [sp, #136]
    ldr w23, [sp, #140]
    ldr w24, [sp, #144]
    ldr w25, [sp, #148]
    ldr w26, [sp, #152]
    ldr w27, [sp, #156]
    add x15, x15, w3, uxtw
    add x15, x15, w11, uxtw
    add x15, x15, w12, uxtw
    sub x15, x15, w14, uxtw
    add x16, x16, w4, uxtw
    add x16, x16, w13, uxtw
    add x16, x16, w14, uxtw
    sub x16, x16, w3, uxtw
    sub x16, x16, w11, uxtw
    add x17, x17, w5, uxtw
    add x17, x17, w14, uxtw
    sub x17, x17, w4, uxtw
    sub x17, x17, w12, uxtw
    add x19, x19, w3, uxtw
    add x19, x19, w6, uxtw
    add x19, x19, w11, uxtw
    add x19, x19, w12, uxtw
    sub x19, x19, w5, uxtw
    sub x19, x19, w13, uxtw
    sub x19, x19, w14, uxtw
    add x20, x20, w12, uxtw
    add x20, x20, w12, uxtw
    add x20, x20, w3, uxtw
    add x20, x20, w4, uxtw
    add x20, x20, w7, uxtw
    add x20, x20, w11, uxtw
    add x20, x20, w13, uxtw
    sub x20, x20, w6, uxtw
    sub x20, x20, w14, uxtw
    sub x20, x20, w14, uxtw
    add x21, x21, w13, uxtw
    add x21, x21, w13, uxtw
    add x21, x21, w4, uxtw
    add x21, x21, w5, uxtw
    add x21, x21, w8, uxtw
    add x21, x21, w12, uxtw
    add x21, x21, w14, uxtw
    sub x21, x21, w7, uxtw
    add x22, x22, w14, uxtw
    add x22, x22, w14, uxtw
    add x22, x22, w5, uxtw
    add x22, x22, w6, uxtw
    add x22, x22, w9, uxtw
    add x22, x22, w13, uxtw
    sub x22, x22, w8, uxtw
    add x23, x23, w6, uxtw
    add x23, x23, w7, uxtw
    add x23, x23, w10, uxtw
    add x23, x23, w14, uxtw
    sub x23, x23, w9, uxtw
    add x24, x24, w7, uxtw
    add x24, x24, w8, uxtw
    add x24, x24, w11, uxtw
    sub x24, x24, w10, uxtw
    add x25, x25, w8, uxtw
    add x25, x25, w9, uxtw
    add x25, x25, w12, uxtw
    sub x25, x25, w11, uxtw
    add x26, x26, w9, uxtw
    add x26, x26, w10, uxtw
    add x26, x26, w13, uxtw
    sub x26, x26, w12, uxtw
    add x27, x27, w10, uxtw
    add x27, x27, w11, uxtw
    add x27, x27, w14, uxtw
    sub x27, x27, w13, uxtw
    add x16, x16, x15, asr #32
    add x17, x17, x16, asr #32
    add x19, x19, x17, asr #32
    add x20, x20, x19, asr #32
    add x21, x21, x20, asr #32
    add x22, x22, x21, asr #32
    add x23, x23, x22, asr #32
    add x24, x24, x23, asr #32
    add x25, x25, x24, asr #32
    add x26, x26, x25, asr #32
    add x27, x27, x26, asr #32
    asr x1, x27, #32
    bfi x15, x16, #32, #32
    bfi x17, x19, #32, #32
    bfi x20, x21, #32, #32
    bfi x22, x23, #32, #32
    bfi x24, x25, #32, #32
    bfi x26, x27, #32, #32
    mov x0, x1
    asr x2, x0, #63
    mov x1, xzr
    /* Fold the signed carry above the prime length. */
    adds x15, x15, x0
    adcs x17, x17, x2
    adcs x20, x20, x2
    adcs x22, x22, x2
    adcs x24, x24, x2
    adcs x26, x26, x2
    adc x1, x1, x2
    lsl x28, x0, #32
    subs x15, x15, x28
    asr x28, x0, #32
    sbcs x17, x17, x28
    sbcs x20, x20, x2
    sbcs x22, x22, x2
    sbcs x24, x24, x2
    sbcs x26, x26, x2
    sbc x1, x1, x2
    lsl x28, x0, #32
    adds x17, x17, x28
    asr x28, x0, #32
    adcs x20, x20, x28
    adcs x22, x22, x2
    adcs x24, x24, x2
    adcs x26, x26, x2
    adc x1, x1, x2
    adds x20, x20, x0
    adcs x22, x22, x2
    adcs x24, x24, x2
    adcs x26, x26, x2
    adc x1, x1, x2
    /* Below zero: add p. Then subtract p unless that borrows. */
    asr x0, x1, #63
    mov x3, #0xffff
    movk x3, #0xffff, lsl #16
    mov x4, #0x0
    movk x4, #0xffff, lsl #32
    movk x4, #0xffff, lsl #48
    mov x5, #0xfffe
    movk x5, #0xffff, lsl #16
    movk x5, #0xffff, lsl #32
    movk x5, #0xffff, lsl #48
    mov x6, #0xffff
    movk x6, #0xffff, lsl #16
    movk x6, #0xffff, lsl #32
    movk x6, #0xffff, lsl #48
    mov x7, #0xffff
    movk x7, #0xffff, lsl #16
    movk x7, #0xffff, lsl #32
    movk x7, #0xffff, lsl #48
    mov x8, #0xffff
    movk x8, #0xffff, lsl #16
    movk x8, #0xffff, lsl #32
    movk x8, #0xffff, lsl #48
    and x2, x3, x0
    adds x15, x15, x2
    and x2, x4, x0
    adcs x17, x17, x2
    and x2, x5, x0
    adcs x20, x20, x2
    and x2, x6, x0
    adcs x22, x22, x2
    and x2, x7, x0
    adcs x24, x24, x2
    and x2, x8, x0
    adcs x26, x26, x2
    adc x1, x1, xzr
    subs x9, x15, x3
    sbcs x10, x17, x4
    sbcs x11, x20, x5
    sbcs x12, x22, x6
    sbcs x13, x24, x7
    sbcs x14, x26, x8
    sbcs x0, x1, xzr
    cmp x0, #0
    csel x15, x15, x9, lt
    csel x17, x17, x10, lt
    csel x20, x20, x11, lt
    csel x22, x22, x12, lt
    csel x24, x24, x13, lt
    csel x26, x26, x14, lt
    ldr x2, [sp, #96]
    ldr x0, [x2]
    add x0, x0, #48
    str x0, [x2, #8]
    mov w1, #12
    strh w1, [x2, #16]
    ror x15, x15, #32
    ror x17, x17, #32
    ror x20, x20, #32
    ror x22, x22, #32
    ror x24, x24, #32
    ror x26, x26, #32
    str x26, [x0, #0]
    str x24, [x0, #8]
    str x22, [x0, #16]
    str x20, [x0, #24]
    str x17, [x0, #32]
    str x15, [x0, #40]
    stp xzr, xzr, [sp, #112]
    stp xzr, xzr, [sp, #128]
    stp xzr, xzr, [sp, #144]
    stp xzr, xzr, [sp, #160]
    stp xzr, xzr, [sp, #176]
    stp xzr, xzr, [sp, #192]
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
    ldp x29, x30, [sp], #208
.cfi_restore 29
.cfi_restore 30
.cfi_def_cfa_offset 0
    ret
.cfi_endproc
.size SharkSslP384_mulmod, .-SharkSslP384_mulmod

.p2align 4
.global SharkSslP384_addmod
.hidden SharkSslP384_addmod
.type SharkSslP384_addmod, %function
SharkSslP384_addmod:
.cfi_startproc
    ldr x2, [x0, #8]
    ldr x1, [x1, #8]
    ldr x3, [x2, #40]
    ldr x9, [x1, #40]
    ror x3, x3, #32
    ror x9, x9, #32
    ldr x4, [x2, #32]
    ldr x10, [x1, #32]
    ror x4, x4, #32
    ror x10, x10, #32
    ldr x5, [x2, #24]
    ldr x11, [x1, #24]
    ror x5, x5, #32
    ror x11, x11, #32
    ldr x6, [x2, #16]
    ldr x12, [x1, #16]
    ror x6, x6, #32
    ror x12, x12, #32
    ldr x7, [x2, #8]
    ldr x13, [x1, #8]
    ror x7, x7, #32
    ror x13, x13, #32
    ldr x8, [x2, #0]
    ldr x14, [x1, #0]
    ror x8, x8, #32
    ror x14, x14, #32
    adds x3, x3, x9
    adcs x4, x4, x10
    adcs x5, x5, x11
    adcs x6, x6, x12
    adcs x7, x7, x13
    adcs x8, x8, x14
    adc x0, xzr, xzr
    mov x15, #0xffff
    movk x15, #0xffff, lsl #16
    subs x9, x3, x15
    mov x15, #0x0
    movk x15, #0xffff, lsl #32
    movk x15, #0xffff, lsl #48
    sbcs x10, x4, x15
    mov x15, #0xfffe
    movk x15, #0xffff, lsl #16
    movk x15, #0xffff, lsl #32
    movk x15, #0xffff, lsl #48
    sbcs x11, x5, x15
    mov x15, #0xffff
    movk x15, #0xffff, lsl #16
    movk x15, #0xffff, lsl #32
    movk x15, #0xffff, lsl #48
    sbcs x12, x6, x15
    mov x15, #0xffff
    movk x15, #0xffff, lsl #16
    movk x15, #0xffff, lsl #32
    movk x15, #0xffff, lsl #48
    sbcs x13, x7, x15
    mov x15, #0xffff
    movk x15, #0xffff, lsl #16
    movk x15, #0xffff, lsl #32
    movk x15, #0xffff, lsl #48
    sbcs x14, x8, x15
    sbcs x0, x0, xzr
    csel x3, x3, x9, lo
    csel x4, x4, x10, lo
    csel x5, x5, x11, lo
    csel x6, x6, x12, lo
    csel x7, x7, x13, lo
    csel x8, x8, x14, lo
    ror x3, x3, #32
    str x3, [x2, #40]
    ror x4, x4, #32
    str x4, [x2, #32]
    ror x5, x5, #32
    str x5, [x2, #24]
    ror x6, x6, #32
    str x6, [x2, #16]
    ror x7, x7, #32
    str x7, [x2, #8]
    ror x8, x8, #32
    str x8, [x2, #0]
    ret
.cfi_endproc
.size SharkSslP384_addmod, .-SharkSslP384_addmod

.p2align 4
.global SharkSslP384_submod
.hidden SharkSslP384_submod
.type SharkSslP384_submod, %function
SharkSslP384_submod:
.cfi_startproc
    ldr x2, [x0, #8]
    ldr x1, [x1, #8]
    ldr x3, [x2, #40]
    ldr x9, [x1, #40]
    ror x3, x3, #32
    ror x9, x9, #32
    ldr x4, [x2, #32]
    ldr x10, [x1, #32]
    ror x4, x4, #32
    ror x10, x10, #32
    ldr x5, [x2, #24]
    ldr x11, [x1, #24]
    ror x5, x5, #32
    ror x11, x11, #32
    ldr x6, [x2, #16]
    ldr x12, [x1, #16]
    ror x6, x6, #32
    ror x12, x12, #32
    ldr x7, [x2, #8]
    ldr x13, [x1, #8]
    ror x7, x7, #32
    ror x13, x13, #32
    ldr x8, [x2, #0]
    ldr x14, [x1, #0]
    ror x8, x8, #32
    ror x14, x14, #32
    subs x3, x3, x9
    sbcs x4, x4, x10
    sbcs x5, x5, x11
    sbcs x6, x6, x12
    sbcs x7, x7, x13
    sbcs x8, x8, x14
    csetm x0, lo
    mov x15, #0xffff
    movk x15, #0xffff, lsl #16
    and x15, x15, x0
    adds x3, x3, x15
    mov x15, #0x0
    movk x15, #0xffff, lsl #32
    movk x15, #0xffff, lsl #48
    and x15, x15, x0
    adcs x4, x4, x15
    mov x15, #0xfffe
    movk x15, #0xffff, lsl #16
    movk x15, #0xffff, lsl #32
    movk x15, #0xffff, lsl #48
    and x15, x15, x0
    adcs x5, x5, x15
    mov x15, #0xffff
    movk x15, #0xffff, lsl #16
    movk x15, #0xffff, lsl #32
    movk x15, #0xffff, lsl #48
    and x15, x15, x0
    adcs x6, x6, x15
    mov x15, #0xffff
    movk x15, #0xffff, lsl #16
    movk x15, #0xffff, lsl #32
    movk x15, #0xffff, lsl #48
    and x15, x15, x0
    adcs x7, x7, x15
    mov x15, #0xffff
    movk x15, #0xffff, lsl #16
    movk x15, #0xffff, lsl #32
    movk x15, #0xffff, lsl #48
    and x15, x15, x0
    adcs x8, x8, x15
    ror x3, x3, #32
    str x3, [x2, #40]
    ror x4, x4, #32
    str x4, [x2, #32]
    ror x5, x5, #32
    str x5, [x2, #24]
    ror x6, x6, #32
    str x6, [x2, #16]
    ror x7, x7, #32
    str x7, [x2, #8]
    ror x8, x8, #32
    str x8, [x2, #0]
    ret
.cfi_endproc
.size SharkSslP384_submod, .-SharkSslP384_submod
#endif
.section .note.GNU-stack,"",%progbits
