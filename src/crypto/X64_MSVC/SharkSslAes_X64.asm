;****************************************************************************
;*     ____             _________                __                _
;*    / __ \___  ____ _/ /_  __(_)___ ___  ___  / /   ____  ____ _(_)____
;*   / /_/ / _ \/ __ `/ / / / / / __ `__ \/ _ \/ /   / __ \/ __ `/ / ___/
;*  / _, _/  __/ /_/ / / / / / / / / / / /  __/ /___/ /_/ / /_/ / / /__
;* /_/ |_|\___/\__,_/_/ /_/ /_/_/ /_/ /_/\___/_____/\____/\__, /_/\___/
;*                                                       /____/
;*
;*                 SharkSSL Embedded SSL/TLS Stack
;****************************************************************************
;*   PROGRAM MODULE
;*
;*   $Id: SharkSslAes_X64.asm 6818 2026-11-08 21:18:02Z gianluca $
;*
;*   COPYRIGHT:  Real Time Logic LLC, 2026
;*
;*   This software is copyrighted by and is the sole property of Real
;*   Time Logic LLC.  All rights, title, ownership, or other interests in
;*   the software remain the property of Real Time Logic LLC.  This
;*   software may only be used in accordance with the terms and
;*   conditions stipulated in the corresponding license agreement under
;*   which the software has been supplied.  Any unauthorized use,
;*   duplication, transmission, distribution, or disclosure of this
;*   software is expressly forbidden.
;*
;*   This Copyright notice may not be removed or modified without prior
;*   written consent of Real Time Logic LLC.
;*
;*   Real Time Logic LLC. reserves the right to modify this software
;*   without notice.
;*
;*               http://www.realtimelogic.com
;*               http://www.sharkssl.com
;****************************************************************************

; SharkSSL AES-NI block cipher for Microsoft x64 MASM, the
; SHARKSSL_OPTIMIZED_AES_ASM backend of SharkSslAesCtx: ECB, CBC, CTR, CCM.
; X64_GCC/SharkSslAes_X64.S is the same code with System V arguments.
;
; rk holds the nr + 1 FIPS 197 round keys as bytes (a decryption context:
; the keys of the equivalent inverse cipher), written by SharkSslCrypto.c.
; AES-NI and the loads and stores are constant time: no key or data value
; selects an address or a branch. The round count is public. Only the
; volatile xmm0-xmm5 are used; they are cleared on return.

OPTION CASEMAP:NONE

.code

; r10 = rk + 16 * nr, r11 = 16 - 16 * nr. Clobbers rax.
AES_SETUP MACRO rk, nr32
    mov eax, nr32
    shl rax, 4
    lea r10, [rk+rax]
    neg rax
    add rax, 16
    mov r11, rax
ENDM

; xmm0-xmm3 = four blocks, xmm4 = round key. Clobbers rax.
AES_ROUNDS4 MACRO op, last
    LOCAL round_loop, round_last
    movdqu xmm4, XMMWORD PTR [r10+r11-16]
    pxor xmm0, xmm4
    pxor xmm1, xmm4
    pxor xmm2, xmm4
    pxor xmm3, xmm4
    mov rax, r11
    test rax, rax
    jz round_last
round_loop:
    movdqu xmm4, XMMWORD PTR [r10+rax]
    op xmm0, xmm4
    op xmm1, xmm4
    op xmm2, xmm4
    op xmm3, xmm4
    add rax, 16
    jnz round_loop
round_last:
    movdqu xmm4, XMMWORD PTR [r10]
    last xmm0, xmm4
    last xmm1, xmm4
    last xmm2, xmm4
    last xmm3, xmm4
ENDM

; xmm0 = one block, xmm4 = round key. Clobbers rax.
AES_ROUNDS1 MACRO op, last
    LOCAL round_loop, round_last
    movdqu xmm4, XMMWORD PTR [r10+r11-16]
    pxor xmm0, xmm4
    mov rax, r11
    test rax, rax
    jz round_last
round_loop:
    movdqu xmm4, XMMWORD PTR [r10+rax]
    op xmm0, xmm4
    add rax, 16
    jnz round_loop
round_last:
    movdqu xmm4, XMMWORD PTR [r10]
    last xmm0, xmm4
ENDM

; xmm0-xmm1 = two blocks, xmm4 = round key. Clobbers rax.
AES_ROUNDS2 MACRO
    LOCAL round_loop, round_last
    movdqu xmm4, XMMWORD PTR [r10+r11-16]
    pxor xmm0, xmm4
    pxor xmm1, xmm4
    mov rax, r11
    test rax, rax
    jz round_last
round_loop:
    movdqu xmm4, XMMWORD PTR [r10+rax]
    aesenc xmm0, xmm4
    aesenc xmm1, xmm4
    add rax, 16
    jnz round_loop
round_last:
    movdqu xmm4, XMMWORD PTR [r10]
    aesenclast xmm0, xmm4
    aesenclast xmm1, xmm4
ENDM

AES_CLEAR MACRO
    pxor xmm0, xmm0
    pxor xmm1, xmm1
    pxor xmm2, xmm2
    pxor xmm3, xmm3
    pxor xmm4, xmm4
    pxor xmm5, xmm5
ENDM

; rdx = in, r8 = out, r9 = blocks
AES_ECB MACRO op, last
    LOCAL four, one, done
four:
    cmp r9, 4
    jb one
    movdqu xmm0, XMMWORD PTR [rdx]
    movdqu xmm1, XMMWORD PTR [rdx+16]
    movdqu xmm2, XMMWORD PTR [rdx+32]
    movdqu xmm3, XMMWORD PTR [rdx+48]
    AES_ROUNDS4 op, last
    movdqu XMMWORD PTR [r8], xmm0
    movdqu XMMWORD PTR [r8+16], xmm1
    movdqu XMMWORD PTR [r8+32], xmm2
    movdqu XMMWORD PTR [r8+48], xmm3
    add rdx, 64
    add r8, 64
    sub r9, 4
    jmp four
one:
    test r9, r9
    jz done
    movdqu xmm0, XMMWORD PTR [rdx]
    AES_ROUNDS1 op, last
    movdqu XMMWORD PTR [r8], xmm0
    add rdx, 16
    add r8, 16
    dec r9
    jmp one
done:
    AES_CLEAR
ENDM

; rcx = ctr, little-endian 128-bit; rsi:rdi = its low and high halves.
AES_CTR_BLOCK MACRO x
    add rsi, 1
    adc rdi, 0
    movq x, rsi
    movq xmm5, rdi
    punpcklqdq x, xmm5
ENDM

; int sharkssl_aes_hw_probe(void): 1 when CPUID reports AES-NI
PUBLIC sharkssl_aes_hw_probe
sharkssl_aes_hw_probe PROC FRAME
    push rbx
    .pushreg rbx
    .endprolog
    xor eax, eax
    cpuid
    xor edx, edx
    cmp eax, 1
    jb probe_done
    mov eax, 1
    xor ecx, ecx
    cpuid
    shr ecx, 25
    and ecx, 1
    mov edx, ecx
probe_done:
    mov eax, edx
    pop rbx
    ret
sharkssl_aes_hw_probe ENDP

; void sharkssl_aes_hw_ecb_encrypt(const U32 *rk, U32 nr, const U8 *in, U8 *out, U32 blocks)
PUBLIC sharkssl_aes_hw_ecb_encrypt
ALIGN 16
sharkssl_aes_hw_ecb_encrypt PROC
    AES_SETUP rcx, edx
    mov rdx, r8
    mov r8, r9
    mov r9d, DWORD PTR [rsp+40]
    AES_ECB aesenc, aesenclast
    ret
sharkssl_aes_hw_ecb_encrypt ENDP

; void sharkssl_aes_hw_ecb_decrypt(const U32 *rk, U32 nr, const U8 *in, U8 *out, U32 blocks)
PUBLIC sharkssl_aes_hw_ecb_decrypt
ALIGN 16
sharkssl_aes_hw_ecb_decrypt PROC
    AES_SETUP rcx, edx
    mov rdx, r8
    mov r8, r9
    mov r9d, DWORD PTR [rsp+40]
    AES_ECB aesdec, aesdeclast
    ret
sharkssl_aes_hw_ecb_decrypt ENDP

; void sharkssl_aes_hw_cbc_encrypt(const U32 *rk, U32 nr, U8 vect[16], const U8 *in, U8 *out, U32 blocks)
PUBLIC sharkssl_aes_hw_cbc_encrypt
ALIGN 16
sharkssl_aes_hw_cbc_encrypt PROC
    AES_SETUP rcx, edx
    mov rcx, r8
    mov rdx, r9
    mov r8, QWORD PTR [rsp+40]
    mov r9d, DWORD PTR [rsp+48]
    movdqu xmm0, XMMWORD PTR [rcx]
cbc_enc_loop:
    test r9, r9
    jz cbc_enc_done
    movdqu xmm4, XMMWORD PTR [rdx]
    pxor xmm0, xmm4
    AES_ROUNDS1 aesenc, aesenclast
    movdqu XMMWORD PTR [r8], xmm0
    add rdx, 16
    add r8, 16
    dec r9
    jmp cbc_enc_loop
cbc_enc_done:
    movdqu XMMWORD PTR [rcx], xmm0
    AES_CLEAR
    ret
sharkssl_aes_hw_cbc_encrypt ENDP

; void sharkssl_aes_hw_cbc_decrypt(const U32 *rk, U32 nr, U8 vect[16], const U8 *in, U8 *out, U32 blocks)
; Every group reads its ciphertext before it writes: in == out works.
PUBLIC sharkssl_aes_hw_cbc_decrypt
ALIGN 16
sharkssl_aes_hw_cbc_decrypt PROC
    AES_SETUP rcx, edx
    mov rcx, r8
    mov rdx, r9
    mov r8, QWORD PTR [rsp+40]
    mov r9d, DWORD PTR [rsp+48]
    movdqu xmm5, XMMWORD PTR [rcx]
cbc_dec_four:
    cmp r9, 4
    jb cbc_dec_one
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
    jmp cbc_dec_four
cbc_dec_one:
    test r9, r9
    jz cbc_dec_done
    movdqu xmm0, XMMWORD PTR [rdx]
    AES_ROUNDS1 aesdec, aesdeclast
    pxor xmm0, xmm5
    movdqu xmm5, XMMWORD PTR [rdx]
    movdqu XMMWORD PTR [r8], xmm0
    add rdx, 16
    add r8, 16
    dec r9
    jmp cbc_dec_one
cbc_dec_done:
    movdqu XMMWORD PTR [rcx], xmm5
    AES_CLEAR
    ret
sharkssl_aes_hw_cbc_decrypt ENDP

; void sharkssl_aes_hw_ctr(const U32 *rk, U32 nr, U8 ctr[16], const U8 *in, U8 *out, U32 blocks)
; Each block increments the little-endian counter first, then encrypts it.
PUBLIC sharkssl_aes_hw_ctr
ALIGN 16
sharkssl_aes_hw_ctr PROC FRAME
    push rsi
    .pushreg rsi
    push rdi
    .pushreg rdi
    .endprolog
    AES_SETUP rcx, edx
    mov rcx, r8
    mov rdx, r9
    mov r8, QWORD PTR [rsp+56]
    mov r9d, DWORD PTR [rsp+64]
    mov rsi, QWORD PTR [rcx]
    mov rdi, QWORD PTR [rcx+8]
ctr_four:
    cmp r9, 4
    jb ctr_one
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
    jmp ctr_four
ctr_one:
    test r9, r9
    jz ctr_done
    AES_CTR_BLOCK xmm0
    AES_ROUNDS1 aesenc, aesenclast
    movdqu xmm4, XMMWORD PTR [rdx]
    pxor xmm0, xmm4
    movdqu XMMWORD PTR [r8], xmm0
    add rdx, 16
    add r8, 16
    dec r9
    jmp ctr_one
ctr_done:
    mov QWORD PTR [rcx], rsi
    mov QWORD PTR [rcx+8], rdi
    AES_CLEAR
    pop rdi
    pop rsi
    ret
sharkssl_aes_hw_ctr ENDP

; CCM over complete blocks. rcx = state: the pending CBC-MAC block, then the
; counter block, whose last four bytes are a big-endian counter; rdx = in,
; r8 = out, the block count stays in its stack argument at [rsp+48]. Per
; block: increment the counter, encrypt the MAC block and the counter
; together, then out = in ^ E(counter) and MAC ^= the plaintext. xmm2 = the
; counter block with its counter bytes zeroed, r9d = counter.
AES_CCM MACRO dec
    LOCAL ccm_loop, ccm_done
    movdqu xmm0, XMMWORD PTR [rcx]
    movdqu xmm2, XMMWORD PTR [rcx+16]
    pslldq xmm2, 4
    psrldq xmm2, 4
    mov r9d, DWORD PTR [rcx+28]
    bswap r9d
ccm_loop:
    cmp DWORD PTR [rsp+48], 0
    je ccm_done
    add r9d, 1
    mov eax, r9d
    bswap eax
    movd xmm1, eax
    pslldq xmm1, 12
    por xmm1, xmm2
    AES_ROUNDS2
    movdqu xmm3, XMMWORD PTR [rdx]
    pxor xmm1, xmm3
IF dec
    pxor xmm0, xmm1
ELSE
    pxor xmm0, xmm3
ENDIF
    movdqu XMMWORD PTR [r8], xmm1
    add rdx, 16
    add r8, 16
    sub DWORD PTR [rsp+48], 1
    jmp ccm_loop
ccm_done:
    movdqu XMMWORD PTR [rcx], xmm0
    bswap r9d
    mov DWORD PTR [rcx+28], r9d
    AES_CLEAR
ENDM

; void sharkssl_aes_hw_ccm_encrypt(const U32 *rk, U32 nr, U8 state[32], const U8 *in, U8 *out, U32 blocks)
PUBLIC sharkssl_aes_hw_ccm_encrypt
ALIGN 16
sharkssl_aes_hw_ccm_encrypt PROC
    AES_SETUP rcx, edx
    mov rcx, r8
    mov rdx, r9
    mov r8, QWORD PTR [rsp+40]
    AES_CCM 0
    ret
sharkssl_aes_hw_ccm_encrypt ENDP

; void sharkssl_aes_hw_ccm_decrypt(const U32 *rk, U32 nr, U8 state[32], const U8 *in, U8 *out, U32 blocks)
; Reads each block before it writes it: in == out works.
PUBLIC sharkssl_aes_hw_ccm_decrypt
ALIGN 16
sharkssl_aes_hw_ccm_decrypt PROC
    AES_SETUP rcx, edx
    mov rcx, r8
    mov rdx, r9
    mov r8, QWORD PTR [rsp+40]
    AES_CCM 1
    ret
sharkssl_aes_hw_ccm_decrypt ENDP

END
