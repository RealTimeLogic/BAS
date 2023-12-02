/*
 *     ____             _________                __                _
 *    / __ \___  ____ _/ /_  __(_)___ ___  ___  / /   ____  ____ _(_)____
 *   / /_/ / _ \/ __ `/ / / / / / __ `__ \/ _ \/ /   / __ \/ __ `/ / ___/
 *  / _, _/  __/ /_/ / / / / / / / / / / /  __/ /___/ /_/ / /_/ / / /__
 * /_/ |_|\___/\__,_/_/ /_/ /_/_/ /_/ /_/\___/_____/\____/\__, /_/\___/
 *                                                       /____/
 *
 *                  Barracuda Embedded Web-Server
 *
 ****************************************************************************
 *			      HEADER
 *
 *   $Id$
 *
 *   COPYRIGHT:  Real Time Logic LLC, 2023
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
 ****************************************************************************
 *
 * This header file, included by all the 'arch' TargConfig.h headers,
 * sets default SharkSSL settings for BAS.
 */

#ifndef _TargConfig_h
#error Only TargConfig.h should include this header file
#endif

#if 1
#ifndef SHARKSSL_ENABLE_ASN1_KEY_CREATION
#define SHARKSSL_ENABLE_ASN1_KEY_CREATION                1
#endif
#ifndef SHARKSSL_ENABLE_RSAKEY_CREATE
#define SHARKSSL_ENABLE_RSAKEY_CREATE                    1
#endif
#ifndef SHARKSSL_ENABLE_ECCKEY_CREATE
#define SHARKSSL_ENABLE_ECCKEY_CREATE                    1
#endif
#ifndef SHARKSSL_ENABLE_CSR_CREATION
#define SHARKSSL_ENABLE_CSR_CREATION                     1
#endif
#ifndef SHARKSSL_ENABLE_CSR_SIGNING
#define SHARKSSL_ENABLE_CSR_SIGNING                      1
#endif
#endif

#ifndef SHARKSSL_USE_SHA_512
#define SHARKSSL_USE_SHA_512 1
#endif

#ifndef SHARKSSL_ECC_USE_SECP256R1
#define SHARKSSL_ECC_USE_SECP256R1 1
#endif
#ifndef SHARKSSL_ECC_USE_SECP384R1
#define SHARKSSL_ECC_USE_SECP384R1 1
#endif
#ifndef SHARKSSL_ECC_USE_SECP521R1
#define SHARKSSL_ECC_USE_SECP521R1 1
#endif
#ifndef SHARKSSL_ECC_USE_BRAINPOOLP256R1
#define SHARKSSL_ECC_USE_BRAINPOOLP256R1 1
#endif
#ifndef SHARKSSL_ECC_USE_BRAINPOOLP384R1
#define SHARKSSL_ECC_USE_BRAINPOOLP384R1 1
#endif
#ifndef SHARKSSL_ECC_USE_BRAINPOOLP512R1
#define SHARKSSL_ECC_USE_BRAINPOOLP512R1 1
#endif

#ifndef SHARKSSL_ENABLE_RSA_OAEP
#define SHARKSSL_ENABLE_RSA_OAEP 1
#endif



