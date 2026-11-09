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
 *   $Id: crypto.h 6818 2026-11-08 21:18:02Z gianluca $
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

/**
 * SharkSSL PSA Crypto API 1.4.1/1.5.0 configurable subset.
 * See src/psa/README.md for supported operations, algorithms and limits.
 * Compile the selected src/psa providers explicitly. Define
 * SHARKSSL_PSA_PROFILE_1_4_1=1 for the PSA Crypto API 1.4.1 profile.
 */
#ifndef SHARKSSL_PSA_CRYPTO_H
#define SHARKSSL_PSA_CRYPTO_H

#include <stddef.h>
#include <stdint.h>
#include <limits.h>
#include "TargConfig.h"
#include "SharkSSL_cfg.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef int32_t  psa_status_t;
typedef uint32_t psa_algorithm_t;
typedef uint16_t psa_key_type_t;
typedef uint32_t psa_key_id_t;
typedef uint32_t psa_key_lifetime_t;
typedef uint8_t  psa_key_persistence_t;
typedef uint32_t psa_key_location_t;
typedef uint32_t psa_key_usage_t;
typedef uint16_t psa_key_derivation_step_t;

#define PSA_CRYPTO_API_VERSION_MAJOR  1
#if (defined(SHARKSSL_PSA_PROFILE_1_4_1) && (SHARKSSL_PSA_PROFILE_1_4_1))
#define PSA_CRYPTO_API_VERSION_MINOR  4
#else
#define PSA_CRYPTO_API_VERSION_MINOR  5
#endif

#define PSA_BITS_TO_BYTES(bits)  (((bits) + CHAR_BIT - 1) / CHAR_BIT)

/* SharkSSL-specific capability flags for this staged PSA subset. */
#define SHARKSSL_PSA_HAS_SHA_256            1
#define SHARKSSL_PSA_HAS_SHA_384            SHARKSSL_USE_SHA_384
#define SHARKSSL_PSA_HAS_SHA_512            SHARKSSL_USE_SHA_512
#define SHARKSSL_PSA_HAS_CHACHA20_POLY1305  (SHARKSSL_USE_CHACHA20 && SHARKSSL_USE_POLY1305)
#define SHARKSSL_PSA_HAS_VOLATILE_KEYS      1
#define SHARKSSL_PSA_HAS_PERSISTENT_KEYS    1
#define SHARKSSL_PSA_HAS_KEY_USAGE_QUERY    1
#define SHARKSSL_PSA_HAS_RANDOM             1
#define SHARKSSL_PSA_HAS_HMAC_OPERATIONS    1
#define SHARKSSL_PSA_HAS_HMAC_SHA_384       SHARKSSL_USE_SHA_384
#define SHARKSSL_PSA_HAS_HMAC_SHA_512       SHARKSSL_USE_SHA_512
#define SHARKSSL_PSA_HAS_AEAD_OPERATIONS    1
#define SHARKSSL_PSA_HAS_AEAD_MULTIPART     1
#define SHARKSSL_PSA_HAS_AES_CCM            (SHARKSSL_ENABLE_AES_GCM && (SHARKSSL_USE_AES_128 || SHARKSSL_USE_AES_256))
#if (SHARKSSL_USE_AES_128 || SHARKSSL_USE_AES_256)
#if (SHARKSSL_DISABLE_AES_ECB_DECRYPT)
#define SHARKSSL_PSA_HAS_CIPHER_OPERATIONS  0
#else
#define SHARKSSL_PSA_HAS_CIPHER_OPERATIONS  1
#endif
#else
#define SHARKSSL_PSA_HAS_CIPHER_OPERATIONS  0
#endif
#define SHARKSSL_PSA_HAS_AES_KW        SHARKSSL_PSA_HAS_CIPHER_OPERATIONS
#define SHARKSSL_PSA_HAS_HKDF          1
#define SHARKSSL_PSA_HAS_HKDF_SHA_384  SHARKSSL_USE_SHA_384
#define SHARKSSL_PSA_HAS_HKDF_SHA_512  SHARKSSL_USE_SHA_512
#if (SHARKSSL_USE_ECC && SHARKSSL_ECC_USE_CURVE25519 && SHARKSSL_ENABLE_X25519_API)
#define SHARKSSL_PSA_HAS_X25519  1
#else
#define SHARKSSL_PSA_HAS_X25519  0
#endif
#if (SHARKSSL_USE_ECC && SHARKSSL_ECC_USE_SECP256R1 && SHARKSSL_ENABLE_ECDSA && \
     (SHARKSSL_ECC_VERIFY_POINT || SHARKSSL_TLS_1_3))
#define SHARKSSL_PSA_HAS_ECDSA_P256  1
#else
#define SHARKSSL_PSA_HAS_ECDSA_P256  0
#endif
#if (SHARKSSL_USE_ECC && SHARKSSL_ECC_USE_SECP384R1 && SHARKSSL_ENABLE_ECDSA && \
     (SHARKSSL_ECC_VERIFY_POINT || SHARKSSL_TLS_1_3))
#define SHARKSSL_PSA_HAS_ECDSA_P384  1
#else
#define SHARKSSL_PSA_HAS_ECDSA_P384  0
#endif
#define SHARKSSL_PSA_HAS_ECDSA_VERIFY  (SHARKSSL_PSA_HAS_ECDSA_P256 || SHARKSSL_PSA_HAS_ECDSA_P384)
#define SHARKSSL_PSA_ECDSA_CURVE_SUPPORTED(bits) \
   ((((bits) == 256) && SHARKSSL_PSA_HAS_ECDSA_P256) || \
    (((bits) == 384) && SHARKSSL_PSA_HAS_ECDSA_P384))
#if (SHARKSSL_PSA_HAS_ECDSA_VERIFY && SHARKSSL_ENABLE_ECDSA_API && \
     (!SHARKSSL_ECDSA_ONLY_VERIFY))
#define SHARKSSL_PSA_HAS_ECDSA_SIGN  1
#else
#define SHARKSSL_PSA_HAS_ECDSA_SIGN  0
#endif
#define SHARKSSL_PSA_HAS_RSA_PKCS1V15_VERIFY     SHARKSSL_ENABLE_RSA
#define SHARKSSL_PSA_HAS_RSA_KEY_PAIR            SHARKSSL_ENABLE_RSA
#define SHARKSSL_PSA_HAS_RSA_KEY_GENERATION      (SHARKSSL_ENABLE_RSA && SHARKSSL_ENABLE_RSA_API && SHARKSSL_ENABLE_RSAKEY_CREATE)
#define SHARKSSL_PSA_HAS_RSA_PKCS1V15_SIGN       (SHARKSSL_ENABLE_RSA && SHARKSSL_ENABLE_RSA_API)
#define SHARKSSL_PSA_HAS_RSA_PKCS1V15_ENCRYPT    SHARKSSL_ENABLE_RSA
#define SHARKSSL_PSA_HAS_RSA_PKCS1V15_DECRYPT    (SHARKSSL_ENABLE_RSA && SHARKSSL_ENABLE_RSA_API)
#define SHARKSSL_PSA_HAS_RSA_ASYMMETRIC_ENCRYPT  SHARKSSL_ENABLE_RSA
#define SHARKSSL_PSA_HAS_RSA_ASYMMETRIC_DECRYPT  (SHARKSSL_ENABLE_RSA && SHARKSSL_ENABLE_RSA_API)
#define SHARKSSL_PSA_HAS_RSA_OAEP_ENCRYPT        SHARKSSL_ENABLE_RSA
#define SHARKSSL_PSA_HAS_RSA_OAEP_DECRYPT        (SHARKSSL_ENABLE_RSA && SHARKSSL_ENABLE_RSA_API)
#define SHARKSSL_PSA_HAS_RSA_PSS_VERIFY          SHARKSSL_ENABLE_RSA
#define SHARKSSL_PSA_HAS_RSA_PSS_SIGN            (SHARKSSL_ENABLE_RSA && SHARKSSL_ENABLE_RSA_API)
#define SHARKSSL_PSA_RSA_PRIVATE_MAX_SIZE        ((size_t)1792)
#define SHARKSSL_PSA_RSA_MAX_BITS                3072
#define SHARKSSL_PSA_RSA_PUBLIC_MAX_BITS         4096
#define SHARKSSL_PSA_RSA_PUBLIC_MAX_SIZE         ((size_t)(SHARKSSL_PSA_RSA_PUBLIC_MAX_BITS / 8 + 16))
#define SHARKSSL_PSA_HAS_ECDH_P256               (SHARKSSL_PSA_HAS_ECDSA_P256 && SHARKSSL_PSA_HAS_ECDSA_SIGN)
#define SHARKSSL_PSA_HAS_ECIES_P256              SHARKSSL_PSA_HAS_ECDH_P256
#if (SHARKSSL_USE_ECC && SHARKSSL_ECC_USE_SECP384R1 && SHARKSSL_ENABLE_ECDSA_API && \
     (!SHARKSSL_ECDSA_ONLY_VERIFY) && (SHARKSSL_ECC_VERIFY_POINT || SHARKSSL_TLS_1_3))
#define SHARKSSL_PSA_HAS_ECDH_P384  1
#else
#define SHARKSSL_PSA_HAS_ECDH_P384  0
#endif
#define SHARKSSL_PSA_HAS_SECP_R1_KEYS     (SHARKSSL_PSA_HAS_ECDSA_VERIFY || SHARKSSL_PSA_HAS_ECDH_P384)
#define SHARKSSL_PSA_HAS_SECP_R1_PRIVATE  (SHARKSSL_PSA_HAS_ECDSA_SIGN || SHARKSSL_PSA_HAS_ECDH_P384)
#define SHARKSSL_PSA_SECP_R1_KEY_SUPPORTED(bits) \
   ((((bits) == 256) && SHARKSSL_PSA_HAS_ECDSA_P256) || \
    (((bits) == 384) && (SHARKSSL_PSA_HAS_ECDSA_P384 || SHARKSSL_PSA_HAS_ECDH_P384)))

#define PSA_SUCCESS                      ((psa_status_t)0)
#define PSA_ERROR_GENERIC_ERROR          ((psa_status_t)-132)
#define PSA_ERROR_NOT_PERMITTED          ((psa_status_t)-133)
#define PSA_ERROR_NOT_SUPPORTED          ((psa_status_t)-134)
#define PSA_ERROR_INVALID_ARGUMENT       ((psa_status_t)-135)
#define PSA_ERROR_INVALID_HANDLE         ((psa_status_t)-136)
#define PSA_ERROR_BAD_STATE              ((psa_status_t)-137)
#define PSA_ERROR_BUFFER_TOO_SMALL       ((psa_status_t)-138)
#define PSA_ERROR_ALREADY_EXISTS         ((psa_status_t)-139)
#define PSA_ERROR_DOES_NOT_EXIST         ((psa_status_t)-140)
#define PSA_ERROR_INSUFFICIENT_MEMORY    ((psa_status_t)-141)
#define PSA_ERROR_INSUFFICIENT_STORAGE   ((psa_status_t)-142)
#define PSA_ERROR_INSUFFICIENT_DATA      ((psa_status_t)-143)
#define PSA_ERROR_COMMUNICATION_FAILURE  ((psa_status_t)-145)
#define PSA_ERROR_STORAGE_FAILURE        ((psa_status_t)-146)
#define PSA_ERROR_HARDWARE_FAILURE       ((psa_status_t)-147)
#define PSA_ERROR_INSUFFICIENT_ENTROPY   ((psa_status_t)-148)
#define PSA_ERROR_INVALID_SIGNATURE      ((psa_status_t)-149)
#define PSA_ERROR_INVALID_PADDING        ((psa_status_t)-150)
#define PSA_ERROR_CORRUPTION_DETECTED    ((psa_status_t)-151)
#define PSA_ERROR_DATA_CORRUPT           ((psa_status_t)-152)
#define PSA_ERROR_DATA_INVALID           ((psa_status_t)-153)

