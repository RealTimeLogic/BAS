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
 *   $Id: SharkSSL_cfg.h 6759 2026-11-05 21:18:27Z gianluca $
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

 Do not directly edit the options in this file. Instead, add your
 custom options in SharkSSL_opts.h

 */
#ifndef _SharkSsl_cfg_h
#define _SharkSsl_cfg_h

/* BAS/BWS supplies the byte order through BaConfigEndian.h. */
#ifndef _BaConfigEndian_h
#include "SharkSslConfigEndian.h"
#else
#include "SharkSSL_opts.h"
#endif


/** @addtogroup SharkSslCfg
@{
*/


/**
 * TLS 1.3 stack
 * Disable by setting SHARKSSL_TLS_1_3=0
 */
#ifndef SHARKSSL_TLS_1_3
#define SHARKSSL_TLS_1_3                                 1
#endif

/**
 * TLS 1.2 stack
 * Disable by setting SHARKSSL_TLS_1_2=0
 */
#ifndef SHARKSSL_TLS_1_2
#define SHARKSSL_TLS_1_2                                 1
#endif

/**
 * Number of volatile key slots in the SharkSSL PSA provider. In the default
 * static mode, all slots reserve memory even when unused; in dynamic mode,
 * this value limits the number of allocated slots. Twelve covers one TLS 1.3
 * RSA/X25519 handshake with session caching and PSA secret handles, including
 * a subsequent KeyUpdate. Increase it for concurrent handshakes or other
 * keys kept alive by the application. This does not configure an external
 * PSA provider used by SHARKSSL_PSA_CONSUMER.
 */
#ifndef SHARKSSL_PSA_KEY_SLOTS
#define SHARKSSL_PSA_KEY_SLOTS                           12
#endif

/**
 * Select 1 to allocate SharkSSL PSA provider key slots on demand. The
 * static mode remains the default for deterministic embedded memory use.
 */
#ifndef SHARKSSL_PSA_DYNAMIC_KEY_SLOTS
#define SHARKSSL_PSA_DYNAMIC_KEY_SLOTS                   0
#endif


/** \defgroup SharkSslCfgPsa PSA consumer
\ingroup SharkSslCfg
@{
*/

/**
 * Select 1 to use an external PSA Crypto provider for TLS operations.
 */
#ifndef SHARKSSL_PSA_CONSUMER
#define SHARKSSL_PSA_CONSUMER                            0
#endif

/**
 * Select 1 to enable standalone AES through the external PSA provider.
 * Link SharkSslPSAConsumerAES.c when enabled.
 */
#ifndef SHARKSSL_PSA_CONSUMER_STANDALONE_AES
#define SHARKSSL_PSA_CONSUMER_STANDALONE_AES             0
#endif

/** Enable standalone ChaCha20 through the external PSA provider. */
#ifndef SHARKSSL_PSA_CONSUMER_STANDALONE_CHACHA20
#define SHARKSSL_PSA_CONSUMER_STANDALONE_CHACHA20        0
#endif

/**
 * Select 1 to keep TLS 1.3 handshake and traffic secrets in PSA key handles.
 * Requires SHARKSSL_PSA_CONSUMER=1, TLS 1.3, and a provider supporting HKDF
 * derivation from key handles, including a key handle as the HKDF salt.
 * TLS 1.2 uses PSA master and record key handles independently of this option.
 * The default consumer path remains available for other PSA providers.
 */
#ifndef SHARKSSL_PSA_CONSUMER_SECRET_HANDLES
#define SHARKSSL_PSA_CONSUMER_SECRET_HANDLES             0
#endif

/**
 * Select 1 to require a PSA signing key for every local TLS identity.
 * Certificate blobs must contain only the public key. The provider key's
 * public half and signing policy are checked when adding the certificate.
 */
#ifndef SHARKSSL_PSA_CONSUMER_STRICT_IDENTITY
#define SHARKSSL_PSA_CONSUMER_STRICT_IDENTITY            0
#endif

/** @} */ /* end group SharkSslCfgPsa */

/**
 * Select 1 to enforce the compile-time feature set required by the general
 * RFC 9846 profile. This verifies configuration prerequisites only; it is not
 * a certification statement and does not replace protocol conformance tests.
 */
#ifndef SHARKSSL_RFC_9846_CONFORMANCE_PROFILE
#define SHARKSSL_RFC_9846_CONFORMANCE_PROFILE            0
#endif

/**
 * Enable/disable AES 256
 */
#ifndef SHARKSSL_USE_AES_256
#define SHARKSSL_USE_AES_256                             1
#endif

/**
 * Enable/disable AES 128
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
 * Software AES constant-time protection is enabled by default.
 * Setting either software CT option to 0 can expose secret-dependent table
 * accesses and timing. Disable these protections at your own risk, after
 * assessing the selected backend and the target's side-channel exposure.
 * The assembler helper options below accelerate the CT implementation;
 * they do not disable its protections.
 */

/**
 * Select fixed-access bitsliced software AES for SharkSslAesCtx: the
 * constructor, block encryption and decryption, and the modes built on them
 * (CBC, CTR, CCM, the PSA AES ciphers and key wrap).
 * The AES lookup tables are then not linked. CTR, CBC decryption and CCM
 * process two blocks per bitsliced call. The context layout and public API
 * stay unchanged. AES-GCM keeps its own option: also select
 * SHARKSSL_AES_GCM_SOFTWARE_CT for a fixed-access GHASH. Not supported with
 * the x86/x64 and ARM64 assembler GCM, which reads FIPS 197 round keys.
 */
#ifndef SHARKSSL_AES_SOFTWARE_CT
#define SHARKSSL_AES_SOFTWARE_CT                         1
#endif

/**
 * Select fixed-access software AES encryption and GHASH for AES-GCM.
 * The AES-GCM context layout and public API stay unchanged. Other AES modes
 * retain their selected backend. Hardware or assembler GCM backends use
 * their own security properties. The option is enabled by default and
 * uses more cycles and stack. SHARKSSL_AES_CIPHER_LOOP_UNROLL also
 * selects the faster (1) or the smaller (0) code for this option.
 */
#ifndef SHARKSSL_AES_GCM_SOFTWARE_CT
#define SHARKSSL_AES_GCM_SOFTWARE_CT                     1
#endif

/**
 * Select the Cortex-M4/M7/M85 GHASH helper for software CT AES-GCM.
 * Compile crypto/Cortex-M_GCC/SharkSslAesGcmCT_M4.S (GCC) or
 * crypto/Cortex-M_IAR/SharkSslAesGcmCT_M4.s (IAR) with the same options.
 * This uses fixed-latency UMULL on the supported little-endian cores.
 * Cortex-M3 is not supported. The public AES-GCM context is unchanged.
 */
