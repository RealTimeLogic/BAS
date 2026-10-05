/* Linux x86-64 ELF/GAS port of the corresponding X64_MSVC file.
 * The public SysV entry adapts arguments to the private kernel schedule.
 * No compiler ABI attributes are needed in common C. */
.intel_syntax noprefix
/* **************************************************************************** */
/* *     ____             _________                __                _ */
/* *    / __ \___  ____ _/ /_  __(_)___ ___  ___  / /   ____  ____ _(_)____ */
/* *   / /_/ / _ \/ __ `/ / / / / / __ `__ \/ _ \/ /   / __ \/ __ `/ / ___/ */
/* *  / _, _/  __/ /_/ / / / / / / / / / / / /  __/ /___/ /_/ /_/ / / /__/ */
/* * /_/ |_|\___/\__,_/_/ /_/ /_/_/ /_/ /_/\___/_____/\____/\__, /_/\___/ */
/* *                                                       /____/ */
/* * */
/* *                 SharkSSL Embedded SSL/TLS Stack */
/* **************************************************************************** */
/* *   PROGRAM MODULE */
/* * */
/* *   $Id: SharkSslCrypto_X64_Poly1305.asm 6169 2026-09-26 18:15:48Z wini $ */
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

/*  SharkSSL Poly1305 AVX2 backend for Microsoft x64 MASM. */

/*  SharkSslCrypto.c dispatches to this kernel only after a CPUID probe */
/*  reports AVX2, OSXSAVE and XMM/YMM state support. */

/*  U32 sharkssl_poly1305_process_avx2(SharkSslPoly1305Ctx *ctx, */
/*                                     const U8 *msg, U32 msg_len); */
/*    Processes the largest multiple of 64 bytes in msg_len (msg_len >= 64) */
/*    and returns the byte count. Every processed block adds the hibit */
/*    (ctx->flag * 2^128). The accumulator is stored back in the portable */
/*    C format (five 32-bit limbs) so the caller can finish the remainder. */

/*  Four 16-byte blocks are processed in parallel, in lane order 0, 2, 1, 3. */
/*  Interior groups advance independent chains with r^4; the final group uses */
/*  weights [r^4,r^2,r^3,r]. Sum the lanes and normalize only at the end of */
/*  this call. Per-lane carries are still propagated after every multiply. */


.equ POLY_FRAME_BYTES, 560

.section .rodata
.balign 16
poly_mask26: .quad 0x03ffffff, 0x03ffffff, 0x03ffffff, 0x03ffffff
poly_lane0: .quad 0x0FFFFFFFFFFFFFFFF, 0, 0, 0

.text

