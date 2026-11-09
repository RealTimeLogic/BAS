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
 *   $Id: SharkSslAes_X64.S 6818 2026-11-08 21:18:02Z gianluca $
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

/* SharkSSL AES-NI block cipher for x86-64 ELF/GAS (System V ABI), the
 * SHARKSSL_OPTIMIZED_AES_ASM backend of SharkSslAesCtx: ECB, CBC, CTR, CCM.
 * Port of X64_MSVC/SharkSslAes_X64.asm; only the argument registers differ.
 *
 * rk holds the nr + 1 FIPS 197 round keys as bytes (a decryption context:
 * the keys of the equivalent inverse cipher), written by SharkSslCrypto.c.
 * AES-NI and the loads and stores are constant time: no key or data value
 * selects an address or a branch. The round count is public. The XMM
 * registers that held keys or data are cleared on return. */
.intel_syntax noprefix
.text

/* r10 = rk + 16 * nr, r11 = 16 - 16 * nr. Clobbers rax. */
.macro AES_SETUP rk, nr32
    mov eax, \nr32
    shl rax, 4
    lea r10, [\rk+rax]
    neg rax
    add rax, 16
    mov r11, rax
.endm

/* xmm0-xmm3 = four blocks, xmm4 = round key. Clobbers rax. */
.macro AES_ROUNDS4 op, last
    movdqu xmm4, XMMWORD PTR [r10+r11-16]
    pxor xmm0, xmm4
    pxor xmm1, xmm4
    pxor xmm2, xmm4
    pxor xmm3, xmm4
    mov rax, r11
    test rax, rax
    jz 2f
1:
    movdqu xmm4, XMMWORD PTR [r10+rax]
    \op xmm0, xmm4
    \op xmm1, xmm4
    \op xmm2, xmm4
    \op xmm3, xmm4
    add rax, 16
    jnz 1b
2:
    movdqu xmm4, XMMWORD PTR [r10]
    \last xmm0, xmm4
    \last xmm1, xmm4
    \last xmm2, xmm4
    \last xmm3, xmm4
.endm

/* xmm0 = one block, xmm4 = round key. Clobbers rax. */
.macro AES_ROUNDS1 op, last
    movdqu xmm4, XMMWORD PTR [r10+r11-16]
    pxor xmm0, xmm4
    mov rax, r11
    test rax, rax
    jz 2f
1:
    movdqu xmm4, XMMWORD PTR [r10+rax]
    \op xmm0, xmm4
    add rax, 16
    jnz 1b
2:
    movdqu xmm4, XMMWORD PTR [r10]
    \last xmm0, xmm4
.endm

/* xmm0-xmm1 = two blocks, xmm4 = round key. Clobbers rax. */
.macro AES_ROUNDS2
    movdqu xmm4, XMMWORD PTR [r10+r11-16]
    pxor xmm0, xmm4
    pxor xmm1, xmm4
    mov rax, r11
    test rax, rax
    jz 2f
1:
    movdqu xmm4, XMMWORD PTR [r10+rax]
    aesenc xmm0, xmm4
    aesenc xmm1, xmm4
    add rax, 16
    jnz 1b
2:
    movdqu xmm4, XMMWORD PTR [r10]
    aesenclast xmm0, xmm4
    aesenclast xmm1, xmm4
.endm

.macro AES_CLEAR
    pxor xmm0, xmm0
    pxor xmm1, xmm1
    pxor xmm2, xmm2
    pxor xmm3, xmm3
    pxor xmm4, xmm4
    pxor xmm5, xmm5
.endm

