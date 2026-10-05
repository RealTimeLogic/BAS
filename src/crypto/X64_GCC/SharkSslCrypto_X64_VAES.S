/* Linux x86-64 ELF/GAS port of the corresponding X64_MSVC file.
 * The public SysV entry adapts arguments to the private kernel schedule.
 * No compiler ABI attributes are needed in common C. */
.intel_syntax noprefix
/* **************************************************************************** */
/* *     ____             _________                __                _ */
/* *    / __ \___  ____ _/ /_  __(_)___ ___  ___  / /   ____  ____ _(_)____ */
/* *   / /_/ / _ \/ __ `/ / / / / / __ `__ \/ _ \/ /   / __ \/ __ `/ / ___/ */
/* *  / _, _/  __/ /_/ / / / / / / / / / / /  __/ /___/ /_/ /_/ / / /__/ */
/* * /_/ |_|\___/\__,_/_/ /_/ /_/_/ /_/ /_/\___/_____/\____/\__, /_/\___/ */
/* *                                                       /____/ */
/* * */
/* *                 SharkSSL Embedded SSL/TLS Stack */
/* **************************************************************************** */
/* *   PROGRAM MODULE */
/* * */
/* *   $Id: SharkSslCrypto_X64_VAES.asm 6169 2026-09-26 18:15:48Z wini $ */
/* * */
/* *   COPYRIGHT:  Real Time Logic LLC, 2010 - 2026 */
/* * */
/* *   This software is copyrighted by and is the sole property of Real */
/* *   Time Logic LLC.  All rights, title, ownership, or other interests in */
/* *   the software remain the property of Real Time Logic LLC.  This */
/* *   software may only be used in accordance with the terms and */
/* *   conditions stipulated in the corresponding license agreement under */
/* *   which the software has been supplied.  Any unauthorized use, */
/* *   duplication, transmission, distribution, or disclosure of this */
/* *   software is expressly forbidden. */
/* * */
/* *   This Copyright notice may not be removed or modified without prior */
/* *   written consent of Real Time Logic LLC. */
/* * */
/* *   Real Time Logic LLC. reserves the right to modify this software */
/* *   without notice. */
/* * */
/* *               http://www.realtimelogic.com */
/* *               http://www.sharkssl.com */
/* **************************************************************************** */

/*  SharkSSL AES-GCM VAES/VPCLMULQDQ backend for Microsoft x64 MASM. */

/*  Eight-/sixteen-block GCM pipeline using 256-bit VAES for AES-CTR and 256-bit */
/*  VPCLMULQDQ for GHASH. Requires XMM/YMM state, AVX2, VAES and VPCLMULQDQ. */
/*  SharkSslCrypto.c dispatches to this kernel only after the CPUID/XGETBV */
/*  probe in SharkSslCrypto_X64_VAES_CPUID.c succeeds. Only 256-bit vector */
/*  registers are used, so the kernel also runs on processors that implement */
/*  VAES/VPCLMULQDQ without 512-bit datapaths (Ice Lake class and later). */

/*  void sharkssl_gcm_crypt_blocks_vaes(SharkSslAesGcmCtx *ctx, U8 counter[16], */
/*                                      U8 tag[16], const U8 *input, U8 *output, */
/*                                      U32 len, SharkSslAesCtx_Type type); */

/*  Windows x64 arguments: */
/*    rcx=ctx, rdx=counter, r8=tag, r9=input */
/*    [rsp+40]=output, [rsp+48]=len, [rsp+56]=type */

/*  len must be a non-zero multiple of 16, as guaranteed by SharkSslCrypto.c. */
/*  Full 128-byte groups use the VAES pipeline; calls of at least 1024 bytes */
/*  fold sixteen blocks at a time. Shorter tails use the four-block and */
/*  single-block AES-NI/PCLMULQDQ paths below. */

/*  The context layout is the same as the AES-NI backend, extended so that */
/*  M0[0..7] holds reflected H^1..H^8 (prepared by sharkssl_gcm_prepare64M0). */
/*  M0[0..3] keeps the exact layout the AES-NI backend expects (H^1..H^4). */


#ifndef SHARKSSL_GCM_M0
#define SHARKSSL_GCM_M0 244
#endif

.equ SHARKSSL_AES_NR, SHARKSSL_GCM_M0 - 4
.equ GCM_VAES_FRAME_BYTES, 512
.equ GCM_VAES_SAVED_BYTES, 7 * 8

.section .rodata
.balign 16
reverse_key_words: .byte 3,2,1,0, 7,6,5,4, 11,10,9,8, 15,14,13,12
reverse_nibbles: .byte 0,8,4,12,2,10,6,14,1,9,5,13,3,11,7,15
low_nibbles: .byte 0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f
ghash_polynomial: .quad 0x087, 0
reverse_key_words256: .byte 3,2,1,0, 7,6,5,4, 11,10,9,8, 15,14,13,12, 3,2,1,0, 7,6,5,4, 11,10,9,8, 15,14,13,12
reverse_nibbles256: .byte 0,8,4,12,2,10,6,14,1,9,5,13,3,11,7,15, 0,8,4,12,2,10,6,14,1,9,5,13,3,11,7,15
low_nibbles256: .byte 0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f
/*  Increment only the final dword of each 128-bit counter (GCM inc32). */
counter_pair12: .long 0,0,0,1, 0,0,0,2
counter_pair_step: .long 0,0,0,2, 0,0,0,2

/*  Reciprocal polynomial x^128+x^127+x^126+x^121+1. */
/*  Byte reversal gives the Montgomery data representation; powers are scaled */
/*  by x so REDC(data * scaled_H) yields the standard GHASH field product. */
ghash_reverse_bytes256: .byte 15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0, 15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0
ghash_reciprocal256: .quad 1, 0x0c200000000000000, 1, 0x0c200000000000000

.text

.if SHARKSSL_GCM_M0 == 244
.globl sharkssl_x86_64_vaes_layout_244
.type sharkssl_x86_64_vaes_layout_244, @function
sharkssl_x86_64_vaes_layout_244:
.cfi_startproc
    ret
