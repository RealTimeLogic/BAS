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
 *   $Id: SharkSslX25519_ARM64.S 6737 2026-11-04 05:37:01Z gianluca $
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
#if SHARKSSL_X25519_ASM_HOOK
/* X25519 for AArch64 with NEON: void sharkssl_x25519_arm64(U8 out[32],
 * const U8 scalar[32], const U8 u[32]). Little-endian byte strings; no clamping
 * here; the top bit of u is ignored.
 *
 * Cortex-A76 executes one UMLAL per cycle, each with two products, and takes
 * seven cycles for a 64x64-bit MUL and UMULH pair. So field elements are ten
 * 26/25-bit limbs, and each 8-byte slot holds one limb of two elements, one per
 * lane. A ladder step is three two-lane multiplications and two two-lane squarings;
 * one lane of the third multiplication is unused. The factors 19 and 2 are in the
 * operands, the column sums stay below 2^64 with operands up to 1.5*2^27 (even
 * limbs) and 1.5*2^26 (odd limbs), and each result is carried below 2^26 and 2^25
 * plus a small excess. Subtractions add 2p, so limbs never go negative.
 *
 * All 256 scalar bits are processed: branches and addresses depend only on the
 * bit counter, and the exchange of the ladder points is a mask. The frame and the
 * registers are cleared before the return. */
