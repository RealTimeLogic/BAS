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
 *   $Id: SharkSslX25519_X64.S 6730 2026-11-03 20:10:54Z gianluca $
 *
 *   COPYRIGHT:  Real Time Logic LLC, 2010 - 2026
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

/* Linux x86-64 ELF/GAS port of the corresponding X64_MSVC file.
 * The public SysV entries adapt arguments to the private kernel schedule.
 * No compiler ABI attributes are needed in common C. */
.intel_syntax noprefix

/* X25519 using BMI2/ADX integer instructions. */
/* Independently implemented from field arithmetic and SharkSSL ladder equations. */
/* No third-party source is incorporated. */

/* Field representation: four little-endian 64-bit limbs, redundant modulo */
/* 2p = 2^256 - 38. Each operation accepts and returns any 256-bit residue. */
/* Products use separate CF/OF chains, with full 512-bit intermediates. */
/* Fold the upper half by 38; fold the final overflow by 38 once more. */
/* Canonical reduction modulo p occurs only at byte input/output boundaries. */
/* Squaring accumulates six cross-products, doubles them, then adds four squares. */

/* The ladder processes all 256 bits without clamping, matching the SharkSSL */
/* internal API. Key-generation clamping and zero-secret rejection stay in C. */
/* Branches and memory addresses depend on public fixed counters, not secrets. */
/* The raw entry point requires BMI2+ADX; callers must test availability first. */
/* Existing C and Cortex-M4 implementations remain separate. */

.text

/* Integer instructions only: BMI2 and ADX do not require AVX OS state. */
.globl sharkssl_x25519_adx_available
.type sharkssl_x25519_adx_available, @function
.p2align 4
sharkssl_x25519_adx_available:
.cfi_startproc
    push rbx
.cfi_adjust_cfa_offset 8
.cfi_offset rbx, -16
    xor eax, eax
    cpuid
    cmp eax, 7
    jb .Lx25519_no_adx
    mov eax, 7
    xor ecx, ecx
    cpuid
    and ebx, 0x80100
    cmp ebx, 0x80100
    sete al
    movzx eax, al
    pop rbx
.cfi_adjust_cfa_offset -8
.cfi_restore rbx
    ret
.cfi_adjust_cfa_offset 8
.cfi_offset rbx, -16
.Lx25519_no_adx:
    xor eax, eax
    pop rbx
.cfi_adjust_cfa_offset -8
.cfi_restore rbx
    ret
.cfi_endproc
.size sharkssl_x25519_adx_available, .-sharkssl_x25519_adx_available
/* Regular row-wise multiply: low halves carry through CF, high halves through OF. */
.macro FIELD_MUL
    mov rdi, rcx
    mov rsi, rdx
    mov rbx, r8
    /* Seed the first product row directly. Only CF is needed for this row. */
    mov rdx, QWORD PTR [rsi]
    xor ebp, ebp
    mulx r9, r8, QWORD PTR [rbx]
    mulx r10, rax, QWORD PTR [rbx+8]
    adcx r9, rax
    mulx r11, rax, QWORD PTR [rbx+16]
    adcx r10, rax
    mulx r12, rax, QWORD PTR [rbx+24]
    adcx r11, rax
    adcx r12, rbp
    xor r13d, r13d
    mov rdx, QWORD PTR [rsi+8]
    xor ebp, ebp
    mulx rax, rcx, QWORD PTR [rbx+0]
    adcx r9, rcx
    adox r10, rax
    mulx rax, rcx, QWORD PTR [rbx+8]
    adcx r10, rcx
    adox r11, rax
    mulx rax, rcx, QWORD PTR [rbx+16]
    adcx r11, rcx
    adox r12, rax
    mulx rax, rcx, QWORD PTR [rbx+24]
    adcx r12, rcx
    adox r13, rax
    adcx r13, rbp
    xor r14d, r14d
    mov rdx, QWORD PTR [rsi+16]
    xor ebp, ebp
    mulx rax, rcx, QWORD PTR [rbx+0]
    adcx r10, rcx
    adox r11, rax
    mulx rax, rcx, QWORD PTR [rbx+8]
    adcx r11, rcx
    adox r12, rax
    mulx rax, rcx, QWORD PTR [rbx+16]
    adcx r12, rcx
    adox r13, rax
    mulx rax, rcx, QWORD PTR [rbx+24]
    adcx r13, rcx
    adox r14, rax
    adcx r14, rbp
    xor r15d, r15d
    mov rdx, QWORD PTR [rsi+24]
    xor ebp, ebp
    mulx rax, rcx, QWORD PTR [rbx+0]
    adcx r11, rcx
    adox r12, rax
    mulx rax, rcx, QWORD PTR [rbx+8]
    adcx r12, rcx
    adox r13, rax
    mulx rax, rcx, QWORD PTR [rbx+16]
    adcx r13, rcx
    adox r14, rax
    mulx rax, rcx, QWORD PTR [rbx+24]
    adcx r14, rcx
    adox r15, rax
    adcx r15, rbp
    mov edx, 38
    xor ebp, ebp
    mulx rax, rcx, r12
    xor r12d, r12d
    adcx r8, rcx
    adox r9, rax
    mulx rax, rcx, r13
    adcx r9, rcx
    adox r10, rax
    mulx rax, rcx, r14
    adcx r10, rcx
    adox r11, rax
    mulx rax, rcx, r15
    adcx r11, rcx
    adox r12, rax
    adcx r12, rbp
    mov rbx, r12
    imul rbx, rbx, 38
    add r8, rbx
    adc r9, 0
    adc r10, 0
    adc r11, 0
    sbb rax, rax
    and eax, 38
    add r8, rax
    adc r9, 0
    adc r10, 0
    adc r11, 0
    mov QWORD PTR [rdi+0], r8
    mov QWORD PTR [rdi+8], r9
    mov QWORD PTR [rdi+16], r10
    mov QWORD PTR [rdi+24], r11
