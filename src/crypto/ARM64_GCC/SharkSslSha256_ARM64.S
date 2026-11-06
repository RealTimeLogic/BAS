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
 *   $Id: SharkSslSha256_ARM64.S 6739 2026-11-04 08:10:12Z gianluca $
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
#if (SHARKSSL_USE_SHA_256 && SHARKSSL_ARM64_SHA256_INTRINSICS && (!SHARKSSL_PSA_CONSUMER))
/* One SHA-256 block with the SHA2 instructions, which SharkSslCrypto.c selects
 * at runtime. The registers that held the state and the message are cleared:
 * in HMAC and HKDF they are key material.
 * x0 ctx (state at offset 8), x1 data, x2 the 64 round constants. */
.macro SHA4 w, k
    ldr q16, [x2, #\k]
    add v16.4s, v16.4s, \w\().4s
    mov v17.16b, v0.16b
    sha256h q0, q1, v16.4s
    sha256h2 q1, q17, v16.4s
.endm

.macro SCHED a, b, c, d
    sha256su0 \a\().4s, \b\().4s
    sha256su1 \a\().4s, \c\().4s, \d\().4s
.endm

.p2align 4
.global SharkSslSha256Ctx_process_arm64
.hidden SharkSslSha256Ctx_process_arm64
.type SharkSslSha256Ctx_process_arm64, %function
SharkSslSha256Ctx_process_arm64:
.cfi_startproc
    add x3, x0, #8
    ldp q0, q1, [x3]
    ldp q4, q5, [x1]
    ldp q6, q7, [x1, #32]
    mov v2.16b, v0.16b
    mov v3.16b, v1.16b
    rev32 v4.16b, v4.16b
    rev32 v5.16b, v5.16b
    rev32 v6.16b, v6.16b
    rev32 v7.16b, v7.16b
    SHA4 v4, 0
    SHA4 v5, 16
    SHA4 v6, 32
    SHA4 v7, 48
    SCHED v4, v5, v6, v7
    SCHED v5, v6, v7, v4
    SCHED v6, v7, v4, v5
    SCHED v7, v4, v5, v6
    SHA4 v4, 64
    SHA4 v5, 80
    SHA4 v6, 96
    SHA4 v7, 112
    SCHED v4, v5, v6, v7
    SCHED v5, v6, v7, v4
    SCHED v6, v7, v4, v5
    SCHED v7, v4, v5, v6
    SHA4 v4, 128
    SHA4 v5, 144
    SHA4 v6, 160
    SHA4 v7, 176
    SCHED v4, v5, v6, v7
    SCHED v5, v6, v7, v4
    SCHED v6, v7, v4, v5
    SCHED v7, v4, v5, v6
    SHA4 v4, 192
    SHA4 v5, 208
    SHA4 v6, 224
    SHA4 v7, 240
    add v0.4s, v0.4s, v2.4s
    add v1.4s, v1.4s, v3.4s
    stp q0, q1, [x3]
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
    ret
.cfi_endproc
.size SharkSslSha256Ctx_process_arm64, .-SharkSslSha256Ctx_process_arm64
#endif
.section .note.GNU-stack,"",%progbits
