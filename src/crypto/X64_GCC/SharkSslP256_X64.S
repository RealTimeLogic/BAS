/**
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
 *   $Id: SharkSslP256_X64.S 6799 2026-11-08 06:51:05Z gianluca $
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
 */

/**
 * Original P-256 field arithmetic from the modulus in NIST SP 800-186,
 * p = 2^256 - 2^224 + 2^192 + 2^96 - 1.
 * https://doi.org/10.6028/NIST.SP.800-186
 * No third-party implementation is used.
 *
 * Raw operands/results: eight native U32 words, most significant first,
 * canonical in [0,p). The full inputs are loaded before output stores:
 * r == a, r == b and a == b work. All schedules/addresses are fixed.
 * Multiply uses BMI2 MULX; the caller must probe BMI2 before entering.
 * Add/sub only use baseline x64 instructions. No vector state is used.
 *
 * Product uses four U64 limbs. Reduction treats it as 16 radix-2^32
 * digits, folds x^8 = x^7 - x^6 - x^3 + 1, then normalizes signed U64
 * coefficients. Their bounds, in multiples of (2^32-1), are
 * [-4,3], [-4,3], [-3,3], [-3,6], [-2,6], [-2,6], [-2,7], [-4,5].
 * Carry after normalization is in [-4,5]. For R=2^256 and Q=R-p,
 * 6Q<R. Two folds of the carry times Q eliminate that carry: a first
 * positive wrap leaves low<5Q, a negative wrap leaves low>=R-4Q, and
 * the second fold therefore cannot wrap. The result is below R<2p;
 * one conditional subtraction gives its canonical representative.
 *
 * The fixed scratch is erased, working registers cleared, and caller's
 * nonvolatile registers restored. This is not power/EM countermeasure.
 */
.intel_syntax noprefix
.text