.cfi_endproc
.size sharkssl_x86_64_vaes_layout_244, .-sharkssl_x86_64_vaes_layout_244
.elseif SHARKSSL_GCM_M0 == 212
.globl sharkssl_x86_64_vaes_layout_212
.type sharkssl_x86_64_vaes_layout_212, @function
sharkssl_x86_64_vaes_layout_212:
.cfi_startproc
    ret
.cfi_endproc
.size sharkssl_x86_64_vaes_layout_212, .-sharkssl_x86_64_vaes_layout_212
.elseif SHARKSSL_GCM_M0 == 180
.globl sharkssl_x86_64_vaes_layout_180
.type sharkssl_x86_64_vaes_layout_180, @function
sharkssl_x86_64_vaes_layout_180:
.cfi_startproc
    ret
.cfi_endproc
.size sharkssl_x86_64_vaes_layout_180, .-sharkssl_x86_64_vaes_layout_180
.else
.error "Invalid SHARKSSL_GCM_M0"
.endif

.macro REFLECT_VALUE value, t1, t2, t3
    vmovdqa \t1, \value
    vpand \value, \value, XMMWORD PTR [rip+low_nibbles]
    vpsrlw \t1, \t1, 4
    vpand \t1, \t1, XMMWORD PTR [rip+low_nibbles]
    vmovdqa \t2, XMMWORD PTR [rip+reverse_nibbles]
    vpshufb \t2, \t2, \value
    vmovdqa \t3, XMMWORD PTR [rip+reverse_nibbles]
    vpshufb \t3, \t3, \t1
    vpsllw \t2, \t2, 4
    vpor \t2, \t2, \t3
    vmovdqa \value, \t2
.endm

/*  Per-lane 256-bit reflection. value holds two independent 128-bit blocks. */
.macro REFLECT_VALUE256 value, t1, t2
    vpand \t1, \value, YMMWORD PTR [rip+low_nibbles256]
    vpsrlw \value, \value, 4
/*  Remove bits shifted in from the adjacent byte before VPSHUFB. */
    vpand \value, \value, YMMWORD PTR [rip+low_nibbles256]
/*  The constant section guarantees only 16-byte alignment. */
    vmovdqu \t2, YMMWORD PTR [rip+reverse_nibbles256]
    vpshufb \t1, \t2, \t1
    vpshufb \value, \t2, \value
    vpsllw \t1, \t1, 4
    vpor \value, \t1, \value
.endm

/*  Multiply two reflected field elements. rcx=a, rdx=b, r8=result. */
.type .Lsharkssl_gcm_mul_reflected_vaes, @function
.Lsharkssl_gcm_mul_reflected_vaes:
.cfi_startproc
    vmovdqu xmm0, XMMWORD PTR [rcx]
    vmovdqu xmm1, XMMWORD PTR [rdx]
    vmovdqa xmm4, xmm0
    vmovdqa xmm5, xmm1
    vmovdqa xmm2, xmm0
    vpclmulqdq xmm2, xmm2, xmm1, 0x00
    vmovdqa xmm3, xmm0
    vpclmulqdq xmm3, xmm3, xmm1, 0x11
    vpclmulqdq xmm4, xmm4, xmm1, 0x01
    vpclmulqdq xmm0, xmm0, xmm5, 0x10
    vpxor xmm4, xmm4, xmm0
    vmovdqa xmm5, xmm4
    vpslldq xmm4, xmm4, 8
    vpsrldq xmm5, xmm5, 8
    vpxor xmm2, xmm2, xmm4
    vpxor xmm3, xmm3, xmm5

    vmovdqa xmm0, xmm3
    vpclmulqdq xmm0, xmm0, XMMWORD PTR [rip+ghash_polynomial], 0x00
    vmovdqa xmm1, xmm3
    vpclmulqdq xmm1, xmm1, XMMWORD PTR [rip+ghash_polynomial], 0x01
    vmovdqa xmm3, xmm1
    vpslldq xmm1, xmm1, 8
    vpsrldq xmm3, xmm3, 8
    vpxor xmm0, xmm0, xmm1
    vpclmulqdq xmm3, xmm3, XMMWORD PTR [rip+ghash_polynomial], 0x00
    vpxor xmm0, xmm0, xmm3
    vpxor xmm0, xmm0, xmm2
    vmovdqu XMMWORD PTR [r8], xmm0
    ret
.cfi_endproc
.size .Lsharkssl_gcm_mul_reflected_vaes, .-.Lsharkssl_gcm_mul_reflected_vaes

/*  Replace the nibble table with reflected H^1..H^8 in M0[0..7]. Called once */
/*  by SharkSslAesGcmCtx_constructor when the VAES kernel is selected. The */
/*  first four entries are identical to sharkssl_gcm_prepare32M0 so the */
/*  single-block and four-block AES-NI GHASH paths keep working. */
.globl sharkssl_gcm_prepare64M0
.type sharkssl_gcm_prepare64M0, @function
sharkssl_gcm_prepare64M0:
.cfi_startproc
    sub rsp, 40
.cfi_adjust_cfa_offset 40
    mov r10, rcx
    mov r11, rdx
    mov rcx, rdi
    mov rdx, rsi
    mov r8, r11
    mov r9, r10
    call .Lsharkssl_gcm_prepare64M0
    add rsp, 40
.cfi_adjust_cfa_offset -40
    ret
.cfi_endproc
.size sharkssl_gcm_prepare64M0, .-sharkssl_gcm_prepare64M0
.type .Lsharkssl_gcm_prepare64M0, @function
.Lsharkssl_gcm_prepare64M0:
.cfi_startproc
    push rbx
.cfi_adjust_cfa_offset 8
.cfi_offset rbx, -(8+8)
    sub rsp, 32