/*  h in ymm0..ymm4; powers addresses five vectors of 26-bit key limbs. */
/*  Default power storage is [rsp+160+32k] (k=0..4). */
/*  Clobbers ymm5..ymm15. Result (normalized 26-bit limbs) in ymm5..ymm9. */
.macro POLY_MUL powers=rsp+160
    vpmuludq ymm5, ymm0, YMMWORD PTR [\powers+0]
    vpmuludq ymm6, ymm0, YMMWORD PTR [\powers+32]
    vpmuludq ymm14, ymm1, YMMWORD PTR [\powers+0]
    vpaddq ymm6, ymm6, ymm14
    vpmuludq ymm7, ymm0, YMMWORD PTR [\powers+64]
    vpmuludq ymm14, ymm1, YMMWORD PTR [\powers+32]
    vpaddq ymm7, ymm7, ymm14
    vpmuludq ymm14, ymm2, YMMWORD PTR [\powers+0]
    vpaddq ymm7, ymm7, ymm14
    vpmuludq ymm8, ymm0, YMMWORD PTR [\powers+96]
    vpmuludq ymm14, ymm1, YMMWORD PTR [\powers+64]
    vpaddq ymm8, ymm8, ymm14
    vpmuludq ymm14, ymm2, YMMWORD PTR [\powers+32]
    vpaddq ymm8, ymm8, ymm14
    vpmuludq ymm14, ymm3, YMMWORD PTR [\powers+0]
    vpaddq ymm8, ymm8, ymm14
    vpmuludq ymm9, ymm0, YMMWORD PTR [\powers+128]
    vpmuludq ymm14, ymm1, YMMWORD PTR [\powers+96]
    vpaddq ymm9, ymm9, ymm14
    vpmuludq ymm14, ymm2, YMMWORD PTR [\powers+64]
    vpaddq ymm9, ymm9, ymm14
    vpmuludq ymm14, ymm3, YMMWORD PTR [\powers+32]
    vpaddq ymm9, ymm9, ymm14
    vpmuludq ymm14, ymm4, YMMWORD PTR [\powers+0]
    vpaddq ymm9, ymm9, ymm14
    vpmuludq ymm10, ymm1, YMMWORD PTR [\powers+128]
    vpmuludq ymm14, ymm2, YMMWORD PTR [\powers+96]
    vpaddq ymm10, ymm10, ymm14
    vpmuludq ymm14, ymm3, YMMWORD PTR [\powers+64]
    vpaddq ymm10, ymm10, ymm14
    vpmuludq ymm14, ymm4, YMMWORD PTR [\powers+32]
    vpaddq ymm10, ymm10, ymm14
    vpmuludq ymm11, ymm2, YMMWORD PTR [\powers+128]
    vpmuludq ymm14, ymm3, YMMWORD PTR [\powers+96]
    vpaddq ymm11, ymm11, ymm14
    vpmuludq ymm14, ymm4, YMMWORD PTR [\powers+64]
    vpaddq ymm11, ymm11, ymm14
    vpmuludq ymm12, ymm3, YMMWORD PTR [\powers+128]
    vpmuludq ymm14, ymm4, YMMWORD PTR [\powers+96]
    vpaddq ymm12, ymm12, ymm14
    vpmuludq ymm13, ymm4, YMMWORD PTR [\powers+128]

/*  fold t5..t8 into t0..t3 (2^130 = 5), 5x = (x << 2) + x */
    vpsllq ymm14, ymm10, 2
    vpaddq ymm14, ymm14, ymm10
    vpaddq ymm5, ymm5, ymm14
    vpsllq ymm14, ymm11, 2
    vpaddq ymm14, ymm14, ymm11
    vpaddq ymm6, ymm6, ymm14
    vpsllq ymm14, ymm12, 2
    vpaddq ymm14, ymm14, ymm12
    vpaddq ymm7, ymm7, ymm14
    vpsllq ymm14, ymm13, 2
    vpaddq ymm14, ymm14, ymm13
    vpaddq ymm8, ymm8, ymm14

/*  carry t0..t4, then fold t4's overflow */
    vpsrlq ymm14, ymm5, 26
    vpand ymm5, ymm5, YMMWORD PTR [rip+poly_mask26]
    vpaddq ymm6, ymm6, ymm14
    vpsrlq ymm14, ymm6, 26
    vpand ymm6, ymm6, YMMWORD PTR [rip+poly_mask26]
    vpaddq ymm7, ymm7, ymm14
    vpsrlq ymm14, ymm7, 26
    vpand ymm7, ymm7, YMMWORD PTR [rip+poly_mask26]
    vpaddq ymm8, ymm8, ymm14
    vpsrlq ymm14, ymm8, 26
    vpand ymm8, ymm8, YMMWORD PTR [rip+poly_mask26]
    vpaddq ymm9, ymm9, ymm14
    vpsrlq ymm14, ymm9, 26
    vpand ymm9, ymm9, YMMWORD PTR [rip+poly_mask26]
    vpsllq ymm15, ymm14, 2
    vpaddq ymm15, ymm15, ymm14
    vpaddq ymm5, ymm5, ymm15

