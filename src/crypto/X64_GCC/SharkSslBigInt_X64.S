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
 *   $Id: SharkSslBigInt_X64.S 6818 2026-11-08 21:18:02Z gianluca $
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
 * Original multiple-precision multiplication and Montgomery reduction from
 * Handbook of Applied Cryptography, Algorithms 14.12 and 14.32:
 * https://cacr.uwaterloo.ca/hac/about/chap14.pdf
 * Instruction semantics: Intel 64 and IA-32 Architectures SDM, volume 2.
 * No third-party implementation is used.
 *
 * Arrays hold native U32 digits most significant first, radix 2^32.
 * Lengths, pointer equality and buffer extents are public. The result must
 * not overlap an input; the two multiplication inputs may be identical.
 * MUL writes alen+blen words. REDC accepts 2*n+1 words containing T,
 * 0<=T<mod*R, mod<R=2^(32*n), odd mod and mu=-mod^-1 modulo 2^32.
 * Its canonical result occupies t[1..n]; t[0] and t[n+1..2*n] are zero.
 * Leading zero digits and odd word counts are supported.
 *
 * Even lengths use paired U64 digits, with ROR32 translating native U32
 * word order. MUL forms the full U128 product; product+digit+carry fits
 * U128. REDC lifts mu by u64=u32*(2+mod_low64*u32) modulo 2^64. Odd
 * lengths retain radix 2^32: IMUL receives zero-extended U32 operands,
 * and product+digit+carry fits U64. Every carry loop has a public extent.
 * Subtraction always runs and uses masked add-back, without a secret
 * branch or address. No division, optional CPU feature, SIMD state, call
 * or dynamic stack allocation. CT assumes data-independent MUL/IMUL
 * latency on the deployment processor; this is not an ISA guarantee.
 * Working registers are cleared before saved registers are restored.
 * These timing/cache properties do not provide power/EM resistance.
 */
#if defined(__x86_64__) && defined(__ELF__) && !defined(__ILP32__)
.intel_syntax noprefix
.text

.macro save_regs
   push rbx
   .cfi_def_cfa_offset 16
   .cfi_offset rbx, -16
   push rbp
   .cfi_def_cfa_offset 24
   .cfi_offset rbp, -24
   push r12
   .cfi_def_cfa_offset 32
   .cfi_offset r12, -32
   push r13
   .cfi_def_cfa_offset 40
   .cfi_offset r13, -40
   push r14
   .cfi_def_cfa_offset 48
   .cfi_offset r14, -48
   push r15
   .cfi_def_cfa_offset 56
   .cfi_offset r15, -56
.endm

.macro restore_regs
   xor eax, eax
   xor ecx, ecx
   xor edx, edx
   xor esi, esi
   xor edi, edi
   xor r8d, r8d
   xor r9d, r9d
   xor r10d, r10d
   xor r11d, r11d
   xor ebx, ebx
   xor ebp, ebp
   xor r12d, r12d
   xor r13d, r13d
   xor r14d, r14d
   xor r15d, r15d
   pop r15
   .cfi_restore r15
   .cfi_def_cfa_offset 48
   pop r14
   .cfi_restore r14
   .cfi_def_cfa_offset 40
   pop r13
   .cfi_restore r13
   .cfi_def_cfa_offset 32
   pop r12
   .cfi_restore r12
   .cfi_def_cfa_offset 24
   pop rbp
   .cfi_restore rbp
   .cfi_def_cfa_offset 16
   pop rbx
   .cfi_restore rbx
   .cfi_def_cfa_offset 8
   ret
.endm

/** One radix-2^32 digit multiply/add, propagating its full U64 carry. */
.macro digit_product displ
   mov eax, DWORD PTR [rsi - 4 - \displ]
   imul rax, r9
   mov edx, DWORD PTR [rdi - \displ]
   add rax, rdx
   add rax, r10
   mov DWORD PTR [rdi - \displ], eax
   shr rax, 32
   mov r10, rax
.endm

/** A paired U64 digit; ROR32 translates the MSW-first native U32 pair. */
.macro pair_product displ
   mov rax, QWORD PTR [rsi - 8 - \displ]
   ror rax, 32
   mul r9
   mov r11, QWORD PTR [rdi - \displ]
   ror r11, 32
   add rax, r11
   adc rdx, 0
   add rax, r10
   adc rdx, 0
   mov r10, rdx
   ror rax, 32
   mov QWORD PTR [rdi - \displ], rax
.endm

