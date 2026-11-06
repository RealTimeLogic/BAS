/*
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
 *   $Id: SharkSslCrypto.h 6756 2026-11-05 17:13:26Z gianluca $
 *
 *   COPYRIGHT:  Real Time Logic LLC, 2010 - 2026
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
#ifndef _SharkSslCrypto_h
#define _SharkSslCrypto_h

#define SHARKSSL_LIB 1
#include "SharkSSL.h"


#define SHARKSSL_MD5_HASH_LEN       16
#define SHARKSSL_SHA1_HASH_LEN      20
#define SHARKSSL_SHA224_HASH_LEN    28
#define SHARKSSL_SHA256_HASH_LEN    32
#define SHARKSSL_SHA384_HASH_LEN    48
#define SHARKSSL_SHA512_HASH_LEN    64
#define SHARKSSL_POLY1305_HASH_LEN  16

#define SHARKSSL_MD5_BLOCK_LEN      64
#define SHARKSSL_SHA1_BLOCK_LEN     64
#define SHARKSSL_SHA256_BLOCK_LEN   64
#define SHARKSSL_SHA384_BLOCK_LEN   128
#define SHARKSSL_SHA512_BLOCK_LEN   128

/* RFC 8032 EdDSA prehash output length */
#define SHARKSSL_EDDSA_PREHASH_LEN  64

/** to be used as 'hashID' parameter in SharkSslHMACCtx_constructor 
 */
#define SHARKSSL_HASHID_MD5         0x01
#define SHARKSSL_HASHID_SHA1        0x02
#define SHARKSSL_HASHID_SHA224      0x03
#define SHARKSSL_HASHID_SHA256      0x04 
#define SHARKSSL_HASHID_SHA384      0x05
#define SHARKSSL_HASHID_SHA512      0x06


#if SHARKSSL_USE_MD5
/** MD5
    \ingroup RayCryptoMD5
*/
typedef struct SharkSslMd5Ctx
{
   U32 total[2];
   U32 state[4];
   U8  buffer[64];
} SharkSslMd5Ctx;
#endif


#if SHARKSSL_USE_SHA1
/** SHA1
    \ingroup RayCryptoSHA1
*/
typedef struct SharkSslSha1Ctx
{
   #if SHARKSSL_PSA_CONSUMER
   psa_hash_operation_t operation;
   psa_status_t status;
   #else
   U32 total[2];
   U32 state[5];
   U8  buffer[64];
   #endif
} SharkSslSha1Ctx;
#endif


#if (SHARKSSL_USE_SHA_224 && SHARKSSL_PSA_CONSUMER)
typedef struct SharkSslSha224Ctx
{
   psa_hash_operation_t operation;
   psa_status_t status;
} SharkSslSha224Ctx;
#endif


#if SHARKSSL_USE_SHA_256
/** SHA256
    \ingroup RayCryptoSHA256
*/
typedef struct SharkSslSha256Ctx
{
   #if SHARKSSL_PSA_CONSUMER
   psa_hash_operation_t operation;
   psa_status_t status;
   #else
   U32 total[2];
   U32 state[8];
   U8  buffer[64];
   #endif
} SharkSslSha256Ctx;
#endif


/**
 * In PSA consumer builds, hash contexts own provider operations. Do not copy
 * an active context with memcpy; use the corresponding clone function with an
 * uninitialized target. Consumer methods return zero on success and -1 on
 * error. Abort an unfinished context before reuse.
 */


#if (SHARKSSL_USE_SHA_384 || SHARKSSL_USE_SHA_512)
/** SHA384
    \ingroup RayCryptoSHA384
*/
typedef struct SharkSslSha384Ctx
{
   #if SHARKSSL_PSA_CONSUMER
   psa_hash_operation_t operation;
   psa_status_t status;
   #else
   U32 total[4];
   U64 state[8];
   U8  buffer[128];
   #endif
} SharkSslSha384Ctx;
#endif


#if SHARKSSL_USE_SHA_512
/** SHA512
    \ingroup RayCryptoSHA512
*/
typedef struct SharkSslSha384Ctx SharkSslSha512Ctx;
#endif


#if (SHARKSSL_USE_SHA_512 || SHARKSSL_USE_SHA_384 || SHARKSSL_USE_SHA_256 || SHARKSSL_USE_SHA1 || SHARKSSL_USE_MD5)
/** HMAC
    \ingroup RayCryptoHMAC
*/
/**
 * In PSA consumer builds, this context owns a volatile PSA key. Do not copy
 * it. Finish and destructor release the key; call destructor if a message is
 * abandoned. Consumer methods return zero on success and -1 on error.
 */