.endm
/* Six off-diagonal terms, doubled, plus the four diagonal squares. */
.macro FIELD_SQ
    mov rdi, rcx
    mov rsi, rdx
    /* Cross-products start in word 1; seed their first row without zeros. */
    mov rdx, QWORD PTR [rsi]
    xor ebp, ebp
    mulx r10, r9, QWORD PTR [rsi+8]
    mulx r11, rax, QWORD PTR [rsi+16]
    adcx r10, rax
    mulx r12, rax, QWORD PTR [rsi+24]
    adcx r11, rax
    adcx r12, rbp
    xor r13d, r13d
    mov rdx, QWORD PTR [rsi+8]
    mulx rax, rcx, QWORD PTR [rsi+16]
    adcx r11, rcx
    adox r12, rax
    mulx rax, rcx, QWORD PTR [rsi+24]
    adcx r12, rcx
    adox r13, rax
    adcx r13, rbp
    mov rdx, QWORD PTR [rsi+16]
    mulx r14, rax, QWORD PTR [rsi+24]
    add r13, rax
    adc r14, 0
    /* CF doubles cross-products while OF adds the four diagonal squares. */
    xor r15d, r15d
    mov rdx, QWORD PTR [rsi]
    mulx rax, r8, rdx
    adcx r9, r9
    adox r9, rax
    mov rdx, QWORD PTR [rsi+8]
    mulx rax, rcx, rdx
    adcx r10, r10
    adox r10, rcx
    adcx r11, r11
    adox r11, rax
    mov rdx, QWORD PTR [rsi+16]
    mulx rax, rcx, rdx
    adcx r12, r12
    adox r12, rcx
    adcx r13, r13
    adox r13, rax
    mov rdx, QWORD PTR [rsi+24]
    mulx rax, rcx, rdx
    adcx r14, r14
    adox r14, rcx
    adcx r15, r15
    adox r15, rax
    mov edx, 38
    xor ebp, ebp
    mulx rax, rcx, r12
    xor r12d, r12d
    adcx r8, rcx
    adox r9, rax
    mulx rax, rcx, r13
    adcx r9, rcx
    adox r10, rax
    mulx rax, rcx, r14
    adcx r10, rcx
    adox r11, rax
    mulx rax, rcx, r15
    adcx r11, rcx
    adox r12, rax
    adcx r12, rbp
    mov rbx, r12
    imul rbx, rbx, 38
    add r8, rbx
    adc r9, 0
    adc r10, 0
    adc r11, 0
    sbb rax, rax
    and eax, 38
    add r8, rax
    adc r9, 0
    adc r10, 0
    adc r11, 0
    mov QWORD PTR [rdi+0], r8
    mov QWORD PTR [rdi+8], r9
    mov QWORD PTR [rdi+16], r10
    mov QWORD PTR [rdi+24], r11
.endm
.macro FIELD_SCALE
    mov rdi, rcx
    mov rsi, rdx
    mov edx, 121666
    mulx r9, r8, QWORD PTR [rsi]
    mulx r10, rax, QWORD PTR [rsi+8]
    add r9, rax
    mulx r11, rax, QWORD PTR [rsi+16]
    adc r10, rax
    mulx rbx, rax, QWORD PTR [rsi+24]
    adc r11, rax
    adc rbx, 0
    imul rbx, rbx, 38
    add r8, rbx
    adc r9, 0
    adc r10, 0
    adc r11, 0
    sbb rax, rax
    and eax, 38
    add r8, rax
    adc r9, 0
    adc r10, 0
    adc r11, 0
    mov QWORD PTR [rdi], r8
    mov QWORD PTR [rdi+8], r9
    mov QWORD PTR [rdi+16], r10
    mov QWORD PTR [rdi+24], r11
