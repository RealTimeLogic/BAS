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
 *   $Id: SharkSslCrypto.h 5149 2022-05-14 21:56:07Z gianluca $
 *
 *   COPYRIGHT:  Real Time Logic LLC, 2010 - 2022
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
#define SHARKSSL_SHA256_HASH_LEN    32
#define SHARKSSL_SHA384_HASH_LEN    48
#define SHARKSSL_SHA512_HASH_LEN    64
#define SHARKSSL_POLY1305_HASH_LEN  16

#define SHARKSSL_MD5_BLOCK_LEN      64
#define SHARKSSL_SHA1_BLOCK_LEN     64
#define SHARKSSL_SHA256_BLOCK_LEN   64
#define SHARKSSL_SHA384_BLOCK_LEN   128
#define SHARKSSL_SHA512_BLOCK_LEN   128

/** to be used as 'hashID' parameter in SharkSslHMACCtx_constructor 
 */
#define SHARKSSL_HASHID_MD5         0x01
#define SHARKSSL_HASHID_SHA1        0x02
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
   U32 total[2];
   U32 state[5];
   U8  buffer[64];
} SharkSslSha1Ctx;
#endif


#if SHARKSSL_USE_SHA_256
/** SHA256
    \ingroup RayCryptoSHA256
*/
typedef struct SharkSslSha256Ctx
{
   U32 total[2];
   U32 state[8];
   U8  buffer[64];
} SharkSslSha256Ctx;
#endif


