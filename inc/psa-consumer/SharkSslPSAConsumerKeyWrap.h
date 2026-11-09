/**
 *     ____             _________                __                _
 *    / __ \___  ____ _/ /_  __(_)___ ___  ___  / /   ____  ____ _(_)____
 *   / /_/ / _ \/ __ `/ / / / / / __ `__ \/ _ \/ /   / __ \/ __ `/ / ___/
 *  / _, _/  __/ /_/ / / / / / / / / /  __/ /___/ /_/ / /_/ / /__
 * /_/ |_|\___/\__,_/_/ /_/ /_/_/ /_/\___/_____/\____/\__, /_/\___/
 *                                                       /____/
 *
 *                 SharkSSL Embedded SSL/TLS Stack
 ****************************************************************************
 *   PROGRAM MODULE
 *
 *   $Id: SharkSslPSAConsumerKeyWrap.h 6818 2026-11-08 21:18:02Z gianluca $
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

#ifndef SHARKSSL_PSA_CONSUMER_KEY_WRAP_H
#define SHARKSSL_PSA_CONSUMER_KEY_WRAP_H

#include "SharkSSL.h"

#if (!SHARKSSL_PSA_CONSUMER)
#error SharkSslPSAConsumerKeyWrap.h requires SHARKSSL_PSA_CONSUMER
#endif

/** Wrap an exportable PSA key under a caller-owned AES key-encryption key.
    The KEK must permit WRAP for algorithm PSA_ALG_KW or PSA_ALG_KWP; the input
    key must permit EXPORT. Clears wrapped and sets wrappedLen to zero on error. */
SHARKSSL_API int SharkSslPSAKeyWrap_wrap(psa_key_id_t kek, psa_algorithm_t algorithm, psa_key_id_t key,
                                         U8 *wrapped, U16 wrappedSize, U16 *wrappedLen);

/** Unwrap to a new PSA key using caller-provided attributes. The KEK must
    permit UNWRAP. The caller owns the returned key and must destroy it.
    Sets *key to PSA_KEY_ID_NULL on error. */
SHARKSSL_API int SharkSslPSAKeyWrap_unwrap(psa_key_id_t kek, psa_algorithm_t algorithm,
                                           const U8 *wrapped, U16 wrappedLen,
                                           const psa_key_attributes_t *attributes, psa_key_id_t *key);

#endif
