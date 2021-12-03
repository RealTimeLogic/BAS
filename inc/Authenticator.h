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
 *   $Id: Authenticator.h 4915 2021-12-01 18:26:55Z wini $
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

#ifndef __Authenticator_h
#define __Authenticator_h

#include "BasicAuthenticator.h"
#include "DigestAuthenticator.h"
#include "FormAuthenticator.h"


/** @addtogroup Authentication
    @{
*/


/** This class implements HTTP Basic, HTTP Digest, and Form-Based
   Authentication.

    \htmlinclude Authenticator.txt
*/
typedef struct Authenticator
#ifdef __cplusplus
   : public AuthenticatorIntf
{
      /**
         \param userDbIntf is a reference to a user database you must
         provide/implement.

         \param realm is the name of the Basic Authenticator realm.

         \param sendLogin must be an implementation of LoginRespIntf.

      */
      Authenticator(UserIntf* userDbIntf,
                    const char* realm,
                    LoginRespIntf* sendLogin);


      /** Prevent dictionary attacks.
          \param tracker the IP address tracker.
      */
      void setLoginTracker(LoginTracker* tracker);

      /** Get the internal BasicAuthenticator
       */
      BasicAuthenticator* getBasicAuthenticator();

      /** Get the internal DigestAuthenticator
       */
      DigestAuthenticator* getDigestAuthenticator();

      /** Get the internal FormAuthenticator
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
BA_API void Authenticator_constructor(Authenticator* o,
                               UserIntf* userDbIntf,
                               const char* realm,
                               LoginRespIntf* sendLogin);
BA_API void Authenticator_destructor(Authenticator* o);
#define Authenticator_setLoginTracker(o, loginTracker)\
   BasicAuthenticator_setLoginTracker(&(o)->basicAuth, loginTracker),\
   DigestAuthenticator_setLoginTracker(&(o)->digestAuth, loginTracker),\
   FormAuthenticator_setLoginTracker(&(o)->formAuth, loginTracker)
#define Authenticator_getBasicAuthenticator(o) (&(o)->basicAuth)
#define Authenticator_getDigestAuthenticator(o) (&(o)->digestAuth)
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