.p2align 4
.globl sharkssl_bigint_mul_x64
.hidden sharkssl_bigint_mul_x64
.type sharkssl_bigint_mul_x64, @function
sharkssl_bigint_mul_x64:
   .cfi_startproc
   save_regs
   mov r12, rdi
   mov ebp, ecx
   mov r15d, r8d
   lea r13, [rsi + rbp*4]
   lea r14, [rdx + r15*4]
   add ecx, r8d
   lea rbx, [rdi + rcx*4]
   xor eax, eax
   rep stosd
   test ebp, ebp
   jz .Lmul_done
   test r15d, r15d
   jz .Lmul_done
   mov eax, ebp
   or eax, r15d
   test eax, 1
   jz .Lmul_pair_start
.Lmul_outer:
   mov r9d, DWORD PTR [r14 - 4]
   sub r14, 4
   sub rbx, 4
   mov rdi, rbx
   mov rsi, r13
   mov ecx, ebp
   xor r10d, r10d
   cmp ecx, 4
   jb .Lmul_tail
.Lmul_four:
   digit_product 0
   digit_product 4
   digit_product 8
   digit_product 12
   sub rsi, 16
   sub rdi, 16
   sub ecx, 4
   cmp ecx, 4
   jae .Lmul_four
.Lmul_tail:
   test ecx, ecx
   jz .Lmul_carry
.Lmul_one:
   digit_product 0
   sub rsi, 4
   sub rdi, 4
   dec ecx
   jnz .Lmul_one
.Lmul_carry:
   mov DWORD PTR [rdi], r10d
   dec r15d
   jnz .Lmul_outer
   jmp .Lmul_done
.Lmul_pair_start:
   shr ebp, 1
   shr r15d, 1
.Lmul_pair_outer:
   mov r9, QWORD PTR [r14 - 8]
   ror r9, 32
   sub r14, 8
   sub rbx, 8
   mov rdi, rbx
   mov rsi, r13
   mov ecx, ebp
   xor r10d, r10d
   cmp ecx, 4
   jb .Lmul_pair_tail
.Lmul_pair_four:
   pair_product 0
   pair_product 8
   pair_product 16
   pair_product 24
   sub rsi, 32
   sub rdi, 32
   sub ecx, 4
   cmp ecx, 4
   jae .Lmul_pair_four
.Lmul_pair_tail:
   test ecx, ecx
   jz .Lmul_pair_carry
.Lmul_pair_one:
   pair_product 0
   sub rsi, 8
   sub rdi, 8
   dec ecx
   jnz .Lmul_pair_one
.Lmul_pair_carry:
   ror r10, 32
   mov QWORD PTR [rdi], r10
   dec r15d
   jnz .Lmul_pair_outer
.Lmul_done:
   restore_regs
   .cfi_endproc
.size sharkssl_bigint_mul_x64, .-sharkssl_bigint_mul_x64

.p2align 4
.globl sharkssl_bigint_redc_x64
.hidden sharkssl_bigint_redc_x64
.type sharkssl_bigint_redc_x64, @function
sharkssl_bigint_redc_x64:
   .cfi_startproc
   save_regs
   mov r12, rdi
   mov ebp, edx
   mov r15d, ecx
   mov r14d, edx
   lea r13, [rsi + rbp*4]
   lea rbx, [rdi + rbp*8]
   add rbx, 4
   test ebp, ebp
   jz .Lredc_done
   test ebp, 1
   jz .Lredc_pair_start
.Lredc_outer:
   sub rbx, 4
   mov r9d, DWORD PTR [rbx]
   imul r9d, r15d
   mov rdi, rbx
   mov rsi, r13
   mov ecx, ebp
   xor r10d, r10d
   cmp ecx, 4
   jb .Lredc_tail
.Lredc_four:
   digit_product 0
   digit_product 4
   digit_product 8
   digit_product 12
   sub rsi, 16
   sub rdi, 16
   sub ecx, 4
   cmp ecx, 4
   jae .Lredc_four
.Lredc_tail:
   test ecx, ecx
   jz .Lredc_carry_start
.Lredc_one:
   digit_product 0
   sub rsi, 4
   sub rdi, 4
   dec ecx
   jnz .Lredc_one
.Lredc_carry_start:
   lea ecx, [r14d + 1]
   add DWORD PTR [rdi], r10d
   lea rdi, [rdi - 4]
   dec ecx
.Lredc_carry:
   adc DWORD PTR [rdi], 0
   lea rdi, [rdi - 4]
   dec ecx
   jnz .Lredc_carry
   dec r14d
   jnz .Lredc_outer

/** Subtract the modulus over n+1 digits and retain the final borrow mask. */
   lea rdi, [r12 + rbp*4]
   mov rsi, r13
   mov ecx, ebp
   xor r10d, r10d
