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
 *   $Id: SharkSslAes_ARM64.S 6818 2026-11-08 21:18:02Z gianluca $
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

/* SharkSSL AES-CE block cipher for AArch64, the SHARKSSL_OPTIMIZED_AES_ASM
 * backend of SharkSslAesCtx: ECB, CBC, CTR, CCM. SharkSslCrypto.c calls it only
 * when sharkssl_arm64_capabilities() reports AES (SharkSslCrypto_ARM64.c).
 *
 * rk holds the nr + 1 FIPS 197 round keys as bytes (a decryption context:
 * the keys of the equivalent inverse cipher), written by SharkSslCrypto.c.
 * They are loaded once per call: k[0] to k[nr] in v(30 - nr) to v30. The
 * AES instructions and the loads and stores are constant time: no key or
 * data value selects an address or a branch; the branches depend on the
 * public round count and length. Only v0-v7 and v16-v31 are used (v8-v15
 * are callee-saved); they are cleared on return. */
#if !defined(__aarch64__) || defined(__AARCH64EB__) || !defined(__LP64__)
#error "Requires little-endian AArch64 LP64"
#endif
.text
.arch armv8-a+crypto

/* x0 = rk, w1 = nr (10, 12 or 14). Clobbers x9. */
.macro AES_LOAD_KEYS
    mov x9, x0
    cmp w1, #12
    b.lt 10f
    b.eq 12f
    ld1 {v16.16b, v17.16b}, [x9], #32
12:
    ld1 {v18.16b, v19.16b}, [x9], #32
10:
    ld1 {v20.16b, v21.16b, v22.16b, v23.16b}, [x9], #64
    ld1 {v24.16b, v25.16b, v26.16b, v27.16b}, [x9], #64
    ld1 {v28.16b, v29.16b, v30.16b}, [x9]
.endm

.macro AES_CLEAR
    movi v0.16b, #0
    movi v1.16b, #0
    movi v2.16b, #0
    movi v3.16b, #0
    movi v4.16b, #0
    movi v5.16b, #0
    movi v6.16b, #0
    movi v7.16b, #0
    movi v16.16b, #0
    movi v17.16b, #0
    movi v18.16b, #0
    movi v19.16b, #0
    movi v20.16b, #0
    movi v21.16b, #0
    movi v22.16b, #0
    movi v23.16b, #0
    movi v24.16b, #0
    movi v25.16b, #0
    movi v26.16b, #0
    movi v27.16b, #0
    movi v28.16b, #0
    movi v29.16b, #0
    movi v30.16b, #0
    movi v31.16b, #0
.endm

/* One round on v0-v3: op = aese/aesd, mix = aesmc/aesimc */
.macro AES_RND4 op, mix, k
    \op v0.16b, \k\().16b
    \mix v0.16b, v0.16b
    \op v1.16b, \k\().16b
    \mix v1.16b, v1.16b
    \op v2.16b, \k\().16b
    \mix v2.16b, v2.16b
    \op v3.16b, \k\().16b
    \mix v3.16b, v3.16b
.endm

.macro AES_RND1 op, mix, k
    \op v0.16b, \k\().16b
    \mix v0.16b, v0.16b
.endm

.macro AES_RND2 k
    aese v0.16b, \k\().16b
    aesmc v0.16b, v0.16b
    aese v1.16b, \k\().16b
    aesmc v1.16b, v1.16b
.endm

/* nr encryption rounds on v0-v1 */
.macro AES_ROUNDS2
    cmp w1, #12
    b.lt 10f
    b.eq 12f
    AES_RND2 v16
    AES_RND2 v17
12:
    AES_RND2 v18
    AES_RND2 v19
10:
    AES_RND2 v20
    AES_RND2 v21
    AES_RND2 v22
    AES_RND2 v23
    AES_RND2 v24
    AES_RND2 v25
    AES_RND2 v26
    AES_RND2 v27
    AES_RND2 v28
    aese v0.16b, v29.16b
    aese v1.16b, v29.16b
    eor v0.16b, v0.16b, v30.16b
    eor v1.16b, v1.16b, v30.16b
.endm

/* nr rounds on v0-v3; the last one has no MixColumns and adds k[nr] */
.macro AES_ROUNDS4 op, mix
    cmp w1, #12
    b.lt 10f
    b.eq 12f
    AES_RND4 \op, \mix, v16
    AES_RND4 \op, \mix, v17
12:
    AES_RND4 \op, \mix, v18
    AES_RND4 \op, \mix, v19
10:
    AES_RND4 \op, \mix, v20
    AES_RND4 \op, \mix, v21
    AES_RND4 \op, \mix, v22
    AES_RND4 \op, \mix, v23
    AES_RND4 \op, \mix, v24
    AES_RND4 \op, \mix, v25
    AES_RND4 \op, \mix, v26
    AES_RND4 \op, \mix, v27
    AES_RND4 \op, \mix, v28
    \op v0.16b, v29.16b
    \op v1.16b, v29.16b
    \op v2.16b, v29.16b
    \op v3.16b, v29.16b
    eor v0.16b, v0.16b, v30.16b
    eor v1.16b, v1.16b, v30.16b
    eor v2.16b, v2.16b, v30.16b
    eor v3.16b, v3.16b, v30.16b