/*  final carry chain */
    vpsrlq ymm14, ymm5, 26
    vpand ymm5, ymm5, YMMWORD PTR [rip+poly_mask26]
    vpaddq ymm6, ymm6, ymm14
    vpsrlq ymm14, ymm6, 26
    vpand ymm6, ymm6, YMMWORD PTR [rip+poly_mask26]
    vpaddq ymm7, ymm7, ymm14
    vpsrlq ymm14, ymm7, 26
    vpand ymm7, ymm7, YMMWORD PTR [rip+poly_mask26]
    vpaddq ymm8, ymm8, ymm14
    vpsrlq ymm14, ymm8, 26
    vpand ymm8, ymm8, YMMWORD PTR [rip+poly_mask26]
    vpaddq ymm9, ymm9, ymm14
    vpsrlq ymm14, ymm9, 26
    vpand ymm9, ymm9, YMMWORD PTR [rip+poly_mask26]
    vpsllq ymm15, ymm14, 2
    vpaddq ymm15, ymm15, ymm14
    vpaddq ymm5, ymm5, ymm15
    vpsrlq ymm14, ymm5, 26
    vpand ymm5, ymm5, YMMWORD PTR [rip+poly_mask26]
    vpaddq ymm6, ymm6, ymm14
    vpsrlq ymm14, ymm6, 26
    vpand ymm6, ymm6, YMMWORD PTR [rip+poly_mask26]
    vpaddq ymm7, ymm7, ymm14
    vpsrlq ymm14, ymm7, 26
    vpand ymm7, ymm7, YMMWORD PTR [rip+poly_mask26]
    vpaddq ymm8, ymm8, ymm14
    vpsrlq ymm14, ymm8, 26
    vpand ymm8, ymm8, YMMWORD PTR [rip+poly_mask26]
    vpaddq ymm9, ymm9, ymm14
.endm

/*  Stack layout after the prologue: */
/*    [rsp+0..160)    saved xmm6..xmm15 */
/*    [rsp+160..320)  per-lane key power slots, limb k at [rsp+160+32k] */
/*    [rsp+320..340)  R1 limbs  [rsp+340..360)  R2 limbs */
/*    [rsp+360..380)  R3 limbs  [rsp+380..400)  R4 limbs */
/*    [rsp+400..480)  unused during scalar power setup */
/*    [rsp+480..512)  hibit broadcast */
/*    [rsp+512..532)  h limb dwords h0..h4 */
/*    [rsp+532..536)  saved msg_len */
/*    [rsp+320..480)  reused after setup for broadcast r^4 (interior chains) */

.globl sharkssl_poly1305_process_avx2
.type sharkssl_poly1305_process_avx2, @function
sharkssl_poly1305_process_avx2:
.cfi_startproc
    sub rsp, 40
.cfi_adjust_cfa_offset 40
    mov r10, rcx
    mov r11, rdx
    mov rcx, rdi
    mov rdx, rsi
    mov r8, r11
    mov r9, r10
    call .Lsharkssl_poly1305_process_avx2
    add rsp, 40
.cfi_adjust_cfa_offset -40
    ret
.cfi_endproc
.size sharkssl_poly1305_process_avx2, .-sharkssl_poly1305_process_avx2
.type .Lsharkssl_poly1305_process_avx2, @function
.Lsharkssl_poly1305_process_avx2:
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
    sub rsp, POLY_FRAME_BYTES
.cfi_adjust_cfa_offset 560
    vmovdqu XMMWORD PTR [rsp], xmm6
    vmovdqu XMMWORD PTR [rsp+16], xmm7
    vmovdqu XMMWORD PTR [rsp+32], xmm8
    vmovdqu XMMWORD PTR [rsp+48], xmm9
    vmovdqu XMMWORD PTR [rsp+64], xmm10
    vmovdqu XMMWORD PTR [rsp+80], xmm11
    vmovdqu XMMWORD PTR [rsp+96], xmm12
    vmovdqu XMMWORD PTR [rsp+112], xmm13
    vmovdqu XMMWORD PTR [rsp+128], xmm14
    vmovdqu XMMWORD PTR [rsp+144], xmm15

    mov DWORD PTR [rsp+532], r8d
    mov eax, r8d
    and eax, 0x0FFFFFFC0
    mov r8d, eax