.endm
/* Modulo 2p: replace a carry out of bit 255 by +38, twice for the boundary. */
.macro FIELD_ADD
    mov rax, r8
    mov r9, QWORD PTR [rdx+0]
    mov r10, QWORD PTR [rdx+8]
    mov r11, QWORD PTR [rdx+16]
    mov r8, QWORD PTR [rdx+24]
    add r9, QWORD PTR [rax+0]
    adc r10, QWORD PTR [rax+8]
    adc r11, QWORD PTR [rax+16]
    adc r8, QWORD PTR [rax+24]
    sbb rax, rax
    and eax, 38
    add r9, rax
    adc r10, 0
    adc r11, 0
    adc r8, 0
    sbb rax, rax
    and eax, 38
    add r9, rax
    adc r10, 0
    adc r11, 0
    adc r8, 0
    mov QWORD PTR [rcx+0], r9
    mov QWORD PTR [rcx+8], r10
    mov QWORD PTR [rcx+16], r11
    mov QWORD PTR [rcx+24], r8
.endm
/* Modulo 2p: correct a 256-bit subtraction borrow by -38, twice if needed. */
.macro FIELD_SUB
    mov rax, r8
    mov r9, QWORD PTR [rdx+0]
    mov r10, QWORD PTR [rdx+8]
    mov r11, QWORD PTR [rdx+16]
    mov r8, QWORD PTR [rdx+24]
    sub r9, QWORD PTR [rax+0]
    sbb r10, QWORD PTR [rax+8]
    sbb r11, QWORD PTR [rax+16]
    sbb r8, QWORD PTR [rax+24]
    sbb rax, rax
    and eax, 38
    sub r9, rax
    sbb r10, 0
    sbb r11, 0
    sbb r8, 0
    sbb rax, rax
    and eax, 38
    sub r9, rax
    sbb r10, 0
    sbb r11, 0
    sbb r8, 0
    mov QWORD PTR [rcx+0], r9
    mov QWORD PTR [rcx+8], r10
    mov QWORD PTR [rcx+16], r11
    mov QWORD PTR [rcx+24], r8
.endm

#ifdef SHARKSSL_X25519_TEST
/* Test entries: SysV (out, a, b); rdi and rsi are scratch in this ABI. */
.globl sharkssl_x25519_mul4_adx
.type sharkssl_x25519_mul4_adx, @function
.p2align 4
sharkssl_x25519_mul4_adx:
.cfi_startproc
    push rbx
.cfi_adjust_cfa_offset 8
.cfi_offset rbx, -(8+8)
    push rbp
.cfi_adjust_cfa_offset 8
.cfi_offset rbp, -(16+8)
    push r12
.cfi_adjust_cfa_offset 8
.cfi_offset r12, -(24+8)
    push r13
.cfi_adjust_cfa_offset 8
.cfi_offset r13, -(32+8)
    push r14
.cfi_adjust_cfa_offset 8
.cfi_offset r14, -(40+8)
    push r15
.cfi_adjust_cfa_offset 8
.cfi_offset r15, -(48+8)
    mov rcx, rdi
    mov r8, rdx
    mov rdx, rsi
    sub rsp, 8
.cfi_adjust_cfa_offset 8
    FIELD_MUL
    add rsp, 8
.cfi_adjust_cfa_offset -8
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
    pop rbp
.cfi_adjust_cfa_offset -8
.cfi_restore rbp
    pop rbx
.cfi_adjust_cfa_offset -8
.cfi_restore rbx
    ret
.cfi_endproc
.size sharkssl_x25519_mul4_adx, .-sharkssl_x25519_mul4_adx
.globl sharkssl_x25519_sq4_adx
.type sharkssl_x25519_sq4_adx, @function
.p2align 4
sharkssl_x25519_sq4_adx:
.cfi_startproc
    push rbx
.cfi_adjust_cfa_offset 8
.cfi_offset rbx, -(8+8)
    push rbp
.cfi_adjust_cfa_offset 8
.cfi_offset rbp, -(16+8)
    push r12
.cfi_adjust_cfa_offset 8
.cfi_offset r12, -(24+8)
    push r13
.cfi_adjust_cfa_offset 8
.cfi_offset r13, -(32+8)
    push r14
.cfi_adjust_cfa_offset 8
.cfi_offset r14, -(40+8)
    push r15
.cfi_adjust_cfa_offset 8
.cfi_offset r15, -(48+8)
    mov rcx, rdi
    mov r8, rdx
    mov rdx, rsi
    sub rsp, 8
.cfi_adjust_cfa_offset 8
    FIELD_SQ
    add rsp, 8
.cfi_adjust_cfa_offset -8
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
    pop rbp
.cfi_adjust_cfa_offset -8
.cfi_restore rbp
    pop rbx
