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
 *   $Id: SharkSslPSAConsumerECDH.h 6622 2026-10-25 23:00:19Z gianluca $
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

#ifndef SHARKSSL_PSA_CONSUMER_ECDH_H
#define SHARKSSL_PSA_CONSUMER_ECDH_H

#include "SharkSSL.h"
#include "SharkSslPSAConsumerCurves.h"

#if !SHARKSSL_PSA_CONSUMER
#error SharkSslPSAConsumerECDH.h requires SHARKSSL_PSA_CONSUMER
#endif

/** Standalone ECDH private key owned by the external PSA provider.
    Initialize once, then call destructor before reusing this context. */
typedef struct SharkSslPSAEcdhKey
{
   psa_key_id_t handle;
   U16 curveID;
} SharkSslPSAEcdhKey;

/** Generate a volatile ECDH private key for a supported standalone curve. */
SHARKSSL_API int SharkSslPSAEcdhKey_generate(SharkSslPSAEcdhKey *o, U16 curveID);

/** Import a private scalar: big-endian for Weierstrass, little-endian for X25519/X448. */
SHARKSSL_API int SharkSslPSAEcdhKey_import(SharkSslPSAEcdhKey *o, U16 curveID,
                                           const U8 *privateKey, U16 privateKeyLen);

/** Export the public key as SEC1 uncompressed or RFC 7748. */
SHARKSSL_API int SharkSslPSAEcdhKey_exportPublic(const SharkSslPSAEcdhKey *o,
                                                 U8 *publicKey, U16 publicKeySize, U16 *publicKeyLen);

/** Derive a raw shared secret; clear it on any failure, including all-zero Montgomery output. */
SHARKSSL_API int SharkSslPSAEcdhKey_agree(const SharkSslPSAEcdhKey *o,
                                          const U8 *peerPublicKey, U16 peerPublicKeyLen,
                                          U8 *sharedSecret, U16 sharedSecretSize, U16 *sharedSecretLen);

/** Destroy the volatile PSA key and clear this context. */
SHARKSSL_API int SharkSslPSAEcdhKey_destructor(SharkSslPSAEcdhKey *o);

#endif