/*  Convert the key, already clamped by the C constructor, to 26-bit limbs. */
    mov r10d, DWORD PTR [rcx+20]
    mov r11d, DWORD PTR [rcx+24]
    mov ebx, DWORD PTR [rcx+28]
    mov esi, DWORD PTR [rcx+32]
    mov edi, r10d
    and edi, 0x03ffffff
    mov DWORD PTR [rsp+320], edi
    shr r10d, 26
    mov edi, r11d
    shl edi, 6
    and edi, 0x03ffffff
    or r10d, edi
    mov DWORD PTR [rsp+324], r10d
    shr r11d, 20
    mov edi, ebx
    shl edi, 12
    and edi, 0x03ffffff
    or r11d, edi
    mov DWORD PTR [rsp+328], r11d
    shr ebx, 14
    mov edi, esi
    shl edi, 18
    and edi, 0x03ffffff
    or ebx, edi
    mov DWORD PTR [rsp+332], ebx
    shr esi, 8
    mov DWORD PTR [rsp+336], esi

/*  Broadcast R1 into the power slots and compute R2 = R1^2. */
    vpbroadcastd ymm0, DWORD PTR [rsp+320]
    vmovdqu YMMWORD PTR [rsp+160], ymm0
    vpbroadcastd ymm1, DWORD PTR [rsp+324]
    vmovdqu YMMWORD PTR [rsp+192], ymm1
    vpbroadcastd ymm2, DWORD PTR [rsp+328]
    vmovdqu YMMWORD PTR [rsp+224], ymm2
    vpbroadcastd ymm3, DWORD PTR [rsp+332]
    vmovdqu YMMWORD PTR [rsp+256], ymm3
    vpbroadcastd ymm4, DWORD PTR [rsp+336]
    vmovdqu YMMWORD PTR [rsp+288], ymm4
    POLY_MUL
    vmovd DWORD PTR [rsp+340], xmm5
    vmovd DWORD PTR [rsp+344], xmm6
    vmovd DWORD PTR [rsp+348], xmm7
    vmovd DWORD PTR [rsp+352], xmm8
    vmovd DWORD PTR [rsp+356], xmm9

/*  R3 = R2 * R1. */
    vpbroadcastd ymm0, DWORD PTR [rsp+340]
    vpbroadcastd ymm1, DWORD PTR [rsp+344]
    vpbroadcastd ymm2, DWORD PTR [rsp+348]
    vpbroadcastd ymm3, DWORD PTR [rsp+352]
    vpbroadcastd ymm4, DWORD PTR [rsp+356]
    POLY_MUL
    vmovd DWORD PTR [rsp+360], xmm5
    vmovd DWORD PTR [rsp+364], xmm6
    vmovd DWORD PTR [rsp+368], xmm7
    vmovd DWORD PTR [rsp+372], xmm8
    vmovd DWORD PTR [rsp+376], xmm9

/*  R4 = R3 * R1. */
    vpbroadcastd ymm0, DWORD PTR [rsp+360]
    vpbroadcastd ymm1, DWORD PTR [rsp+364]
    vpbroadcastd ymm2, DWORD PTR [rsp+368]
    vpbroadcastd ymm3, DWORD PTR [rsp+372]
    vpbroadcastd ymm4, DWORD PTR [rsp+376]
    POLY_MUL
    vmovd DWORD PTR [rsp+380], xmm5
    vmovd DWORD PTR [rsp+384], xmm6
    vmovd DWORD PTR [rsp+388], xmm7
    vmovd DWORD PTR [rsp+392], xmm8
    vmovd DWORD PTR [rsp+396], xmm9

