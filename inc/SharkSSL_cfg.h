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
 *   $Id: SharkSSL_cfg.h 5194 2022-06-29 06:39:42Z gianluca $
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


 Do not directly edit the options in this file. Instead, add your
 custom options in SharkSSL_opts.h

 */
#ifndef _SharkSsl_cfg_h
#define _SharkSsl_cfg_h

#include "SharkSSL_opts.h"

/** @addtogroup SharkSslCfg
@{
*/

/** TLS 1.2 always enabled - forward compatibility to dual 1.2/1.3 stack 
 *  (needed for selib.c)
 */
#undef  SHARKSSL_TLS_1_2
#define SHARKSSL_TLS_1_2                                 1


/** Enable/disable AES 256
 */
#ifndef SHARKSSL_USE_AES_256
#define SHARKSSL_USE_AES_256                             1
#endif

/** Enable/disable AES 128
 */
#ifndef SHARKSSL_USE_AES_128
#define SHARKSSL_USE_AES_128                             1
#endif

/**
 * AES-192 is not used in SSL/TLS
 * enable only if needed in application using the crypto API
 */
#ifndef SHARKSSL_USE_AES_192
#define SHARKSSL_USE_AES_192                             0
#endif

/**
 * AES-GCM require AES:
 * relevant ciphersuites are included
 */
#ifndef SHARKSSL_ENABLE_AES_GCM
#define SHARKSSL_ENABLE_AES_GCM                          1
#endif

/**
 * AES-CCM require AES:
 * only for crypto functions - CCM TLS ciphersuites removed
 */
#ifndef SHARKSSL_ENABLE_AES_CCM
#define SHARKSSL_ENABLE_AES_CCM                          0
#endif

/**
 * AES-CBC require AES:
 * CBC TLS ciphersuites removed
 * for crypto functions and PEM certificate decryption
 */
#ifndef SHARKSSL_ENABLE_AES_CBC
#define SHARKSSL_ENABLE_AES_CBC                          0
#endif

/** Enable/disable CHACHA20 support and also include
    CHACHA20-POLY1305 ciphersuites when TLS1.2 and POLY1305 are enabled
    (#SHARKSSL_USE_POLY1305)
 */
#ifndef SHARKSSL_USE_CHACHA20
#define SHARKSSL_USE_CHACHA20                            1
#endif


/** \defgroup SharkSslCfgHash HASH algorithms
\ingroup SharkSslCfg
@{
*/

/** Enable/disable SHA256 support for certificate signatures (SHA256
    ciphersuites are not included).
    SHA256 must be included (mandatory) for TLS 1.2
 */
#ifndef SHARKSSL_USE_SHA_256
#define SHARKSSL_USE_SHA_256                             1
#endif

/** Enable/disable SHA384 support and also include
    SHA384 ciphersuites 
*/
#ifndef SHARKSSL_USE_SHA_384
#define SHARKSSL_USE_SHA_384                             1
#endif

/** Enable/disable SHA512 support;
    Note SHA512 is not required by SSL/TLS.
*/
#ifndef SHARKSSL_USE_SHA_512
#define SHARKSSL_USE_SHA_512                             0
#endif


/** SHA1 is not used by the SharkSSL TLS stack unless you use RSA OAEP
 * (public RSA API). SHA1 is used by some examples. You can disable
 * SHA1 if you are not using RSA OAEP and/or the examples using SHA1
 */
#ifndef SHARKSSL_USE_SHA1
#define SHARKSSL_USE_SHA1                                1
#endif

/** MD5 must be enabled to support PCKS1-encoded certificates (public PEM API).
 */
#ifndef SHARKSSL_USE_MD5
#define SHARKSSL_USE_MD5                                 0
#endif

/** Enable/disable POLY1305 support and also include
    CHACHA20-POLY1305 ciphersuites when TLS1.2 and CHACHA20 are enabled
    (#SHARKSSL_USE_CHACHA20)
 */
