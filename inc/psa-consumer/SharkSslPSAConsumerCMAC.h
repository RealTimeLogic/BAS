/**
 *     ____             _________                __                _
 *    / __ \___  ____ _/ /_  __(_)___ ___  ___  / /   ____  ____ _(_)____
 *   / /_/ / _ \/ __ `/ / / / / / __ `__ \/ _ \/ /   / __ \/ __ `/ / ___/
 *  / _, _/  __/ /_/ / / / / / / / / /  __/ /___/ /_/ / /_/ / /__
 * /_/ |_|\___/\__,_/_/ /_/ /_/_/ /_/ /_/\___/_____/\____/\__, /_/\___/
 *                                                       /____/
 *
 *                 SharkSSL Embedded SSL/TLS Stack
 ****************************************************************************
 *   PROGRAM MODULE
 *
 *   $Id: SharkSslPSAConsumerCMAC.h 6622 2026-10-25 23:00:19Z gianluca $
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

#ifndef SHARKSSL_PSA_CONSUMER_CMAC_H
#define SHARKSSL_PSA_CONSUMER_CMAC_H

#include "SharkSSL.h"

#if (!SHARKSSL_PSA_CONSUMER)
#error SharkSslPSAConsumerCMAC.h requires SHARKSSL_PSA_CONSUMER
#endif

/** Compute AES-CMAC with a caller-owned PSA key authorized for SIGN_MESSAGE.
    Returns zero on success; clears mac and returns -1 on failure. */
SHARKSSL_API int SharkSslPSACMAC_compute(psa_key_id_t key, const U8 *message, U32 messageLen,
                                         U8 *mac, U16 macSize, U16 *macLen);

/** Verify AES-CMAC with a caller-owned PSA key authorized for VERIFY_MESSAGE. */
SHARKSSL_API int SharkSslPSACMAC_verify(psa_key_id_t key, const U8 *message, U32 messageLen,
                                        const U8 *mac, U16 macLen);

#endif