/* rdx = in, r8 = out, r9 = blocks */
.macro AES_ECB op, last
3:
    cmp r9, 4
    jb 4f
    movdqu xmm0, XMMWORD PTR [rdx]
    movdqu xmm1, XMMWORD PTR [rdx+16]
    movdqu xmm2, XMMWORD PTR [rdx+32]
    movdqu xmm3, XMMWORD PTR [rdx+48]
    AES_ROUNDS4 \op, \last
    movdqu XMMWORD PTR [r8], xmm0
    movdqu XMMWORD PTR [r8+16], xmm1
    movdqu XMMWORD PTR [r8+32], xmm2
    movdqu XMMWORD PTR [r8+48], xmm3
    add rdx, 64
    add r8, 64
    sub r9, 4
    jmp 3b
4:
    test r9, r9
    jz 5f
    movdqu xmm0, XMMWORD PTR [rdx]
    AES_ROUNDS1 \op, \last
    movdqu XMMWORD PTR [r8], xmm0
    add rdx, 16
    add r8, 16
    dec r9
    jmp 4b
5:
    AES_CLEAR
.endm

/* rcx = ctr, little-endian 128-bit; rsi:rdi = its low and high halves. */
.macro AES_CTR_BLOCK x
    add rsi, 1
    adc rdi, 0
    movq \x, rsi
    movq xmm5, rdi
    punpcklqdq \x, xmm5
.endm

/* int sharkssl_aes_hw_probe(void): 1 when CPUID reports AES-NI */
.globl sharkssl_aes_hw_probe
.type sharkssl_aes_hw_probe, @function
sharkssl_aes_hw_probe:
.cfi_startproc
    push rbx
.cfi_adjust_cfa_offset 8
.cfi_offset rbx, -16
    xor eax, eax
    cpuid
    xor edx, edx
    cmp eax, 1
    jb 1f
    mov eax, 1
    xor ecx, ecx
    cpuid
    shr ecx, 25
    and ecx, 1
    mov edx, ecx
1:
    mov eax, edx
    pop rbx
.cfi_adjust_cfa_offset -8
.cfi_restore rbx
    ret
.cfi_endproc
.size sharkssl_aes_hw_probe, .-sharkssl_aes_hw_probe

/* void sharkssl_aes_hw_ecb_encrypt(const U32 *rk, U32 nr, const U8 *in, U8 *out, U32 blocks) */
.globl sharkssl_aes_hw_ecb_encrypt
.type sharkssl_aes_hw_ecb_encrypt, @function
.p2align 4
sharkssl_aes_hw_ecb_encrypt:
.cfi_startproc
    AES_SETUP rdi, esi
    mov r9d, r8d
    mov r8, rcx
    AES_ECB aesenc, aesenclast
    ret
.cfi_endproc
.size sharkssl_aes_hw_ecb_encrypt, .-sharkssl_aes_hw_ecb_encrypt

/* void sharkssl_aes_hw_ecb_decrypt(const U32 *rk, U32 nr, const U8 *in, U8 *out, U32 blocks) */
.globl sharkssl_aes_hw_ecb_decrypt
.type sharkssl_aes_hw_ecb_decrypt, @function
.p2align 4
sharkssl_aes_hw_ecb_decrypt:
.cfi_startproc
    AES_SETUP rdi, esi
    mov r9d, r8d
    mov r8, rcx
    AES_ECB aesdec, aesdeclast
    ret
.cfi_endproc
.size sharkssl_aes_hw_ecb_decrypt, .-sharkssl_aes_hw_ecb_decrypt

/* void sharkssl_aes_hw_cbc_encrypt(const U32 *rk, U32 nr, U8 vect[16], const U8 *in, U8 *out, U32 blocks) */
.globl sharkssl_aes_hw_cbc_encrypt
.type sharkssl_aes_hw_cbc_encrypt, @function
.p2align 4
sharkssl_aes_hw_cbc_encrypt:
.cfi_startproc
    AES_SETUP rdi, esi
    xchg rcx, rdx
    mov r9d, r9d
    movdqu xmm0, XMMWORD PTR [rcx]
3:
    test r9, r9
    jz 4f
    movdqu xmm4, XMMWORD PTR [rdx]
    pxor xmm0, xmm4
    AES_ROUNDS1 aesenc, aesenclast
    movdqu XMMWORD PTR [r8], xmm0
    add rdx, 16
    add r8, 16
    dec r9
    jmp 3b