/*  Build the per-lane power blocks: lane j carries [R4_k R2_k R3_k R1_k] */
/*  for blocks 0, 2, 1, 3. */
    vpinsrd xmm14, xmm14, DWORD PTR [rsp+380], 0
    vpinsrd xmm14, xmm14, DWORD PTR [rsp+340], 1
    vpinsrd xmm14, xmm14, DWORD PTR [rsp+360], 2
    vpinsrd xmm14, xmm14, DWORD PTR [rsp+320], 3
    vpmovzxdq ymm14, xmm14
    vmovdqu YMMWORD PTR [rsp+160], ymm14
    vpinsrd xmm14, xmm14, DWORD PTR [rsp+384], 0
    vpinsrd xmm14, xmm14, DWORD PTR [rsp+344], 1
    vpinsrd xmm14, xmm14, DWORD PTR [rsp+364], 2
    vpinsrd xmm14, xmm14, DWORD PTR [rsp+324], 3
    vpmovzxdq ymm14, xmm14
    vmovdqu YMMWORD PTR [rsp+192], ymm14
    vpinsrd xmm14, xmm14, DWORD PTR [rsp+388], 0
    vpinsrd xmm14, xmm14, DWORD PTR [rsp+348], 1
    vpinsrd xmm14, xmm14, DWORD PTR [rsp+368], 2
    vpinsrd xmm14, xmm14, DWORD PTR [rsp+328], 3
    vpmovzxdq ymm14, xmm14
    vmovdqu YMMWORD PTR [rsp+224], ymm14
    vpinsrd xmm14, xmm14, DWORD PTR [rsp+392], 0
    vpinsrd xmm14, xmm14, DWORD PTR [rsp+352], 1
    vpinsrd xmm14, xmm14, DWORD PTR [rsp+372], 2
    vpinsrd xmm14, xmm14, DWORD PTR [rsp+332], 3
    vpmovzxdq ymm14, xmm14
    vmovdqu YMMWORD PTR [rsp+256], ymm14
    vpinsrd xmm14, xmm14, DWORD PTR [rsp+396], 0
    vpinsrd xmm14, xmm14, DWORD PTR [rsp+356], 1
    vpinsrd xmm14, xmm14, DWORD PTR [rsp+376], 2
    vpinsrd xmm14, xmm14, DWORD PTR [rsp+336], 3
    vpmovzxdq ymm14, xmm14
    vmovdqu YMMWORD PTR [rsp+288], ymm14

/*  Once weighted powers are packed, reuse the scalar power scratch for r^4. */
    vpbroadcastd ymm0, DWORD PTR [rsp+380]
    vpbroadcastd ymm1, DWORD PTR [rsp+384]
    vpbroadcastd ymm2, DWORD PTR [rsp+388]
    vpbroadcastd ymm3, DWORD PTR [rsp+392]
    vpbroadcastd ymm4, DWORD PTR [rsp+396]
    vmovdqu YMMWORD PTR [rsp+320], ymm0
    vmovdqu YMMWORD PTR [rsp+352], ymm1
    vmovdqu YMMWORD PTR [rsp+384], ymm2
    vmovdqu YMMWORD PTR [rsp+416], ymm3
    vmovdqu YMMWORD PTR [rsp+448], ymm4

/*  hibit = flag << 24 broadcast. */
    movzx ebx, BYTE PTR [rcx+68]
    shl ebx, 24
    vmovd xmm14, ebx
    vpbroadcastd ymm14, xmm14
    vmovdqu YMMWORD PTR [rsp+480], ymm14

/*  Convert the portable accumulator (five 32-bit limbs) into 26-bit */
/*  limbs and broadcast across all lanes. r[4] may exceed three bits; */
/*  fold its 2^130 multiples into h0. */
    mov r10d, DWORD PTR [rcx]
    mov r11d, DWORD PTR [rcx+4]
    mov ebx, DWORD PTR [rcx+8]
    mov esi, DWORD PTR [rcx+12]
    mov edi, DWORD PTR [rcx+16]
    mov eax, r10d
    and eax, 0x03ffffff
    mov r9d, edi
    shr r9d, 2
    lea r9d, [r9d+r9d*4]
    add eax, r9d
    mov DWORD PTR [rsp+512], eax
    shr r10d, 26
    mov eax, r11d
    shl eax, 6
    and eax, 0x03ffffff
    or r10d, eax
    mov DWORD PTR [rsp+516], r10d
    shr r11d, 20
    mov eax, ebx
    shl eax, 12
    and eax, 0x03ffffff
    or r11d, eax
    mov DWORD PTR [rsp+520], r11d
    shr ebx, 14
    mov eax, esi
    shl eax, 18
    and eax, 0x03ffffff
    or ebx, eax
    mov DWORD PTR [rsp+524], ebx
    shr esi, 8
    and edi, 3
    shl edi, 24
    or esi, edi
    mov DWORD PTR [rsp+528], esi
    vpbroadcastd ymm0, DWORD PTR [rsp+512]
    vpbroadcastd ymm1, DWORD PTR [rsp+516]
    vpbroadcastd ymm2, DWORD PTR [rsp+520]
    vpbroadcastd ymm3, DWORD PTR [rsp+524]
    vpbroadcastd ymm4, DWORD PTR [rsp+528]
    vpand ymm0, ymm0, YMMWORD PTR [rip+poly_lane0]
    vpand ymm1, ymm1, YMMWORD PTR [rip+poly_lane0]
    vpand ymm2, ymm2, YMMWORD PTR [rip+poly_lane0]
    vpand ymm3, ymm3, YMMWORD PTR [rip+poly_lane0]
    vpand ymm4, ymm4, YMMWORD PTR [rip+poly_lane0]