.cfi_adjust_cfa_offset -8
.cfi_restore rbx
    ret
.cfi_endproc
.size sharkssl_x25519_sq4_adx, .-sharkssl_x25519_sq4_adx
.globl sharkssl_x25519_scale4_adx
.type sharkssl_x25519_scale4_adx, @function
.p2align 4
sharkssl_x25519_scale4_adx:
.cfi_startproc
    push rbx
.cfi_adjust_cfa_offset 8
.cfi_offset rbx, -(8+8)
    mov rcx, rdi
    mov r8, rdx
    mov rdx, rsi
    FIELD_SCALE
    pop rbx
.cfi_adjust_cfa_offset -8
.cfi_restore rbx
    ret
.cfi_endproc
.size sharkssl_x25519_scale4_adx, .-sharkssl_x25519_scale4_adx
.globl sharkssl_x25519_add4_adx
.type sharkssl_x25519_add4_adx, @function
.p2align 4
sharkssl_x25519_add4_adx:
.cfi_startproc
    mov rcx, rdi
    mov r8, rdx
    mov rdx, rsi
    FIELD_ADD
    ret
.cfi_endproc
.size sharkssl_x25519_add4_adx, .-sharkssl_x25519_add4_adx
.globl sharkssl_x25519_sub4_adx
.type sharkssl_x25519_sub4_adx, @function
.p2align 4
sharkssl_x25519_sub4_adx:
.cfi_startproc
    mov rcx, rdi
    mov r8, rdx
    mov rdx, rsi
    FIELD_SUB
    ret
.cfi_endproc
.size sharkssl_x25519_sub4_adx, .-sharkssl_x25519_sub4_adx
#endif

/* Fixed inversion chain from SharkSSL C: 254 squarings and 11 multiplies. */
/* void sharkssl_x25519_inv4_adx(U64 out[4], const U64 in[4]) */
.globl sharkssl_x25519_inv4_adx
.type sharkssl_x25519_inv4_adx, @function
.p2align 4
sharkssl_x25519_inv4_adx:
.cfi_startproc
    sub rsp, 40
.cfi_adjust_cfa_offset 40
    mov rcx, rdi
    mov rdx, rsi
    call .Lsharkssl_x25519_inv4_adx
    add rsp, 40
.cfi_adjust_cfa_offset -40
    ret
.cfi_endproc
.size sharkssl_x25519_inv4_adx, .-sharkssl_x25519_inv4_adx
.type .Lsharkssl_x25519_inv4_adx, @function
.p2align 4
.Lsharkssl_x25519_inv4_adx:
.cfi_startproc
    push rdi
.cfi_adjust_cfa_offset 8
.cfi_offset rdi, -(8+8)
    push rsi
.cfi_adjust_cfa_offset 8
.cfi_offset rsi, -(16+8)
    push rbx
.cfi_adjust_cfa_offset 8
.cfi_offset rbx, -(24+8)
    push rbp
.cfi_adjust_cfa_offset 8
.cfi_offset rbp, -(32+8)
    push r12
.cfi_adjust_cfa_offset 8
.cfi_offset r12, -(40+8)
    push r13
.cfi_adjust_cfa_offset 8
.cfi_offset r13, -(48+8)
    push r14
.cfi_adjust_cfa_offset 8
.cfi_offset r14, -(56+8)
    push r15
.cfi_adjust_cfa_offset 8
.cfi_offset r15, -(64+8)
    sub rsp, 168
.cfi_adjust_cfa_offset 168
    mov QWORD PTR [rsp], rcx
    mov QWORD PTR [rsp+8], rdx
    lea rcx, [rsp+32]
    mov rdx, QWORD PTR [rsp+8]
    FIELD_SQ
    lea rcx, [rsp+64]
    lea rdx, [rsp+32]
    FIELD_SQ
    lea rcx, [rsp+64]
    lea rdx, [rsp+64]
    FIELD_SQ
    lea rcx, [rsp+64]
    mov rdx, QWORD PTR [rsp+8]
    lea r8, [rsp+64]
    FIELD_MUL
    lea rcx, [rsp+32]
    lea rdx, [rsp+32]
    lea r8, [rsp+64]
    FIELD_MUL
    lea rcx, [rsp+96]
    lea rdx, [rsp+32]
    FIELD_SQ
    lea rcx, [rsp+64]
    lea rdx, [rsp+64]
    lea r8, [rsp+96]
    FIELD_MUL
    movdqu xmm0, XMMWORD PTR [rsp+64]
    movdqu xmm1, XMMWORD PTR [rsp+80]
    movdqu XMMWORD PTR [rsp+96], xmm0
    movdqu XMMWORD PTR [rsp+112], xmm1
    mov DWORD PTR [rsp+16], 5
