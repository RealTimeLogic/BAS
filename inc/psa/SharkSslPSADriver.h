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
 *   $Id: SharkSslPSADriver.h 6573 2026-10-23 21:26:04Z gianluca $
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

#ifndef SHARKSSL_PSA_DRIVER_H
#define SHARKSSL_PSA_DRIVER_H

#include "psa/crypto.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SHARKSSL_PSA_DRIVER_LOCATION_VENDOR_MIN       ((psa_key_location_t)0x800000)
#define SHARKSSL_PSA_DRIVER_LOCATION_VENDOR_MAX       ((psa_key_location_t)0xffffff)
#define SHARKSSL_PSA_DRIVER_HANDLE_NULL               ((uint64_t)0)
#define SHARKSSL_PSA_DRIVER_OPERATION_HANDLE_NULL     ((uint64_t)0)
#define SHARKSSL_PSA_DRIVER_MAX_LOCATIONS             4

/**
 * Optional opaque-key driver for volatile HMAC, PASSWORD and PEPPER keys. The provider retains only
 * the attributes and a nonzero driver-owned handle. Key generation occurs
 * entirely in the driver. Import consumes the caller's standard key encoding;
 * the provider does not retain those bytes. This interface does not provide
 * persistent external keys or export. An optional copy callback
 * duplicates a volatile key inside the same driver location without revealing
 * its material to the provider. The returned handle must be distinct from the
 * source handle and remain independently valid after either key is destroyed.
 * A failed copy leaves the output handle null and creates no driver key.
 * attach_key resolves a driver-provisioned key by an opaque label. This
 * profile requires the caller to supply a supported type, byte-aligned size, policy
 * and volatile external lifetime; the driver verifies compatibility. A
 * successful attach returns a unique live handle without creating key
 * material. A failed attach leaves the handle null and no live binding.
 * destroy_key releases an attached handle without erasing the provisioned
 * material; it must support an explicit retry after a failed release.
 *
 * Initialize the entire callback table to zero before assigning its fields.
 * Callbacks run under the provider key-store mutex and must not call PSA or
 * registration functions. The provider copies this table; its context must
 * remain alive until unregistered. Successful import/generate transfers one
 * unique, nonzero live handle to the provider. Failed creation leaves the
 * output handle null and no driver key allocated.
 * destroy success releases the handle; failure must retain a handle that can be
 * retried, even if physical deletion is uncertain. A retry must report success
 * once deletion or absence is confirmed. The provider disables use
 * after the first failed destroy and retains the handle for an explicit retry.
 *
 * mac_compute receives validated policy and an exact, non-wildcard algorithm.
 * It must return precisely PSA_MAC_LENGTH(type, bits, alg) bytes on success.
 * Partial results and results with an invalid length are never published.
 * The multipart MAC callbacks are optional as a group. mac_setup receives a
 * validated usage and algorithm and returns a nonzero operation handle on
 * success. A failed setup leaves the output handle null and creates no state.
 * Update and failed finish callbacks leave the operation available for abort;
 * successful finish consumes it. mac_abort consumes the operation handle even
 * when it reports an error. Operation handles remain unique while active.
 * Driver implementations own entropy, secret erasure and hardware protection,
 * and must enforce volatile lifetime across reset and application termination.
 * A software test driver does not establish secure-element isolation.
 * pbkdf2_block is required for opaque PASSWORD and PEPPER keys. Each callback
 * computes one complete digest-size block for a concrete supported PBKDF2-HMAC
 * algorithm and reports exactly output_size bytes. The provider never receives
 * the opaque root; it may pass the other password and salt components to the
 * driver. A callback failure clears caller output and invalidates the PSA
 * derivation operation. Driver and application must be rebuilt with this table.
 */
typedef struct SharkSslPSAKeyDriver
{
   void *context;
   psa_status_t (*import_key)(void *context, const psa_key_attributes_t *attributes,
                              const uint8_t *data, uint64_t *handle, size_t length);
   psa_status_t (*generate_key)(void *context, const psa_key_attributes_t *attributes,
                                uint64_t *handle);
   psa_status_t (*destroy_key)(void *context, uint64_t handle);
   psa_status_t (*mac_compute)(void *context, const psa_key_attributes_t *attributes,
                               const uint8_t *input, uint8_t *mac, size_t *mac_length,
                               uint64_t handle, psa_algorithm_t alg,
                               size_t input_length, size_t mac_size);
   psa_status_t (*mac_setup)(void *context, const psa_key_attributes_t *attributes,
                             uint64_t key_handle, psa_algorithm_t alg,
                             psa_key_usage_t usage, uint64_t *operation_handle);
   psa_status_t (*mac_update)(void *context, uint64_t operation_handle,
                              const uint8_t *input, size_t input_length);
   psa_status_t (*mac_sign_finish)(void *context, uint64_t operation_handle,
                                   uint8_t *mac, size_t *mac_length, size_t mac_size);
   psa_status_t (*mac_verify_finish)(void *context, uint64_t operation_handle,
                                     const uint8_t *mac, size_t mac_length);
   psa_status_t (*mac_abort)(void *context, uint64_t operation_handle);
   psa_status_t (*copy_key)(void *context, const psa_key_attributes_t *source_attributes,
                            uint64_t source_handle, const psa_key_attributes_t *target_attributes,
                            uint64_t *target_handle);
   psa_status_t (*attach_key)(void *context, const psa_key_attributes_t *attributes,
                              const uint8_t *label, size_t label_length,
                              uint64_t *handle);
   /** Compute one complete PBKDF2 block without exposing an opaque PASSWORD or PEPPER.
    * For PASSWORD, password is NULL and salt_before contains the complete salt.
    * For PEPPER, password contains the HMAC key and the opaque key is inserted
    * between salt_before and salt_after. block_index starts at one. */
   psa_status_t (*pbkdf2_block)(void *context, const psa_key_attributes_t *attributes,
                                uint64_t handle, psa_algorithm_t alg, uint32_t cost,
                                const uint8_t *password, size_t password_length,
                                const uint8_t *salt_before, size_t salt_before_length,
                                const uint8_t *salt_after, size_t salt_after_length,
                                uint8_t block_index, uint8_t *output,
                                size_t *output_length, size_t output_size);
} SharkSslPSAKeyDriver;

/**
 * Register or replace one location; pass NULL to unregister it. Only primary
 * secure-element and vendor-defined locations are accepted. Registration needs
 * an empty key store and no active key dependencies; otherwise BAD_STATE leaves
 * all registrations unchanged. destroy_key, one operation callback and at least one
 * creation callback are mandatory. Multipart MAC callbacks must be supplied
 * as a complete group or omitted. Missing methods are unsupported.
 */
psa_status_t SharkSslPSAKeyDriver_set(const SharkSslPSAKeyDriver *o, psa_key_location_t location);

#ifdef __cplusplus
}
#endif
#endif
