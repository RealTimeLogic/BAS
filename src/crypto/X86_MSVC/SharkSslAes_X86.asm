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
;*   $Id: SharkSslAes_X86.asm 6789 2026-11-07 19:24:20Z gianluca $
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

; SharkSSL AES-NI block cipher for Microsoft 32-bit x86 MASM (cdecl), the
; SHARKSSL_OPTIMIZED_AES_ASM backend of SharkSslAesCtx: ECB, CBC, CTR, CCM.
; X86_GCC/SharkSslAes_X86.S is the same code in GNU syntax.
;
; rk holds the nr + 1 FIPS 197 round keys as bytes (a decryption context:
; the keys of the equivalent inverse cipher), written by SharkSslCrypto.c.
; AES-NI and the loads and stores are constant time: no key or data value
; selects an address or a branch. The round count is public. The XMM
; registers that held keys or data are cleared on return.

.686P
.XMM
.model flat, C
OPTION CASEMAP:NONE

.code

; Saves ebp, ebx, esi and edi: the first argument is then at [esp+20].
AES_ENTER MACRO
    push ebp
    push ebx
    push esi
    push edi
ENDM

AES_LEAVE MACRO
    pop edi
    pop esi
    pop ebx
    pop ebp
ENDM

; ebp = rk + 16 * nr, ebx = 16 - 16 * nr. Clobbers eax.
AES_SETUP MACRO
    mov eax, DWORD PTR [esp+24]
    shl eax, 4
    mov ebp, DWORD PTR [esp+20]
    add ebp, eax
    neg eax
    add eax, 16
    mov ebx, eax
ENDM

; xmm0-xmm3 = four blocks, xmm4 = round key. Clobbers eax.
AES_ROUNDS4 MACRO op, last
    LOCAL round_loop, round_last
    movdqu xmm4, XMMWORD PTR [ebp+ebx-16]
    pxor xmm0, xmm4
    pxor xmm1, xmm4
    pxor xmm2, xmm4
    pxor xmm3, xmm4
    mov eax, ebx
    test eax, eax
    jz round_last
round_loop:
    movdqu xmm4, XMMWORD PTR [ebp+eax]
    op xmm0, xmm4
    op xmm1, xmm4
    op xmm2, xmm4
    op xmm3, xmm4
    add eax, 16
    jnz round_loop
round_last:
    movdqu xmm4, XMMWORD PTR [ebp]
    last xmm0, xmm4
    last xmm1, xmm4
    last xmm2, xmm4
    last xmm3, xmm4
ENDM

; xmm0 = one block, xmm4 = round key. Clobbers eax.
AES_ROUNDS1 MACRO op, last
    LOCAL round_loop, round_last
    movdqu xmm4, XMMWORD PTR [ebp+ebx-16]
    pxor xmm0, xmm4
    mov eax, ebx
    test eax, eax
    jz round_last
round_loop:
    movdqu xmm4, XMMWORD PTR [ebp+eax]
    op xmm0, xmm4
    add eax, 16
    jnz round_loop
round_last:
    movdqu xmm4, XMMWORD PTR [ebp]
    last xmm0, xmm4
ENDM

; xmm0-xmm1 = two blocks, xmm4 = round key. Clobbers eax.
AES_ROUNDS2 MACRO
    LOCAL round_loop, round_last
    movdqu xmm4, XMMWORD PTR [ebp+ebx-16]
    pxor xmm0, xmm4
    pxor xmm1, xmm4
    mov eax, ebx
    test eax, eax
    jz round_last
round_loop:
    movdqu xmm4, XMMWORD PTR [ebp+eax]
    aesenc xmm0, xmm4
    aesenc xmm1, xmm4
    add eax, 16
    jnz round_loop
round_last:
    movdqu xmm4, XMMWORD PTR [ebp]
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
    pxor xmm6, xmm6
    pxor xmm7, xmm7
ENDM

; esi = in, edi = out, edx = blocks
AES_ECB MACRO op, last
    LOCAL four, one, done