.cfi_adjust_cfa_offset 32

    mov rbx, rcx
    vmovdqu xmm0, XMMWORD PTR [rbx+128]
    vmovdqa xmm1, XMMWORD PTR [rip+reverse_key_words]
    vpshufb xmm0, xmm0, xmm1
    REFLECT_VALUE xmm0, xmm1, xmm2, xmm3
    vmovdqu XMMWORD PTR [rbx], xmm0

    mov rcx, rbx
    mov rdx, rbx
    lea r8, [rbx+16]
    call .Lsharkssl_gcm_mul_reflected_vaes
    lea rcx, [rbx+16]
    mov rdx, rbx
    lea r8, [rbx+32]
    call .Lsharkssl_gcm_mul_reflected_vaes
    lea rcx, [rbx+32]
    mov rdx, rbx
    lea r8, [rbx+48]
    call .Lsharkssl_gcm_mul_reflected_vaes
    lea rcx, [rbx+48]
    mov rdx, rbx
    lea r8, [rbx+64]
    call .Lsharkssl_gcm_mul_reflected_vaes
    lea rcx, [rbx+64]
    mov rdx, rbx
    lea r8, [rbx+80]
    call .Lsharkssl_gcm_mul_reflected_vaes
    lea rcx, [rbx+80]
    mov rdx, rbx
    lea r8, [rbx+96]
    call .Lsharkssl_gcm_mul_reflected_vaes
    lea rcx, [rbx+96]
    mov rdx, rbx
    lea r8, [rbx+112]
    call .Lsharkssl_gcm_mul_reflected_vaes

    vzeroupper
    add rsp, 32
.cfi_adjust_cfa_offset -32
    pop rbx
.cfi_adjust_cfa_offset -8
.cfi_restore rbx
    ret
.cfi_endproc
.size .Lsharkssl_gcm_prepare64M0, .-.Lsharkssl_gcm_prepare64M0

.macro CLMUL_ACCUM powerOffset
    vmovdqu xmm1, XMMWORD PTR [rcx+\powerOffset]
    vmovdqa xmm2, xmm0
    vpclmulqdq xmm2, xmm2, xmm1, 0x00
    vmovdqa xmm3, xmm0
    vpclmulqdq xmm3, xmm3, xmm1, 0x11
    vmovdqa xmm4, xmm0
    vpshufd xmm5, xmm0, 0x4e
    vpxor xmm4, xmm4, xmm5
    vmovdqa xmm5, xmm1
    vpshufd xmm0, xmm1, 0x4e
    vpxor xmm5, xmm5, xmm0
    vpclmulqdq xmm4, xmm4, xmm5, 0x00
    vpxor xmm4, xmm4, xmm2
    vpxor xmm4, xmm4, xmm3
    vmovdqa xmm5, xmm4
    vpslldq xmm4, xmm4, 8
    vpsrldq xmm5, xmm5, 8
    vpxor xmm2, xmm2, xmm4
    vpxor xmm3, xmm3, xmm5
    vpxor xmm2, xmm2, XMMWORD PTR [r8+64]
    vpxor xmm3, xmm3, XMMWORD PTR [r8+80]
    vmovdqu XMMWORD PTR [r8+64], xmm2
    vmovdqu XMMWORD PTR [r8+80], xmm3
.endm

/*  Fold four ciphertext blocks into tag. rcx=H powers, rdx=tag, r8=blocks. */
/*  The caller provides two additional 16-byte slots for the product accumulator. */
.type .Lsharkssl_gcm_gmult4_pclmul_vaes, @function
.Lsharkssl_gcm_gmult4_pclmul_vaes:
.cfi_startproc
    vpxor xmm0, xmm0, xmm0
    vmovdqu XMMWORD PTR [r8+64], xmm0
    vmovdqu XMMWORD PTR [r8+80], xmm0

    vmovdqu xmm0, XMMWORD PTR [r8]
    vmovdqu xmm1, XMMWORD PTR [rdx]
    vpxor xmm0, xmm0, xmm1
    REFLECT_VALUE xmm0, xmm1, xmm2, xmm3
    CLMUL_ACCUM 48

    vmovdqu xmm0, XMMWORD PTR [r8+16]
    REFLECT_VALUE xmm0, xmm1, xmm2, xmm3
    CLMUL_ACCUM 32

    vmovdqu xmm0, XMMWORD PTR [r8+32]
    REFLECT_VALUE xmm0, xmm1, xmm2, xmm3
    CLMUL_ACCUM 16

    vmovdqu xmm0, XMMWORD PTR [r8+48]
    REFLECT_VALUE xmm0, xmm1, xmm2, xmm3
    CLMUL_ACCUM 0

    vmovdqu xmm4, XMMWORD PTR [r8+64]
    vmovdqu xmm5, XMMWORD PTR [r8+80]
    vmovdqa xmm0, xmm5
    vpclmulqdq xmm0, xmm0, XMMWORD PTR [rip+ghash_polynomial], 0x00
    vmovdqa xmm1, xmm5
    vpclmulqdq xmm1, xmm1, XMMWORD PTR [rip+ghash_polynomial], 0x01
    vmovdqa xmm3, xmm1
    vpslldq xmm1, xmm1, 8
    vpsrldq xmm3, xmm3, 8
    vpxor xmm0, xmm0, xmm1
    vpclmulqdq xmm3, xmm3, XMMWORD PTR [rip+ghash_polynomial], 0x00
    vpxor xmm0, xmm0, xmm3
    vpxor xmm0, xmm0, xmm4
    REFLECT_VALUE xmm0, xmm1, xmm2, xmm3
    vmovdqu XMMWORD PTR [rdx], xmm0
    ret
.cfi_endproc
.size .Lsharkssl_gcm_gmult4_pclmul_vaes, .-.Lsharkssl_gcm_gmult4_pclmul_vaes