#define PSA_ALG_NONE  ((psa_algorithm_t)0x00000000)
/* Standard identifiers are available even when the provider omits an algorithm. */
#define PSA_ALG_ANY_HASH  ((psa_algorithm_t)0x020000ff)
#define PSA_ALG_SHA_1     ((psa_algorithm_t)0x02000005)
#define PSA_ALG_SHA_224   ((psa_algorithm_t)0x02000008)
#define PSA_ALG_SHA_256   ((psa_algorithm_t)0x02000009)
#define PSA_ALG_SHA_384   ((psa_algorithm_t)0x0200000a)
#define PSA_ALG_SHA_512   ((psa_algorithm_t)0x0200000b)
/**
 * Concrete hashes supported by signature and derivation adapters.
 */
#define SHARKSSL_PSA_HASH_SUPPORTED(alg) \
   ((PSA_ALG_SHA_256 == (alg)) || \
    (SHARKSSL_PSA_HAS_SHA_384 && (PSA_ALG_SHA_384 == (alg))) || \
    (SHARKSSL_PSA_HAS_SHA_512 && (PSA_ALG_SHA_512 == (alg))))
#define PSA_ALG_HMAC(hash_alg) ((psa_algorithm_t)(0x03800000 | ((psa_algorithm_t)(hash_alg) & 0xff)))
#define PSA_ALG_HKDF(hash_alg) ((psa_algorithm_t)(0x08000100 | ((psa_algorithm_t)(hash_alg) & 0xff)))
#define PSA_ALG_HKDF_EXTRACT(hash_alg) ((psa_algorithm_t)(0x08000400 | ((psa_algorithm_t)(hash_alg) & 0xff)))
#define PSA_ALG_HKDF_EXPAND(hash_alg) ((psa_algorithm_t)(0x08000500 | ((psa_algorithm_t)(hash_alg) & 0xff)))
#define PSA_ALG_SP800_108_COUNTER_HMAC(hash_alg) ((psa_algorithm_t)(0x08000700 | ((psa_algorithm_t)(hash_alg) & 0xff)))
#define PSA_ALG_TLS12_PRF(hash_alg) ((psa_algorithm_t)(0x08000200 | ((psa_algorithm_t)(hash_alg) & 0xff)))
#define PSA_ALG_TLS12_PSK_TO_MS(hash_alg) ((psa_algorithm_t)(0x08000300 | ((psa_algorithm_t)(hash_alg) & 0xff)))
#define PSA_ALG_PBKDF2_HMAC(hash_alg) ((psa_algorithm_t)(0x08800100 | ((psa_algorithm_t)(hash_alg) & 0xff)))
/* SharkSSL policy extension: PASSWORD/PEPPER roots may authorize SHA-256 and SHA-512. */
#define SHARKSSL_PSA_ALG_PBKDF2_HMAC_ANY_HASH PSA_ALG_PBKDF2_HMAC(PSA_ALG_ANY_HASH)
#define PSA_ALG_IS_KEY_DERIVATION(alg) \
   (0x08000000 == (((psa_algorithm_t)(alg)) & 0x7f000000))
#define PSA_ALG_IS_HKDF(alg) \
   (0x08000100 == (((psa_algorithm_t)(alg)) & 0xffffff00))
#define PSA_ALG_IS_HKDF_EXTRACT(alg) \
   (0x08000400 == (((psa_algorithm_t)(alg)) & 0xffffff00))
#define PSA_ALG_IS_HKDF_EXPAND(alg) \
   (0x08000500 == (((psa_algorithm_t)(alg)) & 0xffffff00))
#define PSA_ALG_IS_ANY_HKDF(alg) \
   (PSA_ALG_IS_HKDF(alg) || PSA_ALG_IS_HKDF_EXTRACT(alg) || \
    PSA_ALG_IS_HKDF_EXPAND(alg))
#define PSA_ALG_IS_PBKDF2_HMAC(alg) \
   (0x08800100 == (((psa_algorithm_t)(alg)) & 0xffffff00))
#define PSA_ALG_IS_SP800_108_COUNTER_HMAC(alg) \
   (0x08000700 == (((psa_algorithm_t)(alg)) & 0xffffff00))
#define PSA_ALG_IS_TLS12_PRF(alg) \
   (0x08000200 == (((psa_algorithm_t)(alg)) & 0xffffff00))
#define PSA_ALG_IS_TLS12_PSK_TO_MS(alg) \
   (0x08000300 == (((psa_algorithm_t)(alg)) & 0xffffff00))
#define PSA_ALG_CHACHA20_POLY1305  ((psa_algorithm_t)0x05100500)
#define PSA_ALG_GCM                ((psa_algorithm_t)0x05500200)
#define PSA_ALG_CCM                ((psa_algorithm_t)0x05500100)
#define PSA_ALG_CTR                ((psa_algorithm_t)0x04c01000)
#define PSA_ALG_ECB_NO_PADDING     ((psa_algorithm_t)0x04404400)
#define PSA_ALG_CBC_NO_PADDING     ((psa_algorithm_t)0x04404000)
#define PSA_ALG_KW                 ((psa_algorithm_t)0x0b400100)
#define PSA_ALG_KWP                ((psa_algorithm_t)0x0bc00200)
#define PSA_ALG_IS_KEY_WRAP(alg) \
   (((alg) == PSA_ALG_KW) || ((alg) == PSA_ALG_KWP))
#define PSA_ALG_IS_CIPHER(alg) \
   (0x04000000 == (((psa_algorithm_t)(alg)) & 0x7f000000))
#define PSA_ALG_ECDH  ((psa_algorithm_t)0x09020000)
#define PSA_ALG_ECIES_SEC1  ((psa_algorithm_t)0x0c000100)
#define PSA_ALG_IS_KEY_ENCAPSULATION(alg) \
   (0x0c000000 == (((psa_algorithm_t)(alg)) & 0x7f000000))
#define PSA_ALG_KEY_AGREEMENT(ka_alg, kdf_alg) \
   ((psa_algorithm_t)((ka_alg) | (kdf_alg)))
#define PSA_ALG_KEY_AGREEMENT_GET_BASE(alg) \
   ((psa_algorithm_t)(((psa_algorithm_t)(alg) & 0xffff0000) | 0x09000000))
#define PSA_ALG_KEY_AGREEMENT_GET_KDF(alg) \
   ((psa_algorithm_t)(((psa_algorithm_t)(alg) & 0xfe00ffff) | 0x08000000))
#define PSA_ALG_ECDSA(hash_alg) ((psa_algorithm_t)(0x06000600 | ((psa_algorithm_t)(hash_alg) & 0xff)))
#define PSA_ALG_ECDSA_ANY  ((psa_algorithm_t)0x06000600)
#define PSA_ALG_DETERMINISTIC_ECDSA(hash_alg) ((psa_algorithm_t)(0x06000700 | ((psa_algorithm_t)(hash_alg) & 0xff)))
#define PSA_ALG_IS_ECDSA(alg) \
   (0x06000600 == (((psa_algorithm_t)(alg)) & 0x7ffffe00))
#define PSA_ALG_IS_RANDOMIZED_ECDSA(alg) \
   (0x06000600 == (((psa_algorithm_t)(alg)) & 0x7fffff00))
#define PSA_ALG_IS_DETERMINISTIC_ECDSA(alg) \
   (0x06000700 == (((psa_algorithm_t)(alg)) & 0x7fffff00))
/**
 * Randomized ECDSA with arbitrary digests and concrete hash-bound ECDSA schemes.
 */
#define SHARKSSL_PSA_ECDSA_SUPPORTED(alg) \
   ((PSA_ALG_ECDSA_ANY == (psa_algorithm_t)(alg)) || \
    (((PSA_ALG_ECDSA(PSA_ALG_GET_HASH(alg)) == (alg)) || \
      (PSA_ALG_DETERMINISTIC_ECDSA(PSA_ALG_GET_HASH(alg)) == (alg))) && \
     SHARKSSL_PSA_HASH_SUPPORTED(PSA_ALG_GET_HASH(alg))))
#define PSA_ALG_RSA_PKCS1V15_SIGN(hash_alg) \
   ((psa_algorithm_t)(0x06000200 | ((psa_algorithm_t)(hash_alg) & 0xff)))
#define PSA_ALG_RSA_PKCS1V15_SIGN_RAW  ((psa_algorithm_t)0x06000200)
#define PSA_ALG_IS_RSA_PKCS1V15_SIGN(alg) \
   (0x06000200 == (((psa_algorithm_t)(alg)) & 0x7fffff00))
/**
 * Concrete PKCS#1 v1.5 signature algorithms implemented by this provider.
 */
#define SHARKSSL_PSA_RSA_PKCS1V15_SUPPORTED(alg) \
   ((PSA_ALG_RSA_PKCS1V15_SIGN_RAW == (psa_algorithm_t)(alg)) || \
    ((PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_GET_HASH(alg)) == (alg)) && \
     SHARKSSL_PSA_HASH_SUPPORTED(PSA_ALG_GET_HASH(alg))))
#define PSA_ALG_RSA_PSS(hash_alg) \
   ((psa_algorithm_t)(0x06000300 | ((psa_algorithm_t)(hash_alg) & 0xff)))
#define PSA_ALG_RSA_PSS_ANY_SALT(hash_alg) \
   ((psa_algorithm_t)(0x06001300 | ((psa_algorithm_t)(hash_alg) & 0xff)))
#define PSA_ALG_IS_RSA_PSS_STANDARD_SALT(alg) \
   (0x06000300 == (((psa_algorithm_t)(alg)) & 0xffffff00))
#define PSA_ALG_IS_RSA_PSS_ANY_SALT(alg) \
   (0x06001300 == (((psa_algorithm_t)(alg)) & 0xffffff00))
#define PSA_ALG_IS_RSA_PSS(alg) \
   (PSA_ALG_IS_RSA_PSS_STANDARD_SALT(alg) || PSA_ALG_IS_RSA_PSS_ANY_SALT(alg))
#define SHARKSSL_PSA_RSA_PSS_SUPPORTED(alg) \
   (((PSA_ALG_RSA_PSS(PSA_ALG_GET_HASH(alg)) == (alg)) || \
     (PSA_ALG_RSA_PSS_ANY_SALT(PSA_ALG_GET_HASH(alg)) == (alg))) && \
    SHARKSSL_PSA_HASH_SUPPORTED(PSA_ALG_GET_HASH(alg)))
#define PSA_ALG_RSA_PKCS1V15_CRYPT  ((psa_algorithm_t)0x07000200)
#define PSA_ALG_RSA_OAEP(hash_alg) \
   ((psa_algorithm_t)(0x07000300 | ((hash_alg) & 0xff)))
#define PSA_ALG_IS_RSA_OAEP(alg) \
   (0x07000300 == (((psa_algorithm_t)(alg)) & 0xffffff00))
#define PSA_ALG_IS_ASYMMETRIC_ENCRYPTION(alg) \
   (0x07000000 == (((psa_algorithm_t)(alg)) & 0x7f000000))