typedef struct SharkSslHMACCtx
{
   #if SHARKSSL_PSA_CONSUMER
   psa_mac_operation_t operation;
   psa_key_id_t key;
   psa_status_t status;
   U8 hashID;
   #else
   union 
   {
      #if SHARKSSL_USE_MD5
      SharkSslMd5Ctx md5Ctx;
      #endif
      #if SHARKSSL_USE_SHA1
      SharkSslSha1Ctx sha1Ctx;
      #endif
      #if SHARKSSL_USE_SHA_256
      SharkSslSha256Ctx sha256Ctx;
      #endif
      #if SHARKSSL_USE_SHA_384
      SharkSslSha384Ctx sha384Ctx;
      #endif
      #if SHARKSSL_USE_SHA_512
      SharkSslSha512Ctx sha512Ctx;
      #endif
   } hashCtx;
   
   /* descending block length */
   #if   SHARKSSL_USE_SHA_512
   U8 key[SHARKSSL_SHA512_BLOCK_LEN];
   #elif SHARKSSL_USE_SHA_384
   U8 key[SHARKSSL_SHA384_BLOCK_LEN];
   #elif SHARKSSL_USE_SHA_256
   U8 key[SHARKSSL_SHA256_BLOCK_LEN];
   #elif SHARKSSL_USE_SHA1
   U8 key[SHARKSSL_SHA1_BLOCK_LEN];
   #elif SHARKSSL_USE_MD5
   U8 key[SHARKSSL_MD5_BLOCK_LEN];
   #endif
   
   U8 hashID;
   #endif
} SharkSslHMACCtx;
#endif


#if SHARKSSL_USE_POLY1305
/** POLY1305
    \ingroup RayCryptoPOLY1305
*/
typedef struct SharkSslPoly1305Ctx
{
   U32 r[5];
   U32 key[4];
   U32 nonce[4];
   U8  buffer[16];
   U8  flag, blen;
} SharkSslPoly1305Ctx;
#endif


#if SHARKSSL_USE_CHACHA20
/** CHACHA20
    \ingroup RayCryptoCHACHA20
*/
typedef struct
{
   #if SHARKSSL_PSA_CONSUMER_STANDALONE_CHACHA20
   psa_cipher_operation_t operation;
   psa_key_id_t key;
   psa_status_t status;
   U8 stream[64];
   U8 used;
   U8 active;
   #else
   U32 state[16];
   #endif
} SharkSslChaChaCtx;
#endif


#if (SHARKSSL_USE_AES_256 || SHARKSSL_USE_AES_192 || SHARKSSL_USE_AES_128)
/** AES
    \ingroup RayCryptoAES
*/
typedef struct SharkSslAesCtx
{
   #if SHARKSSL_PSA_CONSUMER_STANDALONE_AES
   U8 key[32];
   U8 keyLen;
   U8 valid;
   #else
   #if (SHARKSSL_USE_AES_256 || SHARKSSL_NOPACK)
   U32 key[60];
   #elif SHARKSSL_USE_AES_192
   U32 key[52];
   #else
   U32 key[44];
   #endif
   U16 nr;
   #endif
} SharkSslAesCtx;

typedef enum
{
   SharkSslAesCtx_Decrypt,
   SharkSslAesCtx_Encrypt
} SharkSslAesCtx_Type;

#if SHARKSSL_ENABLE_AES_GCM
/** AesGcmCtx
    \ingroup RayCryptoAesGcm
*/
typedef struct SharkSslAesGcmCtx
{
   SharkSslAesCtx super;
   #if (!SHARKSSL_PSA_CONSUMER_STANDALONE_AES)
   U8 M0[16][16];
   #endif
} SharkSslAesGcmCtx;
#endif

#if SHARKSSL_ENABLE_AES_CCM
/** AesCcmCtx
    \ingroup RayCryptoAesCcm
*/
typedef struct SharkSslAesCcmCtx
{
   SharkSslAesCtx super;
   U8 tagLen;
} SharkSslAesCcmCtx;
#endif
#endif  /* SHARKSSL_USE_AES_256 || SHARKSSL_USE_AES_192 || SHARKSSL_USE_AES_128 */


