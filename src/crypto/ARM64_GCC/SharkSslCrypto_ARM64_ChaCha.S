/* Copyright (c) Real Time Logic LLC, 2026. All rights reserved.
 * Original AArch64 kernels for SharkSSL. Same license as SharkSslCrypto.c.
 */
#include "../../../inc/SharkSSL_cfg.h"
#if !defined(__aarch64__) || defined(__AARCH64EB__) || !defined(__LP64__)
#error "Requires little-endian AArch64 LP64"
#endif
.text
.arch armv8-a+crypto
#if SHARKSSL_USE_CHACHA20 && SHARKSSL_OPTIMIZED_CHACHA_ASM
/* Four blocks in parallel for bulk input; scalar rounds for short input.
 * Interleave independent quarter rounds to hide SIMD/GPR dependency latency.
 * Byte permutations implement the 8- and 16-bit vector rotations.
 * Both paths preserve SharkSSL's full 64-bit counter carry and consume the
 * final partial block. No read or write extends past the supplied length.
 * Only caller-saved SIMD registers are used; v8-v15 and x18 stay untouched.
 */
.p2align 4
.global sharkssl_arm64_chacha_neon
.hidden sharkssl_arm64_chacha_neon
.type sharkssl_arm64_chacha_neon, %function
sharkssl_arm64_chacha_neon:
.cfi_startproc

    cmp w3, #256
    b.lo sharkssl_arm64_chacha_scalar
    adr x9, .Lrotate8
    ldr q28, [x9]