.Lredc_subtract:
   mov eax, DWORD PTR [rdi]
   mov edx, DWORD PTR [rsi - 4]
   add rdx, r10
   sub rax, rdx
   mov DWORD PTR [rdi], eax
   shr rax, 63
   mov r10, rax
   sub rsi, 4
   sub rdi, 4
   dec ecx
   jnz .Lredc_subtract
   mov eax, DWORD PTR [r12]
   sub rax, r10
   sar rax, 63
   mov r11d, eax

/** Add back mod & mask in a fixed n-digit traversal. */
   lea rdi, [r12 + rbp*4]
   mov rsi, r13
   mov ecx, ebp
   xor r10d, r10d
.Lredc_addback:
   mov edx, DWORD PTR [rsi - 4]
   and edx, r11d
   mov eax, DWORD PTR [rdi]
   add rax, rdx
   add rax, r10
   mov DWORD PTR [rdi], eax
   shr rax, 32
   mov r10, rax
   sub rsi, 4
   sub rdi, 4
   dec ecx
   jnz .Lredc_addback
   mov DWORD PTR [r12], 0
   jmp .Lredc_done

/** Even n: pair digits without changing R=2^(32*n) or the U32 ABI. */
.Lredc_pair_start:
   mov rax, QWORD PTR [r13 - 8]
   ror rax, 32
   imul rax, r15
   add rax, 2
   imul r15, rax
   shr ebp, 1
   shr r14d, 1
.Lredc_pair_outer:
   sub rbx, 8
   mov r9, QWORD PTR [rbx]
   ror r9, 32
   imul r9, r15
   mov rdi, rbx
   mov rsi, r13
   mov ecx, ebp
   xor r10d, r10d
   cmp ecx, 4
   jb .Lredc_pair_tail
.Lredc_pair_four:
   pair_product 0
   pair_product 8
   pair_product 16
   pair_product 24
   sub rsi, 32
   sub rdi, 32
   sub ecx, 4
   cmp ecx, 4
   jae .Lredc_pair_four
.Lredc_pair_tail:
   test ecx, ecx
   jz .Lredc_pair_carry_start
.Lredc_pair_one:
   pair_product 0
   sub rsi, 8
   sub rdi, 8
   dec ecx
   jnz .Lredc_pair_one
.Lredc_pair_carry_start:
   mov ecx, r14d
.Lredc_pair_carry:
   mov rax, QWORD PTR [rdi]
   ror rax, 32
   add rax, r10
   setc r10b
   movzx r10d, r10b
   ror rax, 32
   mov QWORD PTR [rdi], rax
   sub rdi, 8
   dec ecx
   jnz .Lredc_pair_carry
   add DWORD PTR [r12], r10d
   dec r14d
   jnz .Lredc_pair_outer

/** Full U64 subtraction uses the borrow flag, not signed headroom. */
   lea rdi, [r12 + rbp*8 - 4]
   mov rsi, r13
   mov ecx, ebp
   xor r10d, r10d
.Lredc_pair_subtract:
   mov rax, QWORD PTR [rdi]
   ror rax, 32
   mov rdx, QWORD PTR [rsi - 8]
   ror rdx, 32
   bt r10, 0
   sbb rax, rdx
   setc r10b
   movzx r10d, r10b
   ror rax, 32
   mov QWORD PTR [rdi], rax
   sub rsi, 8
   sub rdi, 8
   dec ecx
   jnz .Lredc_pair_subtract
   mov eax, DWORD PTR [r12]
   sub rax, r10
   sar rax, 63
   mov r11, rax

/** Masked add-back likewise preserves the full U64 carry explicitly. */
   lea rdi, [r12 + rbp*8 - 4]
   mov rsi, r13
   mov ecx, ebp
   xor r10d, r10d
.Lredc_pair_addback:
   mov rdx, QWORD PTR [rsi - 8]
   ror rdx, 32
   and rdx, r11
   mov rax, QWORD PTR [rdi]
   ror rax, 32
   bt r10, 0
   adc rax, rdx
   setc r10b
   movzx r10d, r10b
   ror rax, 32
   mov QWORD PTR [rdi], rax
   sub rsi, 8
   sub rdi, 8
   dec ecx
   jnz .Lredc_pair_addback
   mov DWORD PTR [r12], 0
.Lredc_done:
   restore_regs
   .cfi_endproc
.size sharkssl_bigint_redc_x64, .-sharkssl_bigint_redc_x64

