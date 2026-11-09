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
 *   $Id: SharkSslPSAFileStorage.h 6818 2026-11-08 21:18:02Z gianluca $
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

#ifndef SHARKSSL_PSA_FILE_STORAGE_H
#define SHARKSSL_PSA_FILE_STORAGE_H

#include "psa/SharkSslPSAStorage.h"

#define SHARKSSL_PSA_FILE_WRAPPING_KEY_SIZE  32
#ifdef __cplusplus
#define SHARKSSL_PSA_FILE_STORAGE_INIT       SharkSslPSAFileStorage()
#else
#define SHARKSSL_PSA_FILE_STORAGE_INIT       {0}
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Optional encrypted filesystem adapter for Windows and Linux hosts.
 * Initialize with SHARKSSL_PSA_FILE_STORAGE_INIT. The object and its callback
 * table must remain alive while registered. Do not copy an open object.
 * open does not register the table.
 */
typedef struct SharkSslPSAFileStorage
{
   SharkSslPSAKeyStorage storage;
   void *state;
} SharkSslPSAFileStorage;

/**
 * Open or create a private directory, locking it exclusively until close.
 * directory is UTF-8 on Windows. wrapping_key must contain 32 independently
 * provisioned bytes; retain the same key outside this directory across restarts.
 * The adapter copies and clears its own key on close; it never persists it.
 * Existing directories must belong to the current user and exclude other users.
 * Windows requires a protected owner-only DACL; Linux requires mode 0700.
 * Parent directories must be trusted. Local filesystems are required.
 * AES-256-GCM is required; unsupported targets/configurations return NOT_SUPPORTED.
 * Files are encrypted and authenticated; rollback of an old valid record and
 * physical secure erasure of filesystem copies are outside this adapter.
 * Nonces come from BCryptGenRandom/getrandom. Use a dedicated wrapping key and
 * manage its lifetime externally, including the random-IV GCM invocation limit.
 * Inherited contexts are rejected after fork; reopen in a quiescent process.
 * If rollback I/O fails, the instance refuses further storage callbacks.
 * Repair storage and reconcile the affected ID before resuming after reopen.
 */
psa_status_t SharkSslPSAFileStorage_open(SharkSslPSAFileStorage *o, const char *directory,
                                         const uint8_t wrapping_key[SHARKSSL_PSA_FILE_WRAPPING_KEY_SIZE]);

/**
 * Unregister the callback table and quiesce all operations before close.
 * Destroy volatile keys and purge persistent caches before unregistering.
 * close releases the OS lock and clears the wrapping key. It does not remove
 * stored keys, automatically unregister callbacks, or provide physical erasure.
 */
void SharkSslPSAFileStorage_close(SharkSslPSAFileStorage *o);

#ifdef __cplusplus
}
#endif
#endif