#ifndef SHARKSSL_USE_POLY1305
#define SHARKSSL_USE_POLY1305                            1
#endif

/** @} */ /* end group SharkSslCfgHash */


/**
 * select 1 to enable SERVER side TLS
 */
#ifndef SHARKSSL_SSL_SERVER_CODE
#define SHARKSSL_SSL_SERVER_CODE                         1
#endif


/**
 * select 1 to enable client authentication from server
 */
#ifndef SHARKSSL_ENABLE_CLIENT_AUTH
#define SHARKSSL_ENABLE_CLIENT_AUTH                      1
#endif


/**
 * select 1 to enable CLIENT side TLS
 */
#ifndef SHARKSSL_SSL_CLIENT_CODE
#define SHARKSSL_SSL_CLIENT_CODE                         1
#endif


/**
 * select 1 to enable support for Server Name Indication
 */
#ifndef SHARKSSL_ENABLE_SNI
#define SHARKSSL_ENABLE_SNI                              1
#endif


/**
 * select 0 to disable RSA ciphersuites
 */
#ifndef SHARKSSL_ENABLE_RSA
#define SHARKSSL_ENABLE_RSA                              1
#endif


/**
 * select 1 to enable session caching
 */
#ifndef SHARKSSL_ENABLE_SESSION_CACHE
#define SHARKSSL_ENABLE_SESSION_CACHE                    1
#endif


/**
 * select 1 to enable renegotiation
 * only secure renegotiation (RFC5746) is supported
 */
#ifndef SHARKSSL_ENABLE_SECURE_RENEGOTIATION
#define SHARKSSL_ENABLE_SECURE_RENEGOTIATION             1
#endif


/*
 * TLS 1.2 requires SHA-256, do not modify the following settings
 * DES and ClientHello v2.0 are deprecated in TLS 1.2 - RFC5246
 */
#undef  SHARKSSL_USE_SHA_256
#define SHARKSSL_USE_SHA_256                             1


/**
 * select 1 to enable DHE_RSA ciphersuites
 */
#ifndef SHARKSSL_ENABLE_DHE_RSA
#define SHARKSSL_ENABLE_DHE_RSA                          1
#endif


/** Enable/disable the SharkSslCon_selectCiphersuite API
 */
#ifndef SHARKSSL_ENABLE_SELECT_CIPHERSUITE
#define SHARKSSL_ENABLE_SELECT_CIPHERSUITE               1
#endif


/** Determine the number of ciphersuites that can be selected, in
    decreasing order of preference; this value is only in effect if the
    #SHARKSSL_ENABLE_SELECT_CIPHERSUITE is selected.
 */
#ifndef SHARKSSL_SELECT_CIPHERSUITE_LIST_DEPTH
#define SHARKSSL_SELECT_CIPHERSUITE_LIST_DEPTH           8
#endif


 /** Enable/disable ALPN API (support for ALPN extension, RFC 7301)
  */
#ifndef SHARKSSL_ENABLE_ALPN_EXTENSION
#define SHARKSSL_ENABLE_ALPN_EXTENSION                   1
#endif


 /** Enable/disable RSA API (sharkssl_RSA_public_encrypt,
 *  sharkssl_RSA_private_decrypt, sharkssl_RSA_private_encrypt,
 *  sharkssl_RSA_public_decrypt, SharkSslRSAKey_size)
 */
#ifndef SHARKSSL_ENABLE_RSA_API
#define SHARKSSL_ENABLE_RSA_API                          1
#endif


/** Enable/disable PKCS1 padding in RSA API
 *  (#SHARKSSL_ENABLE_RSA_API must be enabled)
 *  note: always enabled when SSL client or server enabled
 */
#ifndef SHARKSSL_ENABLE_RSA_PKCS1
#define SHARKSSL_ENABLE_RSA_PKCS1                        1
#endif


/** Enable/disable OAEP padding in RSA API
 *  (#SHARKSSL_ENABLE_RSA_API must be enabled)
 */
