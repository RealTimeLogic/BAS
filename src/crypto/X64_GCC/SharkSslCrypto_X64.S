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
/* *   $Id: SharkSslCrypto_X64.asm 6069 2026-09-20 21:13:20Z gianluca $ */
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

/*  SharkSSL AES-GCM acceleration for Microsoft x64 MASM. */

/*  Full GCM blocks use AES-NI for CTR encryption and PCLMULQDQ for GHASH. */


#ifndef SHARKSSL_GCM_M0
#define SHARKSSL_GCM_M0 244
#endif

.equ SHARKSSL_AES_NR, SHARKSSL_GCM_M0 - 4

.section .rodata
.balign 16
reverse_key_words: .byte 3,2,1,0, 7,6,5,4, 11,10,9,8, 15,14,13,12
reverse_nibbles: .byte 0,8,4,12,2,10,6,14,1,9,5,13,3,11,7,15
low_nibbles: .byte 0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f
ghash_polynomial: .quad 0x087, 0

.text

.if SHARKSSL_GCM_M0 == 244
.globl sharkssl_x86_64_gcm_layout_244
.type sharkssl_x86_64_gcm_layout_244, @function
sharkssl_x86_64_gcm_layout_244:
.cfi_startproc
    ret
.cfi_endproc
.size sharkssl_x86_64_gcm_layout_244, .-sharkssl_x86_64_gcm_layout_244
.elseif SHARKSSL_GCM_M0 == 212
.globl sharkssl_x86_64_gcm_layout_212
.type sharkssl_x86_64_gcm_layout_212, @function
sharkssl_x86_64_gcm_layout_212:
.cfi_startproc
    ret
.cfi_endproc
.size sharkssl_x86_64_gcm_layout_212, .-sharkssl_x86_64_gcm_layout_212
.elseif SHARKSSL_GCM_M0 == 180
.globl sharkssl_x86_64_gcm_layout_180
.type sharkssl_x86_64_gcm_layout_180, @function
sharkssl_x86_64_gcm_layout_180:
.cfi_startproc
    ret
.cfi_endproc
.size sharkssl_x86_64_gcm_layout_180, .-sharkssl_x86_64_gcm_layout_180
.else
.error "Invalid SHARKSSL_GCM_M0"
.endif

.macro REFLECT_VALUE value, t1, t2, t3
    movdqa \t1, \value
    pand \value, XMMWORD PTR [rip+low_nibbles]
    psrlw \t1, 4
    pand \t1, XMMWORD PTR [rip+low_nibbles]
    movdqa \t2, XMMWORD PTR [rip+reverse_nibbles]
    pshufb \t2, \value
    movdqa \t3, XMMWORD PTR [rip+reverse_nibbles]
    pshufb \t3, \t1
    psllw \t2, 4
    por \t2, \t3
    movdqa \value, \t2
.endm

/*  Multiply two reflected field elements. rcx=a, rdx=b, r8=result. */
.type .Lsharkssl_gcm_mul_reflected, @function
.Lsharkssl_gcm_mul_reflected:
.cfi_startproc
    movdqu xmm0, XMMWORD PTR [rcx]
    movdqu xmm1, XMMWORD PTR [rdx]
    movdqa xmm4, xmm0
    movdqa xmm5, xmm1
    movdqa xmm2, xmm0
    pclmulqdq xmm2, xmm1, 0x00
    movdqa xmm3, xmm0
    pclmulqdq xmm3, xmm1, 0x11
    pclmulqdq xmm4, xmm1, 0x01
    pclmulqdq xmm0, xmm5, 0x10
    pxor xmm4, xmm0
    movdqa xmm5, xmm4
    pslldq xmm4, 8
    psrldq xmm5, 8
    pxor xmm2, xmm4
    pxor xmm3, xmm5

    movdqa xmm0, xmm3
    pclmulqdq xmm0, XMMWORD PTR [rip+ghash_polynomial], 0x00
    movdqa xmm1, xmm3
    pclmulqdq xmm1, XMMWORD PTR [rip+ghash_polynomial], 0x01
    movdqa xmm3, xmm1
    pslldq xmm1, 8
    psrldq xmm3, 8
    pxor xmm0, xmm1
    pclmulqdq xmm3, XMMWORD PTR [rip+ghash_polynomial], 0x00
    pxor xmm0, xmm3
    pxor xmm0, xmm2
    movdqu XMMWORD PTR [r8], xmm0
    ret
