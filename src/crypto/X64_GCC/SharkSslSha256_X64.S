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
 *   $Id: SharkSslSha256_X64.S 6792 2026-11-07 21:44:49Z gianluca $
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
 * Original SHA-256 compressor derived from FIPS 180-4 section 6.2.2 and
 * Intel SHA256RNDS2/SHA256MSG1/SHA256MSG2 instruction semantics.
 * https://doi.org/10.6028/NIST.FIPS.180-4
 * https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sha-extensions.html
 *
 * One 64-byte block; the native U32 state begins at context byte offset 8.
 * The caller supplies the 64 FIPS round constants as native U32 words.
 * Context, input and constants need no vector alignment. SHA, SSSE3 and
 * SSE2 must be checked before entry. The only branches are in the public
 * CPU probe: the compressor has fixed addresses and instruction count.
 * Schedule W[64], saved state and all six working XMM registers are erased.
 * No third-party SHA implementation is used.
 */
.intel_syntax noprefix
.text
.p2align 4
.globl SharkSslSha256Ctx_processSHA
.type SharkSslSha256Ctx_processSHA, @function
SharkSslSha256Ctx_processSHA:
   .cfi_startproc
   sub rsp, 312
   .cfi_def_cfa_offset 320
   /* Byte-reversal indices, not secret data. */
   mov DWORD PTR [rsp + 288], 0x00010203
   mov DWORD PTR [rsp + 292], 0x04050607
   mov DWORD PTR [rsp + 296], 0x08090a0b
   mov DWORD PTR [rsp + 300], 0x0c0d0e0f

   /* Low-to-high lanes: xmm4 = F,E,B,A; xmm5 = H,G,D,C. */
   movdqu xmm1, XMMWORD PTR [rdi + 8]
   movdqu xmm2, XMMWORD PTR [rdi + 24]
   pshufd xmm1, xmm1, 0xb1
   pshufd xmm2, xmm2, 0x1b
   movdqa xmm4, xmm1
   palignr xmm4, xmm2, 8
   movdqa xmm5, xmm2
   psrldq xmm1, 8
   punpcklqdq xmm5, xmm1
   movdqu XMMWORD PTR [rsp + 256], xmm4
   movdqu XMMWORD PTR [rsp + 272], xmm5

   /* Rounds 0..3. */
   movdqu xmm1, XMMWORD PTR [rsi + 0]
   movdqu xmm3, XMMWORD PTR [rsp + 288]
   pshufb xmm1, xmm3
   movdqu XMMWORD PTR [rsp + 0], xmm1
   movdqa xmm0, xmm1
   movdqu xmm3, XMMWORD PTR [rdx + 0]
   paddd xmm0, xmm3
   sha256rnds2 xmm5, xmm4
   pshufd xmm0, xmm0, 0x0e
   sha256rnds2 xmm4, xmm5

   /* Rounds 4..7. */
   movdqu xmm1, XMMWORD PTR [rsi + 16]
   movdqu xmm3, XMMWORD PTR [rsp + 288]
   pshufb xmm1, xmm3
   movdqu XMMWORD PTR [rsp + 16], xmm1
   movdqa xmm0, xmm1
   movdqu xmm3, XMMWORD PTR [rdx + 16]
   paddd xmm0, xmm3
   sha256rnds2 xmm5, xmm4
   pshufd xmm0, xmm0, 0x0e
   sha256rnds2 xmm4, xmm5

   /* Rounds 8..11. */
   movdqu xmm1, XMMWORD PTR [rsi + 32]
   movdqu xmm3, XMMWORD PTR [rsp + 288]
   pshufb xmm1, xmm3
   movdqu XMMWORD PTR [rsp + 32], xmm1
   movdqa xmm0, xmm1
   movdqu xmm3, XMMWORD PTR [rdx + 32]
   paddd xmm0, xmm3
   sha256rnds2 xmm5, xmm4
   pshufd xmm0, xmm0, 0x0e
   sha256rnds2 xmm4, xmm5

   /* Rounds 12..15. */
   movdqu xmm1, XMMWORD PTR [rsi + 48]
   movdqu xmm3, XMMWORD PTR [rsp + 288]
   pshufb xmm1, xmm3
   movdqu XMMWORD PTR [rsp + 48], xmm1
   movdqa xmm0, xmm1
   movdqu xmm3, XMMWORD PTR [rdx + 48]
   paddd xmm0, xmm3
   sha256rnds2 xmm5, xmm4
   pshufd xmm0, xmm0, 0x0e
   sha256rnds2 xmm4, xmm5

   /* Rounds 16..19. */
   movdqu xmm1, XMMWORD PTR [rsp + 0]
   sha256msg1 xmm1, XMMWORD PTR [rsp + 16]
   movdqu xmm2, XMMWORD PTR [rsp + 48]
   palignr xmm2, XMMWORD PTR [rsp + 32], 4
   paddd xmm1, xmm2
   sha256msg2 xmm1, XMMWORD PTR [rsp + 48]
   movdqu XMMWORD PTR [rsp + 64], xmm1
   movdqa xmm0, xmm1
   movdqu xmm3, XMMWORD PTR [rdx + 64]
   paddd xmm0, xmm3
   sha256rnds2 xmm5, xmm4
   pshufd xmm0, xmm0, 0x0e
   sha256rnds2 xmm4, xmm5

   /* Rounds 20..23. */
   movdqu xmm1, XMMWORD PTR [rsp + 16]
   sha256msg1 xmm1, XMMWORD PTR [rsp + 32]
   movdqu xmm2, XMMWORD PTR [rsp + 64]
   palignr xmm2, XMMWORD PTR [rsp + 48], 4
   paddd xmm1, xmm2
   sha256msg2 xmm1, XMMWORD PTR [rsp + 64]
   movdqu XMMWORD PTR [rsp + 80], xmm1
   movdqa xmm0, xmm1
   movdqu xmm3, XMMWORD PTR [rdx + 80]
   paddd xmm0, xmm3
   sha256rnds2 xmm5, xmm4
   pshufd xmm0, xmm0, 0x0e
   sha256rnds2 xmm4, xmm5

   /* Rounds 24..27. */
   movdqu xmm1, XMMWORD PTR [rsp + 32]
   sha256msg1 xmm1, XMMWORD PTR [rsp + 48]
   movdqu xmm2, XMMWORD PTR [rsp + 80]
   palignr xmm2, XMMWORD PTR [rsp + 64], 4
   paddd xmm1, xmm2
   sha256msg2 xmm1, XMMWORD PTR [rsp + 80]
   movdqu XMMWORD PTR [rsp + 96], xmm1
   movdqa xmm0, xmm1
   movdqu xmm3, XMMWORD PTR [rdx + 96]
   paddd xmm0, xmm3
   sha256rnds2 xmm5, xmm4
   pshufd xmm0, xmm0, 0x0e
   sha256rnds2 xmm4, xmm5

   /* Rounds 28..31. */
   movdqu xmm1, XMMWORD PTR [rsp + 48]
   sha256msg1 xmm1, XMMWORD PTR [rsp + 64]
   movdqu xmm2, XMMWORD PTR [rsp + 96]
   palignr xmm2, XMMWORD PTR [rsp + 80], 4
   paddd xmm1, xmm2
   sha256msg2 xmm1, XMMWORD PTR [rsp + 96]
   movdqu XMMWORD PTR [rsp + 112], xmm1
   movdqa xmm0, xmm1
   movdqu xmm3, XMMWORD PTR [rdx + 112]
   paddd xmm0, xmm3
   sha256rnds2 xmm5, xmm4
   pshufd xmm0, xmm0, 0x0e
   sha256rnds2 xmm4, xmm5

   /* Rounds 32..35. */
   movdqu xmm1, XMMWORD PTR [rsp + 64]
   sha256msg1 xmm1, XMMWORD PTR [rsp + 80]
   movdqu xmm2, XMMWORD PTR [rsp + 112]
   palignr xmm2, XMMWORD PTR [rsp + 96], 4
   paddd xmm1, xmm2
   sha256msg2 xmm1, XMMWORD PTR [rsp + 112]
   movdqu XMMWORD PTR [rsp + 128], xmm1
   movdqa xmm0, xmm1
   movdqu xmm3, XMMWORD PTR [rdx + 128]
   paddd xmm0, xmm3
   sha256rnds2 xmm5, xmm4
   pshufd xmm0, xmm0, 0x0e
   sha256rnds2 xmm4, xmm5

   /* Rounds 36..39. */
   movdqu xmm1, XMMWORD PTR [rsp + 80]
   sha256msg1 xmm1, XMMWORD PTR [rsp + 96]
   movdqu xmm2, XMMWORD PTR [rsp + 128]
   palignr xmm2, XMMWORD PTR [rsp + 112], 4
   paddd xmm1, xmm2
   sha256msg2 xmm1, XMMWORD PTR [rsp + 128]
   movdqu XMMWORD PTR [rsp + 144], xmm1
   movdqa xmm0, xmm1
   movdqu xmm3, XMMWORD PTR [rdx + 144]
   paddd xmm0, xmm3
   sha256rnds2 xmm5, xmm4
   pshufd xmm0, xmm0, 0x0e
   sha256rnds2 xmm4, xmm5

   /* Rounds 40..43. */
   movdqu xmm1, XMMWORD PTR [rsp + 96]
   sha256msg1 xmm1, XMMWORD PTR [rsp + 112]
   movdqu xmm2, XMMWORD PTR [rsp + 144]
   palignr xmm2, XMMWORD PTR [rsp + 128], 4
   paddd xmm1, xmm2
   sha256msg2 xmm1, XMMWORD PTR [rsp + 144]
   movdqu XMMWORD PTR [rsp + 160], xmm1
   movdqa xmm0, xmm1
   movdqu xmm3, XMMWORD PTR [rdx + 160]
   paddd xmm0, xmm3
   sha256rnds2 xmm5, xmm4
   pshufd xmm0, xmm0, 0x0e
   sha256rnds2 xmm4, xmm5

   /* Rounds 44..47. */
   movdqu xmm1, XMMWORD PTR [rsp + 112]
   sha256msg1 xmm1, XMMWORD PTR [rsp + 128]
   movdqu xmm2, XMMWORD PTR [rsp + 160]
   palignr xmm2, XMMWORD PTR [rsp + 144], 4
   paddd xmm1, xmm2
   sha256msg2 xmm1, XMMWORD PTR [rsp + 160]
   movdqu XMMWORD PTR [rsp + 176], xmm1
   movdqa xmm0, xmm1
   movdqu xmm3, XMMWORD PTR [rdx + 176]
   paddd xmm0, xmm3
   sha256rnds2 xmm5, xmm4
   pshufd xmm0, xmm0, 0x0e
   sha256rnds2 xmm4, xmm5

   /* Rounds 48..51. */
   movdqu xmm1, XMMWORD PTR [rsp + 128]
   sha256msg1 xmm1, XMMWORD PTR [rsp + 144]
   movdqu xmm2, XMMWORD PTR [rsp + 176]
   palignr xmm2, XMMWORD PTR [rsp + 160], 4
   paddd xmm1, xmm2
   sha256msg2 xmm1, XMMWORD PTR [rsp + 176]
   movdqu XMMWORD PTR [rsp + 192], xmm1
   movdqa xmm0, xmm1
   movdqu xmm3, XMMWORD PTR [rdx + 192]
   paddd xmm0, xmm3
   sha256rnds2 xmm5, xmm4
   pshufd xmm0, xmm0, 0x0e
   sha256rnds2 xmm4, xmm5

   /* Rounds 52..55. */
   movdqu xmm1, XMMWORD PTR [rsp + 144]
   sha256msg1 xmm1, XMMWORD PTR [rsp + 160]
   movdqu xmm2, XMMWORD PTR [rsp + 192]
   palignr xmm2, XMMWORD PTR [rsp + 176], 4
   paddd xmm1, xmm2
   sha256msg2 xmm1, XMMWORD PTR [rsp + 192]
   movdqu XMMWORD PTR [rsp + 208], xmm1
   movdqa xmm0, xmm1
   movdqu xmm3, XMMWORD PTR [rdx + 208]
   paddd xmm0, xmm3
   sha256rnds2 xmm5, xmm4
   pshufd xmm0, xmm0, 0x0e
   sha256rnds2 xmm4, xmm5

   /* Rounds 56..59. */
   movdqu xmm1, XMMWORD PTR [rsp + 160]
   sha256msg1 xmm1, XMMWORD PTR [rsp + 176]
   movdqu xmm2, XMMWORD PTR [rsp + 208]
   palignr xmm2, XMMWORD PTR [rsp + 192], 4
   paddd xmm1, xmm2
   sha256msg2 xmm1, XMMWORD PTR [rsp + 208]
   movdqu XMMWORD PTR [rsp + 224], xmm1
   movdqa xmm0, xmm1
   movdqu xmm3, XMMWORD PTR [rdx + 224]
   paddd xmm0, xmm3
   sha256rnds2 xmm5, xmm4
   pshufd xmm0, xmm0, 0x0e
   sha256rnds2 xmm4, xmm5

   /* Rounds 60..63. */
   movdqu xmm1, XMMWORD PTR [rsp + 176]
   sha256msg1 xmm1, XMMWORD PTR [rsp + 192]
   movdqu xmm2, XMMWORD PTR [rsp + 224]
   palignr xmm2, XMMWORD PTR [rsp + 208], 4
   paddd xmm1, xmm2
   sha256msg2 xmm1, XMMWORD PTR [rsp + 224]
   movdqu XMMWORD PTR [rsp + 240], xmm1
   movdqa xmm0, xmm1
   movdqu xmm3, XMMWORD PTR [rdx + 240]
   paddd xmm0, xmm3
   sha256rnds2 xmm5, xmm4
   pshufd xmm0, xmm0, 0x0e
   sha256rnds2 xmm4, xmm5

   /* Feed forward, then restore the native A,B,C,D,E,F,G,H layout. */
   paddd xmm4, XMMWORD PTR [rsp + 256]
   paddd xmm5, XMMWORD PTR [rsp + 272]
   pshufd xmm1, xmm4, 0x1b
   pshufd xmm2, xmm5, 0xb1
   psrldq xmm2, 8
   punpcklqdq xmm1, xmm2
   pshufd xmm4, xmm4, 0xb1
   pshufd xmm5, xmm5, 0x1b
   psrldq xmm5, 8
   punpcklqdq xmm4, xmm5
   movdqu XMMWORD PTR [rdi + 8], xmm1
   movdqu XMMWORD PTR [rdi + 24], xmm4

   pxor xmm0, xmm0
   movdqu XMMWORD PTR [rsp + 0], xmm0
   movdqu XMMWORD PTR [rsp + 16], xmm0
   movdqu XMMWORD PTR [rsp + 32], xmm0
   movdqu XMMWORD PTR [rsp + 48], xmm0
   movdqu XMMWORD PTR [rsp + 64], xmm0
   movdqu XMMWORD PTR [rsp + 80], xmm0
   movdqu XMMWORD PTR [rsp + 96], xmm0
   movdqu XMMWORD PTR [rsp + 112], xmm0
   movdqu XMMWORD PTR [rsp + 128], xmm0
   movdqu XMMWORD PTR [rsp + 144], xmm0
   movdqu XMMWORD PTR [rsp + 160], xmm0
   movdqu XMMWORD PTR [rsp + 176], xmm0
   movdqu XMMWORD PTR [rsp + 192], xmm0
   movdqu XMMWORD PTR [rsp + 208], xmm0
   movdqu XMMWORD PTR [rsp + 224], xmm0
   movdqu XMMWORD PTR [rsp + 240], xmm0
   movdqu XMMWORD PTR [rsp + 256], xmm0
   movdqu XMMWORD PTR [rsp + 272], xmm0
   movdqu XMMWORD PTR [rsp + 288], xmm0
   pxor xmm1, xmm1
   pxor xmm2, xmm2
   pxor xmm3, xmm3
   pxor xmm4, xmm4
   pxor xmm5, xmm5
   add rsp, 312
   .cfi_def_cfa_offset 8
   ret
   .cfi_endproc
