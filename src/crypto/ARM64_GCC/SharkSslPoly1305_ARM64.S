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
 *   $Id: SharkSslPoly1305_ARM64.S 6739 2026-11-04 08:10:12Z gianluca $
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
#if SHARKSSL_USE_POLY1305 && SHARKSSL_OPTIMIZED_POLY1305_ASM
/* Convert the existing 5x32 accumulator and clamped key to 5x26 limbs.
 * Each product sum is below 2^58, so 64-bit UMADDL accumulators suffice. On
 * Cortex-A76 a 32-bit multiply takes one cycle, against seven for MUL and UMULH.
 * ctx->flag supplies the 2^128 bit (zero for the padded final block).
 *
 * From 128 bytes on, NEON multiplies two lanes per instruction. Both lanes step
 * through alternate blocks by r^2; the last pass weighs them r^2 and r, and their
 * sum is the accumulator. Remaining single blocks use the integer loop. Lengths
 * alone decide every branch, and the registers that held r, r^2 or h are cleared.
 * x0 ctx, x1 message, w2 len (multiple of 16).
 */
.p2align 4
.global SharkSslPoly1305Ctx_process
.hidden SharkSslPoly1305Ctx_process
.type SharkSslPoly1305Ctx_process, %function
SharkSslPoly1305Ctx_process:
.cfi_startproc
    cbz w2, .Lpoly_empty
    stp x19, x20, [sp, #-64]!
.cfi_def_cfa_offset 64
.cfi_offset 19, -64
.cfi_offset 20, -56
    stp x21, x22, [sp, #16]
.cfi_offset 21, -48
.cfi_offset 22, -40
    stp x23, x24, [sp, #32]
.cfi_offset 23, -32
.cfi_offset 24, -24
    stp x25, x26, [sp, #48]
.cfi_offset 25, -16
.cfi_offset 26, -8
    ldr x17, [x0, #20]
    ldr x25, [x0, #28]
    and x8, x17, #0x3ffffff
    ubfx x9, x17, #26, #26
    extr x10, x25, x17, #52
    and x10, x10, #0x3ffffff
    ubfx x11, x25, #14, #26
    lsr x12, x25, #40
    add x13, x9, x9, lsl #2
    add x14, x10, x10, lsl #2
    add x15, x11, x11, lsl #2
    add x16, x12, x12, lsl #2
    ldp x17, x25, [x0]
    ldr w26, [x0, #16]
    and x3, x17, #0x3ffffff
    ubfx x4, x17, #26, #26
    extr x5, x25, x17, #52
    and x5, x5, #0x3ffffff
    ubfx x6, x25, #14, #26
    lsr x7, x25, #40
    orr x7, x7, x26, lsl #24
    ldrb w24, [x0, #68]
    lsl x24, x24, #24
    cmp w2, #128
    b.lo .Lpoly_blocks

    /* The accumulator enters lane 0; r^2 and its multiples by 5 fill both lanes. */
    fmov s0, w3
    fmov s1, w4
    fmov s2, w5
    fmov s3, w6
    fmov s4, w7
    mov x3, x8
    mov x4, x9
    mov x5, x10
    mov x6, x11
    mov x7, x12
    umull x19, w3, w8
    umaddl x19, w4, w16, x19
    umaddl x19, w5, w15, x19
    umaddl x19, w6, w14, x19
    umaddl x19, w7, w13, x19
    umull x20, w3, w9
    umaddl x20, w4, w8, x20
    umaddl x20, w5, w16, x20
    umaddl x20, w6, w15, x20
    umaddl x20, w7, w14, x20
    umull x21, w3, w10
    umaddl x21, w4, w9, x21
    umaddl x21, w5, w8, x21
    umaddl x21, w6, w16, x21
    umaddl x21, w7, w15, x21
    umull x22, w3, w11
    umaddl x22, w4, w10, x22
    umaddl x22, w5, w9, x22
    umaddl x22, w6, w8, x22
    umaddl x22, w7, w16, x22
    umull x23, w3, w12
    umaddl x23, w4, w11, x23
    umaddl x23, w5, w10, x23
    umaddl x23, w6, w9, x23
    umaddl x23, w7, w8, x23
    and x3, x19, #0x3ffffff
    add x20, x20, x19, lsr #26
    and x4, x20, #0x3ffffff
    add x21, x21, x20, lsr #26
    and x5, x21, #0x3ffffff
    add x22, x22, x21, lsr #26
    and x6, x22, #0x3ffffff
    add x23, x23, x22, lsr #26
    and x7, x23, #0x3ffffff
    lsr x23, x23, #26
    add x23, x23, x23, lsl #2
    add x3, x3, x23
    add x4, x4, x3, lsr #26
    and x3, x3, #0x3ffffff
    dup v16.2s, w3
    dup v17.2s, w4
    dup v18.2s, w5
    dup v19.2s, w6
    dup v20.2s, w7
    add x4, x4, x4, lsl #2
    add x5, x5, x5, lsl #2
    add x6, x6, x6, lsl #2
    add x7, x7, x7, lsl #2
    dup v21.2s, w4
    dup v22.2s, w5
    dup v23.2s, w6
    dup v24.2s, w7
    mov w19, #0x3ffffff
    dup v28.2s, w19
    mov x19, #0x3ffffff
    dup v27.2d, x19
    and w19, w2, #-32
    sub w2, w2, w19
.Lpoly_neon:
    ldp q25, q26, [x1], #32
    zip1 v29.2d, v25.2d, v26.2d
    zip2 v30.2d, v25.2d, v26.2d
    xtn v5.2s, v29.2d
    shrn v6.2s, v29.2d, #26
    ushr v7.2d, v29.2d, #52
    sli v7.2d, v30.2d, #12
    xtn v7.2s, v7.2d
    shrn v25.2s, v30.2d, #14
    ushr v26.2d, v30.2d, #40
    xtn v26.2s, v26.2d
    and v5.8b, v5.8b, v28.8b
    and v6.8b, v6.8b, v28.8b
    and v7.8b, v7.8b, v28.8b
    and v25.8b, v25.8b, v28.8b
    dup v31.2s, w24
    orr v26.8b, v26.8b, v31.8b
    add v0.2s, v0.2s, v5.2s
    add v1.2s, v1.2s, v6.2s
    add v2.2s, v2.2s, v7.2s
    add v3.2s, v3.2s, v25.2s
    add v4.2s, v4.2s, v26.2s
    subs w19, w19, #32
    b.ne .Lpoly_neon_mul
    /* Last pass: lane 1 multiplies by r. */
    ins v16.s[1], w8
    ins v17.s[1], w9
    ins v18.s[1], w10
    ins v19.s[1], w11
    ins v20.s[1], w12
    ins v21.s[1], w13
    ins v22.s[1], w14
    ins v23.s[1], w15
    ins v24.s[1], w16
    umull v5.2d, v0.2s, v16.2s
    umlal v5.2d, v1.2s, v24.2s
    umlal v5.2d, v2.2s, v23.2s
    umlal v5.2d, v3.2s, v22.2s
    umlal v5.2d, v4.2s, v21.2s
    umull v6.2d, v0.2s, v17.2s
    umlal v6.2d, v1.2s, v16.2s
    umlal v6.2d, v2.2s, v24.2s
    umlal v6.2d, v3.2s, v23.2s
    umlal v6.2d, v4.2s, v22.2s
    umull v7.2d, v0.2s, v18.2s
    umlal v7.2d, v1.2s, v17.2s
    umlal v7.2d, v2.2s, v16.2s
    umlal v7.2d, v3.2s, v24.2s
    umlal v7.2d, v4.2s, v23.2s
    umull v29.2d, v0.2s, v19.2s
    umlal v29.2d, v1.2s, v18.2s
    umlal v29.2d, v2.2s, v17.2s
    umlal v29.2d, v3.2s, v16.2s
    umlal v29.2d, v4.2s, v24.2s
    umull v30.2d, v0.2s, v20.2s
    umlal v30.2d, v1.2s, v19.2s
    umlal v30.2d, v2.2s, v18.2s
    umlal v30.2d, v3.2s, v17.2s
    umlal v30.2d, v4.2s, v16.2s
    addp d5, v5.2d
    addp d6, v6.2d
    addp d7, v7.2d
    addp d29, v29.2d
    addp d30, v30.2d
    fmov x19, d5
    fmov x20, d6
    fmov x21, d7
    fmov x22, d29
    fmov x23, d30
    and x3, x19, #0x3ffffff
    add x20, x20, x19, lsr #26
    and x4, x20, #0x3ffffff
    add x21, x21, x20, lsr #26
    and x5, x21, #0x3ffffff
    add x22, x22, x21, lsr #26
    and x6, x22, #0x3ffffff
    add x23, x23, x22, lsr #26
    and x7, x23, #0x3ffffff
    lsr x23, x23, #26
    add x23, x23, x23, lsl #2
    add x3, x3, x23
    add x4, x4, x3, lsr #26
    and x3, x3, #0x3ffffff
    movi v0.16b, #0
    movi v1.16b, #0
    movi v2.16b, #0
    movi v3.16b, #0
    movi v4.16b, #0
    movi v5.16b, #0
    movi v6.16b, #0
    movi v7.16b, #0
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
    cbz w2, .Lpoly_store
    b .Lpoly_blocks
.Lpoly_neon_mul:
    umull v5.2d, v0.2s, v16.2s
    umlal v5.2d, v1.2s, v24.2s
    umlal v5.2d, v2.2s, v23.2s
    umlal v5.2d, v3.2s, v22.2s
    umlal v5.2d, v4.2s, v21.2s
    umull v6.2d, v0.2s, v17.2s
    umlal v6.2d, v1.2s, v16.2s
    umlal v6.2d, v2.2s, v24.2s
    umlal v6.2d, v3.2s, v23.2s
    umlal v6.2d, v4.2s, v22.2s
    umull v7.2d, v0.2s, v18.2s
    umlal v7.2d, v1.2s, v17.2s
    umlal v7.2d, v2.2s, v16.2s
    umlal v7.2d, v3.2s, v24.2s
    umlal v7.2d, v4.2s, v23.2s
    umull v29.2d, v0.2s, v19.2s
    umlal v29.2d, v1.2s, v18.2s
    umlal v29.2d, v2.2s, v17.2s
    umlal v29.2d, v3.2s, v16.2s
    umlal v29.2d, v4.2s, v24.2s
    umull v30.2d, v0.2s, v20.2s
    umlal v30.2d, v1.2s, v19.2s
    umlal v30.2d, v2.2s, v18.2s
    umlal v30.2d, v3.2s, v17.2s
    umlal v30.2d, v4.2s, v16.2s
    ushr v31.2d, v29.2d, #26
    and v29.16b, v29.16b, v27.16b
    add v30.2d, v30.2d, v31.2d
    ushr v31.2d, v5.2d, #26
    and v5.16b, v5.16b, v27.16b
    add v6.2d, v6.2d, v31.2d
    ushr v31.2d, v30.2d, #26
    and v30.16b, v30.16b, v27.16b
    add v5.2d, v5.2d, v31.2d
    shl v31.2d, v31.2d, #2
    add v5.2d, v5.2d, v31.2d
    ushr v31.2d, v6.2d, #26
    and v6.16b, v6.16b, v27.16b
    add v7.2d, v7.2d, v31.2d
    ushr v31.2d, v7.2d, #26
    and v7.16b, v7.16b, v27.16b
    add v29.2d, v29.2d, v31.2d
    ushr v31.2d, v5.2d, #26
    and v5.16b, v5.16b, v27.16b
    add v6.2d, v6.2d, v31.2d
    ushr v31.2d, v29.2d, #26
    and v29.16b, v29.16b, v27.16b
    add v30.2d, v30.2d, v31.2d
    xtn v0.2s, v5.2d
    xtn v1.2s, v6.2d
    xtn v2.2s, v7.2d
    xtn v3.2s, v29.2d
    xtn v4.2s, v30.2d
    b .Lpoly_neon

.p2align 4
.Lpoly_blocks:
    ldp x17, x25, [x1], #16
    and x26, x17, #0x3ffffff
    add x3, x3, x26
    ubfx x26, x17, #26, #26
    add x4, x4, x26
    extr x26, x25, x17, #52
    and x26, x26, #0x3ffffff
    add x5, x5, x26
    ubfx x26, x25, #14, #26
    add x6, x6, x26
    add x7, x7, x25, lsr #40
    add x7, x7, x24
    umull x19, w3, w8
    umaddl x19, w4, w16, x19
    umaddl x19, w5, w15, x19
    umaddl x19, w6, w14, x19
    umaddl x19, w7, w13, x19
    umull x20, w3, w9
    umaddl x20, w4, w8, x20
    umaddl x20, w5, w16, x20
    umaddl x20, w6, w15, x20
    umaddl x20, w7, w14, x20
    umull x21, w3, w10
    umaddl x21, w4, w9, x21
    umaddl x21, w5, w8, x21
    umaddl x21, w6, w16, x21
    umaddl x21, w7, w15, x21
    umull x22, w3, w11
    umaddl x22, w4, w10, x22
    umaddl x22, w5, w9, x22
    umaddl x22, w6, w8, x22
    umaddl x22, w7, w16, x22
    umull x23, w3, w12
    umaddl x23, w4, w11, x23
    umaddl x23, w5, w10, x23
    umaddl x23, w6, w9, x23
    umaddl x23, w7, w8, x23
    and x3, x19, #0x3ffffff
    add x20, x20, x19, lsr #26
    and x4, x20, #0x3ffffff
    add x21, x21, x20, lsr #26
    and x5, x21, #0x3ffffff
    add x22, x22, x21, lsr #26
    and x6, x22, #0x3ffffff
    add x23, x23, x22, lsr #26
    and x7, x23, #0x3ffffff
    lsr x23, x23, #26
    add x23, x23, x23, lsl #2
    add x3, x3, x23
    add x4, x4, x3, lsr #26
    and x3, x3, #0x3ffffff
    subs w2, w2, #16
    b.ne .Lpoly_blocks
.Lpoly_store:
    /* Add rather than OR: limb 1 may retain a carry after the last fold. */
    add x17, x3, x4, lsl #26
    str w17, [x0]
    lsr x17, x17, #32
    add x17, x17, x5, lsl #20
    str w17, [x0, #4]
    lsr x17, x17, #32
    add x17, x17, x6, lsl #14
    str w17, [x0, #8]
    lsr x17, x17, #32
    add x17, x17, x7, lsl #8
    str w17, [x0, #12]
    lsr x17, x17, #32
    str w17, [x0, #16]
    mov x3, xzr
    mov x4, xzr
    mov x5, xzr
    mov x6, xzr
    mov x7, xzr
    mov x8, xzr
    mov x9, xzr
    mov x10, xzr
    mov x11, xzr
    mov x12, xzr
    mov x13, xzr
    mov x14, xzr
    mov x15, xzr
    mov x16, xzr
    mov x17, xzr
    ldp x25, x26, [sp, #48]
.cfi_restore 25
.cfi_restore 26
    ldp x23, x24, [sp, #32]
.cfi_restore 23
.cfi_restore 24
    ldp x21, x22, [sp, #16]
.cfi_restore 21
.cfi_restore 22
    ldp x19, x20, [sp], #64
.cfi_restore 19
.cfi_restore 20
.cfi_def_cfa_offset 0
.Lpoly_empty:
    ret
.cfi_endproc
.size SharkSslPoly1305Ctx_process, .-SharkSslPoly1305Ctx_process
#endif
.section .note.GNU-stack,"",%progbits