/*  Multiply x by the reflected H stored in M0[0]. */
.type .Lsharkssl_gcm_gmult32M0_vaes, @function
.Lsharkssl_gcm_gmult32M0_vaes:
.cfi_startproc
    vmovdqu xmm0, XMMWORD PTR [rdx]
    REFLECT_VALUE xmm0, xmm1, xmm2, xmm3
    vmovdqu xmm1, XMMWORD PTR [rcx]

    vmovdqa xmm4, xmm0
    vmovdqa xmm5, xmm1
    vmovdqa xmm2, xmm0
    vpclmulqdq xmm2, xmm2, xmm1, 0x00
    vmovdqa xmm3, xmm0
    vpclmulqdq xmm3, xmm3, xmm1, 0x11
    vpclmulqdq xmm4, xmm4, xmm1, 0x01
    vpclmulqdq xmm0, xmm0, xmm5, 0x10
    vpxor xmm4, xmm4, xmm0
    vmovdqa xmm5, xmm4
    vpslldq xmm4, xmm4, 8
    vpsrldq xmm5, xmm5, 8
    vpxor xmm2, xmm2, xmm4
    vpxor xmm3, xmm3, xmm5

    vmovdqa xmm0, xmm3
    vpclmulqdq xmm0, xmm0, XMMWORD PTR [rip+ghash_polynomial], 0x00
    vmovdqa xmm1, xmm3
    vpclmulqdq xmm1, xmm1, XMMWORD PTR [rip+ghash_polynomial], 0x01
    vmovdqa xmm3, xmm1
    vpslldq xmm1, xmm1, 8
    vpsrldq xmm3, xmm3, 8
    vpxor xmm0, xmm0, xmm1
    vpclmulqdq xmm3, xmm3, XMMWORD PTR [rip+ghash_polynomial], 0x00
    vpxor xmm0, xmm0, xmm3
    vpxor xmm0, xmm0, xmm2

    vmovdqa xmm4, XMMWORD PTR [rip+low_nibbles]
    vmovdqa xmm5, XMMWORD PTR [rip+reverse_nibbles]
    vmovdqa xmm2, xmm0
    vpand xmm0, xmm0, xmm4
    vpsrlw xmm2, xmm2, 4
    vpand xmm2, xmm2, xmm4
    vmovdqa xmm3, xmm5
    vpshufb xmm3, xmm3, xmm0
    vmovdqa xmm0, xmm5
    vpshufb xmm0, xmm0, xmm2
    vpsllw xmm3, xmm3, 4
    vpor xmm0, xmm0, xmm3
    vmovdqu XMMWORD PTR [rdx], xmm0
    ret
.cfi_endproc
.size .Lsharkssl_gcm_gmult32M0_vaes, .-.Lsharkssl_gcm_gmult32M0_vaes

/*  Per-lane 256-bit version of CLMUL_ACCUM. ymm5 holds two independent */
/*  byte-reversed blocks, the scaled H powers live at [rsp+powerOffset], and the */
/*  raw Karatsuba products accumulate in ymm6 (low), ymm7 (high), and ymm8 */
/*  (cross). Combine the cross terms only after all batch pairs are accumulated. */
.macro CLMUL_ACCUM256 powerOffset
    vmovdqu ymm4, YMMWORD PTR [rsp+\powerOffset]
    vpclmulqdq ymm0, ymm5, ymm4, 0x00
    vpclmulqdq ymm1, ymm5, ymm4, 0x11
    vpshufd ymm2, ymm5, 0x4e
    vpxor ymm2, ymm2, ymm5
    vpshufd ymm3, ymm4, 0x4e
    vpxor ymm3, ymm3, ymm4
    vpclmulqdq ymm2, ymm2, ymm3, 0x00
    vpxor ymm6, ymm6, ymm0
    vpxor ymm7, ymm7, ymm1
    vpxor ymm8, ymm8, ymm2
.endm

/*  One ordinary AES round for four independent counter pairs. */
.macro GCM_VAES_ROUND 
    vbroadcasti128 ymm4, XMMWORD PTR [r10]
    vpshufb ymm4, ymm4, YMMWORD PTR [rip+reverse_key_words256]
    vaesenc ymm0, ymm0, ymm4
    vaesenc ymm1, ymm1, ymm4
    vaesenc ymm2, ymm2, ymm4
    vaesenc ymm3, ymm3, ymm4
    add r10, 16
.endm

/*  Accumulate a prior ciphertext pair without touching AES states ymm0..ymm3. */
/*  The input pair is consumed; ymm4, ymm5, and ymm14 are temporary. */
.macro GCM_VAES_HASH_PENDING data, powerOffset, includeTag
    vpshufb \data, \data, YMMWORD PTR [rip+ghash_reverse_bytes256]
.if \includeTag
    vpxor \data, \data, ymm9
.endif
    vmovdqu ymm4, YMMWORD PTR [rsp+\powerOffset]
    vpclmulqdq ymm5, \data, ymm4, 0x00
    vpclmulqdq ymm14, \data, ymm4, 0x11
    vpxor ymm6, ymm6, ymm5
    vpxor ymm7, ymm7, ymm14
    vpshufd ymm5, \data, 0x4e
    vpxor ymm5, ymm5, \data
    vpshufd ymm14, ymm4, 0x4e
    vpxor ymm14, ymm14, ymm4
    vpclmulqdq ymm5, ymm5, ymm14, 0x00
    vpxor ymm8, ymm8, ymm5
.endm

/*  When pending is set, start eight ordinary AES rounds for the next batch, */
/*  then hash the preceding ciphertext before completing the AES rounds. All AES key sizes have at */
/*  least nine ordinary rounds, leaving at least one for the counted loop. */
/*  The sixteen-block caller reduces the combined products once at the end. */
.macro GCM_VAES_CRYPT8 pending=0
/*  Generate pairs (c+1,c+2) through (c+7,c+8) in registers. */
/*  Reverse bytes within each dword so VPADDD updates the low 32-bit */
/*  counter modulo 2^32 without carrying into the 96-bit nonce. */
    vbroadcasti128 ymm4, XMMWORD PTR [r13]
    vpshufb ymm4, ymm4, YMMWORD PTR [rip+reverse_key_words256]
    vpaddd ymm0, ymm4, YMMWORD PTR [rip+counter_pair12]
    vpaddd ymm1, ymm0, YMMWORD PTR [rip+counter_pair_step]
    vpaddd ymm2, ymm1, YMMWORD PTR [rip+counter_pair_step]
    vpaddd ymm3, ymm2, YMMWORD PTR [rip+counter_pair_step]
    vpshufb ymm0, ymm0, YMMWORD PTR [rip+reverse_key_words256]
    vpshufb ymm1, ymm1, YMMWORD PTR [rip+reverse_key_words256]
    vpshufb ymm2, ymm2, YMMWORD PTR [rip+reverse_key_words256]
    vpshufb ymm3, ymm3, YMMWORD PTR [rip+reverse_key_words256]