four:
    cmp edx, 4
    jb one
    movdqu xmm0, XMMWORD PTR [esi]
    movdqu xmm1, XMMWORD PTR [esi+16]
    movdqu xmm2, XMMWORD PTR [esi+32]
    movdqu xmm3, XMMWORD PTR [esi+48]
    AES_ROUNDS4 op, last
    movdqu XMMWORD PTR [edi], xmm0
    movdqu XMMWORD PTR [edi+16], xmm1
    movdqu XMMWORD PTR [edi+32], xmm2
    movdqu XMMWORD PTR [edi+48], xmm3
    add esi, 64
    add edi, 64
    sub edx, 4
    jmp four
one:
    test edx, edx
    jz done
    movdqu xmm0, XMMWORD PTR [esi]
    AES_ROUNDS1 op, last
    movdqu XMMWORD PTR [edi], xmm0
    add esi, 16
    add edi, 16
    dec edx
    jmp one
done:
    AES_CLEAR
ENDM

; xmm5 = little-endian 128-bit counter, xmm7 = 0. Adds 1 to the low
; quadword, then subtracts the carry mask (all ones when the low quadword
; wrapped to zero) from the high one. Clobbers xmm4 and xmm6.
AES_CTR_BLOCK MACRO x
    pcmpeqd xmm4, xmm4
    psrlq xmm4, 63
    psrldq xmm4, 8
    paddq xmm5, xmm4
    movdqa xmm6, xmm5
    pcmpeqd xmm6, xmm7
    pshufd xmm4, xmm6, 0B1h
    pand xmm6, xmm4
    pslldq xmm6, 8
    psubq xmm5, xmm6
    movdqa x, xmm5
ENDM

; int sharkssl_aes_hw_probe(void): 1 when CPUID reports AES-NI and SSE2
PUBLIC sharkssl_aes_hw_probe
sharkssl_aes_hw_probe PROC C
    pushfd
    pop eax
    mov ecx, eax
    xor eax, 200000h
    push eax
    popfd
    pushfd
    pop eax
    push ecx
    popfd
    xor eax, ecx
    and eax, 200000h
    jz probe_2
    push ebx
    xor eax, eax
    cpuid
    cmp eax, 1
    jb probe_1
    mov eax, 1
    xor ecx, ecx
    cpuid
    mov eax, ecx
    shr eax, 25
    shr edx, 26
    and eax, edx
    and eax, 1
    pop ebx
    ret
probe_1:
    xor eax, eax
    pop ebx
probe_2:
    ret
sharkssl_aes_hw_probe ENDP

; void sharkssl_aes_hw_ecb_encrypt(const U32 *rk, U32 nr, const U8 *in, U8 *out, U32 blocks)
PUBLIC sharkssl_aes_hw_ecb_encrypt
ALIGN 16
sharkssl_aes_hw_ecb_encrypt PROC C
    AES_ENTER
    AES_SETUP
    mov esi, DWORD PTR [esp+28]
    mov edi, DWORD PTR [esp+32]
    mov edx, DWORD PTR [esp+36]
    AES_ECB aesenc, aesenclast
    AES_LEAVE
    ret
sharkssl_aes_hw_ecb_encrypt ENDP

; void sharkssl_aes_hw_ecb_decrypt(const U32 *rk, U32 nr, const U8 *in, U8 *out, U32 blocks)
PUBLIC sharkssl_aes_hw_ecb_decrypt
ALIGN 16
sharkssl_aes_hw_ecb_decrypt PROC C
    AES_ENTER
    AES_SETUP
    mov esi, DWORD PTR [esp+28]
    mov edi, DWORD PTR [esp+32]
    mov edx, DWORD PTR [esp+36]
    AES_ECB aesdec, aesdeclast
    AES_LEAVE
    ret
sharkssl_aes_hw_ecb_decrypt ENDP

; void sharkssl_aes_hw_cbc_encrypt(const U32 *rk, U32 nr, U8 vect[16], const U8 *in, U8 *out, U32 blocks)
PUBLIC sharkssl_aes_hw_cbc_encrypt
ALIGN 16
sharkssl_aes_hw_cbc_encrypt PROC C
    AES_ENTER
    AES_SETUP
    mov ecx, DWORD PTR [esp+28]
    mov esi, DWORD PTR [esp+32]
    mov edi, DWORD PTR [esp+36]
    mov edx, DWORD PTR [esp+40]
    movdqu xmm0, XMMWORD PTR [ecx]