#define SHARKSSL_PSA_RSA_OAEP_SUPPORTED(alg) \
   (PSA_ALG_IS_RSA_OAEP(alg) && SHARKSSL_PSA_HASH_SUPPORTED(PSA_ALG_GET_HASH(alg)))
/**
 * Concrete RSA encryption algorithms implemented by this provider.
 */
#define SHARKSSL_PSA_RSA_ASYMMETRIC_SUPPORTED(alg) \
   ((PSA_ALG_RSA_PKCS1V15_CRYPT == (psa_algorithm_t)(alg)) || SHARKSSL_PSA_RSA_OAEP_SUPPORTED(alg))
#define PSA_ASYMMETRIC_ENCRYPT_OUTPUT_SIZE(key_type, key_bits, alg) \
   ((SHARKSSL_PSA_HAS_RSA_ASYMMETRIC_ENCRYPT && PSA_KEY_TYPE_IS_RSA(key_type) && \
     (((key_bits) == 2048) || ((key_bits) == 3072)) && \
     SHARKSSL_PSA_RSA_ASYMMETRIC_SUPPORTED(alg)) ? ((size_t)(key_bits) / 8) : 0)
#define PSA_ASYMMETRIC_DECRYPT_OUTPUT_SIZE(key_type, key_bits, alg) \
   ((SHARKSSL_PSA_HAS_RSA_ASYMMETRIC_DECRYPT && (PSA_KEY_TYPE_RSA_KEY_PAIR == (key_type)) && \
     PSA_ASYMMETRIC_ENCRYPT_OUTPUT_SIZE(key_type, key_bits, alg)) ? \
    ((PSA_ALG_RSA_PKCS1V15_CRYPT == (alg)) ? ((size_t)(key_bits) / 8 - 11) : \
     ((size_t)(key_bits) / 8 - 2 * PSA_HASH_LENGTH(PSA_ALG_GET_HASH(alg)) - 2)) : 0)
#define PSA_ASYMMETRIC_ENCRYPT_OUTPUT_MAX_SIZE  (SHARKSSL_PSA_HAS_RSA_ASYMMETRIC_ENCRYPT ? 384 : 0)
#define PSA_ASYMMETRIC_DECRYPT_OUTPUT_MAX_SIZE  (SHARKSSL_PSA_HAS_RSA_ASYMMETRIC_DECRYPT ? 373 : 0)
#define PSA_ALG_IS_HASH_AND_SIGN(alg) \
   ((PSA_ALG_IS_ECDSA(alg) || PSA_ALG_IS_RSA_PSS(alg) || PSA_ALG_IS_RSA_PKCS1V15_SIGN(alg)) && ((alg) & 0xff))
#define PSA_ALG_IS_SIGN_MESSAGE(alg) \
   (PSA_ALG_IS_HASH_AND_SIGN(alg) && ((alg) & 0xff))
/**
 * None of the signature algorithms supported by this provider has a context.
 */
#define PSA_ALG_SIGN_SUPPORTS_CONTEXT(alg) 0
#define PSA_ALG_IS_SIGN_HASH(alg) \
   (PSA_ALG_IS_ECDSA(alg) || PSA_ALG_IS_RSA_PKCS1V15_SIGN(alg) || PSA_ALG_IS_RSA_PSS(alg))
#define PSA_ALG_IS_KEY_AGREEMENT(alg) \
   (0x09000000 == (((psa_algorithm_t)(alg)) & 0x7f000000))
#define PSA_ALG_IS_ECDH(alg) \
   (0x09020000 == (((psa_algorithm_t)(alg)) & 0x7fff0000))
#define PSA_ALG_IS_STANDALONE_KEY_AGREEMENT(alg) \
   (0x09000000 == (((psa_algorithm_t)(alg)) & 0x7f00ffff))
#define PSA_ALG_IS_RAW_KEY_AGREEMENT(alg) PSA_ALG_IS_STANDALONE_KEY_AGREEMENT(alg)
#define PSA_ALG_AEAD_WITH_SHORTENED_TAG(aead_alg, tag_length) \
   (((psa_algorithm_t)(aead_alg) & ~((psa_algorithm_t)0x003f8000)) | \
    (((psa_algorithm_t)(tag_length) & 0x3f) << 16))
#define PSA_ALG_AEAD_WITH_AT_LEAST_THIS_LENGTH_TAG(aead_alg, min_tag_length) \
   (PSA_ALG_AEAD_WITH_SHORTENED_TAG((aead_alg), (min_tag_length)) | ((psa_algorithm_t)0x00008000))
#define PSA_ALG_AEAD_WITH_DEFAULT_LENGTH_TAG(aead_alg) \
   ((((psa_algorithm_t)(aead_alg) & ~((psa_algorithm_t)0x003f8000)) == \
     ((psa_algorithm_t)PSA_ALG_GCM & ~((psa_algorithm_t)0x003f8000))) ? \
    PSA_ALG_GCM : \
    ((((psa_algorithm_t)(aead_alg) & ~((psa_algorithm_t)0x003f8000)) == \
      ((psa_algorithm_t)PSA_ALG_CCM & ~((psa_algorithm_t)0x003f8000))) ? \
     PSA_ALG_CCM : \
     ((((psa_algorithm_t)(aead_alg) & ~((psa_algorithm_t)0x003f8000)) == \
       ((psa_algorithm_t)PSA_ALG_CHACHA20_POLY1305 & ~((psa_algorithm_t)0x003f8000))) ? \
      PSA_ALG_CHACHA20_POLY1305 : PSA_ALG_NONE)))
#define PSA_ALG_TRUNCATED_MAC(mac_alg, mac_length) \
   (((psa_algorithm_t)(mac_alg) & ~((psa_algorithm_t)0x003f8000)) | \
    (((psa_algorithm_t)(mac_length) & 0x3f) << 16))
#define PSA_ALG_AT_LEAST_THIS_LENGTH_MAC(mac_alg, min_mac_length) \
   (PSA_ALG_TRUNCATED_MAC((mac_alg), (min_mac_length)) | ((psa_algorithm_t)0x00008000))
#define PSA_ALG_FULL_LENGTH_MAC(mac_alg) \
   ((psa_algorithm_t)(mac_alg) & ~((psa_algorithm_t)0x003f8000))
#define PSA_ALG_IS_HMAC(alg) \
   (0x03800000 == (((psa_algorithm_t)(alg)) & 0x7fc07f00))
#define PSA_ALG_IS_MAC(alg) \
   (0x03000000 == (((psa_algorithm_t)(alg)) & 0x7f000000))
#define PSA_ALG_IS_AEAD(alg) \
   (0x05000000 == (((psa_algorithm_t)(alg)) & 0x7f000000))
#define PSA_ALG_IS_AEAD_ON_BLOCK_CIPHER(alg) \
   (0x05400000 == (((psa_algorithm_t)(alg)) & 0x7f400000))
#define PSA_ALG_IS_WILDCARD(alg) \
   ((0x03008000 == (((psa_algorithm_t)(alg)) & 0x7f008000)) || \
    (0x05008000 == (((psa_algorithm_t)(alg)) & 0x7f008000)) || \
    (SHARKSSL_PSA_ALG_PBKDF2_HMAC_ANY_HASH == (psa_algorithm_t)(alg)) || \
    (PSA_ALG_IS_HASH_AND_SIGN(alg) && (0xff == ((psa_algorithm_t)(alg) & 0xff))))
/**
 * HMAC, HKDF, TLS 1.2 PRF, TLS 1.2 PSK-to-MS and signatures are hash composites. A bare hash
 * returns NONE, as required by the PSA macro contract.
 */
#define PSA_ALG_GET_HASH(alg) \
   (((PSA_ALG_IS_HMAC(alg) || PSA_ALG_IS_RSA_OAEP(alg) || PSA_ALG_IS_ANY_HKDF(alg) || \
      PSA_ALG_IS_TLS12_PRF(alg) || \
      PSA_ALG_IS_TLS12_PSK_TO_MS(alg) || \
      PSA_ALG_IS_PBKDF2_HMAC(alg) || PSA_ALG_IS_SP800_108_COUNTER_HMAC(alg) || \
      (PSA_ALG_IS_KEY_AGREEMENT(alg) && \
       (!PSA_ALG_IS_STANDALONE_KEY_AGREEMENT(alg))) || \
      PSA_ALG_IS_SIGN_HASH(alg)) && \
     ((((psa_algorithm_t)(alg)) & 0xff) != 0)) ? \
    ((psa_algorithm_t)(0x02000000 | (((psa_algorithm_t)(alg)) & 0xff))) : \
    PSA_ALG_NONE)
#define PSA_MAC_LENGTH(key_type, key_bits, alg) \
   (((key_type) == PSA_KEY_TYPE_HMAC) && ((key_bits) >= 8) && \
    ((key_bits) <= 512) && (((key_bits) & 7) == 0) && \
    ((PSA_ALG_FULL_LENGTH_MAC(alg) == PSA_ALG_HMAC(PSA_ALG_SHA_256)) || \
     (SHARKSSL_PSA_HAS_HMAC_SHA_384 && \
      (PSA_ALG_FULL_LENGTH_MAC(alg) == PSA_ALG_HMAC(PSA_ALG_SHA_384))) || \
     (SHARKSSL_PSA_HAS_HMAC_SHA_512 && \
      (PSA_ALG_FULL_LENGTH_MAC(alg) == PSA_ALG_HMAC(PSA_ALG_SHA_512)))) && \
    (((psa_algorithm_t)(alg) & 0x00008000) == 0) && \
    ((((psa_algorithm_t)(alg) >> 16) & 0x3f) <= \
     PSA_HASH_LENGTH(PSA_ALG_GET_HASH(alg))) && \
    (((((psa_algorithm_t)(alg) >> 16) & 0x3f) == 0) || \
     ((((psa_algorithm_t)(alg) >> 16) & 0x3f) >= 4)) ? \
    ((((psa_algorithm_t)(alg) >> 16) & 0x3f) ? \
     (size_t)(((psa_algorithm_t)(alg) >> 16) & 0x3f) : \
     PSA_HASH_LENGTH(PSA_ALG_GET_HASH(alg))) : (size_t)0)
#define PSA_MAC_MAX_SIZE  ((size_t)(SHARKSSL_PSA_HAS_HMAC_SHA_512 ? 64 : (SHARKSSL_PSA_HAS_HMAC_SHA_384 ? 48 : 32)))
#define PSA_ALG_IS_HASH(alg) (0x02000000 == (((psa_algorithm_t)(alg)) & 0x7f000000))
#define PSA_HASH_LENGTH(alg) \
   (((PSA_ALG_SHA_256 == (alg)) || \
     (PSA_ALG_HMAC(PSA_ALG_SHA_256) == PSA_ALG_FULL_LENGTH_MAC(alg))) ? \
    ((size_t)32) : \
    (((PSA_ALG_SHA_384 == (alg)) || \
      (PSA_ALG_HMAC(PSA_ALG_SHA_384) == PSA_ALG_FULL_LENGTH_MAC(alg))) ? \
     ((size_t)48) : \
     (((PSA_ALG_SHA_512 == (alg)) || \
       (PSA_ALG_HMAC(PSA_ALG_SHA_512) == PSA_ALG_FULL_LENGTH_MAC(alg))) ? \
      ((size_t)64) : ((size_t)0))))
