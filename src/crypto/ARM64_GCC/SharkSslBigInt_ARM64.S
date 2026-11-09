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
 *   $Id: SharkSslBigInt_ARM64.S 6818 2026-11-08 21:18:02Z gianluca $
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
#if SHARKSSL_OPTIMIZED_BIGINT_ASM
/* SharkSSL BigInt core for AArch64. A number is an array of 32-bit words, most
 * significant word first, so a 64-bit limb loads with a rotate by 32; loads and
 * stores may be unaligned. Even lengths multiply 64-bit limbs; an odd length
 * falls back to 32-bit words. Loop counts depend on the public lengths only and
 * no branch or address depends on a value.
 * SharkSslBigInt: mem at 0, beg at 8, len (U16) at 16. */

/* U32 SharkSslBigInt_add(o1, o2): o1 += o2; returns the carry. */
.p2align 4
.global SharkSslBigInt_add
.hidden SharkSslBigInt_add
.type SharkSslBigInt_add, %function
SharkSslBigInt_add:
.cfi_startproc
    ldr x3, [x0, #8]
    ldrh w4, [x0, #16]
    ldr x5, [x1, #8]
    ldrh w6, [x1, #16]
    add x3, x3, x4, lsl #2
    add x5, x5, x6, lsl #2
    mov x7, xzr
    cbz w4, .Ladd_done
.Ladd_loop:
    ldr w8, [x3, #-4]
    cbz w6, .Ladd_short
    ldr w9, [x5, #-4]!
    sub w6, w6, #1
    add x8, x8, x9
.Ladd_short:
    add x8, x8, x7
    str w8, [x3, #-4]!
    lsr x7, x8, #32
    subs w4, w4, #1
    b.ne .Ladd_loop
.Ladd_done:
    mov w0, w7
    ret
.cfi_endproc
.size SharkSslBigInt_add, .-SharkSslBigInt_add

/* U32 SharkSslBigInt_sub(o1, o2): o1 -= o2; returns 0 or an all-one word. */
.p2align 4
.global SharkSslBigInt_sub
.hidden SharkSslBigInt_sub
.type SharkSslBigInt_sub, %function
SharkSslBigInt_sub:
.cfi_startproc
    ldr x3, [x0, #8]
    ldrh w4, [x0, #16]
    ldr x5, [x1, #8]
    ldrh w6, [x1, #16]
    add x3, x3, x4, lsl #2
    add x5, x5, x6, lsl #2
    mov x7, xzr
    cbz w4, .Lsub_done
.Lsub_loop:
    ldr w8, [x3, #-4]
    cbz w6, .Lsub_short
    ldr w9, [x5, #-4]!
    sub w6, w6, #1
    sub x8, x8, x9
.Lsub_short:
    add x8, x8, x7
    str w8, [x3, #-4]!
    asr x7, x8, #32
    subs w4, w4, #1
    b.ne .Lsub_loop
.Lsub_done:
    mov w0, w7
    ret
.cfi_endproc
.size SharkSslBigInt_sub, .-SharkSslBigInt_sub

/* void SharkSslBigInt_shiftr(o): o >>= 1. */
.p2align 4
.global SharkSslBigInt_shiftr
.hidden SharkSslBigInt_shiftr
.type SharkSslBigInt_shiftr, %function
SharkSslBigInt_shiftr:
.cfi_startproc
    ldr x3, [x0, #8]
    ldrh w4, [x0, #16]
    add x3, x3, x4, lsl #2
    ldr w5, [x3, #-4]
.Lshr_loop:
    subs w4, w4, #1
    b.eq .Lshr_last
    ldr w6, [x3, #-8]
    extr w5, w6, w5, #1
    str w5, [x3, #-4]!
    mov w5, w6
    b .Lshr_loop
.Lshr_last:
    lsr w5, w5, #1
    str w5, [x3, #-4]
    ret
.cfi_endproc
.size SharkSslBigInt_shiftr, .-SharkSslBigInt_shiftr

/* void SharkSslBigInt_mult_(o1, o2, result): result->len words are set by the
 * caller; the product ends at the last word, the words above it are zero.
 * Squaring uses half of the products when o1 and o2 are the same object. */
.p2align 4
.global SharkSslBigInt_mult_
.hidden SharkSslBigInt_mult_
.type SharkSslBigInt_mult_, %function
SharkSslBigInt_mult_:
.cfi_startproc
    ldr x3, [x2]
    str x3, [x2, #8]
    ldrh w4, [x2, #16]
    add x5, x3, x4, lsl #2
    cbz w4, .Lmul_zeroed
.Lmul_zero:
    str wzr, [x3], #4
    subs w4, w4, #1
    b.ne .Lmul_zero
.Lmul_zeroed:
    ldr x6, [x0, #8]
    ldrh w7, [x0, #16]
    ldr x8, [x1, #8]
    ldrh w9, [x1, #16]
    cbz w7, .Lmul_done
    cbz w9, .Lmul_done
    add x6, x6, x7, lsl #2
    add x8, x8, x9, lsl #2
    orr w10, w7, w9
    tbnz w10, #0, .Lmul32_row
    lsr w7, w7, #1
    lsr w9, w9, #1
    cmp x0, x1
    b.eq .Lsqr64
.Lmul64_row:
    ldr x10, [x8, #-8]!
    ror x10, x10, #32
    mov x11, x6
    mov x12, x5
    mov w13, w7
    mov x14, xzr
.Lmul64_limb:
    ldr x15, [x11, #-8]!
    ror x15, x15, #32
    mul x16, x15, x10
    umulh x17, x15, x10
    ldr x15, [x12, #-8]
    ror x15, x15, #32
    adds x15, x15, x16
    adc x17, x17, xzr
    adds x15, x15, x14
    adc x14, x17, xzr
    ror x15, x15, #32
    str x15, [x12, #-8]!
    subs w13, w13, #1
    b.ne .Lmul64_limb
    ror x14, x14, #32
    str x14, [x12, #-8]
    sub x5, x5, #8
    subs w9, w9, #1
    b.ne .Lmul64_row
.Lmul_done:
    ret

.Lsqr64:
    mov x10, x6
    mov x4, x5
    mov w13, w7
.Lsqr64_row:
    subs w13, w13, #1
    b.eq .Lsqr64_diag
    ldr x14, [x10, #-8]!
    ror x14, x14, #32
    mov x11, x10
    sub x12, x4, #8
    mov w15, w13
    mov x9, xzr
.Lsqr64_limb:
    ldr x16, [x11, #-8]!
    ror x16, x16, #32
    mul x17, x16, x14
    umulh x16, x16, x14
    ldr x3, [x12, #-8]
    ror x3, x3, #32
    adds x3, x3, x17
    adc x16, x16, xzr
    adds x3, x3, x9
    adc x9, x16, xzr
    ror x3, x3, #32
    str x3, [x12, #-8]!
    subs w15, w15, #1
    b.ne .Lsqr64_limb
    ror x9, x9, #32
    str x9, [x12, #-8]
    sub x4, x4, #16
    b .Lsqr64_row
.Lsqr64_diag:
    /* Double the cross products and add the squares; the carry stays in the flags. */
    mov x9, xzr
    adds xzr, xzr, xzr
.Lsqr64_square:
    ldr x14, [x6, #-8]!
    ror x14, x14, #32
    mul x15, x14, x14
    umulh x16, x14, x14
    ldr x3, [x5, #-8]
    ldr x4, [x5, #-16]
    ror x3, x3, #32
    ror x4, x4, #32
    extr x17, x3, x9, #63
    extr x10, x4, x3, #63
    mov x9, x4
    adcs x17, x17, x15
    adcs x10, x10, x16
    ror x17, x17, #32
    ror x10, x10, #32
    str x17, [x5, #-8]
    str x10, [x5, #-16]!
    sub w7, w7, #1
    cbnz w7, .Lsqr64_square
    ret

.Lmul32_row:
    ldr w10, [x8, #-4]!
    mov x11, x6
    mov x12, x5
    mov w13, w7
    mov x14, xzr
.Lmul32_word:
    ldr w15, [x11, #-4]!
    ldr w16, [x12, #-4]
    umaddl x15, w15, w10, x16
    add x15, x15, x14
    lsr x14, x15, #32
    str w15, [x12, #-4]!
    subs w13, w13, #1
    b.ne .Lmul32_word
    str w14, [x12, #-4]
    sub x5, x5, #4
    subs w9, w9, #1
    b.ne .Lmul32_row
    ret
.cfi_endproc
.size SharkSslBigInt_mult_, .-SharkSslBigInt_mult_

/* void SharkSslBigInt_monpro(o1, o2, result, mod, mu): the Montgomery product with
 * R = 2^(32*mod->len), below mod. The reduction and the final masked correction
 * run on a fixed schedule. With an even length, mu extends to 64 bits by one
 * Newton step: x*(2 + N*x) mod 2^64 is -1/N if x is -1/N mod 2^32. The carry out
 * of each row passes to the next one instead of rippling through the top words. */
.p2align 4
.global SharkSslBigInt_monpro
.hidden SharkSslBigInt_monpro
.type SharkSslBigInt_monpro, %function
SharkSslBigInt_monpro:
.cfi_startproc
    stp x29, x30, [sp, #-48]!
.cfi_def_cfa_offset 48
.cfi_offset 29, -48
.cfi_offset 30, -40
    mov x29, sp
    stp x19, x20, [sp, #16]
.cfi_offset 19, -32
.cfi_offset 20, -24
    str x21, [sp, #32]
.cfi_offset 21, -16
    mov x19, x2
    mov x20, x3
    mov w21, w4
    ldrh w5, [x3, #16]
    lsl w5, w5, #1
    add w5, w5, #1
    strh w5, [x2, #16]
    bl SharkSslBigInt_mult_
    ldr x3, [x19, #8]
    ldr x4, [x20, #8]
    ldrh w5, [x20, #16]
    add x8, x3, x5, lsl #3
    add x8, x8, #4
    add x4, x4, x5, lsl #2
    tbnz w5, #0, .Lmon32

    lsr w9, w5, #1
    ldr x6, [x4, #-8]
    ror x6, x6, #32
    mul x6, x6, x21
    add x6, x6, #2
    mul x6, x6, x21
    mov x7, xzr
.Lmon64_row:
    ldr x10, [x8, #-8]
    ror x10, x10, #32
    mul x10, x10, x6
    mov x11, x4
    mov x12, x8
    lsr w13, w5, #1
    mov x14, xzr
.Lmon64_limb:
    ldr x15, [x11, #-8]!
    ror x15, x15, #32
    mul x16, x15, x10
    umulh x17, x15, x10
    ldr x15, [x12, #-8]
    ror x15, x15, #32
    adds x15, x15, x16
    adc x17, x17, xzr
    adds x15, x15, x14
    adc x14, x17, xzr
    ror x15, x15, #32
    str x15, [x12, #-8]!
    subs w13, w13, #1
    b.ne .Lmon64_limb
    ldr x15, [x12, #-8]
    ror x15, x15, #32
    adds x15, x15, x14
    adc x14, xzr, xzr
    adds x15, x15, x7
    adc x7, x14, xzr
    ror x15, x15, #32
    str x15, [x12, #-8]
    sub x8, x8, #8
    subs w9, w9, #1
    b.ne .Lmon64_row
    ldr w15, [x3]
    add w15, w15, w7
    str w15, [x3]
    b .Lmon_final

.Lmon32:
    mov w9, w5
.Lmon32_row:
    ldr w10, [x8, #-4]
    mul w10, w10, w21
    mov x11, x4
    mov x12, x8
    mov w13, w5
    mov x14, xzr
.Lmon32_word:
    ldr w15, [x11, #-4]!
    ldr w16, [x12, #-4]
    umaddl x15, w15, w10, x16
    add x15, x15, x14
    lsr x14, x15, #32
    str w15, [x12, #-4]!
    subs w13, w13, #1
    b.ne .Lmon32_word
.Lmon32_carry:
    ldr w15, [x12, #-4]
    add x15, x15, x14
    lsr x14, x15, #32
    str w15, [x12, #-4]!
    cmp x12, x3
    b.hi .Lmon32_carry
    sub x8, x8, #4
    subs w9, w9, #1
    b.ne .Lmon32_row

.Lmon_final:
    /* Subtract mod, then add it back through a mask when that borrowed. */
    ldrh w5, [x20, #16]
    add w6, w5, #1
    strh w6, [x19, #16]
    mov x0, x19
    mov x1, x20
    bl SharkSslBigInt_sub
    ldr x3, [x19, #8]
    ldr x4, [x20, #8]
    ldrh w5, [x20, #16]
    add x6, x3, x5, lsl #2
    add x6, x6, #4
    add x4, x4, x5, lsl #2
    mov x7, xzr
.Lmon_addback:
    ldr w8, [x6, #-4]
    ldr w9, [x4, #-4]!
    and w9, w9, w0
    add x8, x8, x9
    add x8, x8, x7
    str w8, [x6, #-4]!
    lsr x7, x8, #32
    subs w5, w5, #1
    b.ne .Lmon_addback
    ldr w8, [x3]
    add w8, w8, w7
    str w8, [x3], #4
    str x3, [x19, #8]
    ldrh w5, [x19, #16]
    sub w5, w5, #1
    strh w5, [x19, #16]
    ldr x21, [sp, #32]
.cfi_restore 21
    ldp x19, x20, [sp, #16]
.cfi_restore 19
.cfi_restore 20
    ldp x29, x30, [sp], #48
.cfi_restore 29
.cfi_restore 30
.cfi_def_cfa_offset 0
    ret
.cfi_endproc
.size SharkSslBigInt_monpro, .-SharkSslBigInt_monpro
#endif
.section .note.GNU-stack,"",%progbits