#ifndef SHARKSSL_AES_GCM_CT_M4_ASM
#define SHARKSSL_AES_GCM_CT_M4_ASM                       0
#endif

/**
 * Select the Cortex-M85 Helium (MVE) GHASH for software CT AES-GCM,
 * instead of SHARKSSL_AES_GCM_CT_M4_ASM. Compile
 * crypto/Cortex-M_GCC/SharkSslAesGcmCT_MVE.S (GCC) or
 * crypto/Cortex-M_IAR/SharkSslAesGcmCT_MVE.s (IAR) with the same options.
 * The core must implement MVE and the platform must enable CP10/CP11 in
 * CPACR before AES-GCM is used. The public AES-GCM context is unchanged.
 */
#ifndef SHARKSSL_AES_GCM_CT_MVE_ASM
#define SHARKSSL_AES_GCM_CT_MVE_ASM                      0
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

/**
 * Enable/disable CHACHA20 support and also include
 * CHACHA20-POLY1305 ciphersuites when TLS1.2 and POLY1305 are enabled
 * (#SHARKSSL_USE_POLY1305)
 */
#ifndef SHARKSSL_USE_CHACHA20
#define SHARKSSL_USE_CHACHA20                            1
#endif


/** \defgroup SharkSslCfgHash HASH algorithms
\ingroup SharkSslCfg
@{
*/


/**
 * Enable/disable SHA256 support for certificate signatures (SHA256
 * ciphersuites are not included).
 * SHA256 must be included (mandatory) for TLS 1.2 and TLS 1.3
 */
#ifndef SHARKSSL_USE_SHA_256
#define SHARKSSL_USE_SHA_256                             1
#endif

/**
 * Enable standalone SHA-224 through the PSA consumer. The native backend has
 * no SHA-224 API, so this option is disabled by default.
 */
#ifndef SHARKSSL_USE_SHA_224
#define SHARKSSL_USE_SHA_224                             0
#endif

/**
 * Enable/disable SHA384 support and also include
 * SHA384 ciphersuites
 */
#ifndef SHARKSSL_USE_SHA_384
#define SHARKSSL_USE_SHA_384                             1
#endif

/**
 * Enable/disable SHA512 support;
 * Note SHA512 is not required by SSL/TLS.
 */
#ifndef SHARKSSL_USE_SHA_512
#define SHARKSSL_USE_SHA_512                             0
#endif

/**
 * SHA1 is not used by the SharkSSL TLS stack unless you use RSA OAEP
 * (public RSA API). SHA1 is used by some examples. You can disable
 * SHA1 if you are not using RSA OAEP and/or the examples using SHA1
 */
#ifndef SHARKSSL_USE_SHA1
#define SHARKSSL_USE_SHA1                                0
#endif

/**
 * MD5 must be enabled to support PCKS1-encoded certificates (public PEM API).
 */
#ifndef SHARKSSL_USE_MD5
#define SHARKSSL_USE_MD5                                 0
#endif

/**
 * Enable/disable POLY1305 support and also include
 * CHACHA20-POLY1305 ciphersuites when TLS1.2 and CHACHA20 are enabled
 * (#SHARKSSL_USE_CHACHA20)
 */
#ifndef SHARKSSL_USE_POLY1305
#define SHARKSSL_USE_POLY1305                            1
#endif


/** @} */ /* end group SharkSslCfgHash */


/**
 * Select 1 to enable SERVER side TLS
 */
#ifndef SHARKSSL_SSL_SERVER_CODE
#define SHARKSSL_SSL_SERVER_CODE                         1
#endif

/**
 * Select 1 to enable client authentication from server
 */
#ifndef SHARKSSL_ENABLE_CLIENT_AUTH
#define SHARKSSL_ENABLE_CLIENT_AUTH                      1
#endif

/**
 * Select 1 to enable support for Post Handshake Authentication
 * (RFC 8446 sections 4.2.6 and 4.6.2)
 * Note: #SHARKSSL_TLS_1_3 and #SHARKSSL_ENABLE_CLIENT_AUTH must be enabled
 * ========================================================================
 * EXPERIMENTAL! Application encrypted data returned by SharkSslCon_encrypt()
 * must be sent before calling SharkSslCon_decrypt() again.
 */
#ifndef SHARKSSL_ENABLE_POST_HANDSHAKE_AUTH
#define SHARKSSL_ENABLE_POST_HANDSHAKE_AUTH              0
#endif

/**
 * Maximum accepted body length of one incoming TLS handshake message.
 * The 16-KiB default preserves the embedded memory profile. Host builds may
 * select, for example, 0x00040000UL (256 KiB); 0x00FFFFFFUL enables the full
 * uint24 range defined by RFC 9846 section 4. Values above the default widen
 * three internal buffer counters from U16 to U32. Input-buffer expansion must
 * also be enabled for those values.
 */
#ifndef SHARKSSL_MAX_HANDSHAKE_LENGTH
#define SHARKSSL_MAX_HANDSHAKE_LENGTH                    0x00004000UL
#endif

/**
 * Compile TLS 1.3 HelloRetryRequest handling. It is enabled by default when
 * TLS 1.3 is enabled. Set this to 0 to omit the retry path on constrained
 * targets; an unsupported HelloRetryRequest is rejected explicitly.
 */
#ifndef SHARKSSL_ENABLE_HELLO_RETRY_REQUEST
#define SHARKSSL_ENABLE_HELLO_RETRY_REQUEST              SHARKSSL_TLS_1_3
#endif

/**
 * Select 1 to send only an X25519 key share in the initial TLS 1.3 ClientHello.
 * This setting is effective only when HelloRetryRequest and X25519 are enabled;
 * otherwise the client sends every available TLS 1.3 key share as before.
 * The PSA consumer sends one P-256 share when X25519 is disabled.
 */
#ifndef SHARKSSL_TLS_1_3_INITIAL_KEY_SHARE_X25519_ONLY
#define SHARKSSL_TLS_1_3_INITIAL_KEY_SHARE_X25519_ONLY   0
#endif

/**
 * Select one RFC 8734 Brainpool group as the initial TLS 1.3 key share
 * Set to TLS_NAMEDGROUP_BRAINPOOLP256R1TLS13, P384R1TLS13, or P512R1TLS13
 * Zero retains the usual initial key shares
 */
#ifndef SHARKSSL_TLS_1_3_INITIAL_BRAINPOOL_GROUP
#define SHARKSSL_TLS_1_3_INITIAL_BRAINPOOL_GROUP         0
#endif