#define PSA_HASH_BLOCK_LENGTH(alg) \
   ((PSA_ALG_SHA_256 == (alg)) ? ((size_t)64) : \
    (((PSA_ALG_SHA_384 == (alg)) || (PSA_ALG_SHA_512 == (alg))) ? ((size_t)128) : ((size_t)0)))
#define PSA_HASH_MAX_SIZE  ((size_t)(SHARKSSL_PSA_HAS_SHA_512 ? 64 : (SHARKSSL_PSA_HAS_SHA_384 ? 48 : 32)))
#define PSA_HASH_SUSPEND_ALGORITHM_FIELD_LENGTH  ((size_t)sizeof(psa_algorithm_t))
#define PSA_HASH_SUSPEND_INPUT_LENGTH_FIELD_LENGTH(alg) \
   ((PSA_ALG_SHA_256 == (alg)) ? ((size_t)sizeof(uint64_t)) : \
    (((PSA_ALG_SHA_384 == (alg)) || (PSA_ALG_SHA_512 == (alg))) ? \
     ((size_t)(2 * sizeof(uint64_t))) : ((size_t)0)))
#define PSA_HASH_SUSPEND_HASH_STATE_FIELD_LENGTH(alg) \
   ((PSA_ALG_SHA_256 == (alg)) ? PSA_HASH_LENGTH(PSA_ALG_SHA_256) : \
    (((PSA_ALG_SHA_384 == (alg)) || (PSA_ALG_SHA_512 == (alg))) ? \
     PSA_HASH_LENGTH(PSA_ALG_SHA_512) : ((size_t)0)))
#define PSA_HASH_SUSPEND_OUTPUT_SIZE(alg) \
   (SHARKSSL_PSA_HASH_SUPPORTED(alg) ? \
    (PSA_HASH_SUSPEND_ALGORITHM_FIELD_LENGTH + \
     PSA_HASH_SUSPEND_INPUT_LENGTH_FIELD_LENGTH(alg) + \
     PSA_HASH_SUSPEND_HASH_STATE_FIELD_LENGTH(alg) + \
     PSA_HASH_BLOCK_LENGTH(alg) - 1) : ((size_t)0))
#define PSA_HASH_SUSPEND_OUTPUT_MAX_SIZE \
   (SHARKSSL_PSA_HAS_SHA_512 ? PSA_HASH_SUSPEND_OUTPUT_SIZE(PSA_ALG_SHA_512) : \
    (SHARKSSL_PSA_HAS_SHA_384 ? PSA_HASH_SUSPEND_OUTPUT_SIZE(PSA_ALG_SHA_384) : \
     PSA_HASH_SUSPEND_OUTPUT_SIZE(PSA_ALG_SHA_256)))

#define PSA_KEY_TYPE_NONE            ((psa_key_type_t)0x0000)
#define PSA_KEY_TYPE_RAW_DATA        ((psa_key_type_t)0x1001)
#define PSA_KEY_TYPE_HMAC            ((psa_key_type_t)0x1100)
#define PSA_KEY_TYPE_DERIVE          ((psa_key_type_t)0x1200)
#define PSA_KEY_TYPE_PASSWORD        ((psa_key_type_t)0x1203)
#define PSA_KEY_TYPE_PASSWORD_HASH   ((psa_key_type_t)0x1205)
#define PSA_KEY_TYPE_PEPPER          ((psa_key_type_t)0x1206)
#define PSA_KEY_TYPE_RSA_PUBLIC_KEY  ((psa_key_type_t)0x4001)
#define PSA_KEY_TYPE_RSA_KEY_PAIR    ((psa_key_type_t)0x7001)
#define PSA_KEY_TYPE_IS_RSA(type) \
   (((type) == PSA_KEY_TYPE_RSA_PUBLIC_KEY) || ((type) == PSA_KEY_TYPE_RSA_KEY_PAIR))
#define PSA_KEY_TYPE_CHACHA20  ((psa_key_type_t)0x2004)
#define PSA_KEY_TYPE_AES       ((psa_key_type_t)0x2400)
typedef uint8_t psa_ecc_family_t;
#define PSA_ECC_FAMILY_SECP_R1          ((psa_ecc_family_t)0x12)
#define PSA_ECC_FAMILY_SECP_K1          ((psa_ecc_family_t)0x17)
#define PSA_ECC_FAMILY_BRAINPOOL_P_R1   ((psa_ecc_family_t)0x30)
#define PSA_ECC_FAMILY_MONTGOMERY       ((psa_ecc_family_t)0x41)
#define PSA_ECC_FAMILY_TWISTED_EDWARDS  ((psa_ecc_family_t)0x42)
#define PSA_KEY_TYPE_ECC_KEY_PAIR(family) ((psa_key_type_t)(0x7100 | ((psa_key_type_t)(family) & 0x007f)))
#define PSA_KEY_TYPE_ECC_PUBLIC_KEY(family) ((psa_key_type_t)(0x4100 | ((psa_key_type_t)(family) & 0x007f)))
#define PSA_KEY_TYPE_PUBLIC_KEY_OF_KEY_PAIR(type) \
   ((psa_key_type_t)((psa_key_type_t)(type) & (psa_key_type_t)0xcfff))
#define PSA_KEY_TYPE_KEY_PAIR_OF_PUBLIC_KEY(type) \
   ((psa_key_type_t)((psa_key_type_t)(type) | (psa_key_type_t)0x3000))
#define PSA_KEY_TYPE_IS_ECC_KEY_PAIR(type) \
   (0x7100 == (((psa_key_type_t)(type)) & 0xff80))
#define PSA_KEY_TYPE_IS_ECC_PUBLIC_KEY(type) \
   (0x4100 == (((psa_key_type_t)(type)) & 0xff80))
#define PSA_KEY_TYPE_IS_ECC(type) \
   (PSA_KEY_TYPE_IS_ECC_KEY_PAIR(type) || PSA_KEY_TYPE_IS_ECC_PUBLIC_KEY(type))
#define PSA_KEY_TYPE_ECC_GET_FAMILY(type) \
   ((psa_ecc_family_t)((psa_key_type_t)(type) & 0x007f))

#define PSA_KEY_ID_NULL                            ((psa_key_id_t)0x00000000)
#define PSA_KEY_ID_USER_MIN                        ((psa_key_id_t)0x00000001)
#define PSA_KEY_ID_USER_MAX                        ((psa_key_id_t)0x3fffffff)
#define PSA_KEY_ID_VENDOR_MIN                      ((psa_key_id_t)0x40000000)
#define PSA_KEY_ID_VENDOR_MAX                      ((psa_key_id_t)0x7fffffff)
#define PSA_KEY_LIFETIME_VOLATILE                  ((psa_key_lifetime_t)0x00000000)
#define PSA_KEY_LIFETIME_PERSISTENT                ((psa_key_lifetime_t)0x00000001)
#define PSA_KEY_PERSISTENCE_VOLATILE               ((psa_key_persistence_t)0x00)
#define PSA_KEY_PERSISTENCE_DEFAULT                ((psa_key_persistence_t)0x01)
#define PSA_KEY_PERSISTENCE_READ_ONLY              ((psa_key_persistence_t)0xff)
#define PSA_KEY_LOCATION_LOCAL_STORAGE             ((psa_key_location_t)0x000000)
#define PSA_KEY_LOCATION_PRIMARY_SECURE_ELEMENT    ((psa_key_location_t)0x000001)
#define PSA_KEY_USAGE_EXPORT                       ((psa_key_usage_t)0x00000001)
#define PSA_KEY_USAGE_COPY                         ((psa_key_usage_t)0x00000002)
#define PSA_KEY_USAGE_CACHE                        ((psa_key_usage_t)0x00000004)
#define PSA_KEY_USAGE_DERIVE_PUBLIC                ((psa_key_usage_t)0x00000080)
#define PSA_KEY_USAGE_ENCRYPT                      ((psa_key_usage_t)0x00000100)
#define PSA_KEY_USAGE_DECRYPT                      ((psa_key_usage_t)0x00000200)
#define PSA_KEY_USAGE_SIGN_MESSAGE                 ((psa_key_usage_t)0x00000400)
#define PSA_KEY_USAGE_VERIFY_MESSAGE               ((psa_key_usage_t)0x00000800)
#define PSA_KEY_USAGE_SIGN_HASH                    ((psa_key_usage_t)0x00001000)
#define PSA_KEY_USAGE_VERIFY_HASH                  ((psa_key_usage_t)0x00002000)
#define PSA_KEY_USAGE_DERIVE                       ((psa_key_usage_t)0x00004000)
#define PSA_KEY_USAGE_VERIFY_DERIVATION            ((psa_key_usage_t)0x00008000)
#define PSA_KEY_USAGE_WRAP                         ((psa_key_usage_t)0x00010000)
#define PSA_KEY_USAGE_UNWRAP                       ((psa_key_usage_t)0x00020000)
#define PSA_KEY_LIFETIME_GET_PERSISTENCE(lifetime) ((psa_key_persistence_t)((psa_key_lifetime_t)(lifetime) & 0xff))
#define PSA_KEY_LIFETIME_GET_LOCATION(lifetime)    ((psa_key_location_t)((psa_key_lifetime_t)(lifetime) >> 8))
#define PSA_KEY_LIFETIME_IS_VOLATILE(lifetime)     (PSA_KEY_LIFETIME_GET_PERSISTENCE(lifetime) == PSA_KEY_PERSISTENCE_VOLATILE)
#define PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION(persistence, location) \
   ((psa_key_lifetime_t)(((psa_key_location_t)(location) << 8) | (psa_key_persistence_t)(persistence)))


/**
 * Attributes are value objects; setters do not allocate resources.
 * Volatile keys have an unspecified identifier in queried attributes.
 */
typedef struct psa_key_attributes_s
{
   psa_key_lifetime_t lifetime;
   psa_key_id_t id;
   psa_key_type_t type;
   size_t bits;
   psa_key_usage_t usage;
   psa_algorithm_t alg;
} psa_key_attributes_t;

#ifdef __cplusplus
#define PSA_KEY_ATTRIBUTES_INIT  psa_key_attributes_t()
#else
#define PSA_KEY_ATTRIBUTES_INIT  {0}
#endif

typedef struct psa_custom_key_parameters_t
{
   uint32_t flags;
} psa_custom_key_parameters_t;

#define PSA_CUSTOM_KEY_PARAMETERS_INIT  {0}

/* AES-KW and AES-KWP wrap exportable keys with AES-128 or AES-256 KEKs. */
#define SHARKSSL_PSA_KW_SEMIBLOCK_LENGTH          8
#define SHARKSSL_PSA_KW_SEMIBLOCK_BITS            (SHARKSSL_PSA_KW_SEMIBLOCK_LENGTH * CHAR_BIT)
#define SHARKSSL_PSA_KW_MIN_KEY_LENGTH            (2 * SHARKSSL_PSA_KW_SEMIBLOCK_LENGTH)
#define SHARKSSL_PSA_KWP_MAX_KEY_LENGTH           64
#define SHARKSSL_PSA_WRAP_ECC_MAX_PRIVATE_LENGTH  48
#define SHARKSSL_PSA_WRAP_ECC_MAX_PUBLIC_LENGTH   97
#define SHARKSSL_PSA_WRAP_RSA_KEY_LENGTH(type, bits) \
   ((SHARKSSL_PSA_HAS_RSA_KEY_PAIR && SHARKSSL_PSA_RSA_EXPORT_SIZE(type, bits)) ? \
    PSA_EXPORT_KEY_OUTPUT_SIZE(type, bits) : (size_t)0)
