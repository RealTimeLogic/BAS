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
 *   $Id: DavAuth.h 4915 2021-12-01 18:26:55Z wini $
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
      /** Create a DavAuth instance.
         \param userDbIntf is a reference to a user database you must
         provide/implement.
         \param realm is the realm name provided to the client.
      */
      DavAuth(UserIntf* userDbIntf, const char* realm);

      /** Get the internal BasicAuthenticator
       */
      BasicAuthenticator* getBasicAuth();

      /** Get the internal DigestAuthenticator
       */
      DigestAuthenticator* getDigestAuth();

      /** Prevent dictionary attacks.
          \param tracker the IP address tracker.
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
BA_API void DavAuth_constructor(DavAuth* o,
                        UserIntf* userDbIntf,
                        const char* realm);

BA_API void DavAuth_destructor(DavAuth* o);
#define DavAuth_getBasicAuth(o) (&(o)->basicAuth)
#define DavAuth_getDigestAuth(o) (&(o)->digestAuth)
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