.cfi_endproc
.size .Lsharkssl_gcm_mul_reflected, .-.Lsharkssl_gcm_mul_reflected

/*  Replace the nibble table with reflected H, H^2, H^3 and H^4. This is called */
/*  once by SharkSslAesGcmCtx_constructor after the generic table is created. */
.globl sharkssl_gcm_prepare32M0
.type sharkssl_gcm_prepare32M0, @function
sharkssl_gcm_prepare32M0:
.cfi_startproc
    sub rsp, 40
.cfi_adjust_cfa_offset 40
    mov r10, rcx
    mov r11, rdx
    mov rcx, rdi
    mov rdx, rsi
    mov r8, r11
    mov r9, r10
    call .Lsharkssl_gcm_prepare32M0
    add rsp, 40
.cfi_adjust_cfa_offset -40
    ret
.cfi_endproc
.size sharkssl_gcm_prepare32M0, .-sharkssl_gcm_prepare32M0
.type .Lsharkssl_gcm_prepare32M0, @function
.Lsharkssl_gcm_prepare32M0:
.cfi_startproc
    push rbx
.cfi_adjust_cfa_offset 8
.cfi_offset rbx, -(8+8)
    sub rsp, 32
.cfi_adjust_cfa_offset 32

    mov rbx, rcx
    movdqu xmm0, XMMWORD PTR [rbx+128]
    movdqa xmm1, XMMWORD PTR [rip+reverse_key_words]
    pshufb xmm0, xmm1
    REFLECT_VALUE xmm0, xmm1, xmm2, xmm3
    movdqu XMMWORD PTR [rbx], xmm0

    mov rcx, rbx
    mov rdx, rbx
    lea r8, [rbx+16]
    call .Lsharkssl_gcm_mul_reflected
    lea rcx, [rbx+16]
    mov rdx, rbx
    lea r8, [rbx+32]
    call .Lsharkssl_gcm_mul_reflected
    lea rcx, [rbx+16]
    mov rdx, rcx
    lea r8, [rbx+48]
    call .Lsharkssl_gcm_mul_reflected

    add rsp, 32
.cfi_adjust_cfa_offset -32
    pop rbx
.cfi_adjust_cfa_offset -8
.cfi_restore rbx
    ret
.cfi_endproc
.size .Lsharkssl_gcm_prepare32M0, .-.Lsharkssl_gcm_prepare32M0

.macro CLMUL_ACCUM powerOffset
    movdqu xmm1, XMMWORD PTR [rcx+\powerOffset]
    movdqa xmm2, xmm0
    pclmulqdq xmm2, xmm1, 0x00
    movdqa xmm3, xmm0
    pclmulqdq xmm3, xmm1, 0x11
    movdqa xmm4, xmm0
    pshufd xmm5, xmm0, 0x4e
    pxor xmm4, xmm5
    movdqa xmm5, xmm1
    pshufd xmm0, xmm1, 0x4e
    pxor xmm5, xmm0
    pclmulqdq xmm4, xmm5, 0x00
    pxor xmm4, xmm2
    pxor xmm4, xmm3
    movdqa xmm5, xmm4
    pslldq xmm4, 8
    psrldq xmm5, 8
    pxor xmm2, xmm4
    pxor xmm3, xmm5
    pxor xmm2, XMMWORD PTR [r8+64]
    pxor xmm3, XMMWORD PTR [r8+80]
    movdqu XMMWORD PTR [r8+64], xmm2
    movdqu XMMWORD PTR [r8+80], xmm3
.endm

