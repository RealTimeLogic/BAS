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
/* *   $Id: SharkSslCrypto_X64_AVX2.asm 6169 2026-09-26 18:15:48Z wini $ */
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

/*  SharkSSL ChaCha20 AVX2 backend for Microsoft x64 MASM. */

/*  SharkSslCrypto.c dispatches to this kernel only after a CPUID probe */
/*  reports AVX2, OSXSAVE and XMM/YMM state support. */

/*  U32 sharkssl_chacha_crypt_avx2(SharkSslChaChaCtx *ctx, const U8 *input, */
/*                                 U8 *output, U32 len); */
/*    Processes all len bytes and returns len. Eight-block batches use YMM */
/*    registers; short messages and remainders use a single block in XMM registers. */
/*    Input and output may alias. */


.section .rodata
.balign 16
chacha_counter_increment: .long 0, 1, 2, 3, 4, 5, 6, 7
chacha_rol16_shuf: .byte 2,3,0,1, 6,7,4,5, 10,11,8,9, 14,15,12,13
.byte 2,3,0,1, 6,7,4,5, 10,11,8,9, 14,15,12,13
chacha_rol8_shuf: .byte 3,0,1,2, 7,4,5,6, 11,8,9,10, 15,12,13,14
.byte 3,0,1,2, 7,4,5,6, 11,8,9,10, 15,12,13,14

.text

/*  Rotate ymm&reg left by 16 or 8 bits: byte shuffles, no temp register. */
.macro CHACHA_ROT16 reg
    vpshufb ymm\reg, ymm\reg, YMMWORD PTR [rip+chacha_rol16_shuf]
.endm

.macro CHACHA_ROT8 reg
    vpshufb ymm\reg, ymm\reg, YMMWORD PTR [rip+chacha_rol8_shuf]
.endm

/*  Rotate four independent B words with one saved helper for the whole group. */
/*  ymm0 holds an A word; it is restored before the next arithmetic phase. */
.macro CHACHA_ROT4 b0, b1, b2, b3, n
    vmovdqu YMMWORD PTR [rsp+512], ymm0
    vpslld ymm0, ymm\b0, \n
    vpsrld ymm\b0, ymm\b0, (32 - \n)
    vpor ymm\b0, ymm\b0, ymm0
    vpslld ymm0, ymm\b1, \n
    vpsrld ymm\b1, ymm\b1, (32 - \n)
    vpor ymm\b1, ymm\b1, ymm0
    vpslld ymm0, ymm\b2, \n
    vpsrld ymm\b2, ymm\b2, (32 - \n)
    vpor ymm\b2, ymm\b2, ymm0
    vpslld ymm0, ymm\b3, \n
    vpsrld ymm\b3, ymm\b3, (32 - \n)
    vpor ymm\b3, ymm\b3, ymm0
    vmovdqu ymm0, YMMWORD PTR [rsp+512]
.endm

