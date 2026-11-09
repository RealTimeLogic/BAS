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
;   $Id: SharkSslP384_X64.asm 6818 2026-11-08 21:18:02Z gianluca $
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
; 

; 


; Original P-384 field arithmetic from NIST SP 800-186 parameters,
; p = 2^384 - 2^128 - 2^96 + 2^32 - 1.
; https://doi.org/10.6028/NIST.SP.800-186
; No third-party implementation is used.
; 
; Raw operands/results: twelve native U32 words, most significant first,
; canonical in [0,p). Full inputs are loaded before stores: exact aliases
; r == a, r == b and a == b work. All schedules/addresses are fixed.
; Multiply requires BMI2 MULX; callers must probe before entering.
; Add/sub use baseline x64 instructions; no vector state is used.
; 
; Six U64 limbs form the product. Reduction folds radix-2^32 digits with
; x^12 = x^4 + x^3 - x + 1, then normalizes signed U64 coefficients.
; Coefficient bounds in multiples of (2^32-1):
; [-1,4],[-2,4],[-2,3],[-3,5],[-3,8],[-1,8],
; [-1,7],[-1,5],[-1,4],[-1,4],[-1,4],[-1,4].
; Normalized carry is in [-2,4]. Set R=2^384 and Q=R-p; 5Q<R.
; A first carry fold can wrap only once, leaving low<4Q or low>=R-2Q.
; A second fold by +/-Q therefore cannot wrap. With low<R<2p, a single
; register-conditional subtraction produces the canonical residue.
; 
; Written scratch and working registers are erased before saved registers
; are restored. This does not constitute a power/EM countermeasure.

OPTION CASEMAP:NONE
.code