.p2align 4
.globl sharkssl_p256_mulmod64
.hidden sharkssl_p256_mulmod64
.type sharkssl_p256_mulmod64, @function
sharkssl_p256_mulmod64:
   .cfi_startproc
   push rdi
   .cfi_def_cfa_offset 16
   .cfi_offset rdi, -16
   push rsi
   .cfi_def_cfa_offset 24
   .cfi_offset rsi, -24
   push rbx
   .cfi_def_cfa_offset 32
   .cfi_offset rbx, -32
   push rbp
   .cfi_def_cfa_offset 40
   .cfi_offset rbp, -40
   push r12
   .cfi_def_cfa_offset 48
   .cfi_offset r12, -48
   push r13
   .cfi_def_cfa_offset 56
   .cfi_offset r13, -56
   push r14
   .cfi_def_cfa_offset 64
   .cfi_offset r14, -64
   push r15
   .cfi_def_cfa_offset 72
   .cfi_offset r15, -72
   sub rsp, 136
   .cfi_def_cfa_offset 208
   mov rcx, rdx
   mov rax, QWORD PTR [rsi + 24]
   ror rax, 32
   mov QWORD PTR [rsp + 64], rax
   mov rax, QWORD PTR [rcx + 24]
   ror rax, 32
   mov QWORD PTR [rsp + 96], rax
   mov rax, QWORD PTR [rsi + 16]
   ror rax, 32
   mov QWORD PTR [rsp + 72], rax
   mov rax, QWORD PTR [rcx + 16]
   ror rax, 32
   mov QWORD PTR [rsp + 104], rax
   mov rax, QWORD PTR [rsi + 8]
   ror rax, 32
   mov QWORD PTR [rsp + 80], rax
   mov rax, QWORD PTR [rcx + 8]
   ror rax, 32
   mov QWORD PTR [rsp + 112], rax
   mov rax, QWORD PTR [rsi + 0]
   ror rax, 32
   mov QWORD PTR [rsp + 88], rax
   mov rax, QWORD PTR [rcx + 0]
   ror rax, 32
   mov QWORD PTR [rsp + 120], rax
   xor eax, eax
   mov QWORD PTR [rsp + 0], rax
   mov QWORD PTR [rsp + 8], rax
   mov QWORD PTR [rsp + 16], rax
   mov QWORD PTR [rsp + 24], rax
   mov QWORD PTR [rsp + 32], rax
   mov QWORD PTR [rsp + 40], rax
   mov QWORD PTR [rsp + 48], rax
   mov QWORD PTR [rsp + 56], rax
   mov rdx, QWORD PTR [rsp + 96]
   xor ebp, ebp
   mulx r10, r9, QWORD PTR [rsp + 64]
   add r9, QWORD PTR [rsp + 0]
   adc r10, 0
   add r9, rbp
   adc r10, 0
   mov QWORD PTR [rsp + 0], r9
   mov rbp, r10
   mulx r10, r9, QWORD PTR [rsp + 72]
   add r9, QWORD PTR [rsp + 8]
   adc r10, 0
   add r9, rbp
   adc r10, 0
   mov QWORD PTR [rsp + 8], r9
   mov rbp, r10
   mulx r10, r9, QWORD PTR [rsp + 80]
   add r9, QWORD PTR [rsp + 16]
   adc r10, 0
   add r9, rbp
   adc r10, 0
   mov QWORD PTR [rsp + 16], r9
   mov rbp, r10
   mulx r10, r9, QWORD PTR [rsp + 88]
   add r9, QWORD PTR [rsp + 24]
   adc r10, 0
   add r9, rbp
   adc r10, 0
   mov QWORD PTR [rsp + 24], r9
   mov rbp, r10
   mov QWORD PTR [rsp + 32], rbp
   mov rdx, QWORD PTR [rsp + 104]
   xor ebp, ebp
   mulx r10, r9, QWORD PTR [rsp + 64]
   add r9, QWORD PTR [rsp + 8]
   adc r10, 0
   add r9, rbp
   adc r10, 0
   mov QWORD PTR [rsp + 8], r9
   mov rbp, r10
   mulx r10, r9, QWORD PTR [rsp + 72]
   add r9, QWORD PTR [rsp + 16]
   adc r10, 0
   add r9, rbp
   adc r10, 0
   mov QWORD PTR [rsp + 16], r9
   mov rbp, r10
   mulx r10, r9, QWORD PTR [rsp + 80]
   add r9, QWORD PTR [rsp + 24]
   adc r10, 0
   add r9, rbp
   adc r10, 0
   mov QWORD PTR [rsp + 24], r9
   mov rbp, r10
   mulx r10, r9, QWORD PTR [rsp + 88]
   add r9, QWORD PTR [rsp + 32]
   adc r10, 0
   add r9, rbp
   adc r10, 0
   mov QWORD PTR [rsp + 32], r9
   mov rbp, r10
   mov QWORD PTR [rsp + 40], rbp
   mov rdx, QWORD PTR [rsp + 112]
   xor ebp, ebp
   mulx r10, r9, QWORD PTR [rsp + 64]
   add r9, QWORD PTR [rsp + 16]
   adc r10, 0
   add r9, rbp
   adc r10, 0
   mov QWORD PTR [rsp + 16], r9
   mov rbp, r10
   mulx r10, r9, QWORD PTR [rsp + 72]
   add r9, QWORD PTR [rsp + 24]
   adc r10, 0
   add r9, rbp
   adc r10, 0
   mov QWORD PTR [rsp + 24], r9
   mov rbp, r10
   mulx r10, r9, QWORD PTR [rsp + 80]
   add r9, QWORD PTR [rsp + 32]
   adc r10, 0
   add r9, rbp
   adc r10, 0
   mov QWORD PTR [rsp + 32], r9
   mov rbp, r10
   mulx r10, r9, QWORD PTR [rsp + 88]
   add r9, QWORD PTR [rsp + 40]
   adc r10, 0
   add r9, rbp
   adc r10, 0
   mov QWORD PTR [rsp + 40], r9
   mov rbp, r10
   mov QWORD PTR [rsp + 48], rbp
   mov rdx, QWORD PTR [rsp + 120]
   xor ebp, ebp
   mulx r10, r9, QWORD PTR [rsp + 64]
   add r9, QWORD PTR [rsp + 24]
   adc r10, 0
   add r9, rbp
   adc r10, 0
   mov QWORD PTR [rsp + 24], r9
   mov rbp, r10
   mulx r10, r9, QWORD PTR [rsp + 72]
   add r9, QWORD PTR [rsp + 32]
   adc r10, 0
   add r9, rbp
   adc r10, 0
   mov QWORD PTR [rsp + 32], r9
   mov rbp, r10
   mulx r10, r9, QWORD PTR [rsp + 80]
   add r9, QWORD PTR [rsp + 40]
   adc r10, 0
   add r9, rbp
   adc r10, 0
   mov QWORD PTR [rsp + 40], r9
   mov rbp, r10
   mulx r10, r9, QWORD PTR [rsp + 88]
   add r9, QWORD PTR [rsp + 48]
   adc r10, 0
   add r9, rbp
   adc r10, 0
   mov QWORD PTR [rsp + 48], r9
   mov rbp, r10
   mov QWORD PTR [rsp + 56], rbp
   mov r8d, DWORD PTR [rsp + 0]
   mov r9d, DWORD PTR [rsp + 4]
   mov r10d, DWORD PTR [rsp + 8]
   mov r11d, DWORD PTR [rsp + 12]
   mov r12d, DWORD PTR [rsp + 16]
   mov r13d, DWORD PTR [rsp + 20]
   mov r14d, DWORD PTR [rsp + 24]
   mov r15d, DWORD PTR [rsp + 28]
   mov eax, DWORD PTR [rsp + 32]
   add r8, rax
   sub r11, rax
   sub r14, rax
   add r15, rax
   mov eax, DWORD PTR [rsp + 36]
   add r8, rax
   add r9, rax
   sub r11, rax
   sub r12, rax
   sub r14, rax
   mov eax, DWORD PTR [rsp + 40]
   add r9, rax
   add r10, rax
   sub r12, rax
   sub r13, rax
   sub r15, rax
   mov eax, DWORD PTR [rsp + 44]
   sub r8, rax
   add r10, rax
   add r11, rax
   add r11, rax
   sub r13, rax
   sub r15, rax
   mov eax, DWORD PTR [rsp + 48]
   sub r8, rax
   sub r9, rax
   add r11, rax
   add r11, rax
   add r12, rax
   add r12, rax
   sub r15, rax
   mov eax, DWORD PTR [rsp + 52]
   sub r8, rax
   sub r9, rax
   sub r10, rax
   add r11, rax
   add r12, rax
   add r12, rax
   add r13, rax
   add r13, rax
   add r14, rax
   sub r15, rax
   mov eax, DWORD PTR [rsp + 56]
   sub r8, rax
   sub r9, rax
   sub r10, rax
   add r12, rax
   add r13, rax
   add r13, rax
   add r14, rax
   add r14, rax
   add r14, rax
   mov eax, DWORD PTR [rsp + 60]
   sub r9, rax
   sub r10, rax
   sub r11, rax
   add r13, rax
   add r14, rax
   add r14, rax
   add r15, rax
   add r15, rax
   add r15, rax
   mov rax, r8
   sar rax, 32
   mov r8d, r8d
   add r9, rax
   mov rax, r9
   sar rax, 32
   mov r9d, r9d
   add r10, rax
   mov rax, r10
   sar rax, 32
   mov r10d, r10d
   add r11, rax
   mov rax, r11
   sar rax, 32
   mov r11d, r11d
   add r12, rax
   mov rax, r12
   sar rax, 32
   mov r12d, r12d
   add r13, rax
   mov rax, r13
   sar rax, 32
   mov r13d, r13d
   add r14, rax
   mov rax, r14
   sar rax, 32
   mov r14d, r14d
   add r15, rax
   mov rax, r15
   sar rax, 32
   mov r15d, r15d
   add r8, rax
   sub r11, rax
   sub r14, rax
   add r15, rax
   mov rax, r8
   sar rax, 32
   mov r8d, r8d
   add r9, rax
   mov rax, r9
   sar rax, 32
   mov r9d, r9d
   add r10, rax
   mov rax, r10
   sar rax, 32
   mov r10d, r10d
   add r11, rax
   mov rax, r11
   sar rax, 32
   mov r11d, r11d
   add r12, rax
   mov rax, r12
   sar rax, 32
   mov r12d, r12d
   add r13, rax
   mov rax, r13
   sar rax, 32
   mov r13d, r13d
   add r14, rax
   mov rax, r14
   sar rax, 32
   mov r14d, r14d
   add r15, rax
   mov rax, r15
   sar rax, 32
   mov r15d, r15d
   add r8, rax
   sub r11, rax
   sub r14, rax
   add r15, rax
   mov rax, r8
   sar rax, 32
   mov r8d, r8d
   add r9, rax
   mov rax, r9
   sar rax, 32
   mov r9d, r9d
   add r10, rax
   mov rax, r10
   sar rax, 32
   mov r10d, r10d
   add r11, rax
   mov rax, r11
   sar rax, 32
   mov r11d, r11d
   add r12, rax
   mov rax, r12
   sar rax, 32
   mov r12d, r12d
   add r13, rax
   mov rax, r13
   sar rax, 32
   mov r13d, r13d
   add r14, rax
   mov rax, r14
   sar rax, 32
   mov r14d, r14d
   add r15, rax
   mov rax, r15
   sar rax, 32
   mov r15d, r15d
   shl r9, 32
   or r8, r9
   shl r11, 32
   or r10, r11
   shl r13, 32
   or r12, r13
   shl r15, 32
   or r14, r15
   mov rax, r8
   mov rcx, r10
   mov rdx, r12
   mov rbp, r14
   mov ebx, 0xffffffff
   sub rax, -1
   sbb rcx, rbx
   sbb rdx, 0
   mov rbx, 0xffffffff00000001
   sbb rbp, rbx
   cmovnc r8, rax
   cmovnc r10, rcx
   cmovnc r12, rdx
   cmovnc r14, rbp
   ror r8, 32
   mov QWORD PTR [rdi + 24], r8
   ror r10, 32
   mov QWORD PTR [rdi + 16], r10
   ror r12, 32
   mov QWORD PTR [rdi + 8], r12
   ror r14, 32
   mov QWORD PTR [rdi + 0], r14
   xor eax, eax
   mov QWORD PTR [rsp + 0], rax
   mov QWORD PTR [rsp + 8], rax
   mov QWORD PTR [rsp + 16], rax
   mov QWORD PTR [rsp + 24], rax
   mov QWORD PTR [rsp + 32], rax
   mov QWORD PTR [rsp + 40], rax
   mov QWORD PTR [rsp + 48], rax
   mov QWORD PTR [rsp + 56], rax
   mov QWORD PTR [rsp + 64], rax
   mov QWORD PTR [rsp + 72], rax
   mov QWORD PTR [rsp + 80], rax
   mov QWORD PTR [rsp + 88], rax
   mov QWORD PTR [rsp + 96], rax
   mov QWORD PTR [rsp + 104], rax
   mov QWORD PTR [rsp + 112], rax
   mov QWORD PTR [rsp + 120], rax
   xor ecx, ecx
   xor edx, edx
   xor ebp, ebp
   xor r8d, r8d
   xor r9d, r9d
   xor r10d, r10d
   xor r11d, r11d
   xor r12d, r12d
   xor r13d, r13d
   xor r14d, r14d
   xor r15d, r15d
   add rsp, 136
   .cfi_def_cfa_offset 72
   pop r15
   .cfi_restore r15
   .cfi_def_cfa_offset 64
   pop r14
   .cfi_restore r14
   .cfi_def_cfa_offset 56
   pop r13
   .cfi_restore r13
   .cfi_def_cfa_offset 48
   pop r12
   .cfi_restore r12
   .cfi_def_cfa_offset 40
   pop rbp
   .cfi_restore rbp
   .cfi_def_cfa_offset 32
   pop rbx
   .cfi_restore rbx
   .cfi_def_cfa_offset 24
   pop rsi
   .cfi_restore rsi
   .cfi_def_cfa_offset 16
   pop rdi
   .cfi_restore rdi
   .cfi_def_cfa_offset 8
   ret
   .cfi_endproc