/*  Execute each phase of four independent quarter-rounds before moving on. */
.macro CHACHA_QR4 b0, b1, b2, b3, c0, c1, c2, c3, d0, d1, d2, d3
    vpaddd ymm0, ymm0, ymm\b0
    vpaddd ymm1, ymm1, ymm\b1
    vpaddd ymm2, ymm2, ymm\b2
    vpaddd ymm3, ymm3, ymm\b3
    vpxor ymm\d0, ymm\d0, ymm0
    vpxor ymm\d1, ymm\d1, ymm1
    vpxor ymm\d2, ymm\d2, ymm2
    vpxor ymm\d3, ymm\d3, ymm3
    CHACHA_ROT16 \d0
    CHACHA_ROT16 \d1
    CHACHA_ROT16 \d2
    CHACHA_ROT16 \d3
    vpaddd ymm\c0, ymm\c0, ymm\d0
    vpaddd ymm\c1, ymm\c1, ymm\d1
    vpaddd ymm\c2, ymm\c2, ymm\d2
    vpaddd ymm\c3, ymm\c3, ymm\d3
    vpxor ymm\b0, ymm\b0, ymm\c0
    vpxor ymm\b1, ymm\b1, ymm\c1
    vpxor ymm\b2, ymm\b2, ymm\c2
    vpxor ymm\b3, ymm\b3, ymm\c3
    CHACHA_ROT4 \b0, \b1, \b2, \b3, 12
    vpaddd ymm0, ymm0, ymm\b0
    vpaddd ymm1, ymm1, ymm\b1
    vpaddd ymm2, ymm2, ymm\b2
    vpaddd ymm3, ymm3, ymm\b3
    vpxor ymm\d0, ymm\d0, ymm0
    vpxor ymm\d1, ymm\d1, ymm1
    vpxor ymm\d2, ymm\d2, ymm2
    vpxor ymm\d3, ymm\d3, ymm3
    CHACHA_ROT8 \d0
    CHACHA_ROT8 \d1
    CHACHA_ROT8 \d2
    CHACHA_ROT8 \d3
    vpaddd ymm\c0, ymm\c0, ymm\d0
    vpaddd ymm\c1, ymm\c1, ymm\d1
    vpaddd ymm\c2, ymm\c2, ymm\d2
    vpaddd ymm\c3, ymm\c3, ymm\d3
    vpxor ymm\b0, ymm\b0, ymm\c0
    vpxor ymm\b1, ymm\b1, ymm\c1
    vpxor ymm\b2, ymm\b2, ymm\c2
    vpxor ymm\b3, ymm\b3, ymm\c3
    CHACHA_ROT4 \b0, \b1, \b2, \b3, 7
.endm

.macro CHACHA_DOUBLE_ROUND 
    CHACHA_QR4 4,5,6,7, 8,9,10,11, 12,13,14,15
    CHACHA_QR4 5,6,7,4, 10,11,8,9, 15,12,13,14
.endm

/*  Transpose word group g (words 4g..4g+3, word-major across 8 blocks). */
/*  Inputs are read from the spill slots; results are stored back so that */
/*  slot [rsp+128*g+32*k] holds words 4g..4g+3 of block k in the low half */
/*  and of block k+4 in the high half. */
.macro CHACHA_TRANSPOSE_GROUP g
    vmovdqu ymm0, YMMWORD PTR [rsp+128*(\g)+0]
    vmovdqu ymm1, YMMWORD PTR [rsp+128*(\g)+32]
    vmovdqu ymm2, YMMWORD PTR [rsp+128*(\g)+64]
    vmovdqu ymm3, YMMWORD PTR [rsp+128*(\g)+96]
    vpunpckldq ymm4, ymm0, ymm1
    vpunpckhdq ymm5, ymm0, ymm1
    vpunpckldq ymm6, ymm2, ymm3
    vpunpckhdq ymm7, ymm2, ymm3
    vpunpcklqdq ymm8, ymm4, ymm6
    vpunpckhqdq ymm9, ymm4, ymm6
    vpunpcklqdq ymm10, ymm5, ymm7
    vpunpckhqdq ymm11, ymm5, ymm7
    vmovdqu YMMWORD PTR [rsp+128*(\g)+0], ymm8
    vmovdqu YMMWORD PTR [rsp+128*(\g)+32], ymm9
    vmovdqu YMMWORD PTR [rsp+128*(\g)+64], ymm10
    vmovdqu YMMWORD PTR [rsp+128*(\g)+96], ymm11
.endm