ALIGN 16
PUBLIC sharkssl_p384_mulmod64
sharkssl_p384_mulmod64 PROC FRAME
   push rdi
   .pushreg rdi
   push rsi
   .pushreg rsi
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
   sub rsp, 200
   .allocstack 200
   .endprolog
   mov rdi, rcx
   mov rsi, rdx
   mov rdx, r8
   mov rcx, rdx
   mov rax, QWORD PTR [rsi + 40]
   ror rax, 32
   mov QWORD PTR [rsp + 96], rax
   mov rax, QWORD PTR [rcx + 40]
   ror rax, 32
   mov QWORD PTR [rsp + 144], rax
   mov rax, QWORD PTR [rsi + 32]
   ror rax, 32
   mov QWORD PTR [rsp + 104], rax
   mov rax, QWORD PTR [rcx + 32]
   ror rax, 32
   mov QWORD PTR [rsp + 152], rax
   mov rax, QWORD PTR [rsi + 24]
   ror rax, 32
   mov QWORD PTR [rsp + 112], rax
   mov rax, QWORD PTR [rcx + 24]
   ror rax, 32
   mov QWORD PTR [rsp + 160], rax
   mov rax, QWORD PTR [rsi + 16]
   ror rax, 32
   mov QWORD PTR [rsp + 120], rax
   mov rax, QWORD PTR [rcx + 16]
   ror rax, 32
   mov QWORD PTR [rsp + 168], rax
   mov rax, QWORD PTR [rsi + 8]
   ror rax, 32
   mov QWORD PTR [rsp + 128], rax
   mov rax, QWORD PTR [rcx + 8]
   ror rax, 32
   mov QWORD PTR [rsp + 176], rax
   mov rax, QWORD PTR [rsi + 0]
   ror rax, 32
   mov QWORD PTR [rsp + 136], rax
   mov rax, QWORD PTR [rcx + 0]
   ror rax, 32
   mov QWORD PTR [rsp + 184], rax
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
   mov rdx, QWORD PTR [rsp + 144]
   xor ebp, ebp
   mulx r10, r9, QWORD PTR [rsp + 96]
   add r9, QWORD PTR [rsp + 0]
   adc r10, 0
   add r9, rbp
   adc r10, 0
   mov QWORD PTR [rsp + 0], r9
   mov rbp, r10
   mulx r10, r9, QWORD PTR [rsp + 104]
   add r9, QWORD PTR [rsp + 8]
   adc r10, 0
   add r9, rbp
   adc r10, 0
   mov QWORD PTR [rsp + 8], r9
   mov rbp, r10
   mulx r10, r9, QWORD PTR [rsp + 112]
   add r9, QWORD PTR [rsp + 16]
   adc r10, 0
   add r9, rbp
   adc r10, 0
   mov QWORD PTR [rsp + 16], r9
   mov rbp, r10
   mulx r10, r9, QWORD PTR [rsp + 120]
   add r9, QWORD PTR [rsp + 24]
   adc r10, 0
   add r9, rbp
   adc r10, 0
   mov QWORD PTR [rsp + 24], r9
   mov rbp, r10
   mulx r10, r9, QWORD PTR [rsp + 128]
   add r9, QWORD PTR [rsp + 32]
   adc r10, 0
   add r9, rbp
   adc r10, 0
   mov QWORD PTR [rsp + 32], r9
   mov rbp, r10
   mulx r10, r9, QWORD PTR [rsp + 136]
   add r9, QWORD PTR [rsp + 40]
   adc r10, 0
   add r9, rbp
   adc r10, 0
   mov QWORD PTR [rsp + 40], r9
   mov rbp, r10
   mov QWORD PTR [rsp + 48], rbp
   mov rdx, QWORD PTR [rsp + 152]
   xor ebp, ebp
   mulx r10, r9, QWORD PTR [rsp + 96]
   add r9, QWORD PTR [rsp + 8]
   adc r10, 0
   add r9, rbp
   adc r10, 0
   mov QWORD PTR [rsp + 8], r9
   mov rbp, r10
   mulx r10, r9, QWORD PTR [rsp + 104]
   add r9, QWORD PTR [rsp + 16]
   adc r10, 0
   add r9, rbp
   adc r10, 0
   mov QWORD PTR [rsp + 16], r9
   mov rbp, r10
   mulx r10, r9, QWORD PTR [rsp + 112]
   add r9, QWORD PTR [rsp + 24]
   adc r10, 0
   add r9, rbp
   adc r10, 0
   mov QWORD PTR [rsp + 24], r9
   mov rbp, r10
   mulx r10, r9, QWORD PTR [rsp + 120]
   add r9, QWORD PTR [rsp + 32]
   adc r10, 0
   add r9, rbp
   adc r10, 0
   mov QWORD PTR [rsp + 32], r9
   mov rbp, r10
   mulx r10, r9, QWORD PTR [rsp + 128]
   add r9, QWORD PTR [rsp + 40]
   adc r10, 0
   add r9, rbp
   adc r10, 0
   mov QWORD PTR [rsp + 40], r9
   mov rbp, r10
   mulx r10, r9, QWORD PTR [rsp + 136]
   add r9, QWORD PTR [rsp + 48]
   adc r10, 0
   add r9, rbp
   adc r10, 0
   mov QWORD PTR [rsp + 48], r9
   mov rbp, r10
   mov QWORD PTR [rsp + 56], rbp
   mov rdx, QWORD PTR [rsp + 160]
   xor ebp, ebp
   mulx r10, r9, QWORD PTR [rsp + 96]
   add r9, QWORD PTR [rsp + 16]
   adc r10, 0
   add r9, rbp
   adc r10, 0
   mov QWORD PTR [rsp + 16], r9
   mov rbp, r10
   mulx r10, r9, QWORD PTR [rsp + 104]
   add r9, QWORD PTR [rsp + 24]
   adc r10, 0
   add r9, rbp
   adc r10, 0
   mov QWORD PTR [rsp + 24], r9
   mov rbp, r10
   mulx r10, r9, QWORD PTR [rsp + 112]
   add r9, QWORD PTR [rsp + 32]
   adc r10, 0
   add r9, rbp
   adc r10, 0
   mov QWORD PTR [rsp + 32], r9
   mov rbp, r10
   mulx r10, r9, QWORD PTR [rsp + 120]
   add r9, QWORD PTR [rsp + 40]
   adc r10, 0
   add r9, rbp
   adc r10, 0
   mov QWORD PTR [rsp + 40], r9
   mov rbp, r10
   mulx r10, r9, QWORD PTR [rsp + 128]
   add r9, QWORD PTR [rsp + 48]
   adc r10, 0
   add r9, rbp
   adc r10, 0
   mov QWORD PTR [rsp + 48], r9
   mov rbp, r10
   mulx r10, r9, QWORD PTR [rsp + 136]
   add r9, QWORD PTR [rsp + 56]
   adc r10, 0
   add r9, rbp
   adc r10, 0
   mov QWORD PTR [rsp + 56], r9
   mov rbp, r10
   mov QWORD PTR [rsp + 64], rbp
   mov rdx, QWORD PTR [rsp + 168]
   xor ebp, ebp
   mulx r10, r9, QWORD PTR [rsp + 96]
   add r9, QWORD PTR [rsp + 24]
   adc r10, 0
   add r9, rbp
   adc r10, 0
   mov QWORD PTR [rsp + 24], r9
   mov rbp, r10
   mulx r10, r9, QWORD PTR [rsp + 104]
   add r9, QWORD PTR [rsp + 32]
   adc r10, 0
   add r9, rbp
   adc r10, 0
   mov QWORD PTR [rsp + 32], r9
   mov rbp, r10
   mulx r10, r9, QWORD PTR [rsp + 112]
   add r9, QWORD PTR [rsp + 40]
   adc r10, 0
   add r9, rbp
   adc r10, 0
   mov QWORD PTR [rsp + 40], r9
   mov rbp, r10
   mulx r10, r9, QWORD PTR [rsp + 120]
   add r9, QWORD PTR [rsp + 48]
   adc r10, 0
   add r9, rbp
   adc r10, 0
   mov QWORD PTR [rsp + 48], r9
   mov rbp, r10
   mulx r10, r9, QWORD PTR [rsp + 128]
   add r9, QWORD PTR [rsp + 56]
   adc r10, 0
   add r9, rbp
   adc r10, 0
   mov QWORD PTR [rsp + 56], r9
   mov rbp, r10
   mulx r10, r9, QWORD PTR [rsp + 136]
   add r9, QWORD PTR [rsp + 64]
   adc r10, 0
   add r9, rbp
   adc r10, 0
   mov QWORD PTR [rsp + 64], r9
   mov rbp, r10
   mov QWORD PTR [rsp + 72], rbp
   mov rdx, QWORD PTR [rsp + 176]
   xor ebp, ebp
   mulx r10, r9, QWORD PTR [rsp + 96]
   add r9, QWORD PTR [rsp + 32]
   adc r10, 0
   add r9, rbp
   adc r10, 0
   mov QWORD PTR [rsp + 32], r9
   mov rbp, r10
   mulx r10, r9, QWORD PTR [rsp + 104]
   add r9, QWORD PTR [rsp + 40]
   adc r10, 0
   add r9, rbp
   adc r10, 0
   mov QWORD PTR [rsp + 40], r9
   mov rbp, r10
   mulx r10, r9, QWORD PTR [rsp + 112]
   add r9, QWORD PTR [rsp + 48]
   adc r10, 0
   add r9, rbp
   adc r10, 0
   mov QWORD PTR [rsp + 48], r9
   mov rbp, r10
   mulx r10, r9, QWORD PTR [rsp + 120]
   add r9, QWORD PTR [rsp + 56]
   adc r10, 0
   add r9, rbp
   adc r10, 0
   mov QWORD PTR [rsp + 56], r9
   mov rbp, r10
   mulx r10, r9, QWORD PTR [rsp + 128]
   add r9, QWORD PTR [rsp + 64]
   adc r10, 0
   add r9, rbp
   adc r10, 0
   mov QWORD PTR [rsp + 64], r9
   mov rbp, r10
   mulx r10, r9, QWORD PTR [rsp + 136]
   add r9, QWORD PTR [rsp + 72]
   adc r10, 0
   add r9, rbp
   adc r10, 0
   mov QWORD PTR [rsp + 72], r9
   mov rbp, r10
   mov QWORD PTR [rsp + 80], rbp
   mov rdx, QWORD PTR [rsp + 184]
   xor ebp, ebp
   mulx r10, r9, QWORD PTR [rsp + 96]
   add r9, QWORD PTR [rsp + 40]
   adc r10, 0
   add r9, rbp
   adc r10, 0
   mov QWORD PTR [rsp + 40], r9
   mov rbp, r10
   mulx r10, r9, QWORD PTR [rsp + 104]
   add r9, QWORD PTR [rsp + 48]
   adc r10, 0
   add r9, rbp
   adc r10, 0
   mov QWORD PTR [rsp + 48], r9
   mov rbp, r10
   mulx r10, r9, QWORD PTR [rsp + 112]
   add r9, QWORD PTR [rsp + 56]
   adc r10, 0
   add r9, rbp
   adc r10, 0
   mov QWORD PTR [rsp + 56], r9
   mov rbp, r10
   mulx r10, r9, QWORD PTR [rsp + 120]
   add r9, QWORD PTR [rsp + 64]
   adc r10, 0
   add r9, rbp
   adc r10, 0
   mov QWORD PTR [rsp + 64], r9
   mov rbp, r10
   mulx r10, r9, QWORD PTR [rsp + 128]
   add r9, QWORD PTR [rsp + 72]
   adc r10, 0
   add r9, rbp
   adc r10, 0
   mov QWORD PTR [rsp + 72], r9
   mov rbp, r10
   mulx r10, r9, QWORD PTR [rsp + 136]
   add r9, QWORD PTR [rsp + 80]
   adc r10, 0
   add r9, rbp
   adc r10, 0
   mov QWORD PTR [rsp + 80], r9
   mov rbp, r10
   mov QWORD PTR [rsp + 88], rbp
   mov r8d, DWORD PTR [rsp + 0]
   mov r9d, DWORD PTR [rsp + 4]
   mov r10d, DWORD PTR [rsp + 8]
   mov r11d, DWORD PTR [rsp + 12]
   mov r12d, DWORD PTR [rsp + 16]
   mov r13d, DWORD PTR [rsp + 20]
   mov r14d, DWORD PTR [rsp + 24]
   mov r15d, DWORD PTR [rsp + 28]
   mov ebx, DWORD PTR [rsp + 32]
   mov ebp, DWORD PTR [rsp + 36]
   mov ecx, DWORD PTR [rsp + 40]
   mov edx, DWORD PTR [rsp + 44]
   mov eax, DWORD PTR [rsp + 48]
   add r8, rax
   sub r9, rax
   add r11, rax
   add r12, rax
   mov eax, DWORD PTR [rsp + 52]
   add r9, rax
   sub r10, rax
   add r12, rax
   add r13, rax
   mov eax, DWORD PTR [rsp + 56]
   add r10, rax
   sub r11, rax
   add r13, rax
   add r14, rax
   mov eax, DWORD PTR [rsp + 60]
   add r11, rax
   sub r12, rax
   add r14, rax
   add r15, rax
   mov eax, DWORD PTR [rsp + 64]
   add r12, rax
   sub r13, rax
   add r15, rax
   add rbx, rax
   mov eax, DWORD PTR [rsp + 68]
   add r13, rax
   sub r14, rax
   add rbx, rax
   add rbp, rax
   mov eax, DWORD PTR [rsp + 72]
   add r14, rax
   sub r15, rax
   add rbp, rax
   add rcx, rax
   mov eax, DWORD PTR [rsp + 76]
   add r15, rax
   sub rbx, rax
   add rcx, rax
   add rdx, rax
   mov eax, DWORD PTR [rsp + 80]
   add r8, rax
   sub r9, rax
   add r11, rax
   add r12, rax
   add rbx, rax
   sub rbp, rax
   add rdx, rax
   mov eax, DWORD PTR [rsp + 84]
   add r8, rax
   sub r10, rax
   add r11, rax
   add r12, rax
   add r12, rax
   add r13, rax
   add rbp, rax
   sub rcx, rax
   mov eax, DWORD PTR [rsp + 88]
   add r9, rax
   sub r11, rax
   add r12, rax
   add r13, rax
   add r13, rax
   add r14, rax
   add rcx, rax
   sub rdx, rax
   mov eax, DWORD PTR [rsp + 92]
   sub r8, rax
   add r9, rax
   add r10, rax
   sub r11, rax
   sub r12, rax
   sub r12, rax
   add r13, rax
   add r14, rax
   add r14, rax
   add r15, rax
   add rdx, rax
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
   add rbx, rax
   mov rax, rbx
   sar rax, 32
   mov ebx, ebx
   add rbp, rax
   mov rax, rbp
   sar rax, 32
   mov ebp, ebp
   add rcx, rax
   mov rax, rcx
   sar rax, 32
   mov ecx, ecx
   add rdx, rax
   mov rax, rdx
   sar rax, 32
   mov edx, edx
   add r8, rax
   sub r9, rax
   add r11, rax
   add r12, rax
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
   add rbx, rax
   mov rax, rbx
   sar rax, 32
   mov ebx, ebx
   add rbp, rax
   mov rax, rbp
   sar rax, 32
   mov ebp, ebp
   add rcx, rax
   mov rax, rcx
   sar rax, 32
   mov ecx, ecx
   add rdx, rax
   mov rax, rdx
   sar rax, 32
   mov edx, edx
   add r8, rax
   sub r9, rax
   add r11, rax
   add r12, rax
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
   add rbx, rax
   mov rax, rbx
   sar rax, 32
   mov ebx, ebx
   add rbp, rax
   mov rax, rbp
   sar rax, 32
   mov ebp, ebp
   add rcx, rax
   mov rax, rcx
   sar rax, 32
   mov ecx, ecx
   add rdx, rax
   mov rax, rdx
   sar rax, 32
   mov edx, edx
   shl r9, 32
   or r8, r9
   shl r11, 32
   or r10, r11
   shl r13, 32
   or r12, r13
   shl r15, 32
   or r14, r15
   shl rbp, 32
   or rbx, rbp
   shl rdx, 32
   or rcx, rdx
   mov r9, r8
   mov r11, r10
   mov r13, r12
   mov r15, r14
   mov rbp, rbx
   mov rdx, rcx
   mov rsi, 0ffffffffh
   sub r9, rsi
   mov rsi, 0ffffffff00000000h
   sbb r11, rsi
   mov rsi, -2
   sbb r13, rsi
   mov rsi, -1
   sbb r15, rsi
   mov rsi, -1
   sbb rbp, rsi
   mov rsi, -1
   sbb rdx, rsi
   cmovnc r8, r9
   cmovnc r10, r11
   cmovnc r12, r13
   cmovnc r14, r15
   cmovnc rbx, rbp
   cmovnc rcx, rdx
   ror r8, 32
   mov QWORD PTR [rdi + 40], r8
   ror r10, 32
   mov QWORD PTR [rdi + 32], r10
   ror r12, 32
   mov QWORD PTR [rdi + 24], r12
   ror r14, 32
   mov QWORD PTR [rdi + 16], r14
   ror rbx, 32
   mov QWORD PTR [rdi + 8], rbx
   ror rcx, 32
   mov QWORD PTR [rdi + 0], rcx
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
   mov QWORD PTR [rsp + 128], rax
   mov QWORD PTR [rsp + 136], rax
   mov QWORD PTR [rsp + 144], rax
   mov QWORD PTR [rsp + 152], rax
   mov QWORD PTR [rsp + 160], rax
   mov QWORD PTR [rsp + 168], rax
   mov QWORD PTR [rsp + 176], rax
   mov QWORD PTR [rsp + 184], rax
   xor ecx, ecx
   xor edx, edx
   xor esi, esi
   xor ebx, ebx
   xor ebp, ebp
   xor r8d, r8d
   xor r9d, r9d
   xor r10d, r10d
   xor r11d, r11d
   xor r12d, r12d
   xor r13d, r13d
   xor r14d, r14d
   xor r15d, r15d
   add rsp, 200
   pop r15
   pop r14
   pop r13
   pop r12
   pop rbp
   pop rbx
   pop rsi
   pop rdi
   ret