.size sharkssl_p256_mulmod64, .-sharkssl_p256_mulmod64

.p2align 4
.globl sharkssl_p256_addmod64
.hidden sharkssl_p256_addmod64
.type sharkssl_p256_addmod64, @function
sharkssl_p256_addmod64:
   .cfi_startproc
   push rdi
   .cfi_def_cfa_offset 16
   .cfi_offset rdi, -16
   push rsi
   .cfi_def_cfa_offset 24
   .cfi_offset rsi, -24
   push rbx
   .cfi_def_cfa_offset 32
   .cfi_offset rbx, -32
   push rbp
   .cfi_def_cfa_offset 40
   .cfi_offset rbp, -40
   push r12
   .cfi_def_cfa_offset 48
   .cfi_offset r12, -48
   push r13
   .cfi_def_cfa_offset 56
   .cfi_offset r13, -56
   push r14
   .cfi_def_cfa_offset 64
   .cfi_offset r14, -64
   push r15
   .cfi_def_cfa_offset 72
   .cfi_offset r15, -72
   sub rsp, 8
   .cfi_def_cfa_offset 80
   mov r8, QWORD PTR [rsi + 24]
   ror r8, 32
   mov r9, QWORD PTR [rsi + 16]
   ror r9, 32
   mov r10, QWORD PTR [rsi + 8]
   ror r10, 32
   mov r11, QWORD PTR [rsi + 0]
   ror r11, 32
   mov r12, QWORD PTR [rdx + 24]
   ror r12, 32
   mov r13, QWORD PTR [rdx + 16]
   ror r13, 32
   mov r14, QWORD PTR [rdx + 8]
   ror r14, 32
   mov r15, QWORD PTR [rdx + 0]
   ror r15, 32
   add r8, r12
   adc r9, r13
   adc r10, r14
   adc r11, r15
   mov eax, 0
   adc rax, 0
   mov r12, r8
   mov r13, r9
   mov r14, r10
   mov r15, r11
   mov ebx, 0xffffffff
   sub r12, -1
   sbb r13, rbx
   sbb r14, 0
   mov rbp, 0xffffffff00000001
   sbb r15, rbp
   sbb rax, 0
   cmovnc r8, r12
   cmovnc r9, r13
   cmovnc r10, r14
   cmovnc r11, r15
   ror r8, 32
   mov QWORD PTR [rdi + 24], r8
   ror r9, 32
   mov QWORD PTR [rdi + 16], r9
   ror r10, 32
   mov QWORD PTR [rdi + 8], r10
   ror r11, 32
   mov QWORD PTR [rdi + 0], r11
   xor eax, eax
   xor ecx, ecx
   xor edx, edx
   xor ebp, ebp
   xor r8d, r8d
   xor r9d, r9d
   xor r10d, r10d
   xor r11d, r11d
   xor r12d, r12d
   xor r13d, r13d
   xor r14d, r14d
   xor r15d, r15d
   add rsp, 8
   .cfi_def_cfa_offset 72
   pop r15
   .cfi_restore r15
   .cfi_def_cfa_offset 64
   pop r14
   .cfi_restore r14
   .cfi_def_cfa_offset 56
   pop r13
   .cfi_restore r13
   .cfi_def_cfa_offset 48
   pop r12
   .cfi_restore r12
   .cfi_def_cfa_offset 40
   pop rbp
   .cfi_restore rbp
   .cfi_def_cfa_offset 32
   pop rbx
   .cfi_restore rbx
   .cfi_def_cfa_offset 24
   pop rsi
   .cfi_restore rsi
   .cfi_def_cfa_offset 16
   pop rdi
   .cfi_restore rdi
   .cfi_def_cfa_offset 8
   ret
   .cfi_endproc