.p2align 4
.type .Lx25519_mul2, %function
.Lx25519_mul2:
.cfi_startproc
    /* x0 = out, x1 = a, x2 = b: each lane of a times the same lane of b. */
    ldp d0, d1, [x2, #0]
    ldp d2, d3, [x2, #16]
    ldp d4, d5, [x2, #32]
    ldp d6, d7, [x2, #48]
    ldp d8, d9, [x2, #64]
    movi v29.2s, #19
    mul v10.2s, v1.2s, v29.2s
    mul v11.2s, v2.2s, v29.2s
    mul v12.2s, v3.2s, v29.2s
    mul v13.2s, v4.2s, v29.2s
    mul v14.2s, v5.2s, v29.2s
    mul v15.2s, v6.2s, v29.2s
    mul v16.2s, v7.2s, v29.2s
    mul v17.2s, v8.2s, v29.2s
    mul v18.2s, v9.2s, v29.2s
    ldr d29, [x1, #0]
    umull v19.2d, v29.2s, v0.2s
    umull v20.2d, v29.2s, v1.2s
    umull v21.2d, v29.2s, v2.2s
    umull v22.2d, v29.2s, v3.2s
    umull v23.2d, v29.2s, v4.2s
    umull v24.2d, v29.2s, v5.2s
    umull v25.2d, v29.2s, v6.2s
    umull v26.2d, v29.2s, v7.2s
    umull v27.2d, v29.2s, v8.2s
    umull v28.2d, v29.2s, v9.2s
    ldr d29, [x1, #8]
    shl v30.2s, v29.2s, #1
    umlal v20.2d, v29.2s, v0.2s
    umlal v21.2d, v30.2s, v1.2s
    umlal v22.2d, v29.2s, v2.2s
    umlal v23.2d, v30.2s, v3.2s
    umlal v24.2d, v29.2s, v4.2s
    umlal v25.2d, v30.2s, v5.2s
    umlal v26.2d, v29.2s, v6.2s
    umlal v27.2d, v30.2s, v7.2s
    umlal v28.2d, v29.2s, v8.2s
    umlal v19.2d, v30.2s, v18.2s
    ldr d29, [x1, #16]
    umlal v21.2d, v29.2s, v0.2s
    umlal v22.2d, v29.2s, v1.2s
    umlal v23.2d, v29.2s, v2.2s
    umlal v24.2d, v29.2s, v3.2s
    umlal v25.2d, v29.2s, v4.2s
    umlal v26.2d, v29.2s, v5.2s
    umlal v27.2d, v29.2s, v6.2s
    umlal v28.2d, v29.2s, v7.2s
    umlal v19.2d, v29.2s, v17.2s
    umlal v20.2d, v29.2s, v18.2s
    ldr d29, [x1, #24]
    shl v30.2s, v29.2s, #1
    umlal v22.2d, v29.2s, v0.2s
    umlal v23.2d, v30.2s, v1.2s
    umlal v24.2d, v29.2s, v2.2s
    umlal v25.2d, v30.2s, v3.2s
    umlal v26.2d, v29.2s, v4.2s
    umlal v27.2d, v30.2s, v5.2s
    umlal v28.2d, v29.2s, v6.2s
    umlal v19.2d, v30.2s, v16.2s
    umlal v20.2d, v29.2s, v17.2s
    umlal v21.2d, v30.2s, v18.2s
    ldr d29, [x1, #32]
    umlal v23.2d, v29.2s, v0.2s
    umlal v24.2d, v29.2s, v1.2s
    umlal v25.2d, v29.2s, v2.2s
    umlal v26.2d, v29.2s, v3.2s
    umlal v27.2d, v29.2s, v4.2s
    umlal v28.2d, v29.2s, v5.2s
    umlal v19.2d, v29.2s, v15.2s
    umlal v20.2d, v29.2s, v16.2s
    umlal v21.2d, v29.2s, v17.2s
    umlal v22.2d, v29.2s, v18.2s
    ldr d29, [x1, #40]
    shl v30.2s, v29.2s, #1
    umlal v24.2d, v29.2s, v0.2s
    umlal v25.2d, v30.2s, v1.2s
    umlal v26.2d, v29.2s, v2.2s
    umlal v27.2d, v30.2s, v3.2s
    umlal v28.2d, v29.2s, v4.2s
    umlal v19.2d, v30.2s, v14.2s
    umlal v20.2d, v29.2s, v15.2s
    umlal v21.2d, v30.2s, v16.2s
    umlal v22.2d, v29.2s, v17.2s
    umlal v23.2d, v30.2s, v18.2s
    ldr d29, [x1, #48]
    umlal v25.2d, v29.2s, v0.2s
    umlal v26.2d, v29.2s, v1.2s
    umlal v27.2d, v29.2s, v2.2s
    umlal v28.2d, v29.2s, v3.2s
    umlal v19.2d, v29.2s, v13.2s
    umlal v20.2d, v29.2s, v14.2s
    umlal v21.2d, v29.2s, v15.2s
    umlal v22.2d, v29.2s, v16.2s
    umlal v23.2d, v29.2s, v17.2s
    umlal v24.2d, v29.2s, v18.2s
    ldr d29, [x1, #56]
    shl v30.2s, v29.2s, #1
    umlal v26.2d, v29.2s, v0.2s
    umlal v27.2d, v30.2s, v1.2s
    umlal v28.2d, v29.2s, v2.2s
    umlal v19.2d, v30.2s, v12.2s
    umlal v20.2d, v29.2s, v13.2s
    umlal v21.2d, v30.2s, v14.2s
    umlal v22.2d, v29.2s, v15.2s
    umlal v23.2d, v30.2s, v16.2s
    umlal v24.2d, v29.2s, v17.2s
    umlal v25.2d, v30.2s, v18.2s
    ldr d29, [x1, #64]
    umlal v27.2d, v29.2s, v0.2s
    umlal v28.2d, v29.2s, v1.2s
    umlal v19.2d, v29.2s, v11.2s
    umlal v20.2d, v29.2s, v12.2s
    umlal v21.2d, v29.2s, v13.2s
    umlal v22.2d, v29.2s, v14.2s
    umlal v23.2d, v29.2s, v15.2s
    umlal v24.2d, v29.2s, v16.2s
    umlal v25.2d, v29.2s, v17.2s
    umlal v26.2d, v29.2s, v18.2s
    ldr d29, [x1, #72]
    shl v30.2s, v29.2s, #1
    umlal v28.2d, v29.2s, v0.2s
    umlal v19.2d, v30.2s, v10.2s
    umlal v20.2d, v29.2s, v11.2s
    umlal v21.2d, v30.2s, v12.2s
    umlal v22.2d, v29.2s, v13.2s
    umlal v23.2d, v30.2s, v14.2s
    umlal v24.2d, v29.2s, v15.2s
    umlal v25.2d, v30.2s, v16.2s
    umlal v26.2d, v29.2s, v17.2s
    umlal v27.2d, v30.2s, v18.2s
    movi v0.16b, #0xff
    ushr v1.2d, v0.2d, #39
    ushr v0.2d, v0.2d, #38
    ushr v2.2d, v19.2d, #26
    and v19.16b, v19.16b, v0.16b
    add v20.2d, v20.2d, v2.2d
    ushr v2.2d, v23.2d, #26
    and v23.16b, v23.16b, v0.16b
    add v24.2d, v24.2d, v2.2d
    ushr v2.2d, v20.2d, #25
    and v20.16b, v20.16b, v1.16b
    add v21.2d, v21.2d, v2.2d
    ushr v2.2d, v24.2d, #25
    and v24.16b, v24.16b, v1.16b
    add v25.2d, v25.2d, v2.2d
    ushr v2.2d, v21.2d, #26
    and v21.16b, v21.16b, v0.16b
    add v22.2d, v22.2d, v2.2d
    ushr v2.2d, v25.2d, #26
    and v25.16b, v25.16b, v0.16b
    add v26.2d, v26.2d, v2.2d
    ushr v2.2d, v22.2d, #25
    and v22.16b, v22.16b, v1.16b
    add v23.2d, v23.2d, v2.2d
    ushr v2.2d, v26.2d, #25
    and v26.16b, v26.16b, v1.16b
    add v27.2d, v27.2d, v2.2d
    ushr v2.2d, v23.2d, #26
    and v23.16b, v23.16b, v0.16b
    add v24.2d, v24.2d, v2.2d
    ushr v2.2d, v27.2d, #26
    and v27.16b, v27.16b, v0.16b
    add v28.2d, v28.2d, v2.2d
    ushr v2.2d, v28.2d, #25
    and v28.16b, v28.16b, v1.16b
    shl v3.2d, v2.2d, #4
    add v3.2d, v3.2d, v2.2d
    add v3.2d, v3.2d, v2.2d
    add v3.2d, v3.2d, v2.2d
    add v19.2d, v19.2d, v3.2d
    ushr v2.2d, v19.2d, #26
    and v19.16b, v19.16b, v0.16b
    add v20.2d, v20.2d, v2.2d
    xtn v19.2s, v19.2d
    xtn v20.2s, v20.2d
    xtn v21.2s, v21.2d
    xtn v22.2s, v22.2d
    xtn v23.2s, v23.2d
    xtn v24.2s, v24.2d
    xtn v25.2s, v25.2d
    xtn v26.2s, v26.2d
    xtn v27.2s, v27.2d
    xtn v28.2s, v28.2d
    stp d19, d20, [x0, #0]
    stp d21, d22, [x0, #16]
    stp d23, d24, [x0, #32]
    stp d25, d26, [x0, #48]
    stp d27, d28, [x0, #64]
    ret
.cfi_endproc
.size .Lx25519_mul2, .-.Lx25519_mul2

.p2align 4
.type .Lx25519_sq2, %function
.Lx25519_sq2:
.cfi_startproc
    /* x0 = out, x1 = a: both lanes squared. */
    ldp d0, d1, [x1, #0]
    ldp d2, d3, [x1, #16]
    ldp d4, d5, [x1, #32]
    ldp d6, d7, [x1, #48]
    ldp d8, d9, [x1, #64]
    movi v29.2s, #19
    mul v10.2s, v1.2s, v29.2s
    mul v11.2s, v2.2s, v29.2s
    mul v12.2s, v3.2s, v29.2s
    mul v13.2s, v4.2s, v29.2s
    mul v14.2s, v5.2s, v29.2s
    mul v15.2s, v6.2s, v29.2s
    mul v16.2s, v7.2s, v29.2s
    mul v17.2s, v8.2s, v29.2s
    mul v18.2s, v9.2s, v29.2s
    shl v30.2s, v0.2s, #1
    umull v19.2d, v0.2s, v0.2s
    umull v20.2d, v30.2s, v1.2s
    umull v21.2d, v30.2s, v2.2s
    umull v22.2d, v30.2s, v3.2s
    umull v23.2d, v30.2s, v4.2s
    umull v24.2d, v30.2s, v5.2s
    umull v25.2d, v30.2s, v6.2s
    umull v26.2d, v30.2s, v7.2s
    umull v27.2d, v30.2s, v8.2s
    umull v28.2d, v30.2s, v9.2s
    shl v30.2s, v1.2s, #1
    shl v31.2s, v30.2s, #1
    umlal v21.2d, v30.2s, v1.2s
    umlal v22.2d, v30.2s, v2.2s
    umlal v23.2d, v31.2s, v3.2s
    umlal v24.2d, v30.2s, v4.2s
    umlal v25.2d, v31.2s, v5.2s
    umlal v26.2d, v30.2s, v6.2s
    umlal v27.2d, v31.2s, v7.2s
    umlal v28.2d, v30.2s, v8.2s
    umlal v19.2d, v31.2s, v18.2s
    shl v30.2s, v2.2s, #1
    umlal v23.2d, v2.2s, v2.2s
    umlal v24.2d, v30.2s, v3.2s
    umlal v25.2d, v30.2s, v4.2s
    umlal v26.2d, v30.2s, v5.2s
    umlal v27.2d, v30.2s, v6.2s
    umlal v28.2d, v30.2s, v7.2s
    umlal v19.2d, v30.2s, v17.2s
    umlal v20.2d, v30.2s, v18.2s
    shl v30.2s, v3.2s, #1
    shl v31.2s, v30.2s, #1
    umlal v25.2d, v30.2s, v3.2s
    umlal v26.2d, v30.2s, v4.2s
    umlal v27.2d, v31.2s, v5.2s
    umlal v28.2d, v30.2s, v6.2s
    umlal v19.2d, v31.2s, v16.2s
    umlal v20.2d, v30.2s, v17.2s
    umlal v21.2d, v31.2s, v18.2s
    shl v30.2s, v4.2s, #1
    umlal v27.2d, v4.2s, v4.2s
    umlal v28.2d, v30.2s, v5.2s
    umlal v19.2d, v30.2s, v15.2s
    umlal v20.2d, v30.2s, v16.2s
    umlal v21.2d, v30.2s, v17.2s
    umlal v22.2d, v30.2s, v18.2s
    shl v30.2s, v5.2s, #1
    shl v31.2s, v30.2s, #1
    umlal v19.2d, v30.2s, v14.2s
    umlal v20.2d, v30.2s, v15.2s
    umlal v21.2d, v31.2s, v16.2s
    umlal v22.2d, v30.2s, v17.2s
    umlal v23.2d, v31.2s, v18.2s
    shl v30.2s, v6.2s, #1
    umlal v21.2d, v6.2s, v15.2s
    umlal v22.2d, v30.2s, v16.2s
    umlal v23.2d, v30.2s, v17.2s
    umlal v24.2d, v30.2s, v18.2s
    shl v30.2s, v7.2s, #1
    shl v31.2s, v30.2s, #1
    umlal v23.2d, v30.2s, v16.2s
    umlal v24.2d, v30.2s, v17.2s
    umlal v25.2d, v31.2s, v18.2s
    shl v30.2s, v8.2s, #1
    umlal v25.2d, v8.2s, v17.2s
    umlal v26.2d, v30.2s, v18.2s
    shl v30.2s, v9.2s, #1
    shl v31.2s, v30.2s, #1
    umlal v27.2d, v30.2s, v18.2s
    movi v0.16b, #0xff
    ushr v1.2d, v0.2d, #39
    ushr v0.2d, v0.2d, #38
    ushr v2.2d, v19.2d, #26
    and v19.16b, v19.16b, v0.16b
    add v20.2d, v20.2d, v2.2d
    ushr v2.2d, v23.2d, #26
    and v23.16b, v23.16b, v0.16b
    add v24.2d, v24.2d, v2.2d
    ushr v2.2d, v20.2d, #25
    and v20.16b, v20.16b, v1.16b
    add v21.2d, v21.2d, v2.2d
    ushr v2.2d, v24.2d, #25
    and v24.16b, v24.16b, v1.16b
    add v25.2d, v25.2d, v2.2d
    ushr v2.2d, v21.2d, #26
    and v21.16b, v21.16b, v0.16b
    add v22.2d, v22.2d, v2.2d
    ushr v2.2d, v25.2d, #26
    and v25.16b, v25.16b, v0.16b
    add v26.2d, v26.2d, v2.2d
    ushr v2.2d, v22.2d, #25
    and v22.16b, v22.16b, v1.16b
    add v23.2d, v23.2d, v2.2d
    ushr v2.2d, v26.2d, #25
    and v26.16b, v26.16b, v1.16b
    add v27.2d, v27.2d, v2.2d
    ushr v2.2d, v23.2d, #26
    and v23.16b, v23.16b, v0.16b
    add v24.2d, v24.2d, v2.2d
    ushr v2.2d, v27.2d, #26
    and v27.16b, v27.16b, v0.16b
    add v28.2d, v28.2d, v2.2d
    ushr v2.2d, v28.2d, #25
    and v28.16b, v28.16b, v1.16b
    shl v3.2d, v2.2d, #4
    add v3.2d, v3.2d, v2.2d
    add v3.2d, v3.2d, v2.2d
    add v3.2d, v3.2d, v2.2d
    add v19.2d, v19.2d, v3.2d
    ushr v2.2d, v19.2d, #26
    and v19.16b, v19.16b, v0.16b
    add v20.2d, v20.2d, v2.2d
    xtn v19.2s, v19.2d
    xtn v20.2s, v20.2d
    xtn v21.2s, v21.2d
    xtn v22.2s, v22.2d
    xtn v23.2s, v23.2d
    xtn v24.2s, v24.2d
    xtn v25.2s, v25.2d
    xtn v26.2s, v26.2d
    xtn v27.2s, v27.2d
    xtn v28.2s, v28.2d
    stp d19, d20, [x0, #0]
    stp d21, d22, [x0, #16]
    stp d23, d24, [x0, #32]
    stp d25, d26, [x0, #48]
    stp d27, d28, [x0, #64]
    ret
.cfi_endproc
.size .Lx25519_sq2, .-.Lx25519_sq2

.p2align 4
.type .Lx25519_add2, %function
.Lx25519_add2:
.cfi_startproc
    /* out = a + b, lane by lane */
    ldp d0, d1, [x1, #0]
    ldp d2, d3, [x2, #0]
    add v0.2s, v0.2s, v2.2s
    add v1.2s, v1.2s, v3.2s
    stp d0, d1, [x0, #0]
    ldp d0, d1, [x1, #16]
    ldp d2, d3, [x2, #16]
    add v0.2s, v0.2s, v2.2s
    add v1.2s, v1.2s, v3.2s
    stp d0, d1, [x0, #16]
    ldp d0, d1, [x1, #32]
    ldp d2, d3, [x2, #32]
    add v0.2s, v0.2s, v2.2s
    add v1.2s, v1.2s, v3.2s
    stp d0, d1, [x0, #32]
    ldp d0, d1, [x1, #48]
    ldp d2, d3, [x2, #48]
    add v0.2s, v0.2s, v2.2s
    add v1.2s, v1.2s, v3.2s
    stp d0, d1, [x0, #48]
    ldp d0, d1, [x1, #64]
    ldp d2, d3, [x2, #64]
    add v0.2s, v0.2s, v2.2s
    add v1.2s, v1.2s, v3.2s
    stp d0, d1, [x0, #64]
    ret
.cfi_endproc
.size .Lx25519_add2, .-.Lx25519_add2

.p2align 4
.type .Lx25519_sub2, %function
.Lx25519_sub2:
.cfi_startproc
    /* out = a + 2p - b, lane by lane */
    mov w3, #0x7fffffe
    dup v4.2s, w3
    mov w3, #0x3fffffe
    dup v5.2s, w3
    mov w3, #0xffda
    movk w3, #0x7ff, lsl #16
    dup v6.2s, w3
    ldp d0, d1, [x1, #0]
    ldp d2, d3, [x2, #0]
    add v0.2s, v0.2s, v6.2s
    add v1.2s, v1.2s, v5.2s
    sub v0.2s, v0.2s, v2.2s
    sub v1.2s, v1.2s, v3.2s
    stp d0, d1, [x0, #0]
    ldp d0, d1, [x1, #16]
    ldp d2, d3, [x2, #16]
    add v0.2s, v0.2s, v4.2s
    add v1.2s, v1.2s, v5.2s
    sub v0.2s, v0.2s, v2.2s
    sub v1.2s, v1.2s, v3.2s
    stp d0, d1, [x0, #16]
    ldp d0, d1, [x1, #32]
    ldp d2, d3, [x2, #32]
    add v0.2s, v0.2s, v4.2s
    add v1.2s, v1.2s, v5.2s
    sub v0.2s, v0.2s, v2.2s
    sub v1.2s, v1.2s, v3.2s
    stp d0, d1, [x0, #32]
    ldp d0, d1, [x1, #48]
    ldp d2, d3, [x2, #48]
    add v0.2s, v0.2s, v4.2s
    add v1.2s, v1.2s, v5.2s
    sub v0.2s, v0.2s, v2.2s
    sub v1.2s, v1.2s, v3.2s
    stp d0, d1, [x0, #48]
    ldp d0, d1, [x1, #64]
    ldp d2, d3, [x2, #64]
    add v0.2s, v0.2s, v4.2s
    add v1.2s, v1.2s, v5.2s
    sub v0.2s, v0.2s, v2.2s
    sub v1.2s, v1.2s, v3.2s
    stp d0, d1, [x0, #64]
    ret
.cfi_endproc
.size .Lx25519_sub2, .-.Lx25519_sub2

.p2align 4
.type .Lx25519_zip1, %function
.Lx25519_zip1:
.cfi_startproc
    /* out = (a.lane0, b.lane0) */
    ldp d0, d1, [x1, #0]
    ldp d2, d3, [x2, #0]
    zip1 v0.2s, v0.2s, v2.2s
    zip1 v1.2s, v1.2s, v3.2s
    stp d0, d1, [x0, #0]
    ldp d0, d1, [x1, #16]
    ldp d2, d3, [x2, #16]
    zip1 v0.2s, v0.2s, v2.2s
    zip1 v1.2s, v1.2s, v3.2s
    stp d0, d1, [x0, #16]
    ldp d0, d1, [x1, #32]
    ldp d2, d3, [x2, #32]
    zip1 v0.2s, v0.2s, v2.2s
    zip1 v1.2s, v1.2s, v3.2s
    stp d0, d1, [x0, #32]
    ldp d0, d1, [x1, #48]
    ldp d2, d3, [x2, #48]
    zip1 v0.2s, v0.2s, v2.2s
    zip1 v1.2s, v1.2s, v3.2s
    stp d0, d1, [x0, #48]
    ldp d0, d1, [x1, #64]
    ldp d2, d3, [x2, #64]
    zip1 v0.2s, v0.2s, v2.2s
    zip1 v1.2s, v1.2s, v3.2s
    stp d0, d1, [x0, #64]
    ret
.cfi_endproc
.size .Lx25519_zip1, .-.Lx25519_zip1

.p2align 4
.type .Lx25519_rev, %function
.Lx25519_rev:
.cfi_startproc
    /* out = a with its lanes exchanged */
    ldp d0, d1, [x1, #0]
    rev64 v0.2s, v0.2s
    rev64 v1.2s, v1.2s
    stp d0, d1, [x0, #0]
    ldp d0, d1, [x1, #16]
    rev64 v0.2s, v0.2s
    rev64 v1.2s, v1.2s
    stp d0, d1, [x0, #16]
    ldp d0, d1, [x1, #32]
    rev64 v0.2s, v0.2s
    rev64 v1.2s, v1.2s
    stp d0, d1, [x0, #32]
    ldp d0, d1, [x1, #48]
    rev64 v0.2s, v0.2s
    rev64 v1.2s, v1.2s
    stp d0, d1, [x0, #48]
    ldp d0, d1, [x1, #64]
    rev64 v0.2s, v0.2s
    rev64 v1.2s, v1.2s
    stp d0, d1, [x0, #64]
    ret
.cfi_endproc
.size .Lx25519_rev, .-.Lx25519_rev

.p2align 4
.type .Lx25519_sumdiff, %function
.Lx25519_sumdiff:
.cfi_startproc
    /* out = (a0 + a1, a0 + 2p - a1) */
    ldp w3, w4, [x1, #0]
    add w5, w3, w4
    mov w6, #0xffda
    movk w6, #0x7ff, lsl #16
    add w3, w3, w6
    sub w3, w3, w4
    stp w5, w3, [x0, #0]
    ldp w3, w4, [x1, #8]
    add w5, w3, w4
    mov w6, #0xfffe
    movk w6, #0x3ff, lsl #16
    add w3, w3, w6
    sub w3, w3, w4
    stp w5, w3, [x0, #8]
    ldp w3, w4, [x1, #16]
    add w5, w3, w4
    mov w6, #0xfffe
    movk w6, #0x7ff, lsl #16
    add w3, w3, w6
    sub w3, w3, w4
    stp w5, w3, [x0, #16]
    ldp w3, w4, [x1, #24]
    add w5, w3, w4
    mov w6, #0xfffe
    movk w6, #0x3ff, lsl #16
    add w3, w3, w6
    sub w3, w3, w4
    stp w5, w3, [x0, #24]
    ldp w3, w4, [x1, #32]
    add w5, w3, w4
    mov w6, #0xfffe
    movk w6, #0x7ff, lsl #16
    add w3, w3, w6
    sub w3, w3, w4
    stp w5, w3, [x0, #32]
    ldp w3, w4, [x1, #40]
    add w5, w3, w4
    mov w6, #0xfffe
    movk w6, #0x3ff, lsl #16
    add w3, w3, w6
    sub w3, w3, w4
    stp w5, w3, [x0, #40]
    ldp w3, w4, [x1, #48]
    add w5, w3, w4
    mov w6, #0xfffe
    movk w6, #0x7ff, lsl #16
    add w3, w3, w6
    sub w3, w3, w4
    stp w5, w3, [x0, #48]
    ldp w3, w4, [x1, #56]
    add w5, w3, w4
    mov w6, #0xfffe
    movk w6, #0x3ff, lsl #16
    add w3, w3, w6
    sub w3, w3, w4
    stp w5, w3, [x0, #56]
    ldp w3, w4, [x1, #64]
    add w5, w3, w4
    mov w6, #0xfffe
    movk w6, #0x7ff, lsl #16
    add w3, w3, w6
    sub w3, w3, w4
    stp w5, w3, [x0, #64]
    ldp w3, w4, [x1, #72]
    add w5, w3, w4
    mov w6, #0xfffe
    movk w6, #0x3ff, lsl #16
    add w3, w3, w6
    sub w3, w3, w4
    stp w5, w3, [x0, #72]
    ret
.cfi_endproc
.size .Lx25519_sumdiff, .-.Lx25519_sumdiff

.p2align 4
.type .Lx25519_ediff, %function
.Lx25519_ediff:
.cfi_startproc
    /* lane 0 of out = a0 + 2p - a1, lane 1 = 0 */
    ldp w3, w4, [x1, #0]
    mov w6, #0xffda
    movk w6, #0x7ff, lsl #16
    add w3, w3, w6
    sub w3, w3, w4
    str x3, [x0, #0]
    ldp w3, w4, [x1, #8]
    mov w6, #0xfffe
    movk w6, #0x3ff, lsl #16
    add w3, w3, w6
    sub w3, w3, w4
    str x3, [x0, #8]
    ldp w3, w4, [x1, #16]
    mov w6, #0xfffe
    movk w6, #0x7ff, lsl #16
    add w3, w3, w6
    sub w3, w3, w4
    str x3, [x0, #16]
    ldp w3, w4, [x1, #24]
    mov w6, #0xfffe
    movk w6, #0x3ff, lsl #16
    add w3, w3, w6
    sub w3, w3, w4
    str x3, [x0, #24]
    ldp w3, w4, [x1, #32]
    mov w6, #0xfffe
    movk w6, #0x7ff, lsl #16
    add w3, w3, w6
    sub w3, w3, w4
    str x3, [x0, #32]
    ldp w3, w4, [x1, #40]
    mov w6, #0xfffe
    movk w6, #0x3ff, lsl #16
    add w3, w3, w6
    sub w3, w3, w4
    str x3, [x0, #40]
    ldp w3, w4, [x1, #48]
    mov w6, #0xfffe
    movk w6, #0x7ff, lsl #16
    add w3, w3, w6
    sub w3, w3, w4
    str x3, [x0, #48]
    ldp w3, w4, [x1, #56]
    mov w6, #0xfffe
    movk w6, #0x3ff, lsl #16
    add w3, w3, w6
    sub w3, w3, w4
    str x3, [x0, #56]
    ldp w3, w4, [x1, #64]
    mov w6, #0xfffe
    movk w6, #0x7ff, lsl #16
    add w3, w3, w6
    sub w3, w3, w4
    str x3, [x0, #64]
    ldp w3, w4, [x1, #72]
    mov w6, #0xfffe
    movk w6, #0x3ff, lsl #16
    add w3, w3, w6
    sub w3, w3, w4
    str x3, [x0, #72]
    ret
.cfi_endproc
.size .Lx25519_ediff, .-.Lx25519_ediff

.p2align 4
.type .Lx25519_scale, %function
.Lx25519_scale:
.cfi_startproc
    /* x0 = W, x1 = E, x2 = AA, lanes 0: W = AA + 121665*E, carried once */
    mov w5, #0xdb41
    movk w5, #1, lsl #16
    ldr w7, [x1, #0]
    ldr w3, [x2, #0]
    umaddl x7, w7, w5, x3
    ldr w8, [x1, #8]
    ldr w3, [x2, #8]
    umaddl x8, w8, w5, x3
    ldr w9, [x1, #16]
    ldr w3, [x2, #16]
    umaddl x9, w9, w5, x3
    ldr w10, [x1, #24]
    ldr w3, [x2, #24]
    umaddl x10, w10, w5, x3
    ldr w11, [x1, #32]
    ldr w3, [x2, #32]
    umaddl x11, w11, w5, x3
    ldr w12, [x1, #40]
    ldr w3, [x2, #40]
    umaddl x12, w12, w5, x3
    ldr w13, [x1, #48]
    ldr w3, [x2, #48]
    umaddl x13, w13, w5, x3
    ldr w14, [x1, #56]
    ldr w3, [x2, #56]
    umaddl x14, w14, w5, x3
    ldr w15, [x1, #64]
    ldr w3, [x2, #64]
    umaddl x15, w15, w5, x3
    ldr w16, [x1, #72]
    ldr w3, [x2, #72]
    umaddl x16, w16, w5, x3
    add x8, x8, x7, lsr #26
    and x7, x7, #0x3ffffff
    add x9, x9, x8, lsr #25
    and x8, x8, #0x1ffffff
    add x10, x10, x9, lsr #26
    and x9, x9, #0x3ffffff
    add x11, x11, x10, lsr #25
    and x10, x10, #0x1ffffff
    add x12, x12, x11, lsr #26
    and x11, x11, #0x3ffffff
    add x13, x13, x12, lsr #25
    and x12, x12, #0x1ffffff
    add x14, x14, x13, lsr #26
    and x13, x13, #0x3ffffff
    add x15, x15, x14, lsr #25
    and x14, x14, #0x1ffffff
    add x16, x16, x15, lsr #26
    and x15, x15, #0x3ffffff
    lsr x3, x16, #25
    and x16, x16, #0x1ffffff
    mov w4, #19
    umaddl x7, w3, w4, x7
    str x7, [x0, #0]
    str x8, [x0, #8]
    str x9, [x0, #16]
    str x10, [x0, #24]
    str x11, [x0, #32]
    str x12, [x0, #40]
    str x13, [x0, #48]
    str x14, [x0, #56]
    str x15, [x0, #64]
    str x16, [x0, #72]
    ret
.cfi_endproc
.size .Lx25519_scale, .-.Lx25519_scale

.p2align 4
.type .Lx25519_mix, %function
.Lx25519_mix:
.cfi_startproc
    /* P = (AA, x1) and S = (BB, U.lane1), x1 = T and x2 = U on entry */
    add x6, sp, #320
    add x7, sp, #640
    ldp w3, w4, [x1, #0]
    ldr w5, [x7, #0]
    stp w3, w5, [x0, #0]
    ldr w5, [x2, #4]
    stp w4, w5, [x6, #0]
    ldp w3, w4, [x1, #8]
    ldr w5, [x7, #8]
    stp w3, w5, [x0, #8]
    ldr w5, [x2, #12]
    stp w4, w5, [x6, #8]
    ldp w3, w4, [x1, #16]
    ldr w5, [x7, #16]
    stp w3, w5, [x0, #16]
    ldr w5, [x2, #20]
    stp w4, w5, [x6, #16]
    ldp w3, w4, [x1, #24]
    ldr w5, [x7, #24]
    stp w3, w5, [x0, #24]
    ldr w5, [x2, #28]
    stp w4, w5, [x6, #24]
    ldp w3, w4, [x1, #32]
    ldr w5, [x7, #32]
    stp w3, w5, [x0, #32]
    ldr w5, [x2, #36]
    stp w4, w5, [x6, #32]
    ldp w3, w4, [x1, #40]
    ldr w5, [x7, #40]
    stp w3, w5, [x0, #40]
    ldr w5, [x2, #44]
    stp w4, w5, [x6, #40]
    ldp w3, w4, [x1, #48]
    ldr w5, [x7, #48]
    stp w3, w5, [x0, #48]
    ldr w5, [x2, #52]
    stp w4, w5, [x6, #48]
    ldp w3, w4, [x1, #56]
    ldr w5, [x7, #56]
    stp w3, w5, [x0, #56]
    ldr w5, [x2, #60]
    stp w4, w5, [x6, #56]
    ldp w3, w4, [x1, #64]
    ldr w5, [x7, #64]
    stp w3, w5, [x0, #64]
    ldr w5, [x2, #68]
    stp w4, w5, [x6, #64]
    ldp w3, w4, [x1, #72]
    ldr w5, [x7, #72]
    stp w3, w5, [x0, #72]
    ldr w5, [x2, #76]
    stp w4, w5, [x6, #72]
    ret
.cfi_endproc
.size .Lx25519_mix, .-.Lx25519_mix

.p2align 4
.global sharkssl_x25519_arm64
.hidden sharkssl_x25519_arm64
.type sharkssl_x25519_arm64, %function
sharkssl_x25519_arm64:
.cfi_startproc
    stp x29, x30, [sp, #-160]!
.cfi_def_cfa_offset 160
.cfi_offset 29, -160
.cfi_offset 30, -152
    mov x29, sp
    stp x19, x20, [sp, #16]
.cfi_offset 19, -144
.cfi_offset 20, -136
    stp x21, x22, [sp, #32]
.cfi_offset 21, -128
.cfi_offset 22, -120
    stp d8, d9, [sp, #48]
.cfi_offset 72, -112
.cfi_offset 73, -104
    stp d10, d11, [sp, #64]
.cfi_offset 74, -96
.cfi_offset 75, -88
    stp d12, d13, [sp, #80]
.cfi_offset 76, -80
.cfi_offset 77, -72
    stp d14, d15, [sp, #96]
.cfi_offset 78, -64
.cfi_offset 79, -56
    mov x19, x0
    /* The scalar goes to the frame: inputs and output may alias. */
    ldp x3, x4, [x1]
    ldp x5, x6, [x1, #16]
    stp x3, x4, [sp, #112]
    stp x5, x6, [sp, #128]
    sub sp, sp, #720
.cfi_def_cfa_offset 880
    /* u to limbs at bits 0, 26, 51, 77, 102, 128, 153, 179, 204, 230 */
    ldp x3, x4, [x2]
    ldp x5, x6, [x2, #16]
    add x8, sp, #640
    and x7, x3, #0x3ffffff
    str w7, [x8, #0]
    ubfx x7, x3, #26, #25
    str w7, [x8, #8]
    extr x7, x4, x3, #51
    and x7, x7, #0x3ffffff
    str w7, [x8, #16]
    ubfx x7, x4, #13, #25
    str w7, [x8, #24]
    ubfx x7, x4, #38, #26
    str w7, [x8, #32]
    and x7, x5, #0x1ffffff
    str w7, [x8, #40]
    ubfx x7, x5, #25, #26
    str w7, [x8, #48]
    extr x7, x6, x5, #51
    and x7, x7, #0x1ffffff
    str w7, [x8, #56]
    ubfx x7, x6, #12, #26
    str w7, [x8, #64]
    ubfx x7, x6, #38, #25
    str w7, [x8, #72]
    /* X = (1, u), Z = (0, 1) */
    ldr w7, [x8, #0]
    lsl x7, x7, #32
    orr x7, x7, #1
    str x7, [sp, #0]
    mov x7, #0x100000000
    str x7, [sp, #80]
    ldr w7, [x8, #8]
    lsl x7, x7, #32
    str x7, [sp, #8]
    str xzr, [sp, #88]
    ldr w7, [x8, #16]
    lsl x7, x7, #32
    str x7, [sp, #16]
    str xzr, [sp, #96]
    ldr w7, [x8, #24]
    lsl x7, x7, #32
    str x7, [sp, #24]
    str xzr, [sp, #104]
    ldr w7, [x8, #32]
    lsl x7, x7, #32
    str x7, [sp, #32]
    str xzr, [sp, #112]
    ldr w7, [x8, #40]
    lsl x7, x7, #32
    str x7, [sp, #40]
    str xzr, [sp, #120]
    ldr w7, [x8, #48]
    lsl x7, x7, #32
    str x7, [sp, #48]
    str xzr, [sp, #128]
    ldr w7, [x8, #56]
    lsl x7, x7, #32
    str x7, [sp, #56]
    str xzr, [sp, #136]
    ldr w7, [x8, #64]
    lsl x7, x7, #32
    str x7, [sp, #64]
    str xzr, [sp, #144]
    ldr w7, [x8, #72]
    lsl x7, x7, #32
    str x7, [sp, #72]
    str xzr, [sp, #152]
    mov w20, #255
    mov w21, #0
    add x22, x29, #112
.Lx25519_ladder:
    /* Exchange the lanes of X and Z when the bit differs from the previous one. */
    lsr w3, w20, #3
    ldrb w3, [x22, x3]
    and w4, w20, #7
    lsr w3, w3, w4
    and w3, w3, #1
    eor w4, w3, w21
    mov w21, w3
    neg w4, w4
    dup v16.2s, w4
    ldr d0, [sp, #0]
    rev64 v1.2s, v0.2s
    bit v0.8b, v1.8b, v16.8b
    str d0, [sp, #0]
    ldr d0, [sp, #80]
    rev64 v1.2s, v0.2s
    bit v0.8b, v1.8b, v16.8b
    str d0, [sp, #80]
    ldr d0, [sp, #8]
    rev64 v1.2s, v0.2s
    bit v0.8b, v1.8b, v16.8b
    str d0, [sp, #8]
    ldr d0, [sp, #88]
    rev64 v1.2s, v0.2s
    bit v0.8b, v1.8b, v16.8b
    str d0, [sp, #88]
    ldr d0, [sp, #16]
    rev64 v1.2s, v0.2s
    bit v0.8b, v1.8b, v16.8b
    str d0, [sp, #16]
    ldr d0, [sp, #96]
    rev64 v1.2s, v0.2s
    bit v0.8b, v1.8b, v16.8b
    str d0, [sp, #96]
    ldr d0, [sp, #24]
    rev64 v1.2s, v0.2s
    bit v0.8b, v1.8b, v16.8b
    str d0, [sp, #24]
    ldr d0, [sp, #104]
    rev64 v1.2s, v0.2s
    bit v0.8b, v1.8b, v16.8b
    str d0, [sp, #104]
    ldr d0, [sp, #32]
    rev64 v1.2s, v0.2s
    bit v0.8b, v1.8b, v16.8b
    str d0, [sp, #32]
    ldr d0, [sp, #112]
    rev64 v1.2s, v0.2s
    bit v0.8b, v1.8b, v16.8b
    str d0, [sp, #112]
    ldr d0, [sp, #40]
    rev64 v1.2s, v0.2s
    bit v0.8b, v1.8b, v16.8b
    str d0, [sp, #40]
    ldr d0, [sp, #120]
    rev64 v1.2s, v0.2s
    bit v0.8b, v1.8b, v16.8b
    str d0, [sp, #120]
    ldr d0, [sp, #48]
    rev64 v1.2s, v0.2s
    bit v0.8b, v1.8b, v16.8b
    str d0, [sp, #48]
    ldr d0, [sp, #128]
    rev64 v1.2s, v0.2s
    bit v0.8b, v1.8b, v16.8b
    str d0, [sp, #128]
    ldr d0, [sp, #56]
    rev64 v1.2s, v0.2s
    bit v0.8b, v1.8b, v16.8b
    str d0, [sp, #56]
    ldr d0, [sp, #136]
    rev64 v1.2s, v0.2s
    bit v0.8b, v1.8b, v16.8b
    str d0, [sp, #136]
    ldr d0, [sp, #64]
    rev64 v1.2s, v0.2s
    bit v0.8b, v1.8b, v16.8b
    str d0, [sp, #64]
    ldr d0, [sp, #144]
    rev64 v1.2s, v0.2s
    bit v0.8b, v1.8b, v16.8b
    str d0, [sp, #144]
    ldr d0, [sp, #72]
    rev64 v1.2s, v0.2s
    bit v0.8b, v1.8b, v16.8b
    str d0, [sp, #72]
    ldr d0, [sp, #152]
    rev64 v1.2s, v0.2s
    bit v0.8b, v1.8b, v16.8b
    str d0, [sp, #152]
    /* (A, C) = X + Z; (B, D) = X - Z */
    add x0, sp, #160
    add x1, sp, #0
    add x2, sp, #80
    bl .Lx25519_add2
    add x0, sp, #240
    add x1, sp, #0
    add x2, sp, #80
    bl .Lx25519_sub2
    /* S = (DA, CB) = (A, C) * (D, B) */
    add x0, sp, #400
    add x1, sp, #240
    bl .Lx25519_rev
    add x0, sp, #320
    add x1, sp, #160
    add x2, sp, #400
    bl .Lx25519_mul2
    /* T = (AA, BB) */
    add x0, sp, #400
    add x1, sp, #160
    add x2, sp, #240
    bl .Lx25519_zip1
    add x0, sp, #400
    add x1, sp, #400
    bl .Lx25519_sq2
    /* U = (DA + CB, DA - CB), squared: (x3, V) */
    add x0, sp, #480
    add x1, sp, #320
    bl .Lx25519_sumdiff
    add x0, sp, #480
    add x1, sp, #480
    bl .Lx25519_sq2
    /* R = (E, 0) with E = AA - BB; Q = (W, 0) with W = AA + 121665*E */
    add x0, sp, #560
    add x1, sp, #400
    bl .Lx25519_ediff
    add x0, sp, #240
    add x1, sp, #560
    add x2, sp, #400
    bl .Lx25519_scale
    /* X = (x2, z3) = (AA, x1) * (BB, V); Z = (z2, 0) = (E, 0) * (W, 0) */
    add x0, sp, #160
    add x1, sp, #400
    add x2, sp, #480
    bl .Lx25519_mix
    add x0, sp, #0
    add x1, sp, #160
    add x2, sp, #320
    bl .Lx25519_mul2
    add x0, sp, #80
    add x1, sp, #560
    add x2, sp, #240
    bl .Lx25519_mul2
    /* X = (x2, x3), Z = (z2, z3) */
    ldr d0, [sp, #0]
    ldr d1, [sp, #80]
    ldr d2, [sp, #480]
    ins v1.s[1], v0.s[1]
    ins v0.s[1], v2.s[0]
    str d0, [sp, #0]
    str d1, [sp, #80]
    ldr d0, [sp, #8]
    ldr d1, [sp, #88]
    ldr d2, [sp, #488]
    ins v1.s[1], v0.s[1]
    ins v0.s[1], v2.s[0]
    str d0, [sp, #8]
    str d1, [sp, #88]
    ldr d0, [sp, #16]
    ldr d1, [sp, #96]
    ldr d2, [sp, #496]
    ins v1.s[1], v0.s[1]
    ins v0.s[1], v2.s[0]
    str d0, [sp, #16]
    str d1, [sp, #96]
    ldr d0, [sp, #24]
    ldr d1, [sp, #104]
    ldr d2, [sp, #504]
    ins v1.s[1], v0.s[1]
    ins v0.s[1], v2.s[0]
    str d0, [sp, #24]
    str d1, [sp, #104]
    ldr d0, [sp, #32]
    ldr d1, [sp, #112]
    ldr d2, [sp, #512]
    ins v1.s[1], v0.s[1]
    ins v0.s[1], v2.s[0]
    str d0, [sp, #32]
    str d1, [sp, #112]
    ldr d0, [sp, #40]
    ldr d1, [sp, #120]
    ldr d2, [sp, #520]
    ins v1.s[1], v0.s[1]
    ins v0.s[1], v2.s[0]
    str d0, [sp, #40]
    str d1, [sp, #120]
    ldr d0, [sp, #48]
    ldr d1, [sp, #128]
    ldr d2, [sp, #528]
    ins v1.s[1], v0.s[1]
    ins v0.s[1], v2.s[0]
    str d0, [sp, #48]
    str d1, [sp, #128]
    ldr d0, [sp, #56]
    ldr d1, [sp, #136]
    ldr d2, [sp, #536]
    ins v1.s[1], v0.s[1]
    ins v0.s[1], v2.s[0]
    str d0, [sp, #56]
    str d1, [sp, #136]
    ldr d0, [sp, #64]
    ldr d1, [sp, #144]
    ldr d2, [sp, #544]
    ins v1.s[1], v0.s[1]
    ins v0.s[1], v2.s[0]
    str d0, [sp, #64]
    str d1, [sp, #144]
    ldr d0, [sp, #72]
    ldr d1, [sp, #152]
    ldr d2, [sp, #552]
    ins v1.s[1], v0.s[1]
    ins v0.s[1], v2.s[0]
    str d0, [sp, #72]
    str d1, [sp, #152]
    subs w20, w20, #1
    b.hs .Lx25519_ladder
    /* The last exchange */
    neg w4, w21
    dup v16.2s, w4
    ldr d0, [sp, #0]
    rev64 v1.2s, v0.2s
    bit v0.8b, v1.8b, v16.8b
    str d0, [sp, #0]
    ldr d0, [sp, #80]
    rev64 v1.2s, v0.2s
    bit v0.8b, v1.8b, v16.8b
    str d0, [sp, #80]
    ldr d0, [sp, #8]
    rev64 v1.2s, v0.2s
    bit v0.8b, v1.8b, v16.8b
    str d0, [sp, #8]
    ldr d0, [sp, #88]
    rev64 v1.2s, v0.2s
    bit v0.8b, v1.8b, v16.8b
    str d0, [sp, #88]
    ldr d0, [sp, #16]
    rev64 v1.2s, v0.2s
    bit v0.8b, v1.8b, v16.8b
    str d0, [sp, #16]
    ldr d0, [sp, #96]
    rev64 v1.2s, v0.2s
    bit v0.8b, v1.8b, v16.8b
    str d0, [sp, #96]
    ldr d0, [sp, #24]
    rev64 v1.2s, v0.2s
    bit v0.8b, v1.8b, v16.8b
    str d0, [sp, #24]
    ldr d0, [sp, #104]
    rev64 v1.2s, v0.2s
    bit v0.8b, v1.8b, v16.8b
    str d0, [sp, #104]
    ldr d0, [sp, #32]
    rev64 v1.2s, v0.2s
    bit v0.8b, v1.8b, v16.8b
    str d0, [sp, #32]
    ldr d0, [sp, #112]
    rev64 v1.2s, v0.2s
    bit v0.8b, v1.8b, v16.8b
    str d0, [sp, #112]
    ldr d0, [sp, #40]
    rev64 v1.2s, v0.2s
    bit v0.8b, v1.8b, v16.8b
    str d0, [sp, #40]
    ldr d0, [sp, #120]
    rev64 v1.2s, v0.2s
    bit v0.8b, v1.8b, v16.8b
    str d0, [sp, #120]
    ldr d0, [sp, #48]
    rev64 v1.2s, v0.2s
    bit v0.8b, v1.8b, v16.8b
    str d0, [sp, #48]
    ldr d0, [sp, #128]
    rev64 v1.2s, v0.2s
    bit v0.8b, v1.8b, v16.8b
    str d0, [sp, #128]
    ldr d0, [sp, #56]
    rev64 v1.2s, v0.2s
    bit v0.8b, v1.8b, v16.8b
    str d0, [sp, #56]
    ldr d0, [sp, #136]
    rev64 v1.2s, v0.2s
    bit v0.8b, v1.8b, v16.8b
    str d0, [sp, #136]
    ldr d0, [sp, #64]
    rev64 v1.2s, v0.2s
    bit v0.8b, v1.8b, v16.8b
    str d0, [sp, #64]
    ldr d0, [sp, #144]
    rev64 v1.2s, v0.2s
    bit v0.8b, v1.8b, v16.8b
    str d0, [sp, #144]
    ldr d0, [sp, #72]
    rev64 v1.2s, v0.2s
    bit v0.8b, v1.8b, v16.8b
    str d0, [sp, #72]
    ldr d0, [sp, #152]
    rev64 v1.2s, v0.2s
    bit v0.8b, v1.8b, v16.8b
    str d0, [sp, #152]
    /* z^(p-2): the addition chain of fe_invert in SharkSslBigInt.c, then x*z */
    add x0, sp, #160
    add x1, sp, #80
    bl .Lx25519_sq2
    add x0, sp, #240
    add x1, sp, #160
    bl .Lx25519_sq2
    mov w20, #1
.Lx25519_inv0:
    add x0, sp, #240
    add x1, sp, #240
    bl .Lx25519_sq2
    subs w20, w20, #1
    b.ne .Lx25519_inv0
    add x0, sp, #240
    add x1, sp, #80
    add x2, sp, #240
    bl .Lx25519_mul2
    add x0, sp, #160
    add x1, sp, #160
    add x2, sp, #240
    bl .Lx25519_mul2
    add x0, sp, #320
    add x1, sp, #160
    bl .Lx25519_sq2
    add x0, sp, #240
    add x1, sp, #240
    add x2, sp, #320
    bl .Lx25519_mul2
    add x0, sp, #320
    add x1, sp, #240
    bl .Lx25519_sq2
    mov w20, #4
.Lx25519_inv1:
    add x0, sp, #320
    add x1, sp, #320
    bl .Lx25519_sq2
    subs w20, w20, #1
    b.ne .Lx25519_inv1
    add x0, sp, #240
    add x1, sp, #320
    add x2, sp, #240
    bl .Lx25519_mul2
    add x0, sp, #320
    add x1, sp, #240
    bl .Lx25519_sq2
    mov w20, #9
.Lx25519_inv2:
    add x0, sp, #320
    add x1, sp, #320
    bl .Lx25519_sq2
    subs w20, w20, #1
    b.ne .Lx25519_inv2
    add x0, sp, #320
    add x1, sp, #320
    add x2, sp, #240
    bl .Lx25519_mul2
    add x0, sp, #400
    add x1, sp, #320
    bl .Lx25519_sq2
    mov w20, #19
.Lx25519_inv3:
    add x0, sp, #400
    add x1, sp, #400
    bl .Lx25519_sq2
    subs w20, w20, #1
    b.ne .Lx25519_inv3
    add x0, sp, #320
    add x1, sp, #400
    add x2, sp, #320
    bl .Lx25519_mul2
    add x0, sp, #320
    add x1, sp, #320
    bl .Lx25519_sq2
    mov w20, #9
.Lx25519_inv4:
    add x0, sp, #320
    add x1, sp, #320
    bl .Lx25519_sq2
    subs w20, w20, #1
    b.ne .Lx25519_inv4
    add x0, sp, #240
    add x1, sp, #320
    add x2, sp, #240
    bl .Lx25519_mul2
    add x0, sp, #320
    add x1, sp, #240
    bl .Lx25519_sq2
    mov w20, #49
.Lx25519_inv5:
    add x0, sp, #320
    add x1, sp, #320
    bl .Lx25519_sq2
    subs w20, w20, #1
    b.ne .Lx25519_inv5
    add x0, sp, #320
    add x1, sp, #320
    add x2, sp, #240
    bl .Lx25519_mul2
    add x0, sp, #400
    add x1, sp, #320
    bl .Lx25519_sq2
    mov w20, #99
.Lx25519_inv6:
    add x0, sp, #400
    add x1, sp, #400
    bl .Lx25519_sq2
    subs w20, w20, #1
    b.ne .Lx25519_inv6
    add x0, sp, #320
    add x1, sp, #400
    add x2, sp, #320
    bl .Lx25519_mul2
    add x0, sp, #320
    add x1, sp, #320
    bl .Lx25519_sq2
    mov w20, #49
.Lx25519_inv7:
    add x0, sp, #320
    add x1, sp, #320
    bl .Lx25519_sq2
    subs w20, w20, #1
    b.ne .Lx25519_inv7
    add x0, sp, #240
    add x1, sp, #320
    add x2, sp, #240
    bl .Lx25519_mul2
    add x0, sp, #240
    add x1, sp, #240
    bl .Lx25519_sq2
    mov w20, #4
.Lx25519_inv8:
    add x0, sp, #240
    add x1, sp, #240
    bl .Lx25519_sq2
    subs w20, w20, #1
    b.ne .Lx25519_inv8
    add x0, sp, #80
    add x1, sp, #240
    add x2, sp, #160
    bl .Lx25519_mul2
    add x0, sp, #80
    add x1, sp, #0
    add x2, sp, #80
    bl .Lx25519_mul2
    /* Freeze lane 0 below p, as fe_tobytes in SharkSslBigInt.c, with no branch. */
    ldr w3, [sp, #80]
    ldr w4, [sp, #88]
    ldr w5, [sp, #96]
    ldr w6, [sp, #104]
    ldr w7, [sp, #112]
    ldr w8, [sp, #120]
    ldr w9, [sp, #128]
    ldr w10, [sp, #136]
    ldr w11, [sp, #144]
    ldr w12, [sp, #152]
    mov w14, #19
    mul w13, w12, w14
    mov w15, #0x1000000
    add w13, w13, w15
    asr w13, w13, #25
    add w13, w3, w13
    asr w13, w13, #26
    add w13, w4, w13
    asr w13, w13, #25
    add w13, w5, w13
    asr w13, w13, #26
    add w13, w6, w13
    asr w13, w13, #25
    add w13, w7, w13
    asr w13, w13, #26
    add w13, w8, w13
    asr w13, w13, #25
    add w13, w9, w13
    asr w13, w13, #26
    add w13, w10, w13
    asr w13, w13, #25
    add w13, w11, w13
    asr w13, w13, #26
    add w13, w12, w13
    asr w13, w13, #25
    mul w13, w13, w14
    add w3, w3, w13
    asr w13, w3, #26
    and w3, w3, #0x3ffffff
    add w4, w4, w13
    asr w13, w4, #25
    and w4, w4, #0x1ffffff
    add w5, w5, w13
    asr w13, w5, #26
    and w5, w5, #0x3ffffff
    add w6, w6, w13
    asr w13, w6, #25
    and w6, w6, #0x1ffffff
    add w7, w7, w13
    asr w13, w7, #26
    and w7, w7, #0x3ffffff
    add w8, w8, w13
    asr w13, w8, #25
    and w8, w8, #0x1ffffff
    add w9, w9, w13
    asr w13, w9, #26
    and w9, w9, #0x3ffffff
    add w10, w10, w13
    asr w13, w10, #25
    and w10, w10, #0x1ffffff
    add w11, w11, w13
    asr w13, w11, #26
    and w11, w11, #0x3ffffff
    add w12, w12, w13
    and w12, w12, #0x1ffffff
    orr x3, x3, x4, lsl #26
    orr x3, x3, x5, lsl #51
    lsr x5, x5, #13
    orr x5, x5, x6, lsl #13
    orr x5, x5, x7, lsl #38
    orr x8, x8, x9, lsl #25
    orr x8, x8, x10, lsl #51
    lsr x10, x10, #13
    orr x10, x10, x11, lsl #12
    orr x10, x10, x12, lsl #38
    stp x3, x5, [x19]
    stp x8, x10, [x19, #16]
    mov x0, sp
    mov w1, #45
.Lx25519_wipe:
    stp xzr, xzr, [x0], #16
    subs w1, w1, #1
    b.ne .Lx25519_wipe
    add sp, sp, #720
.cfi_def_cfa_offset 160
    stp xzr, xzr, [sp, #112]
    stp xzr, xzr, [sp, #128]
    movi v0.16b, #0
    movi v1.16b, #0
    movi v2.16b, #0
    movi v3.16b, #0
    movi v4.16b, #0
    movi v5.16b, #0
    movi v6.16b, #0
    movi v7.16b, #0
    movi v8.16b, #0
    movi v9.16b, #0
    movi v10.16b, #0
    movi v11.16b, #0
    movi v12.16b, #0
    movi v13.16b, #0
    movi v14.16b, #0
    movi v15.16b, #0
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
    mov x0, xzr
    mov x1, xzr
    mov x2, xzr
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
    ldp d8, d9, [sp, #48]
.cfi_restore 72
.cfi_restore 73
    ldp d10, d11, [sp, #64]
.cfi_restore 74
.cfi_restore 75
    ldp d12, d13, [sp, #80]
.cfi_restore 76
.cfi_restore 77
    ldp d14, d15, [sp, #96]
.cfi_restore 78
.cfi_restore 79
    ldp x21, x22, [sp, #32]
.cfi_restore 21
.cfi_restore 22
    ldp x19, x20, [sp, #16]
.cfi_restore 19
.cfi_restore 20
    ldp x29, x30, [sp], #160
.cfi_restore 29
.cfi_restore 30
.cfi_def_cfa_offset 0
    ret
.cfi_endproc
.size sharkssl_x25519_arm64, .-sharkssl_x25519_arm64
#endif
.section .note.GNU-stack,"",%progbits