/*  Publish c+8 once, including when the next iteration is a scalar tail. */
    vextracti128 xmm4, ymm3, 1
    vmovdqu XMMWORD PTR [r13], xmm4

    vbroadcasti128 ymm4, XMMWORD PTR [r12]
    vpshufb ymm4, ymm4, YMMWORD PTR [rip+reverse_key_words256]
    vpxor ymm0, ymm0, ymm4
    vpxor ymm1, ymm1, ymm4
    vpxor ymm2, ymm2, ymm4
    vpxor ymm3, ymm3, ymm4

    lea r10, [r12+16]
    movzx eax, WORD PTR [r12+SHARKSSL_AES_NR]
    cmp eax, 7
    ja gcm_vaes_count_normalized\@
    lea eax, [rax+rax+1]

    gcm_vaes_count_normalized\@:
.if \pending
    GCM_VAES_ROUND
    GCM_VAES_ROUND
    GCM_VAES_ROUND
    GCM_VAES_ROUND
    GCM_VAES_ROUND
    GCM_VAES_ROUND
    GCM_VAES_ROUND
    GCM_VAES_ROUND
    GCM_VAES_HASH_PENDING ymm10, 192, 1
    GCM_VAES_HASH_PENDING ymm11, 224, 0
    GCM_VAES_HASH_PENDING ymm12, 256, 0
    GCM_VAES_HASH_PENDING ymm13, 288, 0
    sub eax, 8
.endif

    gcm_vaes_round_count_ready\@:
    GCM_VAES_ROUND
    dec eax
    jnz gcm_vaes_round_count_ready\@

    vbroadcasti128 ymm4, XMMWORD PTR [r10]
    vpshufb ymm4, ymm4, YMMWORD PTR [rip+reverse_key_words256]
    vaesenclast ymm0, ymm0, ymm4
    vaesenclast ymm1, ymm1, ymm4
    vaesenclast ymm2, ymm2, ymm4
    vaesenclast ymm3, ymm3, ymm4

    cmp ebx, 1
    jne gcm_vaes_decrypt\@
/*  Retain ciphertext in ymm10..ymm13 until the GHASH stage consumes it. */
    vpxor ymm10, ymm0, YMMWORD PTR [r15+0]
    vmovdqu YMMWORD PTR [rdi+0], ymm10
    vpxor ymm11, ymm1, YMMWORD PTR [r15+32]
    vmovdqu YMMWORD PTR [rdi+32], ymm11
    vpxor ymm12, ymm2, YMMWORD PTR [r15+64]
    vmovdqu YMMWORD PTR [rdi+64], ymm12
    vpxor ymm13, ymm3, YMMWORD PTR [r15+96]
    vmovdqu YMMWORD PTR [rdi+96], ymm13
    jmp gcm_vaes_auth_ready\@

    gcm_vaes_decrypt\@:
/*  Save ciphertext before overwriting an in-place input buffer. */
    vmovdqu ymm10, YMMWORD PTR [r15+0]
    vpxor ymm0, ymm0, ymm10
    vmovdqu YMMWORD PTR [rdi+0], ymm0
    vmovdqu ymm11, YMMWORD PTR [r15+32]
    vpxor ymm1, ymm1, ymm11
    vmovdqu YMMWORD PTR [rdi+32], ymm1
    vmovdqu ymm12, YMMWORD PTR [r15+64]
    vpxor ymm2, ymm2, ymm12
    vmovdqu YMMWORD PTR [rdi+64], ymm2
    vmovdqu ymm13, YMMWORD PTR [r15+96]
    vpxor ymm3, ymm3, ymm13
    vmovdqu YMMWORD PTR [rdi+96], ymm3

    gcm_vaes_auth_ready\@:
.endm

/*  Consecutive ciphertext pairs match consecutive descending powers. */
/*  Each lane contributes independently until the final horizontal XOR. */
.macro GCM_VAES_HASH8 powerBase, includeTag
    vpshufb ymm5, ymm10, YMMWORD PTR [rip+ghash_reverse_bytes256]
.if \includeTag
    vpxor ymm5, ymm5, ymm9
.endif
    CLMUL_ACCUM256 \powerBase+0

    vpshufb ymm5, ymm11, YMMWORD PTR [rip+ghash_reverse_bytes256]
    CLMUL_ACCUM256 \powerBase+32

    vpshufb ymm5, ymm12, YMMWORD PTR [rip+ghash_reverse_bytes256]
    CLMUL_ACCUM256 \powerBase+64

    vpshufb ymm5, ymm13, YMMWORD PTR [rip+ghash_reverse_bytes256]
    CLMUL_ACCUM256 \powerBase+96

.endm

.macro GCM_VAES_REDUCE 
    vpxor ymm8, ymm8, ymm6
    vpxor ymm8, ymm8, ymm7
/*  Montgomery reduction divides the product by x^128 modulo P*. */
/*  Each step cancels one low 64-bit limb using P*'s high limb. */
    vmovdqu ymm4, YMMWORD PTR [rip+ghash_reciprocal256]
    vpclmulqdq ymm0, ymm4, ymm6, 0x01
    vpshufd ymm1, ymm6, 0x4e
    vpxor ymm8, ymm8, ymm0
    vpxor ymm8, ymm8, ymm1
    vpclmulqdq ymm0, ymm4, ymm8, 0x01
    vpshufd ymm1, ymm8, 0x4e
    vpxor ymm7, ymm7, ymm0
    vpxor ymm7, ymm7, ymm1
    vextracti128 xmm0, ymm7, 1
    vpxor xmm9, xmm7, xmm0