#define SHARKSSL_PSA_AES_WRAP_KEY_SUPPORTED(type, bits) \
   (((type) == PSA_KEY_TYPE_AES) && \
    (((((bits) == 128) && SHARKSSL_USE_AES_128) || \
      (((bits) == 256) && SHARKSSL_USE_AES_256))))
#define SHARKSSL_PSA_WRAP_KEY_MATERIAL_SUPPORTED(type, bits) \
   (SHARKSSL_PSA_AES_WRAP_KEY_SUPPORTED(type, bits) || \
    (((((type) == PSA_KEY_TYPE_RAW_DATA) || ((type) == PSA_KEY_TYPE_HMAC) || \
       ((type) == PSA_KEY_TYPE_DERIVE) || ((type) == PSA_KEY_TYPE_PASSWORD) || \
       ((type) == PSA_KEY_TYPE_PASSWORD_HASH) || ((type) == PSA_KEY_TYPE_PEPPER)) && \
      ((bits) >= CHAR_BIT) && ((bits) <= (SHARKSSL_PSA_KWP_MAX_KEY_LENGTH * CHAR_BIT)) && \
      (((bits) % CHAR_BIT) == 0))) || \
    (((type) == PSA_KEY_TYPE_CHACHA20) && SHARKSSL_PSA_HAS_CHACHA20_POLY1305 && \
     ((bits) == 256)))
/* SEC1 public points have one prefix byte and two coordinate values. */
#define SHARKSSL_PSA_WRAP_ECC_KEY_LENGTH(type, bits) \
   (((((type) == PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_MONTGOMERY)) || \
      ((type) == PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_MONTGOMERY))) && \
     ((bits) == 255) && SHARKSSL_PSA_HAS_X25519) ? \
    (size_t)(((bits) + CHAR_BIT - 1) / CHAR_BIT) : \
    ((((type) == PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1)) && \
      SHARKSSL_PSA_HAS_SECP_R1_PRIVATE && SHARKSSL_PSA_SECP_R1_KEY_SUPPORTED(bits)) ? \
     (size_t)((bits) / CHAR_BIT) : \
     ((((type) == PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1)) && \
       SHARKSSL_PSA_SECP_R1_KEY_SUPPORTED(bits)) ? \
      (size_t)(1 + 2 * ((bits) / CHAR_BIT)) : (size_t)0)))
#define SHARKSSL_PSA_WRAP_KEY_MATERIAL_LENGTH(type, bits) \
   (SHARKSSL_PSA_WRAP_KEY_MATERIAL_SUPPORTED(type, bits) ? \
    (size_t)((bits) / CHAR_BIT) : \
    (SHARKSSL_PSA_WRAP_ECC_KEY_LENGTH(type, bits) ? \
     SHARKSSL_PSA_WRAP_ECC_KEY_LENGTH(type, bits) : \
     SHARKSSL_PSA_WRAP_RSA_KEY_LENGTH(type, bits)))
#define SHARKSSL_PSA_KW_KEY_SUPPORTED(type, bits) \
   ((!PSA_KEY_TYPE_IS_RSA(type)) && \
    (SHARKSSL_PSA_WRAP_KEY_MATERIAL_LENGTH(type, bits) >= SHARKSSL_PSA_KW_MIN_KEY_LENGTH) && \
    ((SHARKSSL_PSA_WRAP_KEY_MATERIAL_LENGTH(type, bits) % SHARKSSL_PSA_KW_SEMIBLOCK_LENGTH) == 0))
#define SHARKSSL_PSA_KWP_KEY_SUPPORTED(type, bits) \
   (SHARKSSL_PSA_WRAP_KEY_MATERIAL_LENGTH(type, bits) != 0)
#define PSA_WRAP_KEY_OUTPUT_SIZE(wrap_key_type, alg, key_type, key_bits) \
   ((SHARKSSL_PSA_HAS_AES_KW && ((wrap_key_type) == PSA_KEY_TYPE_AES) && \
     (((((alg) == PSA_ALG_KW) && SHARKSSL_PSA_KW_KEY_SUPPORTED(key_type, key_bits)) || \
       (((alg) == PSA_ALG_KWP) && SHARKSSL_PSA_KWP_KEY_SUPPORTED(key_type, key_bits))))) ? \
    ((alg) == PSA_ALG_KW ? \
     (SHARKSSL_PSA_WRAP_KEY_MATERIAL_LENGTH(key_type, key_bits) + SHARKSSL_PSA_KW_SEMIBLOCK_LENGTH) : \
     ((SHARKSSL_PSA_WRAP_KEY_MATERIAL_LENGTH(key_type, key_bits) + SHARKSSL_PSA_KW_SEMIBLOCK_LENGTH - 1) / \
      SHARKSSL_PSA_KW_SEMIBLOCK_LENGTH + 1) * SHARKSSL_PSA_KW_SEMIBLOCK_LENGTH) : (size_t)0)
#define PSA_WRAP_KEY_PAIR_MAX_SIZE \
   ((SHARKSSL_PSA_HAS_AES_KW && SHARKSSL_PSA_HAS_RSA_KEY_PAIR) ? \
    (size_t)(SHARKSSL_PSA_RSA_PRIVATE_MAX_SIZE + SHARKSSL_PSA_KW_SEMIBLOCK_LENGTH) : \
    ((SHARKSSL_PSA_HAS_AES_KW && (SHARKSSL_PSA_HAS_X25519 || SHARKSSL_PSA_HAS_SECP_R1_PRIVATE)) ? \
     (size_t)(SHARKSSL_PSA_WRAP_ECC_MAX_PRIVATE_LENGTH + SHARKSSL_PSA_KW_SEMIBLOCK_LENGTH) : (size_t)0))

/**
 * RAW_DATA, HMAC and DERIVE keys in this provider have lengths of 1..64 bytes.
 * AES-GCM keys have lengths of 16 or 32 bytes. X25519 keys are 255 bits
 * and have a 32-byte binary representation. P-256 private keys are 32-byte
 * big-endian scalars; public keys are 65-byte uncompressed SEC1 points.
 * P-384 ECDH keys use 48-byte scalars and 97-byte uncompressed SEC1 points.
 * RSA public keys are canonical PKCS#1 DER, with 2048/3072/4096-bit moduli
 * and odd public exponents from 3 through UINT32_MAX. RSA key-pair export
 * uses PKCS#1 RSAPrivateKey DER with balanced, full-width factors.
 */
#define SHARKSSL_PSA_RSA_EXPORT_SIZE(type, bits) \
   (PSA_KEY_TYPE_IS_RSA(type) && (((bits) == 2048) || ((bits) == 3072) || \
    (((type) == PSA_KEY_TYPE_RSA_PUBLIC_KEY) && ((bits) == SHARKSSL_PSA_RSA_PUBLIC_MAX_BITS))) ? \
    ((size_t)(bits) / 8 + 16) : (size_t)0)
#define PSA_EXPORT_KEY_OUTPUT_SIZE(type, bits) \
   ((((type) == PSA_KEY_TYPE_RSA_KEY_PAIR) && SHARKSSL_PSA_RSA_EXPORT_SIZE(type, bits)) ? \
    ((size_t)(bits) / 16 * 9 + 64) : \
   (SHARKSSL_PSA_RSA_EXPORT_SIZE(type, bits) ? SHARKSSL_PSA_RSA_EXPORT_SIZE(type, bits) : \
   ((((type) == PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1)) && \
     (((bits) == 256) || ((bits) == 384))) ? ((size_t)(1 + (bits) / 4)) : \
    ((((type) == PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1)) && \
      (((bits) == 256) || ((bits) == 384))) ? ((size_t)(1 + (bits) / 4)) : \
    ((((((type) == PSA_KEY_TYPE_RAW_DATA) || ((type) == PSA_KEY_TYPE_HMAC) || \
       ((type) == PSA_KEY_TYPE_DERIVE) || \
       ((type) == PSA_KEY_TYPE_PASSWORD) || \
       ((type) == PSA_KEY_TYPE_PASSWORD_HASH) || \
       ((type) == PSA_KEY_TYPE_PEPPER)) && \
      ((bits) >= 8) && ((bits) <= 512) && (((bits) & 7) == 0)) || \
     (((type) == PSA_KEY_TYPE_AES) && (((bits) == 128) || ((bits) == 256))) || \
     (((type) == PSA_KEY_TYPE_CHACHA20) && ((bits) == 256)) || \
     ((((type) == PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_MONTGOMERY)) || \
       ((type) == PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_MONTGOMERY))) && \
      ((bits) == 255))) ? \
     (((size_t)(bits) + 7) / 8) : ((size_t)0))))))

#define PSA_EXPORT_PUBLIC_KEY_OUTPUT_SIZE(type, bits) \
   (SHARKSSL_PSA_RSA_EXPORT_SIZE(type, bits) ? SHARKSSL_PSA_RSA_EXPORT_SIZE(type, bits) : \
   (((((type) == PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1)) || \
      ((type) == PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1))) && \
     (((bits) == 256) || ((bits) == 384))) ? ((size_t)(1 + (bits) / 4)) : \
    (((((type) == PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_MONTGOMERY)) || \
      ((type) == PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_MONTGOMERY))) && \
      ((bits) == 255)) ? ((size_t)32) : ((size_t)0))))
#define PSA_EXPORT_PUBLIC_KEY_MAX_SIZE      SHARKSSL_PSA_RSA_PUBLIC_MAX_SIZE
#define PSA_EXPORT_KEY_PAIR_MAX_SIZE        SHARKSSL_PSA_RSA_PRIVATE_MAX_SIZE
#define PSA_EXPORT_ASYMMETRIC_KEY_MAX_SIZE  SHARKSSL_PSA_RSA_PRIVATE_MAX_SIZE

/**
 * Signing supports 64-byte raw (r || s) P-256 signatures and 256/384-byte
 * RSA PKCS#1 v1.5 or PSS signatures. Both schemes and ECDSA support
 * SHA-256, SHA-384 and SHA-512 when enabled.
 */
#define PSA_ECDSA_SIGNATURE_SIZE(curve_bits) \
   ((((curve_bits) == 256) || ((curve_bits) == 384)) ? ((size_t)(curve_bits) / 4) : ((size_t)0))
#define PSA_SIGN_OUTPUT_SIZE(key_type, key_bits, alg) \
   ((((key_type) == PSA_KEY_TYPE_RSA_KEY_PAIR) && \
     (((key_bits) == 2048) || ((key_bits) == 3072)) && \
     (SHARKSSL_PSA_RSA_PKCS1V15_SUPPORTED(alg) || \
      SHARKSSL_PSA_RSA_PSS_SUPPORTED(alg))) ? ((size_t)(key_bits) / 8) : \
   ((((key_type) == PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1)) && \
     SHARKSSL_PSA_ECDSA_CURVE_SUPPORTED(key_bits) && (SHARKSSL_PSA_ECDSA_SUPPORTED(alg))) ? \
    PSA_ECDSA_SIGNATURE_SIZE(key_bits) : ((size_t)0)))
#define PSA_SIGNATURE_MAX_SIZE  ((size_t)(SHARKSSL_PSA_RSA_PUBLIC_MAX_BITS / 8))
#define PSA_RAW_KEY_AGREEMENT_OUTPUT_SIZE(key_type, key_bits) \
   ((((key_type) == PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_MONTGOMERY)) && \
     ((key_bits) == 255)) ? ((size_t)32) : \
    ((((key_type) == PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1)) && \
      (((key_bits) == 256) || ((key_bits) == 384))) ? \
     ((size_t)(key_bits) / 8) : ((size_t)0)))
