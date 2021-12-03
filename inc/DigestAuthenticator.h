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
 *   $Id: DigestAuthenticator.h 4915 2021-12-01 18:26:55Z wini $
 *
 *   COPYRIGHT:  Real Time Logic LLC, 2003-2013
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

#ifndef __DigestAuthenticator_h
#define __DigestAuthenticator_h

#include <AuthenticatedUser.h>

/** @addtogroup Authentication
    @{
 */


/** Implements Digest Authentication.
    Please see the
    <a href="../../authentication.html">User Authentication</a>
    documentation for more information.
 */
typedef struct DigestAuthenticator
#ifdef __cplusplus
: public AuthenticatorIntf
{
      DigestAuthenticator(){}

      /**
         \param userDbIntf is a reference to a user database you must
         provide/implement.
         \param realm is the name displayed in the browser's pop-up window.
         \param sendLogin must be an implementation of LoginRespIntf.
      */
      DigestAuthenticator(UserIntf* userDbIntf,
                          const char* realm,
                          LoginRespIntf* sendLogin);

      /** Prevent dictionary attacks.
          \param tracker the IP address tracker.
      */
      void setLoginTracker(LoginTracker* tracker);

      /** Sets a HTTP digest authenticate header and sets status to
          401. This method can be used to design logic for invalidating
          the user and password saved by a browser.
       */
      static void setAutHeader(const char* realm, HttpResponse* response);

      /**
        Enable strict mode. Few browsers support this.
       */
      void setStrictMode(bool enableStrictMode=false);

#else
{
   AuthenticatorIntf superClass; /*as if inherited*/
#endif
      UserIntf* userDbIntf;
      LoginTracker* tracker;
      char* realm;
      LoginRespIntf* sendLogin;
      BaBool strictMode;
      BaBool filterMsDomain; /* If remove MS domain name prefix */
} DigestAuthenticator;


#ifdef __cplusplus
extern "C" {
#endif
BA_API void DigestAuthenticator_constructor(
   DigestAuthenticator* o,
   UserIntf* userDbIntf,
   const char* realm,
   LoginRespIntf* sendLogin);
BA_API void DigestAuthenticator_destructor(DigestAuthenticator* o);
#define DigestAuthenticator_setLoginTracker(o,loginTracker) \
  (o)->tracker=loginTracker
#define DigestAuthenticator_setFilterMsDomain(o, state) \
  (o)->filterMsDomain=state
BA_API void DigestAuthenticator_setAutHeader(
   const char*,HttpResponse*);
#define DigestAuthenticator_setStrictMode(o, enableStrictMode) \
  (o)->strictMode=enableStrictMode
BA_API BaBool DigestAuthenticator_isDigest(AuthenticatedUser* user);
#ifdef __cplusplus
}
inline DigestAuthenticator::DigestAuthenticator(UserIntf* userDbIntf,
                                                const char* realm,
                                                LoginRespIntf* sendLogin) {
   DigestAuthenticator_constructor(this,userDbIntf,realm,sendLogin);
}
inline void DigestAuthenticator::setLoginTracker(LoginTracker* tracker) {
   DigestAuthenticator_setLoginTracker(this, tracker);
}
inline void DigestAuthenticator::setAutHeader(
   const char* realm, HttpResponse* resp) {
   DigestAuthenticator_setAutHeader(realm,resp); }
inline void DigestAuthenticator::setStrictMode(bool enableStrictMode) {
   DigestAuthenticator_setStrictMode(this, enableStrictMode ? TRUE : FALSE);
}

#endif

/** @} */ /* end of Authentication */

#endif