/**
 * Select 1 to enable TLS 1.3 KeyUpdate support (RFC 9846 section 4.7.3;
 * RFC 8446 section 4.6.3).
 * Note: with the default define below, it is enabled whenever TLS 1.3 is.
 * Disabling this option reduces the per-connection RAM and code footprint,
 * but receiving a TLS 1.3 peer KeyUpdate then closes the connection with a
 * fatal unexpected_message alert. The resulting constrained profile is not
 * fully TLS 1.3 conformant.
 */
#ifndef SHARKSSL_ENABLE_KEY_UPDATE
#define SHARKSSL_ENABLE_KEY_UPDATE                       SHARKSSL_TLS_1_3
#endif

/**
 * Number of TLS 1.3 records that may be encrypted with one AES-GCM key before
 * SharkSSL proactively sends a KeyUpdate. Every record is counted as a full-size
 * record, which is conservative for smaller records. The value may be reduced,
 * but must not exceed the default 2^23-record threshold; this leaves ample room
 * below the RFC 9846 section 5.5 usage limit for a pending split write and the
 * KeyUpdate record itself.
 * This value is effective only when SHARKSSL_ENABLE_KEY_UPDATE is 1.
 */
#ifndef SHARKSSL_TLS_1_3_AES_GCM_KEY_UPDATE_THRESHOLD
#define SHARKSSL_TLS_1_3_AES_GCM_KEY_UPDATE_THRESHOLD    0x00800000UL
#endif

/**
 * Select 1 to enable CLIENT side TLS
 */
#ifndef SHARKSSL_SSL_CLIENT_CODE
#define SHARKSSL_SSL_CLIENT_CODE                         1
#endif

/**
 * Select 1 to randomize the order of extensions in ClientHello
 */
#ifndef SHARKSSL_RANDOMIZE_EXTENSIONS
#define SHARKSSL_RANDOMIZE_EXTENSIONS                    1
#endif

/**
 * Select 1 to enable support for Server Name Indication
 */
#ifndef SHARKSSL_ENABLE_SNI
#define SHARKSSL_ENABLE_SNI                              1
#endif

/**
 * Select 1 to enable support for Certificate Authorities extension
 * (RFC 8446 section 4.2.4)
 * Note: #SHARKSSL_TLS_1_3 must be enabled
 */
#ifndef SHARKSSL_ENABLE_CA_EXTENSION
#define SHARKSSL_ENABLE_CA_EXTENSION                     1
#endif

/**
 * Select 0 to disable RSA ciphersuites
 */
#ifndef SHARKSSL_ENABLE_RSA
#define SHARKSSL_ENABLE_RSA                              1
#endif

/**
 * Select 1 to enable session caching
 */
#ifndef SHARKSSL_ENABLE_SESSION_CACHE
#define SHARKSSL_ENABLE_SESSION_CACHE                    1
#endif

/**
 * Select 1 to enable renegotiation
 * Only secure renegotiation (RFC 5746) is supported
 * Note: with the default define below, it is enabled
 * whenever TLS 1.2 is
 * NOTE: IMPLEMENTED ONLY ON THE SERVER SIDE
 */
#ifndef SHARKSSL_ENABLE_SECURE_RENEGOTIATION
#define SHARKSSL_ENABLE_SECURE_RENEGOTIATION             SHARKSSL_TLS_1_2
#endif

/**
 * Enable legacy TLS 1.2 DHE_RSA ciphersuites.
 * RFC 10015 requires clients not to offer and servers not to select
 * these ciphersuites. Keep disabled for RFC 10015 compliance.
 */
#ifndef SHARKSSL_ENABLE_DHE_RSA
#define SHARKSSL_ENABLE_DHE_RSA                          0
#endif

/**
 * Minimum size in bits of the DHE_RSA prime p sent by a server: the
 * client aborts the handshake with an insufficient_security alert when
 * p is shorter. 2048 follows NIST SP 800-131A; 1024 is the lowest value
 * accepted, for legacy servers only (512- and 768-bit groups are broken).
 */
#ifndef SHARKSSL_DHE_MIN_PRIME_BITS
#define SHARKSSL_DHE_MIN_PRIME_BITS                      2048
#endif

/**
 * TLS 1.2/1.3 require SHA-256, do not modify the following settings
 * DES and ClientHello v2.0 are deprecated in TLS 1.2 - RFC5246
 */
#undef  SHARKSSL_USE_SHA_256
#define SHARKSSL_USE_SHA_256                             1

/**
 * Enable/disable the SharkSslCon_selectCiphersuite API
 */
#ifndef SHARKSSL_ENABLE_SELECT_CIPHERSUITE
#define SHARKSSL_ENABLE_SELECT_CIPHERSUITE               1
#endif

/**
 * Determine the number of ciphersuites that can be selected, in
 * decreasing order of preference; this value is only in effect if the
 * #SHARKSSL_ENABLE_SELECT_CIPHERSUITE is selected.
 */
#ifndef SHARKSSL_SELECT_CIPHERSUITE_LIST_DEPTH
#define SHARKSSL_SELECT_CIPHERSUITE_LIST_DEPTH           8
#endif

/**
 * Enable/disable ALPN API (support for ALPN extension, RFC 7301)
 */
#ifndef SHARKSSL_ENABLE_ALPN_EXTENSION
#define SHARKSSL_ENABLE_ALPN_EXTENSION                   1
#endif

/**
 * Enable/disable RSA API (sharkssl_RSA_public_encrypt,
 * sharkssl_RSA_private_decrypt, sharkssl_RSA_private_encrypt,
 * sharkssl_RSA_public_decrypt, SharkSslRSAKey_size)
 */
#ifndef SHARKSSL_ENABLE_RSA_API
#define SHARKSSL_ENABLE_RSA_API                          1
#endif

/**
 * Enable the native RSA key generator. PSA library builds enable this option.
 */
#ifndef SHARKSSL_ENABLE_RSAKEY_CREATE
#define SHARKSSL_ENABLE_RSAKEY_CREATE                   0
#endif

/**
 * Enable/disable PKCS1 padding in RSA API
 * (#SHARKSSL_ENABLE_RSA_API must be enabled)
 * note: always enabled when SSL client or server enabled
 */
#ifndef SHARKSSL_ENABLE_RSA_PKCS1
#define SHARKSSL_ENABLE_RSA_PKCS1                        1
#endif

/**
 * Enable/disable RSASSA-PSS padding in RSA API (RFC 8017)
 * (#SHARKSSL_ENABLE_RSA_API must be enabled to use RSASSA-PSS
 * through the RSA API)
 *
 * RSASSA-PSS is automatically enabled for RSA CertificateVerify
 * signatures when TLS 1.3 and RSA are enabled
 */
#ifndef SHARKSSL_ENABLE_RSASSA_PSS
#define SHARKSSL_ENABLE_RSASSA_PSS                       1
#endif