.endm

/*  Eight-/sixteen-block VAES/VPCLMULQDQ pipeline. */

/*  Stack layout after the prologue (rsp): */
/*    [0..95]     scratch used by four-block/single-block tails */
/*    [96..111]   saved xmm14; [112..127] unused */
/*    [128..191]  saved xmm10..xmm13, 16 bytes each */
/*    [192..319]  paired scaled H^16..H^9 (large inputs only) */
/*    [320..447]  paired scaled H^8..H^1 */
/*    [448..511]  saved xmm6..xmm9, 16 bytes each */
/*  Ciphertext stays in ymm10..ymm13 between the AES and GHASH stages. */
/*  The overlap stage also uses ymm14 as a temporary; the frame size is unchanged. */
/*  Only the low 128 bits of these registers are nonvolatile on Windows x64. */

.globl sharkssl_gcm_crypt_blocks_vaes
.type sharkssl_gcm_crypt_blocks_vaes, @function
sharkssl_gcm_crypt_blocks_vaes:
.cfi_startproc
    sub rsp, 56
.cfi_adjust_cfa_offset 56
    mov QWORD PTR [rsp+32], r8
    mov QWORD PTR [rsp+40], r9
    mov rax, QWORD PTR [rsp+64]
    mov QWORD PTR [rsp+48], rax
    mov r10, rcx
    mov r11, rdx
    mov rcx, rdi
    mov rdx, rsi
    mov r8, r11
    mov r9, r10
    call .Lsharkssl_gcm_crypt_blocks_vaes
    add rsp, 56
.cfi_adjust_cfa_offset -56
    ret
.cfi_endproc
.size sharkssl_gcm_crypt_blocks_vaes, .-sharkssl_gcm_crypt_blocks_vaes
.type .Lsharkssl_gcm_crypt_blocks_vaes, @function
.Lsharkssl_gcm_crypt_blocks_vaes:
.cfi_startproc
    push rbx
.cfi_adjust_cfa_offset 8
.cfi_offset rbx, -(8+8)
    push rsi
.cfi_adjust_cfa_offset 8
.cfi_offset rsi, -(16+8)
    push rdi
.cfi_adjust_cfa_offset 8
.cfi_offset rdi, -(24+8)
    push r12
.cfi_adjust_cfa_offset 8
.cfi_offset r12, -(32+8)
    push r13
.cfi_adjust_cfa_offset 8
.cfi_offset r13, -(40+8)
    push r14
.cfi_adjust_cfa_offset 8
.cfi_offset r14, -(48+8)
    push r15
.cfi_adjust_cfa_offset 8
.cfi_offset r15, -(56+8)
    sub rsp, GCM_VAES_FRAME_BYTES
.cfi_adjust_cfa_offset 512
    vmovdqu XMMWORD PTR [rsp+448], xmm6
    vmovdqu XMMWORD PTR [rsp+464], xmm7
    vmovdqu XMMWORD PTR [rsp+480], xmm8
    vmovdqu XMMWORD PTR [rsp+496], xmm9
    vmovdqu XMMWORD PTR [rsp+128], xmm10
    vmovdqu XMMWORD PTR [rsp+144], xmm11
    vmovdqu XMMWORD PTR [rsp+160], xmm12
    vmovdqu XMMWORD PTR [rsp+176], xmm13
    vmovdqu XMMWORD PTR [rsp+96], xmm14

    mov r12, rcx
    mov r13, rdx
    mov r14, r8
    mov r15, r9
    mov rdi, QWORD PTR [rsp+GCM_VAES_FRAME_BYTES+GCM_VAES_SAVED_BYTES+40]
    mov esi, DWORD PTR [rsp+GCM_VAES_FRAME_BYTES+GCM_VAES_SAVED_BYTES+48]
    mov ebx, DWORD PTR [rsp+GCM_VAES_FRAME_BYTES+GCM_VAES_SAVED_BYTES+56]

    cmp esi, 128
    jb gcm_vaes_tail

/*  Pair consecutive powers to match adjacent ciphertext blocks. */
    vmovdqu xmm0, XMMWORD PTR [r12+SHARKSSL_GCM_M0+112]
    vinserti128 ymm0, ymm0, XMMWORD PTR [r12+SHARKSSL_GCM_M0+96], 1
    vmovdqu YMMWORD PTR [rsp+320], ymm0
    vmovdqu xmm0, XMMWORD PTR [r12+SHARKSSL_GCM_M0+80]
    vinserti128 ymm0, ymm0, XMMWORD PTR [r12+SHARKSSL_GCM_M0+64], 1
    vmovdqu YMMWORD PTR [rsp+352], ymm0
    vmovdqu xmm0, XMMWORD PTR [r12+SHARKSSL_GCM_M0+48]
    vinserti128 ymm0, ymm0, XMMWORD PTR [r12+SHARKSSL_GCM_M0+32], 1
    vmovdqu YMMWORD PTR [rsp+384], ymm0
    vmovdqu xmm0, XMMWORD PTR [r12+SHARKSSL_GCM_M0+16]
    vinserti128 ymm0, ymm0, XMMWORD PTR [r12+SHARKSSL_GCM_M0+0], 1
    vmovdqu YMMWORD PTR [rsp+416], ymm0

/*  Convert the four paired powers into per-call Montgomery scratch. */
/*  The context and the narrower tail routines retain their old format. */
    lea r10, [rsp+320]
    mov eax, 4
    gcm_vaes_convert_powers:
    vmovdqu ymm5, YMMWORD PTR [r10]
    REFLECT_VALUE256 ymm5, ymm0, ymm1
    vpshufb ymm5, ymm5, YMMWORD PTR [rip+ghash_reverse_bytes256]