cbc_encrypt_3:
    test edx, edx
    jz cbc_encrypt_4
    movdqu xmm4, XMMWORD PTR [esi]
    pxor xmm0, xmm4
    AES_ROUNDS1 aesenc, aesenclast
    movdqu XMMWORD PTR [edi], xmm0
    add esi, 16
    add edi, 16
    dec edx
    jmp cbc_encrypt_3
cbc_encrypt_4:
    movdqu XMMWORD PTR [ecx], xmm0
    AES_CLEAR
    AES_LEAVE
    ret
sharkssl_aes_hw_cbc_encrypt ENDP

; void sharkssl_aes_hw_cbc_decrypt(const U32 *rk, U32 nr, U8 vect[16], const U8 *in, U8 *out, U32 blocks)
; Every group reads its ciphertext before it writes: in == out works.
PUBLIC sharkssl_aes_hw_cbc_decrypt
ALIGN 16
sharkssl_aes_hw_cbc_decrypt PROC C
    AES_ENTER
    AES_SETUP
    mov ecx, DWORD PTR [esp+28]
    mov esi, DWORD PTR [esp+32]
    mov edi, DWORD PTR [esp+36]
    mov edx, DWORD PTR [esp+40]
    movdqu xmm5, XMMWORD PTR [ecx]
cbc_decrypt_3:
    cmp edx, 4
    jb cbc_decrypt_4
    movdqu xmm0, XMMWORD PTR [esi]
    movdqu xmm1, XMMWORD PTR [esi+16]
    movdqu xmm2, XMMWORD PTR [esi+32]
    movdqu xmm3, XMMWORD PTR [esi+48]
    AES_ROUNDS4 aesdec, aesdeclast
    pxor xmm0, xmm5
    movdqu xmm4, XMMWORD PTR [esi]
    pxor xmm1, xmm4
    movdqu xmm4, XMMWORD PTR [esi+16]
    pxor xmm2, xmm4
    movdqu xmm4, XMMWORD PTR [esi+32]
    pxor xmm3, xmm4
    movdqu xmm5, XMMWORD PTR [esi+48]
    movdqu XMMWORD PTR [edi], xmm0
    movdqu XMMWORD PTR [edi+16], xmm1
    movdqu XMMWORD PTR [edi+32], xmm2
    movdqu XMMWORD PTR [edi+48], xmm3
    add esi, 64
    add edi, 64
    sub edx, 4
    jmp cbc_decrypt_3
cbc_decrypt_4:
    test edx, edx
    jz cbc_decrypt_5
    movdqu xmm0, XMMWORD PTR [esi]
    AES_ROUNDS1 aesdec, aesdeclast
    pxor xmm0, xmm5
    movdqu xmm5, XMMWORD PTR [esi]
    movdqu XMMWORD PTR [edi], xmm0
    add esi, 16
    add edi, 16
    dec edx
    jmp cbc_decrypt_4
cbc_decrypt_5:
    movdqu XMMWORD PTR [ecx], xmm5
    AES_CLEAR
    AES_LEAVE
    ret
sharkssl_aes_hw_cbc_decrypt ENDP

; void sharkssl_aes_hw_ctr(const U32 *rk, U32 nr, U8 ctr[16], const U8 *in, U8 *out, U32 blocks)
; Each block increments the little-endian counter first, then encrypts it.
PUBLIC sharkssl_aes_hw_ctr
ALIGN 16
sharkssl_aes_hw_ctr PROC C
    AES_ENTER
    AES_SETUP
    mov ecx, DWORD PTR [esp+28]
    mov esi, DWORD PTR [esp+32]
    mov edi, DWORD PTR [esp+36]
    mov edx, DWORD PTR [esp+40]
    movdqu xmm5, XMMWORD PTR [ecx]
    pxor xmm7, xmm7
