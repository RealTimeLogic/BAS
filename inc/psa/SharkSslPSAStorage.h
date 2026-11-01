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
 *   $Id: SharkSslPSAStorage.h 6405 2026-10-12 21:25:56Z gianluca $
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

#ifndef SHARKSSL_PSA_STORAGE_H
#define SHARKSSL_PSA_STORAGE_H

#include "psa/crypto.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Platform storage for local persistent keys. Callbacks run under the provider
 * key-store lock and must not call PSA or this registration function. The
 * context must remain alive until the backend is unregistered. The provider
 * copies the callback table; registration requires an empty RAM key store.
 * Register at startup or at a quiescent point with no cryptographic operations
 * in progress. A backend must not be switched while its IDs remain in use.
 *
 * read with material == NULL and capacity == 0 queries attributes and length.
 * A second read returns the same complete record or an error. Missing records
 * return PSA_ERROR_DOES_NOT_EXIST. create must atomically create a new record,
 * never replace an existing ID, and return PSA_ERROR_ALREADY_EXISTS on collision.
 * Successful create means attributes and material are durable; a recoverable
 * create failure leaves no new record. remove must erase a record and return
 * DOES_NOT_EXIST if absent; recoverable failure preserves it for a retry.
 * If storage fails during rollback, its durable outcome can be uncertain.
 * Report STORAGE_FAILURE and refuse further callbacks until storage has been
 * repaired and the affected ID reconciled. The provider returns no new handle
 * for failed creation; it cannot recover or prove durability for failed media.
 *
 * The backend owns serialization, format versioning, integrity, confidentiality,
 * access isolation, power-loss atomicity and secure deletion. Raw C attributes
 * are not a portable disk format. No filesystem or flash backend is installed
 * by default. Only PSA_KEY_LIFETIME_PERSISTENT in local storage is supported.
 */
typedef struct SharkSslPSAKeyStorage
{
   void *context;
   psa_status_t (*read)(void *context, psa_key_attributes_t *attributes,
                        uint8_t *material, size_t *length,
                        psa_key_id_t id, size_t capacity);
   psa_status_t (*create)(void *context, const psa_key_attributes_t *attributes,
                          const uint8_t *material, size_t length);
   psa_status_t (*remove)(void *context, psa_key_id_t id);
} SharkSslPSAKeyStorage;

/**
 * Register storage, or pass NULL to unregister. No live RAM keys are allowed;
 * destroy volatile keys and purge persistent keys first. Storage records are
 * unchanged. BAD_STATE preserves the previous backend. Invalid callback tables
 * return INVALID_ARGUMENT. The same backend must own all outstanding IDs.
 */
psa_status_t SharkSslPSAKeyStorage_set(const SharkSslPSAKeyStorage *o);

#ifdef __cplusplus
}
#endif
#endif