4:
    movdqu XMMWORD PTR [rcx], xmm0
    AES_CLEAR
    ret
.cfi_endproc
.size sharkssl_aes_hw_cbc_encrypt, .-sharkssl_aes_hw_cbc_encrypt

/* void sharkssl_aes_hw_cbc_decrypt(const U32 *rk, U32 nr, U8 vect[16], const U8 *in, U8 *out, U32 blocks)
 * Every group reads its ciphertext before it writes: in == out works. */
.globl sharkssl_aes_hw_cbc_decrypt
.type sharkssl_aes_hw_cbc_decrypt, @function
.p2align 4
sharkssl_aes_hw_cbc_decrypt:
.cfi_startproc
    AES_SETUP rdi, esi
    xchg rcx, rdx
    mov r9d, r9d
    movdqu xmm5, XMMWORD PTR [rcx]
3:
    cmp r9, 4
    jb 4f
    movdqu xmm0, XMMWORD PTR [rdx]
    movdqu xmm1, XMMWORD PTR [rdx+16]
    movdqu xmm2, XMMWORD PTR [rdx+32]
    movdqu xmm3, XMMWORD PTR [rdx+48]
    AES_ROUNDS4 aesdec, aesdeclast
    pxor xmm0, xmm5
    movdqu xmm4, XMMWORD PTR [rdx]
    pxor xmm1, xmm4
    movdqu xmm4, XMMWORD PTR [rdx+16]
    pxor xmm2, xmm4
    movdqu xmm4, XMMWORD PTR [rdx+32]
    pxor xmm3, xmm4
    movdqu xmm5, XMMWORD PTR [rdx+48]
    movdqu XMMWORD PTR [r8], xmm0
    movdqu XMMWORD PTR [r8+16], xmm1
    movdqu XMMWORD PTR [r8+32], xmm2
    movdqu XMMWORD PTR [r8+48], xmm3
    add rdx, 64
    add r8, 64
    sub r9, 4
    jmp 3b
4:
    test r9, r9
    jz 5f
    movdqu xmm0, XMMWORD PTR [rdx]
    AES_ROUNDS1 aesdec, aesdeclast
    pxor xmm0, xmm5
    movdqu xmm5, XMMWORD PTR [rdx]
    movdqu XMMWORD PTR [r8], xmm0
    add rdx, 16
    add r8, 16
    dec r9
    jmp 4b
5:
    movdqu XMMWORD PTR [rcx], xmm5
    AES_CLEAR
    ret
.cfi_endproc
.size sharkssl_aes_hw_cbc_decrypt, .-sharkssl_aes_hw_cbc_decrypt

/* void sharkssl_aes_hw_ctr(const U32 *rk, U32 nr, U8 ctr[16], const U8 *in, U8 *out, U32 blocks)
 * Each block increments the little-endian counter first, then encrypts it. */
.globl sharkssl_aes_hw_ctr
.type sharkssl_aes_hw_ctr, @function
.p2align 4
sharkssl_aes_hw_ctr:
.cfi_startproc
    AES_SETUP rdi, esi
    xchg rcx, rdx
    mov r9d, r9d
    mov rsi, QWORD PTR [rcx]
    mov rdi, QWORD PTR [rcx+8]
3:
    cmp r9, 4
    jb 4f
    AES_CTR_BLOCK xmm0
    AES_CTR_BLOCK xmm1
    AES_CTR_BLOCK xmm2
    AES_CTR_BLOCK xmm3
    AES_ROUNDS4 aesenc, aesenclast
    movdqu xmm4, XMMWORD PTR [rdx]
    pxor xmm0, xmm4
    movdqu xmm4, XMMWORD PTR [rdx+16]
    pxor xmm1, xmm4
    movdqu xmm4, XMMWORD PTR [rdx+32]
    pxor xmm2, xmm4
    movdqu xmm4, XMMWORD PTR [rdx+48]
    pxor xmm3, xmm4
    movdqu XMMWORD PTR [r8], xmm0
    movdqu XMMWORD PTR [r8+16], xmm1
    movdqu XMMWORD PTR [r8+32], xmm2
    movdqu XMMWORD PTR [r8+48], xmm3
    add rdx, 64
    add r8, 64
    sub r9, 4
    jmp 3b
