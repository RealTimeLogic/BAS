/*
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
 *               http://www.realtimelogic.com
 *               http://www.sharkssl.com
 ****************************************************************************
 *
 */

#ifndef _SharkSslCSR_h
#define _SharkSslCSR_h

/*! \file SharkSslCSR.h
    \brief Issuer policy for CSR signing
*/

#include "SharkSSL.h"

#ifdef __cplusplus
extern "C" {
#endif

#if (SHARKSSL_ENABLE_CSR_SIGNING)
/**
 * Issuer-selected certificate profiles. CSR extensions are validated but
 * never copied to the certificate. CA profiles require an explicit
 * pathLenConstraint. Use 0xFFFF for leaves.
 */
#define SHARKSSL_CERT_PROFILE_ROOT_CA          1
#define SHARKSSL_CERT_PROFILE_INTERMEDIATE_CA  2
#define SHARKSSL_CERT_PROFILE_TLS_SERVER       3
#define SHARKSSL_CERT_PROFILE_TLS_CLIENT       4
#define SHARKSSL_CERT_PROFILE_OPCUA_SERVER     5
#define SHARKSSL_CERT_PROFILE_OPCUA_CLIENT     6

#define SHARKSSL_CERT_SAN_DNS                  2
#define SHARKSSL_CERT_SAN_URI                  6
#define SHARKSSL_CERT_SAN_IP                   7

/** DNS/URI values are ASCII bytes; IP values are 4 or 16 network-order bytes. */
typedef struct SharkSslCertSAN
{
   const U8 *value;
   U16 length;
   U8 type;
} SharkSslCertSAN;

typedef struct SharkSslCertProfile
{
   int (*approveSANs)(void *context, const SharkSslCertSAN *names, U16 count);
   void *context;
   U16 pathLenConstraint;
   U8 type;
} SharkSslCertProfile;

/** One bounded DER certificate; a chain runs from operational CA to root. */
typedef struct SharkSslCertDER
{
   const U8 *data;
   U16 length;
} SharkSslCertDER;

/**
 * Validate an imported CA chain before enabling its issuing key. chain[0]
 * is the operational CA and chain[count - 1] must be byte-for-byte equal to
 * trustedRoot. expectedKey is the operational CA's SharkSSL public key.
 * utcNow is a NUL-terminated YYYYMMDDHHMMSS UTC timestamp. Rejects weak
 * signatures, unsupported constraints, and chains over eight certificates
 * or with DER entries over 32767 bytes. Issuer/subject names must have the
 * same DER encoding; self-issued CAs count toward path length. Returns zero
 * only when all checks pass. The caller selects the trusted root from its
 * configured trust store and revalidates when the time or chain changes.
 */
SHARKSSL_API int
SharkSslCert_validateCAChain(const SharkSslCertDER *chain,
                            const SharkSslCertDER *trustedRoot,
                            const SharkSslKey expectedKey,
                            const char *utcNow,
                            U16 count);

/**
 * Check an issued leaf against the approved key, exact typed DNS/URI/IP SAN
 * set, issuer, and validity timestamps. The issuer certificate must already
 * belong to a validated CA chain. The function verifies the leaf signature
 * with that issuer's public key and rejects cA=TRUE or keyCertSign. Profile
 * purposes and revocation remain the caller's policy checks. DER inputs are
 * bounded to 32767 bytes and the SAN set to 16 entries. Timestamps are
 * NUL-terminated YYYYMMDDHHMMSS UTC strings. Returns zero only on success.
 */
SHARKSSL_API int
SharkSslCert_validateIssuedIdentity(const SharkSslCertDER *issued,
                                    const SharkSslCertDER *issuer,
                                    const SharkSslKey expectedKey,
                                    const SharkSslCertSAN *approvedSANs,
                                    const char *validFrom,
                                    const char *validTo,
                                    const char *utcNow,
                                    U16 sanCount);

/**
 * Sign a verified CSR under an explicit issuer policy. For leaf certificates,
 * approveSANs must return zero only when all typed DNS/URI/IP names in the CSR
 * match the authorized enrollment identity. The callback receives pointers
 * into csrData that remain valid only during this call. No requested
 * extension bytes are copied to the certificate. Root CA requires caCert
 * to be NULL and privKey to match the CSR; other profiles require caCert.
 * For non-root issuance, caCert must identify a CA permitted to sign and its
 * attached signing key must match the certificate's public key. The caller
 * must validate the issuer's trust chain and current validity separately.
 * OPC UA profiles require exactly one absolute URI SAN, nonempty commonName
 * and organization, and a DNS/IP SAN for servers. They select OPC UA Key
 * Usage for the subject's RSA/ECC key; servers include both TLS EKU purposes.
 * validFrom and validTo use UTC YYYYMMDDHHMMSS. On success, signedCSR
 * receives an allocated SharkSSL certificate that the caller frees with
 * baFree; the return value is its size. A negative value indicates failure.
 */
SHARKSSL_API int
SharkSslCert_signCSR(SharkSslCert *signedCSR,
                    const SharkSslCertProfile *profile,
                    const U8 *csrData,
                    const SharkSslCert caCert,
                    const SharkSslKey privKey,
                    const char *validFrom,
                    const char *validTo,
                    int csrDataLen,
                    SharkCertSerialNumber serialNumber,
                    U8 hashID);
#endif

#ifdef __cplusplus
}
#endif

#endif