/**
 * Enable/disable OAEP padding in RSA API
 * (#SHARKSSL_ENABLE_RSA_API must be enabled)
 */
#ifndef SHARKSSL_ENABLE_RSA_OAEP
#define SHARKSSL_ENABLE_RSA_OAEP                         0
#endif

/**
 * Enable/disable ECDSA API (sharkssl_ECDSA_sign,
 * sharkssl_ECDSA_verify, SharkSslECDSA_siglen)
 */
#ifndef SHARKSSL_ENABLE_ECDSA_API
#define SHARKSSL_ENABLE_ECDSA_API                        1
#endif

/**
 * Disable ECDSA sign API functions (sharkssl_ECDSA_sign,
 * SharkSslECDSA_siglen) - effective only if ECDSA API is
 * compiled (#SHARKSSL_ENABLE_ECDSA_API must be enabled)
 * and no SSL/TLS library used (only RayCrypto); used to
 * achieve minimum code size
 */
#ifndef SHARKSSL_ECDSA_ONLY_VERIFY
#define SHARKSSL_ECDSA_ONLY_VERIFY                       0
#endif

/**
 * Select 1 to enable PEM certs/keys decoding
 * If RSA_API is enabled, then also the functions
 * sharkssl_PEM_to_RSAKey and SharkSslRSAKey_free are available
 * If ECDSA_API is enabled, then also the functions
 * sharkssl_PEM_to_ECCKey and SharkSslECCKey_free are available
 */
#ifndef SHARKSSL_ENABLE_PEM_API
#define SHARKSSL_ENABLE_PEM_API                          1
#endif

/**
 * Enable/disable support for encrypted PKCS#8 certificates
 * in sharkssl_PEM function
 * (requires SHARKSSL_ENABLE_AES_CBC)
 */
#ifndef SHARKSSL_ENABLE_ENCRYPTED_PKCS8_SUPPORT
#define SHARKSSL_ENABLE_ENCRYPTED_PKCS8_SUPPORT          0
#endif

/**
 * Enable/disable #SharkSslCon_getCiphersuite
 */
#ifndef SHARKSSL_ENABLE_INFO_API
#define SHARKSSL_ENABLE_INFO_API                         1
#endif

/**
 * Select 1 to enable certificate chain support
 */
#ifndef SHARKSSL_ENABLE_CERT_CHAIN
#define SHARKSSL_ENABLE_CERT_CHAIN                       1
#endif

/**
 * Select 1 to enable CA check
 * (client or server with client auth)
 */
#ifndef SHARKSSL_ENABLE_CA_LIST
#define SHARKSSL_ENABLE_CA_LIST                          1
#endif

/**
 * Select 1 to enable certificate storage
 */
#ifndef SHARKSSL_ENABLE_CERTSTORE_API
#define SHARKSSL_ENABLE_CERTSTORE_API                    1
#endif

/**
 * Select 1 to enable #SharkSslCertInfo_parseCert for local DER certificates.
 * Requires RSA or ECDSA and certificate parsing for TLS, CSR signing, or tools.
 */
#ifndef SHARKSSL_ENABLE_CERTINFO_PARSE_API
#define SHARKSSL_ENABLE_CERTINFO_PARSE_API               0
#endif

/**
 * Automatic certificate cloning - always enabled
 */
#ifdef SHARKSSL_ENABLE_CLONE_CERTINFO
#if (!SHARKSSL_ENABLE_CLONE_CERTINFO)
#error SHARKSSL_ENABLE_CLONE_CERTINFO is now enabled by default - please remove its #define
#endif
#endif

/**
 * Select 1 to expose parsed KeyUsage and ExtendedKeyUsage information;
 * TLS 1.3 always parses KeyUsage to enforce certificate signing policy.
 */
#ifndef SHARKSSL_ENABLE_CERT_KEYUSAGE
#define SHARKSSL_ENABLE_CERT_KEYUSAGE                    0
#endif

/**
 * Select 1 (small ROM footprint, slow) or 0 (large, fast)
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

/**
 * Select 1 for smaller, but slower SHA256
 */
#ifndef SHARKSSL_SHA256_SMALL_FOOTPRINT
#define SHARKSSL_SHA256_SMALL_FOOTPRINT                  0
#endif

/**
 * Select a window size between 1 (slower, less RAM) and 5
 */
#ifndef SHARKSSL_BIGINT_EXP_SLIDING_WINDOW_K
#define SHARKSSL_BIGINT_EXP_SLIDING_WINDOW_K             4
#endif

/**
 * Select 0 (slower, less ROM) or 1 (20% faster, more ROM)
 */
#ifndef SHARKSSL_BIGINT_MULT_LOOP_UNROLL
#define SHARKSSL_BIGINT_MULT_LOOP_UNROLL                 1
#endif

/**
 * Select 1 to include AES CTR mode (USE_AES_xxx must be enabled)
 */
#ifndef SHARKSSL_ENABLE_AES_CTR_MODE
#define SHARKSSL_ENABLE_AES_CTR_MODE                     1
#endif

/**
 * Select 0 (35% less ROM) or 1 (10-15% faster)
 */
#ifndef SHARKSSL_AES_CIPHER_LOOP_UNROLL
#define SHARKSSL_AES_CIPHER_LOOP_UNROLL                  1
#endif

/**
 * Select 1 if your architecture supports unaligned memory
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
 * Elliptic Curve Cryptography
 */
#ifndef SHARKSSL_USE_ECC
#define SHARKSSL_USE_ECC                                 1
#endif

/**
 * Select 1 to enable generation and verification of
 * elliptic curve digital signatures
 */
#ifndef SHARKSSL_ENABLE_ECDSA
#define SHARKSSL_ENABLE_ECDSA                            1
#endif

/**
 * Select 1 to verify that a point lies on a curve;
 * TLS 1.3 always enables this verification for peer key shares.
 * Verification requires more ROM for curve parameters and code,
 * and slightly increases execution time.
 */
#ifndef SHARKSSL_ECC_VERIFY_POINT
#define SHARKSSL_ECC_VERIFY_POINT                        1
#endif

/**
 * Enable Ed25519/Ed448 support (RFC 8032).
 * Certificate parsing is available; signature verification currently
 * requires the PSA consumer. Native verification and TLS signature
 * scheme integration are not implemented.
 */
#ifndef SHARKSSL_ENABLE_EDDSA
#define SHARKSSL_ENABLE_EDDSA                            0
#endif