.p2align 4
.globl SharkSslBigInt_add
.hidden SharkSslBigInt_add
.type SharkSslBigInt_add, @function
SharkSslBigInt_add:
   .cfi_startproc
   mov r8, QWORD PTR [rdi + 8]
   movzx r11d, WORD PTR [rdi + 16]
   mov r9, QWORD PTR [rsi + 8]
   movzx edx, WORD PTR [rsi + 16]
   mov ecx, r11d
   lea r8, [r8 + rcx*4 - 4]
   lea r9, [r9 + rdx*4 - 4]
   cmp edx, ecx
   cmova edx, ecx
   sub ecx, edx
   xor r10d, r10d
   test edx, edx
   jz .Lfull_add_prefix
.Lfull_add_both:
   mov eax, DWORD PTR [r8]
   mov r11d, DWORD PTR [r9]
   add rax, r11
   add rax, r10
   mov DWORD PTR [r8], eax
   shr rax, 32
   mov r10, rax
   sub r8, 4
   sub r9, 4
   dec edx
   jnz .Lfull_add_both
.Lfull_add_prefix:
   test ecx, ecx
   jz .Lfull_add_done
.Lfull_add_carry:
   mov eax, DWORD PTR [r8]
   add rax, r10
   mov DWORD PTR [r8], eax
   shr rax, 32
   mov r10, rax
   sub r8, 4
   dec ecx
   jnz .Lfull_add_carry
.Lfull_add_done:
   mov eax, r10d
   xor ecx, ecx
   xor edx, edx
   xor r8d, r8d
   xor r9d, r9d
   xor r10d, r10d
   xor r11d, r11d
   ret
   .cfi_endproc
.size SharkSslBigInt_add, .-SharkSslBigInt_add

.p2align 4
.globl SharkSslBigInt_sub
.hidden SharkSslBigInt_sub
.type SharkSslBigInt_sub, @function
SharkSslBigInt_sub:
   .cfi_startproc
   mov r8, QWORD PTR [rdi + 8]
   movzx r11d, WORD PTR [rdi + 16]
   mov r9, QWORD PTR [rsi + 8]
   movzx edx, WORD PTR [rsi + 16]
   mov ecx, r11d
   lea r8, [r8 + rcx*4 - 4]
   lea r9, [r9 + rdx*4 - 4]
   cmp edx, ecx
   cmova edx, ecx
   sub ecx, edx
   xor r10d, r10d
   test edx, edx
   jz .Lfull_sub_prefix
.Lfull_sub_both:
   mov eax, DWORD PTR [r8]
   mov r11d, DWORD PTR [r9]
   sub rax, r11
   add rax, r10
   mov DWORD PTR [r8], eax
   sar rax, 32
   mov r10, rax
   sub r8, 4
   sub r9, 4
   dec edx
   jnz .Lfull_sub_both
.Lfull_sub_prefix:
   test ecx, ecx
   jz .Lfull_sub_done
.Lfull_sub_carry:
   mov eax, DWORD PTR [r8]
   add rax, r10
   mov DWORD PTR [r8], eax
   sar rax, 32
   mov r10, rax
   sub r8, 4
   dec ecx
   jnz .Lfull_sub_carry
.Lfull_sub_done:
   mov eax, r10d
   xor ecx, ecx
   xor edx, edx
   xor r8d, r8d
   xor r9d, r9d
   xor r10d, r10d
   xor r11d, r11d
   ret
   .cfi_endproc
.size SharkSslBigInt_sub, .-SharkSslBigInt_sub

.p2align 4
.globl SharkSslBigInt_shiftr
.hidden SharkSslBigInt_shiftr
.type SharkSslBigInt_shiftr, @function
SharkSslBigInt_shiftr:
   .cfi_startproc
   mov r8, QWORD PTR [rdi + 8]
   movzx r11d, WORD PTR [rdi + 16]
   mov ecx, r11d
   xor r9d, r9d
   test ecx, ecx
   jz .Lfull_shiftr_done
.Lfull_shiftr_loop:
   mov eax, DWORD PTR [r8]
   mov edx, eax
   shl edx, 31
   shr eax, 1
   or eax, r9d
   mov DWORD PTR [r8], eax
   mov r9d, edx
   add r8, 4
   dec ecx
   jnz .Lfull_shiftr_loop
.Lfull_shiftr_done:
   xor eax, eax
   xor ecx, ecx
   xor edx, edx
   xor r8d, r8d
   xor r9d, r9d
   xor r10d, r10d
   xor r11d, r11d
   ret
   .cfi_endproc
.size SharkSslBigInt_shiftr, .-SharkSslBigInt_shiftr

#endif
.section .note.GNU-stack,"",@progbits