sharkssl_p384_mulmod64 ENDP

ALIGN 16
PUBLIC sharkssl_p384_addmod64
sharkssl_p384_addmod64 PROC FRAME
   push rdi
   .pushreg rdi
   push rsi
   .pushreg rsi
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
   sub rsp, 8
   .allocstack 8
   .endprolog
   mov rdi, rcx
   mov rsi, rdx
   mov rdx, r8
   mov r8, QWORD PTR [rsi + 40]
   ror r8, 32
   mov r9, QWORD PTR [rsi + 32]
   ror r9, 32
   mov r10, QWORD PTR [rsi + 24]
   ror r10, 32
   mov r11, QWORD PTR [rsi + 16]
   ror r11, 32
   mov r12, QWORD PTR [rsi + 8]
   ror r12, 32
   mov r13, QWORD PTR [rsi + 0]
   ror r13, 32
   mov r14, QWORD PTR [rdx + 40]
   ror r14, 32
   mov r15, QWORD PTR [rdx + 32]
   ror r15, 32
   mov rbx, QWORD PTR [rdx + 24]
   ror rbx, 32
   mov rbp, QWORD PTR [rdx + 16]
   ror rbp, 32
   mov rcx, QWORD PTR [rdx + 8]
   ror rcx, 32
   mov rax, QWORD PTR [rdx + 0]
   ror rax, 32
   add r8, r14
   adc r9, r15
   adc r10, rbx
   adc r11, rbp
   adc r12, rcx
   adc r13, rax
   mov edx, 0
   adc rdx, 0
   mov r14, r8
   mov r15, r9
   mov rbx, r10
   mov rbp, r11
   mov rcx, r12
   mov rax, r13
   mov rsi, 0ffffffffh
   sub r14, rsi
   mov rsi, 0ffffffff00000000h
   sbb r15, rsi
   mov rsi, -2
   sbb rbx, rsi
   mov rsi, -1
   sbb rbp, rsi
   mov rsi, -1
   sbb rcx, rsi
   mov rsi, -1
   sbb rax, rsi
   sbb rdx, 0
   cmovnc r8, r14
   cmovnc r9, r15
   cmovnc r10, rbx
   cmovnc r11, rbp
   cmovnc r12, rcx
   cmovnc r13, rax
   ror r8, 32
   mov QWORD PTR [rdi + 40], r8
   ror r9, 32
   mov QWORD PTR [rdi + 32], r9
   ror r10, 32
   mov QWORD PTR [rdi + 24], r10
   ror r11, 32
   mov QWORD PTR [rdi + 16], r11
   ror r12, 32
   mov QWORD PTR [rdi + 8], r12
   ror r13, 32
   mov QWORD PTR [rdi + 0], r13
   xor eax, eax
   xor ecx, ecx
   xor edx, edx
   xor esi, esi
   xor ebx, ebx
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
   pop r15
   pop r14
   pop r13
   pop r12
   pop rbp
   pop rbx
   pop rsi
   pop rdi
   ret