#define PSA_RAW_KEY_AGREEMENT_OUTPUT_MAX_SIZE  ((size_t)48)
#define SHARKSSL_PSA_ECIES_P256_BITS  256
#define PSA_ENCAPSULATE_CIPHERTEXT_SIZE(key_type, key_bits, alg) \
   ((SHARKSSL_PSA_HAS_ECIES_P256 && ((alg) == PSA_ALG_ECIES_SEC1) && \
     (((key_type) == PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1)) || \
      ((key_type) == PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1))) && \
     ((key_bits) == SHARKSSL_PSA_ECIES_P256_BITS)) ? \
    PSA_EXPORT_PUBLIC_KEY_OUTPUT_SIZE(PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1), \
                                      SHARKSSL_PSA_ECIES_P256_BITS) : (size_t)0)
#define PSA_ENCAPSULATE_CIPHERTEXT_MAX_SIZE \
   (SHARKSSL_PSA_HAS_ECIES_P256 ? \
    PSA_EXPORT_PUBLIC_KEY_OUTPUT_SIZE(PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1), \
                                      SHARKSSL_PSA_ECIES_P256_BITS) : (size_t)0)

#define SHARKSSL_PSA_CCM_TAG_FIELD(alg) ((((psa_algorithm_t)(alg)) >> 16) & 0x3f)
#define SHARKSSL_PSA_AEAD_CCM_TAG_LENGTH(alg) \
   (((SHARKSSL_PSA_HAS_AES_CCM) && \
     (PSA_ALG_CCM == PSA_ALG_AEAD_WITH_DEFAULT_LENGTH_TAG(alg)) && \
     ((((psa_algorithm_t)(alg)) & 0x00008000) == 0) && \
     ((SHARKSSL_PSA_CCM_TAG_FIELD(alg) == 0) || \
      ((SHARKSSL_PSA_CCM_TAG_FIELD(alg) >= 4) && \
       (SHARKSSL_PSA_CCM_TAG_FIELD(alg) <= 16) && \
       ((SHARKSSL_PSA_CCM_TAG_FIELD(alg) & 1) == 0)))) ? \
    ((SHARKSSL_PSA_CCM_TAG_FIELD(alg) == 0) ? (size_t)16 : \
     (size_t)SHARKSSL_PSA_CCM_TAG_FIELD(alg)) : (size_t)0)
#define SHARKSSL_PSA_AEAD_TAG_LENGTH(alg) \
   ((((alg) == PSA_ALG_GCM) || \
     (SHARKSSL_PSA_HAS_CHACHA20_POLY1305 && ((alg) == PSA_ALG_CHACHA20_POLY1305))) ? \
    (size_t)16 : SHARKSSL_PSA_AEAD_CCM_TAG_LENGTH(alg))
/**
 * Match each AEAD algorithm to its required symmetric key type.
 */
#define SHARKSSL_PSA_AEAD_KEY_SUPPORTED(key_type, alg) \
   (((PSA_KEY_TYPE_CHACHA20 == (key_type)) && SHARKSSL_PSA_HAS_CHACHA20_POLY1305 && \
     (PSA_ALG_CHACHA20_POLY1305 == (alg))) || \
    ((PSA_KEY_TYPE_AES == (key_type)) && (PSA_ALG_CHACHA20_POLY1305 != (alg)) && \
     (SHARKSSL_PSA_AEAD_TAG_LENGTH(alg) != 0)))
#define PSA_AEAD_TAG_LENGTH(key_type, key_bits, alg) \
   ((SHARKSSL_PSA_AEAD_KEY_SUPPORTED(key_type, alg) && \
     (((key_bits) == 256) || (((key_type) == PSA_KEY_TYPE_AES) && ((key_bits) == 128)))) ? \
    SHARKSSL_PSA_AEAD_TAG_LENGTH(alg) : ((size_t)0))
#define PSA_AEAD_ENCRYPT_OUTPUT_SIZE(key_type, alg, plaintext_length) \
   ((SHARKSSL_PSA_AEAD_KEY_SUPPORTED(key_type, alg) && \
     ((size_t)(plaintext_length) <= SIZE_MAX - SHARKSSL_PSA_AEAD_TAG_LENGTH(alg))) ? \
    ((size_t)(plaintext_length) + SHARKSSL_PSA_AEAD_TAG_LENGTH(alg)) : ((size_t)0))
#define PSA_AEAD_ENCRYPT_OUTPUT_MAX_SIZE(plaintext_length) \
   (((size_t)(plaintext_length) <= SIZE_MAX - 16) ? \
    ((size_t)(plaintext_length) + (size_t)16) : ((size_t)0))
#define PSA_AEAD_NONCE_LENGTH(key_type, alg) \
   ((((key_type) == PSA_KEY_TYPE_CHACHA20) && SHARKSSL_PSA_HAS_CHACHA20_POLY1305 && \
     ((alg) == PSA_ALG_CHACHA20_POLY1305)) ? ((size_t)12) : \
    (((key_type) == PSA_KEY_TYPE_AES) && ((alg) == PSA_ALG_GCM)) ? ((size_t)12) : \
    (((SHARKSSL_PSA_HAS_AES_CCM) && ((key_type) == PSA_KEY_TYPE_AES) && \
      (PSA_ALG_CCM == PSA_ALG_AEAD_WITH_DEFAULT_LENGTH_TAG(alg))) ? ((size_t)13) : ((size_t)0)))
#define PSA_AEAD_NONCE_MAX_SIZE  ((size_t)13)
#define PSA_AEAD_DECRYPT_OUTPUT_SIZE(key_type, alg, ciphertext_length) \
   ((SHARKSSL_PSA_AEAD_KEY_SUPPORTED(key_type, alg) && \
     ((ciphertext_length) >= SHARKSSL_PSA_AEAD_TAG_LENGTH(alg))) ? \
    ((size_t)(ciphertext_length) - SHARKSSL_PSA_AEAD_TAG_LENGTH(alg)) : ((size_t)0))
#define PSA_AEAD_DECRYPT_OUTPUT_MAX_SIZE(ciphertext_length) ((size_t)(ciphertext_length))
#define PSA_AEAD_TAG_MAX_SIZE  ((size_t)16)

#define PSA_AEAD_UPDATE_OUTPUT_SIZE(key_type, alg, input_length) \
   ((SHARKSSL_PSA_AEAD_KEY_SUPPORTED(key_type, alg)) ? \
    ((size_t)(input_length)) : ((size_t)0))
#define PSA_AEAD_UPDATE_OUTPUT_MAX_SIZE(input_length) ((size_t)(input_length))
#define PSA_AEAD_FINISH_OUTPUT_SIZE(key_type, alg) \
   ((SHARKSSL_PSA_AEAD_KEY_SUPPORTED(key_type, alg)) ? ((size_t)16) : ((size_t)0))
#define PSA_AEAD_FINISH_OUTPUT_MAX_SIZE  ((size_t)16)
#define PSA_AEAD_VERIFY_OUTPUT_SIZE(key_type, alg) \
   ((SHARKSSL_PSA_AEAD_KEY_SUPPORTED(key_type, alg)) ? ((size_t)16) : ((size_t)0))
#define PSA_AEAD_VERIFY_OUTPUT_MAX_SIZE  ((size_t)16)


#define PSA_BLOCK_CIPHER_BLOCK_LENGTH(key_type) \
   (((key_type) == PSA_KEY_TYPE_AES) ? ((size_t)16) : ((size_t)0))
#define PSA_BLOCK_CIPHER_BLOCK_MAX_SIZE  ((size_t)16)
#define PSA_CIPHER_IV_LENGTH(key_type, alg) \
   ((((key_type) == PSA_KEY_TYPE_AES) && \
     (((alg) == PSA_ALG_CTR) || ((alg) == PSA_ALG_CBC_NO_PADDING))) ? \
    ((size_t)16) : ((size_t)0))
#define PSA_CIPHER_IV_MAX_SIZE  ((size_t)16)
#define PSA_CIPHER_ENCRYPT_OUTPUT_SIZE(key_type, alg, input_length) \
   ((((key_type) == PSA_KEY_TYPE_AES) && \
     (((alg) == PSA_ALG_CTR) || ((alg) == PSA_ALG_ECB_NO_PADDING) || \
      ((alg) == PSA_ALG_CBC_NO_PADDING)) && \
     ((size_t)(input_length) <= SIZE_MAX - PSA_CIPHER_IV_LENGTH(key_type, alg))) ? \
    ((size_t)(input_length) + PSA_CIPHER_IV_LENGTH(key_type, alg)) : ((size_t)0))
#define PSA_CIPHER_ENCRYPT_OUTPUT_MAX_SIZE(input_length) \
   (((size_t)(input_length) <= SIZE_MAX - 16) ? \
    ((size_t)(input_length) + 16) : ((size_t)0))
#define PSA_CIPHER_DECRYPT_OUTPUT_SIZE(key_type, alg, input_length) \
   ((((key_type) == PSA_KEY_TYPE_AES) && \
     (((alg) == PSA_ALG_CTR) || ((alg) == PSA_ALG_ECB_NO_PADDING) || \
      ((alg) == PSA_ALG_CBC_NO_PADDING))) ? \
    ((size_t)(input_length)) : ((size_t)0))
#define PSA_CIPHER_DECRYPT_OUTPUT_MAX_SIZE(input_length) ((size_t)(input_length))
#define PSA_CIPHER_UPDATE_OUTPUT_SIZE(key_type, alg, input_length) \
   ((((key_type) == PSA_KEY_TYPE_AES) && ((alg) == PSA_ALG_CTR)) ? \
    ((size_t)(input_length)) : \
    (((key_type) == PSA_KEY_TYPE_AES) && \
     (((alg) == PSA_ALG_ECB_NO_PADDING) || ((alg) == PSA_ALG_CBC_NO_PADDING)) && \
     ((size_t)(input_length) <= SIZE_MAX - 15)) ? \
    (((size_t)(input_length) + 15) & ~(size_t)15) : ((size_t)0))
#define PSA_CIPHER_UPDATE_OUTPUT_MAX_SIZE(input_length) \
   (((size_t)(input_length) <= SIZE_MAX - 15) ? \
    (((size_t)(input_length) + 15) & ~(size_t)15) : ((size_t)0))
#define PSA_CIPHER_FINISH_OUTPUT_SIZE(key_type, alg) ((size_t)0)
#define PSA_CIPHER_FINISH_OUTPUT_MAX_SIZE  ((size_t)16)

/**
 * Hash operation storage is implementation-defined. The context reserves
 * 208 bytes for SHA-384 and a possible SHA-512 implementation; callers must not
 * inspect or modify either member.
 */
typedef struct psa_hash_operation_s
{
   uint32_t state;
   uint32_t context[52];
} psa_hash_operation_t;

#ifdef __cplusplus
#define PSA_HASH_OPERATION_INIT  psa_hash_operation_t()
#else
#define PSA_HASH_OPERATION_INIT  {0, {0}}
#endif

#if (PSA_CRYPTO_API_VERSION_MINOR >= 5)
/**
 * Multi-part signature operations retain a key dependency. Active objects
 * must not be copied or moved; terminate them with finish or abort. Build
 * clients and the provider with the same PSA profile and feature settings.
 */
