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
 *   $Id: SharkSslSha512_ARM64.S 6818 2026-11-08 21:18:02Z gianluca $
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
#if ((SHARKSSL_USE_SHA_384 || SHARKSSL_USE_SHA_512) && SHARKSSL_OPTIMIZED_SHA384_ASM)
/* One SHA-384/512 block on integer registers. The Cortex-A76 has no SHA-512
 * instructions, and the rotate-in-operand forms of this code run three times
 * faster than the compiled C loop. Addresses and branches depend only on the
 * round number. The message schedule and the registers are cleared: in HMAC and
 * HKDF they are key material.
 * x0 ctx (state at offset 16), x1 data. */
.p2align 4
.global SharkSslSha384Ctx_process
.hidden SharkSslSha384Ctx_process
.type SharkSslSha384Ctx_process, %function
SharkSslSha384Ctx_process:
.cfi_startproc
    sub sp, sp, #128
.cfi_def_cfa_offset 128
    add x2, x0, #16
    ldp x3, x4, [x2]
    ldp x5, x6, [x2, #16]
    ldp x7, x8, [x2, #32]
    ldp x9, x10, [x2, #48]
    adrp x11, .Lsha512_k
    add x11, x11, :lo12:.Lsha512_k
    ldr x12, [x1], #8
    rev x12, x12
    str x12, [sp, #0]
    ldr x13, [x11], #8
    add x10, x10, x12
    add x10, x10, x13
    ror x16, x7, #14
    eor x16, x16, x7, ror #18
    eor x16, x16, x7, ror #41
    add x10, x10, x16
    eor x16, x8, x9
    and x16, x16, x7
    eor x16, x16, x9
    add x10, x10, x16
    add x6, x6, x10
    ror x16, x3, #28
    eor x16, x16, x3, ror #34
    eor x16, x16, x3, ror #39
    add x10, x10, x16
    eor x16, x3, x4
    eor x17, x4, x5
    and x16, x16, x17
    eor x16, x16, x4
    add x10, x10, x16
    ldr x12, [x1], #8
    rev x12, x12
    str x12, [sp, #8]
    ldr x13, [x11], #8
    add x9, x9, x12
    add x9, x9, x13
    ror x16, x6, #14
    eor x16, x16, x6, ror #18
    eor x16, x16, x6, ror #41
    add x9, x9, x16
    eor x16, x7, x8
    and x16, x16, x6
    eor x16, x16, x8
    add x9, x9, x16
    add x5, x5, x9
    ror x16, x10, #28
    eor x16, x16, x10, ror #34
    eor x16, x16, x10, ror #39
    add x9, x9, x16
    eor x16, x10, x3
    eor x17, x3, x4
    and x16, x16, x17
    eor x16, x16, x3
    add x9, x9, x16
    ldr x12, [x1], #8
    rev x12, x12
    str x12, [sp, #16]
    ldr x13, [x11], #8
    add x8, x8, x12
    add x8, x8, x13
    ror x16, x5, #14
    eor x16, x16, x5, ror #18
    eor x16, x16, x5, ror #41
    add x8, x8, x16
    eor x16, x6, x7
    and x16, x16, x5
    eor x16, x16, x7
    add x8, x8, x16
    add x4, x4, x8
    ror x16, x9, #28
    eor x16, x16, x9, ror #34
    eor x16, x16, x9, ror #39
    add x8, x8, x16
    eor x16, x9, x10
    eor x17, x10, x3
    and x16, x16, x17
    eor x16, x16, x10
    add x8, x8, x16
    ldr x12, [x1], #8
    rev x12, x12
    str x12, [sp, #24]
    ldr x13, [x11], #8
    add x7, x7, x12
    add x7, x7, x13
    ror x16, x4, #14
    eor x16, x16, x4, ror #18
    eor x16, x16, x4, ror #41
    add x7, x7, x16
    eor x16, x5, x6
    and x16, x16, x4
    eor x16, x16, x6
    add x7, x7, x16
    add x3, x3, x7
    ror x16, x8, #28
    eor x16, x16, x8, ror #34
    eor x16, x16, x8, ror #39
    add x7, x7, x16
    eor x16, x8, x9
    eor x17, x9, x10
    and x16, x16, x17
    eor x16, x16, x9
    add x7, x7, x16
    ldr x12, [x1], #8
    rev x12, x12
    str x12, [sp, #32]
    ldr x13, [x11], #8
    add x6, x6, x12
    add x6, x6, x13
    ror x16, x3, #14
    eor x16, x16, x3, ror #18
    eor x16, x16, x3, ror #41
    add x6, x6, x16
    eor x16, x4, x5
    and x16, x16, x3
    eor x16, x16, x5
    add x6, x6, x16
    add x10, x10, x6
    ror x16, x7, #28
    eor x16, x16, x7, ror #34
    eor x16, x16, x7, ror #39
    add x6, x6, x16
    eor x16, x7, x8
    eor x17, x8, x9
    and x16, x16, x17
    eor x16, x16, x8
    add x6, x6, x16
    ldr x12, [x1], #8
    rev x12, x12
    str x12, [sp, #40]
    ldr x13, [x11], #8
    add x5, x5, x12
    add x5, x5, x13
    ror x16, x10, #14
    eor x16, x16, x10, ror #18
    eor x16, x16, x10, ror #41
    add x5, x5, x16
    eor x16, x3, x4
    and x16, x16, x10
    eor x16, x16, x4
    add x5, x5, x16
    add x9, x9, x5
    ror x16, x6, #28
    eor x16, x16, x6, ror #34
    eor x16, x16, x6, ror #39
    add x5, x5, x16
    eor x16, x6, x7
    eor x17, x7, x8
    and x16, x16, x17
    eor x16, x16, x7
    add x5, x5, x16
    ldr x12, [x1], #8
    rev x12, x12
    str x12, [sp, #48]
    ldr x13, [x11], #8
    add x4, x4, x12
    add x4, x4, x13
    ror x16, x9, #14
    eor x16, x16, x9, ror #18
    eor x16, x16, x9, ror #41
    add x4, x4, x16
    eor x16, x10, x3
    and x16, x16, x9
    eor x16, x16, x3
    add x4, x4, x16
    add x8, x8, x4
    ror x16, x5, #28
    eor x16, x16, x5, ror #34
    eor x16, x16, x5, ror #39
    add x4, x4, x16
    eor x16, x5, x6
    eor x17, x6, x7
    and x16, x16, x17
    eor x16, x16, x6
    add x4, x4, x16
    ldr x12, [x1], #8
    rev x12, x12
    str x12, [sp, #56]
    ldr x13, [x11], #8
    add x3, x3, x12
    add x3, x3, x13
    ror x16, x8, #14
    eor x16, x16, x8, ror #18
    eor x16, x16, x8, ror #41
    add x3, x3, x16
    eor x16, x9, x10
    and x16, x16, x8
    eor x16, x16, x10
    add x3, x3, x16
    add x7, x7, x3
    ror x16, x4, #28
    eor x16, x16, x4, ror #34
    eor x16, x16, x4, ror #39
    add x3, x3, x16
    eor x16, x4, x5
    eor x17, x5, x6
    and x16, x16, x17
    eor x16, x16, x5
    add x3, x3, x16
    ldr x12, [x1], #8
    rev x12, x12
    str x12, [sp, #64]
    ldr x13, [x11], #8
    add x10, x10, x12
    add x10, x10, x13
    ror x16, x7, #14
    eor x16, x16, x7, ror #18
    eor x16, x16, x7, ror #41
    add x10, x10, x16
    eor x16, x8, x9
    and x16, x16, x7
    eor x16, x16, x9
    add x10, x10, x16
    add x6, x6, x10
    ror x16, x3, #28
    eor x16, x16, x3, ror #34
    eor x16, x16, x3, ror #39
    add x10, x10, x16
    eor x16, x3, x4
    eor x17, x4, x5
    and x16, x16, x17
    eor x16, x16, x4
    add x10, x10, x16
    ldr x12, [x1], #8
    rev x12, x12
    str x12, [sp, #72]
    ldr x13, [x11], #8
    add x9, x9, x12
    add x9, x9, x13
    ror x16, x6, #14
    eor x16, x16, x6, ror #18
    eor x16, x16, x6, ror #41
    add x9, x9, x16
    eor x16, x7, x8
    and x16, x16, x6
    eor x16, x16, x8
    add x9, x9, x16
    add x5, x5, x9
    ror x16, x10, #28
    eor x16, x16, x10, ror #34
    eor x16, x16, x10, ror #39
    add x9, x9, x16
    eor x16, x10, x3
    eor x17, x3, x4
    and x16, x16, x17
    eor x16, x16, x3
    add x9, x9, x16
    ldr x12, [x1], #8
    rev x12, x12
    str x12, [sp, #80]
    ldr x13, [x11], #8
    add x8, x8, x12
    add x8, x8, x13
    ror x16, x5, #14
    eor x16, x16, x5, ror #18
    eor x16, x16, x5, ror #41
    add x8, x8, x16
    eor x16, x6, x7
    and x16, x16, x5
    eor x16, x16, x7
    add x8, x8, x16
    add x4, x4, x8
    ror x16, x9, #28
    eor x16, x16, x9, ror #34
    eor x16, x16, x9, ror #39
    add x8, x8, x16
    eor x16, x9, x10
    eor x17, x10, x3
    and x16, x16, x17
    eor x16, x16, x10
    add x8, x8, x16
    ldr x12, [x1], #8
    rev x12, x12
    str x12, [sp, #88]
    ldr x13, [x11], #8
    add x7, x7, x12
    add x7, x7, x13
    ror x16, x4, #14
    eor x16, x16, x4, ror #18
    eor x16, x16, x4, ror #41
    add x7, x7, x16
    eor x16, x5, x6
    and x16, x16, x4
    eor x16, x16, x6
    add x7, x7, x16
    add x3, x3, x7
    ror x16, x8, #28
    eor x16, x16, x8, ror #34
    eor x16, x16, x8, ror #39
    add x7, x7, x16
    eor x16, x8, x9
    eor x17, x9, x10
    and x16, x16, x17
    eor x16, x16, x9
    add x7, x7, x16
    ldr x12, [x1], #8
    rev x12, x12
    str x12, [sp, #96]
    ldr x13, [x11], #8
    add x6, x6, x12
    add x6, x6, x13
    ror x16, x3, #14
    eor x16, x16, x3, ror #18
    eor x16, x16, x3, ror #41
    add x6, x6, x16
    eor x16, x4, x5
    and x16, x16, x3
    eor x16, x16, x5
    add x6, x6, x16
    add x10, x10, x6
    ror x16, x7, #28
    eor x16, x16, x7, ror #34
    eor x16, x16, x7, ror #39
    add x6, x6, x16
    eor x16, x7, x8
    eor x17, x8, x9
    and x16, x16, x17
    eor x16, x16, x8
    add x6, x6, x16
    ldr x12, [x1], #8
    rev x12, x12
    str x12, [sp, #104]
    ldr x13, [x11], #8
    add x5, x5, x12
    add x5, x5, x13
    ror x16, x10, #14
    eor x16, x16, x10, ror #18
    eor x16, x16, x10, ror #41
    add x5, x5, x16
    eor x16, x3, x4
    and x16, x16, x10
    eor x16, x16, x4
    add x5, x5, x16
    add x9, x9, x5
    ror x16, x6, #28
    eor x16, x16, x6, ror #34
    eor x16, x16, x6, ror #39
    add x5, x5, x16
    eor x16, x6, x7
    eor x17, x7, x8
    and x16, x16, x17
    eor x16, x16, x7
    add x5, x5, x16
    ldr x12, [x1], #8
    rev x12, x12
    str x12, [sp, #112]
    ldr x13, [x11], #8
    add x4, x4, x12
    add x4, x4, x13
    ror x16, x9, #14
    eor x16, x16, x9, ror #18
    eor x16, x16, x9, ror #41
    add x4, x4, x16
    eor x16, x10, x3
    and x16, x16, x9
    eor x16, x16, x3
    add x4, x4, x16
    add x8, x8, x4
    ror x16, x5, #28
    eor x16, x16, x5, ror #34
    eor x16, x16, x5, ror #39
    add x4, x4, x16
    eor x16, x5, x6
    eor x17, x6, x7
    and x16, x16, x17
    eor x16, x16, x6
    add x4, x4, x16
    ldr x12, [x1], #8
    rev x12, x12
    str x12, [sp, #120]
    ldr x13, [x11], #8
    add x3, x3, x12
    add x3, x3, x13
    ror x16, x8, #14
    eor x16, x16, x8, ror #18
    eor x16, x16, x8, ror #41
    add x3, x3, x16
    eor x16, x9, x10
    and x16, x16, x8
    eor x16, x16, x10
    add x3, x3, x16
    add x7, x7, x3
    ror x16, x4, #28
    eor x16, x16, x4, ror #34
    eor x16, x16, x4, ror #39
    add x3, x3, x16
    eor x16, x4, x5
    eor x17, x5, x6
    and x16, x16, x17
    eor x16, x16, x5
    add x3, x3, x16
    mov w1, #4
.Lsha512_loop:
    ldr x12, [sp, #0]
    ldr x13, [sp, #8]
    ldr x14, [sp, #112]
    ldr x15, [sp, #72]
    ror x16, x13, #1
    eor x16, x16, x13, ror #8
    eor x16, x16, x13, lsr #7
    ror x17, x14, #19
    eor x17, x17, x14, ror #61
    eor x17, x17, x14, lsr #6
    add x12, x12, x15
    add x12, x12, x16
    add x12, x12, x17
    str x12, [sp, #0]
    ldr x13, [x11], #8
    add x10, x10, x12
    add x10, x10, x13
    ror x16, x7, #14
    eor x16, x16, x7, ror #18
    eor x16, x16, x7, ror #41
    add x10, x10, x16
    eor x16, x8, x9
    and x16, x16, x7
    eor x16, x16, x9
    add x10, x10, x16
    add x6, x6, x10
    ror x16, x3, #28
    eor x16, x16, x3, ror #34
    eor x16, x16, x3, ror #39
    add x10, x10, x16
    eor x16, x3, x4
    eor x17, x4, x5
    and x16, x16, x17
    eor x16, x16, x4
    add x10, x10, x16
    ldr x12, [sp, #8]
    ldr x13, [sp, #16]
    ldr x14, [sp, #120]
    ldr x15, [sp, #80]
    ror x16, x13, #1
    eor x16, x16, x13, ror #8
    eor x16, x16, x13, lsr #7
    ror x17, x14, #19
    eor x17, x17, x14, ror #61
    eor x17, x17, x14, lsr #6
    add x12, x12, x15
    add x12, x12, x16
    add x12, x12, x17
    str x12, [sp, #8]
    ldr x13, [x11], #8
    add x9, x9, x12
    add x9, x9, x13
    ror x16, x6, #14
    eor x16, x16, x6, ror #18
    eor x16, x16, x6, ror #41
    add x9, x9, x16
    eor x16, x7, x8
    and x16, x16, x6
    eor x16, x16, x8
    add x9, x9, x16
    add x5, x5, x9
    ror x16, x10, #28
    eor x16, x16, x10, ror #34
    eor x16, x16, x10, ror #39
    add x9, x9, x16
    eor x16, x10, x3
    eor x17, x3, x4
    and x16, x16, x17
    eor x16, x16, x3
    add x9, x9, x16
    ldr x12, [sp, #16]
    ldr x13, [sp, #24]
    ldr x14, [sp, #0]
    ldr x15, [sp, #88]
    ror x16, x13, #1
    eor x16, x16, x13, ror #8
    eor x16, x16, x13, lsr #7
    ror x17, x14, #19
    eor x17, x17, x14, ror #61
    eor x17, x17, x14, lsr #6
    add x12, x12, x15
    add x12, x12, x16
    add x12, x12, x17
    str x12, [sp, #16]
    ldr x13, [x11], #8
    add x8, x8, x12
    add x8, x8, x13
    ror x16, x5, #14
    eor x16, x16, x5, ror #18
    eor x16, x16, x5, ror #41
    add x8, x8, x16
    eor x16, x6, x7
    and x16, x16, x5
    eor x16, x16, x7
    add x8, x8, x16
    add x4, x4, x8
    ror x16, x9, #28
    eor x16, x16, x9, ror #34
    eor x16, x16, x9, ror #39
    add x8, x8, x16
    eor x16, x9, x10
    eor x17, x10, x3
    and x16, x16, x17
    eor x16, x16, x10
    add x8, x8, x16
    ldr x12, [sp, #24]
    ldr x13, [sp, #32]
    ldr x14, [sp, #8]
    ldr x15, [sp, #96]
    ror x16, x13, #1
    eor x16, x16, x13, ror #8
    eor x16, x16, x13, lsr #7
    ror x17, x14, #19
    eor x17, x17, x14, ror #61
    eor x17, x17, x14, lsr #6
    add x12, x12, x15
    add x12, x12, x16
    add x12, x12, x17
    str x12, [sp, #24]
    ldr x13, [x11], #8
    add x7, x7, x12
    add x7, x7, x13
    ror x16, x4, #14
    eor x16, x16, x4, ror #18
    eor x16, x16, x4, ror #41
    add x7, x7, x16
    eor x16, x5, x6
    and x16, x16, x4
    eor x16, x16, x6
    add x7, x7, x16
    add x3, x3, x7
    ror x16, x8, #28
    eor x16, x16, x8, ror #34
    eor x16, x16, x8, ror #39
    add x7, x7, x16
    eor x16, x8, x9
    eor x17, x9, x10
    and x16, x16, x17
    eor x16, x16, x9
    add x7, x7, x16
    ldr x12, [sp, #32]
    ldr x13, [sp, #40]
    ldr x14, [sp, #16]
    ldr x15, [sp, #104]
    ror x16, x13, #1
    eor x16, x16, x13, ror #8
    eor x16, x16, x13, lsr #7
    ror x17, x14, #19
    eor x17, x17, x14, ror #61
    eor x17, x17, x14, lsr #6
    add x12, x12, x15
    add x12, x12, x16
    add x12, x12, x17
    str x12, [sp, #32]
    ldr x13, [x11], #8
    add x6, x6, x12
    add x6, x6, x13
    ror x16, x3, #14
    eor x16, x16, x3, ror #18
    eor x16, x16, x3, ror #41
    add x6, x6, x16
    eor x16, x4, x5
    and x16, x16, x3
    eor x16, x16, x5
    add x6, x6, x16
    add x10, x10, x6
    ror x16, x7, #28
    eor x16, x16, x7, ror #34
    eor x16, x16, x7, ror #39
    add x6, x6, x16
    eor x16, x7, x8
    eor x17, x8, x9
    and x16, x16, x17
    eor x16, x16, x8
    add x6, x6, x16
    ldr x12, [sp, #40]
    ldr x13, [sp, #48]
    ldr x14, [sp, #24]
    ldr x15, [sp, #112]
    ror x16, x13, #1
    eor x16, x16, x13, ror #8
    eor x16, x16, x13, lsr #7
    ror x17, x14, #19
    eor x17, x17, x14, ror #61
    eor x17, x17, x14, lsr #6
    add x12, x12, x15
    add x12, x12, x16
    add x12, x12, x17
    str x12, [sp, #40]
    ldr x13, [x11], #8
    add x5, x5, x12
    add x5, x5, x13
    ror x16, x10, #14
    eor x16, x16, x10, ror #18
    eor x16, x16, x10, ror #41
    add x5, x5, x16
    eor x16, x3, x4
    and x16, x16, x10
    eor x16, x16, x4
    add x5, x5, x16
    add x9, x9, x5
    ror x16, x6, #28
    eor x16, x16, x6, ror #34
    eor x16, x16, x6, ror #39
    add x5, x5, x16
    eor x16, x6, x7
    eor x17, x7, x8
    and x16, x16, x17
    eor x16, x16, x7
    add x5, x5, x16
    ldr x12, [sp, #48]
    ldr x13, [sp, #56]
    ldr x14, [sp, #32]
    ldr x15, [sp, #120]
    ror x16, x13, #1
    eor x16, x16, x13, ror #8
    eor x16, x16, x13, lsr #7
    ror x17, x14, #19
    eor x17, x17, x14, ror #61
    eor x17, x17, x14, lsr #6
    add x12, x12, x15
    add x12, x12, x16
    add x12, x12, x17
    str x12, [sp, #48]
    ldr x13, [x11], #8
    add x4, x4, x12
    add x4, x4, x13
    ror x16, x9, #14
    eor x16, x16, x9, ror #18
    eor x16, x16, x9, ror #41
    add x4, x4, x16
    eor x16, x10, x3
    and x16, x16, x9
    eor x16, x16, x3
    add x4, x4, x16
    add x8, x8, x4
    ror x16, x5, #28
    eor x16, x16, x5, ror #34
    eor x16, x16, x5, ror #39
    add x4, x4, x16
    eor x16, x5, x6
    eor x17, x6, x7
    and x16, x16, x17
    eor x16, x16, x6
    add x4, x4, x16
    ldr x12, [sp, #56]
    ldr x13, [sp, #64]
    ldr x14, [sp, #40]
    ldr x15, [sp, #0]
    ror x16, x13, #1
    eor x16, x16, x13, ror #8
    eor x16, x16, x13, lsr #7
    ror x17, x14, #19
    eor x17, x17, x14, ror #61
    eor x17, x17, x14, lsr #6
    add x12, x12, x15
    add x12, x12, x16
    add x12, x12, x17
    str x12, [sp, #56]
    ldr x13, [x11], #8
    add x3, x3, x12
    add x3, x3, x13
    ror x16, x8, #14
    eor x16, x16, x8, ror #18
    eor x16, x16, x8, ror #41
    add x3, x3, x16
    eor x16, x9, x10
    and x16, x16, x8
    eor x16, x16, x10
    add x3, x3, x16
    add x7, x7, x3
    ror x16, x4, #28
    eor x16, x16, x4, ror #34
    eor x16, x16, x4, ror #39
    add x3, x3, x16
    eor x16, x4, x5
    eor x17, x5, x6
    and x16, x16, x17
    eor x16, x16, x5
    add x3, x3, x16
    ldr x12, [sp, #64]
    ldr x13, [sp, #72]
    ldr x14, [sp, #48]
    ldr x15, [sp, #8]
    ror x16, x13, #1
    eor x16, x16, x13, ror #8
    eor x16, x16, x13, lsr #7
    ror x17, x14, #19
    eor x17, x17, x14, ror #61
    eor x17, x17, x14, lsr #6
    add x12, x12, x15
    add x12, x12, x16
    add x12, x12, x17
    str x12, [sp, #64]
    ldr x13, [x11], #8
    add x10, x10, x12
    add x10, x10, x13
    ror x16, x7, #14
    eor x16, x16, x7, ror #18
    eor x16, x16, x7, ror #41
    add x10, x10, x16
    eor x16, x8, x9
    and x16, x16, x7
    eor x16, x16, x9
    add x10, x10, x16
    add x6, x6, x10
    ror x16, x3, #28
    eor x16, x16, x3, ror #34
    eor x16, x16, x3, ror #39
    add x10, x10, x16
    eor x16, x3, x4
    eor x17, x4, x5
    and x16, x16, x17
    eor x16, x16, x4
    add x10, x10, x16
    ldr x12, [sp, #72]
    ldr x13, [sp, #80]
    ldr x14, [sp, #56]
    ldr x15, [sp, #16]
    ror x16, x13, #1
    eor x16, x16, x13, ror #8
    eor x16, x16, x13, lsr #7
    ror x17, x14, #19
    eor x17, x17, x14, ror #61
    eor x17, x17, x14, lsr #6
    add x12, x12, x15
    add x12, x12, x16
    add x12, x12, x17
    str x12, [sp, #72]
    ldr x13, [x11], #8
    add x9, x9, x12
    add x9, x9, x13
    ror x16, x6, #14
    eor x16, x16, x6, ror #18
    eor x16, x16, x6, ror #41
    add x9, x9, x16
    eor x16, x7, x8
    and x16, x16, x6
    eor x16, x16, x8
    add x9, x9, x16
    add x5, x5, x9
    ror x16, x10, #28
    eor x16, x16, x10, ror #34
    eor x16, x16, x10, ror #39
    add x9, x9, x16
    eor x16, x10, x3
    eor x17, x3, x4
    and x16, x16, x17
    eor x16, x16, x3
    add x9, x9, x16
    ldr x12, [sp, #80]
    ldr x13, [sp, #88]
    ldr x14, [sp, #64]
    ldr x15, [sp, #24]
    ror x16, x13, #1
    eor x16, x16, x13, ror #8
    eor x16, x16, x13, lsr #7
    ror x17, x14, #19
    eor x17, x17, x14, ror #61
    eor x17, x17, x14, lsr #6
    add x12, x12, x15
    add x12, x12, x16
    add x12, x12, x17
    str x12, [sp, #80]
    ldr x13, [x11], #8
    add x8, x8, x12
    add x8, x8, x13
    ror x16, x5, #14
    eor x16, x16, x5, ror #18
    eor x16, x16, x5, ror #41
    add x8, x8, x16
    eor x16, x6, x7
    and x16, x16, x5
    eor x16, x16, x7
    add x8, x8, x16
    add x4, x4, x8
    ror x16, x9, #28
    eor x16, x16, x9, ror #34
    eor x16, x16, x9, ror #39
    add x8, x8, x16
    eor x16, x9, x10
    eor x17, x10, x3
    and x16, x16, x17
    eor x16, x16, x10
    add x8, x8, x16
    ldr x12, [sp, #88]
    ldr x13, [sp, #96]
    ldr x14, [sp, #72]
    ldr x15, [sp, #32]
    ror x16, x13, #1
    eor x16, x16, x13, ror #8
    eor x16, x16, x13, lsr #7
    ror x17, x14, #19
    eor x17, x17, x14, ror #61
    eor x17, x17, x14, lsr #6
    add x12, x12, x15
    add x12, x12, x16
    add x12, x12, x17
    str x12, [sp, #88]
    ldr x13, [x11], #8
    add x7, x7, x12
    add x7, x7, x13
    ror x16, x4, #14
    eor x16, x16, x4, ror #18
    eor x16, x16, x4, ror #41
    add x7, x7, x16
    eor x16, x5, x6
    and x16, x16, x4
    eor x16, x16, x6
    add x7, x7, x16
    add x3, x3, x7
    ror x16, x8, #28
    eor x16, x16, x8, ror #34
    eor x16, x16, x8, ror #39
    add x7, x7, x16
    eor x16, x8, x9
    eor x17, x9, x10
    and x16, x16, x17
    eor x16, x16, x9
    add x7, x7, x16
    ldr x12, [sp, #96]
    ldr x13, [sp, #104]
    ldr x14, [sp, #80]
    ldr x15, [sp, #40]
    ror x16, x13, #1
    eor x16, x16, x13, ror #8
    eor x16, x16, x13, lsr #7
    ror x17, x14, #19
    eor x17, x17, x14, ror #61
    eor x17, x17, x14, lsr #6
    add x12, x12, x15
    add x12, x12, x16
    add x12, x12, x17
    str x12, [sp, #96]
    ldr x13, [x11], #8
    add x6, x6, x12
    add x6, x6, x13
    ror x16, x3, #14
    eor x16, x16, x3, ror #18
    eor x16, x16, x3, ror #41
    add x6, x6, x16
    eor x16, x4, x5
    and x16, x16, x3
    eor x16, x16, x5
    add x6, x6, x16
    add x10, x10, x6
    ror x16, x7, #28
    eor x16, x16, x7, ror #34
    eor x16, x16, x7, ror #39
    add x6, x6, x16
    eor x16, x7, x8
    eor x17, x8, x9
    and x16, x16, x17
    eor x16, x16, x8
    add x6, x6, x16
    ldr x12, [sp, #104]
    ldr x13, [sp, #112]
    ldr x14, [sp, #88]
    ldr x15, [sp, #48]
    ror x16, x13, #1
    eor x16, x16, x13, ror #8
    eor x16, x16, x13, lsr #7
    ror x17, x14, #19
    eor x17, x17, x14, ror #61
    eor x17, x17, x14, lsr #6
    add x12, x12, x15
    add x12, x12, x16
    add x12, x12, x17
    str x12, [sp, #104]
    ldr x13, [x11], #8
    add x5, x5, x12
    add x5, x5, x13
    ror x16, x10, #14
    eor x16, x16, x10, ror #18
    eor x16, x16, x10, ror #41
    add x5, x5, x16
    eor x16, x3, x4
    and x16, x16, x10
    eor x16, x16, x4
    add x5, x5, x16
    add x9, x9, x5
    ror x16, x6, #28
    eor x16, x16, x6, ror #34
    eor x16, x16, x6, ror #39
    add x5, x5, x16
    eor x16, x6, x7
    eor x17, x7, x8
    and x16, x16, x17
    eor x16, x16, x7
    add x5, x5, x16
    ldr x12, [sp, #112]
    ldr x13, [sp, #120]
    ldr x14, [sp, #96]
    ldr x15, [sp, #56]
    ror x16, x13, #1
    eor x16, x16, x13, ror #8
    eor x16, x16, x13, lsr #7
    ror x17, x14, #19
    eor x17, x17, x14, ror #61
    eor x17, x17, x14, lsr #6
    add x12, x12, x15
    add x12, x12, x16
    add x12, x12, x17
    str x12, [sp, #112]
    ldr x13, [x11], #8
    add x4, x4, x12
    add x4, x4, x13
    ror x16, x9, #14
    eor x16, x16, x9, ror #18
    eor x16, x16, x9, ror #41
    add x4, x4, x16
    eor x16, x10, x3
    and x16, x16, x9
    eor x16, x16, x3
    add x4, x4, x16
    add x8, x8, x4
    ror x16, x5, #28
    eor x16, x16, x5, ror #34
    eor x16, x16, x5, ror #39
    add x4, x4, x16
    eor x16, x5, x6
    eor x17, x6, x7
    and x16, x16, x17
    eor x16, x16, x6
    add x4, x4, x16
    ldr x12, [sp, #120]
    ldr x13, [sp, #0]
    ldr x14, [sp, #104]
    ldr x15, [sp, #64]
    ror x16, x13, #1
    eor x16, x16, x13, ror #8
    eor x16, x16, x13, lsr #7
    ror x17, x14, #19
    eor x17, x17, x14, ror #61
    eor x17, x17, x14, lsr #6
    add x12, x12, x15
    add x12, x12, x16
    add x12, x12, x17
    str x12, [sp, #120]
    ldr x13, [x11], #8
    add x3, x3, x12
    add x3, x3, x13
    ror x16, x8, #14
    eor x16, x16, x8, ror #18
    eor x16, x16, x8, ror #41
    add x3, x3, x16
    eor x16, x9, x10
    and x16, x16, x8
    eor x16, x16, x10
    add x3, x3, x16
    add x7, x7, x3
    ror x16, x4, #28
    eor x16, x16, x4, ror #34
    eor x16, x16, x4, ror #39
    add x3, x3, x16
    eor x16, x4, x5
    eor x17, x5, x6
    and x16, x16, x17
    eor x16, x16, x5
    add x3, x3, x16
    subs w1, w1, #1
    b.ne .Lsha512_loop
    ldp x12, x13, [x2]
    ldp x14, x15, [x2, #16]
    add x3, x3, x12
    add x4, x4, x13
    add x5, x5, x14
    add x6, x6, x15
    stp x3, x4, [x2]
    stp x5, x6, [x2, #16]
    ldp x12, x13, [x2, #32]
    ldp x14, x15, [x2, #48]
    add x7, x7, x12
    add x8, x8, x13
    add x9, x9, x14
    add x10, x10, x15
    stp x7, x8, [x2, #32]
    stp x9, x10, [x2, #48]
    stp xzr, xzr, [sp, #0]
    stp xzr, xzr, [sp, #16]
    stp xzr, xzr, [sp, #32]
    stp xzr, xzr, [sp, #48]
    stp xzr, xzr, [sp, #64]
    stp xzr, xzr, [sp, #80]
    stp xzr, xzr, [sp, #96]
    stp xzr, xzr, [sp, #112]
    mov x3, xzr
    mov x4, xzr
    mov x5, xzr
    mov x6, xzr
    mov x7, xzr
    mov x8, xzr
    mov x9, xzr
    mov x10, xzr
    mov x12, xzr
    mov x13, xzr
    mov x14, xzr
    mov x15, xzr
    mov x16, xzr
    mov x17, xzr
    add sp, sp, #128
.cfi_def_cfa_offset 0
    ret
.cfi_endproc
.size SharkSslSha384Ctx_process, .-SharkSslSha384Ctx_process
.section .rodata
.p2align 4
.Lsha512_k:
    .quad 0x428a2f98d728ae22, 0x7137449123ef65cd, 0xb5c0fbcfec4d3b2f, 0xe9b5dba58189dbbc, 0x3956c25bf348b538, 0x59f111f1b605d019, 0x923f82a4af194f9b, 0xab1c5ed5da6d8118
    .quad 0xd807aa98a3030242, 0x12835b0145706fbe, 0x243185be4ee4b28c, 0x550c7dc3d5ffb4e2, 0x72be5d74f27b896f, 0x80deb1fe3b1696b1, 0x9bdc06a725c71235, 0xc19bf174cf692694
    .quad 0xe49b69c19ef14ad2, 0xefbe4786384f25e3, 0x0fc19dc68b8cd5b5, 0x240ca1cc77ac9c65, 0x2de92c6f592b0275, 0x4a7484aa6ea6e483, 0x5cb0a9dcbd41fbd4, 0x76f988da831153b5
    .quad 0x983e5152ee66dfab, 0xa831c66d2db43210, 0xb00327c898fb213f, 0xbf597fc7beef0ee4, 0xc6e00bf33da88fc2, 0xd5a79147930aa725, 0x06ca6351e003826f, 0x142929670a0e6e70
    .quad 0x27b70a8546d22ffc, 0x2e1b21385c26c926, 0x4d2c6dfc5ac42aed, 0x53380d139d95b3df, 0x650a73548baf63de, 0x766a0abb3c77b2a8, 0x81c2c92e47edaee6, 0x92722c851482353b
    .quad 0xa2bfe8a14cf10364, 0xa81a664bbc423001, 0xc24b8b70d0f89791, 0xc76c51a30654be30, 0xd192e819d6ef5218, 0xd69906245565a910, 0xf40e35855771202a, 0x106aa07032bbd1b8
    .quad 0x19a4c116b8d2d0c8, 0x1e376c085141ab53, 0x2748774cdf8eeb99, 0x34b0bcb5e19b48a8, 0x391c0cb3c5c95a63, 0x4ed8aa4ae3418acb, 0x5b9cca4f7763e373, 0x682e6ff3d6b2b8a3
    .quad 0x748f82ee5defb2fc, 0x78a5636f43172f60, 0x84c87814a1f0ab72, 0x8cc702081a6439ec, 0x90befffa23631e28, 0xa4506cebde82bde9, 0xbef9a3f7b2c67915, 0xc67178f2e372532b
    .quad 0xca273eceea26619c, 0xd186b8c721c0c207, 0xeada7dd6cde0eb1e, 0xf57d4f7fee6ed178, 0x06f067aa72176fba, 0x0a637dc5a2c898a6, 0x113f9804bef90dae, 0x1b710b35131c471b
    .quad 0x28db77f523047d84, 0x32caab7b40c72493, 0x3c9ebe0a15c9bebc, 0x431d67c49c100d4c, 0x4cc5d4becb3e42b6, 0x597f299cfc657e2a, 0x5fcb6fab3ad6faec, 0x6c44198c4a475817
#endif
.section .note.GNU-stack,"",%progbits