/*  Emit blocks k and k+4 from the transposed spill layout. rdx=input, */
/*  r8=output. Slots [rsp+32*k], [rsp+128+32*k], [rsp+256+32*k], */
/*  [rsp+384+32*k] hold word groups 0..3 of block k (low) and k+4 (high). */
.macro CHACHA_EMIT_BLOCK k
    vmovdqu ymm0, YMMWORD PTR [rsp+32*\k]
    vmovdqu ymm1, YMMWORD PTR [rsp+128+32*\k]
    vmovdqu ymm2, YMMWORD PTR [rsp+256+32*\k]
    vmovdqu ymm3, YMMWORD PTR [rsp+384+32*\k]
    vperm2i128 ymm4, ymm0, ymm1, 0x20
    vperm2i128 ymm5, ymm2, ymm3, 0x20
    vperm2i128 ymm6, ymm0, ymm1, 0x31
    vperm2i128 ymm7, ymm2, ymm3, 0x31
    vmovdqu ymm0, YMMWORD PTR [rdx+64*\k]
    vpxor ymm4, ymm4, ymm0
    vmovdqu YMMWORD PTR [r8+64*\k], ymm4
    vmovdqu ymm0, YMMWORD PTR [rdx+64*\k+32]
    vpxor ymm5, ymm5, ymm0
    vmovdqu YMMWORD PTR [r8+64*\k+32], ymm5
    vmovdqu ymm0, YMMWORD PTR [rdx+256+64*\k]
    vpxor ymm6, ymm6, ymm0
    vmovdqu YMMWORD PTR [r8+256+64*\k], ymm6
    vmovdqu ymm0, YMMWORD PTR [rdx+256+64*\k+32]
    vpxor ymm7, ymm7, ymm0
    vmovdqu YMMWORD PTR [r8+256+64*\k+32], ymm7
.endm

/*  ymm0..ymm15 hold the 16 ChaCha words across 8 blocks: lane j of ymm_i is */
/*  word i of block j. Stack layout after the prologue: */
/*    [rsp+0..512)    16 x 32-byte spill slots, i at [rsp+32*i] */
/*    [rsp+512..544)  rotate scratch slot */
/*    [rsp+544..704)  saved xmm6..xmm15 */
/*  [rsp+32*i] first holds the original state word i, then the final */
/*  keystream word i during the transpose. */

/*  Public dispatcher and short-block path use only volatile registers. The */
/*  72-byte frame supplies shadow space for the bulk call, a 64-byte tail buffer, */
/*  and the original length at +64. No nonvolatile XMM saves on short calls. */
.globl sharkssl_chacha_crypt_avx2
.type sharkssl_chacha_crypt_avx2, @function
sharkssl_chacha_crypt_avx2:
.cfi_startproc
    sub rsp, 40
.cfi_adjust_cfa_offset 40
    mov r10, rcx
    mov r11, rdx
    mov rcx, rdi
    mov rdx, rsi
    mov r8, r11
    mov r9, r10
    call .Lsharkssl_chacha_crypt_avx2
    add rsp, 40
.cfi_adjust_cfa_offset -40
    ret
.cfi_endproc
.size sharkssl_chacha_crypt_avx2, .-sharkssl_chacha_crypt_avx2
.type .Lsharkssl_chacha_crypt_avx2, @function
.Lsharkssl_chacha_crypt_avx2:
.cfi_startproc
    sub rsp, 72
.cfi_adjust_cfa_offset 72
    mov DWORD PTR [rsp+64], r9d
    cmp r9d, 512
    jb small_dispatch
    call .Lchacha_bulk
    small_dispatch:
    test r9d, r9d
    jz small_done
