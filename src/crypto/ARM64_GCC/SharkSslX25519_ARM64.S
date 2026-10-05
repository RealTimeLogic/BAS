/* Copyright (c) Real Time Logic LLC, 2026. All rights reserved.
 * Same license as SharkSslBigInt.c. Independently derived from SharkSSL's
 * field equations; no third-party implementation code is incorporated.
 *
 * Four unsigned 64-bit limbs, little-endian. Private field operations accept
 * every 256-bit input and return a congruent 256-bit residue modulo 2^256-38.
 * A 128-bit Karatsuba split uses 12 wide products for multiplication and
 * nine for squaring. Signed differences use masks; intermediates are combined
 * modulo 2^512, where the exact unsigned product always fits.
 * Reduction computes low + 38*high, folds its carry, then one boundary carry.
 * Canonicalization modulo 2^255-19 is needed only at byte output.
 *
 * Private helpers clobber x0-x17 and x19-x22; preserve x18,x23-x30 and SIMD.
 * x0 remains the destination pointer in field helpers. The outer entry saves
 * callee-saved registers once. No secret-dependent branches or addresses.
 * Raw scalar bytes are NOT clamped: all 256 bits match the existing hook.
 */
#include "../../../inc/SharkSSL_cfg.h"
#if SHARKSSL_X25519_ASM_HOOK
#if !defined(__aarch64__) || defined(__AARCH64EB__) || !defined(__LP64__) || defined(__ILP32__) || (!defined(__linux__) && !defined(__QNXNTO__)) || !defined(__ELF__) || defined(__ANDROID__)
#error "Requires little-endian AArch64 Linux/QNX LP64 ELF"
#endif
.text
.arch armv8-a

