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
 *   $Id: FormAuthenticator.h 5978 2026-09-11 16:13:48Z wini $
 *
 *   COPYRIGHT:  Real Time Logic LLC, 2003 - 2017
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

/** @file FormAuthenticator.h */

#ifndef __FormAuthenticator_h
#define __FormAuthenticator_h

#include <AuthenticatedUser.h>

#ifndef __DOXYGEN__
typedef struct
{
      AuthenticatedUser superClass; /*as if inherited*/
      const char* errorMessagePage; /*Path to a user defined error page.*/
      BaBool isFirstTime;
} FormAuthUser;
#endif

/** @addtogroup Authentication
    @{
 */


/** Implements browser-oriented form-based authentication.
    See the
    <a href="../../authentication.html">User Authentication</a>
    documentation for an introduction to authentication and authorization.
    A form authenticator can be used only by browser clients.

    \sa Authenticator
 */
typedef struct FormAuthenticator
#ifdef __cplusplus
: public AuthenticatorIntf
{
      /** Leave storage uninitialized; call FormAuthenticator_constructor before use. */
      FormAuthenticator(){}
      /** Construct an authenticator using application-provided user lookup.
        @param userDbIntf Required borrowed user database interface; keep it alive
        while this authenticator is used.
        @param realm Copied NUL-terminated realm. NULL selects an empty realm;
        supply the matching realm when the database stores HA1 password hashes.
        @param sendLogin Required borrowed login-response interface, which must
        outlive authentication calls.
        Constructors have no error return; realm allocation failure cannot be
        reported through the constructor signature. Detach the authenticator from
        directories and stop its users before calling FormAuthenticator_destructor().
        The C++ interface has no destructor that performs this cleanup automatically.
       */
      FormAuthenticator(
         UserIntf* userDbIntf, const char* realm, LoginRespIntf* sendLogin);

      /** Configure login-attempt tracking.
        @param tracker Borrowed tracker, or NULL to disable tracking (the default).
        Keep the tracker alive while configured. This setter does not allocate
        or destroy it.
       */
      void setLoginTracker(LoginTracker* tracker);

      /** Set the authenticator into secure mode and accept only SSL/TLS connections.
          The authenticator ignores non-secure connections and
          directly calls the LoginRespIntf callback if not secure. You
          must add logic for testing for non-secure connections in
          your callback.
      */
      void setSecure();

#else
{
   AuthenticatorIntf superClass; /*as if inherited*/
#endif
      UserIntf* userDbIntf;
      LoginTracker* tracker;
      char* realm;
      LoginRespIntf* sendLogin;
      BaBool secure;
      U8 aesKey[32];
} FormAuthenticator;


#ifdef __cplusplus
extern "C" {
#endif
/** C form of FormAuthenticator::FormAuthenticator.
    @param o Required storage to initialize.
    @param userDbIntf Required borrowed user database.
    @param realm Copied realm string; see the C++ constructor for NULL handling.
    @param login Required borrowed login-response interface.
 */
BA_API void FormAuthenticator_constructor(
   FormAuthenticator* o,
   UserIntf* userDbIntf,
   const char* realm,
   LoginRespIntf* login);

/** Release authenticator-owned realm storage after detaching all users.
    @param o Required initialized authenticator. Borrowed dependencies are not freed.
 */
#define FormAuthenticator_destructor(o) do { \
      if((o)->realm)                            \
         baFree((o)->realm);                    \
      (o)->realm=0;                             \
   } while(0)

/** C form of FormAuthenticator::setLoginTracker.
    @param o Required initialized authenticator.
    @param loginTracker Borrowed tracker, or NULL to disable.
 */
#define FormAuthenticator_setLoginTracker(o,loginTracker) \
   (o)->tracker=loginTracker

/** C form of FormAuthenticator::setSecure.
    @param o Required initialized authenticator. The flag is initially FALSE;
    this setter enables it permanently for the lifetime of this initialization.
 */
#define FormAuthenticator_setSecure(o) (o)->secure=TRUE

#ifdef __cplusplus
}
inline FormAuthenticator::FormAuthenticator(UserIntf* userDbIntf,
                                            const char* realm,
                                            LoginRespIntf* login) {
   FormAuthenticator_constructor(
      this, userDbIntf, realm, login);
}
inline void FormAuthenticator::setLoginTracker(LoginTracker* tracker) {
   FormAuthenticator_setLoginTracker(this, tracker);
}
inline void FormAuthenticator::setSecure() {
   FormAuthenticator_setSecure(this);
}
#endif

/** @} */ /* end of Authentication */


#endif