/**
 * Compute k*G, for G the base point of SECP256R1 and SECP384R1 (key
 * generation, ECDSA signing), with a fixed-base comb and a table of 16
 * precomputed points per curve in flash: about 1 KB for SECP256R1 and
 * 1.5 KB for SECP384R1 with 32-bit words. ECDSA signing is 2.4 to 3.3
 * times faster than with the generic multiplication, with the same
 * timing resistance. Set it to 0 to save the tables and about 3 KB of
 * code.
 */
#ifndef SHARKSSL_ECC_FIXED_BASE_COMB
#define SHARKSSL_ECC_FIXED_BASE_COMB                     1
#endif

/**
 * Compute k*Q for any other point Q of SECP256R1, SECP384R1 and
 * SECP521R1 (ECDH shared secret) with a fixed window of this many bits
 * and the same timing resistance: about 35% faster than the generic
 * multiplication. The window holds 2^(bits - 1) precomputed points in
 * RAM during the multiplication: with 4, 768 bytes for SECP256R1, 1152
 * for SECP384R1 and 1632 for SECP521R1; 5 doubles them for 0 to 4% more
 * speed. Set it to 0 to use the generic multiplication, which needs
 * less RAM and about 2.5 KB less code.
 */
#ifndef SHARKSSL_ECC_MULT_FIXED_WINDOW
#define SHARKSSL_ECC_MULT_FIXED_WINDOW                   4
#endif

/**
 * Enable/disable the SECP256R1 curve
 */
#ifndef SHARKSSL_ECC_USE_SECP256R1
#define SHARKSSL_ECC_USE_SECP256R1                       1
#endif

/**
 * Enable/disable the SECP384R1 curve
 */
#ifndef SHARKSSL_ECC_USE_SECP384R1
#define SHARKSSL_ECC_USE_SECP384R1                       1
#endif

/**
 * Enable/disable the SECP521R1 curve
 */
#ifndef SHARKSSL_ECC_USE_SECP521R1
#define SHARKSSL_ECC_USE_SECP521R1                       1
#endif

/**
 * Enable/disable the brainpoolP256r1 curve (RFC 5639)
 */
#ifndef SHARKSSL_ECC_USE_BRAINPOOLP256R1
#define SHARKSSL_ECC_USE_BRAINPOOLP256R1                 1
#endif

/**
 * Enable/disable the brainpoolP384r1 curve (RFC 5639)
 */
#ifndef SHARKSSL_ECC_USE_BRAINPOOLP384R1
#define SHARKSSL_ECC_USE_BRAINPOOLP384R1                 1
#endif

/**
 * Enable/disable the brainpoolP512r1 curve (RFC 5639)
 */
#ifndef SHARKSSL_ECC_USE_BRAINPOOLP512R1
#define SHARKSSL_ECC_USE_BRAINPOOLP512R1                 1
#endif

/**
 * Enable/disable the Curve25519 curve (RFC 7748)
 */
#ifndef SHARKSSL_ECC_USE_CURVE25519
#define SHARKSSL_ECC_USE_CURVE25519                      1
#endif

/**
 * Enable/disable the X25519 public API
 * (sharkssl_X25519_createKeyPair, sharkssl_X25519_sharedSecret)
 */
#ifndef SHARKSSL_ENABLE_X25519_API
#define SHARKSSL_ENABLE_X25519_API                       1
#endif

/**
 * Select 1 to use dedicated Curve25519 field arithmetic
 * (SharkSslBigInt.c) modulo p = 2^255 - 19, with a fixed inversion
 * chain. If \ref SHARKSSL_X25519_ASM is also enabled, the dedicated
 * whole-ASM backend is used instead. Select 0 for the generic BigInt
 * ladder, which may use optional BigInt assembly kernels. This will
 * reduce code size in flash at the cost of slower X25519 operations.
 * This option has no effect unless Curve25519
 * (#SHARKSSL_ECC_USE_CURVE25519) is enabled.
 */
#ifndef SHARKSSL_X25519_DEDICATED
#define SHARKSSL_X25519_DEDICATED                        1
#endif

/**
 * Enable/disable the Curve448 curve (RFC 7748)
 */
#ifndef SHARKSSL_ECC_USE_CURVE448
#define SHARKSSL_ECC_USE_CURVE448                        0
#endif

/**
 * Select 1 to enable ECDHE_RSA ciphersuites (RFC 4492)
 * Elliptic Curve Cryptography (#SHARKSSL_USE_ECC) must be enabled
 * RSA (#SHARKSSL_ENABLE_RSA) must be enabled
 */
#ifndef SHARKSSL_ENABLE_ECDHE_RSA
#define SHARKSSL_ENABLE_ECDHE_RSA                        1
#endif

/**
 * Select 1 to enable ECDHE_ECDSA ciphersuites (RFC 4492)
 * Elliptic Curve Cryptography (#SHARKSSL_USE_ECC) must be enabled
 * SHARKSSL_ENABLE_ECDSA must be set
 */
#ifndef SHARKSSL_ENABLE_ECDHE_ECDSA
#define SHARKSSL_ENABLE_ECDHE_ECDSA                      1
#endif

/**
 * Enabling big integer assembler library requires SharkSslBigInt_XX.s
 */
#ifndef SHARKSSL_OPTIMIZED_BIGINT_ASM
#define SHARKSSL_OPTIMIZED_BIGINT_ASM                    0
#endif

/**
 * Select an assembly backend for BigInt add, subtract and right shift only.
 * Multiplication and Montgomery product remain in SharkSslBigInt.c. The
 * MIPS32 big-endian GCC backend uses this option; it is independent of the
 * all-BigInt assembler option above and must not be combined with it.
 */
#ifndef SHARKSSL_OPTIMIZED_BIGINT_WORD_ASM
#define SHARKSSL_OPTIMIZED_BIGINT_WORD_ASM               0
#endif

/**
 * Select 1 to use the assembly for the P-256 (secp256r1) field arithmetic:
 * on Cortex-M4/M7 SharkSslP256_M4.S for GCC, SharkSslP256_M4.s for IAR
 * and Keil; on RXv2/RXv3 SharkSslP256_RX.S for GCC, SharkSslP256_RX.s
 * for IAR; on AArch64 SharkSslP256_ARM64.S for GCC. Multiplication, addition and subtraction modulo the P-256
 * prime then run in assembly and in constant time; the other curves are
 * not affected. Independent of #SHARKSSL_OPTIMIZED_BIGINT_ASM.
 */
#ifndef SHARKSSL_ECC_P256_ASM
#define SHARKSSL_ECC_P256_ASM                            0
#endif