/*  Four rows stay in xmm0-3 for all rounds; xmm4 supplies rotate scratch. */
    small_block:
    vmovdqu xmm0, XMMWORD PTR [rcx+0]
    vmovdqu xmm1, XMMWORD PTR [rcx+16]
    vmovdqu xmm2, XMMWORD PTR [rcx+32]
    vmovdqu xmm3, XMMWORD PTR [rcx+48]
    mov r10d, 10
    small_round:
    vpaddd xmm0, xmm0, xmm1
    vpxor xmm3, xmm3, xmm0
    vpshufb xmm3, xmm3, XMMWORD PTR [rip+chacha_rol16_shuf]
    vpaddd xmm2, xmm2, xmm3
    vpxor xmm1, xmm1, xmm2
    vpslld xmm4, xmm1, 12
    vpsrld xmm1, xmm1, 20
    vpor xmm1, xmm1, xmm4
    vpaddd xmm0, xmm0, xmm1
    vpxor xmm3, xmm3, xmm0
    vpshufb xmm3, xmm3, XMMWORD PTR [rip+chacha_rol8_shuf]
    vpaddd xmm2, xmm2, xmm3
    vpxor xmm1, xmm1, xmm2
    vpslld xmm4, xmm1, 7
    vpsrld xmm1, xmm1, 25
    vpor xmm1, xmm1, xmm4
    vpshufd xmm1, xmm1, 0x39
    vpshufd xmm2, xmm2, 0x4E
    vpshufd xmm3, xmm3, 0x93
    vpaddd xmm0, xmm0, xmm1
    vpxor xmm3, xmm3, xmm0
    vpshufb xmm3, xmm3, XMMWORD PTR [rip+chacha_rol16_shuf]
    vpaddd xmm2, xmm2, xmm3
    vpxor xmm1, xmm1, xmm2
    vpslld xmm4, xmm1, 12
    vpsrld xmm1, xmm1, 20
    vpor xmm1, xmm1, xmm4
    vpaddd xmm0, xmm0, xmm1
    vpxor xmm3, xmm3, xmm0
    vpshufb xmm3, xmm3, XMMWORD PTR [rip+chacha_rol8_shuf]
    vpaddd xmm2, xmm2, xmm3
    vpxor xmm1, xmm1, xmm2
    vpslld xmm4, xmm1, 7
    vpsrld xmm1, xmm1, 25
    vpor xmm1, xmm1, xmm4
    vpshufd xmm1, xmm1, 0x93
    vpshufd xmm2, xmm2, 0x4E
    vpshufd xmm3, xmm3, 0x39
    dec r10d
    jnz small_round
    vpaddd xmm0, xmm0, XMMWORD PTR [rcx+0]
    vpaddd xmm1, xmm1, XMMWORD PTR [rcx+16]
    vpaddd xmm2, xmm2, XMMWORD PTR [rcx+32]
    vpaddd xmm3, xmm3, XMMWORD PTR [rcx+48]
    add DWORD PTR [rcx+48], 1
    adc DWORD PTR [rcx+52], 0
    cmp r9d, 64
    jb small_tail
    vpxor xmm0, xmm0, XMMWORD PTR [rdx+0]
    vmovdqu XMMWORD PTR [r8+0], xmm0
    vpxor xmm1, xmm1, XMMWORD PTR [rdx+16]
    vmovdqu XMMWORD PTR [r8+16], xmm1
    vpxor xmm2, xmm2, XMMWORD PTR [rdx+32]
    vmovdqu XMMWORD PTR [r8+32], xmm2
    vpxor xmm3, xmm3, XMMWORD PTR [rdx+48]
    vmovdqu XMMWORD PTR [r8+48], xmm3
    add rdx, 64
    add r8, 64
    sub r9d, 64
    jnz small_block
    jmp small_done
    small_tail:
    vmovdqu XMMWORD PTR [rsp+0], xmm0
    vmovdqu XMMWORD PTR [rsp+16], xmm1
    vmovdqu XMMWORD PTR [rsp+32], xmm2
    vmovdqu XMMWORD PTR [rsp+48], xmm3
    xor r10d, r10d
    cmp r9d, 16
    jb small_bytes
    small_vector:
    vmovdqu xmm0, XMMWORD PTR [rdx+r10]
    vpxor xmm0, xmm0, XMMWORD PTR [rsp+r10]
    vmovdqu XMMWORD PTR [r8+r10], xmm0
    add r10d, 16
    sub r9d, 16
    cmp r9d, 16
    jae small_vector
    small_bytes:
    test r9d, r9d
    jz small_done
    small_byte:
    mov al, [rdx+r10]
    xor al, [rsp+r10]
    mov [r8+r10], al
    inc r10d
    dec r9d
    jnz small_byte
    small_done:
    mov eax, DWORD PTR [rsp+64]
    vzeroupper
    add rsp, 72
.cfi_adjust_cfa_offset -72
    ret
.cfi_endproc
.size .Lsharkssl_chacha_crypt_avx2, .-.Lsharkssl_chacha_crypt_avx2

.type .Lchacha_bulk, @function
.Lchacha_bulk:
.cfi_startproc
    sub rsp, 704