ctr_3:
    cmp edx, 4
    jb ctr_4
    AES_CTR_BLOCK xmm0
    AES_CTR_BLOCK xmm1
    AES_CTR_BLOCK xmm2
    AES_CTR_BLOCK xmm3
    AES_ROUNDS4 aesenc, aesenclast
    movdqu xmm4, XMMWORD PTR [esi]
    pxor xmm0, xmm4
    movdqu xmm4, XMMWORD PTR [esi+16]
    pxor xmm1, xmm4
    movdqu xmm4, XMMWORD PTR [esi+32]
    pxor xmm2, xmm4
    movdqu xmm4, XMMWORD PTR [esi+48]
    pxor xmm3, xmm4
    movdqu XMMWORD PTR [edi], xmm0
    movdqu XMMWORD PTR [edi+16], xmm1
    movdqu XMMWORD PTR [edi+32], xmm2
    movdqu XMMWORD PTR [edi+48], xmm3
    add esi, 64
    add edi, 64
    sub edx, 4
    jmp ctr_3
ctr_4:
    test edx, edx
    jz ctr_5
    AES_CTR_BLOCK xmm0
    AES_ROUNDS1 aesenc, aesenclast
    movdqu xmm4, XMMWORD PTR [esi]
    pxor xmm0, xmm4
    movdqu XMMWORD PTR [edi], xmm0
    add esi, 16
    add edi, 16
    dec edx
    jmp ctr_4
ctr_5:
    movdqu XMMWORD PTR [ecx], xmm5
    AES_CLEAR
    AES_LEAVE
    ret
sharkssl_aes_hw_ctr ENDP

; CCM over complete blocks. ecx = state: the pending CBC-MAC block, then the
; counter block, whose last four bytes are a big-endian counter; esi = in,
; edi = out, the block count stays in its stack argument at [esp+40]. Per
; block: increment the counter, encrypt the MAC block and the counter
; together, then out = in ^ E(counter) and MAC ^= the plaintext. xmm2 = the
; counter block with its counter bytes zeroed, edx = counter.
AES_CCM MACRO dec
    LOCAL ccm_loop, ccm_done
    movdqu xmm0, XMMWORD PTR [ecx]
    movdqu xmm2, XMMWORD PTR [ecx+16]
    pslldq xmm2, 4
    psrldq xmm2, 4
    mov edx, DWORD PTR [ecx+28]
    bswap edx
ccm_loop:
    cmp DWORD PTR [esp+40], 0
    je ccm_done
    add edx, 1
    mov eax, edx
    bswap eax
    movd xmm1, eax
    pslldq xmm1, 12
    por xmm1, xmm2
    AES_ROUNDS2
    movdqu xmm3, XMMWORD PTR [esi]
    pxor xmm1, xmm3
IF dec
    pxor xmm0, xmm1
ELSE
    pxor xmm0, xmm3
ENDIF
    movdqu XMMWORD PTR [edi], xmm1
    add esi, 16
    add edi, 16
    sub DWORD PTR [esp+40], 1
    jmp ccm_loop
ccm_done:
    movdqu XMMWORD PTR [ecx], xmm0
    bswap edx
    mov DWORD PTR [ecx+28], edx
    AES_CLEAR
ENDM

; void sharkssl_aes_hw_ccm_encrypt(const U32 *rk, U32 nr, U8 state[32], const U8 *in, U8 *out, U32 blocks)
PUBLIC sharkssl_aes_hw_ccm_encrypt
ALIGN 16
sharkssl_aes_hw_ccm_encrypt PROC C
    AES_ENTER
    AES_SETUP
    mov ecx, DWORD PTR [esp+28]
    mov esi, DWORD PTR [esp+32]
    mov edi, DWORD PTR [esp+36]
    AES_CCM 0
    AES_LEAVE
    ret
sharkssl_aes_hw_ccm_encrypt ENDP

; void sharkssl_aes_hw_ccm_decrypt(const U32 *rk, U32 nr, U8 state[32], const U8 *in, U8 *out, U32 blocks)
; Reads each block before it writes it: in == out works.
PUBLIC sharkssl_aes_hw_ccm_decrypt
ALIGN 16
sharkssl_aes_hw_ccm_decrypt PROC C
    AES_ENTER
    AES_SETUP
    mov ecx, DWORD PTR [esp+28]
    mov esi, DWORD PTR [esp+32]
    mov edi, DWORD PTR [esp+36]
    AES_CCM 1
    AES_LEAVE
    ret
sharkssl_aes_hw_ccm_decrypt ENDP

END