/**
 * Select 1 to use the assembly for the P-384 (secp384r1) field arithmetic:
 * on Cortex-M4/M7 SharkSslP384_M4.S for GCC, SharkSslP384_M4.s for IAR
 * and Keil; on RXv2/RXv3 SharkSslP384_RX.S for GCC, SharkSslP384_RX.s
 * for IAR; on AArch64 SharkSslP384_ARM64.S for GCC. Multiplication, addition and subtraction modulo the P-384
 * prime then run in assembly and in constant time; the other curves are
 * not affected. Independent of #SHARKSSL_ECC_P256_ASM and
 * #SHARKSSL_OPTIMIZED_BIGINT_ASM.
 */
#ifndef SHARKSSL_ECC_P384_ASM
#define SHARKSSL_ECC_P384_ASM                            0
#endif

/**
 * Enabling assembler optimized CHACHA requires SharkSslCrypto_XX.s
 */
#ifndef SHARKSSL_OPTIMIZED_CHACHA_ASM
#define SHARKSSL_OPTIMIZED_CHACHA_ASM                    0
#endif

/**
 * Enabling assembler optimized POLY requires SharkSslCrypto_XX.s
 */
#ifndef SHARKSSL_OPTIMIZED_POLY1305_ASM
#define SHARKSSL_OPTIMIZED_POLY1305_ASM                  0
#endif

/**
 * Enabling assembler optimized SHA-256 requires SharkSslCrypto_XX.s
 */
#ifndef SHARKSSL_OPTIMIZED_SHA256_ASM
#define SHARKSSL_OPTIMIZED_SHA256_ASM                    0
#endif

/**
 * Enabling assembler optimized SHA-384 and SHA-512 requires
 * SharkSslCrypto_XX.s
 */
#ifndef SHARKSSL_OPTIMIZED_SHA384_ASM
#define SHARKSSL_OPTIMIZED_SHA384_ASM                    0
#endif

/**
 * Enabling assembler optimized GHASH requires SharkSslCrypto_XX.s
 */
#ifndef SHARKSSL_OPTIMIZED_GHASH_ASM
#define SHARKSSL_OPTIMIZED_GHASH_ASM                     0
#endif

/**
 * Enabling the combined assembler AES-GCM block kernel requires
 * SharkSslCrypto_XX.s. GHASH may use either its C or assembler implementation.
 */
#ifndef SHARKSSL_OPTIMIZED_GCM_ASM
#define SHARKSSL_OPTIMIZED_GCM_ASM                       0
#endif

/**
 * Enabling the VAES/VPCLMULQDQ AES-GCM backend requires the VAES
 * assembly file (SharkSslCrypto_X64_VAES.asm) and its CPUID probe to
 * be linked in addition to the AES-NI kernel. Microsoft x64 only.
 */
#ifndef SHARKSSL_OPTIMIZED_GCM_VAES_ASM
#define SHARKSSL_OPTIMIZED_GCM_VAES_ASM                  0
#endif

/**
 * The native cryptographic RNG is SHA-256 Hash_DRBG (SP 800-90A Rev. 1).
 * PSA consumers use the selected provider's RNG instead. There is no
 * separate algorithm selector for the native generator.
 */
#if ((defined(SHARKSSL_USE_RNG_TINYMT) && SHARKSSL_USE_RNG_TINYMT) || (defined(SHARKSSL_USE_RNG_FORTUNA) && SHARKSSL_USE_RNG_FORTUNA))
#error TinyMT and Fortuna have been removed; native SharkSSL uses SHA-256 Hash_DRBG
#endif


/**
 * Setting this macro to 1 enables native sharkssl_rng use in a multithreaded
 * environment. Initialize the native RNG with at least 48 unpredictable
 * bytes through sharkssl_entropyEx before starting worker threads.
 */
#ifndef SHARKSSL_RNG_MULTITHREADED
#define SHARKSSL_RNG_MULTITHREADED                       1
#endif

/**
 * Do not pack option
 */
#ifndef SHARKSSL_NOPACK
#define SHARKSSL_NOPACK                                  0
#endif

/**
 * SharkSslCon_trusted also checks certificate expiration and returns
 * SharkSslConTrust_CertCnDate if date(s) are within: timeFrom <= now
 * and timeTo >= now
 * This setting requires baGetUnixTime() returning the correct time.
 */
#ifdef __DOXYGEN__
#define SHARKSSL_CHECK_DATE                              0
#endif
#ifndef SHARKSSL_CHECK_DATE
#ifdef SHARKSSL_BA /* if SharkSSL embedded in BAS */
#define SHARKSSL_CHECK_DATE                              1
#else
#define SHARKSSL_CHECK_DATE                              0
#endif
#endif


/** @} */ /* end group SharkSslCfg */


/**
 * sanity defines --- do not edit below this line!
 */
#if ((SHARKSSL_MAX_HANDSHAKE_LENGTH == 0) || (SHARKSSL_MAX_HANDSHAKE_LENGTH > 0x00FFFFFFUL))
#error SHARKSSL_MAX_HANDSHAKE_LENGTH must be in the range 1..0x00FFFFFF
#endif

#if (SHARKSSL_DHE_MIN_PRIME_BITS < 1024)
#error SHARKSSL_DHE_MIN_PRIME_BITS must be at least 1024
#endif

#if (SHARKSSL_OPTIMIZED_BIGINT_ASM && SHARKSSL_OPTIMIZED_BIGINT_WORD_ASM)
#error Select only one BigInt assembly backend
#endif

/**
 * The SHARKSSL_BIGINT_TIMING_RESISTANT option has been removed. Reject
 * configurations that still select its former variable-time mode.
 */
#if (defined(SHARKSSL_BIGINT_TIMING_RESISTANT) && (!SHARKSSL_BIGINT_TIMING_RESISTANT))
#error SHARKSSL_BIGINT_TIMING_RESISTANT=0 is no longer supported
#endif

/**
 * The SHARKSSL_ECC_TIMING_RESISTANT option has been removed: the ECC code
 * is always timing resistant. Reject configurations that still select its
 * former variable-time mode.
 */
#if (defined(SHARKSSL_ECC_TIMING_RESISTANT) && (!SHARKSSL_ECC_TIMING_RESISTANT))
#error SHARKSSL_ECC_TIMING_RESISTANT=0 is no longer supported
#endif

/**
 * TLS 1.3 sanity \#defines
 */
#if SHARKSSL_ENABLE_KEY_UPDATE
#if (!SHARKSSL_TLS_1_3)
#undef  SHARKSSL_ENABLE_KEY_UPDATE
#define SHARKSSL_ENABLE_KEY_UPDATE                       0
#elif SHARKSSL_ENABLE_AES_GCM
#if ((SHARKSSL_TLS_1_3_AES_GCM_KEY_UPDATE_THRESHOLD == 0) || (SHARKSSL_TLS_1_3_AES_GCM_KEY_UPDATE_THRESHOLD > 0x00800000UL))
#error SHARKSSL_TLS_1_3_AES_GCM_KEY_UPDATE_THRESHOLD must be in the range 1..2^23
#endif
#endif
#endif  /* SHARKSSL_ENABLE_KEY_UPDATE */