.Lneon_next:
    cmp w3, #256
    b.lo .Lneon_short
    ldr w9, [x0, #0]
    dup v0.4s, w9
    ldr w9, [x0, #4]
    dup v1.4s, w9
    ldr w9, [x0, #8]
    dup v2.4s, w9
    ldr w9, [x0, #12]
    dup v3.4s, w9
    ldr w9, [x0, #16]
    dup v4.4s, w9
    ldr w9, [x0, #20]
    dup v5.4s, w9
    ldr w9, [x0, #24]
    dup v6.4s, w9
    ldr w9, [x0, #28]
    dup v7.4s, w9
    ldr w9, [x0, #32]
    dup v16.4s, w9
    ldr w9, [x0, #36]
    dup v17.4s, w9
    ldr w9, [x0, #40]
    dup v18.4s, w9
    ldr w9, [x0, #44]
    dup v19.4s, w9
    ldr w9, [x0, #56]
    dup v22.4s, w9
    ldr w9, [x0, #60]
    dup v23.4s, w9
    ldr x9, [x0, #48]
    add x10, x9, #0
    ins v20.s[0], w10
    lsr x11, x10, #32
    ins v21.s[0], w11
    add x10, x9, #1
    ins v20.s[1], w10
    lsr x11, x10, #32
    ins v21.s[1], w11
    add x10, x9, #2
    ins v20.s[2], w10
    lsr x11, x10, #32
    ins v21.s[2], w11
    add x10, x9, #3
    ins v20.s[3], w10
    lsr x11, x10, #32
    ins v21.s[3], w11
    mov v26.16b, v20.16b
    mov v27.16b, v21.16b
    mov w4, #10
.Lneon_four_round:
    add v0.4s, v0.4s, v4.4s
    add v1.4s, v1.4s, v5.4s
    add v2.4s, v2.4s, v6.4s
    add v3.4s, v3.4s, v7.4s
    eor v20.16b, v20.16b, v0.16b
    eor v21.16b, v21.16b, v1.16b
    eor v22.16b, v22.16b, v2.16b
    eor v23.16b, v23.16b, v3.16b
    rev32 v20.8h, v20.8h
    rev32 v21.8h, v21.8h
    rev32 v22.8h, v22.8h
    rev32 v23.8h, v23.8h
    add v16.4s, v16.4s, v20.4s
    add v17.4s, v17.4s, v21.4s
    add v18.4s, v18.4s, v22.4s
    add v19.4s, v19.4s, v23.4s
    eor v24.16b, v4.16b, v16.16b
    eor v25.16b, v5.16b, v17.16b
    eor v29.16b, v6.16b, v18.16b
    eor v30.16b, v7.16b, v19.16b
    shl v4.4s, v24.4s, #12
    shl v5.4s, v25.4s, #12
    shl v6.4s, v29.4s, #12
    shl v7.4s, v30.4s, #12
    sri v4.4s, v24.4s, #20
    sri v5.4s, v25.4s, #20
    sri v6.4s, v29.4s, #20
    sri v7.4s, v30.4s, #20
    add v0.4s, v0.4s, v4.4s
    add v1.4s, v1.4s, v5.4s
    add v2.4s, v2.4s, v6.4s
    add v3.4s, v3.4s, v7.4s
    eor v20.16b, v20.16b, v0.16b
    eor v21.16b, v21.16b, v1.16b
    eor v22.16b, v22.16b, v2.16b
    eor v23.16b, v23.16b, v3.16b
    tbl v20.16b, {v20.16b}, v28.16b
    tbl v21.16b, {v21.16b}, v28.16b
    tbl v22.16b, {v22.16b}, v28.16b
    tbl v23.16b, {v23.16b}, v28.16b
    add v16.4s, v16.4s, v20.4s
    add v17.4s, v17.4s, v21.4s
    add v18.4s, v18.4s, v22.4s
    add v19.4s, v19.4s, v23.4s
    eor v24.16b, v4.16b, v16.16b
    eor v25.16b, v5.16b, v17.16b
    eor v29.16b, v6.16b, v18.16b
    eor v30.16b, v7.16b, v19.16b
    shl v4.4s, v24.4s, #7
    shl v5.4s, v25.4s, #7
    shl v6.4s, v29.4s, #7
    shl v7.4s, v30.4s, #7
    sri v4.4s, v24.4s, #25
    sri v5.4s, v25.4s, #25
    sri v6.4s, v29.4s, #25
    sri v7.4s, v30.4s, #25
    add v0.4s, v0.4s, v5.4s
    add v1.4s, v1.4s, v6.4s
    add v2.4s, v2.4s, v7.4s
    add v3.4s, v3.4s, v4.4s
    eor v23.16b, v23.16b, v0.16b
    eor v20.16b, v20.16b, v1.16b
    eor v21.16b, v21.16b, v2.16b
    eor v22.16b, v22.16b, v3.16b
    rev32 v23.8h, v23.8h
    rev32 v20.8h, v20.8h
    rev32 v21.8h, v21.8h
    rev32 v22.8h, v22.8h
    add v18.4s, v18.4s, v23.4s
    add v19.4s, v19.4s, v20.4s
    add v16.4s, v16.4s, v21.4s
    add v17.4s, v17.4s, v22.4s
    eor v24.16b, v5.16b, v18.16b
    eor v25.16b, v6.16b, v19.16b
    eor v29.16b, v7.16b, v16.16b
    eor v30.16b, v4.16b, v17.16b
    shl v5.4s, v24.4s, #12
    shl v6.4s, v25.4s, #12
    shl v7.4s, v29.4s, #12
    shl v4.4s, v30.4s, #12
    sri v5.4s, v24.4s, #20
    sri v6.4s, v25.4s, #20
    sri v7.4s, v29.4s, #20
    sri v4.4s, v30.4s, #20
    add v0.4s, v0.4s, v5.4s
    add v1.4s, v1.4s, v6.4s
    add v2.4s, v2.4s, v7.4s
    add v3.4s, v3.4s, v4.4s
    eor v23.16b, v23.16b, v0.16b
    eor v20.16b, v20.16b, v1.16b
    eor v21.16b, v21.16b, v2.16b
    eor v22.16b, v22.16b, v3.16b
    tbl v23.16b, {v23.16b}, v28.16b
    tbl v20.16b, {v20.16b}, v28.16b
    tbl v21.16b, {v21.16b}, v28.16b
    tbl v22.16b, {v22.16b}, v28.16b
    add v18.4s, v18.4s, v23.4s
    add v19.4s, v19.4s, v20.4s
    add v16.4s, v16.4s, v21.4s
    add v17.4s, v17.4s, v22.4s
    eor v24.16b, v5.16b, v18.16b
    eor v25.16b, v6.16b, v19.16b
    eor v29.16b, v7.16b, v16.16b
    eor v30.16b, v4.16b, v17.16b
    shl v5.4s, v24.4s, #7
    shl v6.4s, v25.4s, #7
    shl v7.4s, v29.4s, #7
    shl v4.4s, v30.4s, #7
    sri v5.4s, v24.4s, #25
    sri v6.4s, v25.4s, #25
    sri v7.4s, v29.4s, #25
    sri v4.4s, v30.4s, #25
    subs w4, w4, #1
    b.ne .Lneon_four_round
    ldr w9, [x0, #0]
    dup v24.4s, w9
    add v0.4s, v0.4s, v24.4s
    ldr w9, [x0, #4]
    dup v24.4s, w9
    add v1.4s, v1.4s, v24.4s
    ldr w9, [x0, #8]
    dup v24.4s, w9
    add v2.4s, v2.4s, v24.4s
    ldr w9, [x0, #12]
    dup v24.4s, w9
    add v3.4s, v3.4s, v24.4s
    ldr w9, [x0, #16]
    dup v24.4s, w9
    add v4.4s, v4.4s, v24.4s
    ldr w9, [x0, #20]
    dup v24.4s, w9
    add v5.4s, v5.4s, v24.4s
    ldr w9, [x0, #24]
    dup v24.4s, w9
    add v6.4s, v6.4s, v24.4s
    ldr w9, [x0, #28]
    dup v24.4s, w9
    add v7.4s, v7.4s, v24.4s
    ldr w9, [x0, #32]
    dup v24.4s, w9
    add v16.4s, v16.4s, v24.4s
    ldr w9, [x0, #36]
    dup v24.4s, w9
    add v17.4s, v17.4s, v24.4s
    ldr w9, [x0, #40]
    dup v24.4s, w9
    add v18.4s, v18.4s, v24.4s
    ldr w9, [x0, #44]
    dup v24.4s, w9
    add v19.4s, v19.4s, v24.4s
    add v20.4s, v20.4s, v26.4s
    add v21.4s, v21.4s, v27.4s
    ldr w9, [x0, #56]
    dup v24.4s, w9
    add v22.4s, v22.4s, v24.4s
    ldr w9, [x0, #60]
    dup v24.4s, w9
    add v23.4s, v23.4s, v24.4s
    /* Transpose four word columns to block rows without spilling secrets. */
    zip1 v24.4s, v0.4s, v1.4s
    zip2 v25.4s, v0.4s, v1.4s
    zip1 v26.4s, v2.4s, v3.4s
    zip2 v27.4s, v2.4s, v3.4s
    zip1 v29.2d, v24.2d, v26.2d
    ldr q30, [x1, #0]
    eor v29.16b, v29.16b, v30.16b
    str q29, [x2, #0]
    zip2 v29.2d, v24.2d, v26.2d
    ldr q30, [x1, #64]
    eor v29.16b, v29.16b, v30.16b
    str q29, [x2, #64]
    zip1 v29.2d, v25.2d, v27.2d
    ldr q30, [x1, #128]
    eor v29.16b, v29.16b, v30.16b
    str q29, [x2, #128]
    zip2 v29.2d, v25.2d, v27.2d
    ldr q30, [x1, #192]
    eor v29.16b, v29.16b, v30.16b
    str q29, [x2, #192]
    zip1 v24.4s, v4.4s, v5.4s
    zip2 v25.4s, v4.4s, v5.4s
    zip1 v26.4s, v6.4s, v7.4s
    zip2 v27.4s, v6.4s, v7.4s
    zip1 v29.2d, v24.2d, v26.2d
    ldr q30, [x1, #16]
    eor v29.16b, v29.16b, v30.16b
    str q29, [x2, #16]
    zip2 v29.2d, v24.2d, v26.2d
    ldr q30, [x1, #80]
    eor v29.16b, v29.16b, v30.16b
    str q29, [x2, #80]
    zip1 v29.2d, v25.2d, v27.2d
    ldr q30, [x1, #144]
    eor v29.16b, v29.16b, v30.16b
    str q29, [x2, #144]
    zip2 v29.2d, v25.2d, v27.2d
    ldr q30, [x1, #208]
    eor v29.16b, v29.16b, v30.16b
    str q29, [x2, #208]
    zip1 v24.4s, v16.4s, v17.4s
    zip2 v25.4s, v16.4s, v17.4s
    zip1 v26.4s, v18.4s, v19.4s
    zip2 v27.4s, v18.4s, v19.4s
    zip1 v29.2d, v24.2d, v26.2d
    ldr q30, [x1, #32]
    eor v29.16b, v29.16b, v30.16b
    str q29, [x2, #32]
    zip2 v29.2d, v24.2d, v26.2d
    ldr q30, [x1, #96]
    eor v29.16b, v29.16b, v30.16b
    str q29, [x2, #96]
    zip1 v29.2d, v25.2d, v27.2d
    ldr q30, [x1, #160]
    eor v29.16b, v29.16b, v30.16b
    str q29, [x2, #160]
    zip2 v29.2d, v25.2d, v27.2d
    ldr q30, [x1, #224]
    eor v29.16b, v29.16b, v30.16b
    str q29, [x2, #224]
    zip1 v24.4s, v20.4s, v21.4s
    zip2 v25.4s, v20.4s, v21.4s
    zip1 v26.4s, v22.4s, v23.4s
    zip2 v27.4s, v22.4s, v23.4s
    zip1 v29.2d, v24.2d, v26.2d
    ldr q30, [x1, #48]
    eor v29.16b, v29.16b, v30.16b
    str q29, [x2, #48]
    zip2 v29.2d, v24.2d, v26.2d
    ldr q30, [x1, #112]
    eor v29.16b, v29.16b, v30.16b
    str q29, [x2, #112]
    zip1 v29.2d, v25.2d, v27.2d
    ldr q30, [x1, #176]
    eor v29.16b, v29.16b, v30.16b
    str q29, [x2, #176]
    zip2 v29.2d, v25.2d, v27.2d
    ldr q30, [x1, #240]
    eor v29.16b, v29.16b, v30.16b
    str q29, [x2, #240]
    add x1, x1, #256
    add x2, x2, #256
    ldr x9, [x0, #48]
    add x9, x9, #4
    str x9, [x0, #48]
    sub w3, w3, #256
    cbnz w3, .Lneon_next
.Lneon_short:
    cbnz w3, sharkssl_arm64_chacha_scalar
    ret
.cfi_endproc
.size sharkssl_arm64_chacha_neon, .-sharkssl_arm64_chacha_neon

/* Integer-only fallback, with all sixteen state words resident in GPRs. */
.p2align 4
.global sharkssl_arm64_chacha_scalar
.hidden sharkssl_arm64_chacha_scalar
.type sharkssl_arm64_chacha_scalar, %function
sharkssl_arm64_chacha_scalar:
.cfi_startproc

    cbz w3, .Lscalar_empty
    stp x19, x20, [sp, #-96]!
.cfi_def_cfa_offset 96
.cfi_offset 19, -96
.cfi_offset 20, -88
    stp x21, x22, [sp, #16]
.cfi_offset 21, -80
.cfi_offset 22, -72
.Lscalar_next:
    ldp w4, w5, [x0, #0]
    ldp w6, w7, [x0, #8]
    ldp w8, w9, [x0, #16]
    ldp w10, w11, [x0, #24]
    ldp w12, w13, [x0, #32]
    ldp w14, w15, [x0, #40]
    ldp w16, w17, [x0, #48]
    ldp w19, w20, [x0, #56]
    mov w21, #10
.Lscalar_round:
    add w4, w4, w8
    add w5, w5, w9
    add w6, w6, w10
    add w7, w7, w11
    eor w16, w16, w4
    eor w17, w17, w5
    eor w19, w19, w6
    eor w20, w20, w7
    ror w16, w16, #16
    ror w17, w17, #16
    ror w19, w19, #16
    ror w20, w20, #16
    add w12, w12, w16
    add w13, w13, w17
    add w14, w14, w19
    add w15, w15, w20
    eor w8, w8, w12
    eor w9, w9, w13
    eor w10, w10, w14
    eor w11, w11, w15
    ror w8, w8, #20
    ror w9, w9, #20
    ror w10, w10, #20
    ror w11, w11, #20
    add w4, w4, w8
    add w5, w5, w9
    add w6, w6, w10
    add w7, w7, w11
    eor w16, w16, w4
    eor w17, w17, w5
    eor w19, w19, w6
    eor w20, w20, w7
    ror w16, w16, #24
    ror w17, w17, #24
    ror w19, w19, #24
    ror w20, w20, #24
    add w12, w12, w16
    add w13, w13, w17
    add w14, w14, w19
    add w15, w15, w20
    eor w8, w8, w12
    eor w9, w9, w13
    eor w10, w10, w14
    eor w11, w11, w15
    ror w8, w8, #25
    ror w9, w9, #25
    ror w10, w10, #25
    ror w11, w11, #25
    add w4, w4, w9
    add w5, w5, w10
    add w6, w6, w11
    add w7, w7, w8
    eor w20, w20, w4
    eor w16, w16, w5
    eor w17, w17, w6
    eor w19, w19, w7
    ror w20, w20, #16
    ror w16, w16, #16
    ror w17, w17, #16
    ror w19, w19, #16
    add w14, w14, w20
    add w15, w15, w16
    add w12, w12, w17
    add w13, w13, w19
    eor w9, w9, w14
    eor w10, w10, w15
    eor w11, w11, w12
    eor w8, w8, w13
    ror w9, w9, #20
    ror w10, w10, #20
    ror w11, w11, #20
    ror w8, w8, #20
    add w4, w4, w9
    add w5, w5, w10
    add w6, w6, w11
    add w7, w7, w8
    eor w20, w20, w4
    eor w16, w16, w5
    eor w17, w17, w6
    eor w19, w19, w7
    ror w20, w20, #24
    ror w16, w16, #24
    ror w17, w17, #24
    ror w19, w19, #24
    add w14, w14, w20
    add w15, w15, w16
    add w12, w12, w17
    add w13, w13, w19
    eor w9, w9, w14
    eor w10, w10, w15
    eor w11, w11, w12
    eor w8, w8, w13
    ror w9, w9, #25
    ror w10, w10, #25
    ror w11, w11, #25
    ror w8, w8, #25
    subs w21, w21, #1
    b.ne .Lscalar_round
    ldr w22, [x0, #0]
    add w4, w4, w22
    str w4, [sp, #32]
    ldr w22, [x0, #4]
    add w5, w5, w22
    str w5, [sp, #36]
    ldr w22, [x0, #8]
    add w6, w6, w22
    str w6, [sp, #40]
    ldr w22, [x0, #12]
    add w7, w7, w22
    str w7, [sp, #44]
    ldr w22, [x0, #16]
    add w8, w8, w22
    str w8, [sp, #48]
    ldr w22, [x0, #20]
    add w9, w9, w22
    str w9, [sp, #52]
    ldr w22, [x0, #24]
    add w10, w10, w22
    str w10, [sp, #56]
    ldr w22, [x0, #28]
    add w11, w11, w22
    str w11, [sp, #60]
    ldr w22, [x0, #32]
    add w12, w12, w22
    str w12, [sp, #64]
    ldr w22, [x0, #36]
    add w13, w13, w22
    str w13, [sp, #68]
    ldr w22, [x0, #40]
    add w14, w14, w22
    str w14, [sp, #72]
    ldr w22, [x0, #44]
    add w15, w15, w22
    str w15, [sp, #76]
    ldr w22, [x0, #48]
    add w16, w16, w22
    str w16, [sp, #80]
    ldr w22, [x0, #52]
    add w17, w17, w22
    str w17, [sp, #84]
    ldr w22, [x0, #56]
    add w19, w19, w22
    str w19, [sp, #88]
    ldr w22, [x0, #60]
    add w20, w20, w22
    str w20, [sp, #92]
    mov w21, #64
    cmp w3, w21
    csel w21, w3, w21, lo
    sub w3, w3, w21
    add x22, sp, #32
    cmp w21, #16
    b.lo .Lscalar_tail
.Lscalar_xor16:
    ldp x4, x5, [x1], #16
    ldp x6, x7, [x22], #16
    eor x4, x4, x6
    eor x5, x5, x7
    stp x4, x5, [x2], #16
    sub w21, w21, #16
    cmp w21, #16
    b.hs .Lscalar_xor16
    cbz w21, .Lscalar_advance
.Lscalar_tail:
    ldrb w4, [x1], #1
    ldrb w5, [x22], #1
    eor w4, w4, w5
    strb w4, [x2], #1
    subs w21, w21, #1
    b.ne .Lscalar_tail
.Lscalar_advance:
    ldr x4, [x0, #48]
    add x4, x4, #1
    str x4, [x0, #48]
    cbnz w3, .Lscalar_next
    stp xzr, xzr, [sp, #32]
    stp xzr, xzr, [sp, #48]
    stp xzr, xzr, [sp, #64]
    stp xzr, xzr, [sp, #80]
    ldp x21, x22, [sp, #16]
.cfi_restore 21
.cfi_restore 22
    ldp x19, x20, [sp], #96
.cfi_restore 19
.cfi_restore 20
.cfi_def_cfa_offset 0
.Lscalar_empty:
    ret
.cfi_endproc
.size sharkssl_arm64_chacha_scalar, .-sharkssl_arm64_chacha_scalar
.p2align 4
.Lrotate8:
    .byte 3,0,1,2,7,4,5,6,11,8,9,10,15,12,13,14
#endif
.section .note.GNU-stack,"",%progbits

