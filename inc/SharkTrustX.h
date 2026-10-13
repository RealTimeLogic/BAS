/*
 *     ____             _________                __                _
 *    / __ \___  ____ _/ /_  __(_)___ ___  ___  / /   ____  ____ _(_)____
 *   / /_/ / _ \/ __ `/ / / / / / __ `__ \/ _ \/ /   / __ \/ __ `/ / ___/
 *  / _, _/  __/ /_/ / / / / / / / / / / /  __/ /___/ /_/ / /_/ / / /__
 * /_/ |_|\___/\__,_/_/ /_/ /_/_/ /_/ /_/\___/_____/\____/\__, /_/\___/
 *                                                       /____/
 *
 *                  Barracuda Embedded Web-Server
 ****************************************************************************
 *   PROGRAM MODULE
 *
 *   $Id$
 *
 *   COPYRIGHT:  Real Time Logic LLC, 2026
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
 *               https://realtimelogic.com
 ****************************************************************************
 */

#ifndef SharkTrustX_h
#define SharkTrustX_h

#include <HttpServer.h>
#include <BaTimer.h>
#include <IoIntf.h>
#include <SharkSSL.h>
#include <stddef.h>

/** @file SharkTrustX.h
 * @brief Automatic certificates for one native BWS HTTPS port.
 *
 * Start with the @ref SharkTrustXSharkSslPublicApi "SharkTrustX C Client" manual
 * for certificate basics, a C integration example, build requirements, storage,
 * and cleanup. The runnable example is in examples/SharkTrustX/readme.html.
 *
 * Compile BWS.c with SHARKTRUSTX, BA_JSON=1, and
 * SHARKSSL_ENABLE_CERTINFO_PARSE_API=1. BWS includes the client and its
 * dependencies; do not compile SharkTrustX.c separately. Compile the portal's
 * generated tokengen.c with TOKENGEN_C_API.
 *
 * One client manages one HTTPS port, using IPv6 first and then IPv4 when
 * USE_IPV6 is enabled. Hold the server dispatcher mutex when
 * calling this API; never call from an interrupt. Keep configuration and
 * borrowed objects alive and unchanged until successful cleanup completes.
 * start() acceptance is separate from certificate readiness. clean() erases
 * state and is not a normal shutdown operation. Saved keys are UNENCRYPTED:
 * the supplied IoIntf must be private and must never be served over HTTP.
 */