.endm

.macro AES_ROUNDS1 op, mix
    cmp w1, #12
    b.lt 10f
    b.eq 12f
    AES_RND1 \op, \mix, v16
    AES_RND1 \op, \mix, v17
12:
    AES_RND1 \op, \mix, v18
    AES_RND1 \op, \mix, v19
10:
    AES_RND1 \op, \mix, v20
    AES_RND1 \op, \mix, v21
    AES_RND1 \op, \mix, v22
    AES_RND1 \op, \mix, v23
    AES_RND1 \op, \mix, v24
    AES_RND1 \op, \mix, v25
    AES_RND1 \op, \mix, v26
    AES_RND1 \op, \mix, v27
    AES_RND1 \op, \mix, v28
    \op v0.16b, v29.16b
    eor v0.16b, v0.16b, v30.16b
.endm

/* x2 = in, x3 = out, w4 = blocks */
.macro AES_ECB op, mix
    AES_LOAD_KEYS
1:
    cmp w4, #4
    b.lo 2f
    ld1 {v0.16b, v1.16b, v2.16b, v3.16b}, [x2], #64
    AES_ROUNDS4 \op, \mix
    st1 {v0.16b, v1.16b, v2.16b, v3.16b}, [x3], #64
    sub w4, w4, #4
    b 1b
2:
    cbz w4, 3f
    ld1 {v0.16b}, [x2], #16
    AES_ROUNDS1 \op, \mix
    st1 {v0.16b}, [x3], #16
    sub w4, w4, #1
    b 2b
3:
    AES_CLEAR
.endm

/* x6:x7 = little-endian 128-bit counter: increment, then copy to \v */
.macro AES_CTR_BLOCK v
    adds x6, x6, #1
    adc x7, x7, xzr
    mov \v\().d[0], x6
    mov \v\().d[1], x7
.endm

/* void sharkssl_aes_hw_ecb_encrypt(const U32 *rk, U32 nr, const U8 *in, U8 *out, U32 blocks) */
.p2align 4
.global sharkssl_aes_hw_ecb_encrypt
.type sharkssl_aes_hw_ecb_encrypt, %function
sharkssl_aes_hw_ecb_encrypt:
.cfi_startproc
    AES_ECB aese, aesmc
    ret
.cfi_endproc
.size sharkssl_aes_hw_ecb_encrypt, .-sharkssl_aes_hw_ecb_encrypt

/* void sharkssl_aes_hw_ecb_decrypt(const U32 *rk, U32 nr, const U8 *in, U8 *out, U32 blocks) */
.p2align 4
.global sharkssl_aes_hw_ecb_decrypt
.type sharkssl_aes_hw_ecb_decrypt, %function
sharkssl_aes_hw_ecb_decrypt:
.cfi_startproc
    AES_ECB aesd, aesimc
    ret
.cfi_endproc
.size sharkssl_aes_hw_ecb_decrypt, .-sharkssl_aes_hw_ecb_decrypt

/* void sharkssl_aes_hw_cbc_encrypt(const U32 *rk, U32 nr, U8 vect[16], const U8 *in, U8 *out, U32 blocks) */
.p2align 4
.global sharkssl_aes_hw_cbc_encrypt
.type sharkssl_aes_hw_cbc_encrypt, %function
sharkssl_aes_hw_cbc_encrypt:
.cfi_startproc
    AES_LOAD_KEYS
    ld1 {v0.16b}, [x2]
1:
    cbz w5, 2f
    ld1 {v4.16b}, [x3], #16
    eor v0.16b, v0.16b, v4.16b
    AES_ROUNDS1 aese, aesmc
    st1 {v0.16b}, [x4], #16
    sub w5, w5, #1
    b 1b
2:
    st1 {v0.16b}, [x2]
    AES_CLEAR
    ret
.cfi_endproc
.size sharkssl_aes_hw_cbc_encrypt, .-sharkssl_aes_hw_cbc_encrypt

/* void sharkssl_aes_hw_cbc_decrypt(const U32 *rk, U32 nr, U8 vect[16], const U8 *in, U8 *out, U32 blocks)
 * Every group reads its ciphertext before it writes: in == out works. */
.p2align 4
.global sharkssl_aes_hw_cbc_decrypt
.type sharkssl_aes_hw_cbc_decrypt, %function
sharkssl_aes_hw_cbc_decrypt:
.cfi_startproc
    AES_LOAD_KEYS
    ld1 {v31.16b}, [x2]
1:
    cmp w5, #4
    b.lo 2f
    ld1 {v0.16b, v1.16b, v2.16b, v3.16b}, [x3]
    ld1 {v4.16b, v5.16b, v6.16b, v7.16b}, [x3], #64
    AES_ROUNDS4 aesd, aesimc
    eor v0.16b, v0.16b, v31.16b
    eor v1.16b, v1.16b, v4.16b
    eor v2.16b, v2.16b, v5.16b
    eor v3.16b, v3.16b, v6.16b
    mov v31.16b, v7.16b
    st1 {v0.16b, v1.16b, v2.16b, v3.16b}, [x4], #64
    sub w5, w5, #4
    b 1b