.cfi_adjust_cfa_offset 704
    vmovdqu XMMWORD PTR [rsp+544], xmm6
    vmovdqu XMMWORD PTR [rsp+560], xmm7
    vmovdqu XMMWORD PTR [rsp+576], xmm8
    vmovdqu XMMWORD PTR [rsp+592], xmm9
    vmovdqu XMMWORD PTR [rsp+608], xmm10
    vmovdqu XMMWORD PTR [rsp+624], xmm11
    vmovdqu XMMWORD PTR [rsp+640], xmm12
    vmovdqu XMMWORD PTR [rsp+656], xmm13
    vmovdqu XMMWORD PTR [rsp+672], xmm14
    vmovdqu XMMWORD PTR [rsp+688], xmm15

    xor eax, eax
    cmp r9d, 512
    jb chacha_done

    chacha_loop:
/*  Broadcast the 16 state words across 8 lanes; word 12 receives the */
/*  per-block counter increment. Keep the originals in the spill slots. */
    vpbroadcastd ymm0, DWORD PTR [rcx]
    vmovdqu YMMWORD PTR [rsp], ymm0
    vpbroadcastd ymm1, DWORD PTR [rcx+4]
    vmovdqu YMMWORD PTR [rsp+32], ymm1
    vpbroadcastd ymm2, DWORD PTR [rcx+8]
    vmovdqu YMMWORD PTR [rsp+64], ymm2
    vpbroadcastd ymm3, DWORD PTR [rcx+12]
    vmovdqu YMMWORD PTR [rsp+96], ymm3
    vpbroadcastd ymm4, DWORD PTR [rcx+16]
    vmovdqu YMMWORD PTR [rsp+128], ymm4
    vpbroadcastd ymm5, DWORD PTR [rcx+20]
    vmovdqu YMMWORD PTR [rsp+160], ymm5
    vpbroadcastd ymm6, DWORD PTR [rcx+24]
    vmovdqu YMMWORD PTR [rsp+192], ymm6
    vpbroadcastd ymm7, DWORD PTR [rcx+28]
    vmovdqu YMMWORD PTR [rsp+224], ymm7
    vpbroadcastd ymm8, DWORD PTR [rcx+32]
    vmovdqu YMMWORD PTR [rsp+256], ymm8
    vpbroadcastd ymm9, DWORD PTR [rcx+36]
    vmovdqu YMMWORD PTR [rsp+288], ymm9
    vpbroadcastd ymm10, DWORD PTR [rcx+40]
    vmovdqu YMMWORD PTR [rsp+320], ymm10
    vpbroadcastd ymm11, DWORD PTR [rcx+44]
    vmovdqu YMMWORD PTR [rsp+352], ymm11
    vpbroadcastd ymm12, DWORD PTR [rcx+48]
    vpaddd ymm12, ymm12, YMMWORD PTR [rip+chacha_counter_increment]
    vmovdqu YMMWORD PTR [rsp+384], ymm12
    vpbroadcastd ymm13, DWORD PTR [rcx+52]
/*  Usually no lane wraps the low counter word. For the last seven */
/*  starting values, propagate each lane's carry into state word 13, */
/*  matching the existing portable 64-bit counter convention. */
    cmp DWORD PTR [rcx+48], 0x0fffffff8
    jbe chacha_counter_ready
    vpcmpeqd ymm14, ymm14, ymm14
    vpsrld ymm14, ymm14, 31
    vpsrad ymm15, ymm12, 31
    vpaddd ymm15, ymm15, ymm14
    vpaddd ymm13, ymm13, ymm15
    chacha_counter_ready:
    vmovdqu YMMWORD PTR [rsp+416], ymm13
    vpbroadcastd ymm14, DWORD PTR [rcx+56]
    vmovdqu YMMWORD PTR [rsp+448], ymm14
    vpbroadcastd ymm15, DWORD PTR [rcx+60]
    vmovdqu YMMWORD PTR [rsp+480], ymm15

.rept 10
    CHACHA_DOUBLE_ROUND
.endr