typedef struct psa_sign_operation_s
{
   psa_hash_operation_t hash;
   void *key_references;
   psa_algorithm_t alg;
   psa_key_id_t key;
   uint32_t state;
   uint8_t flags;
} psa_sign_operation_t;

typedef struct psa_verify_operation_s
{
   psa_hash_operation_t hash;
   void *key_references;
   size_t signature_length;
   psa_algorithm_t alg;
   psa_key_id_t key;
   uint32_t state;
   uint8_t flags;
   uint8_t signature[PSA_SIGNATURE_MAX_SIZE];
} psa_verify_operation_t;

#ifdef __cplusplus
#define PSA_SIGN_OPERATION_INIT    psa_sign_operation_t()
#define PSA_VERIFY_OPERATION_INIT  psa_verify_operation_t()
#else
#define PSA_SIGN_OPERATION_INIT    {0}
#define PSA_VERIFY_OPERATION_INIT  {0}
#endif
#endif

/**
 * MAC operation storage is implementation-defined. Key dependency handles in
 * MAC, cipher, AEAD and derivation operations require matching client/provider
 * builds. Active objects must not be copied or moved; terminate them with abort.
 */
typedef struct psa_mac_operation_s
{
   uint32_t state;
   psa_algorithm_t alg;
   uint32_t context[96];
   void *key_references;
} psa_mac_operation_t;

#ifdef __cplusplus
#define PSA_MAC_OPERATION_INIT  psa_mac_operation_t()
#else
#define PSA_MAC_OPERATION_INIT  {0}
#endif

/** AES cipher state; applications must treat all members as opaque. */
typedef struct psa_cipher_operation_s
{
   uint32_t state;
   psa_algorithm_t alg;
   uint32_t aes_context[64];
   uint8_t chain[16];
   uint8_t pending[16];
   uint8_t stream[16];
   uint8_t pending_length;
   uint8_t stream_offset;
   uint8_t direction;
   uint8_t iv_set;
   void *key_references;
} psa_cipher_operation_t;

#ifdef __cplusplus
#define PSA_CIPHER_OPERATION_INIT  psa_cipher_operation_t()
#else
#define PSA_CIPHER_OPERATION_INIT  {0}
#endif

/**
 * AEAD storage is implementation-defined; all members are opaque.
 * Multipart decryption returns tentative plaintext until verify succeeds.
 */
typedef struct psa_aead_operation_s
{
   uint32_t state;
   uint32_t gcm_context[128];
   uint32_t ad_length;
   uint32_t body_length;
   uint32_t ad_remaining;
   uint32_t body_remaining;
   uint8_t counter[16];
   uint8_t hash[16];
   uint8_t pending[16];
   uint8_t stream[16];
   uint8_t pending_length;
   uint8_t stream_offset;
   uint8_t nonce_length;
   uint8_t tag_length;
   void *key_references;
} psa_aead_operation_t;

#ifdef __cplusplus
#define PSA_AEAD_OPERATION_INIT  psa_aead_operation_t()
#else
#define PSA_AEAD_OPERATION_INIT  {0}
#endif


/**
 * Key-derivation state. agreement_alg holds the combined ECDH/KDF
 * algorithm when applicable; alg holds the underlying KDF. HKDF information
 * and PBKDF2 salt are limited to 256 bytes; TLS 1.2 seed is limited to
 * 64 bytes, label to 256 bytes and PSK-to-MS other secret to 64 bytes.
 * HKDF output is limited to 255 digest blocks, except HKDF-Extract, whose
 * output is one digest. The other HMAC derivations support SHA-256/384/512 when enabled.
 * SP 800-108 has a 128-byte label, 256-byte context and 32-bit counter;
 * its initial capacity is 0x1fffffff bytes. The first output fixes L.
 * Applications must treat the complete object as opaque.
 */
typedef struct psa_key_derivation_operation_s
{
   uint32_t state;
   psa_algorithm_t alg;
   psa_algorithm_t agreement_alg;
   uint32_t cost;
   uint32_t sp800108_counter;
   uint32_t sp800108_bits;
   size_t capacity;
   size_t info_length;
   uint8_t salt[128];
   uint8_t prk[128];
   uint8_t block[64];
   uint8_t prf_a[64];
   uint8_t other_secret[64];
   uint8_t info[256];
   uint16_t salt_length;
   uint8_t block_number;
   uint8_t block_offset;
   uint8_t prk_length;
   uint8_t other_secret_length;
   uint8_t flags;
   void *key_references;
   psa_key_id_t opaque_pbkdf2_key;
   psa_key_derivation_step_t opaque_pbkdf2_step;
   size_t opaque_salt_offset;
} psa_key_derivation_operation_t;

#ifdef __cplusplus
#define PSA_KEY_DERIVATION_OPERATION_INIT  psa_key_derivation_operation_t()
#else
#define PSA_KEY_DERIVATION_OPERATION_INIT  {0}
#endif

#define PSA_KEY_DERIVATION_INPUT_SECRET        ((psa_key_derivation_step_t)1)
#define PSA_KEY_DERIVATION_INPUT_SALT          ((psa_key_derivation_step_t)2)
#define PSA_KEY_DERIVATION_INPUT_INFO          ((psa_key_derivation_step_t)3)
#define PSA_KEY_DERIVATION_INPUT_PASSWORD      ((psa_key_derivation_step_t)4)
#define PSA_KEY_DERIVATION_INPUT_COST          ((psa_key_derivation_step_t)5)
#define PSA_KEY_DERIVATION_INPUT_SEED          ((psa_key_derivation_step_t)6)
#define PSA_KEY_DERIVATION_INPUT_LABEL         ((psa_key_derivation_step_t)7)
#define PSA_KEY_DERIVATION_INPUT_OTHER_SECRET  ((psa_key_derivation_step_t)8)
#define PSA_KEY_DERIVATION_INPUT_CONTEXT       ((psa_key_derivation_step_t)9)
#define PSA_TLS12_PSK_TO_MS_PSK_MAX_SIZE       ((size_t)64)

psa_status_t psa_crypto_init(void);
psa_status_t psa_generate_random(uint8_t *output, size_t output_size);
psa_status_t psa_hash_compute(psa_algorithm_t alg, const uint8_t *input,
                              size_t input_length, uint8_t *hash,
                              size_t hash_size, size_t *hash_length);
psa_status_t psa_hash_compare(psa_algorithm_t alg, const uint8_t *input,
                              size_t input_length, const uint8_t *hash,
                              size_t hash_length);
psa_hash_operation_t psa_hash_operation_init(void);
psa_status_t psa_hash_setup(psa_hash_operation_t *operation, psa_algorithm_t alg);
psa_status_t psa_hash_update(psa_hash_operation_t *operation, const uint8_t *input,
                             size_t input_length);
psa_status_t psa_hash_finish(psa_hash_operation_t *operation, uint8_t *hash,
                             size_t hash_size, size_t *hash_length);
psa_status_t psa_hash_verify(psa_hash_operation_t *operation, const uint8_t *hash,
                             size_t hash_length);
psa_status_t psa_hash_abort(psa_hash_operation_t *operation);
psa_status_t psa_hash_suspend(psa_hash_operation_t *operation, uint8_t *hash_state,
                              size_t hash_state_size, size_t *hash_state_length);
psa_status_t psa_hash_resume(psa_hash_operation_t *operation, const uint8_t *hash_state,
                             size_t hash_state_length);
psa_status_t psa_hash_clone(const psa_hash_operation_t *source_operation,
                            psa_hash_operation_t *target_operation);

psa_mac_operation_t psa_mac_operation_init(void);
psa_status_t psa_mac_compute(psa_key_id_t key, psa_algorithm_t alg,
                             const uint8_t *input, size_t input_length,
                             uint8_t *mac, size_t mac_size, size_t *mac_length);
psa_status_t psa_mac_verify(psa_key_id_t key, psa_algorithm_t alg,
                            const uint8_t *input, size_t input_length,
                            const uint8_t *mac, size_t mac_length);
psa_status_t psa_mac_sign_setup(psa_mac_operation_t *operation,
                                psa_key_id_t key, psa_algorithm_t alg);
psa_status_t psa_mac_verify_setup(psa_mac_operation_t *operation,
                                  psa_key_id_t key, psa_algorithm_t alg);
psa_status_t psa_mac_update(psa_mac_operation_t *operation,
                            const uint8_t *input, size_t input_length);
psa_status_t psa_mac_sign_finish(psa_mac_operation_t *operation,
                                 uint8_t *mac, size_t mac_size, size_t *mac_length);
psa_status_t psa_mac_verify_finish(psa_mac_operation_t *operation,
                                   const uint8_t *mac, size_t mac_length);
psa_status_t psa_mac_abort(psa_mac_operation_t *operation);

psa_status_t psa_aead_encrypt(psa_key_id_t key, psa_algorithm_t alg,
                              const uint8_t *nonce, size_t nonce_length,
                              const uint8_t *additional_data, size_t additional_data_length,
                              const uint8_t *plaintext, size_t plaintext_length,
                              uint8_t *ciphertext, size_t ciphertext_size,
                              size_t *ciphertext_length);
psa_status_t psa_aead_decrypt(psa_key_id_t key, psa_algorithm_t alg,
                              const uint8_t *nonce, size_t nonce_length,
                              const uint8_t *additional_data, size_t additional_data_length,
                              const uint8_t *ciphertext, size_t ciphertext_length,
                              uint8_t *plaintext, size_t plaintext_size,
                              size_t *plaintext_length);

psa_aead_operation_t psa_aead_operation_init(void);
psa_status_t psa_aead_encrypt_setup(psa_aead_operation_t *operation, psa_key_id_t key, psa_algorithm_t alg);
psa_status_t psa_aead_decrypt_setup(psa_aead_operation_t *operation, psa_key_id_t key, psa_algorithm_t alg);
psa_status_t psa_aead_set_lengths(psa_aead_operation_t *operation, size_t ad_length, size_t plaintext_length);
psa_status_t psa_aead_generate_nonce(psa_aead_operation_t *operation, uint8_t *nonce, size_t nonce_size, size_t *nonce_length);
psa_status_t psa_aead_set_nonce(psa_aead_operation_t *operation, const uint8_t *nonce, size_t nonce_length);
psa_status_t psa_aead_update_ad(psa_aead_operation_t *operation, const uint8_t *input, size_t input_length);
psa_status_t psa_aead_update(psa_aead_operation_t *operation, const uint8_t *input, size_t input_length,
                             uint8_t *output, size_t output_size, size_t *output_length);
psa_status_t psa_aead_finish(psa_aead_operation_t *operation, uint8_t *ciphertext, size_t ciphertext_size,
                             size_t *ciphertext_length, uint8_t *tag, size_t tag_size, size_t *tag_length);
psa_status_t psa_aead_verify(psa_aead_operation_t *operation, uint8_t *plaintext, size_t plaintext_size,
                             size_t *plaintext_length, const uint8_t *tag, size_t tag_length);
psa_status_t psa_aead_abort(psa_aead_operation_t *operation);


psa_cipher_operation_t psa_cipher_operation_init(void);
psa_status_t psa_cipher_encrypt_setup(psa_cipher_operation_t *operation,
                                      psa_key_id_t key, psa_algorithm_t alg);
psa_status_t psa_cipher_decrypt_setup(psa_cipher_operation_t *operation,
                                      psa_key_id_t key, psa_algorithm_t alg);