sharkssl_p384_addmod64 ENDP

ALIGN 16
PUBLIC sharkssl_p384_submod64
sharkssl_p384_submod64 PROC FRAME
   push rdi
   .pushreg rdi
   push rsi
   .pushreg rsi
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
   sub rsp, 8
   .allocstack 8
   .endprolog
   mov rdi, rcx
   mov rsi, rdx
   mov rdx, r8
   mov r8, QWORD PTR [rsi + 40]
   ror r8, 32
   mov r9, QWORD PTR [rsi + 32]
   ror r9, 32
   mov r10, QWORD PTR [rsi + 24]
   ror r10, 32
   mov r11, QWORD PTR [rsi + 16]
   ror r11, 32
   mov r12, QWORD PTR [rsi + 8]
   ror r12, 32
   mov r13, QWORD PTR [rsi + 0]
   ror r13, 32
   mov r14, QWORD PTR [rdx + 40]
   ror r14, 32
   mov r15, QWORD PTR [rdx + 32]
   ror r15, 32
   mov rbx, QWORD PTR [rdx + 24]
   ror rbx, 32
   mov rbp, QWORD PTR [rdx + 16]
   ror rbp, 32
   mov rcx, QWORD PTR [rdx + 8]
   ror rcx, 32
   mov rax, QWORD PTR [rdx + 0]
   ror rax, 32
   sub r8, r14
   sbb r9, r15
   sbb r10, rbx
   sbb r11, rbp
   sbb r12, rcx
   sbb r13, rax
   sbb rdx, rdx
   mov r14d, 0ffffffffh
   and r14, rdx
   mov r15, 0ffffffff00000000h
   and r15, rdx
   mov rbx, -2
   and rbx, rdx
   add r8, r14
   adc r9, r15
   adc r10, rbx
   adc r11, rdx
   adc r12, rdx
   adc r13, rdx
   ror r8, 32
   mov QWORD PTR [rdi + 40], r8
   ror r9, 32
   mov QWORD PTR [rdi + 32], r9
   ror r10, 32
   mov QWORD PTR [rdi + 24], r10
   ror r11, 32
   mov QWORD PTR [rdi + 16], r11
   ror r12, 32
   mov QWORD PTR [rdi + 8], r12
   ror r13, 32
   mov QWORD PTR [rdi + 0], r13
   xor eax, eax
   xor ecx, ecx
   xor edx, edx
   xor esi, esi
   xor ebx, ebx
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
   pop r15
   pop r14
   pop r13
   pop r12
   pop rbp
   pop rbx
   pop rsi
   pop rdi
   ret
sharkssl_p384_submod64 ENDP

ALIGN 16
PUBLIC sharkssl_p384_hw_probe
sharkssl_p384_hw_probe PROC FRAME
   push rbx
   .pushreg rbx
   .endprolog
   xor eax, eax
   cpuid
   cmp eax, 7
   jb no_p384_bmi2
   mov eax, 7
   xor ecx, ecx
   cpuid
   mov eax, ebx
   shr eax, 8
   and eax, 1
   pop rbx
   ret
no_p384_bmi2:
   xor eax, eax
   pop rbx
   ret
sharkssl_p384_hw_probe ENDP
END