.Linv_loop_0:
    lea rcx, [rsp+96]
    lea rdx, [rsp+96]
    FIELD_SQ
    dec DWORD PTR [rsp+16]
    jnz .Linv_loop_0
    lea rcx, [rsp+64]
    lea rdx, [rsp+96]
    lea r8, [rsp+64]
    FIELD_MUL
    movdqu xmm0, XMMWORD PTR [rsp+64]
    movdqu xmm1, XMMWORD PTR [rsp+80]
    movdqu XMMWORD PTR [rsp+96], xmm0
    movdqu XMMWORD PTR [rsp+112], xmm1
    mov DWORD PTR [rsp+16], 10
.Linv_loop_1:
    lea rcx, [rsp+96]
    lea rdx, [rsp+96]
    FIELD_SQ
    dec DWORD PTR [rsp+16]
    jnz .Linv_loop_1
    lea rcx, [rsp+96]
    lea rdx, [rsp+96]
    lea r8, [rsp+64]
    FIELD_MUL
    movdqu xmm0, XMMWORD PTR [rsp+96]
    movdqu xmm1, XMMWORD PTR [rsp+112]
    movdqu XMMWORD PTR [rsp+128], xmm0
    movdqu XMMWORD PTR [rsp+144], xmm1
    mov DWORD PTR [rsp+16], 20
.Linv_loop_2:
    lea rcx, [rsp+128]
    lea rdx, [rsp+128]
    FIELD_SQ
    dec DWORD PTR [rsp+16]
    jnz .Linv_loop_2
    lea rcx, [rsp+96]
    lea rdx, [rsp+128]
    lea r8, [rsp+96]
    FIELD_MUL
    mov DWORD PTR [rsp+16], 10
.Linv_loop_3:
    lea rcx, [rsp+96]
    lea rdx, [rsp+96]
    FIELD_SQ
    dec DWORD PTR [rsp+16]
    jnz .Linv_loop_3
    lea rcx, [rsp+64]
    lea rdx, [rsp+96]
    lea r8, [rsp+64]
    FIELD_MUL
    movdqu xmm0, XMMWORD PTR [rsp+64]
    movdqu xmm1, XMMWORD PTR [rsp+80]
    movdqu XMMWORD PTR [rsp+96], xmm0
    movdqu XMMWORD PTR [rsp+112], xmm1
    mov DWORD PTR [rsp+16], 50
.Linv_loop_4:
    lea rcx, [rsp+96]
    lea rdx, [rsp+96]
    FIELD_SQ
    dec DWORD PTR [rsp+16]
    jnz .Linv_loop_4
    lea rcx, [rsp+96]
    lea rdx, [rsp+96]
    lea r8, [rsp+64]
    FIELD_MUL
    movdqu xmm0, XMMWORD PTR [rsp+96]
    movdqu xmm1, XMMWORD PTR [rsp+112]
    movdqu XMMWORD PTR [rsp+128], xmm0
    movdqu XMMWORD PTR [rsp+144], xmm1
    mov DWORD PTR [rsp+16], 100
.Linv_loop_5:
    lea rcx, [rsp+128]
    lea rdx, [rsp+128]
    FIELD_SQ
    dec DWORD PTR [rsp+16]
    jnz .Linv_loop_5
    lea rcx, [rsp+96]
    lea rdx, [rsp+128]
    lea r8, [rsp+96]
    FIELD_MUL
    mov DWORD PTR [rsp+16], 50
.Linv_loop_6:
    lea rcx, [rsp+96]
    lea rdx, [rsp+96]
    FIELD_SQ
    dec DWORD PTR [rsp+16]
    jnz .Linv_loop_6
    lea rcx, [rsp+64]
    lea rdx, [rsp+96]
    lea r8, [rsp+64]
    FIELD_MUL
    mov DWORD PTR [rsp+16], 5
.Linv_loop_7:
    lea rcx, [rsp+64]
    lea rdx, [rsp+64]
    FIELD_SQ
    dec DWORD PTR [rsp+16]
    jnz .Linv_loop_7
    mov rcx, QWORD PTR [rsp]
    lea rdx, [rsp+64]
    lea r8, [rsp+32]
    FIELD_MUL
    /* Erase the fixed scratch frame, including field intermediates. */
    xor eax, eax
    mov ecx, 21
    mov rdi, rsp
    rep stosq
    pxor xmm0, xmm0
    pxor xmm1, xmm1
    xor edx, edx
    xor r8d, r8d
    xor r9d, r9d
    xor r10d, r10d
    xor r11d, r11d
    add rsp, 168
.cfi_adjust_cfa_offset -168
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
    pop rbp
.cfi_adjust_cfa_offset -8
.cfi_restore rbp
    pop rbx
.cfi_adjust_cfa_offset -8
.cfi_restore rbx
    pop rsi
.cfi_adjust_cfa_offset -8
.cfi_restore rsi
    pop rdi