#ifdef __cplusplus
extern "C" {
#endif

/* SharkSslCrypto.c */
#if SHARKSSL_PSA_CONSUMER
/**
 * Initialize the selected PSA Crypto provider. Complete any platform-specific
 * hardware or storage setup before calling this function. Entropy is managed
 * by the provider. Returns zero on success, -1 on initialization failure.
 */
SHARKSSL_API int sharkssl_entropy(void);
#else
/**
 * Seed the native global DRBG atomically from an entropy buffer. Before the
 * first output supply at least 48 entropy bytes; later reseeds require at
 * least 32. The source must provide the required entropy, not just length.
 * Initialize before starting threads. Returns zero only when ready, -1 on
 * invalid input.
 */
SHARKSSL_API int sharkssl_entropyEx(const U8 *entropy, U16 len);
#endif


SHARKSSL_API int sharkssl_rng(U8*, U16);
SHARKSSL_API int sharkssl_kmemcmp(const void *a, const void *b, U32 n);


#if SHARKSSL_USE_MD5
/** Initialize
    \ingroup RayCryptoMD5

    \param ctx Uninitialized data of size sizeof(SharkSslMd5Ctx).
*/
SHARKSSL_API void  SharkSslMd5Ctx_constructor(SharkSslMd5Ctx *ctx);

/** append
    \ingroup RayCryptoMD5
*/
SHARKSSL_API void  SharkSslMd5Ctx_append(SharkSslMd5Ctx *ctx, const U8 *data, U32 len);

/** finish
    \ingroup RayCryptoMD5
*/
SHARKSSL_API void  SharkSslMd5Ctx_finish(SharkSslMd5Ctx *ctx, U8 digest[SHARKSSL_MD5_HASH_LEN]);

/** md5
    \ingroup RayCryptoMD5
*/
SHARKSSL_API int   sharkssl_md5(const U8*, U32, U8*);
#endif

#if SHARKSSL_USE_SHA1
/** Initialize
    \ingroup RayCryptoSHA1

    \param ctx Uninitialized data of size sizeof(SharkSslSha1Ctx).
*/
SHARKSSL_API int SharkSslSha1Ctx_constructor(SharkSslSha1Ctx *ctx);

/** append
    \ingroup RayCryptoSHA1
*/
SHARKSSL_API int SharkSslSha1Ctx_append(SharkSslSha1Ctx *ctx, const U8 *data, U32 len);

/** finish
    \ingroup RayCryptoSHA1
*/
SHARKSSL_API int SharkSslSha1Ctx_finish(SharkSslSha1Ctx *ctx, U8 digest[SHARKSSL_SHA1_HASH_LEN]);
#if SHARKSSL_PSA_CONSUMER
SHARKSSL_API int SharkSslSha1Ctx_clone(const SharkSslSha1Ctx *source, SharkSslSha1Ctx *target);
SHARKSSL_API int SharkSslSha1Ctx_abort(SharkSslSha1Ctx *ctx);
#endif

/** sha1
    \ingroup RayCryptoSHA1
*/
SHARKSSL_API int   sharkssl_sha1(const U8*, U32, U8*);
#endif

#if (SHARKSSL_USE_SHA_224 && SHARKSSL_PSA_CONSUMER)
SHARKSSL_API int SharkSslSha224Ctx_constructor(SharkSslSha224Ctx *ctx);
SHARKSSL_API int SharkSslSha224Ctx_append(SharkSslSha224Ctx *ctx, const U8 *data, U32 len);
SHARKSSL_API int SharkSslSha224Ctx_finish(SharkSslSha224Ctx *ctx, U8 digest[SHARKSSL_SHA224_HASH_LEN]);
SHARKSSL_API int SharkSslSha224Ctx_clone(const SharkSslSha224Ctx *source, SharkSslSha224Ctx *target);
SHARKSSL_API int SharkSslSha224Ctx_abort(SharkSslSha224Ctx *ctx);
SHARKSSL_API int sharkssl_sha224(const U8 *data, U32 len, U8 *digest);
#endif

#if SHARKSSL_USE_SHA_256
/** Initialize
    \ingroup RayCryptoSHA256

    \param ctx Uninitialized data of size sizeof(SharkSslSha256Ctx).
*/
SHARKSSL_API int SharkSslSha256Ctx_constructor(SharkSslSha256Ctx *ctx);

/** append
    \ingroup RayCryptoSHA256
*/
SHARKSSL_API int SharkSslSha256Ctx_append(SharkSslSha256Ctx*, const U8 *data, U32 len);

/** finish
    \ingroup RayCryptoSHA256
*/
SHARKSSL_API int SharkSslSha256Ctx_finish(SharkSslSha256Ctx*, U8 digest[SHARKSSL_SHA256_HASH_LEN]);
#if SHARKSSL_PSA_CONSUMER
SHARKSSL_API int SharkSslSha256Ctx_clone(const SharkSslSha256Ctx *source, SharkSslSha256Ctx *target);
SHARKSSL_API int SharkSslSha256Ctx_abort(SharkSslSha256Ctx *ctx);
#endif

/** sha256
    \ingroup RayCryptoSHA256
*/
SHARKSSL_API int   sharkssl_sha256(const U8*, U32, U8*);
#endif

#if SHARKSSL_USE_SHA_384
/** Initialize
    \ingroup RayCryptoSHA384

    \param ctx Uninitialized data of size sizeof(SharkSslSha384Ctx).
*/
SHARKSSL_API int SharkSslSha384Ctx_constructor(SharkSslSha384Ctx *ctx);
#endif

#if (SHARKSSL_USE_SHA_384 || SHARKSSL_USE_SHA_512)

/** append
    \ingroup RayCryptoSHA384
*/
SHARKSSL_API int SharkSslSha384Ctx_append(SharkSslSha384Ctx*, const U8 *data, U32 len);

/** finish
    \ingroup RayCryptoSHA384
*/
SHARKSSL_API int SharkSslSha384Ctx_finish(SharkSslSha384Ctx*, U8 digest[SHARKSSL_SHA384_HASH_LEN]);
#if SHARKSSL_PSA_CONSUMER
SHARKSSL_API int SharkSslSha384Ctx_clone(const SharkSslSha384Ctx *source, SharkSslSha384Ctx *target);
SHARKSSL_API int SharkSslSha384Ctx_abort(SharkSslSha384Ctx *ctx);
#endif
#endif

#if SHARKSSL_USE_SHA_384

/** sha384
    \ingroup RayCryptoSHA384
*/
SHARKSSL_API int   sharkssl_sha384(const U8*, U32, U8*);
#endif

#if SHARKSSL_USE_SHA_512
/** Initialize
    \ingroup RayCryptoSHA512

    \param ctx Uninitialized data of size sizeof(SharkSslSha512Ctx).
*/
SHARKSSL_API int SharkSslSha512Ctx_constructor(SharkSslSha512Ctx *ctx);

/** append
    \ingroup RayCryptoSHA512
*/
SHARKSSL_API int SharkSslSha512Ctx_append(SharkSslSha512Ctx *ctx, const U8 *data, U32 len);

/** finish
    \ingroup RayCryptoSHA512
*/
SHARKSSL_API int SharkSslSha512Ctx_finish(SharkSslSha512Ctx *ctx, U8 digest[SHARKSSL_SHA512_HASH_LEN]);
#if SHARKSSL_PSA_CONSUMER
SHARKSSL_API int SharkSslSha512Ctx_clone(const SharkSslSha512Ctx *source, SharkSslSha512Ctx *target);
SHARKSSL_API int SharkSslSha512Ctx_abort(SharkSslSha512Ctx *ctx);
#endif

/** sha512
    \ingroup RayCryptoSHA512
*/
SHARKSSL_API int   sharkssl_sha512(const U8*, U32, U8*);
#endif

#if (SHARKSSL_USE_SHA_512 || SHARKSSL_USE_SHA_384 || SHARKSSL_USE_SHA_256 || SHARKSSL_USE_SHA1 || SHARKSSL_USE_MD5)
/** Initialize
    \ingroup RayCryptoHMAC

    \param ctx Uninitialized data of size sizeof(SharkSslHMACCtx).
    \param hashID the hash function to be used, can be one of: 
     SHARKSSL_HASHID_MD5, SHARKSSL_HASHID_SHA1, SHARKSSL_HASHID_SHA256, SHARKSSL_HASHID_SHA384, SHARKSSL_HASHID_SHA512
    \param key pointer to the cryptographic key
    \param keyLen 'key' length
*/
SHARKSSL_API int SharkSslHMACCtx_constructor(SharkSslHMACCtx *ctx, U8 hashID, const U8 *key, U16 keyLen);

/** append
    \ingroup RayCryptoHMAC
*/
SHARKSSL_API int SharkSslHMACCtx_append(SharkSslHMACCtx*, const U8 *data, U32 len);

/** finish
    \ingroup RayCryptoHMAC
*/
SHARKSSL_API int SharkSslHMACCtx_finish(SharkSslHMACCtx*, U8 *HMAC);
SHARKSSL_API int SharkSslHMACCtx_destructor(SharkSslHMACCtx *ctx);

/** HMAC
    \ingroup RayCryptoHMAC

    \param hashID the hash function to be used, can be one of: 
     SHARKSSL_HASHID_MD5, SHARKSSL_HASHID_SHA1, SHARKSSL_HASHID_SHA256, SHARKSSL_HASHID_SHA384, SHARKSSL_HASHID_SHA512
    \param data pointer to the data to be hashed
    \param len data length in bytes
    \param key pointer to the cryptographic key
    \param keyLen 'key' length
    \param digest output HMAC
*/
SHARKSSL_API int sharkssl_HMAC(const U8 hashID, const U8 *data, U32 len, const U8 *key, U16 keyLen, U8 *digest);
#endif

void sharkssl_clear(void *data, U32 len);
U16 sharkssl_getHashLen(U8 hashID);
int sharkssl_hash(U8 *digest, U8 *data, U32 len, U8 hashID);


#if SHARKSSL_USE_POLY1305
/** Initialize
    \ingroup RayCryptoPOLY1305

    \param ctx Uninitialized data of size sizeof(SharkSslPoly1305Ctx).
    \param key 16-byte key + 16-byte nonce
*/
SHARKSSL_API void  SharkSslPoly1305Ctx_constructor(SharkSslPoly1305Ctx *ctx, const U8 key[32]);

#define SharkSslPoly1305Ctx_destructor(o) sharkssl_clear(o, sizeof(SharkSslPoly1305Ctx))

/** append
    \ingroup RayCryptoPOLY1305
*/
SHARKSSL_API void  SharkSslPoly1305Ctx_append(SharkSslPoly1305Ctx *ctx, const U8 *in, U32 len);

/** finish
    \ingroup RayCryptoPOLY1305
*/
SHARKSSL_API void  SharkSslPoly1305Ctx_finish(SharkSslPoly1305Ctx *ctx, U8 digest[SHARKSSL_POLY1305_HASH_LEN]);

/** poly1305
    \ingroup RayCryptoPOLY1305
*/
SHARKSSL_API int   sharkssl_poly1305(const U8 *data, U32 len, U8 *digest, const U8 key[32]);
#endif

#if SHARKSSL_USE_CHACHA20
#if SHARKSSL_PSA_CONSUMER_STANDALONE_CHACHA20
#define SHARKSSL_CHACHA_API_STATUS int
#else
#define SHARKSSL_CHACHA_API_STATUS void
#endif
/** Initialize
    \ingroup RayCryptoCHACHA20

    \param ctx Uninitialized data of size sizeof(SharkSslChaChaCtx)
    \param key 16- or 32-byte key; the PSA consumer profile requires 32 bytes
    \param keyLen the key length in bytes (32 in the PSA consumer profile)
*/
SHARKSSL_API SHARKSSL_CHACHA_API_STATUS SharkSslChaChaCtx_constructor(SharkSslChaChaCtx *ctx,
                                                                      const U8 *key, U8 keyLen);

#if SHARKSSL_PSA_CONSUMER_STANDALONE_CHACHA20
SHARKSSL_API int SharkSslChaChaCtx_destructor(SharkSslChaChaCtx *ctx);
#else
#define SharkSslChaChaCtx_destructor(ctx) sharkssl_clear(ctx, sizeof(SharkSslChaChaCtx))
#endif

/** Initialize
    \ingroup RayCryptoCHACHA20

    \param ctx Initialized data of size sizeof(SharkSslChaChaCtx)
    \param IV 12-byte initialization vector (per RFC 7905)
*/
SHARKSSL_API SHARKSSL_CHACHA_API_STATUS SharkSslChaChaCtx_setIV(SharkSslChaChaCtx *ctx, const U8 IV[12]);

/** Encrypt or decrypt
    \ingroup RayCryptoCHACHA20
*/
SHARKSSL_API SHARKSSL_CHACHA_API_STATUS SharkSslChaChaCtx_crypt(
   SharkSslChaChaCtx *ctx, const U8 *input, U8 *output, U32 len);
#undef SHARKSSL_CHACHA_API_STATUS
#if SHARKSSL_PSA_CONSUMER_STANDALONE_CHACHA20
SHARKSSL_API int sharkssl_chacha20poly1305_encrypt(const U8 key[32], const U8 nonce[12],
                                                   const U8 *aad, const U8 *input, U8 *output, U8 tag[16], U32 len, U16 aadLen);
SHARKSSL_API int sharkssl_chacha20poly1305_decrypt(const U8 key[32], const U8 nonce[12],
                                                   const U8 *aad, const U8 *input, const U8 tag[16], U8 *output, U32 len, U16 aadLen);
#endif
#endif


#if (SHARKSSL_USE_AES_256 || SHARKSSL_USE_AES_192 || SHARKSSL_USE_AES_128)
#if SHARKSSL_PSA_CONSUMER_STANDALONE_AES
#define SHARKSSL_AES_API_STATUS int
#else
#define SHARKSSL_AES_API_STATUS void
#endif
/** Initialize
    \ingroup RayCryptoAES

    \param ctx Uninitialized data of size sizeof(SharkSslAesCtx)
    \param type encrypt/decrypt
    \param key the encryption/decryption key
    \param keyLen 'key' length
*/
SHARKSSL_API SHARKSSL_AES_API_STATUS SharkSslAesCtx_constructor(SharkSslAesCtx *ctx,
                                                                SharkSslAesCtx_Type type,
                                                                const U8 *key, U8 keyLen);
#define SharkSslAesCtx_destructor(ctx) sharkssl_clear(ctx, sizeof(SharkSslAesCtx))

#if (!SHARKSSL_DISABLE_AES_ECB_DECRYPT)
/** Decrypt
    \ingroup RayCryptoAES
*/
SHARKSSL_API SHARKSSL_AES_API_STATUS SharkSslAesCtx_decrypt(SharkSslAesCtx *ctx, const U8 input[16], U8 output[16]);
#endif

/** Encrypt
    \ingroup RayCryptoAES
*/
SHARKSSL_API SHARKSSL_AES_API_STATUS SharkSslAesCtx_encrypt(SharkSslAesCtx *ctx, U8 input[16], U8 output[16]);

#if SHARKSSL_ENABLE_AES_CBC
/** Encrypt
    \ingroup RayCryptoAES
*/
SHARKSSL_API SHARKSSL_AES_API_STATUS SharkSslAesCtx_cbc_encrypt(SharkSslAesCtx *ctx, U8 vect[16],
                                                                const U8 *input, U8 *output, U32 len);

/** Decrypt
    \ingroup RayCryptoAES
*/
SHARKSSL_API SHARKSSL_AES_API_STATUS SharkSslAesCtx_cbc_decrypt(SharkSslAesCtx *ctx, U8 vect[16],
                                                                const U8 *input, U8 *output, U32 len);
#if (SHARKSSL_PSA_CONSUMER_STANDALONE_AES && defined(PSA_ALG_CBC_PKCS7))
/**
 * CBC with PKCS#7 padding. On failure, output is cleared and outputLen is zero.
 */
SHARKSSL_API int SharkSslAesCtx_cbc_pkcs7_encrypt(SharkSslAesCtx *ctx, const U8 iv[16],
                                                  const U8 *input, U8 *output, U32 *outputLen, U32 len, U32 outputCapacity);
SHARKSSL_API int SharkSslAesCtx_cbc_pkcs7_decrypt(SharkSslAesCtx *ctx, const U8 iv[16],
                                                  const U8 *input, U8 *output, U32 *outputLen, U32 len, U32 outputCapacity);
#endif
#endif
#if SHARKSSL_ENABLE_AES_CTR_MODE

/**
    \ingroup RayCryptoAES
*/
SHARKSSL_API SHARKSSL_AES_API_STATUS SharkSslAesCtx_ctr_mode(SharkSslAesCtx *ctx, U8 ctr[16],
                                                             const U8 *input, U8 *output, U32 len);
#endif
#if SHARKSSL_ENABLE_AES_GCM
/** Initialize
    \ingroup RayCryptoAesGcm

    \param ctx Uninitialized data of size sizeof(SharkSslAesGcmCtx).
    \param key the encryption/decryption key
    \param keyLen 'key' length
*/
SHARKSSL_API SHARKSSL_AES_API_STATUS SharkSslAesGcmCtx_constructor(SharkSslAesGcmCtx *ctx,
                                                                   const U8 *key, U8 keyLen);

#define SharkSslAesGcmCtx_destructor(ctx) sharkssl_clear(ctx, sizeof(SharkSslAesGcmCtx))

/** Encrypt and authenticate one complete message.
    \ingroup RayCryptoAesGcm
    \param ctx initialized context, retained for reuse with distinct nonces.
    \param vect required 12-byte nonce. Never reuse a nonce with the same key.
    \param tagout required 16-byte output tag; overwritten, not accumulated.
    \param auth optional additional authenticated data (AAD). These bytes
    are authenticated but not encrypted; NULL requires authlen=0.
    \param authlen AAD length in bytes.
    \param input plaintext bytes, borrowed for this call.
    \param output writable ciphertext buffer of at least len bytes.
    Exact in-place operation is supported.
    \param len plaintext length in bytes. Keep it below 2^29 when using
    the common native implementation, whose GCM bit-length field is 32 bits.

    Each call starts a separate GCM message. To store a large object in
    chunks, assign a distinct nonce to every chunk under this key and keep
    each chunk's nonce, AAD, and tag for decryption. A previous tag is not
    input to the next call. Authenticate chunk order/identity in the AAD
    when the application must detect rearrangement or substitution.
*/
SHARKSSL_API int   SharkSslAesGcmCtx_encrypt(SharkSslAesGcmCtx *ctx,
                                             const U8 vect[12], U8 tagout[16],
                                             const U8 *auth, U16 authlen,
                                             const U8 *input, U8 *output, U32 len);


/**
 * Decrypt and authenticate one complete message.
 * \ingroup RayCryptoAesGcm
 * \param ctx context initialized by SharkSslAesGcmCtx_constructor.
 * \param vect the same IV as used in SharkSslAesGcmCtx_encrypt.
 * \param tagin required 16-byte expected tag from the matching encryption
 * call. The native and PSA implementations read it without modification.
 * \param auth the same auth as used in SharkSslAesGcmCtx_encrypt or
 * NULL if not used.
 * \param authlen the length of the 'auth' parameter.
 * \param input the data to be decrypted.
 * \param output the decrypted plaintext. This buffer may be the same as input.
 * On authentication failure, its contents are invalid and may be modified.
 * Discard them; in-place decryption may also destroy the ciphertext.
 * \param len ciphertext length in bytes. Keep it below 2^29 with the
 * common native implementation, matching the encryption restriction.
 * \return 0 when the tag matches, nonzero otherwise. Do not access output
 * until this function returns.
 */
SHARKSSL_API int   SharkSslAesGcmCtx_decrypt(SharkSslAesGcmCtx *ctx,
                                             const U8 vect[12], U8 tagin[16],
                                             const U8 *auth, U16 authlen,
                                             U8 *input, U8 *output, U32 len);
#endif
#if SHARKSSL_ENABLE_AES_CCM
/** Initialize
    \ingroup RayCryptoAesCcm

    \param ctx Uninitialized data of size sizeof(SharkSslAesCcmCtx).
    \param key the encryption/decryption key
    \param keyLen 'key' length
    \param tagLen length of the tag (8 or 16 bytes)
*/
SHARKSSL_API SHARKSSL_AES_API_STATUS SharkSslAesCcmCtx_constructor(SharkSslAesCcmCtx *ctx,
                                                                   const U8 *key, U8 keyLen, U8 tagLen);

#define SharkSslAesCcmCtx_destructor(ctx) sharkssl_clear(ctx, sizeof(SharkSslAesCcmCtx))

/** Encrypt
    \ingroup RayCryptoAesCcm
*/
SHARKSSL_API int   SharkSslAesCcmCtx_encrypt(SharkSslAesCcmCtx *ctx,
                                             const U8 vect[12], U8 *tagout,
                                             const U8 *auth, U16 authlen,
                                             const U8 *input, U8 *output, U32 len);


/** Decrypt
    \ingroup RayCryptoAesCcm
*/
SHARKSSL_API int   SharkSslAesCcmCtx_decrypt(SharkSslAesCcmCtx *ctx,
                                             const U8 vect[12], U8 *tagin,
                                             const U8 *auth, U16 authlen,
                                             const U8 *input, U8 *output, U32 len);
#endif
#undef SHARKSSL_AES_API_STATUS
#endif

#ifdef __cplusplus
}
#endif


#endif /* _SharkSslCrypto_h */
