; 
;     ____             _________                __                _
;    / __ \___  ____ _/ /_  __(_)___ ___  ___  / /   ____  ____ _(_)____
;   / /_/ / _ \/ __ `/ / / / / / __ `__ \/ _ \/ /   / __ \/ __ `/ / ___/
;  / _, _/  __/ /_/ / / / / / / / / / / /  __/ /___/ /_/ / /_/ / / /__
; /_/ |_|\___/\__,_/_/ /_/ /_/_/ /_/ /_/\___/_____/\____/\__, /_/\___/
;                                                       /____/
; 
;                 SharkSSL Embedded SSL/TLS Stack
; ***************************************************************************
;   PROGRAM MODULE
; 
;   $Id: SharkSslBigInt_X64.asm 6818 2026-11-08 21:18:02Z gianluca $
; 
;   COPYRIGHT:  Real Time Logic LLC, 2026
; 
;   This software is copyrighted by and is the sole property of Real
;   Time Logic LLC.  All rights, title, ownership, or other interests in
;   the software remain the property of Real Time Logic LLC.  This
;   software may only be used in accordance with the terms and
;   conditions stipulated in the corresponding license agreement under
;   which the software has been supplied.  Any unauthorized use,
;   duplication, transmission, distribution, or disclosure of this
;   software is expressly forbidden.
; 
;   This Copyright notice may not be removed or modified without prior
;   written consent of Real Time Logic LLC.
; 
;   Real Time Logic LLC. reserves the right to modify this software
;   without notice.
; 
;               http://www.realtimelogic.com
;               http://www.sharkssl.com
; ***************************************************************************
; 
; /
; 
; 
; Original multiple-precision multiplication and Montgomery reduction from
; Handbook of Applied Cryptography, Algorithms 14.12 and 14.32:
; https://cacr.uwaterloo.ca/hac/about/chap14.pdf
; Instruction semantics: Intel 64 and IA-32 Architectures SDM, volume 2.
; No third-party implementation is used.
; 
; Arrays hold native U32 digits most significant first, radix 2^32.
; Lengths, pointer equality and buffer extents are public. The result must
; not overlap an input; the two multiplication inputs may be identical.
; MUL writes alen+blen words. REDC accepts 2*n+1 words containing T,
; 0<=T<mod*R, mod<R=2^(32*n), odd mod and mu=-mod^-1 modulo 2^32.
; Its canonical result occupies t[1..n]; t[0] and t[n+1..2*n] are zero.
; Leading zero digits and odd word counts are supported.
; 
; Even lengths use paired U64 digits, with ROR32 translating native U32
; word order. MUL forms the full U128 product; product+digit+carry fits
; U128. REDC lifts mu by u64=u32*(2+mod_low64*u32) modulo 2^64. Odd
; lengths retain radix 2^32: IMUL receives zero-extended U32 operands,
; and product+digit+carry fits U64. Every carry loop has a public extent.
; Subtraction always runs and uses masked add-back, without a secret
; branch or address. No division, optional CPU feature, SIMD state, call
; or dynamic stack allocation. CT assumes data-independent MUL/IMUL
; latency on the deployment processor; this is not an ISA guarantee.
; Working registers are cleared before saved registers are restored.
; These timing/cache properties do not provide power/EM resistance.
; /

; Win64 argument shims and unwind metadata; arithmetic core matches GNU.

OPTION CASEMAP:NONE
.code

save_regs MACRO
   push rbx
   .pushreg rbx
   push rbp
   .pushreg rbp
   push r12
   .pushreg r12
   push r13
   .pushreg r13
   push r14
   .pushreg r14
   push r15
   .pushreg r15
   push rdi
   .pushreg rdi
   push rsi
   .pushreg rsi
   .endprolog
ENDM

restore_regs MACRO
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
   pop rsi
   pop rdi
   pop r15
   pop r14
   pop r13
   pop r12
   pop rbp
   pop rbx
   ret
ENDM


digit_product MACRO displ
   mov eax, DWORD PTR [rsi - 4 - displ]
   imul rax, r9
   mov edx, DWORD PTR [rdi - displ]
   add rax, rdx
   add rax, r10
   mov DWORD PTR [rdi - displ], eax
   shr rax, 32
   mov r10, rax
ENDM

; A paired U64 digit; ROR32 translates the MSW-first native U32 pair.
pair_product MACRO displ
   mov rax, QWORD PTR [rsi - 8 - displ]
   ror rax, 32
   mul r9
   mov r11, QWORD PTR [rdi - displ]
   ror r11, 32
   add rax, r11
   adc rdx, 0
   add rax, r10
   adc rdx, 0
   mov r10, rdx
   ror rax, 32
   mov QWORD PTR [rdi - displ], rax
ENDM


ALIGN 16
PUBLIC sharkssl_bigint_mul_x64
sharkssl_bigint_mul_x64 PROC FRAME
   save_regs
   mov rdi, rcx
   mov rsi, rdx
   mov rdx, r8
   mov ecx, r9d
   mov r8d, DWORD PTR [rsp + 104]
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
   jz L_mul_done
   test r15d, r15d
   jz L_mul_done
   mov eax, ebp
   or eax, r15d
   test eax, 1
   jz L_mul_pair_start
L_mul_outer:
   mov r9d, DWORD PTR [r14 - 4]
   sub r14, 4
   sub rbx, 4
   mov rdi, rbx
   mov rsi, r13
   mov ecx, ebp
   xor r10d, r10d
   cmp ecx, 4
   jb L_mul_tail
L_mul_four:
   digit_product 0
   digit_product 4
   digit_product 8
   digit_product 12
   sub rsi, 16
   sub rdi, 16
   sub ecx, 4
   cmp ecx, 4
   jae L_mul_four
L_mul_tail:
   test ecx, ecx
   jz L_mul_carry
L_mul_one:
   digit_product 0
   sub rsi, 4
   sub rdi, 4
   dec ecx
   jnz L_mul_one
L_mul_carry:
   mov DWORD PTR [rdi], r10d
   dec r15d
   jnz L_mul_outer
   jmp L_mul_done
L_mul_pair_start:
   shr ebp, 1
   shr r15d, 1
L_mul_pair_outer:
   mov r9, QWORD PTR [r14 - 8]
   ror r9, 32
   sub r14, 8
   sub rbx, 8
   mov rdi, rbx
   mov rsi, r13
   mov ecx, ebp
   xor r10d, r10d
   cmp ecx, 4
   jb L_mul_pair_tail
L_mul_pair_four:
   pair_product 0
   pair_product 8
   pair_product 16
   pair_product 24
   sub rsi, 32
   sub rdi, 32
   sub ecx, 4
   cmp ecx, 4
   jae L_mul_pair_four
L_mul_pair_tail:
   test ecx, ecx
   jz L_mul_pair_carry
L_mul_pair_one:
   pair_product 0
   sub rsi, 8
   sub rdi, 8
   dec ecx
   jnz L_mul_pair_one
L_mul_pair_carry:
   ror r10, 32
   mov QWORD PTR [rdi], r10
   dec r15d
   jnz L_mul_pair_outer
L_mul_done:
   restore_regs
sharkssl_bigint_mul_x64 ENDP

ALIGN 16
PUBLIC sharkssl_bigint_redc_x64
sharkssl_bigint_redc_x64 PROC FRAME
   save_regs
   mov rdi, rcx
   mov rsi, rdx
   mov edx, r8d
   mov ecx, r9d
   mov r12, rdi
   mov ebp, edx
   mov r15d, ecx
   mov r14d, edx
   lea r13, [rsi + rbp*4]
   lea rbx, [rdi + rbp*8]
   add rbx, 4
   test ebp, ebp
   jz L_redc_done
   test ebp, 1
   jz L_redc_pair_start
L_redc_outer:
   sub rbx, 4
   mov r9d, DWORD PTR [rbx]
   imul r9d, r15d
   mov rdi, rbx
   mov rsi, r13
   mov ecx, ebp
   xor r10d, r10d
   cmp ecx, 4
   jb L_redc_tail
L_redc_four:
   digit_product 0
   digit_product 4
   digit_product 8
   digit_product 12
   sub rsi, 16
   sub rdi, 16
   sub ecx, 4
   cmp ecx, 4
   jae L_redc_four
L_redc_tail:
   test ecx, ecx
   jz L_redc_carry_start
L_redc_one:
   digit_product 0
   sub rsi, 4
   sub rdi, 4
   dec ecx
   jnz L_redc_one
L_redc_carry_start:
   lea ecx, [r14 + 1]
   add DWORD PTR [rdi], r10d
   lea rdi, [rdi - 4]
   dec ecx
L_redc_carry:
   adc DWORD PTR [rdi], 0
   lea rdi, [rdi - 4]
   dec ecx
   jnz L_redc_carry
   dec r14d
   jnz L_redc_outer

; Subtract the modulus over n+1 digits and retain the final borrow mask.
   lea rdi, [r12 + rbp*4]
   mov rsi, r13
   mov ecx, ebp
   xor r10d, r10d
L_redc_subtract:
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
   jnz L_redc_subtract
   mov eax, DWORD PTR [r12]
   sub rax, r10
   sar rax, 63
   mov r11d, eax

; Add back mod & mask in a fixed n-digit traversal.
   lea rdi, [r12 + rbp*4]
   mov rsi, r13
   mov ecx, ebp
   xor r10d, r10d
L_redc_addback:
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
   jnz L_redc_addback
   mov DWORD PTR [r12], 0
   jmp L_redc_done

; Even n: pair digits without changing R=2^(32*n) or the U32 ABI.
L_redc_pair_start:
   mov rax, QWORD PTR [r13 - 8]
   ror rax, 32
   imul rax, r15
   add rax, 2
   imul r15, rax
   shr ebp, 1
   shr r14d, 1
L_redc_pair_outer:
   sub rbx, 8
   mov r9, QWORD PTR [rbx]
   ror r9, 32
   imul r9, r15
   mov rdi, rbx
   mov rsi, r13
   mov ecx, ebp
   xor r10d, r10d
   cmp ecx, 4
   jb L_redc_pair_tail
L_redc_pair_four:
   pair_product 0
   pair_product 8
   pair_product 16
   pair_product 24
   sub rsi, 32
   sub rdi, 32
   sub ecx, 4
   cmp ecx, 4
   jae L_redc_pair_four
L_redc_pair_tail:
   test ecx, ecx
   jz L_redc_pair_carry_start
L_redc_pair_one:
   pair_product 0
   sub rsi, 8
   sub rdi, 8
   dec ecx
   jnz L_redc_pair_one
L_redc_pair_carry_start:
   mov ecx, r14d
L_redc_pair_carry:
   mov rax, QWORD PTR [rdi]
   ror rax, 32
   add rax, r10
   setc r10b
   movzx r10d, r10b
   ror rax, 32
   mov QWORD PTR [rdi], rax
   sub rdi, 8
   dec ecx
   jnz L_redc_pair_carry
   add DWORD PTR [r12], r10d
   dec r14d
   jnz L_redc_pair_outer

; Full U64 subtraction uses the borrow flag, not signed headroom.
   lea rdi, [r12 + rbp*8 - 4]
   mov rsi, r13
   mov ecx, ebp
   xor r10d, r10d
L_redc_pair_subtract:
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
   jnz L_redc_pair_subtract
   mov eax, DWORD PTR [r12]
   sub rax, r10
   sar rax, 63
   mov r11, rax

; Masked add-back likewise preserves the full U64 carry explicitly.
   lea rdi, [r12 + rbp*8 - 4]
   mov rsi, r13
   mov ecx, ebp
   xor r10d, r10d
L_redc_pair_addback:
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
   jnz L_redc_pair_addback
   mov DWORD PTR [r12], 0
L_redc_done:
   restore_regs
sharkssl_bigint_redc_x64 ENDP


ALIGN 16
PUBLIC SharkSslBigInt_add
SharkSslBigInt_add PROC
   mov r8, QWORD PTR [rcx + 8]
   movzx r11d, WORD PTR [rcx + 16]
   mov r9, QWORD PTR [rdx + 8]
   movzx edx, WORD PTR [rdx + 16]
   mov ecx, r11d
   lea r8, [r8 + rcx*4 - 4]
   lea r9, [r9 + rdx*4 - 4]
   cmp edx, ecx
   cmova edx, ecx
   sub ecx, edx
   xor r10d, r10d
   test edx, edx
   jz full_add_prefix
full_add_both:
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
   jnz full_add_both
full_add_prefix:
   test ecx, ecx
   jz full_add_done
full_add_carry:
   mov eax, DWORD PTR [r8]
   add rax, r10
   mov DWORD PTR [r8], eax
   shr rax, 32
   mov r10, rax
   sub r8, 4
   dec ecx
   jnz full_add_carry
full_add_done:
   mov eax, r10d
   xor ecx, ecx
   xor edx, edx
   xor r8d, r8d
   xor r9d, r9d
   xor r10d, r10d
   xor r11d, r11d
   ret
SharkSslBigInt_add ENDP

ALIGN 16
PUBLIC SharkSslBigInt_sub
SharkSslBigInt_sub PROC
   mov r8, QWORD PTR [rcx + 8]
   movzx r11d, WORD PTR [rcx + 16]
   mov r9, QWORD PTR [rdx + 8]
   movzx edx, WORD PTR [rdx + 16]
   mov ecx, r11d
   lea r8, [r8 + rcx*4 - 4]
   lea r9, [r9 + rdx*4 - 4]
   cmp edx, ecx
   cmova edx, ecx
   sub ecx, edx
   xor r10d, r10d
   test edx, edx
   jz full_sub_prefix
full_sub_both:
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
   jnz full_sub_both
full_sub_prefix:
   test ecx, ecx
   jz full_sub_done
full_sub_carry:
   mov eax, DWORD PTR [r8]
   add rax, r10
   mov DWORD PTR [r8], eax
   sar rax, 32
   mov r10, rax
   sub r8, 4
   dec ecx
   jnz full_sub_carry
full_sub_done:
   mov eax, r10d
   xor ecx, ecx
   xor edx, edx
   xor r8d, r8d
   xor r9d, r9d
   xor r10d, r10d
   xor r11d, r11d
   ret
SharkSslBigInt_sub ENDP

ALIGN 16
PUBLIC SharkSslBigInt_shiftr
SharkSslBigInt_shiftr PROC
   mov r8, QWORD PTR [rcx + 8]
   movzx r11d, WORD PTR [rcx + 16]
   mov ecx, r11d
   xor r9d, r9d
   test ecx, ecx
   jz full_shiftr_done
full_shiftr_loop:
   mov eax, DWORD PTR [r8]
   mov edx, eax
   shl edx, 31
   shr eax, 1
   or eax, r9d
   mov DWORD PTR [r8], eax
   mov r9d, edx
   add r8, 4
   dec ecx
   jnz full_shiftr_loop
full_shiftr_done:
   xor eax, eax
   xor ecx, ecx
   xor edx, edx
   xor r8d, r8d
   xor r9d, r9d
   xor r10d, r10d
   xor r11d, r11d
   ret
SharkSslBigInt_shiftr ENDP

END