psa_status_t psa_cipher_generate_iv(psa_cipher_operation_t *operation,
                                    uint8_t *iv, size_t iv_size,
                                    size_t *iv_length);
psa_status_t psa_cipher_set_iv(psa_cipher_operation_t *operation,
                               const uint8_t *iv, size_t iv_length);
psa_status_t psa_cipher_update(psa_cipher_operation_t *operation,
                               const uint8_t *input, size_t input_length,
                               uint8_t *output, size_t output_size,
                               size_t *output_length);
psa_status_t psa_cipher_finish(psa_cipher_operation_t *operation,
                               uint8_t *output, size_t output_size,
                               size_t *output_length);
psa_status_t psa_cipher_abort(psa_cipher_operation_t *operation);
psa_status_t psa_cipher_encrypt(psa_key_id_t key, psa_algorithm_t alg,
                                const uint8_t *input, size_t input_length,
                                uint8_t *output, size_t output_size,
                                size_t *output_length);
psa_status_t psa_cipher_decrypt(psa_key_id_t key, psa_algorithm_t alg,
                                const uint8_t *input, size_t input_length,
                                uint8_t *output, size_t output_size,
                                size_t *output_length);

psa_key_attributes_t psa_key_attributes_init(void);
void psa_reset_key_attributes(psa_key_attributes_t *attributes);
void psa_set_key_type(psa_key_attributes_t *attributes, psa_key_type_t type);
psa_key_type_t psa_get_key_type(const psa_key_attributes_t *attributes);
void psa_set_key_bits(psa_key_attributes_t *attributes, size_t bits);
size_t psa_get_key_bits(const psa_key_attributes_t *attributes);
void psa_set_key_lifetime(psa_key_attributes_t *attributes, psa_key_lifetime_t lifetime);
psa_key_lifetime_t psa_get_key_lifetime(const psa_key_attributes_t *attributes);
void psa_set_key_id(psa_key_attributes_t *attributes, psa_key_id_t id);
psa_key_id_t psa_get_key_id(const psa_key_attributes_t *attributes);
void psa_set_key_usage_flags(psa_key_attributes_t *attributes, psa_key_usage_t usage_flags);
psa_key_usage_t psa_get_key_usage_flags(const psa_key_attributes_t *attributes);
void psa_set_key_algorithm(psa_key_attributes_t *attributes, psa_algorithm_t alg);
psa_algorithm_t psa_get_key_algorithm(const psa_key_attributes_t *attributes);
psa_status_t psa_get_key_attributes(psa_key_id_t key, psa_key_attributes_t *attributes);
psa_status_t psa_check_key_usage(psa_key_id_t key, psa_algorithm_t alg,
                                 psa_key_usage_t usage);
psa_status_t psa_import_key(const psa_key_attributes_t *attributes, const uint8_t *data,
                            size_t data_length, psa_key_id_t *key);
psa_status_t psa_attach_key(const psa_key_attributes_t *attributes,
                            const uint8_t *label, size_t label_length,
                            psa_key_id_t *key);
psa_status_t psa_generate_key(const psa_key_attributes_t *attributes, psa_key_id_t *key);
psa_status_t psa_generate_key_custom(const psa_key_attributes_t *attributes,
                                     const psa_custom_key_parameters_t *custom,
                                     const uint8_t *custom_data,
                                     size_t custom_data_length, psa_key_id_t *key);
psa_status_t psa_copy_key(psa_key_id_t source_key,
                          const psa_key_attributes_t *attributes,
                          psa_key_id_t *target_key);
psa_status_t psa_export_key(psa_key_id_t key, uint8_t *data, size_t data_size,
                            size_t *data_length);
psa_status_t psa_wrap_key(psa_key_id_t wrapping_key, psa_algorithm_t alg,
                          psa_key_id_t key, uint8_t *data, size_t data_size,
                          size_t *data_length);
psa_status_t psa_unwrap_key(const psa_key_attributes_t *attributes,
                            psa_key_id_t wrapping_key, psa_algorithm_t alg,
                            const uint8_t *data, size_t data_length,
                            psa_key_id_t *key);
psa_status_t psa_export_public_key(psa_key_id_t key, uint8_t *data, size_t data_size,
                                   size_t *data_length);
psa_status_t psa_raw_key_agreement(psa_algorithm_t alg, psa_key_id_t private_key,
                                   const uint8_t *peer_key, size_t peer_key_length,
                                   uint8_t *output, size_t output_size,
                                   size_t *output_length);
psa_status_t psa_key_agreement(psa_key_id_t private_key,
                               const uint8_t *peer_key, size_t peer_key_length,
                               psa_algorithm_t alg,
                               const psa_key_attributes_t *attributes,
                               psa_key_id_t *key);
psa_status_t psa_encapsulate(psa_key_id_t key, psa_algorithm_t alg,
                             const psa_key_attributes_t *attributes,
                             psa_key_id_t *output_key, uint8_t *ciphertext,
                             size_t ciphertext_size, size_t *ciphertext_length);
psa_status_t psa_decapsulate(psa_key_id_t key, psa_algorithm_t alg,
                             const uint8_t *ciphertext, size_t ciphertext_length,
                             const psa_key_attributes_t *attributes,
                             psa_key_id_t *output_key);
psa_status_t psa_sign_message(psa_key_id_t key, psa_algorithm_t alg,
                              const uint8_t *input, size_t input_length,
                              uint8_t *signature, size_t signature_size,
                              size_t *signature_length);
psa_status_t psa_sign_hash(psa_key_id_t key, psa_algorithm_t alg,
                           const uint8_t *hash, size_t hash_length,
                           uint8_t *signature, size_t signature_size,
                           size_t *signature_length);
psa_status_t psa_verify_message(psa_key_id_t key, psa_algorithm_t alg,
                                const uint8_t *input, size_t input_length,
                                const uint8_t *signature, size_t signature_length);
psa_status_t psa_verify_hash(psa_key_id_t key, psa_algorithm_t alg,
                             const uint8_t *hash, size_t hash_length,
                             const uint8_t *signature, size_t signature_length);
psa_status_t psa_sign_message_with_context(psa_key_id_t key, psa_algorithm_t alg,
                                           const uint8_t *input, size_t input_length,
                                           const uint8_t *context, size_t context_length,
                                           uint8_t *signature, size_t signature_size,
                                           size_t *signature_length);

psa_status_t psa_verify_message_with_context(psa_key_id_t key, psa_algorithm_t alg,
                                             const uint8_t *input, size_t input_length,
                                             const uint8_t *context, size_t context_length,
                                             const uint8_t *signature, size_t signature_length);

psa_status_t psa_sign_hash_with_context(psa_key_id_t key, psa_algorithm_t alg,
                                        const uint8_t *hash, size_t hash_length,
                                        const uint8_t *context, size_t context_length,
                                        uint8_t *signature, size_t signature_size,
                                        size_t *signature_length);

psa_status_t psa_verify_hash_with_context(psa_key_id_t key, psa_algorithm_t alg,
                                          const uint8_t *hash, size_t hash_length,
                                          const uint8_t *context, size_t context_length,
                                          const uint8_t *signature, size_t signature_length);

#if (PSA_CRYPTO_API_VERSION_MINOR >= 5)
psa_sign_operation_t psa_sign_operation_init(void);
psa_status_t psa_sign_setup(psa_sign_operation_t *operation, psa_key_id_t key, psa_algorithm_t alg);
psa_status_t psa_sign_set_context(psa_sign_operation_t *operation, const uint8_t *context, size_t context_length);
psa_status_t psa_sign_update(psa_sign_operation_t *operation, const uint8_t *input, size_t input_length);
psa_status_t psa_sign_finish(psa_sign_operation_t *operation, uint8_t *signature, size_t signature_size,
                             size_t *signature_length);
psa_status_t psa_sign_abort(psa_sign_operation_t *operation);

psa_verify_operation_t psa_verify_operation_init(void);
psa_status_t psa_verify_setup(psa_verify_operation_t *operation, psa_key_id_t key, psa_algorithm_t alg,
                              const uint8_t *signature, size_t signature_length);
psa_status_t psa_verify_set_context(psa_verify_operation_t *operation, const uint8_t *context, size_t context_length);
psa_status_t psa_verify_update(psa_verify_operation_t *operation, const uint8_t *input, size_t input_length);
psa_status_t psa_verify_finish(psa_verify_operation_t *operation);
psa_status_t psa_verify_abort(psa_verify_operation_t *operation);
#endif

psa_status_t psa_asymmetric_encrypt(psa_key_id_t key, psa_algorithm_t alg,
                                    const uint8_t *input, size_t input_length,
                                    const uint8_t *salt, size_t salt_length,
                                    uint8_t *output, size_t output_size,
                                    size_t *output_length);
psa_status_t psa_asymmetric_decrypt(psa_key_id_t key, psa_algorithm_t alg,
                                    const uint8_t *input, size_t input_length,
                                    const uint8_t *salt, size_t salt_length,
                                    uint8_t *output, size_t output_size,
                                    size_t *output_length);
psa_status_t psa_destroy_key(psa_key_id_t key);
psa_status_t psa_purge_key(psa_key_id_t key);

psa_key_derivation_operation_t psa_key_derivation_operation_init(void);
psa_status_t psa_key_derivation_setup(psa_key_derivation_operation_t *operation,
                                      psa_algorithm_t alg);
psa_status_t psa_key_derivation_get_capacity(const psa_key_derivation_operation_t *operation,
                                             size_t *capacity);
psa_status_t psa_key_derivation_set_capacity(psa_key_derivation_operation_t *operation,
                                             size_t capacity);
psa_status_t psa_key_derivation_input_bytes(psa_key_derivation_operation_t *operation,
                                            psa_key_derivation_step_t step,
                                            const uint8_t *data, size_t data_length);
psa_status_t psa_key_derivation_input_key(psa_key_derivation_operation_t *operation,
                                          psa_key_derivation_step_t step,
                                          psa_key_id_t key);
psa_status_t psa_key_derivation_key_agreement(psa_key_derivation_operation_t *operation,
                                              psa_key_derivation_step_t step,
                                              psa_key_id_t private_key,
                                              const uint8_t *peer_key,
                                              size_t peer_key_length);
psa_status_t psa_key_derivation_input_integer(psa_key_derivation_operation_t *operation,
                                              psa_key_derivation_step_t step,
                                              uint64_t value);
psa_status_t psa_key_derivation_output_bytes(psa_key_derivation_operation_t *operation,
                                             uint8_t *output, size_t output_length);
psa_status_t psa_key_derivation_output_key(const psa_key_attributes_t *attributes,
                                           psa_key_derivation_operation_t *operation,
                                           psa_key_id_t *key);
psa_status_t psa_key_derivation_output_key_custom(const psa_key_attributes_t *attributes,
                                                  psa_key_derivation_operation_t *operation,
                                                  const psa_custom_key_parameters_t *custom,
                                                  const uint8_t *custom_data,
                                                  size_t custom_data_length,
                                                  psa_key_id_t *key);
psa_status_t psa_key_derivation_verify_bytes(psa_key_derivation_operation_t *operation,
                                             const uint8_t *expected_output,
                                             size_t output_length);
psa_status_t psa_key_derivation_verify_key(psa_key_derivation_operation_t *operation,
                                           psa_key_id_t expected);
psa_status_t psa_key_derivation_abort(psa_key_derivation_operation_t *operation);

#ifdef __cplusplus
}
#endif

#endif