/*  Multiply each 128-bit value by x modulo the reciprocal polynomial. */
    vpshufd ymm2, ymm5, 0x0ff
    vpsrad ymm2, ymm2, 31
    vpand ymm2, ymm2, YMMWORD PTR [rip+ghash_reciprocal256]
    vpsrlq ymm0, ymm5, 63
    vpslldq ymm0, ymm0, 8
    vpsllq ymm5, ymm5, 1
    vpor ymm5, ymm5, ymm0
    vpxor ymm5, ymm5, ymm2
    vmovdqu YMMWORD PTR [r10], ymm5
    add r10, 32
    dec eax
    jnz gcm_vaes_convert_powers

/*  Keep the tag byte-reversed across all bulk batches. VEX.128 */
/*  instructions leave the upper half of ymm9 zero for the lane-0 XOR. */
    vmovdqu xmm9, XMMWORD PTR [r14]
    vpshufb xmm9, xmm9, XMMWORD PTR [rip+ghash_reverse_bytes256]

/*  H^9..H^16 are needed only for inputs large enough to amortize setup. */
    cmp esi, 1024
    jb gcm_vaes_eight_block_loop
/*  Multiplying scaled powers with REDC preserves their scale factor. */
    vbroadcasti128 ymm4, XMMWORD PTR [rsp+320]
    vmovdqu ymm6, YMMWORD PTR [rip+ghash_reciprocal256]
    lea r10, [rsp+320]
    mov ecx, 4
    gcm_vaes_extend_powers:
    vmovdqu ymm5, YMMWORD PTR [r10]
    vpclmulqdq ymm0, ymm5, ymm4, 0x00
    vpclmulqdq ymm1, ymm5, ymm4, 0x11
    vpshufd ymm2, ymm5, 0x4e
    vpxor ymm2, ymm2, ymm5
    vpshufd ymm3, ymm4, 0x4e
    vpxor ymm3, ymm3, ymm4
    vpclmulqdq ymm2, ymm2, ymm3, 0x00
    vpxor ymm2, ymm2, ymm0
    vpxor ymm2, ymm2, ymm1
    vpclmulqdq ymm5, ymm6, ymm0, 0x01
    vpshufd ymm0, ymm0, 0x4e
    vpxor ymm2, ymm2, ymm5
    vpxor ymm2, ymm2, ymm0
    vpclmulqdq ymm5, ymm6, ymm2, 0x01
    vpshufd ymm2, ymm2, 0x4e
    vpxor ymm1, ymm1, ymm5
    vpxor ymm1, ymm1, ymm2
    vmovdqu YMMWORD PTR [r10-128], ymm1
    add r10, 32
    dec ecx
    jnz gcm_vaes_extend_powers

    gcm_vaes_sixteen_block_loop:
    vpxor ymm6, ymm6, ymm6
    vpxor ymm7, ymm7, ymm7
    vpxor ymm8, ymm8, ymm8
    GCM_VAES_CRYPT8
    add r15, 128
    add rdi, 128
/*  Consume the first half with H^16..H^9 while encrypting the second. */
    GCM_VAES_CRYPT8 1
    GCM_VAES_HASH8 320, 0
    GCM_VAES_REDUCE
    add r15, 128
    add rdi, 128
    sub esi, 256
    cmp esi, 256
    jae gcm_vaes_sixteen_block_loop
    cmp esi, 128
    jb gcm_vaes_publish_tag

    gcm_vaes_eight_block_loop:
    vpxor ymm6, ymm6, ymm6
    vpxor ymm7, ymm7, ymm7
    vpxor ymm8, ymm8, ymm8
    GCM_VAES_CRYPT8
    GCM_VAES_HASH8 320, 1
    GCM_VAES_REDUCE
    add r15, 128
    add rdi, 128
    sub esi, 128
    cmp esi, 128
    jae gcm_vaes_eight_block_loop
    gcm_vaes_publish_tag:
/*  Publish the external tag format once, before returning or entering */
/*  the four-block/single-block tails that use the existing representation. */
    vpshufb xmm9, xmm9, XMMWORD PTR [rip+ghash_reverse_bytes256]
    vmovdqu XMMWORD PTR [r14], xmm9
    test esi, esi
    jz gcm_vaes_blocks_done

    gcm_vaes_tail:
    cmp esi, 64
    jb gcm_vaes_block_loop

    gcm_vaes_four_block_loop:
