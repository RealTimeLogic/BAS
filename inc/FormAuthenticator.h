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
 *   $Id: FormAuthenticator.h 4915 2021-12-01 18:26:55Z wini $
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


/** Implements Form-Based Authentication.
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
      FormAuthenticator(){}
      /**
         \param userDbIntf is a reference to a user database you must
         provide/implement.
         \param realm is required when using HA1 password hashes.
         \param sendLogin must be an implementation of LoginRespIntf.
      */
      FormAuthenticator(
         UserIntf* userDbIntf, const char* realm, LoginRespIntf* sendLogin);

      /** Prevent dictionary attacks.

      An interesting feature of the FormAuthenticator is that the form
      authenticator makes it difficult to perform dictionary attacks
      even without using the LoginTracker. The FormAuthenticator will
      make it extremely difficult to distinguish between valid and
      invalid login attempts, since the form authenticator sends HTTP
      200 OK response messages instead of the numerous 400 type
      errors.
    
          \param tracker the IP address tracker.
      */
      void setLoginTracker(LoginTracker* tracker);

      /** Set authenticator into secure mode and accept only SSL connections.
          The authenticator ignores non secure connections and
          directly calls the LoginRespIntf callback if not secure. You
          must add logic for testing for non secure connections in
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
BA_API void FormAuthenticator_constructor(
   FormAuthenticator* o,
   UserIntf* userDbIntf,
   const char* realm,
   LoginRespIntf* login);

#define FormAuthenticator_destructor(o) do { \
   if((o)->realm) baFree((o)->realm); (o)->realm=0;} while(0)

#define FormAuthenticator_setLoginTracker(o,loginTracker) \
   (o)->tracker=loginTracker

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