.cfi_adjust_cfa_offset -8
.cfi_restore rdi
    ret
.cfi_endproc
.size .Lsharkssl_x25519_inv4_adx, .-.Lsharkssl_x25519_inv4_adx

/* void sharkssl_x25519_adx(U8 out[32], const U8 scalar[32], const U8 u[32]) */
/* Scalar bytes are little-endian and unclamped. */
.globl sharkssl_x25519_adx
.type sharkssl_x25519_adx, @function
.p2align 4
sharkssl_x25519_adx:
.cfi_startproc
    sub rsp, 40
.cfi_adjust_cfa_offset 40
    mov rcx, rdi
    mov r8, rdx
    mov rdx, rsi
    call .Lsharkssl_x25519_adx
    add rsp, 40
.cfi_adjust_cfa_offset -40
    ret
.cfi_endproc
.size sharkssl_x25519_adx, .-sharkssl_x25519_adx
.type .Lsharkssl_x25519_adx, @function
.p2align 4
.Lsharkssl_x25519_adx:
.cfi_startproc
    push rdi
.cfi_adjust_cfa_offset 8
.cfi_offset rdi, -(8+8)
    push rsi
.cfi_adjust_cfa_offset 8
.cfi_offset rsi, -(16+8)
    push rbx
.cfi_adjust_cfa_offset 8
.cfi_offset rbx, -(24+8)
    push rbp
.cfi_adjust_cfa_offset 8
.cfi_offset rbp, -(32+8)
    push r12
.cfi_adjust_cfa_offset 8
.cfi_offset r12, -(40+8)
    push r13
.cfi_adjust_cfa_offset 8
.cfi_offset r13, -(48+8)
    push r14
.cfi_adjust_cfa_offset 8
.cfi_offset r14, -(56+8)
    push r15
.cfi_adjust_cfa_offset 8
.cfi_offset r15, -(64+8)
    sub rsp, 296
.cfi_adjust_cfa_offset 296
    mov QWORD PTR [rsp+256], rcx
    mov QWORD PTR [rsp+264], rdx
    mov rdx, r8
    pxor xmm0, xmm0
    movdqu XMMWORD PTR [rsp+32], xmm0
    movdqu XMMWORD PTR [rsp+48], xmm0
    movdqu XMMWORD PTR [rsp+64], xmm0
    movdqu XMMWORD PTR [rsp+80], xmm0
    movdqu XMMWORD PTR [rsp+96], xmm0
    movdqu XMMWORD PTR [rsp+112], xmm0
    movdqu XMMWORD PTR [rsp+128], xmm0
    movdqu XMMWORD PTR [rsp+144], xmm0
    movdqu XMMWORD PTR [rsp+160], xmm0
    movdqu XMMWORD PTR [rsp+176], xmm0
    movdqu XMMWORD PTR [rsp+192], xmm0
    movdqu XMMWORD PTR [rsp+208], xmm0
    movdqu XMMWORD PTR [rsp+224], xmm0
    movdqu XMMWORD PTR [rsp+240], xmm0
    mov QWORD PTR [rsp+64], 1
    mov QWORD PTR [rsp+160], 1
    mov r9, QWORD PTR [rdx+0]
    mov r10, QWORD PTR [rdx+8]
    mov r11, QWORD PTR [rdx+16]
    mov r8, QWORD PTR [rdx+24]
    btr r8, 63
    lea rcx, [rsp+32]
    mov rax, r8
    shr rax, 63
    btr r8, 63
    imul rax, rax, 19
    add r9, rax
    adc r10, 0
    adc r11, 0
    adc r8, 0
    mov rax, r8
    shr rax, 63
    btr r8, 63
    imul rax, rax, 19
    add r9, rax
    adc r10, 0
    adc r11, 0
    adc r8, 0
    mov QWORD PTR [rcx+0], r9
    mov QWORD PTR [rcx+8], r10
    mov QWORD PTR [rcx+16], r11
    mov QWORD PTR [rcx+24], r8
    add r9, 19
    adc r10, 0
    adc r11, 0
    adc r8, 0
    btr r8, 63
    cmovnc r9, QWORD PTR [rcx+0]
    cmovnc r10, QWORD PTR [rcx+8]
    cmovnc r11, QWORD PTR [rcx+16]
    cmovnc r8, QWORD PTR [rcx+24]
    mov QWORD PTR [rcx+0], r9
    mov QWORD PTR [rcx+8], r10
    mov QWORD PTR [rcx+16], r11
    mov QWORD PTR [rcx+24], r8
    mov rax, QWORD PTR [rsp+32]
    mov QWORD PTR [rsp+128], rax
    mov rax, QWORD PTR [rsp+40]
    mov QWORD PTR [rsp+136], rax
    mov rax, QWORD PTR [rsp+48]
    mov QWORD PTR [rsp+144], rax
    mov rax, QWORD PTR [rsp+56]
    mov QWORD PTR [rsp+152], rax
    mov DWORD PTR [rsp+272], 255
    mov DWORD PTR [rsp+280], 0