/*  Fold four ciphertext blocks into tag. rcx=H powers, rdx=tag, r8=blocks. */
/*  The caller provides two additional 16-byte slots for the product accumulator. */
.type .Lsharkssl_gcm_gmult4_pclmul, @function
.Lsharkssl_gcm_gmult4_pclmul:
.cfi_startproc
    pxor xmm0, xmm0
    movdqu XMMWORD PTR [r8+64], xmm0
    movdqu XMMWORD PTR [r8+80], xmm0

    movdqu xmm0, XMMWORD PTR [r8]
    movdqu xmm1, XMMWORD PTR [rdx]
    pxor xmm0, xmm1
    REFLECT_VALUE xmm0, xmm1, xmm2, xmm3
    CLMUL_ACCUM 48

    movdqu xmm0, XMMWORD PTR [r8+16]
    REFLECT_VALUE xmm0, xmm1, xmm2, xmm3
    CLMUL_ACCUM 32

    movdqu xmm0, XMMWORD PTR [r8+32]
    REFLECT_VALUE xmm0, xmm1, xmm2, xmm3
    CLMUL_ACCUM 16

    movdqu xmm0, XMMWORD PTR [r8+48]
    REFLECT_VALUE xmm0, xmm1, xmm2, xmm3
    CLMUL_ACCUM 0

    movdqu xmm4, XMMWORD PTR [r8+64]
    movdqu xmm5, XMMWORD PTR [r8+80]
    movdqa xmm0, xmm5
    pclmulqdq xmm0, XMMWORD PTR [rip+ghash_polynomial], 0x00
    movdqa xmm1, xmm5
    pclmulqdq xmm1, XMMWORD PTR [rip+ghash_polynomial], 0x01
    movdqa xmm3, xmm1
    pslldq xmm1, 8
    psrldq xmm3, 8
    pxor xmm0, xmm1
    pclmulqdq xmm3, XMMWORD PTR [rip+ghash_polynomial], 0x00
    pxor xmm0, xmm3
    pxor xmm0, xmm4
    REFLECT_VALUE xmm0, xmm1, xmm2, xmm3
    movdqu XMMWORD PTR [rdx], xmm0
    ret
.cfi_endproc
.size .Lsharkssl_gcm_gmult4_pclmul, .-.Lsharkssl_gcm_gmult4_pclmul

/*  Multiply x by the reflected H stored in M0[0]. */
.globl sharkssl_gcm_gmult32M0
.type sharkssl_gcm_gmult32M0, @function
sharkssl_gcm_gmult32M0:
.cfi_startproc
    sub rsp, 40
.cfi_adjust_cfa_offset 40
    mov r10, rcx
    mov r11, rdx
    mov rcx, rdi
    mov rdx, rsi
    mov r8, r11
    mov r9, r10
    call .Lsharkssl_gcm_gmult32M0
    add rsp, 40
.cfi_adjust_cfa_offset -40
    ret
.cfi_endproc
.size sharkssl_gcm_gmult32M0, .-sharkssl_gcm_gmult32M0
.type .Lsharkssl_gcm_gmult32M0, @function
.Lsharkssl_gcm_gmult32M0:
.cfi_startproc
    movdqu xmm0, XMMWORD PTR [rdx]
    REFLECT_VALUE xmm0, xmm1, xmm2, xmm3
    movdqu xmm1, XMMWORD PTR [rcx]

/*  128x128 carry-less multiplication. xmm2:xmm3 becomes the 256-bit */
/*  product, with xmm2 holding its low half. */
    movdqa xmm4, xmm0
    movdqa xmm5, xmm1
    movdqa xmm2, xmm0
    pclmulqdq xmm2, xmm1, 0x00
    movdqa xmm3, xmm0
    pclmulqdq xmm3, xmm1, 0x11
    pclmulqdq xmm4, xmm1, 0x01
    pclmulqdq xmm0, xmm5, 0x10
    pxor xmm4, xmm0
    movdqa xmm5, xmm4
    pslldq xmm4, 8
    psrldq xmm5, 8
    pxor xmm2, xmm4
    pxor xmm3, xmm5

/*  Reduce the high half modulo x^128+x^7+x^2+x+1. Multiplication by */
/*  the low polynomial 0x87 can overflow by at most seven bits; fold that */
/*  small overflow once more with the same polynomial. */
    movdqa xmm0, xmm3
    pclmulqdq xmm0, XMMWORD PTR [rip+ghash_polynomial], 0x00
    movdqa xmm1, xmm3
    pclmulqdq xmm1, XMMWORD PTR [rip+ghash_polynomial], 0x01
    movdqa xmm3, xmm1
    pslldq xmm1, 8
    psrldq xmm3, 8
    pxor xmm0, xmm1
    pclmulqdq xmm3, XMMWORD PTR [rip+ghash_polynomial], 0x00
    pxor xmm0, xmm3
    pxor xmm0, xmm2