#ifndef SHARKSSL_ENABLE_RSA_OAEP
#define SHARKSSL_ENABLE_RSA_OAEP                         0
#endif


/** Enable/disable ECDSA API (sharkssl_ECDSA_sign,
 *  sharkssl_ECDSA_verify, SharkSslECDSA_siglen)
 */
#ifndef SHARKSSL_ENABLE_ECDSA_API
#define SHARKSSL_ENABLE_ECDSA_API                        1
#endif


/** Disable ECDSA sign API functions (sharkssl_ECDSA_sign,
 *  SharkSslECDSA_siglen) - effective only if ECDSA API is
 *  compiled (#SHARKSSL_ENABLE_ECDSA_API must be enabled)
 *  and no SSL/TLS library used (only RayCrypto); used to
 *  achieve minimum code size
 */
#ifndef SHARKSSL_ECDSA_ONLY_VERIFY
#define SHARKSSL_ECDSA_ONLY_VERIFY                       0
#endif


/**
 * select 1 to enable PEM certs/keys decoding
 * if RSA_API is enabled, then also the functions
 * sharkssl_PEM_to_RSAKey and SharkSslRSAKey_free are available
 * if ECDSA_API is enabled, then also the functions
 * sharkssl_PEM_to_ECCKey and SharkSslECCKey_free are available
 */
#ifndef SHARKSSL_ENABLE_PEM_API
#define SHARKSSL_ENABLE_PEM_API                          1
#endif


/** Enable/disable support for encrypted PKCS#8 certificates
 *  in sharkssl_PEM function
 * (requires SHARKSSL_ENABLE_AES_CBC)
 */
#ifndef SHARKSSL_ENABLE_ENCRYPTED_PKCS8_SUPPORT
#define SHARKSSL_ENABLE_ENCRYPTED_PKCS8_SUPPORT          0
#endif


/** Enable/disable #SharkSslCon_getCiphersuite
 */
#ifndef SHARKSSL_ENABLE_INFO_API
#define SHARKSSL_ENABLE_INFO_API                         1
#endif


/**
 * select 1 to enable certificate chain support
 */
#ifndef SHARKSSL_ENABLE_CERT_CHAIN
#define SHARKSSL_ENABLE_CERT_CHAIN                       1
#endif


/**
 * select 1 to enable CA check
 * (client or server with client auth)
 */
#ifndef SHARKSSL_ENABLE_CA_LIST
#define SHARKSSL_ENABLE_CA_LIST                          1
#endif


/**
 * select 1 to enable certificate storage
 */
#ifndef SHARKSSL_ENABLE_CERTSTORE_API
#define SHARKSSL_ENABLE_CERTSTORE_API                    1
#endif


/**
 * select 1 to enable automatic certificate cloning
 */
#ifndef SHARKSSL_ENABLE_CLONE_CERTINFO
#define SHARKSSL_ENABLE_CLONE_CERTINFO                   1
#endif


/**
 * select 1 to enable parsing KeyUsage and ExtendedKeyUsage
 * in the certificates
 */
#ifndef SHARKSSL_ENABLE_CERT_KEYUSAGE
#define SHARKSSL_ENABLE_CERT_KEYUSAGE                    0
#endif


/**
 * select 1 (small ROM footprint, slow) or 0 (large, fast)
 *
 * SHA 384 is only available in small footprint version,
 * being the fast version only 20% faster at the expense
 * of an 8x code size (benchmarked on ARM Cortex M3)
 */
#ifndef SHARKSSL_MD5_SMALL_FOOTPRINT
#define SHARKSSL_MD5_SMALL_FOOTPRINT                     0
#endif

#ifndef SHARKSSL_SHA1_SMALL_FOOTPRINT
#define SHARKSSL_SHA1_SMALL_FOOTPRINT                    0
#endif

/** Select 1 for smaller, but slower SHA256
 */
#ifndef SHARKSSL_SHA256_SMALL_FOOTPRINT
#define SHARKSSL_SHA256_SMALL_FOOTPRINT                  0
#endif