#if SHARKSSL_TLS_1_3
#if SHARKSSL_ENABLE_RSA
#undef  SHARKSSL_ENABLE_RSASSA_PSS
#define SHARKSSL_ENABLE_RSASSA_PSS                       1
#endif
#if (!SHARKSSL_TLS_1_2)
#if SHARKSSL_ENABLE_SECURE_RENEGOTIATION
#undef SHARKSSL_ENABLE_SECURE_RENEGOTIATION
#define SHARKSSL_ENABLE_SECURE_RENEGOTIATION             0
#endif
#if SHARKSSL_ENABLE_DHE_RSA
#undef SHARKSSL_ENABLE_DHE_RSA
#define SHARKSSL_ENABLE_DHE_RSA                          0
#endif
#endif
#if (0 == SHARKSSL_ECC_USE_SECP256R1)
#error TLS 1.3 requires NIST curve secp256r1
#endif
#if (0 == SHARKSSL_USE_AES_128)
#error TLS 1.3 requires AES 128
#endif
#if (0 == SHARKSSL_USE_SHA_256)
#error TLS 1.3 requires SHA 256
#endif
#endif  /* SHARKSSL_TLS_1_3 */

#if SHARKSSL_RFC_9846_CONFORMANCE_PROFILE
#if (!SHARKSSL_TLS_1_3)
#error SHARKSSL_RFC_9846_CONFORMANCE_PROFILE requires SHARKSSL_TLS_1_3
#endif
#if ((!SHARKSSL_SSL_CLIENT_CODE) && (!SHARKSSL_SSL_SERVER_CODE))
#error SHARKSSL_RFC_9846_CONFORMANCE_PROFILE requires a TLS client or server role
#endif
#if ((!SHARKSSL_USE_AES_128) || (!SHARKSSL_ENABLE_AES_GCM) || (!SHARKSSL_USE_SHA_256))
#error SHARKSSL_RFC_9846_CONFORMANCE_PROFILE requires TLS_AES_128_GCM_SHA256
#endif
#if ((!SHARKSSL_ENABLE_RSA) || (!SHARKSSL_ENABLE_RSA_PKCS1) || (!SHARKSSL_ENABLE_RSASSA_PSS))
#error SHARKSSL_RFC_9846_CONFORMANCE_PROFILE requires RSA PKCS1 and RSASSA-PSS SHA-256 signatures
#endif
#if ((!SHARKSSL_USE_ECC) || (!SHARKSSL_ENABLE_ECDSA) || (!SHARKSSL_ECC_USE_SECP256R1))
#error SHARKSSL_RFC_9846_CONFORMANCE_PROFILE requires ECDSA and ECDHE with secp256r1
#endif
#if (!SHARKSSL_ENABLE_SNI)
#error SHARKSSL_RFC_9846_CONFORMANCE_PROFILE requires Server Name Indication
#endif
#if (!SHARKSSL_ENABLE_HELLO_RETRY_REQUEST)
#error SHARKSSL_RFC_9846_CONFORMANCE_PROFILE requires HelloRetryRequest
#endif
#if (!SHARKSSL_ENABLE_KEY_UPDATE)
#error SHARKSSL_RFC_9846_CONFORMANCE_PROFILE requires TLS 1.3 KeyUpdate
#endif
#endif  /* SHARKSSL_RFC_9846_CONFORMANCE_PROFILE */

/* Check the selected PSA consumer profile after all TLS options are resolved. */
#if SHARKSSL_AES_GCM_CT_M4_ASM
#if (!SHARKSSL_AES_GCM_SOFTWARE_CT)
#error SHARKSSL_AES_GCM_CT_M4_ASM requires SHARKSSL_AES_GCM_SOFTWARE_CT
#endif
/**
 * GCC 13 identifies M85 as ARMv8-M Mainline (Clang 18 as ARMv8.1-M Mainline)
 * with the ARMv8.1-M BTI feature.
 * IAR identifies M4/M7 as __ARM7EM__ and M85 as __ARM8EM_MAINLINE__ with BTI.
 * This is an instruction-set check, not a requirement to enable BTI at run time.
 */
#if (defined(__ICCARM__) || defined(__IAR_SYSTEMS_ASM__))
#if ((!defined(__ARM_PROFILE_M__)) || ((__CORE__ != __ARM7EM__) && ((__CORE__ != __ARM8EM_MAINLINE__) || (!defined(__ARM_FEATURE_BTI)))) || (BA_BYTE_ORDER != BA_BYTE_ORDER_LITTLE_ENDIAN))
#error SHARKSSL_AES_GCM_CT_M4_ASM requires little-endian Cortex-M4/M7/M85 and GCC Thumb or IAR
#endif
#elif (((!defined(__ARM_ARCH_7EM__)) && (((!defined(__ARM_ARCH_8M_MAIN__)) && (!defined(__ARM_ARCH_8_1M_MAIN__))) || (!defined(__ARM_FEATURE_BTI)))) || (!defined(__GNUC__)) || (!defined(__thumb__)) || (BA_BYTE_ORDER != BA_BYTE_ORDER_LITTLE_ENDIAN))
#error SHARKSSL_AES_GCM_CT_M4_ASM requires little-endian Cortex-M4/M7/M85 and GCC Thumb or IAR
#endif
#endif
#if SHARKSSL_AES_GCM_CT_MVE_ASM
#if (!SHARKSSL_AES_GCM_SOFTWARE_CT)
#error SHARKSSL_AES_GCM_CT_MVE_ASM requires SHARKSSL_AES_GCM_SOFTWARE_CT
#endif
#if SHARKSSL_AES_GCM_CT_M4_ASM
#error SHARKSSL_AES_GCM_CT_MVE_ASM and SHARKSSL_AES_GCM_CT_M4_ASM are exclusive
#endif
/**
 * Only Cortex-M85 is accepted (see SHARKSSL_AES_GCM_CT_M4_ASM). GCC does not
 * define __ARM_FEATURE_MVE with -mfloat-abi=soft, so the C code is not
 * required to use MVE; the GCC assembly file enables MVE itself.
 */