/*  For G groups, lane j accumulates its messages in radix r^4. Only */
/*  lane 0 starts with h. Final weights [r^4,r^2,r^3,r] reconstruct the */
/*  ordinary Horner recurrence and advance the incoming h by r^(4G). */
    poly_group_loop:
    lea r9, [rsp+320]
    lea r10, [rsp+160]
    cmp r8d, 64
    cmove r9, r10
/*  Load four blocks and build the per-lane (d0,d1) pairs in ymm12 (G) */
/*  and (d2,d3) pairs in ymm13 (H). Lane j holds block bj with */
/*  b0..b3 = blocks 0, 2, 1, 3. */
    vmovdqu ymm14, YMMWORD PTR [rdx]
    vmovdqu ymm15, YMMWORD PTR [rdx+32]
    vshufps ymm12, ymm14, ymm15, 0x44
    vshufps ymm13, ymm14, ymm15, 0x0EE

/*  Extract the 26-bit message limbs, add the hibit, and fold into h. */
    vpand ymm5, ymm12, YMMWORD PTR [rip+poly_mask26]
    vpaddq ymm0, ymm0, ymm5
    vpsrlq ymm5, ymm12, 26
    vpand ymm5, ymm5, YMMWORD PTR [rip+poly_mask26]
    vpaddq ymm1, ymm1, ymm5
    vpsrlq ymm5, ymm12, 52
    vpsllq ymm14, ymm13, 12
    vpand ymm14, ymm14, YMMWORD PTR [rip+poly_mask26]
    vpor ymm5, ymm5, ymm14
    vpaddq ymm2, ymm2, ymm5
    vpsrlq ymm5, ymm13, 14
    vpand ymm5, ymm5, YMMWORD PTR [rip+poly_mask26]
    vpaddq ymm3, ymm3, ymm5
    vpsrlq ymm5, ymm13, 40
    vpor ymm5, ymm5, YMMWORD PTR [rsp+480]
    vpaddq ymm4, ymm4, ymm5

    POLY_MUL r9

    add rdx, 64
    sub r8d, 64
    jz poly_combine_lanes
    vmovdqa ymm0, ymm5
    vmovdqa ymm1, ymm6
    vmovdqa ymm2, ymm7
    vmovdqa ymm3, ymm8
    vmovdqa ymm4, ymm9
    jmp poly_group_loop

    poly_combine_lanes:

/*  Sum the four lanes horizontally into lane 0 of xmm5..xmm9. */
    vextracti128 xmm14, ymm5, 1
    vpaddq xmm5, xmm5, xmm14
    vpshufd xmm14, xmm5, 0x0EE
    vpaddq xmm5, xmm5, xmm14
    vextracti128 xmm14, ymm6, 1
    vpaddq xmm6, xmm6, xmm14
    vpshufd xmm14, xmm6, 0x0EE
    vpaddq xmm6, xmm6, xmm14
    vextracti128 xmm14, ymm7, 1
    vpaddq xmm7, xmm7, xmm14
    vpshufd xmm14, xmm7, 0x0EE
    vpaddq xmm7, xmm7, xmm14
    vextracti128 xmm14, ymm8, 1
    vpaddq xmm8, xmm8, xmm14
    vpshufd xmm14, xmm8, 0x0EE
    vpaddq xmm8, xmm8, xmm14
    vextracti128 xmm14, ymm9, 1
    vpaddq xmm9, xmm9, xmm14
    vpshufd xmm14, xmm9, 0x0EE
    vpaddq xmm9, xmm9, xmm14