/**
 * select a window size between 1 (slower, less RAM) and 5
 */
#ifndef SHARKSSL_BIGINT_EXP_SLIDING_WINDOW_K
#define SHARKSSL_BIGINT_EXP_SLIDING_WINDOW_K             4
#endif


/**
 * select 0 (slower, less ROM) or 1 (20% faster, more ROM)
 */
#ifndef SHARKSSL_BIGINT_MULT_LOOP_UNROLL
#define SHARKSSL_BIGINT_MULT_LOOP_UNROLL                 1
#endif


/**
 * select 1 to include AES CTR mode (USE_AES_xxx must be enabled)
 */
#ifndef SHARKSSL_ENABLE_AES_CTR_MODE
#define SHARKSSL_ENABLE_AES_CTR_MODE                     1
#endif


/**
 * select 0 (35% less ROM) or 1 (10-15% faster)
 */
#ifndef SHARKSSL_AES_CIPHER_LOOP_UNROLL
#define SHARKSSL_AES_CIPHER_LOOP_UNROLL                  1
#endif


/**
 * select 1 if your architecture supports unaligned memory
 * access (x86, ARM-Cortex-M3, ColdFire)
 */
#ifndef SHARKSSL_UNALIGNED_ACCESS
#ifdef UNALIGNED_ACCESS
#define SHARKSSL_UNALIGNED_ACCESS                        1
#else
#define SHARKSSL_UNALIGNED_ACCESS                        0
#endif
#endif


/**
 * select 8, 16 or 32 according to your architecture
 */
#ifndef SHARKSSL_BIGINT_WORDSIZE
#define SHARKSSL_BIGINT_WORDSIZE                         32
#endif


/**
 * Elliptic Curve Cryptography
 */
#ifndef SHARKSSL_USE_ECC
#define SHARKSSL_USE_ECC                                 1
#endif


/**
 * select 1 to enable generation and verification of
 * elliptic curve digital signatures
 */
#ifndef SHARKSSL_ENABLE_ECDSA
#define SHARKSSL_ENABLE_ECDSA                            1
#endif


/**
 * select 1 to verify that a point lies on a curve
 * verification in function SharkSslECNISTCurve_setPoint
 * -larger ROM (parameter B for each curve stored, more code)
 * -slightly slower execution
 */
#ifndef SHARKSSL_ECC_VERIFY_POINT
#define SHARKSSL_ECC_VERIFY_POINT                        1
#endif


/** Enable timing resistant
 */
#ifndef SHARKSSL_ECC_TIMING_RESISTANT
#define SHARKSSL_ECC_TIMING_RESISTANT                    0
#endif

/** Enable/disable the SECP256R1 curve
 */
#ifndef SHARKSSL_ECC_USE_SECP256R1
#define SHARKSSL_ECC_USE_SECP256R1                       1
#endif

/** Enable/disable the SECP384R1 curve
 */
#ifndef SHARKSSL_ECC_USE_SECP384R1
#define SHARKSSL_ECC_USE_SECP384R1                       1
#endif

/** Enable/disable the SECP521R1 curve
 */
#ifndef SHARKSSL_ECC_USE_SECP521R1
#define SHARKSSL_ECC_USE_SECP521R1                       1
#endif


/** Enable/disable the brainpoolP256r1 curve (RFC5639)
 */
#ifndef SHARKSSL_ECC_USE_BRAINPOOLP256R1
#define SHARKSSL_ECC_USE_BRAINPOOLP256R1                 1
#endif

/** Enable/disable the brainpoolP384r1 curve (RFC5639)
 */
#ifndef SHARKSSL_ECC_USE_BRAINPOOLP384R1
#define SHARKSSL_ECC_USE_BRAINPOOLP384R1                 1
#endif

/** Enable/disable the brainpoolP512r1 curve (RFC5639)
 */