/*  Reflect the reduced value back to the GHASH bit ordering. */
    movdqa xmm4, XMMWORD PTR [rip+low_nibbles]
    movdqa xmm5, XMMWORD PTR [rip+reverse_nibbles]
    movdqa xmm2, xmm0
    pand xmm0, xmm4
    psrlw xmm2, 4
    pand xmm2, xmm4
    movdqa xmm3, xmm5
    pshufb xmm3, xmm0
    movdqa xmm0, xmm5
    pshufb xmm0, xmm2
    psllw xmm3, 4
    por xmm0, xmm3
    movdqu XMMWORD PTR [rdx], xmm0
    ret
.cfi_endproc
.size .Lsharkssl_gcm_gmult32M0, .-.Lsharkssl_gcm_gmult32M0

/*  void sharkssl_gcm_crypt_blocks(SharkSslAesGcmCtx *ctx, U8 counter[16], */
/*                                 U8 tag[16], const U8 *input, U8 *output, */
/*                                 U32 len, SharkSslAesCtx_Type type); */

/*  Windows x64 arguments: */
/*    rcx=ctx, rdx=counter, r8=tag, r9=input */
/*    [rsp+40]=output, [rsp+48]=len, [rsp+56]=type */

/*  len must be a non-zero multiple of 16, as guaranteed by SharkSslCrypto.c. */
.globl sharkssl_gcm_crypt_blocks
.type sharkssl_gcm_crypt_blocks, @function
sharkssl_gcm_crypt_blocks:
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
    call .Lsharkssl_gcm_crypt_blocks
    add rsp, 56
.cfi_adjust_cfa_offset -56
    ret
.cfi_endproc
.size sharkssl_gcm_crypt_blocks, .-sharkssl_gcm_crypt_blocks
.type .Lsharkssl_gcm_crypt_blocks, @function
.Lsharkssl_gcm_crypt_blocks:
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
    sub rsp, 128
.cfi_adjust_cfa_offset 128

    mov r12, rcx
    mov r13, rdx
    mov r14, r8
    mov r15, r9
    mov rdi, QWORD PTR [rsp+224]
    mov esi, DWORD PTR [rsp+232]
    mov ebx, DWORD PTR [rsp+240]

    cmp esi, 64
    jb gcm_block_loop

    gcm_four_block_loop:
/*  Materialize four consecutive big-endian counters. */
    mov eax, DWORD PTR [r13+12]
    bswap eax
    inc eax
    mov edx, eax
    bswap edx
    mov DWORD PTR [r13+12], edx
    movdqu xmm0, XMMWORD PTR [r13]
    inc eax
    mov edx, eax
    bswap edx
    mov DWORD PTR [r13+12], edx
    movdqu xmm1, XMMWORD PTR [r13]
    inc eax
    mov edx, eax
    bswap edx
    mov DWORD PTR [r13+12], edx
    movdqu xmm2, XMMWORD PTR [r13]
    inc eax
    mov edx, eax
    bswap edx
    mov DWORD PTR [r13+12], edx
    movdqu xmm3, XMMWORD PTR [r13]

    movdqa xmm5, XMMWORD PTR [rip+reverse_key_words]
    movdqu xmm4, XMMWORD PTR [r12]
    pshufb xmm4, xmm5
    pxor xmm0, xmm4
    pxor xmm1, xmm4
    pxor xmm2, xmm4
    pxor xmm3, xmm4

    lea r10, [r12+16]
    movzx eax, WORD PTR [r12+SHARKSSL_AES_NR]
    cmp eax, 7
    ja aes_four_round_count_ready
    lea eax, [rax+rax+1]

    aes_four_round_count_ready:
    movdqu xmm4, XMMWORD PTR [r10]
    pshufb xmm4, xmm5
    aesenc xmm0, xmm4
    aesenc xmm1, xmm4
    aesenc xmm2, xmm4
    aesenc xmm3, xmm4
    add r10, 16
    dec eax
    jnz aes_four_round_count_ready

    movdqu xmm4, XMMWORD PTR [r10]
    pshufb xmm4, xmm5
    aesenclast xmm0, xmm4
    aesenclast xmm1, xmm4
    aesenclast xmm2, xmm4
    aesenclast xmm3, xmm4

    movdqu xmm4, XMMWORD PTR [r15]
    movdqu XMMWORD PTR [rsp+32], xmm4
    pxor xmm0, xmm4
    movdqu XMMWORD PTR [rdi], xmm0
    movdqu xmm4, XMMWORD PTR [r15+16]
    movdqu XMMWORD PTR [rsp+48], xmm4
    pxor xmm1, xmm4
    movdqu XMMWORD PTR [rdi+16], xmm1
    movdqu xmm4, XMMWORD PTR [r15+32]
    movdqu XMMWORD PTR [rsp+64], xmm4
    pxor xmm2, xmm4
    movdqu XMMWORD PTR [rdi+32], xmm2
    movdqu xmm4, XMMWORD PTR [r15+48]
    movdqu XMMWORD PTR [rsp+80], xmm4
    pxor xmm3, xmm4
    movdqu XMMWORD PTR [rdi+48], xmm3

    cmp ebx, 1
    jne gcm_four_auth_ready
    movdqu XMMWORD PTR [rsp+32], xmm0
    movdqu XMMWORD PTR [rsp+48], xmm1
    movdqu XMMWORD PTR [rsp+64], xmm2
    movdqu XMMWORD PTR [rsp+80], xmm3

    gcm_four_auth_ready:
    lea rcx, [r12+SHARKSSL_GCM_M0]
    mov rdx, r14
    lea r8, [rsp+32]
    call .Lsharkssl_gcm_gmult4_pclmul

    add r15, 64
    add rdi, 64
    sub esi, 64
    cmp esi, 64
    jae gcm_four_block_loop
    test esi, esi
    jz gcm_blocks_done

    gcm_block_loop:
/*  Increment the low 32 bits of the big-endian GCM counter. */
    mov eax, DWORD PTR [r13+12]
    bswap eax
    inc eax
    bswap eax
    mov DWORD PTR [r13+12], eax

/*  SharkSSL stores each expanded-key word in host byte order. Reverse the */
/*  bytes in every 32-bit word before feeding round keys to AES-NI. */
    movdqu xmm0, XMMWORD PTR [r13]
    movdqa xmm2, XMMWORD PTR [rip+reverse_key_words]
    movdqu xmm1, XMMWORD PTR [r12]
    pshufb xmm1, xmm2
    pxor xmm0, xmm1

    lea r10, [r12+16]
    movzx eax, WORD PTR [r12+SHARKSSL_AES_NR]

/*  With SHARKSSL_AES_CIPHER_LOOP_UNROLL, nr is stored in compressed form. */
    cmp eax, 7
    ja aes_round_count_ready
    lea eax, [rax+rax+1]

    aes_round_count_ready:
    movdqu xmm1, XMMWORD PTR [r10]
    pshufb xmm1, xmm2
    aesenc xmm0, xmm1
    add r10, 16
    dec eax
    jnz aes_round_count_ready

    movdqu xmm1, XMMWORD PTR [r10]
    pshufb xmm1, xmm2
    aesenclast xmm0, xmm1

    movdqu xmm3, XMMWORD PTR [r15]
    pxor xmm0, xmm3
    movdqu XMMWORD PTR [rdi], xmm0

    movdqu xmm1, XMMWORD PTR [r14]
    cmp ebx, 1
    jne gcm_decrypt_auth
    pxor xmm1, xmm0
    jmp gcm_authenticate

    gcm_decrypt_auth:
    pxor xmm1, xmm3

    gcm_authenticate:
    movdqu XMMWORD PTR [r14], xmm1
    lea rcx, [r12+SHARKSSL_GCM_M0]
    mov rdx, r14
    call .Lsharkssl_gcm_gmult32M0

    add r15, 16
    add rdi, 16
    sub esi, 16
    jnz gcm_block_loop

    gcm_blocks_done:
    add rsp, 128
.cfi_adjust_cfa_offset -128
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
.size .Lsharkssl_gcm_crypt_blocks, .-.Lsharkssl_gcm_crypt_blocks

.section .note.GNU-stack,"",@progbits