2:
    cbz w5, 3f
    ld1 {v0.16b}, [x3]
    ld1 {v4.16b}, [x3], #16
    AES_ROUNDS1 aesd, aesimc
    eor v0.16b, v0.16b, v31.16b
    mov v31.16b, v4.16b
    st1 {v0.16b}, [x4], #16
    sub w5, w5, #1
    b 2b
3:
    st1 {v31.16b}, [x2]
    AES_CLEAR
    ret
.cfi_endproc
.size sharkssl_aes_hw_cbc_decrypt, .-sharkssl_aes_hw_cbc_decrypt

/* void sharkssl_aes_hw_ctr(const U32 *rk, U32 nr, U8 ctr[16], const U8 *in, U8 *out, U32 blocks)
 * Each block increments the little-endian counter first, then encrypts it. */
.p2align 4
.global sharkssl_aes_hw_ctr
.type sharkssl_aes_hw_ctr, %function
sharkssl_aes_hw_ctr:
.cfi_startproc
    AES_LOAD_KEYS
    ldp x6, x7, [x2]
1:
    cmp w5, #4
    b.lo 2f
    AES_CTR_BLOCK v0
    AES_CTR_BLOCK v1
    AES_CTR_BLOCK v2
    AES_CTR_BLOCK v3
    AES_ROUNDS4 aese, aesmc
    ld1 {v4.16b, v5.16b, v6.16b, v7.16b}, [x3], #64
    eor v0.16b, v0.16b, v4.16b
    eor v1.16b, v1.16b, v5.16b
    eor v2.16b, v2.16b, v6.16b
    eor v3.16b, v3.16b, v7.16b
    st1 {v0.16b, v1.16b, v2.16b, v3.16b}, [x4], #64
    sub w5, w5, #4
    b 1b
2:
    cbz w5, 3f
    AES_CTR_BLOCK v0
    AES_ROUNDS1 aese, aesmc
    ld1 {v4.16b}, [x3], #16
    eor v0.16b, v0.16b, v4.16b
    st1 {v0.16b}, [x4], #16
    sub w5, w5, #1
    b 2b
3:
    stp x6, x7, [x2]
    AES_CLEAR
    ret
.cfi_endproc
.size sharkssl_aes_hw_ctr, .-sharkssl_aes_hw_ctr

/* CCM over complete blocks. x2 = state: the pending CBC-MAC block, then the
 * counter block, whose last four bytes are a big-endian counter; x3 = in,
 * x4 = out, w5 = blocks. Per block: increment the counter, encrypt the MAC
 * block and the counter together, then out = in ^ E(counter) and MAC ^= the
 * plaintext. v0 = MAC block, v2 = counter block, w6 = counter. */
.macro AES_CCM dec
    AES_LOAD_KEYS
    ldp q0, q2, [x2]
    mov w6, v2.s[3]
    rev w6, w6
1:
    cbz w5, 2f
    add w6, w6, #1
    rev w7, w6
    mov v1.16b, v2.16b
    mov v1.s[3], w7
    AES_ROUNDS2
    ld1 {v3.16b}, [x3], #16
    eor v1.16b, v1.16b, v3.16b
.if \dec
    eor v0.16b, v0.16b, v1.16b
.else
    eor v0.16b, v0.16b, v3.16b
.endif
    st1 {v1.16b}, [x4], #16
    sub w5, w5, #1
    b 1b
2:
    rev w7, w6
    mov v2.s[3], w7
    stp q0, q2, [x2]
    AES_CLEAR
.endm

/* void sharkssl_aes_hw_ccm_encrypt(const U32 *rk, U32 nr, U8 state[32], const U8 *in, U8 *out, U32 blocks) */
.p2align 4
.global sharkssl_aes_hw_ccm_encrypt
.type sharkssl_aes_hw_ccm_encrypt, %function
sharkssl_aes_hw_ccm_encrypt:
.cfi_startproc
    AES_CCM 0
    ret
.cfi_endproc
.size sharkssl_aes_hw_ccm_encrypt, .-sharkssl_aes_hw_ccm_encrypt

/* void sharkssl_aes_hw_ccm_decrypt(const U32 *rk, U32 nr, U8 state[32], const U8 *in, U8 *out, U32 blocks)
 * Reads each block before it writes it: in == out works. */
.p2align 4
.global sharkssl_aes_hw_ccm_decrypt
.type sharkssl_aes_hw_ccm_decrypt, %function
sharkssl_aes_hw_ccm_decrypt:
.cfi_startproc
    AES_CCM 1
    ret
.cfi_endproc
.size sharkssl_aes_hw_ccm_decrypt, .-sharkssl_aes_hw_ccm_decrypt

.section .note.GNU-stack,"",%progbits