.Lladder_loop:
    mov eax, DWORD PTR [rsp+272]
    shr eax, 3
    mov rdx, QWORD PTR [rsp+264]
    movzx eax, BYTE PTR [rdx+rax]
    mov ecx, DWORD PTR [rsp+272]
    and ecx, 7
    shr eax, cl
    and eax, 1
    mov edx, eax
    xor eax, DWORD PTR [rsp+280]
    mov DWORD PTR [rsp+280], edx
    neg rax
    mov rdx, QWORD PTR [rsp+64]
    xor rdx, QWORD PTR [rsp+128]
    and rdx, rax
    xor QWORD PTR [rsp+64], rdx
    xor QWORD PTR [rsp+128], rdx
    mov rdx, QWORD PTR [rsp+72]
    xor rdx, QWORD PTR [rsp+136]
    and rdx, rax
    xor QWORD PTR [rsp+72], rdx
    xor QWORD PTR [rsp+136], rdx
    mov rdx, QWORD PTR [rsp+80]
    xor rdx, QWORD PTR [rsp+144]
    and rdx, rax
    xor QWORD PTR [rsp+80], rdx
    xor QWORD PTR [rsp+144], rdx
    mov rdx, QWORD PTR [rsp+88]
    xor rdx, QWORD PTR [rsp+152]
    and rdx, rax
    xor QWORD PTR [rsp+88], rdx
    xor QWORD PTR [rsp+152], rdx
    mov rdx, QWORD PTR [rsp+96]
    xor rdx, QWORD PTR [rsp+160]
    and rdx, rax
    xor QWORD PTR [rsp+96], rdx
    xor QWORD PTR [rsp+160], rdx
    mov rdx, QWORD PTR [rsp+104]
    xor rdx, QWORD PTR [rsp+168]
    and rdx, rax
    xor QWORD PTR [rsp+104], rdx
    xor QWORD PTR [rsp+168], rdx
    mov rdx, QWORD PTR [rsp+112]
    xor rdx, QWORD PTR [rsp+176]
    and rdx, rax
    xor QWORD PTR [rsp+112], rdx
    xor QWORD PTR [rsp+176], rdx
    mov rdx, QWORD PTR [rsp+120]
    xor rdx, QWORD PTR [rsp+184]
    and rdx, rax
    xor QWORD PTR [rsp+120], rdx
    xor QWORD PTR [rsp+184], rdx
    lea rcx, [rsp+192]
    lea rdx, [rsp+128]
    lea r8, [rsp+160]
    FIELD_SUB
    lea rcx, [rsp+224]
    lea rdx, [rsp+64]
    lea r8, [rsp+96]
    FIELD_SUB
    lea rcx, [rsp+64]
    lea rdx, [rsp+64]
    lea r8, [rsp+96]
    FIELD_ADD
    lea rcx, [rsp+96]
    lea rdx, [rsp+128]
    lea r8, [rsp+160]
    FIELD_ADD
    lea rcx, [rsp+160]
    lea rdx, [rsp+192]
    lea r8, [rsp+64]
    FIELD_MUL
    lea rcx, [rsp+96]
    lea rdx, [rsp+96]
    lea r8, [rsp+224]
    FIELD_MUL
    lea rcx, [rsp+192]
    lea rdx, [rsp+224]
    FIELD_SQ
    lea rcx, [rsp+224]
    lea rdx, [rsp+64]
    FIELD_SQ
    lea rcx, [rsp+128]
    lea rdx, [rsp+160]
    lea r8, [rsp+96]
    FIELD_ADD
    lea rcx, [rsp+96]
    lea rdx, [rsp+160]
    lea r8, [rsp+96]
    FIELD_SUB
    lea rcx, [rsp+64]
    lea rdx, [rsp+224]
    lea r8, [rsp+192]
    FIELD_MUL
    lea rcx, [rsp+224]
    lea rdx, [rsp+224]
    lea r8, [rsp+192]
    FIELD_SUB
    lea rcx, [rsp+96]
    lea rdx, [rsp+96]
    FIELD_SQ
    lea rcx, [rsp+160]
    lea rdx, [rsp+224]
    FIELD_SCALE
    lea rcx, [rsp+128]
    lea rdx, [rsp+128]
    FIELD_SQ
    lea rcx, [rsp+192]
    lea rdx, [rsp+192]
    lea r8, [rsp+160]
    FIELD_ADD
    lea rcx, [rsp+160]
    lea rdx, [rsp+32]
    lea r8, [rsp+96]
    FIELD_MUL
    lea rcx, [rsp+96]
    lea rdx, [rsp+224]
    lea r8, [rsp+192]
    FIELD_MUL
    dec DWORD PTR [rsp+272]
    jns .Lladder_loop
    mov eax, DWORD PTR [rsp+280]
    neg rax
    mov rdx, QWORD PTR [rsp+64]
    xor rdx, QWORD PTR [rsp+128]
    and rdx, rax
    xor QWORD PTR [rsp+64], rdx
    xor QWORD PTR [rsp+128], rdx
    mov rdx, QWORD PTR [rsp+72]
    xor rdx, QWORD PTR [rsp+136]
    and rdx, rax
    xor QWORD PTR [rsp+72], rdx
    xor QWORD PTR [rsp+136], rdx
    mov rdx, QWORD PTR [rsp+80]
    xor rdx, QWORD PTR [rsp+144]
    and rdx, rax
    xor QWORD PTR [rsp+80], rdx
    xor QWORD PTR [rsp+144], rdx
    mov rdx, QWORD PTR [rsp+88]
    xor rdx, QWORD PTR [rsp+152]
    and rdx, rax
    xor QWORD PTR [rsp+88], rdx
    xor QWORD PTR [rsp+152], rdx
    mov rdx, QWORD PTR [rsp+96]
    xor rdx, QWORD PTR [rsp+160]
    and rdx, rax
    xor QWORD PTR [rsp+96], rdx
    xor QWORD PTR [rsp+160], rdx
    mov rdx, QWORD PTR [rsp+104]
    xor rdx, QWORD PTR [rsp+168]
    and rdx, rax
    xor QWORD PTR [rsp+104], rdx
    xor QWORD PTR [rsp+168], rdx
    mov rdx, QWORD PTR [rsp+112]
    xor rdx, QWORD PTR [rsp+176]
    and rdx, rax
    xor QWORD PTR [rsp+112], rdx
    xor QWORD PTR [rsp+176], rdx
    mov rdx, QWORD PTR [rsp+120]
    xor rdx, QWORD PTR [rsp+184]
    and rdx, rax
    xor QWORD PTR [rsp+120], rdx
    xor QWORD PTR [rsp+184], rdx
    lea rcx, [rsp+96]
    lea rdx, [rsp+96]
    call .Lsharkssl_x25519_inv4_adx
    mov rcx, QWORD PTR [rsp+256]
    lea rdx, [rsp+64]
    lea r8, [rsp+96]
    FIELD_MUL
    mov rcx, QWORD PTR [rsp+256]
    mov r9, QWORD PTR [rcx+0]
    mov r10, QWORD PTR [rcx+8]
    mov r11, QWORD PTR [rcx+16]
    mov r8, QWORD PTR [rcx+24]
    mov rax, r8
    shr rax, 63
    btr r8, 63
    imul rax, rax, 19
    add r9, rax
    adc r10, 0
    adc r11, 0
    adc r8, 0
    mov rax, r8
    shr rax, 63
    btr r8, 63
    imul rax, rax, 19
    add r9, rax
    adc r10, 0
    adc r11, 0
    adc r8, 0
    mov QWORD PTR [rcx+0], r9
    mov QWORD PTR [rcx+8], r10
    mov QWORD PTR [rcx+16], r11
    mov QWORD PTR [rcx+24], r8
    add r9, 19
    adc r10, 0
    adc r11, 0
    adc r8, 0
    btr r8, 63
    cmovnc r9, QWORD PTR [rcx+0]
    cmovnc r10, QWORD PTR [rcx+8]
    cmovnc r11, QWORD PTR [rcx+16]
    cmovnc r8, QWORD PTR [rcx+24]
    mov QWORD PTR [rcx+0], r9
    mov QWORD PTR [rcx+8], r10
    mov QWORD PTR [rcx+16], r11
    mov QWORD PTR [rcx+24], r8
    xor eax, eax
    /* Erase the fixed scratch frame, including field intermediates. */
    xor eax, eax
    mov ecx, 37
    mov rdi, rsp
    rep stosq
    pxor xmm0, xmm0
    pxor xmm1, xmm1
    xor edx, edx
    xor r8d, r8d
    xor r9d, r9d
    xor r10d, r10d
    xor r11d, r11d
    add rsp, 296
.cfi_adjust_cfa_offset -296
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
    pop rbp
.cfi_adjust_cfa_offset -8
.cfi_restore rbp
    pop rbx
.cfi_adjust_cfa_offset -8
.cfi_restore rbx
    pop rsi
.cfi_adjust_cfa_offset -8
.cfi_restore rsi
    pop rdi
.cfi_adjust_cfa_offset -8
.cfi_restore rdi
    ret
.cfi_endproc
.size .Lsharkssl_x25519_adx, .-.Lsharkssl_x25519_adx

.section .note.GNU-stack,"",@progbits
