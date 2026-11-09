/**
 *     ____             _________                __                _
 *    / __ \___  ____ _/ /_  __(_)___ ___  ___  / /   ____  ____ _(_)____
 *   / /_/ / _ \/ __ `/ / / / / / __ `__ \/ _ \/ /   / __ \/ __ `/ / ___/
 *  / _, _/  __/ /_/ / / / / / / / /  __/ /___/ /_/ / /_/ / /__
 * /_/ |_|\___/\__,_/_/ /_/ /_/_/ /_/ /_/\___/_____/\____/\__, /_/\___/
 *                                                       /____/
 *
 *                 SharkSSL Embedded SSL/TLS Stack
 ****************************************************************************
 *   PROGRAM MODULE
 *
 *   $Id: SharkSslPSAConsumerRSA.h 6818 2026-11-08 21:18:02Z gianluca $
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
 */

#ifndef SHARKSSL_PSA_CONSUMER_RSA_H
#define SHARKSSL_PSA_CONSUMER_RSA_H

#include "SharkSSL.h"

#if (!SHARKSSL_PSA_CONSUMER)
#error SharkSslPSAConsumerRSA.h requires SHARKSSL_PSA_CONSUMER
#endif

/**
 * An RSA key owned by the external PSA provider. Algorithm is either
 * PSA_ALG_RSA_PKCS1V15_CRYPT, PSA_ALG_RSA_OAEP(hash),
 * PSA_ALG_RSA_PKCS1V15_SIGN(hash), or PSA_ALG_RSA_PSS(hash) with standard
 * salt length
 * Private keys may be non-exportable; the public key can always be exported
 * A signing key can verify with its private handle when the provider supports it
 * Otherwise, verification uses a temporary public handle
 * Initialize once, then call destructor before reusing this context
 */
typedef struct SharkSslPSARSAKey
{
   psa_key_id_t handle;
   psa_algorithm_t alg;
   U16 bits;
   U8 publicOnly;
   U8 exportable;
} SharkSslPSARSAKey;

SHARKSSL_API int SharkSslPSARSAKey_generate(SharkSslPSARSAKey *o,
                                            psa_algorithm_t alg, int exportable, U16 bits);
SHARKSSL_API int SharkSslPSARSAKey_importPrivate(SharkSslPSARSAKey *o, const U8 *privateDer,
                                                 psa_algorithm_t alg, int exportable,
                                                 U16 privateDerLen);
SHARKSSL_API int SharkSslPSARSAKey_importPublic(SharkSslPSARSAKey *o, const U8 *publicDer,
                                                psa_algorithm_t alg, U16 publicDerLen);
SHARKSSL_API int SharkSslPSARSAKey_exportPrivate(const SharkSslPSARSAKey *o,
                                                 U8 *privateDer, U16 privateDerSize,
                                                 U16 *privateDerLen);
SHARKSSL_API int SharkSslPSARSAKey_exportPublic(const SharkSslPSARSAKey *o,
                                                U8 *publicDer, U16 publicDerSize,
                                                U16 *publicDerLen);
SHARKSSL_API int SharkSslPSARSAKey_encrypt(const SharkSslPSARSAKey *o, const U8 *plaintext,
                                           const U8 *label, U8 *ciphertext,
                                           U16 plaintextLen, U16 labelLen,
                                           U16 ciphertextSize, U16 *ciphertextLen);
SHARKSSL_API int SharkSslPSARSAKey_decrypt(const SharkSslPSARSAKey *o, const U8 *ciphertext,
                                           const U8 *label, U8 *plaintext,
                                           U16 ciphertextLen, U16 labelLen,
                                           U16 plaintextSize, U16 *plaintextLen);
SHARKSSL_API int SharkSslPSARSAKey_signHash(const SharkSslPSARSAKey *o, const U8 *hash,
                                            U8 *signature, U16 *signatureLen,
                                            U16 hashLen, U16 signatureSize);
SHARKSSL_API int SharkSslPSARSAKey_verifyHash(const SharkSslPSARSAKey *o, const U8 *hash,
                                              const U8 *signature, U16 hashLen,
                                              U16 signatureLen);
#if (SHARKSSL_ENABLE_CSR_CREATION && SHARKSSL_ENABLE_RSA)
/**
 * Create a PKCS#10 CSR with a non-exportable RSA private key
 * The key policy must select PKCS#1 v1.5 or standard-salt PSS with SHA-256,
 * SHA-384, or SHA-512
 * The public key is exported from the same PSA handle
 */
SHARKSSL_API int SharkSslASN1Create_CSRWithPSARSAKey(struct SharkSslASN1Create *o,
                                                     const SharkSslPSARSAKey *key,
                                                     struct SharkSslCertDN *certDN,
                                                     const char *SAN,
                                                     struct SharkSslBitExtReq *keyUsage,
                                                     struct SharkSslBitExtReq *nsCertType);
#endif
SHARKSSL_API int SharkSslPSARSAKey_destructor(SharkSslPSARSAKey *o);

#endif