.size SharkSslSha256Ctx_processSHA, .-SharkSslSha256Ctx_processSHA

/**
 * Public CPU dispatch: SHA (CPUID.7.0:EBX[29]), SSSE3 (1:ECX[9]) and
 * SSE2 (1:EDX[26]). No AVX or OSXSAVE state is used by this kernel.
 */
.p2align 4
.globl sharkssl_sha256_hw_probe
.type sharkssl_sha256_hw_probe, @function
sharkssl_sha256_hw_probe:
   .cfi_startproc
   push rbx
   .cfi_def_cfa_offset 16
   .cfi_offset rbx, -16
   xor eax, eax
   cpuid
   cmp eax, 7
   jb .Lsha256_no
   mov eax, 1
   cpuid
   test edx, 0x04000000
   jz .Lsha256_no
   test ecx, 0x00000200
   jz .Lsha256_no
   mov eax, 7
   xor ecx, ecx
   cpuid
   shr ebx, 29
   and ebx, 1
   mov eax, ebx
   jmp .Lsha256_probe_return
.Lsha256_no:
   xor eax, eax
.Lsha256_probe_return:
   pop rbx
   .cfi_restore rbx
   .cfi_def_cfa_offset 8
   ret
   .cfi_endproc
.size sharkssl_sha256_hw_probe, .-sharkssl_sha256_hw_probe
.section .note.GNU-stack,"",@progbits
