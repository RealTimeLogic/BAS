/**
 *     ____             _________                __                _
 *    / __ \___  ____ _/ /_  __(_)___ ___  ___  / /   ____  ____ _(_)____
 *   / /_/ / _ \/ __ `/ / / / / / __ `__ \/ _ \/ /   / __ \/ __ `/ / ___/
 *  / _, _/  __/ /_/ / / / / / / / / / /  __/ /___/ /_/ / /_/ / /__
 * /_/ |_|\___/\__,_/_/ /_/ /_/_/ /_/ /_/\___/_____/\____/\__, /_/\___/
 *                                                       /____/
 *
 *                 SharkSSL Embedded SSL/TLS Stack
 ****************************************************************************
 *   PROGRAM MODULE
 *
 *   $Id: SharkSslPSAConsumerECDSA.h 6818 2026-11-08 21:18:02Z gianluca $
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

#ifndef SHARKSSL_PSA_CONSUMER_ECDSA_H
#define SHARKSSL_PSA_CONSUMER_ECDSA_H

#include "SharkSSL.h"
#include "SharkSslPSAConsumerCurves.h"

#if !SHARKSSL_PSA_CONSUMER
#error SharkSslPSAConsumerECDSA.h requires SHARKSSL_PSA_CONSUMER
#endif

/**
 * A volatile PSA key owned by this context
 * Initialize once, then destroy before reusing it
 * Signatures are fixed-width raw r||s
 * Private signing keys can verify directly or through a temporary public PSA handle
 */
typedef struct SharkSslPSAEcdsaKey
{
   psa_key_id_t handle;
   psa_algorithm_t algorithm;
   U16 curveID;
   U8 publicOnly;
} SharkSslPSAEcdsaKey;

SHARKSSL_API int SharkSslPSAEcdsaKey_generate(SharkSslPSAEcdsaKey *o, U16 curveID,
                                              psa_algorithm_t hashAlg, int deterministic);
SHARKSSL_API int SharkSslPSAEcdsaKey_importPrivate(SharkSslPSAEcdsaKey *o, U16 curveID,
                                                   psa_algorithm_t hashAlg, int deterministic,
                                                   const U8 *privateKey, U16 privateKeyLen);
SHARKSSL_API int SharkSslPSAEcdsaKey_importPublic(SharkSslPSAEcdsaKey *o, U16 curveID,
                                                  psa_algorithm_t hashAlg, int deterministic,
                                                  const U8 *publicKey, U16 publicKeyLen);
SHARKSSL_API int SharkSslPSAEcdsaKey_exportPublic(const SharkSslPSAEcdsaKey *o,
                                                  U8 *publicKey, U16 publicKeySize, U16 *publicKeyLen);
SHARKSSL_API int SharkSslPSAEcdsaKey_signRaw(const SharkSslPSAEcdsaKey *o,
                                             const U8 *hash, U16 hashLen,
                                             U8 *signature, U16 signatureSize, U16 *signatureLen);
SHARKSSL_API int SharkSslPSAEcdsaKey_verifyRaw(const SharkSslPSAEcdsaKey *o,
                                               const U8 *hash, U16 hashLen,
                                               const U8 *signature, U16 signatureLen);
SHARKSSL_API int SharkSslPSAEcdsa_rawToDer(U16 curveID, const U8 *raw, U16 rawLen,
                                           U8 *der, U16 derSize, U16 *derLen);
SHARKSSL_API int SharkSslPSAEcdsa_derToRaw(U16 curveID, const U8 *der, U16 derLen,
                                           U8 *raw, U16 rawSize, U16 *rawLen);
#if SHARKSSL_ENABLE_CSR_CREATION
/**
 * Create a PKCS#10 CSR using a private P-256, P-384, or P-521 PSA handle
 * The handle must permit ECDSA signing with SHA-256, SHA-384, or SHA-512
 */
SHARKSSL_API int SharkSslASN1Create_CSRWithPSAECDSAKey(struct SharkSslASN1Create *o,
                                                       const SharkSslPSAEcdsaKey *key,
                                                       struct SharkSslCertDN *certDN,
                                                       const char *SAN,
                                                       struct SharkSslBitExtReq *keyUsage,
                                                       struct SharkSslBitExtReq *nsCertType);
/** 
 * Select a concrete digest when the signing key permits ECDSA(ANY_HASH).
 * The key must allow SIGN_HASH for the selected digest; ANY_HASH is never
 * passed as an operation algorithm.
 */
SHARKSSL_API int SharkSslASN1Create_CSRWithPSAECDSAKeyHash(struct SharkSslASN1Create *o,
                                                           const SharkSslPSAEcdsaKey *key,
                                                           struct SharkSslCertDN *certDN,
                                                           const char *SAN,
                                                           struct SharkSslBitExtReq *keyUsage,
                                                           struct SharkSslBitExtReq *nsCertType,
                                                           U8 hashID);
#endif  /* SHARKSSL_ENABLE_CSR_CREATION */
SHARKSSL_API int SharkSslPSAEcdsaKey_destructor(SharkSslPSAEcdsaKey *o);

#endif