#if (defined(__ICCARM__) || defined(__IAR_SYSTEMS_ASM__))
#if ((!defined(__ARM_PROFILE_M__)) || (__CORE__ != __ARM8EM_MAINLINE__) || (!defined(__ARM_FEATURE_BTI)) || (BA_BYTE_ORDER != BA_BYTE_ORDER_LITTLE_ENDIAN))
#error SHARKSSL_AES_GCM_CT_MVE_ASM requires little-endian Cortex-M85 and GCC Thumb or IAR
#endif
#elif (((!defined(__ARM_ARCH_8M_MAIN__)) && (!defined(__ARM_ARCH_8_1M_MAIN__))) || (!defined(__ARM_FEATURE_BTI)) || (!defined(__GNUC__)) || (!defined(__thumb__)) || (BA_BYTE_ORDER != BA_BYTE_ORDER_LITTLE_ENDIAN))
#error SHARKSSL_AES_GCM_CT_MVE_ASM requires little-endian Cortex-M85 and GCC Thumb or IAR
#endif
#endif
#if (SHARKSSL_AES_GCM_SOFTWARE_CT && (!SHARKSSL_ENABLE_AES_GCM))
#error SHARKSSL_AES_GCM_SOFTWARE_CT requires AES-GCM
#endif
#if (SHARKSSL_AES_GCM_SOFTWARE_CT && SHARKSSL_PSA_CONSUMER_STANDALONE_AES)
#error SHARKSSL_AES_GCM_SOFTWARE_CT requires the native AES context
#endif
#if (SHARKSSL_AES_SOFTWARE_CT && SHARKSSL_PSA_CONSUMER_STANDALONE_AES)
#error SHARKSSL_AES_SOFTWARE_CT requires the native AES context
#endif
#if (SHARKSSL_AES_GCM_SOFTWARE_CT && (SHARKSSL_OPTIMIZED_GHASH_ASM || SHARKSSL_OPTIMIZED_GCM_ASM || SHARKSSL_OPTIMIZED_GCM_VAES_ASM))
#error SHARKSSL_AES_GCM_SOFTWARE_CT cannot be combined with GCM/GHASH assembler
#endif
#if (SHARKSSL_PSA_CONSUMER_STRICT_IDENTITY && (!SHARKSSL_PSA_CONSUMER))
#error SHARKSSL_PSA_CONSUMER_STRICT_IDENTITY requires the PSA consumer
#endif
#if (SHARKSSL_PSA_CONSUMER_SECRET_HANDLES && ((!SHARKSSL_PSA_CONSUMER) || (!SHARKSSL_TLS_1_3)))
#error SHARKSSL_PSA_CONSUMER_SECRET_HANDLES requires the TLS 1.3 PSA consumer
#endif
#if (SHARKSSL_PSA_CONSUMER_STANDALONE_AES && (!SHARKSSL_PSA_CONSUMER))
#error SHARKSSL_PSA_CONSUMER_STANDALONE_AES requires the PSA consumer
#endif
#if (SHARKSSL_PSA_CONSUMER_STANDALONE_CHACHA20 && ((!SHARKSSL_PSA_CONSUMER) || (!SHARKSSL_USE_CHACHA20)))
#error SHARKSSL_PSA_CONSUMER_STANDALONE_CHACHA20 requires the PSA consumer and ChaCha20
#endif
#if (SHARKSSL_USE_SHA_224 && (!SHARKSSL_PSA_CONSUMER))
#error SHARKSSL_USE_SHA_224 requires the PSA consumer
#endif

#if SHARKSSL_PSA_CONSUMER
#if ((!SHARKSSL_TLS_1_2) && (!SHARKSSL_TLS_1_3))
#error SHARKSSL_PSA_CONSUMER requires TLS 1.2 or TLS 1.3
#endif
#if ((!SHARKSSL_SSL_CLIENT_CODE) && (!SHARKSSL_SSL_SERVER_CODE))
#error SHARKSSL_PSA_CONSUMER requires a TLS client or server role
#endif
#if ((!SHARKSSL_USE_AES_128) || (!SHARKSSL_ENABLE_AES_GCM) || (!SHARKSSL_USE_SHA_256))
#error SHARKSSL_PSA_CONSUMER requires AES-128-GCM and SHA-256
#endif
#if ((!SHARKSSL_PSA_CONSUMER_STANDALONE_AES) && (SHARKSSL_USE_AES_192 || SHARKSSL_ENABLE_AES_CCM || SHARKSSL_ENABLE_AES_CBC || SHARKSSL_ENABLE_AES_CTR_MODE))
#error SHARKSSL_PSA_CONSUMER standalone AES modes require SHARKSSL_PSA_CONSUMER_STANDALONE_AES
#endif
#if (SHARKSSL_USE_CHACHA20 != SHARKSSL_USE_POLY1305)
#error SHARKSSL_PSA_CONSUMER requires ChaCha20 and Poly1305 together
#endif
#if SHARKSSL_USE_MD5
#error SHARKSSL_PSA_CONSUMER does not support MD5
#endif
#if ((!SHARKSSL_USE_ECC) || (!SHARKSSL_ECC_USE_SECP256R1) || (!SHARKSSL_ENABLE_ECDSA) || (!SHARKSSL_ENABLE_ECDHE_ECDSA))
#error SHARKSSL_PSA_CONSUMER requires ECDHE-ECDSA with P-256
#endif
#if (SHARKSSL_TLS_1_3 && (!SHARKSSL_ENABLE_HELLO_RETRY_REQUEST))
#error SHARKSSL_PSA_CONSUMER requires HelloRetryRequest
#endif
#if (SHARKSSL_TLS_1_3 && SHARKSSL_ECC_USE_CURVE25519 && (!SHARKSSL_TLS_1_3_INITIAL_KEY_SHARE_X25519_ONLY))
#error SHARKSSL_PSA_CONSUMER requires a single initial X25519 key share when X25519 is enabled
#endif
#if (SHARKSSL_ENABLE_RSA_API || SHARKSSL_ENABLE_RSA_OAEP || SHARKSSL_ENABLE_DHE_RSA)
#error SHARKSSL_PSA_CONSUMER excludes RSA encryption and DHE_RSA
#endif
#if (SHARKSSL_ENABLE_RSA && ((!SHARKSSL_ENABLE_RSA_PKCS1) || (!SHARKSSL_ENABLE_RSASSA_PSS)))
#error SHARKSSL_PSA_CONSUMER RSA requires PKCS1 certificate signatures and RSA-PSS
#endif
#if SHARKSSL_ENABLE_ECDSA_API
#error SHARKSSL_PSA_CONSUMER excludes standalone ECDSA
#endif
#if (SHARKSSL_ENABLE_POST_HANDSHAKE_AUTH && ((!SHARKSSL_ENABLE_CLIENT_AUTH) || (!SHARKSSL_SSL_SERVER_CODE)))
#error SHARKSSL_PSA_CONSUMER post-handshake auth requires client auth and server code
#endif
#endif  /* SHARKSSL_PSA_CONSUMER */

#endif