.size sharkssl_p256_addmod64, .-sharkssl_p256_addmod64

.p2align 4
.globl sharkssl_p256_submod64
.hidden sharkssl_p256_submod64
.type sharkssl_p256_submod64, @function
sharkssl_p256_submod64:
   .cfi_startproc
   push rdi
   .cfi_def_cfa_offset 16
   .cfi_offset rdi, -16
   push rsi
   .cfi_def_cfa_offset 24
   .cfi_offset rsi, -24
   push rbx
   .cfi_def_cfa_offset 32
   .cfi_offset rbx, -32
   push rbp
   .cfi_def_cfa_offset 40
   .cfi_offset rbp, -40
   push r12
   .cfi_def_cfa_offset 48
   .cfi_offset r12, -48
   push r13
   .cfi_def_cfa_offset 56
   .cfi_offset r13, -56
   push r14
   .cfi_def_cfa_offset 64
   .cfi_offset r14, -64
   push r15
   .cfi_def_cfa_offset 72
   .cfi_offset r15, -72
   sub rsp, 8
   .cfi_def_cfa_offset 80
   mov r8, QWORD PTR [rsi + 24]
   ror r8, 32
   mov r9, QWORD PTR [rsi + 16]
   ror r9, 32
   mov r10, QWORD PTR [rsi + 8]
   ror r10, 32
   mov r11, QWORD PTR [rsi + 0]
   ror r11, 32
   mov r12, QWORD PTR [rdx + 24]
   ror r12, 32
   mov r13, QWORD PTR [rdx + 16]
   ror r13, 32
   mov r14, QWORD PTR [rdx + 8]
   ror r14, 32
   mov r15, QWORD PTR [rdx + 0]
   ror r15, 32
   sub r8, r12
   sbb r9, r13
   sbb r10, r14
   sbb r11, r15
   sbb rax, rax
   mov ebx, 0xffffffff
   and rbx, rax
   mov rbp, 0xffffffff00000001
   and rbp, rax
   add r8, rax
   adc r9, rbx
   adc r10, 0
   adc r11, rbp
   ror r8, 32
   mov QWORD PTR [rdi + 24], r8
   ror r9, 32
   mov QWORD PTR [rdi + 16], r9
   ror r10, 32
   mov QWORD PTR [rdi + 8], r10
   ror r11, 32
   mov QWORD PTR [rdi + 0], r11
   xor eax, eax
   xor ecx, ecx
   xor edx, edx
   xor ebp, ebp
   xor r8d, r8d
   xor r9d, r9d
   xor r10d, r10d
   xor r11d, r11d
   xor r12d, r12d
   xor r13d, r13d
   xor r14d, r14d
   xor r15d, r15d
   add rsp, 8
   .cfi_def_cfa_offset 72
   pop r15
   .cfi_restore r15
   .cfi_def_cfa_offset 64
   pop r14
   .cfi_restore r14
   .cfi_def_cfa_offset 56
   pop r13
   .cfi_restore r13
   .cfi_def_cfa_offset 48
   pop r12
   .cfi_restore r12
   .cfi_def_cfa_offset 40
   pop rbp
   .cfi_restore rbp
   .cfi_def_cfa_offset 32
   pop rbx
   .cfi_restore rbx
   .cfi_def_cfa_offset 24
   pop rsi
   .cfi_restore rsi
   .cfi_def_cfa_offset 16
   pop rdi
   .cfi_restore rdi
   .cfi_def_cfa_offset 8
   ret
   .cfi_endproc
.size sharkssl_p256_submod64, .-sharkssl_p256_submod64

.p2align 4
.globl sharkssl_p256_hw_probe
.hidden sharkssl_p256_hw_probe
.type sharkssl_p256_hw_probe, @function
sharkssl_p256_hw_probe:
   .cfi_startproc
   push rbx
   .cfi_def_cfa_offset 16
   .cfi_offset rbx, -16
   xor eax, eax
   cpuid
   cmp eax, 7
   jb 1f
   mov eax, 7
   xor ecx, ecx
   cpuid
   mov eax, ebx
   shr eax, 8
   and eax, 1
   jmp 2f
1:
   xor eax, eax
2:
   pop rbx
   .cfi_def_cfa_offset 8
   .cfi_restore rbx
   ret
   .cfi_endproc
.size sharkssl_p256_hw_probe, .-sharkssl_p256_hw_probe
.section .note.GNU-stack,"",@progbits
