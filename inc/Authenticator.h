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
 *   $Id: Authenticator.h 5978 2026-09-11 16:13:48Z wini $
 *
 *   COPYRIGHT:  Real Time Logic LLC, 2005-2008
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

/** @file Authenticator.h */

#ifndef __Authenticator_h
#define __Authenticator_h

#include "BasicAuthenticator.h"
#include "DigestAuthenticator.h"
#include "FormAuthenticator.h"


/** @addtogroup Authentication
    @{
*/


/** Combines HTTP Basic, HTTP Digest, and form-based authentication.

    Authenticator lets the client select between the built-in
    authentication mechanisms and shares the same user database and
    login response interface between them.

    \htmlinclude Authenticator.txt
*/
typedef struct Authenticator
#ifdef __cplusplus
   : public AuthenticatorIntf
{
      /** Construct an authenticator using application-provided user lookup.
        @param userDbIntf Required borrowed user database interface; keep it alive
        while this authenticator is used.
        @param realm Required NUL-terminated realm string, copied during
        construction. It identifies the authentication realm sent to clients.
        @param sendLogin Required borrowed login-response interface, which must
        outlive authentication calls.
        Constructors have no error return; realm allocation failure cannot be
        reported through the constructor signature. Detach the authenticator from
        directories and stop its users before calling Authenticator_destructor().
        The C++ interface has no destructor that performs this cleanup automatically.
       */
      Authenticator(UserIntf* userDbIntf,
                    const char* realm,
                    LoginRespIntf* sendLogin);


      /** Configure login-attempt tracking.
        @param tracker Borrowed tracker, or NULL to disable tracking (the default).
        Keep the tracker alive while configured. This setter does not allocate
        or destroy it.
        Applies to all embedded authentication mechanisms.
       */
      void setLoginTracker(LoginTracker* tracker);

      /** Access the embedded Basic authenticator.
        @return Non-NULL borrowed pointer with the parent's lifetime. Use it to
        configure that mechanism; do not destroy or free it independently.
       */
      BasicAuthenticator* getBasicAuthenticator();

      /** Access the embedded Digest authenticator.
        @return Non-NULL borrowed pointer with the parent's lifetime. Use it to
        configure that mechanism; do not destroy or free it independently.
       */
      DigestAuthenticator* getDigestAuthenticator();

      /** Access the embedded Form authenticator.
        @return Non-NULL borrowed pointer with the parent's lifetime. Use it to
        configure that mechanism; do not destroy or free it independently.
       */
      FormAuthenticator* getFormAuthenticator();
#else
      {
         AuthenticatorIntf superClass; /*as if inherited*/
#endif
         BasicAuthenticator basicAuth;
         DigestAuthenticator digestAuth;
         FormAuthenticator  formAuth;
         int authpref;
      } Authenticator;


#ifdef __cplusplus
extern "C" {
#endif
/** C form of Authenticator::Authenticator.
    @param o Required storage to initialize.
    @param userDbIntf Required borrowed user database.
    @param realm Copied realm string; see the C++ constructor for NULL handling.
    @param sendLogin Required borrowed login-response interface.
 */
BA_API void Authenticator_constructor(Authenticator* o,
                               UserIntf* userDbIntf,
                               const char* realm,
                               LoginRespIntf* sendLogin);
/** Release authenticator-owned realm storage after detaching all users.
    @param o Required initialized authenticator. Borrowed dependencies are not freed.
    @note The current composite destructor omits its Digest realm cleanup.
    This limitation is recorded for a separate implementation repair.
 */
BA_API void Authenticator_destructor(Authenticator* o);
/** C form of Authenticator::setLoginTracker.
    @param o Required initialized authenticator.
    @param loginTracker Borrowed tracker, or NULL to disable.
 */
#define Authenticator_setLoginTracker(o, loginTracker)\
   BasicAuthenticator_setLoginTracker(&(o)->basicAuth, loginTracker),\
   DigestAuthenticator_setLoginTracker(&(o)->digestAuth, loginTracker),\
   FormAuthenticator_setLoginTracker(&(o)->formAuth, loginTracker)
/** @copydoc Authenticator::getBasicAuthenticator
    @param o Required initialized parent authenticator.
 */
#define Authenticator_getBasicAuthenticator(o) (&(o)->basicAuth)
/** @copydoc Authenticator::getDigestAuthenticator
    @param o Required initialized parent authenticator.
 */
#define Authenticator_getDigestAuthenticator(o) (&(o)->digestAuth)
/** @copydoc Authenticator::getFormAuthenticator
    @param o Required initialized parent authenticator.
 */
#define Authenticator_getFormAuthenticator(o) (&(o)->formAuth)
#ifdef __cplusplus
}
inline Authenticator::Authenticator(
   UserIntf* userDbIntf,
   const char* realm,
   LoginRespIntf* sendLogin) {
   Authenticator_constructor(this, userDbIntf, realm, sendLogin);
}
inline void Authenticator::setLoginTracker(LoginTracker* tracker) {
   Authenticator_setLoginTracker(this, tracker);
}
inline BasicAuthenticator* Authenticator::getBasicAuthenticator() {
   return Authenticator_getBasicAuthenticator(this);
}
inline DigestAuthenticator* Authenticator::getDigestAuthenticator() {
   return Authenticator_getDigestAuthenticator(this);
}
inline FormAuthenticator* Authenticator::getFormAuthenticator() {
   return Authenticator_getFormAuthenticator(this);
}
#endif

/** @} */ /* end of Authentication */


#endif