#if (SHARKSSL_USE_SHA_384 || SHARKSSL_USE_SHA_512)
/** SHA384
    \ingroup RayCryptoSHA384
*/
typedef struct SharkSslSha384Ctx
{
   U32 total[4];
   U64 state[8];
   U8  buffer[128];
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
typedef struct SharkSslHMACCtx
{
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
   U32 state[16];
} SharkSslChaChaCtx;
#endif


#if (SHARKSSL_USE_AES_256 || SHARKSSL_USE_AES_192 || SHARKSSL_USE_AES_128)
/** AES
    \ingroup RayCryptoAES
*/
typedef struct SharkSslAesCtx
{
   #if (SHARKSSL_USE_AES_256 || SHARKSSL_NOPACK)
   U32 key[60];
   #elif SHARKSSL_USE_AES_192
   U32 key[52];
   #else
   U32 key[44];
   #endif
   U16 nr;
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
   U8 M0[16][16];
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
SHARKSSL_API int   sharkssl_entropy(U32);
SHARKSSL_API int   sharkssl_rng(U8*, U16);
SHARKSSL_API int   sharkssl_kmemcmp(const void *a, const void *b, U32 n);


#if SHARKSSL_USE_MD5
/** Initialize
    \ingroup RayCryptoMD5

    \param ctx Uninitialized data of size sizeof(SharkSslMd5Ctx).
*/
SHARKSSL_API void  SharkSslMd5Ctx_constructor(SharkSslMd5Ctx* ctx);

/** append
    \ingroup RayCryptoMD5
*/
SHARKSSL_API void  SharkSslMd5Ctx_append(SharkSslMd5Ctx* ctx, const U8* data, U32 len);

/** finish
    \ingroup RayCryptoMD5
*/
SHARKSSL_API void  SharkSslMd5Ctx_finish(SharkSslMd5Ctx* ctx, U8 digest[SHARKSSL_MD5_HASH_LEN]);

/** md5
    \ingroup RayCryptoMD5
*/
SHARKSSL_API int   sharkssl_md5(const U8*, U16, U8*);
#endif

#if SHARKSSL_USE_SHA1
/** Initialize
    \ingroup RayCryptoSHA1

    \param ctx Uninitialized data of size sizeof(SharkSslSha1Ctx).
*/
SHARKSSL_API void  SharkSslSha1Ctx_constructor(SharkSslSha1Ctx* ctx);

/** append
    \ingroup RayCryptoSHA1
*/
SHARKSSL_API void  SharkSslSha1Ctx_append(SharkSslSha1Ctx* ctx, const U8* data, U32 len);

/** finish
    \ingroup RayCryptoSHA1
*/
SHARKSSL_API void  SharkSslSha1Ctx_finish(SharkSslSha1Ctx*, U8 digest[SHARKSSL_SHA1_HASH_LEN]);

/** sha1
    \ingroup RayCryptoSHA1
*/
SHARKSSL_API int   sharkssl_sha1(const U8*, U16, U8*);
#endif

#if SHARKSSL_USE_SHA_256
/** Initialize
    \ingroup RayCryptoSHA256

    \param ctx Uninitialized data of size sizeof(SharkSslSha256Ctx).
*/
SHARKSSL_API void  SharkSslSha256Ctx_constructor(SharkSslSha256Ctx* ctx);

/** append
    \ingroup RayCryptoSHA256
*/
SHARKSSL_API void  SharkSslSha256Ctx_append(SharkSslSha256Ctx*, const U8* data, U32 len);

/** finish
    \ingroup RayCryptoSHA256
*/
SHARKSSL_API void  SharkSslSha256Ctx_finish(SharkSslSha256Ctx*, U8 digest[SHARKSSL_SHA256_HASH_LEN]);

/** sha256
    \ingroup RayCryptoSHA256
*/
SHARKSSL_API int   sharkssl_sha256(const U8*, U16, U8*);
#endif

#if SHARKSSL_USE_SHA_384
/** Initialize
    \ingroup RayCryptoSHA384

    \param ctx Uninitialized data of size sizeof(SharkSslSha384Ctx).
*/
SHARKSSL_API void  SharkSslSha384Ctx_constructor(SharkSslSha384Ctx* ctx);

/** append
    \ingroup RayCryptoSHA384
*/
SHARKSSL_API void  SharkSslSha384Ctx_append(SharkSslSha384Ctx*, const U8* data, U32 len);

/** finish
    \ingroup RayCryptoSHA384
*/
SHARKSSL_API void  SharkSslSha384Ctx_finish(SharkSslSha384Ctx*, U8 digest[SHARKSSL_SHA384_HASH_LEN]);

/** sha384
    \ingroup RayCryptoSHA384
*/
SHARKSSL_API int   sharkssl_sha384(const U8*, U16, U8*);
#endif

#if SHARKSSL_USE_SHA_512
/** Initialize
    \ingroup RayCryptoSHA512

    \param ctx Uninitialized data of size sizeof(SharkSslSha512Ctx).
*/
SHARKSSL_API void  SharkSslSha512Ctx_constructor(SharkSslSha512Ctx* ctx);

/** append
    \ingroup RayCryptoSHA512
*/
#define SharkSslSha512Ctx_append(ctx, d, l) \
   SharkSslSha384Ctx_append((SharkSslSha384Ctx*)ctx, d, l)

/** finish
    \ingroup RayCryptoSHA512
*/
SHARKSSL_API void  SharkSslSha512Ctx_finish(SharkSslSha512Ctx*, U8 digest[SHARKSSL_SHA512_HASH_LEN]);

/** sha512
    \ingroup RayCryptoSHA512
*/
SHARKSSL_API int   sharkssl_sha512(const U8*, U16, U8*);
#endif

#if (SHARKSSL_USE_SHA_512 || SHARKSSL_USE_SHA_384 || SHARKSSL_USE_SHA_256 || SHARKSSL_USE_SHA1 || SHARKSSL_USE_MD5)
/** Initialize
    \ingroup RayCryptoHMAC

    \param ctx Uninitialized data of size sizeof(SharkSslHMACCtx).
    \param hashID - the hash function to be used, can be one of: 
     SHARKSSL_HASHID_MD5, SHARKSSL_HASHID_SHA1, SHARKSSL_HASHID_SHA256, SHARKSSL_HASHID_SHA384, SHARKSSL_HASHID_SHA512
    \param key pointer to the cryptographic key
    \param keyLen 'key' length
*/
SHARKSSL_API void  SharkSslHMACCtx_constructor(SharkSslHMACCtx* ctx, U8 hashID, const U8* key, U16 keyLen);

/** append
    \ingroup RayCryptoHMAC
*/
SHARKSSL_API void  SharkSslHMACCtx_append(SharkSslHMACCtx*, const U8* data, U32 len);

/** finish
    \ingroup RayCryptoHMAC
*/
SHARKSSL_API void  SharkSslHMACCtx_finish(SharkSslHMACCtx*, U8 *HMAC);

#define SharkSslHMACCtx_destructor(o) memset(o, 0, sizeof(SharkSslHMACCtx))

/** HMAC
    \ingroup RayCryptoHMAC

    \param hashID - the hash function to be used, can be one of: 
     SHARKSSL_HASHID_MD5, SHARKSSL_HASHID_SHA1, SHARKSSL_HASHID_SHA256, SHARKSSL_HASHID_SHA384, SHARKSSL_HASHID_SHA512
    \param data pointer to the data to be hashed
    \param len data length in bytes
    \param key pointer to the cryptographic key
    \param keyLen 'key' length
    \param digest output HMAC
*/
SHARKSSL_API int sharkssl_HMAC(const U8 hashID, const U8 *data, U16 len, const U8 *key, U16 keyLen, U8 *digest);
#endif

U16 sharkssl_getHashLen(U8 hashID);
int sharkssl_hash(U8 *digest, U8 *data, U16 len, U8 hashID);


#if SHARKSSL_USE_POLY1305
/** Initialize
    \ingroup RayCryptoPOLY1305

    \param ctx Uninitialized data of size sizeof(SharkSslPoly1305Ctx).
    \param key 16-byte key + 16-byte nonce
*/
SHARKSSL_API void  SharkSslPoly1305Ctx_constructor(SharkSslPoly1305Ctx *ctx, const U8 key[32]);

#define SharkSslPoly1305Ctx_destructor(o) memset(o, 0, sizeof(SharkSslPoly1305Ctx))

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
SHARKSSL_API int   sharkssl_poly1305(const U8 *data, U16 len, U8 *digest, const U8 key[32]);
#endif

#if SHARKSSL_USE_CHACHA20
/** Initialize
    \ingroup RayCryptoCHACHA20

    \param ctx Uninitialized data of size sizeof(SharkSslChaChaCtx)
    \param key 16- or 32-byte key
    \param keyLen the key length in bytes (either 16 or 32)
*/
SHARKSSL_API void SharkSslChaChaCtx_constructor(SharkSslChaChaCtx *ctx,
                                                const U8 *key, U8 keyLen);

#define SharkSslChaChaCtx_destructor(ctx) memset(ctx, 0, sizeof(SharkSslChaChaCtx))

/** Initialize
    \ingroup RayCryptoCHACHA20

    \param ctx Initialized data of size sizeof(SharkSslChaChaCtx)
    \param IV 12-byte initialization vector (per RFC 7905)
*/
SHARKSSL_API void SharkSslChaChaCtx_setIV(SharkSslChaChaCtx *ctx, const U8 IV[12]);

/** Encrypt or decrypt
    \ingroup RayCryptoCHACHA20
*/
SHARKSSL_API void  SharkSslChaChaCtx_crypt(
   SharkSslChaChaCtx *ctx, const U8 *input, U8 *output, U32 len);
#endif


#if (SHARKSSL_USE_AES_256 || SHARKSSL_USE_AES_192 || SHARKSSL_USE_AES_128)
/** Initialize
    \ingroup RayCryptoAES

    \param ctx Uninitialized data of size sizeof(SharkSslAesCtx)
    \param type encrypt/decrypt
    \param key the encryption/decryption key
    \param keyLen 'key' length
*/
SHARKSSL_API void  SharkSslAesCtx_constructor(SharkSslAesCtx *ctx,
                                              SharkSslAesCtx_Type type,
                                              const U8 *key, U8 keyLen);
#define SharkSslAesCtx_destructor(ctx) memset(ctx, 0, sizeof(SharkSslAesCtx))

#if (!SHARKSSL_DISABLE_AES_ECB_DECRYPT)
/** Decrypt
    \ingroup RayCryptoAES
*/
SHARKSSL_API void  SharkSslAesCtx_decrypt(SharkSslAesCtx *ctx, const U8 input[16], U8 output[16]);
#endif

/** Encrypt
    \ingroup RayCryptoAES
*/
SHARKSSL_API void  SharkSslAesCtx_encrypt(SharkSslAesCtx *ctx, U8 input[16], U8 output[16]);

#if SHARKSSL_ENABLE_AES_CBC
/** Encrypt
    \ingroup RayCryptoAES
*/
SHARKSSL_API void  SharkSslAesCtx_cbc_encrypt(SharkSslAesCtx *ctx, U8 vect[16],
                                              const U8 *input, U8 *output, U16 len);

/** Decrypt
    \ingroup RayCryptoAES
*/
SHARKSSL_API void  SharkSslAesCtx_cbc_decrypt(SharkSslAesCtx *ctx, U8 vect[16],
                                              const U8 *input, U8 *output, U16 len);
#endif
#if SHARKSSL_ENABLE_AES_CTR_MODE

/**
    \ingroup RayCryptoAES
*/
SHARKSSL_API void  SharkSslAesCtx_ctr_mode(SharkSslAesCtx *ctx, U8 ctr[16],
                                           const U8 *input, U8 *output, U16 len);
#endif
#if SHARKSSL_ENABLE_AES_GCM
/** Initialize
    \ingroup RayCryptoAesGcm

    \param ctx Uninitialized data of size sizeof(SharkSslAesGcmCtx).
    \param key the encryption/decryption key
    \param keyLen 'key' length
*/
SHARKSSL_API void  SharkSslAesGcmCtx_constructor(SharkSslAesGcmCtx *ctx,
                                                 const U8 *key, U8 keyLen);

#define SharkSslAesGcmCtx_destructor(ctx) \
   memset(ctx, 0, sizeof(SharkSslAesGcmCtx))

/** Encrypt data or a chunk of a large data set.
    \ingroup RayCryptoAesGcm
    \param ctx context initialized by SharkSslAesGcmCtx_constructor.
    \param vect the initialization vector (IV) is a (public)
    fixed-size input typically created from random data.
    \param tagout a 16 byte xored tag created as the data is
    encrypted. This data must be fed into subsequent calls and
    eventually used for decrypting the data. You may optionally set
    this to a random number for the first call to this function, or
    just leave the 16 byte data-buffer uninitialized.
    \param auth the cipher's optional additional authenticated
    data. The auth parameter makes the encryption stronger, but you
    may set this parameter to NULL.
    \param authlen the length of the 'auth' parameter.
    \param input the data to be encrypted.
    \param output the encrypted output data. This buffer may be the
    same as the input buffer.
    \param len the length of the input block.

    Encrypt data as follows:

    -# Calculate vect (IV), auth.
    -# For large data sets, loop over the data and encrypt data in
       chunks by calling SharkSslAesGcmCtx_encrypt with the data
       chunk, IV and the optional auth calculated in (1).
    -# Send/save IV, auth, and tagout as part of the encrypted
       data chunk. This data, which may be public, will be needed when you
       decrypt the data.
*/
SHARKSSL_API int   SharkSslAesGcmCtx_encrypt(SharkSslAesGcmCtx *ctx,
                                             const U8 vect[12], U8 tagout[16],
                                             const U8 *auth, U16 authlen,
                                             const U8 *input, U8 *output, U16 len);


/** Decrypt data or a chunk of a large data set.
    \ingroup RayCryptoAesGcm
    \param ctx context initialized by SharkSslAesGcmCtx_constructor.
    \param vect the same IV as used in SharkSslAesGcmCtx_encrypt.
    \param tagin the tagout from SharkSslAesGcmCtx_encrypt. This data
    will change for each call to SharkSslAesGcmCtx_decrypt.
    \param auth the same auth as used in SharkSslAesGcmCtx_encrypt or
    NULL if not used.
    \param authlen the length of the 'auth' parameter.
    \param input the data to be decrypted.
    \param output the decrypted (plaintext) output data. This buffer
    may be the same as the input buffer.
    \param len the length of the input block.
*/
SHARKSSL_API int   SharkSslAesGcmCtx_decrypt(SharkSslAesGcmCtx *ctx,
                                             const U8 vect[12], U8 tagin[16],
                                             const U8 *auth, U16 authlen,
                                             U8 *input, U8 *output, U16 len);
#endif
#if SHARKSSL_ENABLE_AES_CCM
/** Initialize
    \ingroup RayCryptoAesCcm

    \param ctx Uninitialized data of size sizeof(SharkSslAesCcmCtx).
    \param key the encryption/decryption key
    \param keyLen 'key' length
    \param tagLen length of the tag (8 or 16 bytes)
*/
SHARKSSL_API void  SharkSslAesCcmCtx_constructor(SharkSslAesCcmCtx *ctx,
                                                 const U8 *key, U8 keyLen, U8 tagLen);

#define SharkSslAesCcmCtx_destructor(ctx) memset(ctx, 0, sizeof(SharkSslAesCcmCtx))

/** Encrypt
    \ingroup RayCryptoAesCcm
*/
SHARKSSL_API int   SharkSslAesCcmCtx_encrypt(SharkSslAesCcmCtx *ctx,
                                             const U8 vect[12], U8 *tagout,
                                             const U8 *auth, U16 authlen,
                                             const U8 *input, U8 *output, U16 len);


/** Decrypt
    \ingroup RayCryptoAesCcm
*/
SHARKSSL_API int   SharkSslAesCcmCtx_decrypt(SharkSslAesCcmCtx *ctx,
                                             const U8 vect[12], U8 *tagin,
                                             const U8 *auth, U16 authlen,
                                             const U8 *input, U8 *output, U16 len);
#endif
#endif

#ifdef __cplusplus
}
#endif


#endif /* _SharkSslCrypto_h */
