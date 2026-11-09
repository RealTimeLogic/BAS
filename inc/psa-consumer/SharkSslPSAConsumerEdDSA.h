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
 *   $Id: SharkSslPSAConsumerEdDSA.h 6818 2026-11-08 21:18:02Z gianluca $
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

#ifndef SHARKSSL_PSA_CONSUMER_EDDSA_H
#define SHARKSSL_PSA_CONSUMER_EDDSA_H

#include "SharkSSL.h"

#if !SHARKSSL_PSA_CONSUMER
#error SharkSslPSAConsumerEdDSA.h requires SHARKSSL_PSA_CONSUMER
#endif

/**
 * A volatile PSA Ed25519 or Ed448 key. PureEdDSA signs the message;
 * HashEdDSA uses SHA-512 for Ed25519ph and SHAKE256-512 for Ed448ph.
 * Signatures are the RFC 8032 raw R||S encoding.
 */
typedef struct SharkSslPSAEdDSAKey
{
   psa_key_id_t handle;
   U16 bits;
   U8 prehash;
   U8 publicOnly;
} SharkSslPSAEdDSAKey;

SHARKSSL_API int SharkSslPSAEdDSAKey_generate(SharkSslPSAEdDSAKey *o, U16 bits, int prehash);
SHARKSSL_API int SharkSslPSAEdDSAKey_importPrivate(SharkSslPSAEdDSAKey *o, const U8 *seed,
                                                   U16 seedLen, U16 bits, int prehash);
SHARKSSL_API int SharkSslPSAEdDSAKey_importPublic(SharkSslPSAEdDSAKey *o, const U8 *publicKey,
                                                  U16 publicKeyLen, U16 bits, int prehash);
SHARKSSL_API int SharkSslPSAEdDSAKey_exportPublic(const SharkSslPSAEdDSAKey *o, U8 *publicKey,
                                                  U16 publicKeySize, U16 *publicKeyLen);
SHARKSSL_API int SharkSslPSAEdDSAKey_sign(const SharkSslPSAEdDSAKey *o, const U8 *message,
                                          U32 messageLen, U8 *signature, U16 signatureSize,
                                          U16 *signatureLen);
SHARKSSL_API int SharkSslPSAEdDSAKey_verify(const SharkSslPSAEdDSAKey *o, const U8 *message,
                                            U32 messageLen, const U8 *signature, U16 signatureLen);
#if (SHARKSSL_ENABLE_CSR_CREATION && SHARKSSL_ENABLE_EDDSA)
/**
 * Create a PKCS#10 CSR using a private PureEdDSA PSA handle
 * The key must be Ed25519 or Ed448 and permit message signing
 */
SHARKSSL_API int SharkSslASN1Create_CSRWithPSAEdDSAKey(struct SharkSslASN1Create *o,
                                                       const SharkSslPSAEdDSAKey *key,
                                                       struct SharkSslCertDN *certDN,
                                                       const char *SAN,
                                                       struct SharkSslBitExtReq *keyUsage,
                                                       struct SharkSslBitExtReq *nsCertType);
#endif
SHARKSSL_API int SharkSslPSAEdDSAKey_destructor(SharkSslPSAEdDSAKey *o);

#endif
