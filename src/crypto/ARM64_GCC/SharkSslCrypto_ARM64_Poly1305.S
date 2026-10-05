/* Copyright (c) Real Time Logic LLC, 2026. All rights reserved.
 * Original AArch64 kernels for SharkSSL. Same license as SharkSslCrypto.c.
 */
#include "../../../inc/SharkSSL_cfg.h"
#if !defined(__aarch64__) || defined(__AARCH64EB__) || !defined(__LP64__)
#error "Requires little-endian AArch64 LP64"
#endif
.text
.arch armv8-a+crypto
#if SHARKSSL_USE_POLY1305 && SHARKSSL_OPTIMIZED_POLY1305_ASM
/* Convert the existing 5x32 accumulator and clamped key to 5x26 limbs.
 * Each product sum is below 2^58, so 64-bit UMADDL accumulators suffice.
 * Keep the key and accumulator in registers for the entire block sequence.
 * ctx->flag supplies the 2^128 bit (zero for the padded final block).
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