/*  Normalize the summed limbs. */
    vpsrlq xmm14, xmm5, 26
    vpand xmm5, xmm5, XMMWORD PTR [rip+poly_mask26]
    vpaddq xmm6, xmm6, xmm14
    vpsrlq xmm14, xmm6, 26
    vpand xmm6, xmm6, XMMWORD PTR [rip+poly_mask26]
    vpaddq xmm7, xmm7, xmm14
    vpsrlq xmm14, xmm7, 26
    vpand xmm7, xmm7, XMMWORD PTR [rip+poly_mask26]
    vpaddq xmm8, xmm8, xmm14
    vpsrlq xmm14, xmm8, 26
    vpand xmm8, xmm8, XMMWORD PTR [rip+poly_mask26]
    vpaddq xmm9, xmm9, xmm14
    vpsrlq xmm14, xmm9, 26
    vpand xmm9, xmm9, XMMWORD PTR [rip+poly_mask26]
    vpsllq xmm15, xmm14, 2
    vpaddq xmm15, xmm15, xmm14
    vpaddq xmm5, xmm5, xmm15
    vpsrlq xmm14, xmm5, 26
    vpand xmm5, xmm5, XMMWORD PTR [rip+poly_mask26]
    vpaddq xmm6, xmm6, xmm14
    vpsrlq xmm14, xmm6, 26
    vpand xmm6, xmm6, XMMWORD PTR [rip+poly_mask26]
    vpaddq xmm7, xmm7, xmm14
    vpsrlq xmm14, xmm7, 26
    vpand xmm7, xmm7, XMMWORD PTR [rip+poly_mask26]
    vpaddq xmm8, xmm8, xmm14
    vpsrlq xmm14, xmm8, 26
    vpand xmm8, xmm8, XMMWORD PTR [rip+poly_mask26]
    vpaddq xmm9, xmm9, xmm14

    poly_groups_done:
/*  Convert the 26-bit limbs back to the portable five-word format and */
/*  store into ctx->r. */
    vpsllq xmm14, xmm6, 26
    vpaddq xmm5, xmm5, xmm14
    vpsrlq xmm14, xmm5, 32
    vpsllq xmm10, xmm7, 20
    vpaddq xmm6, xmm10, xmm14
    vpsrlq xmm14, xmm6, 32
    vpsllq xmm10, xmm8, 14
    vpaddq xmm7, xmm10, xmm14
    vpsrlq xmm14, xmm7, 32
    vpsllq xmm10, xmm9, 8
    vpaddq xmm8, xmm10, xmm14
    vpsrlq xmm9, xmm8, 32
    vmovd DWORD PTR [rcx], xmm5
    vmovd DWORD PTR [rcx+4], xmm6
    vmovd DWORD PTR [rcx+8], xmm7
    vmovd DWORD PTR [rcx+12], xmm8
    vmovd DWORD PTR [rcx+16], xmm9

    mov eax, DWORD PTR [rsp+532]
    and eax, 0x0FFFFFFC0

    vmovdqu xmm6, XMMWORD PTR [rsp]
    vmovdqu xmm7, XMMWORD PTR [rsp+16]
    vmovdqu xmm8, XMMWORD PTR [rsp+32]
    vmovdqu xmm9, XMMWORD PTR [rsp+48]
    vmovdqu xmm10, XMMWORD PTR [rsp+64]
    vmovdqu xmm11, XMMWORD PTR [rsp+80]
    vmovdqu xmm12, XMMWORD PTR [rsp+96]
    vmovdqu xmm13, XMMWORD PTR [rsp+112]
    vmovdqu xmm14, XMMWORD PTR [rsp+128]
    vmovdqu xmm15, XMMWORD PTR [rsp+144]
/*  Keep the Windows unwind-recognized epilogue (add/pop/ret) contiguous. */
    vzeroupper
    add rsp, POLY_FRAME_BYTES
.cfi_adjust_cfa_offset -560
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
.size .Lsharkssl_poly1305_process_avx2, .-.Lsharkssl_poly1305_process_avx2

.section .note.GNU-stack,"",@progbits
