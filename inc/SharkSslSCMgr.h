/*
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
 *   $Id: SharkSslSCMgr.h 6111 2026-09-23 18:49:11Z gianluca $
 *
 *   COPYRIGHT:  Real Time Logic LLC, 2013 - 2026
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
#ifndef _SharkSslSCMgr_h
#define _SharkSslSCMgr_h

#include <SharkSslEx.h>
#include "SplayTree.h"
#include "DoubleList.h"

#ifndef SHARKSSL_API
#define SHARKSSL_API
#else  /* Barracuda */
#define SHARKSSL_BA 1
#endif 


/** @addtogroup SharkSslSCMgr
@{
*/

/** The handle returned by #SharkSslSCMgr_get. Pass a non-NULL handle to
    #SharkSslSCMgr_replace when a resumed TLS 1.3 connection receives a
    fresh session ticket. Call #SharkSslSCMgr_save when the returned
    handle is NULL.
 */
typedef struct
{
   SplayTreeNode super;
   DoubleLink dlink;
   SharkSslSession* ss;
   const char* host;
   U16 hostLen;
   U16 port;
} SharkSslSCMgrNode;



/** See #SharkSslSCMgr_constructor for details.
 */
typedef struct
{
   SharkSslIntf super;
   SplayTree stree;
   DoubleList dlist;
   SharkSsl* ssl;
   U32 maxTime;
   int noOfSessions;
} SharkSslSCMgr;


#ifdef __cplusplus
extern "C" {
#endif

/** SharkSslSCMgr simplifies using the session API for TLS clients;
    the constructor initializes a SharkSslSCMgr instance.
    \param o an uninitialized static object or dynamically allocated object.
    \param ssl an initialized SharkSsl instance.
    \param maxTime the maximum time for stored sessions in seconds. A
    good value would be 60*60.
 */
SHARKSSL_API void SharkSslSCMgr_constructor(
   SharkSslSCMgr* o, SharkSsl* ssl, U32 maxTime);

/**
 * Resume a session before starting the handshake. The returned handle
 * must not be modified by the caller; NULL means no session was found.
 * The host key must identify the same server name supplied as SNI in the
 * new ClientHello, when SNI is used. Store only sessions whose original
 * server certificate was authenticated for that name; this manager does
 * not verify certificates.
 */
SHARKSSL_API SharkSslSCMgrNode* SharkSslSCMgr_get(
   SharkSslSCMgr* o,SharkSslCon* scon,const char* host,U16 port);

/** Replace the saved session after a resumed TLS 1.3 connection receives
    a fresh session ticket. The saved session remains unchanged if the
    replacement cannot be acquired.

    \param o an initialized SharkSslSCMgr object.
    \param n the non-NULL handle returned by #SharkSslSCMgr_get.
    \param scon a valid SharkSslCon object.
    \return 0 if the session was replaced, otherwise -1 is returned.
 */
SHARKSSL_API int SharkSslSCMgr_replace(
   SharkSslSCMgr* o, SharkSslSCMgrNode* n, SharkSslCon* scon);

/**
 * Save the session when #SharkSslSCMgr_get returns NULL. It is an
 * error calling this method if #SharkSslSCMgr_get returns a
 * handle. The method should be called when closing the connection
 * and just before terminating the SharkSslCon object. For TLS 1.3, save
 * it only after authenticating the server certificate for host.
 *
 * \param o an initialized SharkSslSCMgrNode object
 * \param scon a valid SharkSslCon object.
 * \param host the server's domain name
 * \param port the server's port number e.g. 443
 * \return 0 if session was saved, otherwise -1 is returned.
 */
SHARKSSL_API int SharkSslSCMgr_save(
   SharkSslSCMgr* o, SharkSslCon* scon, const char* host, U16 port);

#ifdef __cplusplus
}
#endif

/** @} */ /* end group SharkSslSCMgr */ 

#endif