/*  Add the original state, counter included. */
    vpaddd ymm0, ymm0, YMMWORD PTR [rsp]
    vpaddd ymm1, ymm1, YMMWORD PTR [rsp+32]
    vpaddd ymm2, ymm2, YMMWORD PTR [rsp+64]
    vpaddd ymm3, ymm3, YMMWORD PTR [rsp+96]
    vpaddd ymm4, ymm4, YMMWORD PTR [rsp+128]
    vpaddd ymm5, ymm5, YMMWORD PTR [rsp+160]
    vpaddd ymm6, ymm6, YMMWORD PTR [rsp+192]
    vpaddd ymm7, ymm7, YMMWORD PTR [rsp+224]
    vpaddd ymm8, ymm8, YMMWORD PTR [rsp+256]
    vpaddd ymm9, ymm9, YMMWORD PTR [rsp+288]
    vpaddd ymm10, ymm10, YMMWORD PTR [rsp+320]
    vpaddd ymm11, ymm11, YMMWORD PTR [rsp+352]
    vpaddd ymm12, ymm12, YMMWORD PTR [rsp+384]
    vpaddd ymm13, ymm13, YMMWORD PTR [rsp+416]
    vpaddd ymm14, ymm14, YMMWORD PTR [rsp+448]
    vpaddd ymm15, ymm15, YMMWORD PTR [rsp+480]

/*  Spill the final keystream words, then transpose word-major into */
/*  block-major order. */
    vmovdqu YMMWORD PTR [rsp], ymm0
    vmovdqu YMMWORD PTR [rsp+32], ymm1
    vmovdqu YMMWORD PTR [rsp+64], ymm2
    vmovdqu YMMWORD PTR [rsp+96], ymm3
    vmovdqu YMMWORD PTR [rsp+128], ymm4
    vmovdqu YMMWORD PTR [rsp+160], ymm5
    vmovdqu YMMWORD PTR [rsp+192], ymm6
    vmovdqu YMMWORD PTR [rsp+224], ymm7
    vmovdqu YMMWORD PTR [rsp+256], ymm8
    vmovdqu YMMWORD PTR [rsp+288], ymm9
    vmovdqu YMMWORD PTR [rsp+320], ymm10
    vmovdqu YMMWORD PTR [rsp+352], ymm11
    vmovdqu YMMWORD PTR [rsp+384], ymm12
    vmovdqu YMMWORD PTR [rsp+416], ymm13
    vmovdqu YMMWORD PTR [rsp+448], ymm14
    vmovdqu YMMWORD PTR [rsp+480], ymm15

    CHACHA_TRANSPOSE_GROUP 0
    CHACHA_TRANSPOSE_GROUP 1
    CHACHA_TRANSPOSE_GROUP 2
    CHACHA_TRANSPOSE_GROUP 3

/*  Emit the 8 blocks and XOR with the input. */
    CHACHA_EMIT_BLOCK 0
    CHACHA_EMIT_BLOCK 1
    CHACHA_EMIT_BLOCK 2
    CHACHA_EMIT_BLOCK 3

/*  Advance the 64-bit block counter at ctx->state[12..13]. */
    add DWORD PTR [rcx+48], 8
    adc DWORD PTR [rcx+52], 0

    add rdx, 512
    add r8, 512
    add eax, 512
    sub r9d, 512
    cmp r9d, 512
    jae chacha_loop

    chacha_done:
    vmovdqu xmm6, XMMWORD PTR [rsp+544]
    vmovdqu xmm7, XMMWORD PTR [rsp+560]
    vmovdqu xmm8, XMMWORD PTR [rsp+576]
    vmovdqu xmm9, XMMWORD PTR [rsp+592]
    vmovdqu xmm10, XMMWORD PTR [rsp+608]
    vmovdqu xmm11, XMMWORD PTR [rsp+624]
    vmovdqu xmm12, XMMWORD PTR [rsp+640]
    vmovdqu xmm13, XMMWORD PTR [rsp+656]
    vmovdqu xmm14, XMMWORD PTR [rsp+672]
    vmovdqu xmm15, XMMWORD PTR [rsp+688]
    vzeroupper
    add rsp, 704
.cfi_adjust_cfa_offset -704
    ret
.cfi_endproc
.size .Lchacha_bulk, .-.Lchacha_bulk

.section .note.GNU-stack,"",@progbits