.p2align 4
.type .Lx_mul, %function
.Lx_mul:
.cfi_startproc
    ldp x3, x4, [x1]
    ldp x5, x6, [x1, #16]
    ldp x7, x8, [x2]
    ldp x9, x10, [x2, #16]
    mul x11, x3, x7
    umulh x12, x3, x7
    mul x13, x4, x8
    umulh x14, x4, x8
    mul x1, x3, x8
    umulh x2, x3, x8
    adds x12, x12, x1
    adcs x13, x13, x2
    adc x14, x14, xzr
    mul x1, x4, x7
    umulh x2, x4, x7
    adds x12, x12, x1
    adcs x13, x13, x2
    adc x14, x14, xzr
    mul x15, x5, x9
    umulh x16, x5, x9
    mul x17, x6, x10
    umulh x19, x6, x10
    mul x1, x5, x10
    umulh x2, x5, x10
    adds x16, x16, x1
    adcs x17, x17, x2
    adc x19, x19, xzr
    mul x1, x6, x9
    umulh x2, x6, x9
    adds x16, x16, x1
    adcs x17, x17, x2
    adc x19, x19, xzr
    subs x3, x5, x3
    sbcs x4, x6, x4
    csetm x20, cc
    subs x5, x7, x9
    sbcs x6, x8, x10
    csetm x21, cc
    eor x22, x20, x21
    eor x3, x3, x20
    eor x4, x4, x20
    subs x3, x3, x20
    sbc x4, x4, x20
    eor x5, x5, x21
    eor x6, x6, x21
    subs x5, x5, x21
    sbc x6, x6, x21
    mul x7, x3, x5
    umulh x8, x3, x5
    mul x9, x4, x6
    umulh x10, x4, x6
    mul x1, x3, x6
    umulh x2, x3, x6
    adds x8, x8, x1
    adcs x9, x9, x2
    adc x10, x10, xzr
    mul x1, x4, x5
    umulh x2, x4, x5
    adds x8, x8, x1
    adcs x9, x9, x2
    adc x10, x10, xzr
    /* Add (L + H + signed difference product) at bit 128. */
    mov x1, x13
    mov x2, x14
    mov x3, x15
    mov x4, x16
    mov x5, x17
    mov x6, x19
    adds x13, x13, x11
    adcs x14, x14, x12
    adcs x15, x15, x1
    adcs x16, x16, x2
    adcs x17, x17, xzr
    adc x19, x19, xzr
    adds x13, x13, x3
    adcs x14, x14, x4
    adcs x15, x15, x5
    adcs x16, x16, x6
    adcs x17, x17, xzr
    adc x19, x19, xzr
    eor x7, x7, x22
    eor x8, x8, x22
    eor x9, x9, x22
    eor x10, x10, x22
    adds xzr, x22, #1
    adcs x13, x13, x7
    adcs x14, x14, x8
    adcs x15, x15, x9
    adcs x16, x16, x10
    adcs x17, x17, x22
    adc x19, x19, x22
    b .Lx_reduce
.cfi_endproc
.size .Lx_mul, .-.Lx_mul

.p2align 4
.type .Lx_sq, %function
.Lx_sq:
.cfi_startproc
    ldp x3, x4, [x1]
    ldp x5, x6, [x1, #16]
    mul x11, x3, x3
    umulh x12, x3, x3
    mul x13, x4, x4
    umulh x14, x4, x4
    mul x1, x3, x4
    umulh x2, x3, x4
    adds x1, x1, x1
    adcs x2, x2, x2
    cset x20, cs
    adds x12, x12, x1
    adcs x13, x13, x2
    adc x14, x14, x20
    mul x15, x5, x5
    umulh x16, x5, x5
    mul x17, x6, x6
    umulh x19, x6, x6
    mul x1, x5, x6
    umulh x2, x5, x6
    adds x1, x1, x1
    adcs x2, x2, x2
    cset x20, cs
    adds x16, x16, x1
    adcs x17, x17, x2
    adc x19, x19, x20
    subs x3, x5, x3
    sbcs x4, x6, x4
    csetm x20, cc
    eor x3, x3, x20
    eor x4, x4, x20
    subs x3, x3, x20
    sbc x4, x4, x20
    mul x7, x3, x3
    umulh x8, x3, x3
    mul x9, x4, x4
    umulh x10, x4, x4
    mul x1, x3, x4
    umulh x2, x3, x4
    adds x1, x1, x1
    adcs x2, x2, x2
    cset x20, cs
    adds x8, x8, x1
    adcs x9, x9, x2
    adc x10, x10, x20
    /* Cross term is L + H - (high - low)^2. */
    mov x1, x13
    mov x2, x14
    mov x3, x15
    mov x4, x16
    mov x5, x17
    mov x6, x19
    adds x13, x13, x11
    adcs x14, x14, x12
    adcs x15, x15, x1
    adcs x16, x16, x2
    adcs x17, x17, xzr
    adc x19, x19, xzr
    adds x13, x13, x3
    adcs x14, x14, x4
    adcs x15, x15, x5
    adcs x16, x16, x6
    adcs x17, x17, xzr
    adc x19, x19, xzr
    subs x13, x13, x7
    sbcs x14, x14, x8
    sbcs x15, x15, x9
    sbcs x16, x16, x10
    sbcs x17, x17, xzr
    sbc x19, x19, xzr
    b .Lx_reduce
.cfi_endproc
.size .Lx_sq, .-.Lx_sq

.p2align 4
.type .Lx_reduce, %function
.Lx_reduce:
.cfi_startproc
    mov x1, #38
    /* Independent wide products expose instruction-level parallelism. */
    mul x3, x15, x1
    umulh x4, x15, x1
    mul x5, x16, x1
    umulh x6, x16, x1
    mul x7, x17, x1
    umulh x8, x17, x1
    mul x9, x19, x1
    umulh x10, x19, x1
    adds x11, x11, x3
    adcs x12, x12, x5
    adcs x13, x13, x7
    adcs x14, x14, x9
    adc x20, x10, xzr
    adds x12, x12, x4
    adcs x13, x13, x6
    adcs x14, x14, x8
    adc x20, x20, xzr
.Lx_fold:
    mov x3, #38
    mul x20, x20, x3
    adds x11, x11, x20
    adcs x12, x12, xzr
    adcs x13, x13, xzr
    adcs x14, x14, xzr
    csel x3, x3, xzr, cs
    adds x11, x11, x3
    adcs x12, x12, xzr
    adcs x13, x13, xzr
    adc x14, x14, xzr
    stp x11, x12, [x0]
    stp x13, x14, [x0, #16]
    ret
.cfi_endproc
.size .Lx_reduce, .-.Lx_reduce

.p2align 4
.type .Lx_add, %function
.Lx_add:
.cfi_startproc
    ldp x3, x4, [x1]
    ldp x5, x6, [x1, #16]
    ldp x7, x8, [x2]
    ldp x9, x10, [x2, #16]
    mov x11, #38
    adds x3, x3, x7
    adcs x4, x4, x8
    adcs x5, x5, x9
    adcs x6, x6, x10
    csel x12, x11, xzr, cs
    adds x3, x3, x12
    adcs x4, x4, xzr
    adcs x5, x5, xzr
    adcs x6, x6, xzr
    csel x12, x11, xzr, cs
    adds x3, x3, x12
    adcs x4, x4, xzr
    adcs x5, x5, xzr
    adcs x6, x6, xzr
    stp x3, x4, [x0]
    stp x5, x6, [x0, #16]
    ret
.cfi_endproc
.size .Lx_add, .-.Lx_add

.p2align 4
.type .Lx_sub, %function
.Lx_sub:
.cfi_startproc
    ldp x3, x4, [x1]
    ldp x5, x6, [x1, #16]
    ldp x7, x8, [x2]
    ldp x9, x10, [x2, #16]
    mov x11, #38
    subs x3, x3, x7
    sbcs x4, x4, x8
    sbcs x5, x5, x9
    sbcs x6, x6, x10
    csel x12, x11, xzr, cc
    subs x3, x3, x12
    sbcs x4, x4, xzr
    sbcs x5, x5, xzr
    sbcs x6, x6, xzr
    csel x12, x11, xzr, cc
    subs x3, x3, x12
    sbcs x4, x4, xzr
    sbcs x5, x5, xzr
    sbcs x6, x6, xzr
    stp x3, x4, [x0]
    stp x5, x6, [x0, #16]
    ret
.cfi_endproc
.size .Lx_sub, .-.Lx_sub

.p2align 4
.type .Lx_scale, %function
.Lx_scale:
.cfi_startproc
    ldp x3, x4, [x1]
    ldp x5, x6, [x1, #16]
    mov x7, #56130
    movk x7, #1, lsl #16
    mov x20, xzr
    mul x1, x3, x7
    umulh x2, x3, x7
    adds x1, x1, x20
    adc x20, x2, xzr
    mov x11, x1
    mul x1, x4, x7
    umulh x2, x4, x7
    adds x1, x1, x20
    adc x20, x2, xzr
    mov x12, x1
    mul x1, x5, x7
    umulh x2, x5, x7
    adds x1, x1, x20
    adc x20, x2, xzr
    mov x13, x1
    mul x1, x6, x7
    umulh x2, x6, x7
    adds x1, x1, x20
    adc x20, x2, xzr
    mov x14, x1
    b .Lx_fold
.cfi_endproc
.size .Lx_scale, .-.Lx_scale

.p2align 4
.type .Lx_inv, %function
.Lx_inv:
.cfi_startproc
    sub sp, sp, #224
.cfi_def_cfa_offset 224
    stp x23, x24, [sp, #0]
.cfi_offset 23, -224
.cfi_offset 24, -216
    stp x25, x26, [sp, #16]
.cfi_offset 25, -208
.cfi_offset 26, -200
    stp x27, x28, [sp, #32]
.cfi_offset 27, -192
.cfi_offset 28, -184
    stp x29, x30, [sp, #48]
.cfi_offset 29, -176
.cfi_offset 30, -168
    mov x28, x0
    ldp x3, x4, [x1]
    ldp x5, x6, [x1, #16]
    stp x3, x4, [sp, #64]
    stp x5, x6, [sp, #80]
    add x0, sp, #96
    add x1, sp, #64
    bl .Lx_sq
    add x0, sp, #128
    add x1, sp, #96
    bl .Lx_sq
    add x0, sp, #128
    add x1, sp, #128
    bl .Lx_sq
    add x0, sp, #128
    add x1, sp, #64
    add x2, sp, #128
    bl .Lx_mul
    add x0, sp, #96
    add x1, sp, #96
    add x2, sp, #128
    bl .Lx_mul
    add x0, sp, #160
    add x1, sp, #96
    bl .Lx_sq
    add x0, sp, #128
    add x1, sp, #128
    add x2, sp, #160
    bl .Lx_mul
    add x0, sp, #160
    add x1, sp, #128
    bl .Lx_sq
    mov w25, #4
.Lx_inv_loop0:
    add x0, sp, #160
    add x1, sp, #160
    bl .Lx_sq
    subs w25, w25, #1
    b.ne .Lx_inv_loop0
    add x0, sp, #128
    add x1, sp, #160
    add x2, sp, #128
    bl .Lx_mul
    add x0, sp, #160
    add x1, sp, #128
    bl .Lx_sq
    mov w25, #9
.Lx_inv_loop1:
    add x0, sp, #160
    add x1, sp, #160
    bl .Lx_sq
    subs w25, w25, #1
    b.ne .Lx_inv_loop1
    add x0, sp, #160
    add x1, sp, #160
    add x2, sp, #128
    bl .Lx_mul
    add x0, sp, #192
    add x1, sp, #160
    bl .Lx_sq
    mov w25, #19
.Lx_inv_loop2:
    add x0, sp, #192
    add x1, sp, #192
    bl .Lx_sq
    subs w25, w25, #1
    b.ne .Lx_inv_loop2
    add x0, sp, #160
    add x1, sp, #192
    add x2, sp, #160
    bl .Lx_mul
    add x0, sp, #160
    add x1, sp, #160
    bl .Lx_sq
    mov w25, #9
.Lx_inv_loop3:
    add x0, sp, #160
    add x1, sp, #160
    bl .Lx_sq
    subs w25, w25, #1
    b.ne .Lx_inv_loop3
    add x0, sp, #128
    add x1, sp, #160
    add x2, sp, #128
    bl .Lx_mul
    add x0, sp, #160
    add x1, sp, #128
    bl .Lx_sq
    mov w25, #49
.Lx_inv_loop4:
    add x0, sp, #160
    add x1, sp, #160
    bl .Lx_sq
    subs w25, w25, #1
    b.ne .Lx_inv_loop4
    add x0, sp, #160
    add x1, sp, #160
    add x2, sp, #128
    bl .Lx_mul
    add x0, sp, #192
    add x1, sp, #160
    bl .Lx_sq
    mov w25, #99
.Lx_inv_loop5:
    add x0, sp, #192
    add x1, sp, #192
    bl .Lx_sq
    subs w25, w25, #1
    b.ne .Lx_inv_loop5
    add x0, sp, #160
    add x1, sp, #192
    add x2, sp, #160
    bl .Lx_mul
    add x0, sp, #160
    add x1, sp, #160
    bl .Lx_sq
    mov w25, #49
.Lx_inv_loop6:
    add x0, sp, #160
    add x1, sp, #160
    bl .Lx_sq
    subs w25, w25, #1
    b.ne .Lx_inv_loop6
    add x0, sp, #128
    add x1, sp, #160
    add x2, sp, #128
    bl .Lx_mul
    add x0, sp, #128
    add x1, sp, #128
    bl .Lx_sq
    mov w25, #4
.Lx_inv_loop7:
    add x0, sp, #128
    add x1, sp, #128
    bl .Lx_sq
    subs w25, w25, #1
    b.ne .Lx_inv_loop7
    mov x0, x28
    add x1, sp, #128
    add x2, sp, #96
    bl .Lx_mul
    add x1, sp, #64
    mov w2, #10
.Lx_inv_wipe:
    stp xzr, xzr, [x1], #16
    subs w2, w2, #1
    b.ne .Lx_inv_wipe
    ldp x23, x24, [sp, #0]
.cfi_restore 23
.cfi_restore 24
    ldp x25, x26, [sp, #16]
.cfi_restore 25
.cfi_restore 26
    ldp x27, x28, [sp, #32]
.cfi_restore 27
.cfi_restore 28
    ldp x29, x30, [sp, #48]
.cfi_restore 29
.cfi_restore 30
    add sp, sp, #224
.cfi_def_cfa_offset 0
    ret
.cfi_endproc
.size .Lx_inv, .-.Lx_inv

.global sharkssl_x25519_arm64
.hidden sharkssl_x25519_arm64
.p2align 4
.type sharkssl_x25519_arm64, %function
sharkssl_x25519_arm64:
.cfi_startproc
    sub sp, sp, #352
.cfi_def_cfa_offset 352
    stp x19, x20, [sp, #0]
.cfi_offset 19, -352
.cfi_offset 20, -344
    stp x21, x22, [sp, #16]
.cfi_offset 21, -336
.cfi_offset 22, -328
    stp x23, x24, [sp, #32]
.cfi_offset 23, -320
.cfi_offset 24, -312
    stp x25, x26, [sp, #48]
.cfi_offset 25, -304
.cfi_offset 26, -296
    stp x27, x28, [sp, #64]
.cfi_offset 27, -288
.cfi_offset 28, -280
    stp x29, x30, [sp, #80]
.cfi_offset 29, -272
.cfi_offset 30, -264
    mov x28, x0
    ldp x3, x4, [x1]
    ldp x5, x6, [x1, #16]
    stp x3, x4, [sp, #96]
    stp x5, x6, [sp, #112]
    ldp x3, x4, [x2]
    ldp x5, x6, [x2, #16]
    and x6, x6, #0x7fffffffffffffff
    stp x3, x4, [sp, #128]
    stp x5, x6, [sp, #144]
    stp x3, x4, [sp, #224]
    stp x5, x6, [sp, #240]
    mov x3, #1
    stp x3, xzr, [sp, #160]
    stp xzr, xzr, [sp, #176]
    stp xzr, xzr, [sp, #192]
    stp xzr, xzr, [sp, #208]
    stp x3, xzr, [sp, #256]
    stp xzr, xzr, [sp, #272]
    mov w26, #255
    mov w27, #0
.Lx_ladder:
    lsr w3, w26, #3
    add x4, sp, #96
    ldrb w3, [x4, x3]
    and w4, w26, #7
    lsr w3, w3, w4
    and w3, w3, #1
    eor w4, w3, w27
    mov w27, w3
    neg x9, x4
    ldp x3, x4, [sp, #160]
    ldp x5, x6, [sp, #224]
    eor x7, x3, x5
    eor x8, x4, x6
    and x7, x7, x9
    and x8, x8, x9
    eor x3, x3, x7
    eor x4, x4, x8
    eor x5, x5, x7
    eor x6, x6, x8
    stp x3, x4, [sp, #160]
    stp x5, x6, [sp, #224]
    ldp x3, x4, [sp, #176]
    ldp x5, x6, [sp, #240]
    eor x7, x3, x5
    eor x8, x4, x6
    and x7, x7, x9
    and x8, x8, x9
    eor x3, x3, x7
    eor x4, x4, x8
    eor x5, x5, x7
    eor x6, x6, x8
    stp x3, x4, [sp, #176]
    stp x5, x6, [sp, #240]
    ldp x3, x4, [sp, #192]
    ldp x5, x6, [sp, #256]
    eor x7, x3, x5
    eor x8, x4, x6
    and x7, x7, x9
    and x8, x8, x9
    eor x3, x3, x7
    eor x4, x4, x8
    eor x5, x5, x7
    eor x6, x6, x8
    stp x3, x4, [sp, #192]
    stp x5, x6, [sp, #256]
    ldp x3, x4, [sp, #208]
    ldp x5, x6, [sp, #272]
    eor x7, x3, x5
    eor x8, x4, x6
    and x7, x7, x9
    and x8, x8, x9
    eor x3, x3, x7
    eor x4, x4, x8
    eor x5, x5, x7
    eor x6, x6, x8
    stp x3, x4, [sp, #208]
    stp x5, x6, [sp, #272]
    add x0, sp, #288
    add x1, sp, #224
    add x2, sp, #256
    bl .Lx_sub
    add x0, sp, #320
    add x1, sp, #160
    add x2, sp, #192
    bl .Lx_sub
    add x0, sp, #160
    add x1, sp, #160
    add x2, sp, #192
    bl .Lx_add
    add x0, sp, #192
    add x1, sp, #224
    add x2, sp, #256
    bl .Lx_add
    add x0, sp, #256
    add x1, sp, #288
    add x2, sp, #160
    bl .Lx_mul
    add x0, sp, #192
    add x1, sp, #192
    add x2, sp, #320
    bl .Lx_mul
    add x0, sp, #288
    add x1, sp, #320
    bl .Lx_sq
    add x0, sp, #320
    add x1, sp, #160
    bl .Lx_sq
    add x0, sp, #224
    add x1, sp, #256
    add x2, sp, #192
    bl .Lx_add
    add x0, sp, #192
    add x1, sp, #256
    add x2, sp, #192
    bl .Lx_sub
    add x0, sp, #160
    add x1, sp, #320
    add x2, sp, #288
    bl .Lx_mul
    add x0, sp, #320
    add x1, sp, #320
    add x2, sp, #288
    bl .Lx_sub
    add x0, sp, #192
    add x1, sp, #192
    bl .Lx_sq
    add x0, sp, #256
    add x1, sp, #320
    bl .Lx_scale
    add x0, sp, #224
    add x1, sp, #224
    bl .Lx_sq
    add x0, sp, #288
    add x1, sp, #288
    add x2, sp, #256
    bl .Lx_add
    add x0, sp, #256
    add x1, sp, #128
    add x2, sp, #192
    bl .Lx_mul
    add x0, sp, #192
    add x1, sp, #320
    add x2, sp, #288
    bl .Lx_mul
    subs w26, w26, #1
    b.pl .Lx_ladder
    neg x9, x27
    ldp x3, x4, [sp, #160]
    ldp x5, x6, [sp, #224]
    eor x7, x3, x5
    eor x8, x4, x6
    and x7, x7, x9
    and x8, x8, x9
    eor x3, x3, x7
    eor x4, x4, x8
    eor x5, x5, x7
    eor x6, x6, x8
    stp x3, x4, [sp, #160]
    stp x5, x6, [sp, #224]
    ldp x3, x4, [sp, #176]
    ldp x5, x6, [sp, #240]
    eor x7, x3, x5
    eor x8, x4, x6
    and x7, x7, x9
    and x8, x8, x9
    eor x3, x3, x7
    eor x4, x4, x8
    eor x5, x5, x7
    eor x6, x6, x8
    stp x3, x4, [sp, #176]
    stp x5, x6, [sp, #240]
    ldp x3, x4, [sp, #192]
    ldp x5, x6, [sp, #256]
    eor x7, x3, x5
    eor x8, x4, x6
    and x7, x7, x9
    and x8, x8, x9
    eor x3, x3, x7
    eor x4, x4, x8
    eor x5, x5, x7
    eor x6, x6, x8
    stp x3, x4, [sp, #192]
    stp x5, x6, [sp, #256]
    ldp x3, x4, [sp, #208]
    ldp x5, x6, [sp, #272]
    eor x7, x3, x5
    eor x8, x4, x6
    and x7, x7, x9
    and x8, x8, x9
    eor x3, x3, x7
    eor x4, x4, x8
    eor x5, x5, x7
    eor x6, x6, x8
    stp x3, x4, [sp, #208]
    stp x5, x6, [sp, #272]
    add x0, sp, #192
    add x1, sp, #192
    bl .Lx_inv
    add x0, sp, #160
    add x1, sp, #160
    add x2, sp, #192
    bl .Lx_mul
    ldp x3, x4, [sp, #160]
    ldp x5, x6, [sp, #176]
    mov x11, #19
    /* Fold bit 255 twice, then conditionally subtract p. */
    lsr x7, x6, #63
    and x6, x6, #0x7fffffffffffffff
    mul x7, x7, x11
    adds x3, x3, x7
    adcs x4, x4, xzr
    adcs x5, x5, xzr
    adc x6, x6, xzr
    lsr x7, x6, #63
    and x6, x6, #0x7fffffffffffffff
    mul x7, x7, x11
    adds x3, x3, x7
    adcs x4, x4, xzr
    adcs x5, x5, xzr
    adc x6, x6, xzr
    adds x7, x3, x11
    adcs x8, x4, xzr
    adcs x9, x5, xzr
    adc x10, x6, xzr
    tst x10, #0x8000000000000000
    and x10, x10, #0x7fffffffffffffff
    csel x3, x7, x3, ne
    csel x4, x8, x4, ne
    csel x5, x9, x5, ne
    csel x6, x10, x6, ne
    stp x3, x4, [x28]
    stp x5, x6, [x28, #16]
    add x1, sp, #96
    mov w2, #16
.Lx_raw_wipe:
    stp xzr, xzr, [x1], #16
    subs w2, w2, #1
    b.ne .Lx_raw_wipe
    ldp x19, x20, [sp, #0]
.cfi_restore 19
.cfi_restore 20
    ldp x21, x22, [sp, #16]
.cfi_restore 21
.cfi_restore 22
    ldp x23, x24, [sp, #32]
.cfi_restore 23
.cfi_restore 24
    ldp x25, x26, [sp, #48]
.cfi_restore 25
.cfi_restore 26
    ldp x27, x28, [sp, #64]
.cfi_restore 27
.cfi_restore 28
    ldp x29, x30, [sp, #80]
.cfi_restore 29
.cfi_restore 30
    add sp, sp, #352
.cfi_def_cfa_offset 0
    ret
.cfi_endproc
.size sharkssl_x25519_arm64, .-sharkssl_x25519_arm64

#ifdef SHARKSSL_X25519_TEST
/* Test-only ABI wrappers around the private arithmetic convention. */
.global sharkssl_x25519_arm64_mul
.hidden sharkssl_x25519_arm64_mul
.p2align 4
.type sharkssl_x25519_arm64_mul, %function
sharkssl_x25519_arm64_mul:
.cfi_startproc
    sub sp, sp, #48
.cfi_def_cfa_offset 48
    stp x19, x20, [sp, #0]
.cfi_offset 19, -48
.cfi_offset 20, -40
    stp x21, x22, [sp, #16]
.cfi_offset 21, -32
.cfi_offset 22, -24
    stp x29, x30, [sp, #32]
.cfi_offset 29, -16
.cfi_offset 30, -8
    bl .Lx_mul
    ldp x19, x20, [sp, #0]
.cfi_restore 19
.cfi_restore 20
    ldp x21, x22, [sp, #16]
.cfi_restore 21
.cfi_restore 22
    ldp x29, x30, [sp, #32]
.cfi_restore 29
.cfi_restore 30
    add sp, sp, #48
.cfi_def_cfa_offset 0
    ret
.cfi_endproc
.size sharkssl_x25519_arm64_mul, .-sharkssl_x25519_arm64_mul

.global sharkssl_x25519_arm64_sq
.hidden sharkssl_x25519_arm64_sq
.p2align 4
.type sharkssl_x25519_arm64_sq, %function
sharkssl_x25519_arm64_sq:
.cfi_startproc
    sub sp, sp, #48
.cfi_def_cfa_offset 48
    stp x19, x20, [sp, #0]
.cfi_offset 19, -48
.cfi_offset 20, -40
    stp x21, x22, [sp, #16]
.cfi_offset 21, -32
.cfi_offset 22, -24
    stp x29, x30, [sp, #32]
.cfi_offset 29, -16
.cfi_offset 30, -8
    bl .Lx_sq
    ldp x19, x20, [sp, #0]
.cfi_restore 19
.cfi_restore 20
    ldp x21, x22, [sp, #16]
.cfi_restore 21
.cfi_restore 22
    ldp x29, x30, [sp, #32]
.cfi_restore 29
.cfi_restore 30
    add sp, sp, #48
.cfi_def_cfa_offset 0
    ret
.cfi_endproc
.size sharkssl_x25519_arm64_sq, .-sharkssl_x25519_arm64_sq

.global sharkssl_x25519_arm64_add
.hidden sharkssl_x25519_arm64_add
.p2align 4
.type sharkssl_x25519_arm64_add, %function
sharkssl_x25519_arm64_add:
.cfi_startproc
    sub sp, sp, #48
.cfi_def_cfa_offset 48
    stp x19, x20, [sp, #0]
.cfi_offset 19, -48
.cfi_offset 20, -40
    stp x21, x22, [sp, #16]
.cfi_offset 21, -32
.cfi_offset 22, -24
    stp x29, x30, [sp, #32]
.cfi_offset 29, -16
.cfi_offset 30, -8
    bl .Lx_add
    ldp x19, x20, [sp, #0]
.cfi_restore 19
.cfi_restore 20
    ldp x21, x22, [sp, #16]
.cfi_restore 21
.cfi_restore 22
    ldp x29, x30, [sp, #32]
.cfi_restore 29
.cfi_restore 30
    add sp, sp, #48
.cfi_def_cfa_offset 0
    ret
.cfi_endproc
.size sharkssl_x25519_arm64_add, .-sharkssl_x25519_arm64_add

.global sharkssl_x25519_arm64_sub
.hidden sharkssl_x25519_arm64_sub
.p2align 4
.type sharkssl_x25519_arm64_sub, %function
sharkssl_x25519_arm64_sub:
.cfi_startproc
    sub sp, sp, #48
.cfi_def_cfa_offset 48
    stp x19, x20, [sp, #0]
.cfi_offset 19, -48
.cfi_offset 20, -40
    stp x21, x22, [sp, #16]
.cfi_offset 21, -32
.cfi_offset 22, -24
    stp x29, x30, [sp, #32]
.cfi_offset 29, -16
.cfi_offset 30, -8
    bl .Lx_sub
    ldp x19, x20, [sp, #0]
.cfi_restore 19
.cfi_restore 20
    ldp x21, x22, [sp, #16]
.cfi_restore 21
.cfi_restore 22
    ldp x29, x30, [sp, #32]
.cfi_restore 29
.cfi_restore 30
    add sp, sp, #48
.cfi_def_cfa_offset 0
    ret
.cfi_endproc
.size sharkssl_x25519_arm64_sub, .-sharkssl_x25519_arm64_sub

.global sharkssl_x25519_arm64_scale
.hidden sharkssl_x25519_arm64_scale
.p2align 4
.type sharkssl_x25519_arm64_scale, %function
sharkssl_x25519_arm64_scale:
.cfi_startproc
    sub sp, sp, #48
.cfi_def_cfa_offset 48
    stp x19, x20, [sp, #0]
.cfi_offset 19, -48
.cfi_offset 20, -40
    stp x21, x22, [sp, #16]
.cfi_offset 21, -32
.cfi_offset 22, -24
    stp x29, x30, [sp, #32]
.cfi_offset 29, -16
.cfi_offset 30, -8
    bl .Lx_scale
    ldp x19, x20, [sp, #0]
.cfi_restore 19
.cfi_restore 20
    ldp x21, x22, [sp, #16]
.cfi_restore 21
.cfi_restore 22
    ldp x29, x30, [sp, #32]
.cfi_restore 29
.cfi_restore 30
    add sp, sp, #48
.cfi_def_cfa_offset 0
    ret
.cfi_endproc
.size sharkssl_x25519_arm64_scale, .-sharkssl_x25519_arm64_scale

.global sharkssl_x25519_arm64_inv
.hidden sharkssl_x25519_arm64_inv
.p2align 4
.type sharkssl_x25519_arm64_inv, %function
sharkssl_x25519_arm64_inv:
.cfi_startproc
    sub sp, sp, #48
.cfi_def_cfa_offset 48
    stp x19, x20, [sp, #0]
.cfi_offset 19, -48
.cfi_offset 20, -40
    stp x21, x22, [sp, #16]
.cfi_offset 21, -32
.cfi_offset 22, -24
    stp x29, x30, [sp, #32]
.cfi_offset 29, -16
.cfi_offset 30, -8
    bl .Lx_inv
    ldp x19, x20, [sp, #0]
.cfi_restore 19
.cfi_restore 20
    ldp x21, x22, [sp, #16]
.cfi_restore 21
.cfi_restore 22
    ldp x29, x30, [sp, #32]
.cfi_restore 29
.cfi_restore 30
    add sp, sp, #48
.cfi_def_cfa_offset 0
    ret
.cfi_endproc
.size sharkssl_x25519_arm64_inv, .-sharkssl_x25519_arm64_inv

#endif
#endif
.section .note.GNU-stack,"",%progbits
