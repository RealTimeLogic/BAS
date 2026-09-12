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
 *            HEADER
 *
 *   $Id: DavAuth.h 5978 2026-09-11 16:13:48Z wini $
 *
 *   COPYRIGHT:  Real Time Logic, 2006
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
 */

/** @file DavAuth.h */

#ifndef __DavAuth_h
#define __DavAuth_h

#include "BasicAuthenticator.h"
#include "DigestAuthenticator.h"


/** @addtogroup Authentication
    @{
*/


/** This class implements HTTP Basic and HTTP Digest authentication.

The client selects the HTTP authentication method it wants to use. The
authenticator also handles the domain name prefix added to the user
name by many Microsoft HTTP clients.

This class was specifically designed for our WebDAV plugin, but the
authenticator is also useful when authenticating non-browser clients
in a mixed environment.
*/
typedef struct DavAuth
#ifdef __cplusplus
   : public AuthenticatorIntf
{
      /** Construct an authenticator using application-provided user lookup.
        @param userDbIntf Required borrowed user database interface; keep it alive
        while this authenticator is used.
        @param realm Required NUL-terminated realm string, copied during
        construction. It identifies the authentication realm sent to clients.
        Constructors have no error return; realm allocation failure cannot be
        reported through the constructor signature. Detach the authenticator from
        directories and stop its users before calling DavAuth_destructor().
        The C++ interface has no destructor that performs this cleanup automatically.
        Microsoft domain-prefix filtering is enabled for both embedded authenticators.
       */
      DavAuth(UserIntf* userDbIntf, const char* realm);

      /** Access the embedded Basic authenticator.
        @return Non-NULL borrowed pointer with the parent's lifetime. Use it to
        configure that mechanism; do not destroy or free it independently.
       */
      BasicAuthenticator* getBasicAuth();

      /** Access the embedded Digest authenticator.
        @return Non-NULL borrowed pointer with the parent's lifetime. Use it to
        configure that mechanism; do not destroy or free it independently.
       */
      DigestAuthenticator* getDigestAuth();

      /** Configure login-attempt tracking.
        @param tracker Borrowed tracker, or NULL to disable tracking (the default).
        Keep the tracker alive while configured. This setter does not allocate
        or destroy it.
        Applies to all embedded authentication mechanisms.
       */
      void setLoginTracker(LoginTracker* tracker);
#else
      {
         AuthenticatorIntf superClass; /*as if inherited*/
#endif
         LoginRespIntf sendLogin;
         BasicAuthenticator basicAuth;
         DigestAuthenticator digestAuth;
      } DavAuth;


#ifdef __cplusplus
extern "C" {
#endif
/** C form of DavAuth::DavAuth.
    @param o Required storage to initialize.
    @param userDbIntf Required borrowed user database.
    @param realm Copied realm string; see the C++ constructor for NULL handling.
 */
BA_API void DavAuth_constructor(DavAuth* o,
                        UserIntf* userDbIntf,
                        const char* realm);

/** Release authenticator-owned realm storage after detaching all users.
    @param o Required initialized authenticator. Borrowed dependencies are not freed.
 */
BA_API void DavAuth_destructor(DavAuth* o);
/** @copydoc DavAuth::getBasicAuth
    @param o Required initialized parent authenticator.
 */
#define DavAuth_getBasicAuth(o) (&(o)->basicAuth)
/** @copydoc DavAuth::getDigestAuth
    @param o Required initialized parent authenticator.
 */
#define DavAuth_getDigestAuth(o) (&(o)->digestAuth)
/** C form of DavAuth::setLoginTracker.
    @param o Required initialized authenticator.
    @param loginTracker Borrowed tracker, or NULL to disable.
 */
#define DavAuth_setLoginTracker(o, loginTracker) do{\
   BasicAuthenticator_setLoginTracker(&(o)->basicAuth, loginTracker);\
   DigestAuthenticator_setLoginTracker(&(o)->digestAuth, loginTracker);\
   }while(0)
#ifdef __cplusplus
}
inline DavAuth::DavAuth(UserIntf* userDbIntf,const char* realm) {
   DavAuth_constructor(this, userDbIntf, realm);
}
inline BasicAuthenticator* DavAuth::getBasicAuth() {
   return DavAuth_getBasicAuth(this);
}
inline DigestAuthenticator* DavAuth::getDigestAuth() {
   return DavAuth_getDigestAuth(this);
}
inline void DavAuth::setLoginTracker(LoginTracker* tracker) {
   DavAuth_setLoginTracker(this, tracker);
}
#endif

/** @} */ /* end of Authentication */


#endif