#ifdef __cplusplus
extern "C" {
#endif

/** Numeric lifecycle notifications and errors.
 * Zero is an accepted start/cleanup/address request, or successful cleanup
 * completion. getStatus() instead returns a BaBool indicating active management.
 */
typedef enum SharkTrustX_Code
{
   SharkTrustX_Ready = 1, /**< Usable certificate installed; renewal scheduled. */
   SharkTrustX_Retry = 2, /**< A retry follows the preceding negative event. */
   SharkTrustX_Busy = -1, /**< A client or cleanup already owns the lifecycle. */
   SharkTrustX_Identity = -2, /**< Saved identity, name, or provider mismatch. */
   SharkTrustX_Storage = -3, /**< Cannot read, commit, or clean local state. */
   SharkTrustX_Portal = -4, /**< Invalid configuration or portal/DNS failure. */
   SharkTrustX_Acme = -5, /**< ACME account, order, or authorization failure. */
   SharkTrustX_Trust = -6, /**< HTTPS peer did not pass trust/name/date checks. */
   SharkTrustX_Crypto = -7, /**< Allocation, key, signing, or listener failure. */
   SharkTrustX_Terms = -9 /**< Public CA terms were not accepted. */
} SharkTrustX_Code;

/** Compute the portal proof using generated tokengen code.
 * @param message Borrowed binary input, possibly containing zero bytes.
 * @param messageLen Number of input bytes, not strlen(message).
 * @param proof Required output buffer for exactly 32 bytes.
 * @return Zero on success, nonzero on failure. Do not retain input pointers.
 */
typedef int (*SharkTrustX_Proof)(const unsigned char* message,
                               size_t messageLen, unsigned char proof[32]);

/** Receive one numeric event.
 * @param context Application pointer from configuration or clean().
 * @param code A SharkTrustX_Code; a cleanup callback receives 0 or a negative
 * error. No keys or credentials are passed to callbacks.
 * onEvent runs on the timer worker with the dispatcher mutex held. onClean
 * uses that worker for active cleanup, or runs inline for an inactive store.
 * Keep callbacks short and nonblocking. See @ref stx-callbacks.
 */
typedef void (*SharkTrustX_Event)(void* context, int code);

/** Receive the portal-confirmed DNS name, independently of HTTPS readiness.
 * @param context Application pointer from configuration.
 * @param name Borrowed, NUL-terminated full DNS name, valid during this call;
 * NULL for an IP-only SharkCA enrollment. Copy it if needed after returning.
 * @param length DNS name length in bytes, or zero for IP-only enrollment.
 * Called once after confirmation on each start, then only when the name
 * changes. A SharkCA host must arrange its own .local mDNS advertisement.
 */
typedef void (*SharkTrustX_Name)(void* context, const char* name, size_t length);

/** Required integration objects and provisioning information.
 * Initialize to zero, then set required members. All pointers and strings are
 * borrowed until successful cleanup completes, including the config itself.
 * Do not modify them while management or cleanup is active. See @ref stx-config.
 */
typedef struct SharkTrustX_Config
{
   HttpServer* server; /**< Required initialized server with running dispatcher. */
   BaTimer* timer; /**< Required dedicated timer using the server's mutex. */
   IoIntf* io; /**< Required private writable state store; see @ref stx-storage. */
   SharkSsl* clientSsl; /**< Required client TLS context with trusted CA list. */
   const char* portalDomain; /**< Required tokengen domain only, without scheme/path. */
   const unsigned char* zoneKey; /**< Required borrowed 32-byte tokengen zone key. */
   SharkTrustX_Proof proof; /**< Required tokengen proof callback. */
   const char* name; /**< Preferred hostname label, 1..63 ASCII letters/digits/hyphens;
                     * no leading/trailing hyphen. The portal assigns the final name.
                     * NULL allowed only for SharkCA IP-only enrollment. */
   const char* directoryUrl; /**< Required absolute HTTPS ACME directory in public
                             * builds. Ignored in private builds, which derive
                             * https://portalDomain/acme/directory. */
   const char* email; /**< Required contact email in public builds; ignored privately. */
   BaBool acceptTerms; /**< Must be TRUE publicly; ignored privately. */
   const char* interfaceName; /**< Optional bind address passed to both listener
                              * attempts; NULL selects all interfaces. A literal
                              * address restricts binding to its address family.
                              * Same meaning as HttpSharkSslServCon. */
   U16 port; /**< Required unused HTTPS port, 1..65535, normally 443.
              * Shared by the IPv6 and IPv4 listener attempts. */
   SharkTrustX_Event onEvent; /**< Optional status callback, or NULL. */
   SharkTrustX_Name onName; /**< Optional confirmed-name callback, or NULL. */
   void* context; /**< Optional borrowed application pointer for both callbacks. */
} SharkTrustX_Config;

/** Start registration, certificate installation, and automatic renewal.
 * @param config Required configuration retained until cleanup completes.
 * @return 0 when accepted; a negative SharkTrustX_Code for immediate rejection.
 * Acceptance does not mean HTTPS is ready. Subsequent errors are delivered via
 * onEvent and retried. Saved identity changes require cleanup, never automatic
 * re-enrollment. Provisioning and directory identity are checked before use.
 * With USE_IPV6, certificate installation tries IPv6 first, then IPv4 on the
 * same port, sharing one TLS context. Otherwise it tries only IPv4. Successful
 * listeners are retained; installation fails if neither opens. The platform
 * determines whether an IPv6 socket also accepts IPv4 connections.
 */
BA_API int SharkTrustX_start(const SharkTrustX_Config* config);

/** Snapshot of the certificate currently served by the managed listeners.
 * This value contains no pointers or private key material.
 */
typedef struct SharkTrustX_Status
{
   BaBool ready; /**< TRUE when at least one listener has an unexpired certificate.
                  * Does not guarantee reachability over both address families. */
   BaTime expires; /**< Certificate expiry, UTC Unix seconds; zero if not ready. */
   BaTime renew; /**< Renewal due time, UTC Unix seconds; zero if not ready.
                 * This is the certificate lifetime midpoint. It can be in the
                 * past during issuance or retries; it is not a completion
                 * guarantee or the next retry time. */
} SharkTrustX_Status;

/** Copy current certificate status without network or storage operations.
 * @param status Required caller-owned output; all fields are overwritten.
 * @return TRUE while the client manages certificates, including initial
 * enrollment without a certificate. FALSE before start, during cleanup, or
 * after failed cleanup; all output fields are then zero.
 * Hold the server dispatcher mutex, including when called from a web page.
 * Calling from onEvent is allowed. The copied value remains owned by the
 * caller, but may become stale after releasing the mutex.
 */
BA_API BaBool SharkTrustX_getStatus(SharkTrustX_Status* status);

/** Ask the active client to publish a changed local IPv4 address.
 * @param address Dotted IPv4 string, copied before returning; NULL discovers
 * the address of the next trusted portal connection. No periodic address
 * discovery is performed. The host calls this after a network change.
 * @return 0 when queued, -1 without active management, -4 for invalid IPv4.
 * Changes are serialized with issuance. SharkCA identifier changes trigger
 * reissuance. Failures use the configured numeric event callback. A successful
 * address update alone does not emit a certificate-ready event.
 */
BA_API int SharkTrustX_setIpAddress(const char* address);

/** Stop work, close the managed listener, and erase owned certificate state.
 * @param io Required store used by the active client, or an inactive store.
 * @param all TRUE also removes portal enrollment; FALSE retains completed or
 * pending enrollment. SharkCA also retains its bound account key/directory;
 * public builds remove account/provider state, permitting a provider change.
 * @param onClean Optional completion callback; may run before return if idle.
 * @param context Optional borrowed pointer for onClean.
 * @return 0 when accepted, -1 for a conflicting cleanup/store, -3 for NULL io.
 * Active issuance
 * does not prevent cleanup. Completion code 0 guarantees no old callback,
 * persistence write, or listener install can follow. On failure management
 * stays stopped and start remains blocked until cleanup succeeds. A durable
 * reset marker finishes interrupted cleanup before state can be restored.
 * Acceptance does not report storage success; inspect onClean's code.
 * See @ref stx-cleanup for retained state and a clean-then-start example.
 */
BA_API int SharkTrustX_clean(IoIntf* io, BaBool all,
                           SharkTrustX_Event onClean, void* context);

#ifdef __cplusplus
}
#endif
#endif