/*  Materialize four consecutive big-endian counters. */
    mov eax, DWORD PTR [r13+12]
    bswap eax
    inc eax
    mov edx, eax
    bswap edx
    mov DWORD PTR [r13+12], edx
    vmovdqu xmm0, XMMWORD PTR [r13]
    inc eax
    mov edx, eax
    bswap edx
    mov DWORD PTR [r13+12], edx
    vmovdqu xmm1, XMMWORD PTR [r13]
    inc eax
    mov edx, eax
    bswap edx
    mov DWORD PTR [r13+12], edx
    vmovdqu xmm2, XMMWORD PTR [r13]
    inc eax
    mov edx, eax
    bswap edx
    mov DWORD PTR [r13+12], edx
    vmovdqu xmm3, XMMWORD PTR [r13]

    vmovdqa xmm5, XMMWORD PTR [rip+reverse_key_words]
    vmovdqu xmm4, XMMWORD PTR [r12]
    vpshufb xmm4, xmm4, xmm5
    vpxor xmm0, xmm0, xmm4
    vpxor xmm1, xmm1, xmm4
    vpxor xmm2, xmm2, xmm4
    vpxor xmm3, xmm3, xmm4

    lea r10, [r12+16]
    movzx eax, WORD PTR [r12+SHARKSSL_AES_NR]
    cmp eax, 7
    ja gcm_vaes_four_round_count_ready
    lea eax, [rax+rax+1]

    gcm_vaes_four_round_count_ready:
    vmovdqu xmm4, XMMWORD PTR [r10]
    vpshufb xmm4, xmm4, xmm5
    vaesenc xmm0, xmm0, xmm4
    vaesenc xmm1, xmm1, xmm4
    vaesenc xmm2, xmm2, xmm4
    vaesenc xmm3, xmm3, xmm4
    add r10, 16
    dec eax
    jnz gcm_vaes_four_round_count_ready

    vmovdqu xmm4, XMMWORD PTR [r10]
    vpshufb xmm4, xmm4, xmm5
    vaesenclast xmm0, xmm0, xmm4
    vaesenclast xmm1, xmm1, xmm4
    vaesenclast xmm2, xmm2, xmm4
    vaesenclast xmm3, xmm3, xmm4

    vmovdqu xmm4, XMMWORD PTR [r15]
    vmovdqu XMMWORD PTR [rsp+0], xmm4
    vpxor xmm0, xmm0, xmm4
    vmovdqu XMMWORD PTR [rdi], xmm0
    vmovdqu xmm4, XMMWORD PTR [r15+16]
    vmovdqu XMMWORD PTR [rsp+16], xmm4
    vpxor xmm1, xmm1, xmm4
    vmovdqu XMMWORD PTR [rdi+16], xmm1
    vmovdqu xmm4, XMMWORD PTR [r15+32]
    vmovdqu XMMWORD PTR [rsp+32], xmm4
    vpxor xmm2, xmm2, xmm4
    vmovdqu XMMWORD PTR [rdi+32], xmm2
    vmovdqu xmm4, XMMWORD PTR [r15+48]
    vmovdqu XMMWORD PTR [rsp+48], xmm4
    vpxor xmm3, xmm3, xmm4
    vmovdqu XMMWORD PTR [rdi+48], xmm3

    cmp ebx, 1
    jne gcm_vaes_four_auth_ready
    vmovdqu XMMWORD PTR [rsp+0], xmm0
    vmovdqu XMMWORD PTR [rsp+16], xmm1
    vmovdqu XMMWORD PTR [rsp+32], xmm2
    vmovdqu XMMWORD PTR [rsp+48], xmm3

    gcm_vaes_four_auth_ready:
    lea rcx, [r12+SHARKSSL_GCM_M0]
    mov rdx, r14
    lea r8, [rsp+0]
    call .Lsharkssl_gcm_gmult4_pclmul_vaes

    add r15, 64
    add rdi, 64
    sub esi, 64
    cmp esi, 64
    jae gcm_vaes_four_block_loop
    test esi, esi
    jz gcm_vaes_blocks_done

    gcm_vaes_block_loop:
/*  Increment the low 32 bits of the big-endian GCM counter. */
    mov eax, DWORD PTR [r13+12]
    bswap eax
    inc eax
    bswap eax
    mov DWORD PTR [r13+12], eax

    vmovdqu xmm0, XMMWORD PTR [r13]
    vmovdqa xmm2, XMMWORD PTR [rip+reverse_key_words]
    vmovdqu xmm1, XMMWORD PTR [r12]
    vpshufb xmm1, xmm1, xmm2
    vpxor xmm0, xmm0, xmm1

    lea r10, [r12+16]
    movzx eax, WORD PTR [r12+SHARKSSL_AES_NR]
    cmp eax, 7
    ja gcm_vaes_block_round_count_ready
    lea eax, [rax+rax+1]

    gcm_vaes_block_round_count_ready:
    vmovdqu xmm1, XMMWORD PTR [r10]
    vpshufb xmm1, xmm1, xmm2
    vaesenc xmm0, xmm0, xmm1
    add r10, 16
    dec eax
    jnz gcm_vaes_block_round_count_ready

    vmovdqu xmm1, XMMWORD PTR [r10]
    vpshufb xmm1, xmm1, xmm2
    vaesenclast xmm0, xmm0, xmm1

    vmovdqu xmm3, XMMWORD PTR [r15]
    vpxor xmm0, xmm0, xmm3
    vmovdqu XMMWORD PTR [rdi], xmm0

    vmovdqu xmm1, XMMWORD PTR [r14]
    cmp ebx, 1
    jne gcm_vaes_decrypt_auth
    vpxor xmm1, xmm1, xmm0
    jmp gcm_vaes_authenticate

    gcm_vaes_decrypt_auth:
    vpxor xmm1, xmm1, xmm3

    gcm_vaes_authenticate:
    vmovdqu XMMWORD PTR [r14], xmm1
    lea rcx, [r12+SHARKSSL_GCM_M0]
    mov rdx, r14
    call .Lsharkssl_gcm_gmult32M0_vaes

    add r15, 16
    add rdi, 16
    sub esi, 16
    jnz gcm_vaes_block_loop

    gcm_vaes_blocks_done:
    vmovdqu xmm6, XMMWORD PTR [rsp+448]
    vmovdqu xmm7, XMMWORD PTR [rsp+464]
    vmovdqu xmm8, XMMWORD PTR [rsp+480]
    vmovdqu xmm9, XMMWORD PTR [rsp+496]
    vmovdqu xmm10, XMMWORD PTR [rsp+128]
    vmovdqu xmm11, XMMWORD PTR [rsp+144]
    vmovdqu xmm12, XMMWORD PTR [rsp+160]
    vmovdqu xmm13, XMMWORD PTR [rsp+176]
    vmovdqu xmm14, XMMWORD PTR [rsp+96]
    vzeroupper
    add rsp, GCM_VAES_FRAME_BYTES
.cfi_adjust_cfa_offset -512
    pop r15
.cfi_adjust_cfa_offset -8
.cfi_restore r15
    pop r14
.cfi_adjust_cfa_offset -8
.cfi_restore r14
    pop r13
.cfi_adjust_cfa_offset -8
.cfi_restore r13
    pop r12
.cfi_adjust_cfa_offset -8
.cfi_restore r12
    pop rdi
.cfi_adjust_cfa_offset -8
.cfi_restore rdi
    pop rsi
.cfi_adjust_cfa_offset -8
.cfi_restore rsi
    pop rbx
.cfi_adjust_cfa_offset -8
.cfi_restore rbx
    ret
.cfi_endproc
.size .Lsharkssl_gcm_crypt_blocks_vaes, .-.Lsharkssl_gcm_crypt_blocks_vaes

.section .note.GNU-stack,"",@progbits
