/**
 *     ____             _________                __                _
 *    / __ \___  ____ _/ /_  __(_)___ ___  ___  / /   ____  ____ _(_)____
 *   / /_/ / _ \/ __ `/ / / / / / __ `__ \/ _ \/ /   / __ \/ __ `/ / ___/
 *  / _, _/  __/ /_/ / / / / / /  __/ /___/ /_/ / /_/ / /__
 * /_/ |_|\___/\__,_/_/ /_/ /_/_/ /_/\___/_____/\____/\__, /_/\___/
 *                                                       /____/
 *
 *                 SharkSSL Embedded SSL/TLS Stack
 ****************************************************************************
 *   PROGRAM MODULE
 *
 *   $Id: SharkSslPSAConsumerPBKDF2.h 6818 2026-11-08 21:18:02Z gianluca $
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

#ifndef SHARKSSL_PSA_CONSUMER_PBKDF2_H
#define SHARKSSL_PSA_CONSUMER_PBKDF2_H

#include "SharkSSL.h"

#if (!SHARKSSL_PSA_CONSUMER)
#error SharkSslPSAConsumerPBKDF2.h requires SHARKSSL_PSA_CONSUMER
#endif

/**
 * Derive bytes with PSA PBKDF2-HMAC or PBKDF2-AES-CMAC-PRF-128.
 * The caller owns output and supplies its exact size in outputLen.
 * Returns zero on success; clears output and returns -1 on failure.
 */
SHARKSSL_API int SharkSslPSAPBKDF2_derive(U8 *output, const U8 *password, const U8 *salt,
                                          psa_algorithm_t alg, U32 passwordLen,
                                          U32 saltLen, U32 iterations, U16 outputLen);

#endif