#ifndef SHARKSSL_ECC_USE_BRAINPOOLP512R1
#define SHARKSSL_ECC_USE_BRAINPOOLP512R1                 1
#endif


/**
 * select 1 to enable ECDHE_RSA ciphersuites (RFC 4492)
 * Elliptic Curve Cryptography (#SHARKSSL_USE_ECC) must be enabled
 */
#ifndef SHARKSSL_ENABLE_ECDHE_RSA
#define SHARKSSL_ENABLE_ECDHE_RSA                        1
#endif


/**
 * select 1 to enable ECDHE_ECDSA ciphersuites (RFC 4492)
 * Elliptic Curve Cryptography (#SHARKSSL_USE_ECC) must be enabled
 * SHARKSSL_ENABLE_ECDSA must be set
 */
#ifndef SHARKSSL_ENABLE_ECDHE_ECDSA
#define SHARKSSL_ENABLE_ECDHE_ECDSA                      1
#endif


/** Enabling big integer assembler library requires SharkSslBigInt_XX.s
 */
#ifndef SHARKSSL_OPTIMIZED_BIGINT_ASM
#define SHARKSSL_OPTIMIZED_BIGINT_ASM                    0
#endif

/** Enabling assembler optimized CHACHA requires SharkSslCrypto_XX.s
 */
#ifndef SHARKSSL_OPTIMIZED_CHACHA_ASM
#define SHARKSSL_OPTIMIZED_CHACHA_ASM                    0
#endif

/** Enabling assembler optimized POLY requires SharkSslCrypto_XX.s
 */
#ifndef SHARKSSL_OPTIMIZED_POLY1305_ASM
#define SHARKSSL_OPTIMIZED_POLY1305_ASM                  0
#endif


/** Setting this macro to 1 enables TINYMT32 and disables other RNG's
 *  Please notice that the TinyMT is not recommended for cryptographic applications
 *  The SharkSSL implementation passed anyway the bbattery_FIPS_140_2 test
 *  of TestU01 (http://simul.iro.umontreal.ca/testu01/tu01.html)
 *  http://simul.iro.umontreal.ca/testu01/tu01.html
 */
#ifndef SHARKSSL_USE_RNG_TINYMT
#define SHARKSSL_USE_RNG_TINYMT                          0
#endif

/** Setting this macro to 1 enables Fortuna RNG's
 *  Suitable to cryptographic applications
 *  SHARKSSL_USE_RNG_TINYMT must be disabled
 *  AES 256 and SHA 256 must be enabled
 *  The SharkSSL implementation passed the bbattery_FIPS_140_2 test
 *  of TestU01 (http://simul.iro.umontreal.ca/testu01/tu01.html)
 */
#ifndef SHARKSSL_USE_RNG_FORTUNA
#define SHARKSSL_USE_RNG_FORTUNA                         0
#endif


/** Setting this macro to 1 enables the usage of sharkssl_rng
 *  in a multithreaded environment. Please remember to initialize
 *  the RNG by calling sharkssl_entropy at least once
 *  before calling sharkssl_rng
 */
#ifndef SHARKSSL_RNG_MULTITHREADED
#define SHARKSSL_RNG_MULTITHREADED                       1
#endif

/** Do not pack option */
#ifndef SHARKSSL_NOPACK
#define SHARKSSL_NOPACK                                  0
#endif

/** SharkSslCon_trusted also checks certificate expiration and returns
    SharkSslConTrust_CertCnDate if date(s) are within: timeFrom <= now
    and timeTo >= now
    This setting requires baGetUnixTime() returning the correct time.
*/
#ifdef __DOXYGEN__
#define SHARKSSL_CHECK_DATE                               0
#endif
#ifndef SHARKSSL_CHECK_DATE
#ifdef SHARKSSL_BA /* if SharkSSL embedded in BAS */
#define SHARKSSL_CHECK_DATE                               1
#else
#define SHARKSSL_CHECK_DATE                               0
#endif
#endif


/** @} */ /* end group SharkSslCfg */

#endif