4:
    test r9, r9
    jz 5f
    AES_CTR_BLOCK xmm0
    AES_ROUNDS1 aesenc, aesenclast
    movdqu xmm4, XMMWORD PTR [rdx]
    pxor xmm0, xmm4
    movdqu XMMWORD PTR [r8], xmm0
    add rdx, 16
    add r8, 16
    dec r9
    jmp 4b
5:
    mov QWORD PTR [rcx], rsi
    mov QWORD PTR [rcx+8], rdi
    AES_CLEAR
    ret
.cfi_endproc
.size sharkssl_aes_hw_ctr, .-sharkssl_aes_hw_ctr

/* CCM over complete blocks. rcx = state: the pending CBC-MAC block, then the
 * counter block, whose last four bytes are a big-endian counter; rdx = in,
 * r8 = out, rsi = blocks. Per block: increment the counter, encrypt the MAC
 * block and the counter together, then out = in ^ E(counter) and MAC ^= the
 * plaintext. xmm2 = counter block with its counter bytes zeroed, r9d = counter. */
.macro AES_CCM dec
    movdqu xmm0, XMMWORD PTR [rcx]
    movdqu xmm2, XMMWORD PTR [rcx+16]
    pslldq xmm2, 4
    psrldq xmm2, 4
    mov r9d, DWORD PTR [rcx+28]
    bswap r9d
3:
    test rsi, rsi
    jz 4f
    add r9d, 1
    mov eax, r9d
    bswap eax
    movd xmm1, eax
    pslldq xmm1, 12
    por xmm1, xmm2
    AES_ROUNDS2
    movdqu xmm3, XMMWORD PTR [rdx]
    pxor xmm1, xmm3
.if \dec
    pxor xmm0, xmm1
.else
    pxor xmm0, xmm3
.endif
    movdqu XMMWORD PTR [r8], xmm1
    add rdx, 16
    add r8, 16
    dec rsi
    jmp 3b
4:
    movdqu XMMWORD PTR [rcx], xmm0
    bswap r9d
    mov DWORD PTR [rcx+28], r9d
    AES_CLEAR
.endm

/* void sharkssl_aes_hw_ccm_encrypt(const U32 *rk, U32 nr, U8 state[32], const U8 *in, U8 *out, U32 blocks) */
.globl sharkssl_aes_hw_ccm_encrypt
.type sharkssl_aes_hw_ccm_encrypt, @function
.p2align 4
sharkssl_aes_hw_ccm_encrypt:
.cfi_startproc
    AES_SETUP rdi, esi
    xchg rcx, rdx
    mov esi, r9d
    AES_CCM 0
    ret
.cfi_endproc
.size sharkssl_aes_hw_ccm_encrypt, .-sharkssl_aes_hw_ccm_encrypt

/* void sharkssl_aes_hw_ccm_decrypt(const U32 *rk, U32 nr, U8 state[32], const U8 *in, U8 *out, U32 blocks)
 * Reads each block before it writes it: in == out works. */
.globl sharkssl_aes_hw_ccm_decrypt
.type sharkssl_aes_hw_ccm_decrypt, @function
.p2align 4
sharkssl_aes_hw_ccm_decrypt:
.cfi_startproc
    AES_SETUP rdi, esi
    xchg rcx, rdx
    mov esi, r9d
    AES_CCM 1
    ret
.cfi_endproc
.size sharkssl_aes_hw_ccm_decrypt, .-sharkssl_aes_hw_ccm_decrypt

.section .note.GNU-stack,"",@progbits
