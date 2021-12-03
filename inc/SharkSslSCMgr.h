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
 *   $Id: SharkSslSCMgr.h 4915 2021-12-01 18:26:55Z wini $
 *
 *   COPYRIGHT:  Real Time Logic LLC, 2013
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

/** The handle returned by #SharkSslSCMgr_get and passed into
    #SharkSslSCMgr_save. The handle will be NULL when SharkSslSCMgr_get
    does not have a saved session. The null pointer must be passed into
    SharkSslSCMgr_save.
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

/** Initialize a SharkSslSCMgr instance.
    \param o an uninitialized static object or dynamically allocated object.
    \param ssl an initialized SharkSsl instance.
    \param maxTime the maximum time for stored sessions in seconds. A
    good value would be 60*60.
 */
SHARKSSL_API void SharkSslSCMgr_constructor(
   SharkSslSCMgr* o, SharkSsl* ssl, U32 maxTime);

/** Get and restore the session, if any.
 */
SHARKSSL_API SharkSslSCMgrNode* SharkSslSCMgr_get(
   SharkSslSCMgr* o,SharkSslCon* scon,const char* host,U16 port);

/** Save the session, if any.
 */
SHARKSSL_API void SharkSslSCMgr_save(
   SharkSslSCMgr* o, SharkSslCon* scon,
   const char* host, U16 port,  SharkSslSCMgrNode* n);

#ifdef __cplusplus
}
#endif

/** @} */ /* end group SharkSslSCMgr */ 

#endif
