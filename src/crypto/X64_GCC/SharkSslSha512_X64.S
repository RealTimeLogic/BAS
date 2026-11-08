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
 *   $Id: SharkSslSha512_X64.S 6795 2026-11-07 22:37:57Z gianluca $
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
 * Original SHA-384/SHA-512 compression derived from FIPS 180-4 section 6.4.2.
 * https://doi.org/10.6028/NIST.FIPS.180-4
 * RORX semantics: Intel 64 and IA-32 Architectures SDM, instruction reference.
 * No third-party implementation is used. One 128-byte block; native U64 state
 * at context byte offset 16. The caller passes the existing 80 FIPS constants.
 * Requires BMI2 (CPUID.7.0:EBX[8]); no vector or extended OS state is used.
 * All 80 rounds and schedule accesses are fixed. Only the public CPU probe
 * branches. Written W[16] scratch and working registers are erased on return.
 * Input, context and constants require no vector alignment.
 */
.intel_syntax noprefix
.text
.p2align 4
.globl SharkSslSha384Ctx_processBMI2
.type SharkSslSha384Ctx_processBMI2, @function
SharkSslSha384Ctx_processBMI2:
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
   mov rbx, rdx
   mov rax, QWORD PTR [rsi + 0]
   bswap rax
   mov QWORD PTR [rsp + 0], rax
   mov rax, QWORD PTR [rsi + 8]
   bswap rax
   mov QWORD PTR [rsp + 8], rax
   mov rax, QWORD PTR [rsi + 16]
   bswap rax
   mov QWORD PTR [rsp + 16], rax
   mov rax, QWORD PTR [rsi + 24]
   bswap rax
   mov QWORD PTR [rsp + 24], rax
   mov rax, QWORD PTR [rsi + 32]
   bswap rax
   mov QWORD PTR [rsp + 32], rax
   mov rax, QWORD PTR [rsi + 40]
   bswap rax
   mov QWORD PTR [rsp + 40], rax
   mov rax, QWORD PTR [rsi + 48]
   bswap rax
   mov QWORD PTR [rsp + 48], rax
   mov rax, QWORD PTR [rsi + 56]
   bswap rax
   mov QWORD PTR [rsp + 56], rax
   mov rax, QWORD PTR [rsi + 64]
   bswap rax
   mov QWORD PTR [rsp + 64], rax
   mov rax, QWORD PTR [rsi + 72]
   bswap rax
   mov QWORD PTR [rsp + 72], rax
   mov rax, QWORD PTR [rsi + 80]
   bswap rax
   mov QWORD PTR [rsp + 80], rax
   mov rax, QWORD PTR [rsi + 88]
   bswap rax
   mov QWORD PTR [rsp + 88], rax
   mov rax, QWORD PTR [rsi + 96]
   bswap rax
   mov QWORD PTR [rsp + 96], rax
   mov rax, QWORD PTR [rsi + 104]
   bswap rax
   mov QWORD PTR [rsp + 104], rax
   mov rax, QWORD PTR [rsi + 112]
   bswap rax
   mov QWORD PTR [rsp + 112], rax
   mov rax, QWORD PTR [rsi + 120]
   bswap rax
   mov QWORD PTR [rsp + 120], rax
   mov r8, QWORD PTR [rdi + 16]
   mov r9, QWORD PTR [rdi + 24]
   mov r10, QWORD PTR [rdi + 32]
   mov r11, QWORD PTR [rdi + 40]
   mov r12, QWORD PTR [rdi + 48]
   mov r13, QWORD PTR [rdi + 56]
   mov r14, QWORD PTR [rdi + 64]
   mov r15, QWORD PTR [rdi + 72]
   /* FIPS 180-4 round 0. */
   mov rax, QWORD PTR [rsp + 0]
   add r15, rax
   add r15, QWORD PTR [rbx + 0]
   rorx rax, r12, 14
   rorx rcx, r12, 18
   rorx rdx, r12, 41
   xor rax, rcx
   xor rax, rdx
   add r15, rax
   mov rax, r13
   xor rax, r14
   and rax, r12
   xor rax, r14
   add r15, rax
   add r11, r15
   rorx rax, r8, 28
   rorx rcx, r8, 34
   rorx rdx, r8, 39
   xor rax, rcx
   xor rax, rdx
   add r15, rax
   mov rax, r8
   and rax, r9
   mov rcx, r8
   or rcx, r9
   and rcx, r10
   or rax, rcx
   add r15, rax
   /* FIPS 180-4 round 1. */
   mov rax, QWORD PTR [rsp + 8]
   add r14, rax
   add r14, QWORD PTR [rbx + 8]
   rorx rax, r11, 14
   rorx rcx, r11, 18
   rorx rdx, r11, 41
   xor rax, rcx
   xor rax, rdx
   add r14, rax
   mov rax, r12
   xor rax, r13
   and rax, r11
   xor rax, r13
   add r14, rax
   add r10, r14
   rorx rax, r15, 28
   rorx rcx, r15, 34
   rorx rdx, r15, 39
   xor rax, rcx
   xor rax, rdx
   add r14, rax
   mov rax, r15
   and rax, r8
   mov rcx, r15
   or rcx, r8
   and rcx, r9
   or rax, rcx
   add r14, rax
   /* FIPS 180-4 round 2. */
   mov rax, QWORD PTR [rsp + 16]
   add r13, rax
   add r13, QWORD PTR [rbx + 16]
   rorx rax, r10, 14
   rorx rcx, r10, 18
   rorx rdx, r10, 41
   xor rax, rcx
   xor rax, rdx
   add r13, rax
   mov rax, r11
   xor rax, r12
   and rax, r10
   xor rax, r12
   add r13, rax
   add r9, r13
   rorx rax, r14, 28
   rorx rcx, r14, 34
   rorx rdx, r14, 39
   xor rax, rcx
   xor rax, rdx
   add r13, rax
   mov rax, r14
   and rax, r15
   mov rcx, r14
   or rcx, r15
   and rcx, r8
   or rax, rcx
   add r13, rax
   /* FIPS 180-4 round 3. */
   mov rax, QWORD PTR [rsp + 24]
   add r12, rax
   add r12, QWORD PTR [rbx + 24]
   rorx rax, r9, 14
   rorx rcx, r9, 18
   rorx rdx, r9, 41
   xor rax, rcx
   xor rax, rdx
   add r12, rax
   mov rax, r10
   xor rax, r11
   and rax, r9
   xor rax, r11
   add r12, rax
   add r8, r12
   rorx rax, r13, 28
   rorx rcx, r13, 34
   rorx rdx, r13, 39
   xor rax, rcx
   xor rax, rdx
   add r12, rax
   mov rax, r13
   and rax, r14
   mov rcx, r13
   or rcx, r14
   and rcx, r15
   or rax, rcx
   add r12, rax
   /* FIPS 180-4 round 4. */
   mov rax, QWORD PTR [rsp + 32]
   add r11, rax
   add r11, QWORD PTR [rbx + 32]
   rorx rax, r8, 14
   rorx rcx, r8, 18
   rorx rdx, r8, 41
   xor rax, rcx
   xor rax, rdx
   add r11, rax
   mov rax, r9
   xor rax, r10
   and rax, r8
   xor rax, r10
   add r11, rax
   add r15, r11
   rorx rax, r12, 28
   rorx rcx, r12, 34
   rorx rdx, r12, 39
   xor rax, rcx
   xor rax, rdx
   add r11, rax
   mov rax, r12
   and rax, r13
   mov rcx, r12
   or rcx, r13
   and rcx, r14
   or rax, rcx
   add r11, rax
   /* FIPS 180-4 round 5. */
   mov rax, QWORD PTR [rsp + 40]
   add r10, rax
   add r10, QWORD PTR [rbx + 40]
   rorx rax, r15, 14
   rorx rcx, r15, 18
   rorx rdx, r15, 41
   xor rax, rcx
   xor rax, rdx
   add r10, rax
   mov rax, r8
   xor rax, r9
   and rax, r15
   xor rax, r9
   add r10, rax
   add r14, r10
   rorx rax, r11, 28
   rorx rcx, r11, 34
   rorx rdx, r11, 39
   xor rax, rcx
   xor rax, rdx
   add r10, rax
   mov rax, r11
   and rax, r12
   mov rcx, r11
   or rcx, r12
   and rcx, r13
   or rax, rcx
   add r10, rax
   /* FIPS 180-4 round 6. */
   mov rax, QWORD PTR [rsp + 48]
   add r9, rax
   add r9, QWORD PTR [rbx + 48]
   rorx rax, r14, 14
   rorx rcx, r14, 18
   rorx rdx, r14, 41
   xor rax, rcx
   xor rax, rdx
   add r9, rax
   mov rax, r15
   xor rax, r8
   and rax, r14
   xor rax, r8
   add r9, rax
   add r13, r9
   rorx rax, r10, 28
   rorx rcx, r10, 34
   rorx rdx, r10, 39
   xor rax, rcx
   xor rax, rdx
   add r9, rax
   mov rax, r10
   and rax, r11
   mov rcx, r10
   or rcx, r11
   and rcx, r12
   or rax, rcx
   add r9, rax
   /* FIPS 180-4 round 7. */
   mov rax, QWORD PTR [rsp + 56]
   add r8, rax
   add r8, QWORD PTR [rbx + 56]
   rorx rax, r13, 14
   rorx rcx, r13, 18
   rorx rdx, r13, 41
   xor rax, rcx
   xor rax, rdx
   add r8, rax
   mov rax, r14
   xor rax, r15
   and rax, r13
   xor rax, r15
   add r8, rax
   add r12, r8
   rorx rax, r9, 28
   rorx rcx, r9, 34
   rorx rdx, r9, 39
   xor rax, rcx
   xor rax, rdx
   add r8, rax
   mov rax, r9
   and rax, r10
   mov rcx, r9
   or rcx, r10
   and rcx, r11
   or rax, rcx
   add r8, rax
   /* FIPS 180-4 round 8. */
   mov rax, QWORD PTR [rsp + 64]
   add r15, rax
   add r15, QWORD PTR [rbx + 64]
   rorx rax, r12, 14
   rorx rcx, r12, 18
   rorx rdx, r12, 41
   xor rax, rcx
   xor rax, rdx
   add r15, rax
   mov rax, r13
   xor rax, r14
   and rax, r12
   xor rax, r14
   add r15, rax
   add r11, r15
   rorx rax, r8, 28
   rorx rcx, r8, 34
   rorx rdx, r8, 39
   xor rax, rcx
   xor rax, rdx
   add r15, rax
   mov rax, r8
   and rax, r9
   mov rcx, r8
   or rcx, r9
   and rcx, r10
   or rax, rcx
   add r15, rax
   /* FIPS 180-4 round 9. */
   mov rax, QWORD PTR [rsp + 72]
   add r14, rax
   add r14, QWORD PTR [rbx + 72]
   rorx rax, r11, 14
   rorx rcx, r11, 18
   rorx rdx, r11, 41
   xor rax, rcx
   xor rax, rdx
   add r14, rax
   mov rax, r12
   xor rax, r13
   and rax, r11
   xor rax, r13
   add r14, rax
   add r10, r14
   rorx rax, r15, 28
   rorx rcx, r15, 34
   rorx rdx, r15, 39
   xor rax, rcx
   xor rax, rdx
   add r14, rax
   mov rax, r15
   and rax, r8
   mov rcx, r15
   or rcx, r8
   and rcx, r9
   or rax, rcx
   add r14, rax
   /* FIPS 180-4 round 10. */
   mov rax, QWORD PTR [rsp + 80]
   add r13, rax
   add r13, QWORD PTR [rbx + 80]
   rorx rax, r10, 14
   rorx rcx, r10, 18
   rorx rdx, r10, 41
   xor rax, rcx
   xor rax, rdx
   add r13, rax
   mov rax, r11
   xor rax, r12
   and rax, r10
   xor rax, r12
   add r13, rax
   add r9, r13
   rorx rax, r14, 28
   rorx rcx, r14, 34
   rorx rdx, r14, 39
   xor rax, rcx
   xor rax, rdx
   add r13, rax
   mov rax, r14
   and rax, r15
   mov rcx, r14
   or rcx, r15
   and rcx, r8
   or rax, rcx
   add r13, rax
   /* FIPS 180-4 round 11. */
   mov rax, QWORD PTR [rsp + 88]
   add r12, rax
   add r12, QWORD PTR [rbx + 88]
   rorx rax, r9, 14
   rorx rcx, r9, 18
   rorx rdx, r9, 41
   xor rax, rcx
   xor rax, rdx
   add r12, rax
   mov rax, r10
   xor rax, r11
   and rax, r9
   xor rax, r11
   add r12, rax
   add r8, r12
   rorx rax, r13, 28
   rorx rcx, r13, 34
   rorx rdx, r13, 39
   xor rax, rcx
   xor rax, rdx
   add r12, rax
   mov rax, r13
   and rax, r14
   mov rcx, r13
   or rcx, r14
   and rcx, r15
   or rax, rcx
   add r12, rax
   /* FIPS 180-4 round 12. */
   mov rax, QWORD PTR [rsp + 96]
   add r11, rax
   add r11, QWORD PTR [rbx + 96]
   rorx rax, r8, 14
   rorx rcx, r8, 18
   rorx rdx, r8, 41
   xor rax, rcx
   xor rax, rdx
   add r11, rax
   mov rax, r9
   xor rax, r10
   and rax, r8
   xor rax, r10
   add r11, rax
   add r15, r11
   rorx rax, r12, 28
   rorx rcx, r12, 34
   rorx rdx, r12, 39
   xor rax, rcx
   xor rax, rdx
   add r11, rax
   mov rax, r12
   and rax, r13
   mov rcx, r12
   or rcx, r13
   and rcx, r14
   or rax, rcx
   add r11, rax
   /* FIPS 180-4 round 13. */
   mov rax, QWORD PTR [rsp + 104]
   add r10, rax
   add r10, QWORD PTR [rbx + 104]
   rorx rax, r15, 14
   rorx rcx, r15, 18
   rorx rdx, r15, 41
   xor rax, rcx
   xor rax, rdx
   add r10, rax
   mov rax, r8
   xor rax, r9
   and rax, r15
   xor rax, r9
   add r10, rax
   add r14, r10
   rorx rax, r11, 28
   rorx rcx, r11, 34
   rorx rdx, r11, 39
   xor rax, rcx
   xor rax, rdx
   add r10, rax
   mov rax, r11
   and rax, r12
   mov rcx, r11
   or rcx, r12
   and rcx, r13
   or rax, rcx
   add r10, rax
   /* FIPS 180-4 round 14. */
   mov rax, QWORD PTR [rsp + 112]
   add r9, rax
   add r9, QWORD PTR [rbx + 112]
   rorx rax, r14, 14
   rorx rcx, r14, 18
   rorx rdx, r14, 41
   xor rax, rcx
   xor rax, rdx
   add r9, rax
   mov rax, r15
   xor rax, r8
   and rax, r14
   xor rax, r8
   add r9, rax
   add r13, r9
   rorx rax, r10, 28
   rorx rcx, r10, 34
   rorx rdx, r10, 39
   xor rax, rcx
   xor rax, rdx
   add r9, rax
   mov rax, r10
   and rax, r11
   mov rcx, r10
   or rcx, r11
   and rcx, r12
   or rax, rcx
   add r9, rax
   /* FIPS 180-4 round 15. */
   mov rax, QWORD PTR [rsp + 120]
   add r8, rax
   add r8, QWORD PTR [rbx + 120]
   rorx rax, r13, 14
   rorx rcx, r13, 18
   rorx rdx, r13, 41
   xor rax, rcx
   xor rax, rdx
   add r8, rax
   mov rax, r14
   xor rax, r15
   and rax, r13
   xor rax, r15
   add r8, rax
   add r12, r8
   rorx rax, r9, 28
   rorx rcx, r9, 34
   rorx rdx, r9, 39
   xor rax, rcx
   xor rax, rdx
   add r8, rax
   mov rax, r9
   and rax, r10
   mov rcx, r9
   or rcx, r10
   and rcx, r11
   or rax, rcx
   add r8, rax
   /* FIPS 180-4 round 16. */
   mov rax, QWORD PTR [rsp + 8]
   rorx rcx, rax, 1
   rorx rdx, rax, 8
   shr rax, 7
   xor rax, rcx
   xor rax, rdx
   add rax, QWORD PTR [rsp + 0]
   add rax, QWORD PTR [rsp + 72]
   mov rcx, QWORD PTR [rsp + 112]
   rorx rdx, rcx, 19
   rorx rbp, rcx, 61
   shr rcx, 6
   xor rcx, rdx
   xor rcx, rbp
   add rax, rcx
   mov QWORD PTR [rsp + 0], rax
   add r15, rax
   add r15, QWORD PTR [rbx + 128]
   rorx rax, r12, 14
   rorx rcx, r12, 18
   rorx rdx, r12, 41
   xor rax, rcx
   xor rax, rdx
   add r15, rax
   mov rax, r13
   xor rax, r14
   and rax, r12
   xor rax, r14
   add r15, rax
   add r11, r15
   rorx rax, r8, 28
   rorx rcx, r8, 34
   rorx rdx, r8, 39
   xor rax, rcx
   xor rax, rdx
   add r15, rax
   mov rax, r8
   and rax, r9
   mov rcx, r8
   or rcx, r9
   and rcx, r10
   or rax, rcx
   add r15, rax
   /* FIPS 180-4 round 17. */
   mov rax, QWORD PTR [rsp + 16]
   rorx rcx, rax, 1
   rorx rdx, rax, 8
   shr rax, 7
   xor rax, rcx
   xor rax, rdx
   add rax, QWORD PTR [rsp + 8]
   add rax, QWORD PTR [rsp + 80]
   mov rcx, QWORD PTR [rsp + 120]
   rorx rdx, rcx, 19
   rorx rbp, rcx, 61
   shr rcx, 6
   xor rcx, rdx
   xor rcx, rbp
   add rax, rcx
   mov QWORD PTR [rsp + 8], rax
   add r14, rax
   add r14, QWORD PTR [rbx + 136]
   rorx rax, r11, 14
   rorx rcx, r11, 18
   rorx rdx, r11, 41
   xor rax, rcx
   xor rax, rdx
   add r14, rax
   mov rax, r12
   xor rax, r13
   and rax, r11
   xor rax, r13
   add r14, rax
   add r10, r14
   rorx rax, r15, 28
   rorx rcx, r15, 34
   rorx rdx, r15, 39
   xor rax, rcx
   xor rax, rdx
   add r14, rax
   mov rax, r15
   and rax, r8
   mov rcx, r15
   or rcx, r8
   and rcx, r9
   or rax, rcx
   add r14, rax
   /* FIPS 180-4 round 18. */
   mov rax, QWORD PTR [rsp + 24]
   rorx rcx, rax, 1
   rorx rdx, rax, 8
   shr rax, 7
   xor rax, rcx
   xor rax, rdx
   add rax, QWORD PTR [rsp + 16]
   add rax, QWORD PTR [rsp + 88]
   mov rcx, QWORD PTR [rsp + 0]
   rorx rdx, rcx, 19
   rorx rbp, rcx, 61
   shr rcx, 6
   xor rcx, rdx
   xor rcx, rbp
   add rax, rcx
   mov QWORD PTR [rsp + 16], rax
   add r13, rax
   add r13, QWORD PTR [rbx + 144]
   rorx rax, r10, 14
   rorx rcx, r10, 18
   rorx rdx, r10, 41
   xor rax, rcx
   xor rax, rdx
   add r13, rax
   mov rax, r11
   xor rax, r12
   and rax, r10
   xor rax, r12
   add r13, rax
   add r9, r13
   rorx rax, r14, 28
   rorx rcx, r14, 34
   rorx rdx, r14, 39
   xor rax, rcx
   xor rax, rdx
   add r13, rax
   mov rax, r14
   and rax, r15
   mov rcx, r14
   or rcx, r15
   and rcx, r8
   or rax, rcx
   add r13, rax
   /* FIPS 180-4 round 19. */
   mov rax, QWORD PTR [rsp + 32]
   rorx rcx, rax, 1
   rorx rdx, rax, 8
   shr rax, 7
   xor rax, rcx
   xor rax, rdx
   add rax, QWORD PTR [rsp + 24]
   add rax, QWORD PTR [rsp + 96]
   mov rcx, QWORD PTR [rsp + 8]
   rorx rdx, rcx, 19
   rorx rbp, rcx, 61
   shr rcx, 6
   xor rcx, rdx
   xor rcx, rbp
   add rax, rcx
   mov QWORD PTR [rsp + 24], rax
   add r12, rax
   add r12, QWORD PTR [rbx + 152]
   rorx rax, r9, 14
   rorx rcx, r9, 18
   rorx rdx, r9, 41
   xor rax, rcx
   xor rax, rdx
   add r12, rax
   mov rax, r10
   xor rax, r11
   and rax, r9
   xor rax, r11
   add r12, rax
   add r8, r12
   rorx rax, r13, 28
   rorx rcx, r13, 34
   rorx rdx, r13, 39
   xor rax, rcx
   xor rax, rdx
   add r12, rax
   mov rax, r13
   and rax, r14
   mov rcx, r13
   or rcx, r14
   and rcx, r15
   or rax, rcx
   add r12, rax
   /* FIPS 180-4 round 20. */
   mov rax, QWORD PTR [rsp + 40]
   rorx rcx, rax, 1
   rorx rdx, rax, 8
   shr rax, 7
   xor rax, rcx
   xor rax, rdx
   add rax, QWORD PTR [rsp + 32]
   add rax, QWORD PTR [rsp + 104]
   mov rcx, QWORD PTR [rsp + 16]
   rorx rdx, rcx, 19
   rorx rbp, rcx, 61
   shr rcx, 6
   xor rcx, rdx
   xor rcx, rbp
   add rax, rcx
   mov QWORD PTR [rsp + 32], rax
   add r11, rax
   add r11, QWORD PTR [rbx + 160]
   rorx rax, r8, 14
   rorx rcx, r8, 18
   rorx rdx, r8, 41
   xor rax, rcx
   xor rax, rdx
   add r11, rax
   mov rax, r9
   xor rax, r10
   and rax, r8
   xor rax, r10
   add r11, rax
   add r15, r11
   rorx rax, r12, 28
   rorx rcx, r12, 34
   rorx rdx, r12, 39
   xor rax, rcx
   xor rax, rdx
   add r11, rax
   mov rax, r12
   and rax, r13
   mov rcx, r12
   or rcx, r13
   and rcx, r14
   or rax, rcx
   add r11, rax
   /* FIPS 180-4 round 21. */
   mov rax, QWORD PTR [rsp + 48]
   rorx rcx, rax, 1
   rorx rdx, rax, 8
   shr rax, 7
   xor rax, rcx
   xor rax, rdx
   add rax, QWORD PTR [rsp + 40]
   add rax, QWORD PTR [rsp + 112]
   mov rcx, QWORD PTR [rsp + 24]
   rorx rdx, rcx, 19
   rorx rbp, rcx, 61
   shr rcx, 6
   xor rcx, rdx
   xor rcx, rbp
   add rax, rcx
   mov QWORD PTR [rsp + 40], rax
   add r10, rax
   add r10, QWORD PTR [rbx + 168]
   rorx rax, r15, 14
   rorx rcx, r15, 18
   rorx rdx, r15, 41
   xor rax, rcx
   xor rax, rdx
   add r10, rax
   mov rax, r8
   xor rax, r9
   and rax, r15
   xor rax, r9
   add r10, rax
   add r14, r10
   rorx rax, r11, 28
   rorx rcx, r11, 34
   rorx rdx, r11, 39
   xor rax, rcx
   xor rax, rdx
   add r10, rax
   mov rax, r11
   and rax, r12
   mov rcx, r11
   or rcx, r12
   and rcx, r13
   or rax, rcx
   add r10, rax
   /* FIPS 180-4 round 22. */
   mov rax, QWORD PTR [rsp + 56]
   rorx rcx, rax, 1
   rorx rdx, rax, 8
   shr rax, 7
   xor rax, rcx
   xor rax, rdx
   add rax, QWORD PTR [rsp + 48]
   add rax, QWORD PTR [rsp + 120]
   mov rcx, QWORD PTR [rsp + 32]
   rorx rdx, rcx, 19
   rorx rbp, rcx, 61
   shr rcx, 6
   xor rcx, rdx
   xor rcx, rbp
   add rax, rcx
   mov QWORD PTR [rsp + 48], rax
   add r9, rax
   add r9, QWORD PTR [rbx + 176]
   rorx rax, r14, 14
   rorx rcx, r14, 18
   rorx rdx, r14, 41
   xor rax, rcx
   xor rax, rdx
   add r9, rax
   mov rax, r15
   xor rax, r8
   and rax, r14
   xor rax, r8
   add r9, rax
   add r13, r9
   rorx rax, r10, 28
   rorx rcx, r10, 34
   rorx rdx, r10, 39
   xor rax, rcx
   xor rax, rdx
   add r9, rax
   mov rax, r10
   and rax, r11
   mov rcx, r10
   or rcx, r11
   and rcx, r12
   or rax, rcx
   add r9, rax
   /* FIPS 180-4 round 23. */
   mov rax, QWORD PTR [rsp + 64]
   rorx rcx, rax, 1
   rorx rdx, rax, 8
   shr rax, 7
   xor rax, rcx
   xor rax, rdx
   add rax, QWORD PTR [rsp + 56]
   add rax, QWORD PTR [rsp + 0]
   mov rcx, QWORD PTR [rsp + 40]
   rorx rdx, rcx, 19
   rorx rbp, rcx, 61
   shr rcx, 6
   xor rcx, rdx
   xor rcx, rbp
   add rax, rcx
   mov QWORD PTR [rsp + 56], rax
   add r8, rax
   add r8, QWORD PTR [rbx + 184]
   rorx rax, r13, 14
   rorx rcx, r13, 18
   rorx rdx, r13, 41
   xor rax, rcx
   xor rax, rdx
   add r8, rax
   mov rax, r14
   xor rax, r15
   and rax, r13
   xor rax, r15
   add r8, rax
   add r12, r8
   rorx rax, r9, 28
   rorx rcx, r9, 34
   rorx rdx, r9, 39
   xor rax, rcx
   xor rax, rdx
   add r8, rax
   mov rax, r9
   and rax, r10
   mov rcx, r9
   or rcx, r10
   and rcx, r11
   or rax, rcx
   add r8, rax
   /* FIPS 180-4 round 24. */
   mov rax, QWORD PTR [rsp + 72]
   rorx rcx, rax, 1
   rorx rdx, rax, 8
   shr rax, 7
   xor rax, rcx
   xor rax, rdx
   add rax, QWORD PTR [rsp + 64]
   add rax, QWORD PTR [rsp + 8]
   mov rcx, QWORD PTR [rsp + 48]
   rorx rdx, rcx, 19
   rorx rbp, rcx, 61
   shr rcx, 6
   xor rcx, rdx
   xor rcx, rbp
   add rax, rcx
   mov QWORD PTR [rsp + 64], rax
   add r15, rax
   add r15, QWORD PTR [rbx + 192]
   rorx rax, r12, 14
   rorx rcx, r12, 18
   rorx rdx, r12, 41
   xor rax, rcx
   xor rax, rdx
   add r15, rax
   mov rax, r13
   xor rax, r14
   and rax, r12
   xor rax, r14
   add r15, rax
   add r11, r15
   rorx rax, r8, 28
   rorx rcx, r8, 34
   rorx rdx, r8, 39
   xor rax, rcx
   xor rax, rdx
   add r15, rax
   mov rax, r8
   and rax, r9
   mov rcx, r8
   or rcx, r9
   and rcx, r10
   or rax, rcx
   add r15, rax
   /* FIPS 180-4 round 25. */
   mov rax, QWORD PTR [rsp + 80]
   rorx rcx, rax, 1
   rorx rdx, rax, 8
   shr rax, 7
   xor rax, rcx
   xor rax, rdx
   add rax, QWORD PTR [rsp + 72]
   add rax, QWORD PTR [rsp + 16]
   mov rcx, QWORD PTR [rsp + 56]
   rorx rdx, rcx, 19
   rorx rbp, rcx, 61
   shr rcx, 6
   xor rcx, rdx
   xor rcx, rbp
   add rax, rcx
   mov QWORD PTR [rsp + 72], rax
   add r14, rax
   add r14, QWORD PTR [rbx + 200]
   rorx rax, r11, 14
   rorx rcx, r11, 18
   rorx rdx, r11, 41
   xor rax, rcx
   xor rax, rdx
   add r14, rax
   mov rax, r12
   xor rax, r13
   and rax, r11
   xor rax, r13
   add r14, rax
   add r10, r14
   rorx rax, r15, 28
   rorx rcx, r15, 34
   rorx rdx, r15, 39
   xor rax, rcx
   xor rax, rdx
   add r14, rax
   mov rax, r15
   and rax, r8
   mov rcx, r15
   or rcx, r8
   and rcx, r9
   or rax, rcx
   add r14, rax
   /* FIPS 180-4 round 26. */
   mov rax, QWORD PTR [rsp + 88]
   rorx rcx, rax, 1
   rorx rdx, rax, 8
   shr rax, 7
   xor rax, rcx
   xor rax, rdx
   add rax, QWORD PTR [rsp + 80]
   add rax, QWORD PTR [rsp + 24]
   mov rcx, QWORD PTR [rsp + 64]
   rorx rdx, rcx, 19
   rorx rbp, rcx, 61
   shr rcx, 6
   xor rcx, rdx
   xor rcx, rbp
   add rax, rcx
   mov QWORD PTR [rsp + 80], rax
   add r13, rax
   add r13, QWORD PTR [rbx + 208]
   rorx rax, r10, 14
   rorx rcx, r10, 18
   rorx rdx, r10, 41
   xor rax, rcx
   xor rax, rdx
   add r13, rax
   mov rax, r11
   xor rax, r12
   and rax, r10
   xor rax, r12
   add r13, rax
   add r9, r13
   rorx rax, r14, 28
   rorx rcx, r14, 34
   rorx rdx, r14, 39
   xor rax, rcx
   xor rax, rdx
   add r13, rax
   mov rax, r14
   and rax, r15
   mov rcx, r14
   or rcx, r15
   and rcx, r8
   or rax, rcx
   add r13, rax
   /* FIPS 180-4 round 27. */
   mov rax, QWORD PTR [rsp + 96]
   rorx rcx, rax, 1
   rorx rdx, rax, 8
   shr rax, 7
   xor rax, rcx
   xor rax, rdx
   add rax, QWORD PTR [rsp + 88]
   add rax, QWORD PTR [rsp + 32]
   mov rcx, QWORD PTR [rsp + 72]
   rorx rdx, rcx, 19
   rorx rbp, rcx, 61
   shr rcx, 6
   xor rcx, rdx
   xor rcx, rbp
   add rax, rcx
   mov QWORD PTR [rsp + 88], rax
   add r12, rax
   add r12, QWORD PTR [rbx + 216]
   rorx rax, r9, 14
   rorx rcx, r9, 18
   rorx rdx, r9, 41
   xor rax, rcx
   xor rax, rdx
   add r12, rax
   mov rax, r10
   xor rax, r11
   and rax, r9
   xor rax, r11
   add r12, rax
   add r8, r12
   rorx rax, r13, 28
   rorx rcx, r13, 34
   rorx rdx, r13, 39
   xor rax, rcx
   xor rax, rdx
   add r12, rax
   mov rax, r13
   and rax, r14
   mov rcx, r13
   or rcx, r14
   and rcx, r15
   or rax, rcx
   add r12, rax
   /* FIPS 180-4 round 28. */
   mov rax, QWORD PTR [rsp + 104]
   rorx rcx, rax, 1
   rorx rdx, rax, 8
   shr rax, 7
   xor rax, rcx
   xor rax, rdx
   add rax, QWORD PTR [rsp + 96]
   add rax, QWORD PTR [rsp + 40]
   mov rcx, QWORD PTR [rsp + 80]
   rorx rdx, rcx, 19
   rorx rbp, rcx, 61
   shr rcx, 6
   xor rcx, rdx
   xor rcx, rbp
   add rax, rcx
   mov QWORD PTR [rsp + 96], rax
   add r11, rax
   add r11, QWORD PTR [rbx + 224]
   rorx rax, r8, 14
   rorx rcx, r8, 18
   rorx rdx, r8, 41
   xor rax, rcx
   xor rax, rdx
   add r11, rax
   mov rax, r9
   xor rax, r10
   and rax, r8
   xor rax, r10
   add r11, rax
   add r15, r11
   rorx rax, r12, 28
   rorx rcx, r12, 34
   rorx rdx, r12, 39
   xor rax, rcx
   xor rax, rdx
   add r11, rax
   mov rax, r12
   and rax, r13
   mov rcx, r12
   or rcx, r13
   and rcx, r14
   or rax, rcx
   add r11, rax
   /* FIPS 180-4 round 29. */
   mov rax, QWORD PTR [rsp + 112]
   rorx rcx, rax, 1
   rorx rdx, rax, 8
   shr rax, 7
   xor rax, rcx
   xor rax, rdx
   add rax, QWORD PTR [rsp + 104]
   add rax, QWORD PTR [rsp + 48]
   mov rcx, QWORD PTR [rsp + 88]
   rorx rdx, rcx, 19
   rorx rbp, rcx, 61
   shr rcx, 6
   xor rcx, rdx
   xor rcx, rbp
   add rax, rcx
   mov QWORD PTR [rsp + 104], rax
   add r10, rax
   add r10, QWORD PTR [rbx + 232]
   rorx rax, r15, 14
   rorx rcx, r15, 18
   rorx rdx, r15, 41
   xor rax, rcx
   xor rax, rdx
   add r10, rax
   mov rax, r8
   xor rax, r9
   and rax, r15
   xor rax, r9
   add r10, rax
   add r14, r10
   rorx rax, r11, 28
   rorx rcx, r11, 34
   rorx rdx, r11, 39
   xor rax, rcx
   xor rax, rdx
   add r10, rax
   mov rax, r11
   and rax, r12
   mov rcx, r11
   or rcx, r12
   and rcx, r13
   or rax, rcx
   add r10, rax
   /* FIPS 180-4 round 30. */
   mov rax, QWORD PTR [rsp + 120]
   rorx rcx, rax, 1
   rorx rdx, rax, 8
   shr rax, 7
   xor rax, rcx
   xor rax, rdx
   add rax, QWORD PTR [rsp + 112]
   add rax, QWORD PTR [rsp + 56]
   mov rcx, QWORD PTR [rsp + 96]
   rorx rdx, rcx, 19
   rorx rbp, rcx, 61
   shr rcx, 6
   xor rcx, rdx
   xor rcx, rbp
   add rax, rcx
   mov QWORD PTR [rsp + 112], rax
   add r9, rax
   add r9, QWORD PTR [rbx + 240]
   rorx rax, r14, 14
   rorx rcx, r14, 18
   rorx rdx, r14, 41
   xor rax, rcx
   xor rax, rdx
   add r9, rax
   mov rax, r15
   xor rax, r8
   and rax, r14
   xor rax, r8
   add r9, rax
   add r13, r9
   rorx rax, r10, 28
   rorx rcx, r10, 34
   rorx rdx, r10, 39
   xor rax, rcx
   xor rax, rdx
   add r9, rax
   mov rax, r10
   and rax, r11
   mov rcx, r10
   or rcx, r11
   and rcx, r12
   or rax, rcx
   add r9, rax
   /* FIPS 180-4 round 31. */
   mov rax, QWORD PTR [rsp + 0]
   rorx rcx, rax, 1
   rorx rdx, rax, 8
   shr rax, 7
   xor rax, rcx
   xor rax, rdx
   add rax, QWORD PTR [rsp + 120]
   add rax, QWORD PTR [rsp + 64]
   mov rcx, QWORD PTR [rsp + 104]
   rorx rdx, rcx, 19
   rorx rbp, rcx, 61
   shr rcx, 6
   xor rcx, rdx
   xor rcx, rbp
   add rax, rcx
   mov QWORD PTR [rsp + 120], rax
   add r8, rax
   add r8, QWORD PTR [rbx + 248]
   rorx rax, r13, 14
   rorx rcx, r13, 18
   rorx rdx, r13, 41
   xor rax, rcx
   xor rax, rdx
   add r8, rax
   mov rax, r14
   xor rax, r15
   and rax, r13
   xor rax, r15
   add r8, rax
   add r12, r8
   rorx rax, r9, 28
   rorx rcx, r9, 34
   rorx rdx, r9, 39
   xor rax, rcx
   xor rax, rdx
   add r8, rax
   mov rax, r9
   and rax, r10
   mov rcx, r9
   or rcx, r10
   and rcx, r11
   or rax, rcx
   add r8, rax
   /* FIPS 180-4 round 32. */
   mov rax, QWORD PTR [rsp + 8]
   rorx rcx, rax, 1
   rorx rdx, rax, 8
   shr rax, 7
   xor rax, rcx
   xor rax, rdx
   add rax, QWORD PTR [rsp + 0]
   add rax, QWORD PTR [rsp + 72]
   mov rcx, QWORD PTR [rsp + 112]
   rorx rdx, rcx, 19
   rorx rbp, rcx, 61
   shr rcx, 6
   xor rcx, rdx
   xor rcx, rbp
   add rax, rcx
   mov QWORD PTR [rsp + 0], rax
   add r15, rax
   add r15, QWORD PTR [rbx + 256]
   rorx rax, r12, 14
   rorx rcx, r12, 18
   rorx rdx, r12, 41
   xor rax, rcx
   xor rax, rdx
   add r15, rax
   mov rax, r13
   xor rax, r14
   and rax, r12
   xor rax, r14
   add r15, rax
   add r11, r15
   rorx rax, r8, 28
   rorx rcx, r8, 34
   rorx rdx, r8, 39
   xor rax, rcx
   xor rax, rdx
   add r15, rax
   mov rax, r8
   and rax, r9
   mov rcx, r8
   or rcx, r9
   and rcx, r10
   or rax, rcx
   add r15, rax
   /* FIPS 180-4 round 33. */
   mov rax, QWORD PTR [rsp + 16]
   rorx rcx, rax, 1
   rorx rdx, rax, 8
   shr rax, 7
   xor rax, rcx
   xor rax, rdx
   add rax, QWORD PTR [rsp + 8]
   add rax, QWORD PTR [rsp + 80]
   mov rcx, QWORD PTR [rsp + 120]
   rorx rdx, rcx, 19
   rorx rbp, rcx, 61
   shr rcx, 6
   xor rcx, rdx
   xor rcx, rbp
   add rax, rcx
   mov QWORD PTR [rsp + 8], rax
   add r14, rax
   add r14, QWORD PTR [rbx + 264]
   rorx rax, r11, 14
   rorx rcx, r11, 18
   rorx rdx, r11, 41
   xor rax, rcx
   xor rax, rdx
   add r14, rax
   mov rax, r12
   xor rax, r13
   and rax, r11
   xor rax, r13
   add r14, rax
   add r10, r14
   rorx rax, r15, 28
   rorx rcx, r15, 34
   rorx rdx, r15, 39
   xor rax, rcx
   xor rax, rdx
   add r14, rax
   mov rax, r15
   and rax, r8
   mov rcx, r15
   or rcx, r8
   and rcx, r9
   or rax, rcx
   add r14, rax
   /* FIPS 180-4 round 34. */
   mov rax, QWORD PTR [rsp + 24]
   rorx rcx, rax, 1
   rorx rdx, rax, 8
   shr rax, 7
   xor rax, rcx
   xor rax, rdx
   add rax, QWORD PTR [rsp + 16]
   add rax, QWORD PTR [rsp + 88]
   mov rcx, QWORD PTR [rsp + 0]
   rorx rdx, rcx, 19
   rorx rbp, rcx, 61
   shr rcx, 6
   xor rcx, rdx
   xor rcx, rbp
   add rax, rcx
   mov QWORD PTR [rsp + 16], rax
   add r13, rax
   add r13, QWORD PTR [rbx + 272]
   rorx rax, r10, 14
   rorx rcx, r10, 18
   rorx rdx, r10, 41
   xor rax, rcx
   xor rax, rdx
   add r13, rax
   mov rax, r11
   xor rax, r12
   and rax, r10
   xor rax, r12
   add r13, rax
   add r9, r13
   rorx rax, r14, 28
   rorx rcx, r14, 34
   rorx rdx, r14, 39
   xor rax, rcx
   xor rax, rdx
   add r13, rax
   mov rax, r14
   and rax, r15
   mov rcx, r14
   or rcx, r15
   and rcx, r8
   or rax, rcx
   add r13, rax
   /* FIPS 180-4 round 35. */
   mov rax, QWORD PTR [rsp + 32]
   rorx rcx, rax, 1
   rorx rdx, rax, 8
   shr rax, 7
   xor rax, rcx
   xor rax, rdx
   add rax, QWORD PTR [rsp + 24]
   add rax, QWORD PTR [rsp + 96]
   mov rcx, QWORD PTR [rsp + 8]
   rorx rdx, rcx, 19
   rorx rbp, rcx, 61
   shr rcx, 6
   xor rcx, rdx
   xor rcx, rbp
   add rax, rcx
   mov QWORD PTR [rsp + 24], rax
   add r12, rax
   add r12, QWORD PTR [rbx + 280]
   rorx rax, r9, 14
   rorx rcx, r9, 18
   rorx rdx, r9, 41
   xor rax, rcx
   xor rax, rdx
   add r12, rax
   mov rax, r10
   xor rax, r11
   and rax, r9
   xor rax, r11
   add r12, rax
   add r8, r12
   rorx rax, r13, 28
   rorx rcx, r13, 34
   rorx rdx, r13, 39
   xor rax, rcx
   xor rax, rdx
   add r12, rax
   mov rax, r13
   and rax, r14
   mov rcx, r13
   or rcx, r14
   and rcx, r15
   or rax, rcx
   add r12, rax
   /* FIPS 180-4 round 36. */
   mov rax, QWORD PTR [rsp + 40]
   rorx rcx, rax, 1
   rorx rdx, rax, 8
   shr rax, 7
   xor rax, rcx
   xor rax, rdx
   add rax, QWORD PTR [rsp + 32]
   add rax, QWORD PTR [rsp + 104]
   mov rcx, QWORD PTR [rsp + 16]
   rorx rdx, rcx, 19
   rorx rbp, rcx, 61
   shr rcx, 6
   xor rcx, rdx
   xor rcx, rbp
   add rax, rcx
   mov QWORD PTR [rsp + 32], rax
   add r11, rax
   add r11, QWORD PTR [rbx + 288]
   rorx rax, r8, 14
   rorx rcx, r8, 18
   rorx rdx, r8, 41
   xor rax, rcx
   xor rax, rdx
   add r11, rax
   mov rax, r9
   xor rax, r10
   and rax, r8
   xor rax, r10
   add r11, rax
   add r15, r11
   rorx rax, r12, 28
   rorx rcx, r12, 34
   rorx rdx, r12, 39
   xor rax, rcx
   xor rax, rdx
   add r11, rax
   mov rax, r12
   and rax, r13
   mov rcx, r12
   or rcx, r13
   and rcx, r14
   or rax, rcx
   add r11, rax
   /* FIPS 180-4 round 37. */
   mov rax, QWORD PTR [rsp + 48]
   rorx rcx, rax, 1
   rorx rdx, rax, 8
   shr rax, 7
   xor rax, rcx
   xor rax, rdx
   add rax, QWORD PTR [rsp + 40]
   add rax, QWORD PTR [rsp + 112]
   mov rcx, QWORD PTR [rsp + 24]
   rorx rdx, rcx, 19
   rorx rbp, rcx, 61
   shr rcx, 6
   xor rcx, rdx
   xor rcx, rbp
   add rax, rcx
   mov QWORD PTR [rsp + 40], rax
   add r10, rax
   add r10, QWORD PTR [rbx + 296]
   rorx rax, r15, 14
   rorx rcx, r15, 18
   rorx rdx, r15, 41
   xor rax, rcx
   xor rax, rdx
   add r10, rax
   mov rax, r8
   xor rax, r9
   and rax, r15
   xor rax, r9
   add r10, rax
   add r14, r10
   rorx rax, r11, 28
   rorx rcx, r11, 34
   rorx rdx, r11, 39
   xor rax, rcx
   xor rax, rdx
   add r10, rax
   mov rax, r11
   and rax, r12
   mov rcx, r11
   or rcx, r12
   and rcx, r13
   or rax, rcx
   add r10, rax
   /* FIPS 180-4 round 38. */
   mov rax, QWORD PTR [rsp + 56]
   rorx rcx, rax, 1
   rorx rdx, rax, 8
   shr rax, 7
   xor rax, rcx
   xor rax, rdx
   add rax, QWORD PTR [rsp + 48]
   add rax, QWORD PTR [rsp + 120]
   mov rcx, QWORD PTR [rsp + 32]
   rorx rdx, rcx, 19
   rorx rbp, rcx, 61
   shr rcx, 6
   xor rcx, rdx
   xor rcx, rbp
   add rax, rcx
   mov QWORD PTR [rsp + 48], rax
   add r9, rax
   add r9, QWORD PTR [rbx + 304]
   rorx rax, r14, 14
   rorx rcx, r14, 18
   rorx rdx, r14, 41
   xor rax, rcx
   xor rax, rdx
   add r9, rax
   mov rax, r15
   xor rax, r8
   and rax, r14
   xor rax, r8
   add r9, rax
   add r13, r9
   rorx rax, r10, 28
   rorx rcx, r10, 34
   rorx rdx, r10, 39
   xor rax, rcx
   xor rax, rdx
   add r9, rax
   mov rax, r10
   and rax, r11
   mov rcx, r10
   or rcx, r11
   and rcx, r12
   or rax, rcx
   add r9, rax
   /* FIPS 180-4 round 39. */
   mov rax, QWORD PTR [rsp + 64]
   rorx rcx, rax, 1
   rorx rdx, rax, 8
   shr rax, 7
   xor rax, rcx
   xor rax, rdx
   add rax, QWORD PTR [rsp + 56]
   add rax, QWORD PTR [rsp + 0]
   mov rcx, QWORD PTR [rsp + 40]
   rorx rdx, rcx, 19
   rorx rbp, rcx, 61
   shr rcx, 6
   xor rcx, rdx
   xor rcx, rbp
   add rax, rcx
   mov QWORD PTR [rsp + 56], rax
   add r8, rax
   add r8, QWORD PTR [rbx + 312]
   rorx rax, r13, 14
   rorx rcx, r13, 18
   rorx rdx, r13, 41
   xor rax, rcx
   xor rax, rdx
   add r8, rax
   mov rax, r14
   xor rax, r15
   and rax, r13
   xor rax, r15
   add r8, rax
   add r12, r8
   rorx rax, r9, 28
   rorx rcx, r9, 34
   rorx rdx, r9, 39
   xor rax, rcx
   xor rax, rdx
   add r8, rax
   mov rax, r9
   and rax, r10
   mov rcx, r9
   or rcx, r10
   and rcx, r11
   or rax, rcx
   add r8, rax
   /* FIPS 180-4 round 40. */
   mov rax, QWORD PTR [rsp + 72]
   rorx rcx, rax, 1
   rorx rdx, rax, 8
   shr rax, 7
   xor rax, rcx
   xor rax, rdx
   add rax, QWORD PTR [rsp + 64]
   add rax, QWORD PTR [rsp + 8]
   mov rcx, QWORD PTR [rsp + 48]
   rorx rdx, rcx, 19
   rorx rbp, rcx, 61
   shr rcx, 6
   xor rcx, rdx
   xor rcx, rbp
   add rax, rcx
   mov QWORD PTR [rsp + 64], rax
   add r15, rax
   add r15, QWORD PTR [rbx + 320]
   rorx rax, r12, 14
   rorx rcx, r12, 18
   rorx rdx, r12, 41
   xor rax, rcx
   xor rax, rdx
   add r15, rax
   mov rax, r13
   xor rax, r14
   and rax, r12
   xor rax, r14
   add r15, rax
   add r11, r15
   rorx rax, r8, 28
   rorx rcx, r8, 34
   rorx rdx, r8, 39
   xor rax, rcx
   xor rax, rdx
   add r15, rax
   mov rax, r8
   and rax, r9
   mov rcx, r8
   or rcx, r9
   and rcx, r10
   or rax, rcx
   add r15, rax
   /* FIPS 180-4 round 41. */
   mov rax, QWORD PTR [rsp + 80]
   rorx rcx, rax, 1
   rorx rdx, rax, 8
   shr rax, 7
   xor rax, rcx
   xor rax, rdx
   add rax, QWORD PTR [rsp + 72]
   add rax, QWORD PTR [rsp + 16]
   mov rcx, QWORD PTR [rsp + 56]
   rorx rdx, rcx, 19
   rorx rbp, rcx, 61
   shr rcx, 6
   xor rcx, rdx
   xor rcx, rbp
   add rax, rcx
   mov QWORD PTR [rsp + 72], rax
   add r14, rax
   add r14, QWORD PTR [rbx + 328]
   rorx rax, r11, 14
   rorx rcx, r11, 18
   rorx rdx, r11, 41
   xor rax, rcx
   xor rax, rdx
   add r14, rax
   mov rax, r12
   xor rax, r13
   and rax, r11
   xor rax, r13
   add r14, rax
   add r10, r14
   rorx rax, r15, 28
   rorx rcx, r15, 34
   rorx rdx, r15, 39
   xor rax, rcx
   xor rax, rdx
   add r14, rax
   mov rax, r15
   and rax, r8
   mov rcx, r15
   or rcx, r8
   and rcx, r9
   or rax, rcx
   add r14, rax
   /* FIPS 180-4 round 42. */
   mov rax, QWORD PTR [rsp + 88]
   rorx rcx, rax, 1
   rorx rdx, rax, 8
   shr rax, 7
   xor rax, rcx
   xor rax, rdx
   add rax, QWORD PTR [rsp + 80]
   add rax, QWORD PTR [rsp + 24]
   mov rcx, QWORD PTR [rsp + 64]
   rorx rdx, rcx, 19
   rorx rbp, rcx, 61
   shr rcx, 6
   xor rcx, rdx
   xor rcx, rbp
   add rax, rcx
   mov QWORD PTR [rsp + 80], rax
   add r13, rax
   add r13, QWORD PTR [rbx + 336]
   rorx rax, r10, 14
   rorx rcx, r10, 18
   rorx rdx, r10, 41
   xor rax, rcx
   xor rax, rdx
   add r13, rax
   mov rax, r11
   xor rax, r12
   and rax, r10
   xor rax, r12
   add r13, rax
   add r9, r13
   rorx rax, r14, 28
   rorx rcx, r14, 34
   rorx rdx, r14, 39
   xor rax, rcx
   xor rax, rdx
   add r13, rax
   mov rax, r14
   and rax, r15
   mov rcx, r14
   or rcx, r15
   and rcx, r8
   or rax, rcx
   add r13, rax
   /* FIPS 180-4 round 43. */
   mov rax, QWORD PTR [rsp + 96]
   rorx rcx, rax, 1
   rorx rdx, rax, 8
   shr rax, 7
   xor rax, rcx
   xor rax, rdx
   add rax, QWORD PTR [rsp + 88]
   add rax, QWORD PTR [rsp + 32]
   mov rcx, QWORD PTR [rsp + 72]
   rorx rdx, rcx, 19
   rorx rbp, rcx, 61
   shr rcx, 6
   xor rcx, rdx
   xor rcx, rbp
   add rax, rcx
   mov QWORD PTR [rsp + 88], rax
   add r12, rax
   add r12, QWORD PTR [rbx + 344]
   rorx rax, r9, 14
   rorx rcx, r9, 18
   rorx rdx, r9, 41
   xor rax, rcx
   xor rax, rdx
   add r12, rax
   mov rax, r10
   xor rax, r11
   and rax, r9
   xor rax, r11
   add r12, rax
   add r8, r12
   rorx rax, r13, 28
   rorx rcx, r13, 34
   rorx rdx, r13, 39
   xor rax, rcx
   xor rax, rdx
   add r12, rax
   mov rax, r13
   and rax, r14
   mov rcx, r13
   or rcx, r14
   and rcx, r15
   or rax, rcx
   add r12, rax
   /* FIPS 180-4 round 44. */
   mov rax, QWORD PTR [rsp + 104]
   rorx rcx, rax, 1
   rorx rdx, rax, 8
   shr rax, 7
   xor rax, rcx
   xor rax, rdx
   add rax, QWORD PTR [rsp + 96]
   add rax, QWORD PTR [rsp + 40]
   mov rcx, QWORD PTR [rsp + 80]
   rorx rdx, rcx, 19
   rorx rbp, rcx, 61
   shr rcx, 6
   xor rcx, rdx
   xor rcx, rbp
   add rax, rcx
   mov QWORD PTR [rsp + 96], rax
   add r11, rax
   add r11, QWORD PTR [rbx + 352]
   rorx rax, r8, 14
   rorx rcx, r8, 18
   rorx rdx, r8, 41
   xor rax, rcx
   xor rax, rdx
   add r11, rax
   mov rax, r9
   xor rax, r10
   and rax, r8
   xor rax, r10
   add r11, rax
   add r15, r11
   rorx rax, r12, 28
   rorx rcx, r12, 34
   rorx rdx, r12, 39
   xor rax, rcx
   xor rax, rdx
   add r11, rax
   mov rax, r12
   and rax, r13
   mov rcx, r12
   or rcx, r13
   and rcx, r14
   or rax, rcx
   add r11, rax
   /* FIPS 180-4 round 45. */
   mov rax, QWORD PTR [rsp + 112]
   rorx rcx, rax, 1
   rorx rdx, rax, 8
   shr rax, 7
   xor rax, rcx
   xor rax, rdx
   add rax, QWORD PTR [rsp + 104]
   add rax, QWORD PTR [rsp + 48]
   mov rcx, QWORD PTR [rsp + 88]
   rorx rdx, rcx, 19
   rorx rbp, rcx, 61
   shr rcx, 6
   xor rcx, rdx
   xor rcx, rbp
   add rax, rcx
   mov QWORD PTR [rsp + 104], rax
   add r10, rax
   add r10, QWORD PTR [rbx + 360]
   rorx rax, r15, 14
   rorx rcx, r15, 18
   rorx rdx, r15, 41
   xor rax, rcx
   xor rax, rdx
   add r10, rax
   mov rax, r8
   xor rax, r9
   and rax, r15
   xor rax, r9
   add r10, rax
   add r14, r10
   rorx rax, r11, 28
   rorx rcx, r11, 34
   rorx rdx, r11, 39
   xor rax, rcx
   xor rax, rdx
   add r10, rax
   mov rax, r11
   and rax, r12
   mov rcx, r11
   or rcx, r12
   and rcx, r13
   or rax, rcx
   add r10, rax
   /* FIPS 180-4 round 46. */
   mov rax, QWORD PTR [rsp + 120]
   rorx rcx, rax, 1
   rorx rdx, rax, 8
   shr rax, 7
   xor rax, rcx
   xor rax, rdx
   add rax, QWORD PTR [rsp + 112]
   add rax, QWORD PTR [rsp + 56]
   mov rcx, QWORD PTR [rsp + 96]
   rorx rdx, rcx, 19
   rorx rbp, rcx, 61
   shr rcx, 6
   xor rcx, rdx
   xor rcx, rbp
   add rax, rcx
   mov QWORD PTR [rsp + 112], rax
   add r9, rax
   add r9, QWORD PTR [rbx + 368]
   rorx rax, r14, 14
   rorx rcx, r14, 18
   rorx rdx, r14, 41
   xor rax, rcx
   xor rax, rdx
   add r9, rax
   mov rax, r15
   xor rax, r8
   and rax, r14
   xor rax, r8
   add r9, rax
   add r13, r9
   rorx rax, r10, 28
   rorx rcx, r10, 34
   rorx rdx, r10, 39
   xor rax, rcx
   xor rax, rdx
   add r9, rax
   mov rax, r10
   and rax, r11
   mov rcx, r10
   or rcx, r11
   and rcx, r12
   or rax, rcx
   add r9, rax
   /* FIPS 180-4 round 47. */
   mov rax, QWORD PTR [rsp + 0]
   rorx rcx, rax, 1
   rorx rdx, rax, 8
   shr rax, 7
   xor rax, rcx
   xor rax, rdx
   add rax, QWORD PTR [rsp + 120]
   add rax, QWORD PTR [rsp + 64]
   mov rcx, QWORD PTR [rsp + 104]
   rorx rdx, rcx, 19
   rorx rbp, rcx, 61
   shr rcx, 6
   xor rcx, rdx
   xor rcx, rbp
   add rax, rcx
   mov QWORD PTR [rsp + 120], rax
   add r8, rax
   add r8, QWORD PTR [rbx + 376]
   rorx rax, r13, 14
   rorx rcx, r13, 18
   rorx rdx, r13, 41
   xor rax, rcx
   xor rax, rdx
   add r8, rax
   mov rax, r14
   xor rax, r15
   and rax, r13
   xor rax, r15
   add r8, rax
   add r12, r8
   rorx rax, r9, 28
   rorx rcx, r9, 34
   rorx rdx, r9, 39
   xor rax, rcx
   xor rax, rdx
   add r8, rax
   mov rax, r9
   and rax, r10
   mov rcx, r9
   or rcx, r10
   and rcx, r11
   or rax, rcx
   add r8, rax
   /* FIPS 180-4 round 48. */
   mov rax, QWORD PTR [rsp + 8]
   rorx rcx, rax, 1
   rorx rdx, rax, 8
   shr rax, 7
   xor rax, rcx
   xor rax, rdx
   add rax, QWORD PTR [rsp + 0]
   add rax, QWORD PTR [rsp + 72]
   mov rcx, QWORD PTR [rsp + 112]
   rorx rdx, rcx, 19
   rorx rbp, rcx, 61
   shr rcx, 6
   xor rcx, rdx
   xor rcx, rbp
   add rax, rcx
   mov QWORD PTR [rsp + 0], rax
   add r15, rax
   add r15, QWORD PTR [rbx + 384]
   rorx rax, r12, 14
   rorx rcx, r12, 18
   rorx rdx, r12, 41
   xor rax, rcx
   xor rax, rdx
   add r15, rax
   mov rax, r13
   xor rax, r14
   and rax, r12
   xor rax, r14
   add r15, rax
   add r11, r15
   rorx rax, r8, 28
   rorx rcx, r8, 34
   rorx rdx, r8, 39
   xor rax, rcx
   xor rax, rdx
   add r15, rax
   mov rax, r8
   and rax, r9
   mov rcx, r8
   or rcx, r9
   and rcx, r10
   or rax, rcx
   add r15, rax
   /* FIPS 180-4 round 49. */
   mov rax, QWORD PTR [rsp + 16]
   rorx rcx, rax, 1
   rorx rdx, rax, 8
   shr rax, 7
   xor rax, rcx
   xor rax, rdx
   add rax, QWORD PTR [rsp + 8]
   add rax, QWORD PTR [rsp + 80]
   mov rcx, QWORD PTR [rsp + 120]
   rorx rdx, rcx, 19
   rorx rbp, rcx, 61
   shr rcx, 6
   xor rcx, rdx
   xor rcx, rbp
   add rax, rcx
   mov QWORD PTR [rsp + 8], rax
   add r14, rax
   add r14, QWORD PTR [rbx + 392]
   rorx rax, r11, 14
   rorx rcx, r11, 18
   rorx rdx, r11, 41
   xor rax, rcx
   xor rax, rdx
   add r14, rax
   mov rax, r12
   xor rax, r13
   and rax, r11
   xor rax, r13
   add r14, rax
   add r10, r14
   rorx rax, r15, 28
   rorx rcx, r15, 34
   rorx rdx, r15, 39
   xor rax, rcx
   xor rax, rdx
   add r14, rax
   mov rax, r15
   and rax, r8
   mov rcx, r15
   or rcx, r8
   and rcx, r9
   or rax, rcx
   add r14, rax
   /* FIPS 180-4 round 50. */
   mov rax, QWORD PTR [rsp + 24]
   rorx rcx, rax, 1
   rorx rdx, rax, 8
   shr rax, 7
   xor rax, rcx
   xor rax, rdx
   add rax, QWORD PTR [rsp + 16]
   add rax, QWORD PTR [rsp + 88]
   mov rcx, QWORD PTR [rsp + 0]
   rorx rdx, rcx, 19
   rorx rbp, rcx, 61
   shr rcx, 6
   xor rcx, rdx
   xor rcx, rbp
   add rax, rcx
   mov QWORD PTR [rsp + 16], rax
   add r13, rax
   add r13, QWORD PTR [rbx + 400]
   rorx rax, r10, 14
   rorx rcx, r10, 18
   rorx rdx, r10, 41
   xor rax, rcx
   xor rax, rdx
   add r13, rax
   mov rax, r11
   xor rax, r12
   and rax, r10
   xor rax, r12
   add r13, rax
   add r9, r13
   rorx rax, r14, 28
   rorx rcx, r14, 34
   rorx rdx, r14, 39
   xor rax, rcx
   xor rax, rdx
   add r13, rax
   mov rax, r14
   and rax, r15
   mov rcx, r14
   or rcx, r15
   and rcx, r8
   or rax, rcx
   add r13, rax
   /* FIPS 180-4 round 51. */
   mov rax, QWORD PTR [rsp + 32]
   rorx rcx, rax, 1
   rorx rdx, rax, 8
   shr rax, 7
   xor rax, rcx
   xor rax, rdx
   add rax, QWORD PTR [rsp + 24]
   add rax, QWORD PTR [rsp + 96]
   mov rcx, QWORD PTR [rsp + 8]
   rorx rdx, rcx, 19
   rorx rbp, rcx, 61
   shr rcx, 6
   xor rcx, rdx
   xor rcx, rbp
   add rax, rcx
   mov QWORD PTR [rsp + 24], rax
   add r12, rax
   add r12, QWORD PTR [rbx + 408]
   rorx rax, r9, 14
   rorx rcx, r9, 18
   rorx rdx, r9, 41
   xor rax, rcx
   xor rax, rdx
   add r12, rax
   mov rax, r10
   xor rax, r11
   and rax, r9
   xor rax, r11
   add r12, rax
   add r8, r12
   rorx rax, r13, 28
   rorx rcx, r13, 34
   rorx rdx, r13, 39
   xor rax, rcx
   xor rax, rdx
   add r12, rax
   mov rax, r13
   and rax, r14
   mov rcx, r13
   or rcx, r14
   and rcx, r15
   or rax, rcx
   add r12, rax
   /* FIPS 180-4 round 52. */
   mov rax, QWORD PTR [rsp + 40]
   rorx rcx, rax, 1
   rorx rdx, rax, 8
   shr rax, 7
   xor rax, rcx
   xor rax, rdx
   add rax, QWORD PTR [rsp + 32]
   add rax, QWORD PTR [rsp + 104]
   mov rcx, QWORD PTR [rsp + 16]
   rorx rdx, rcx, 19
   rorx rbp, rcx, 61
   shr rcx, 6
   xor rcx, rdx
   xor rcx, rbp
   add rax, rcx
   mov QWORD PTR [rsp + 32], rax
   add r11, rax
   add r11, QWORD PTR [rbx + 416]
   rorx rax, r8, 14
   rorx rcx, r8, 18
   rorx rdx, r8, 41
   xor rax, rcx
   xor rax, rdx
   add r11, rax
   mov rax, r9
   xor rax, r10
   and rax, r8
   xor rax, r10
   add r11, rax
   add r15, r11
   rorx rax, r12, 28
   rorx rcx, r12, 34
   rorx rdx, r12, 39
   xor rax, rcx
   xor rax, rdx
   add r11, rax
   mov rax, r12
   and rax, r13
   mov rcx, r12
   or rcx, r13
   and rcx, r14
   or rax, rcx
   add r11, rax
   /* FIPS 180-4 round 53. */
   mov rax, QWORD PTR [rsp + 48]
   rorx rcx, rax, 1
   rorx rdx, rax, 8
   shr rax, 7
   xor rax, rcx
   xor rax, rdx
   add rax, QWORD PTR [rsp + 40]
   add rax, QWORD PTR [rsp + 112]
   mov rcx, QWORD PTR [rsp + 24]
   rorx rdx, rcx, 19
   rorx rbp, rcx, 61
   shr rcx, 6
   xor rcx, rdx
   xor rcx, rbp
   add rax, rcx
   mov QWORD PTR [rsp + 40], rax
   add r10, rax
   add r10, QWORD PTR [rbx + 424]
   rorx rax, r15, 14
   rorx rcx, r15, 18
   rorx rdx, r15, 41
   xor rax, rcx
   xor rax, rdx
   add r10, rax
   mov rax, r8
   xor rax, r9
   and rax, r15
   xor rax, r9
   add r10, rax
   add r14, r10
   rorx rax, r11, 28
   rorx rcx, r11, 34
   rorx rdx, r11, 39
   xor rax, rcx
   xor rax, rdx
   add r10, rax
   mov rax, r11
   and rax, r12
   mov rcx, r11
   or rcx, r12
   and rcx, r13
   or rax, rcx
   add r10, rax
   /* FIPS 180-4 round 54. */
   mov rax, QWORD PTR [rsp + 56]
   rorx rcx, rax, 1
   rorx rdx, rax, 8
   shr rax, 7
   xor rax, rcx
   xor rax, rdx
   add rax, QWORD PTR [rsp + 48]
   add rax, QWORD PTR [rsp + 120]
   mov rcx, QWORD PTR [rsp + 32]
   rorx rdx, rcx, 19
   rorx rbp, rcx, 61
   shr rcx, 6
   xor rcx, rdx
   xor rcx, rbp
   add rax, rcx
   mov QWORD PTR [rsp + 48], rax
   add r9, rax
   add r9, QWORD PTR [rbx + 432]
   rorx rax, r14, 14
   rorx rcx, r14, 18
   rorx rdx, r14, 41
   xor rax, rcx
   xor rax, rdx
   add r9, rax
   mov rax, r15
   xor rax, r8
   and rax, r14
   xor rax, r8
   add r9, rax
   add r13, r9
   rorx rax, r10, 28
   rorx rcx, r10, 34
   rorx rdx, r10, 39
   xor rax, rcx
   xor rax, rdx
   add r9, rax
   mov rax, r10
   and rax, r11
   mov rcx, r10
   or rcx, r11
   and rcx, r12
   or rax, rcx
   add r9, rax
   /* FIPS 180-4 round 55. */
   mov rax, QWORD PTR [rsp + 64]
   rorx rcx, rax, 1
   rorx rdx, rax, 8
   shr rax, 7
   xor rax, rcx
   xor rax, rdx
   add rax, QWORD PTR [rsp + 56]
   add rax, QWORD PTR [rsp + 0]
   mov rcx, QWORD PTR [rsp + 40]
   rorx rdx, rcx, 19
   rorx rbp, rcx, 61
   shr rcx, 6
   xor rcx, rdx
   xor rcx, rbp
   add rax, rcx
   mov QWORD PTR [rsp + 56], rax
   add r8, rax
   add r8, QWORD PTR [rbx + 440]
   rorx rax, r13, 14
   rorx rcx, r13, 18
   rorx rdx, r13, 41
   xor rax, rcx
   xor rax, rdx
   add r8, rax
   mov rax, r14
   xor rax, r15
   and rax, r13
   xor rax, r15
   add r8, rax
   add r12, r8
   rorx rax, r9, 28
   rorx rcx, r9, 34
   rorx rdx, r9, 39
   xor rax, rcx
   xor rax, rdx
   add r8, rax
   mov rax, r9
   and rax, r10
   mov rcx, r9
   or rcx, r10
   and rcx, r11
   or rax, rcx
   add r8, rax
   /* FIPS 180-4 round 56. */
   mov rax, QWORD PTR [rsp + 72]
   rorx rcx, rax, 1
   rorx rdx, rax, 8
   shr rax, 7
   xor rax, rcx
   xor rax, rdx
   add rax, QWORD PTR [rsp + 64]
   add rax, QWORD PTR [rsp + 8]
   mov rcx, QWORD PTR [rsp + 48]
   rorx rdx, rcx, 19
   rorx rbp, rcx, 61
   shr rcx, 6
   xor rcx, rdx
   xor rcx, rbp
   add rax, rcx
   mov QWORD PTR [rsp + 64], rax
   add r15, rax
   add r15, QWORD PTR [rbx + 448]
   rorx rax, r12, 14
   rorx rcx, r12, 18
   rorx rdx, r12, 41
   xor rax, rcx
   xor rax, rdx
   add r15, rax
   mov rax, r13
   xor rax, r14
   and rax, r12
   xor rax, r14
   add r15, rax
   add r11, r15
   rorx rax, r8, 28
   rorx rcx, r8, 34
   rorx rdx, r8, 39
   xor rax, rcx
   xor rax, rdx
   add r15, rax
   mov rax, r8
   and rax, r9
   mov rcx, r8
   or rcx, r9
   and rcx, r10
   or rax, rcx
   add r15, rax
   /* FIPS 180-4 round 57. */
   mov rax, QWORD PTR [rsp + 80]
   rorx rcx, rax, 1
   rorx rdx, rax, 8
   shr rax, 7
   xor rax, rcx
   xor rax, rdx
   add rax, QWORD PTR [rsp + 72]
   add rax, QWORD PTR [rsp + 16]
   mov rcx, QWORD PTR [rsp + 56]
   rorx rdx, rcx, 19
   rorx rbp, rcx, 61
   shr rcx, 6
   xor rcx, rdx
   xor rcx, rbp
   add rax, rcx
   mov QWORD PTR [rsp + 72], rax
   add r14, rax
   add r14, QWORD PTR [rbx + 456]
   rorx rax, r11, 14
   rorx rcx, r11, 18
   rorx rdx, r11, 41
   xor rax, rcx
   xor rax, rdx
   add r14, rax
   mov rax, r12
   xor rax, r13
   and rax, r11
   xor rax, r13
   add r14, rax
   add r10, r14
   rorx rax, r15, 28
   rorx rcx, r15, 34
   rorx rdx, r15, 39
   xor rax, rcx
   xor rax, rdx
   add r14, rax
   mov rax, r15
   and rax, r8
   mov rcx, r15
   or rcx, r8
   and rcx, r9
   or rax, rcx
   add r14, rax
   /* FIPS 180-4 round 58. */
   mov rax, QWORD PTR [rsp + 88]
   rorx rcx, rax, 1
   rorx rdx, rax, 8
   shr rax, 7
   xor rax, rcx
   xor rax, rdx
   add rax, QWORD PTR [rsp + 80]
   add rax, QWORD PTR [rsp + 24]
   mov rcx, QWORD PTR [rsp + 64]
   rorx rdx, rcx, 19
   rorx rbp, rcx, 61
   shr rcx, 6
   xor rcx, rdx
   xor rcx, rbp
   add rax, rcx
   mov QWORD PTR [rsp + 80], rax
   add r13, rax
   add r13, QWORD PTR [rbx + 464]
   rorx rax, r10, 14
   rorx rcx, r10, 18
   rorx rdx, r10, 41
   xor rax, rcx
   xor rax, rdx
   add r13, rax
   mov rax, r11
   xor rax, r12
   and rax, r10
   xor rax, r12
   add r13, rax
   add r9, r13
   rorx rax, r14, 28
   rorx rcx, r14, 34
   rorx rdx, r14, 39
   xor rax, rcx
   xor rax, rdx
   add r13, rax
   mov rax, r14
   and rax, r15
   mov rcx, r14
   or rcx, r15
   and rcx, r8
   or rax, rcx
   add r13, rax
   /* FIPS 180-4 round 59. */
   mov rax, QWORD PTR [rsp + 96]
   rorx rcx, rax, 1
   rorx rdx, rax, 8
   shr rax, 7
   xor rax, rcx
   xor rax, rdx
   add rax, QWORD PTR [rsp + 88]
   add rax, QWORD PTR [rsp + 32]
   mov rcx, QWORD PTR [rsp + 72]
   rorx rdx, rcx, 19
   rorx rbp, rcx, 61
   shr rcx, 6
   xor rcx, rdx
   xor rcx, rbp
   add rax, rcx
   mov QWORD PTR [rsp + 88], rax
   add r12, rax
   add r12, QWORD PTR [rbx + 472]
   rorx rax, r9, 14
   rorx rcx, r9, 18
   rorx rdx, r9, 41
   xor rax, rcx
   xor rax, rdx
   add r12, rax
   mov rax, r10
   xor rax, r11
   and rax, r9
   xor rax, r11
   add r12, rax
   add r8, r12
   rorx rax, r13, 28
   rorx rcx, r13, 34
   rorx rdx, r13, 39
   xor rax, rcx
   xor rax, rdx
   add r12, rax
   mov rax, r13
   and rax, r14
   mov rcx, r13
   or rcx, r14
   and rcx, r15
   or rax, rcx
   add r12, rax
   /* FIPS 180-4 round 60. */
   mov rax, QWORD PTR [rsp + 104]
   rorx rcx, rax, 1
   rorx rdx, rax, 8
   shr rax, 7
   xor rax, rcx
   xor rax, rdx
   add rax, QWORD PTR [rsp + 96]
   add rax, QWORD PTR [rsp + 40]
   mov rcx, QWORD PTR [rsp + 80]
   rorx rdx, rcx, 19
   rorx rbp, rcx, 61
   shr rcx, 6
   xor rcx, rdx
   xor rcx, rbp
   add rax, rcx
   mov QWORD PTR [rsp + 96], rax
   add r11, rax
   add r11, QWORD PTR [rbx + 480]
   rorx rax, r8, 14
   rorx rcx, r8, 18
   rorx rdx, r8, 41
   xor rax, rcx
   xor rax, rdx
   add r11, rax
   mov rax, r9
   xor rax, r10
   and rax, r8
   xor rax, r10
   add r11, rax
   add r15, r11
   rorx rax, r12, 28
   rorx rcx, r12, 34
   rorx rdx, r12, 39
   xor rax, rcx
   xor rax, rdx
   add r11, rax
   mov rax, r12
   and rax, r13
   mov rcx, r12
   or rcx, r13
   and rcx, r14
   or rax, rcx
   add r11, rax
   /* FIPS 180-4 round 61. */
   mov rax, QWORD PTR [rsp + 112]
   rorx rcx, rax, 1
   rorx rdx, rax, 8
   shr rax, 7
   xor rax, rcx
   xor rax, rdx
   add rax, QWORD PTR [rsp + 104]
   add rax, QWORD PTR [rsp + 48]
   mov rcx, QWORD PTR [rsp + 88]
   rorx rdx, rcx, 19
   rorx rbp, rcx, 61
   shr rcx, 6
   xor rcx, rdx
   xor rcx, rbp
   add rax, rcx
   mov QWORD PTR [rsp + 104], rax
   add r10, rax
   add r10, QWORD PTR [rbx + 488]
   rorx rax, r15, 14
   rorx rcx, r15, 18
   rorx rdx, r15, 41
   xor rax, rcx
   xor rax, rdx
   add r10, rax
   mov rax, r8
   xor rax, r9
   and rax, r15
   xor rax, r9
   add r10, rax
   add r14, r10
   rorx rax, r11, 28
   rorx rcx, r11, 34
   rorx rdx, r11, 39
   xor rax, rcx
   xor rax, rdx
   add r10, rax
   mov rax, r11
   and rax, r12
   mov rcx, r11
   or rcx, r12
   and rcx, r13
   or rax, rcx
   add r10, rax
   /* FIPS 180-4 round 62. */
   mov rax, QWORD PTR [rsp + 120]
   rorx rcx, rax, 1
   rorx rdx, rax, 8
   shr rax, 7
   xor rax, rcx
   xor rax, rdx
   add rax, QWORD PTR [rsp + 112]
   add rax, QWORD PTR [rsp + 56]
   mov rcx, QWORD PTR [rsp + 96]
   rorx rdx, rcx, 19
   rorx rbp, rcx, 61
   shr rcx, 6
   xor rcx, rdx
   xor rcx, rbp
   add rax, rcx
   mov QWORD PTR [rsp + 112], rax
   add r9, rax
   add r9, QWORD PTR [rbx + 496]
   rorx rax, r14, 14
   rorx rcx, r14, 18
   rorx rdx, r14, 41
   xor rax, rcx
   xor rax, rdx
   add r9, rax
   mov rax, r15
   xor rax, r8
   and rax, r14
   xor rax, r8
   add r9, rax
   add r13, r9
   rorx rax, r10, 28
   rorx rcx, r10, 34
   rorx rdx, r10, 39
   xor rax, rcx
   xor rax, rdx
   add r9, rax
   mov rax, r10
   and rax, r11
   mov rcx, r10
   or rcx, r11
   and rcx, r12
   or rax, rcx
   add r9, rax
   /* FIPS 180-4 round 63. */
   mov rax, QWORD PTR [rsp + 0]
   rorx rcx, rax, 1
   rorx rdx, rax, 8
   shr rax, 7
   xor rax, rcx
   xor rax, rdx
   add rax, QWORD PTR [rsp + 120]
   add rax, QWORD PTR [rsp + 64]
   mov rcx, QWORD PTR [rsp + 104]
   rorx rdx, rcx, 19
   rorx rbp, rcx, 61
   shr rcx, 6
   xor rcx, rdx
   xor rcx, rbp
   add rax, rcx
   mov QWORD PTR [rsp + 120], rax
   add r8, rax
   add r8, QWORD PTR [rbx + 504]
   rorx rax, r13, 14
   rorx rcx, r13, 18
   rorx rdx, r13, 41
   xor rax, rcx
   xor rax, rdx
   add r8, rax
   mov rax, r14
   xor rax, r15
   and rax, r13
   xor rax, r15
   add r8, rax
   add r12, r8
   rorx rax, r9, 28
   rorx rcx, r9, 34
   rorx rdx, r9, 39
   xor rax, rcx
   xor rax, rdx
   add r8, rax
   mov rax, r9
   and rax, r10
   mov rcx, r9
   or rcx, r10
   and rcx, r11
   or rax, rcx
   add r8, rax
   /* FIPS 180-4 round 64. */
   mov rax, QWORD PTR [rsp + 8]
   rorx rcx, rax, 1
   rorx rdx, rax, 8
   shr rax, 7
   xor rax, rcx
   xor rax, rdx
   add rax, QWORD PTR [rsp + 0]
   add rax, QWORD PTR [rsp + 72]
   mov rcx, QWORD PTR [rsp + 112]
   rorx rdx, rcx, 19
   rorx rbp, rcx, 61
   shr rcx, 6
   xor rcx, rdx
   xor rcx, rbp
   add rax, rcx
   mov QWORD PTR [rsp + 0], rax
   add r15, rax
   add r15, QWORD PTR [rbx + 512]
   rorx rax, r12, 14
   rorx rcx, r12, 18
   rorx rdx, r12, 41
   xor rax, rcx
   xor rax, rdx
   add r15, rax
   mov rax, r13
   xor rax, r14
   and rax, r12
   xor rax, r14
   add r15, rax
   add r11, r15
   rorx rax, r8, 28
   rorx rcx, r8, 34
   rorx rdx, r8, 39
   xor rax, rcx
   xor rax, rdx
   add r15, rax
   mov rax, r8
   and rax, r9
   mov rcx, r8
   or rcx, r9
   and rcx, r10
   or rax, rcx
   add r15, rax
   /* FIPS 180-4 round 65. */
   mov rax, QWORD PTR [rsp + 16]
   rorx rcx, rax, 1
   rorx rdx, rax, 8
   shr rax, 7
   xor rax, rcx
   xor rax, rdx
   add rax, QWORD PTR [rsp + 8]
   add rax, QWORD PTR [rsp + 80]
   mov rcx, QWORD PTR [rsp + 120]
   rorx rdx, rcx, 19
   rorx rbp, rcx, 61
   shr rcx, 6
   xor rcx, rdx
   xor rcx, rbp
   add rax, rcx
   mov QWORD PTR [rsp + 8], rax
   add r14, rax
   add r14, QWORD PTR [rbx + 520]
   rorx rax, r11, 14
   rorx rcx, r11, 18
   rorx rdx, r11, 41
   xor rax, rcx
   xor rax, rdx
   add r14, rax
   mov rax, r12
   xor rax, r13
   and rax, r11
   xor rax, r13
   add r14, rax
   add r10, r14
   rorx rax, r15, 28
   rorx rcx, r15, 34
   rorx rdx, r15, 39
   xor rax, rcx
   xor rax, rdx
   add r14, rax
   mov rax, r15
   and rax, r8
   mov rcx, r15
   or rcx, r8
   and rcx, r9
   or rax, rcx
   add r14, rax
   /* FIPS 180-4 round 66. */
   mov rax, QWORD PTR [rsp + 24]
   rorx rcx, rax, 1
   rorx rdx, rax, 8
   shr rax, 7
   xor rax, rcx
   xor rax, rdx
   add rax, QWORD PTR [rsp + 16]
   add rax, QWORD PTR [rsp + 88]
   mov rcx, QWORD PTR [rsp + 0]
   rorx rdx, rcx, 19
   rorx rbp, rcx, 61
   shr rcx, 6
   xor rcx, rdx
   xor rcx, rbp
   add rax, rcx
   mov QWORD PTR [rsp + 16], rax
   add r13, rax
   add r13, QWORD PTR [rbx + 528]
   rorx rax, r10, 14
   rorx rcx, r10, 18
   rorx rdx, r10, 41
   xor rax, rcx
   xor rax, rdx
   add r13, rax
   mov rax, r11
   xor rax, r12
   and rax, r10
   xor rax, r12
   add r13, rax
   add r9, r13
   rorx rax, r14, 28
   rorx rcx, r14, 34
   rorx rdx, r14, 39
   xor rax, rcx
   xor rax, rdx
   add r13, rax
   mov rax, r14
   and rax, r15
   mov rcx, r14
   or rcx, r15
   and rcx, r8
   or rax, rcx
   add r13, rax
   /* FIPS 180-4 round 67. */
   mov rax, QWORD PTR [rsp + 32]
   rorx rcx, rax, 1
   rorx rdx, rax, 8
   shr rax, 7
   xor rax, rcx
   xor rax, rdx
   add rax, QWORD PTR [rsp + 24]
   add rax, QWORD PTR [rsp + 96]
   mov rcx, QWORD PTR [rsp + 8]
   rorx rdx, rcx, 19
   rorx rbp, rcx, 61
   shr rcx, 6
   xor rcx, rdx
   xor rcx, rbp
   add rax, rcx
   mov QWORD PTR [rsp + 24], rax
   add r12, rax
   add r12, QWORD PTR [rbx + 536]
   rorx rax, r9, 14
   rorx rcx, r9, 18
   rorx rdx, r9, 41
   xor rax, rcx
   xor rax, rdx
   add r12, rax
   mov rax, r10
   xor rax, r11
   and rax, r9
   xor rax, r11
   add r12, rax
   add r8, r12
   rorx rax, r13, 28
   rorx rcx, r13, 34
   rorx rdx, r13, 39
   xor rax, rcx
   xor rax, rdx
   add r12, rax
   mov rax, r13
   and rax, r14
   mov rcx, r13
   or rcx, r14
   and rcx, r15
   or rax, rcx
   add r12, rax
   /* FIPS 180-4 round 68. */
   mov rax, QWORD PTR [rsp + 40]
   rorx rcx, rax, 1
   rorx rdx, rax, 8
   shr rax, 7
   xor rax, rcx
   xor rax, rdx
   add rax, QWORD PTR [rsp + 32]
   add rax, QWORD PTR [rsp + 104]
   mov rcx, QWORD PTR [rsp + 16]
   rorx rdx, rcx, 19
   rorx rbp, rcx, 61
   shr rcx, 6
   xor rcx, rdx
   xor rcx, rbp
   add rax, rcx
   mov QWORD PTR [rsp + 32], rax
   add r11, rax
   add r11, QWORD PTR [rbx + 544]
   rorx rax, r8, 14
   rorx rcx, r8, 18
   rorx rdx, r8, 41
   xor rax, rcx
   xor rax, rdx
   add r11, rax
   mov rax, r9
   xor rax, r10
   and rax, r8
   xor rax, r10
   add r11, rax
   add r15, r11
   rorx rax, r12, 28
   rorx rcx, r12, 34
   rorx rdx, r12, 39
   xor rax, rcx
   xor rax, rdx
   add r11, rax
   mov rax, r12
   and rax, r13
   mov rcx, r12
   or rcx, r13
   and rcx, r14
   or rax, rcx
   add r11, rax
   /* FIPS 180-4 round 69. */
   mov rax, QWORD PTR [rsp + 48]
   rorx rcx, rax, 1
   rorx rdx, rax, 8
   shr rax, 7
   xor rax, rcx
   xor rax, rdx
   add rax, QWORD PTR [rsp + 40]
   add rax, QWORD PTR [rsp + 112]
   mov rcx, QWORD PTR [rsp + 24]
   rorx rdx, rcx, 19
   rorx rbp, rcx, 61
   shr rcx, 6
   xor rcx, rdx
   xor rcx, rbp
   add rax, rcx
   mov QWORD PTR [rsp + 40], rax
   add r10, rax
   add r10, QWORD PTR [rbx + 552]
   rorx rax, r15, 14
   rorx rcx, r15, 18
   rorx rdx, r15, 41
   xor rax, rcx
   xor rax, rdx
   add r10, rax
   mov rax, r8
   xor rax, r9
   and rax, r15
   xor rax, r9
   add r10, rax
   add r14, r10
   rorx rax, r11, 28
   rorx rcx, r11, 34
   rorx rdx, r11, 39
   xor rax, rcx
   xor rax, rdx
   add r10, rax
   mov rax, r11
   and rax, r12
   mov rcx, r11
   or rcx, r12
   and rcx, r13
   or rax, rcx
   add r10, rax
   /* FIPS 180-4 round 70. */
   mov rax, QWORD PTR [rsp + 56]
   rorx rcx, rax, 1
   rorx rdx, rax, 8
   shr rax, 7
   xor rax, rcx
   xor rax, rdx
   add rax, QWORD PTR [rsp + 48]
   add rax, QWORD PTR [rsp + 120]
   mov rcx, QWORD PTR [rsp + 32]
   rorx rdx, rcx, 19
   rorx rbp, rcx, 61
   shr rcx, 6
   xor rcx, rdx
   xor rcx, rbp
   add rax, rcx
   mov QWORD PTR [rsp + 48], rax
   add r9, rax
   add r9, QWORD PTR [rbx + 560]
   rorx rax, r14, 14
   rorx rcx, r14, 18
   rorx rdx, r14, 41
   xor rax, rcx
   xor rax, rdx
   add r9, rax
   mov rax, r15
   xor rax, r8
   and rax, r14
   xor rax, r8
   add r9, rax
   add r13, r9
   rorx rax, r10, 28
   rorx rcx, r10, 34
   rorx rdx, r10, 39
   xor rax, rcx
   xor rax, rdx
   add r9, rax
   mov rax, r10
   and rax, r11
   mov rcx, r10
   or rcx, r11
   and rcx, r12
   or rax, rcx
   add r9, rax
   /* FIPS 180-4 round 71. */
   mov rax, QWORD PTR [rsp + 64]
   rorx rcx, rax, 1
   rorx rdx, rax, 8
   shr rax, 7
   xor rax, rcx
   xor rax, rdx
   add rax, QWORD PTR [rsp + 56]
   add rax, QWORD PTR [rsp + 0]
   mov rcx, QWORD PTR [rsp + 40]
   rorx rdx, rcx, 19
   rorx rbp, rcx, 61
   shr rcx, 6
   xor rcx, rdx
   xor rcx, rbp
   add rax, rcx
   mov QWORD PTR [rsp + 56], rax
   add r8, rax
   add r8, QWORD PTR [rbx + 568]
   rorx rax, r13, 14
   rorx rcx, r13, 18
   rorx rdx, r13, 41
   xor rax, rcx
   xor rax, rdx
   add r8, rax
   mov rax, r14
   xor rax, r15
   and rax, r13
   xor rax, r15
   add r8, rax
   add r12, r8
   rorx rax, r9, 28
   rorx rcx, r9, 34
   rorx rdx, r9, 39
   xor rax, rcx
   xor rax, rdx
   add r8, rax
   mov rax, r9
   and rax, r10
   mov rcx, r9
   or rcx, r10
   and rcx, r11
   or rax, rcx
   add r8, rax
   /* FIPS 180-4 round 72. */
   mov rax, QWORD PTR [rsp + 72]
   rorx rcx, rax, 1
   rorx rdx, rax, 8
   shr rax, 7
   xor rax, rcx
   xor rax, rdx
   add rax, QWORD PTR [rsp + 64]
   add rax, QWORD PTR [rsp + 8]
   mov rcx, QWORD PTR [rsp + 48]
   rorx rdx, rcx, 19
   rorx rbp, rcx, 61
   shr rcx, 6
   xor rcx, rdx
   xor rcx, rbp
   add rax, rcx
   mov QWORD PTR [rsp + 64], rax
   add r15, rax
   add r15, QWORD PTR [rbx + 576]
   rorx rax, r12, 14
   rorx rcx, r12, 18
   rorx rdx, r12, 41
   xor rax, rcx
   xor rax, rdx
   add r15, rax
   mov rax, r13
   xor rax, r14
   and rax, r12
   xor rax, r14
   add r15, rax
   add r11, r15
   rorx rax, r8, 28
   rorx rcx, r8, 34
   rorx rdx, r8, 39
   xor rax, rcx
   xor rax, rdx
   add r15, rax
   mov rax, r8
   and rax, r9
   mov rcx, r8
   or rcx, r9
   and rcx, r10
   or rax, rcx
   add r15, rax
   /* FIPS 180-4 round 73. */
   mov rax, QWORD PTR [rsp + 80]
   rorx rcx, rax, 1
   rorx rdx, rax, 8
   shr rax, 7
   xor rax, rcx
   xor rax, rdx
   add rax, QWORD PTR [rsp + 72]
   add rax, QWORD PTR [rsp + 16]
   mov rcx, QWORD PTR [rsp + 56]
   rorx rdx, rcx, 19
   rorx rbp, rcx, 61
   shr rcx, 6
   xor rcx, rdx
   xor rcx, rbp
   add rax, rcx
   mov QWORD PTR [rsp + 72], rax
   add r14, rax
   add r14, QWORD PTR [rbx + 584]
   rorx rax, r11, 14
   rorx rcx, r11, 18
   rorx rdx, r11, 41
   xor rax, rcx
   xor rax, rdx
   add r14, rax
   mov rax, r12
   xor rax, r13
   and rax, r11
   xor rax, r13
   add r14, rax
   add r10, r14
   rorx rax, r15, 28
   rorx rcx, r15, 34
   rorx rdx, r15, 39
   xor rax, rcx
   xor rax, rdx
   add r14, rax
   mov rax, r15
   and rax, r8
   mov rcx, r15
   or rcx, r8
   and rcx, r9
   or rax, rcx
   add r14, rax
   /* FIPS 180-4 round 74. */
   mov rax, QWORD PTR [rsp + 88]
   rorx rcx, rax, 1
   rorx rdx, rax, 8
   shr rax, 7
   xor rax, rcx
   xor rax, rdx
   add rax, QWORD PTR [rsp + 80]
   add rax, QWORD PTR [rsp + 24]
   mov rcx, QWORD PTR [rsp + 64]
   rorx rdx, rcx, 19
   rorx rbp, rcx, 61
   shr rcx, 6
   xor rcx, rdx
   xor rcx, rbp
   add rax, rcx
   mov QWORD PTR [rsp + 80], rax
   add r13, rax
   add r13, QWORD PTR [rbx + 592]
   rorx rax, r10, 14
   rorx rcx, r10, 18
   rorx rdx, r10, 41
   xor rax, rcx
   xor rax, rdx
   add r13, rax
   mov rax, r11
   xor rax, r12
   and rax, r10
   xor rax, r12
   add r13, rax
   add r9, r13
   rorx rax, r14, 28
   rorx rcx, r14, 34
   rorx rdx, r14, 39
   xor rax, rcx
   xor rax, rdx
   add r13, rax
   mov rax, r14
   and rax, r15
   mov rcx, r14
   or rcx, r15
   and rcx, r8
   or rax, rcx
   add r13, rax
   /* FIPS 180-4 round 75. */
   mov rax, QWORD PTR [rsp + 96]
   rorx rcx, rax, 1
   rorx rdx, rax, 8
   shr rax, 7
   xor rax, rcx
   xor rax, rdx
   add rax, QWORD PTR [rsp + 88]
   add rax, QWORD PTR [rsp + 32]
   mov rcx, QWORD PTR [rsp + 72]
   rorx rdx, rcx, 19
   rorx rbp, rcx, 61
   shr rcx, 6
   xor rcx, rdx
   xor rcx, rbp
   add rax, rcx
   mov QWORD PTR [rsp + 88], rax
   add r12, rax
   add r12, QWORD PTR [rbx + 600]
   rorx rax, r9, 14
   rorx rcx, r9, 18
   rorx rdx, r9, 41
   xor rax, rcx
   xor rax, rdx
   add r12, rax
   mov rax, r10
   xor rax, r11
   and rax, r9
   xor rax, r11
   add r12, rax
   add r8, r12
   rorx rax, r13, 28
   rorx rcx, r13, 34
   rorx rdx, r13, 39
   xor rax, rcx
   xor rax, rdx
   add r12, rax
   mov rax, r13
   and rax, r14
   mov rcx, r13
   or rcx, r14
   and rcx, r15
   or rax, rcx
   add r12, rax
   /* FIPS 180-4 round 76. */
   mov rax, QWORD PTR [rsp + 104]
   rorx rcx, rax, 1
   rorx rdx, rax, 8
   shr rax, 7
   xor rax, rcx
   xor rax, rdx
   add rax, QWORD PTR [rsp + 96]
   add rax, QWORD PTR [rsp + 40]
   mov rcx, QWORD PTR [rsp + 80]
   rorx rdx, rcx, 19
   rorx rbp, rcx, 61
   shr rcx, 6
   xor rcx, rdx
   xor rcx, rbp
   add rax, rcx
   mov QWORD PTR [rsp + 96], rax
   add r11, rax
   add r11, QWORD PTR [rbx + 608]
   rorx rax, r8, 14
   rorx rcx, r8, 18
   rorx rdx, r8, 41
   xor rax, rcx
   xor rax, rdx
   add r11, rax
   mov rax, r9
   xor rax, r10
   and rax, r8
   xor rax, r10
   add r11, rax
   add r15, r11
   rorx rax, r12, 28
   rorx rcx, r12, 34
   rorx rdx, r12, 39
   xor rax, rcx
   xor rax, rdx
   add r11, rax
   mov rax, r12
   and rax, r13
   mov rcx, r12
   or rcx, r13
   and rcx, r14
   or rax, rcx
   add r11, rax
   /* FIPS 180-4 round 77. */
   mov rax, QWORD PTR [rsp + 112]
   rorx rcx, rax, 1
   rorx rdx, rax, 8
   shr rax, 7
   xor rax, rcx
   xor rax, rdx
   add rax, QWORD PTR [rsp + 104]
   add rax, QWORD PTR [rsp + 48]
   mov rcx, QWORD PTR [rsp + 88]
   rorx rdx, rcx, 19
   rorx rbp, rcx, 61
   shr rcx, 6
   xor rcx, rdx
   xor rcx, rbp
   add rax, rcx
   mov QWORD PTR [rsp + 104], rax
   add r10, rax
   add r10, QWORD PTR [rbx + 616]
   rorx rax, r15, 14
   rorx rcx, r15, 18
   rorx rdx, r15, 41
   xor rax, rcx
   xor rax, rdx
   add r10, rax
   mov rax, r8
   xor rax, r9
   and rax, r15
   xor rax, r9
   add r10, rax
   add r14, r10
   rorx rax, r11, 28
   rorx rcx, r11, 34
   rorx rdx, r11, 39
   xor rax, rcx
   xor rax, rdx
   add r10, rax
   mov rax, r11
   and rax, r12
   mov rcx, r11
   or rcx, r12
   and rcx, r13
   or rax, rcx
   add r10, rax
   /* FIPS 180-4 round 78. */
   mov rax, QWORD PTR [rsp + 120]
   rorx rcx, rax, 1
   rorx rdx, rax, 8
   shr rax, 7
   xor rax, rcx
   xor rax, rdx
   add rax, QWORD PTR [rsp + 112]
   add rax, QWORD PTR [rsp + 56]
   mov rcx, QWORD PTR [rsp + 96]
   rorx rdx, rcx, 19
   rorx rbp, rcx, 61
   shr rcx, 6
   xor rcx, rdx
   xor rcx, rbp
   add rax, rcx
   mov QWORD PTR [rsp + 112], rax
   add r9, rax
   add r9, QWORD PTR [rbx + 624]
   rorx rax, r14, 14
   rorx rcx, r14, 18
   rorx rdx, r14, 41
   xor rax, rcx
   xor rax, rdx
   add r9, rax
   mov rax, r15
   xor rax, r8
   and rax, r14
   xor rax, r8
   add r9, rax
   add r13, r9
   rorx rax, r10, 28
   rorx rcx, r10, 34
   rorx rdx, r10, 39
   xor rax, rcx
   xor rax, rdx
   add r9, rax
   mov rax, r10
   and rax, r11
   mov rcx, r10
   or rcx, r11
   and rcx, r12
   or rax, rcx
   add r9, rax
   /* FIPS 180-4 round 79. */
   mov rax, QWORD PTR [rsp + 0]
   rorx rcx, rax, 1
   rorx rdx, rax, 8
   shr rax, 7
   xor rax, rcx
   xor rax, rdx
   add rax, QWORD PTR [rsp + 120]
   add rax, QWORD PTR [rsp + 64]
   mov rcx, QWORD PTR [rsp + 104]
   rorx rdx, rcx, 19
   rorx rbp, rcx, 61
   shr rcx, 6
   xor rcx, rdx
   xor rcx, rbp
   add rax, rcx
   mov QWORD PTR [rsp + 120], rax
   add r8, rax
   add r8, QWORD PTR [rbx + 632]
   rorx rax, r13, 14
   rorx rcx, r13, 18
   rorx rdx, r13, 41
   xor rax, rcx
   xor rax, rdx
   add r8, rax
   mov rax, r14
   xor rax, r15
   and rax, r13
   xor rax, r15
   add r8, rax
   add r12, r8
   rorx rax, r9, 28
   rorx rcx, r9, 34
   rorx rdx, r9, 39
   xor rax, rcx
   xor rax, rdx
   add r8, rax
   mov rax, r9
   and rax, r10
   mov rcx, r9
   or rcx, r10
   and rcx, r11
   or rax, rcx
   add r8, rax
   add QWORD PTR [rdi + 16], r8
   add QWORD PTR [rdi + 24], r9
   add QWORD PTR [rdi + 32], r10
   add QWORD PTR [rdi + 40], r11
   add QWORD PTR [rdi + 48], r12
   add QWORD PTR [rdi + 56], r13
   add QWORD PTR [rdi + 64], r14
   add QWORD PTR [rdi + 72], r15
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
.size SharkSslSha384Ctx_processBMI2, .-SharkSslSha384Ctx_processBMI2

.p2align 4
.globl sharkssl_sha512_bmi2_probe
.type sharkssl_sha512_bmi2_probe, @function
sharkssl_sha512_bmi2_probe:
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
.size sharkssl_sha512_bmi2_probe, .-sharkssl_sha512_bmi2_probe
.section .note.GNU-stack,"",@progbits
