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
 *   $Id: BasicAuthenticator.h 5978 2026-09-11 16:13:48Z wini $
 *
 *   COPYRIGHT:  Real Time Logic LLC, 2003-2026
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

/** @file BasicAuthenticator.h */

#ifndef __BasicAuthenticator_h
#define __BasicAuthenticator_h

#include <AuthenticatedUser.h>

/** @addtogroup Authentication
    @{
 */

/** Implements HTTP Basic authentication.
    Please see the
    <a href="../../authentication.html">User Authentication</a>
    documentation for more information.
 */
typedef struct BasicAuthenticator
#ifdef __cplusplus
: public AuthenticatorIntf
{
      /** Leave storage uninitialized; call BasicAuthenticator_constructor before use. */
      BasicAuthenticator(){}
      /** Construct an authenticator using application-provided user lookup.
        @param userDbIntf Required borrowed user database interface; keep it alive
        while this authenticator is used.
        @param realm Required NUL-terminated realm string, copied during
        construction. It identifies the authentication realm sent to clients.
        @param sendLogin Required borrowed login-response interface, which must
        outlive authentication calls.
        Constructors have no error return; realm allocation failure cannot be
        reported through the constructor signature. Detach the authenticator from
        directories and stop its users before calling BasicAuthenticator_destructor().
        The C++ interface has no destructor that performs this cleanup automatically.
       */
      BasicAuthenticator(UserIntf* userDbIntf,
                         const char* realm,
                         LoginRespIntf* sendLogin);

      /** Configure login-attempt tracking.
        @param tracker Borrowed tracker, or NULL to disable tracking (the default).
        Keep the tracker alive while configured. This setter does not allocate
        or destroy it.
       */
      void setLoginTracker(LoginTracker* tracker);

      /** Sets an HTTP Basic authentication challenge and status code
          401. This method can be used to design logic for invalidating
          the user and password saved by a browser.
          \param realm Required NUL-terminated realm used in the challenge;
          supply a value suitable for an HTTP quoted string.
          \param response Required response receiving the 401 challenge.
          The response retains its own header value.
          \return 0 on success, including an ignored call during inclusion;
          E_MALLOC if header storage fails; E_IS_COMMITTED if already committed
          outside inclusion. Failure leaves the status code unchanged.
       */
      static int setAutHeader(const char* realm, HttpResponse* response);

#else
{
   AuthenticatorIntf superClass; /*as if inherited*/
#endif
      UserIntf* userDbIntf;
      LoginTracker* tracker;
      char* realm;
      LoginRespIntf* sendLogin;
      BaBool filterMsDomain; /* If remove MS domain name prefix */
} BasicAuthenticator;


#ifdef __cplusplus
extern "C" {
#endif
/** C form of BasicAuthenticator::BasicAuthenticator.
    @param o Required storage to initialize.
    @param userDbIntf Required borrowed user database.
    @param realm Copied realm string; see the C++ constructor for NULL handling.
    @param sendLogin Required borrowed login-response interface.
 */
BA_API void BasicAuthenticator_constructor(
   BasicAuthenticator* o,
   UserIntf* userDbIntf,
   const char* realm,
   LoginRespIntf* sendLogin);
/** Release authenticator-owned realm storage after detaching all users.
    @param o Required initialized authenticator. Borrowed dependencies are not freed.
 */
BA_API void BasicAuthenticator_destructor(BasicAuthenticator* o);
/** C form of BasicAuthenticator::setLoginTracker.
    @param o Required initialized authenticator.
    @param loginTracker Borrowed tracker, or NULL to disable.
 */
#define BasicAuthenticator_setLoginTracker(o,loginTracker) \
   (o)->tracker=loginTracker
/** Select user-name domain-prefix filtering (initially FALSE).
    @param o Required initialized authenticator.
    @param state TRUE removes the prefix through the first backslash before user
    lookup; FALSE uses the complete supplied user name.
 */
#define BasicAuthenticator_setFilterMsDomain(o, state) \
  (o)->filterMsDomain=state
/* Returns 0 on success (including an ignored include), E_MALLOC when header
 * storage fails, or E_IS_COMMITTED. Failure does not set status 401. */
/** C form of BasicAuthenticator::setAutHeader. The first argument is the
    required realm string; the second is the required response.
    @return Zero, E_MALLOC, or E_IS_COMMITTED as described by setAutHeader().
 */
BA_API int BasicAuthenticator_setAutHeader(
   const char* realm,HttpResponse* resp);
#ifdef __cplusplus
}
inline BasicAuthenticator::BasicAuthenticator(UserIntf* userDbIntf,
                                              const char* realm,
                                              LoginRespIntf* sendLogin) {
   BasicAuthenticator_constructor(
      this,userDbIntf,realm,sendLogin); }
inline void BasicAuthenticator::setLoginTracker(LoginTracker* tracker) {
   BasicAuthenticator_setLoginTracker(this, tracker); }
inline int BasicAuthenticator::setAutHeader(
   const char* realm, HttpResponse* resp) {
   return BasicAuthenticator_setAutHeader(realm,resp); }
#endif

/** @} */ /* end of Authentication */

#endif
