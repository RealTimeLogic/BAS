/*
 *     ____             _________                __                _     
 *    / __ \___  ____ _/ /_  __(_)___ ___  ___  / /   ____  ____ _(_)____
 *   / /_/ / _ \/ __ `/ / / / / / __ `__ \/ _ \/ /   / __ \/ __ `/ / ___/
 *  / _, _/  __/ /_/ / / / / / / / / / / /  __/ /___/ /_/ / /_/ / / /__  
 * /_/ |_|\___/\__,_/_/ /_/ /_/_/ /_/ /_/\___/_____/\____/\__, /_/\___/  
 *                                                       /____/          
 *
 *                  Barracuda Embedded Web-Server
 *
 ****************************************************************************
 *			      HEADER
 *
 *   $Id: AuthenticatedUser.h 5978 2026-09-11 16:13:48Z wini $
 *
 *   COPYRIGHT:  Real Time Logic LLC, 2006 - 2023
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
 *
 */
/** @file AuthenticatedUser.h */

#ifndef __AuthenticatedUser_h
#define __AuthenticatedUser_h

#include <HttpServer.h>
#include <BaServerLib.h>
#include <DoubleList.h>
#include <SplayTree.h>

#ifndef __DOXYGEN__
struct AuthenticatorIntf;
struct UserIntf;
struct AuthorizerIntf;
struct AuthenticatedUser;
struct LoginRespIntf;
struct LoginTracker;
struct LoginTrackerIntf;
struct LoginTrackerNode;
struct AuthInfo;
#endif


#ifdef __cplusplus
extern "C" {
#endif
extern const char BasicAuthUser_derivedType[];
extern const char DigestAuthUser_derivedType[];
extern const char FormAuthUser_derivedType[];
#ifdef __cplusplus
}
#endif


/** @defgroup Authentication Authentication and authorization

    Please see <a href="../../authentication.html">Authenticating
    and authorizing users</a> for an introduction to the classes in
    the Authentication group.

    \sa <a href="../../index.html" target="top"> Barracuda Introduction </a>

   @{
 */

/** The authenticator types */
typedef enum {
   AuthenticatedUserType_Unknown,
   AuthenticatedUserType_Digest,
   AuthenticatedUserType_Basic,
   AuthenticatedUserType_Form
} AuthenticatedUserType;


/** Prototype for the Authorize callback method.

    \param intf The object pointer, which you must upcast to your class 
    implementation; i.e., MySecurityRealm* o = (MySecurityRealm*)intf;

    \param user A reference to the authenticated user. The method
    must return false if user is NULL.

    \param httpMethod The HTTP method type: From HttpRequest::getMethodType

    \param path Borrowed NUL-terminated relative resource path for this call.
    @return TRUE to permit access, FALSE to deny. No ownership is transferred.
*/
typedef BaBool (*AuthorizerIntf_Authorize)(
   struct AuthorizerIntf* intf,
   struct AuthenticatedUser* user,
   HttpMethod httpMethod,
   const char* path);

/** An abstract class, which you must implement, provides a method of
    authorizing an authenticated user.
 */
typedef struct AuthorizerIntf
{
#ifdef __cplusplus
      AuthorizerIntf(){}

      /** The constructor
          \param authorize Required callback; remains callable while this interface is used.
       */
      AuthorizerIntf(AuthorizerIntf_Authorize authorize);

      /** Returns TRUE if user is authorized.
          \param user AuthenticatedUser::get
          \param method The HTTP method type: From HttpRequest::getMethodType
          \param path The relative path element of the URL requested
          by the user.
      */
      bool authorize(struct AuthenticatedUser* user,
                     HttpMethod method,
                     const char* path);
#endif
      AuthorizerIntf_Authorize authorizeFP;
} AuthorizerIntf;

/** Install the callback used by AuthorizerIntf.
    @param authorize Required callback; remains callable while installed.
    @param o Required storage to initialize.
 */
#define AuthorizerIntf_constructor(o, authorize) (o)->authorizeFP=authorize
/** @copydoc AuthorizerIntf::authorize
    @param o Required initialized interface.
 */
#define AuthorizerIntf_authorize(o, user, method, path) \
  (o)->authorizeFP(o, user, method, path)

#ifdef __cplusplus
inline
AuthorizerIntf::AuthorizerIntf(AuthorizerIntf_Authorize authorize) {
   AuthorizerIntf_constructor(this,authorize); }
inline bool
AuthorizerIntf::authorize(struct AuthenticatedUser* user,
                             HttpMethod method,
                             const char* path) {
   return AuthorizerIntf_authorize(this,user,method,path) ? true : false; }
#endif


/**
  User database callback used by authenticators.

  The callback searches for info->username and sets AuthInfo::password,
  AuthInfo::ct, or both if the user is found.

  info->userObj is NULL, but can be set in this callback to signal
  information to the other callbacks such as
  LoginRespIntf_Service.

  info->user is NULL when this method is called.
  
  The callback is allowed to set header values and work with the
  response object.  The authenticator stops authentication and returns
  FALSE if the response object is committed; i.e., the login fails.

  The authenticator checks if the response is committed on return.
  The authenticator assumes the user is not authenticated if the
  response is committed.

    @param intf Required application interface receiving this call.
    @param info Required input/output authentication record. Read username/type/upwd
    and fill password/ct and optional policy fields. All pointers are borrowed
    for this synchronous call; do not retain the stack record.
    @note A direct database lookup can supply NULL cmd and tracker and type Unknown.
    Test cmd before using request/response APIs. Basic and Form permit ct=Valid
    after a callback comparison; Digest requires password or HA1 data.
*/
typedef void (*UserIntf_GetPwd)(struct UserIntf* intf,struct AuthInfo* info);

/** User database interface used by the authentication classes.
 *   The getPwd function populates AuthInfo with password data when a
 *   matching user is found.
 */
typedef struct UserIntf
{
#ifdef __cplusplus

      UserIntf() {}
      
      /** The UserIntf constructor.
          \param getPwd Required user-lookup callback; remains callable while installed.
      */
      UserIntf(UserIntf_GetPwd getPwd);

#endif
      UserIntf_GetPwd getPwdFp;
} UserIntf;

#ifdef __cplusplus
extern "C" {
#endif

/** Install the callback used by UserIntf.
    @param getPwd Required callback; remains callable while installed.
    @param o Required storage to initialize.
 */
#define UserIntf_constructor(o, getPwd) (o)->getPwdFp = getPwd

/** Invoke UserIntf_GetPwd synchronously.
    @param o Required initialized interface.
    @param username Required AuthInfo pointer, despite this historical macro argument name; it is not a string.
 */
#define UserIntf_getPwd(o, username) (o)->getPwdFp(o, username)
#ifdef __cplusplus
}
inline UserIntf::UserIntf(
   UserIntf_GetPwd getPwd) {
   UserIntf_constructor(this, getPwd);
}
#endif


#ifndef __DOXYGEN__
typedef struct AuthUserList
{
#ifdef __cplusplus
#endif
      SplayTreeNode super; /* inherits from SplayTreeNode */
      DoubleList list; /* List of AuthenticatedUser objects */
      struct UserIntf* userDb;
      HttpServer* server;
      char* username;
      char* password;
      int listLen; /* Number of objects in 'list' */
} AuthUserList;
#endif

BA_API int AuthUserList_createOrCheck(struct AuthInfo* info,
                               UserIntf* userDb,
                               void** ptr,
                               size_t size);


typedef DoubleListEnumerator AuthUserListEnumerator;
#define AuthUserListEnumerator_constructor(e, o) \
   DoubleListEnumerator_constructor(e, &(o)->list)
BA_API struct AuthenticatedUser*
AuthUserListEnumerator_getElement(DoubleListEnumerator* o);
BA_API struct AuthenticatedUser*
AuthUserListEnumerator_nextElement(DoubleListEnumerator* o);
void AuthUserList_termIfEmpty(AuthUserList* o);



/** Abstract base class implemented by BasicAuthUser, FormAuthUser and
 *  DigestAuthUser.
    Please see the
    <a href="../../authentication.html">User Authentication</a>
    documentation for more information.
 */ 
typedef struct AuthenticatedUser
{
#ifdef __cplusplus

      /** Find the authenticated user without creating a session.
    @param request Required current request.
    @return Borrowed user owned by the existing session, or NULL if unavailable.
    Do not free it or retain it beyond logout/session destruction.
    C equivalent: AuthenticatedUser_get1().
 */ 
      static AuthenticatedUser* get(HttpRequest* request);


      /** Find the authenticated-user session attribute.
    @param session Existing session, or NULL.
    @return Borrowed authenticated user, or NULL when no such attribute exists.
    C equivalent: AuthenticatedUser_get2().
 */
      static AuthenticatedUser* get(HttpSession* session);

      /** Get the containing session.
    @return Borrowed session pointer, or NULL if not attached. Do not terminate
    it directly to log out a user; use logout().
 */
      HttpSession* getSession();

      /** Access the stored credential representation.
    @return Borrowed NUL-terminated password/hash, or NULL if unavailable. Basic
    and Form callback-validated credentials can be represented by the placeholder
    "?" rather than the original password. Copy before logout/session destruction.
 */
      const char* getPassword();

      /** Access the authenticated name.
    @return Borrowed NUL-terminated user name, or NULL if unavailable. It remains
    valid only while the associated authentication record remains alive.
 */
      const char* getName();

      /** Log out and terminate the associated session or sessions.
    @param all False (default) terminates this session; true terminates the
    sessions sharing this user record. Session destruction may be deferred while
    in use, but the login slot is released immediately. Do not reuse the user pointer.
    Basic/Digest clients can automatically log in again using cached credentials.
    This call cannot erase credentials stored by the browser.
    @code
    AuthenticatedUser* user = AuthenticatedUser::get(request);
    if(user) user->logout(); // Avoid calling a C++ member through NULL.
    @endcode
    The C function AuthenticatedUser_logout(NULL, FALSE) is a no-op.
 */
      void logout(bool all=false);

      /** Identify the authenticator that created this user.
    @return Basic, Digest, Form, or Unknown from AuthenticatedUserType.
    Unknown includes the shared anonymous object and unrecognized derived types.
 */
      AuthenticatedUserType getType();

      /** Access the shared anonymous user.
    @return Non-NULL borrowed static object named "anonymous", with type Unknown.
    It is not a session login. Do not log it out, destroy, free, or modify it.
 */
      static AuthenticatedUser* getAnonymous();
#endif
      HttpSessionAttribute superClass; /*as if inherited */
      DoubleLink dlink; /* In AuthUserList */
      AuthUserList* authUserList;
      const char* derivedType; /* Used for dynamic cast */
} AuthenticatedUser;

#ifdef __cplusplus
extern "C" {
#endif

BA_API void
AuthenticatedUser_constructor(AuthenticatedUser* o,
                              const char* derivedType,
                              AuthUserList* list,
                              HttpSessionAttribute_Destructor destructor);
BA_API void AuthenticatedUser_destructor(AuthenticatedUser* o);
/** @copydoc AuthenticatedUser::get(HttpRequest*)
 */
BA_API AuthenticatedUser* AuthenticatedUser_get1(HttpRequest* request);
/** @copydoc AuthenticatedUser::get(HttpSession*)
 */
BA_API AuthenticatedUser* AuthenticatedUser_get2(HttpSession* session);
/** @copydoc AuthenticatedUser::getName
    @param o User pointer, or NULL to return NULL.
 */
#define AuthenticatedUser_getName(o) \
  ((o) && (o)->authUserList && (o)->authUserList->username ? \
  (o)->authUserList->username : 0)
#define AuthenticatedUser_getDerivedType(o) (o)->derivedType
/** @copydoc AuthenticatedUser::getSession
    @param o Required user.
 */
#define AuthenticatedUser_getSession(o) \
   HttpSessionAttribute_getSession((HttpSessionAttribute*)o)
/** @copydoc AuthenticatedUser::getPassword
    @param o User pointer, or NULL to return NULL.
 */
#define AuthenticatedUser_getPassword(o) \
   ((o) && (o)->authUserList && (o)->authUserList->password ? \
    (o)->authUserList->password : 0)
/** @copydoc AuthenticatedUser::logout
    @param o Session-owned authenticated user, or NULL for a no-op. Do not pass the anonymous object.
 */
BA_API void AuthenticatedUser_logout(AuthenticatedUser* o, BaBool all);
/** @copydoc AuthenticatedUser::getType
    @param o Required authenticated user.
 */
BA_API AuthenticatedUserType AuthenticatedUser_getType(AuthenticatedUser* o);
/** @copydoc AuthenticatedUser::getAnonymous
 */
BA_API AuthenticatedUser* AuthenticatedUser_getAnonymous(void);
#ifdef __cplusplus
}
inline AuthenticatedUser* AuthenticatedUser::get(HttpRequest* request){
   return AuthenticatedUser_get1(request); }
inline AuthenticatedUser* AuthenticatedUser::get(HttpSession* session){
   return AuthenticatedUser_get2(session); }
inline const char* AuthenticatedUser::getName() {
   return AuthenticatedUser_getName(this); }
inline HttpSession* AuthenticatedUser::getSession() {
   return AuthenticatedUser_getSession(this); }
inline const char* AuthenticatedUser::getPassword() {
   return AuthenticatedUser_getPassword(this); }
inline void AuthenticatedUser::logout(bool all) {
   AuthenticatedUser_logout(this, all ? TRUE : FALSE); }
inline AuthenticatedUserType AuthenticatedUser::getType() {
   return AuthenticatedUser_getType(this); }
inline AuthenticatedUser* AuthenticatedUser::getAnonymous() {
   return AuthenticatedUser_getAnonymous(); }
#endif

/** The authenticator callback method for the abstract class
    AuthenticatorIntf.

    \param super a pointer to the super class.
    \param relPath the URL's relative path
    \param cmd The HttpRequest HttpResponse container.
    \return The AuthenticatedUser if authenticated, otherwise NULL is returned.
 */
typedef AuthenticatedUser* (*AuthenticatorIntf_Authenticate)(
   struct AuthenticatorIntf* super,
   const char* relPath,
   HttpCommand* cmd);


/** Abstract interface class implemented by DigestAuthenticator,
 *  FormAuthenticator and BasicAuthenticator.
 */ 
typedef struct AuthenticatorIntf
{
#ifdef __cplusplus
      /*Only to be used as default constructor when sub-classing with C code*/
      AuthenticatorIntf(){}

      /** Install an authentication callback.
          @param authenticate Required callback; remains callable while installed.
       */
      AuthenticatorIntf(AuthenticatorIntf_Authenticate authenticate);

      /** Authenticate the user.
          @param relPath Borrowed NUL-terminated relative resource path.
          @param cmd Required current request/response container.
          \return The AuthenticatedUser if authenticated, otherwise
          NULL is returned.
      */
      AuthenticatedUser* authenticate(const char* relPath, HttpCommand* cmd);
#endif
      AuthenticatorIntf_Authenticate authenticateCB;
} AuthenticatorIntf;

#ifdef __cplusplus
extern "C" {
#endif

/** @copydoc AuthenticatorIntf::authenticate
    @param o Required initialized interface.
 */
#define AuthenticatorIntf_authenticate(o, relPath, cmd) \
  (o)->authenticateCB(o, relPath, cmd)

/** Install the callback used by AuthenticatorIntf.
    @param authenticate Required callback; remains callable while installed.
    @param o Required storage to initialize.
 */
BA_API void AuthenticatorIntf_constructor(
   AuthenticatorIntf* o,
   AuthenticatorIntf_Authenticate authenticate);
#ifdef __cplusplus
}
inline AuthenticatorIntf::AuthenticatorIntf(
   AuthenticatorIntf_Authenticate authenticate) {
   AuthenticatorIntf_constructor(this,authenticate);
}
inline AuthenticatedUser* AuthenticatorIntf::authenticate(
   const char* relPath, HttpCommand* cmd) {
   return AuthenticatorIntf_authenticate(this, relPath, cmd);
}
#endif


/** This callback function is called if the user failed to
    authenticate with one of DigestAuthenticator, BasicAuthenticator,
    or FormAuthenticator. The service function must send an appropriate
    error message to the client.

    The callback is also called when a FormAuthenticator instance
    needs to send the form login page to the client. This callback can
    detect the difference between sending the login page and the error
    page by checking info->username. This variable is NULL when the
    callback must send the login page.
 
    @param intf Required application login-response interface.
    @param info Required borrowed authentication record for this call. Built-in
    authenticator calls provide cmd for sending the response. The callback has
    no return value; it communicates by writing the response.
*/
typedef void (*LoginRespIntf_Service)(struct LoginRespIntf* intf,
                                      struct AuthInfo* info);

/** The LoginRespIntf is an abstract class, which must be implemented
    when using one of DigestAuthenticator, BasicAuthenticator, and
    FormAuthenticator.

    The Barracuda authenticators call the service method if the user
    is not authenticated or failed to login. The service method must
    respond by sending a message to the client.
*/
typedef struct LoginRespIntf
{
#ifdef __cplusplus
      LoginRespIntf() {}
      /** Install the required login-response callback.
          \param service a pointer to the response service callback function.
      */
      LoginRespIntf(LoginRespIntf_Service service);
#endif
      LoginRespIntf_Service serviceFp;
} LoginRespIntf;

/** Install the callback used by LoginRespIntf.
    @param service Required callback; remains callable while installed.
    @param o Required storage to initialize.
 */
#define LoginRespIntf_constructor(o, service) (o)->serviceFp=service
#ifdef __cplusplus
inline LoginRespIntf::LoginRespIntf(LoginRespIntf_Service service) {
   LoginRespIntf_constructor(this, service); }
#endif


/** AuthInfo Credential Type can optionally be used by the
 * #UserIntf_GetPwd callback function.
 */
typedef enum {
   /** The default. Password is returned in plaintext. */
   AuthInfoCT_Password=5,
   /** The password is returned as a HA1 hash, which is:
       MD5(username ":" realm ":" password)
   */
   AuthInfoCT_HA1,
   /** Set when getpwd callback successfully compared AuthInfo::upwd
    * with stored password.
    */
   AuthInfoCT_Valid,

   /** Set when getpwd callback failed comparing AuthInfo::upwd
    * with stored password.
    */
   AuthInfoCT_Invalid
} AuthInfoCT;


/**  An instance of the AuthInfo struct is created on the stack in the
     Barracuda authenticators and is used as a container object for
     sending information to the registered user callback methods.
*/
typedef struct AuthInfo
{
   /** Borrowed optional tracker; NULL when tracking is disabled. */
   struct LoginTracker* tracker;

   /** Borrowed command for normal authentication. May be NULL for a direct user-database lookup; check before accessing HTTP state. */
   HttpCommand* cmd;

   /** Borrowed NUL-terminated input user name, or NULL when no login was supplied. Valid only during the callback flow. */
   const char* username;

   /** Borrowed supplied password when available for Basic/Form; NULL when unavailable, including Digest. Valid only during the callback flow. */
   const char* upwd;

   /** Borrowed authenticated user when available, otherwise NULL. Initially NULL during database lookup. */
   AuthenticatedUser* user;

   /** Input authentication mechanism; Unknown is permitted for direct database lookup. Use this to interpret available credential fields. */
   AuthenticatedUserType type;
   
   /** Credential interpretation selected by UserIntf_GetPwd. Initially AuthInfoCT_Password. HA1 requires a 32-character hexadecimal MD5 value in password. Valid/Invalid report a callback comparison for Basic/Form; they cannot replace Digest credential data. */
   AuthInfoCT ct;

   /** Application pointer shared between callbacks in this authentication flow; initially NULL. BAS neither owns nor frees it. */
   void* userObj;

   AuthUserList* authUserList;

   /** This variable specifies the time, in seconds, between client
       requests before the user is automatically logged out. The
       default value for HttpSession is used if this value is
       zero. The default value can be changed in
       UserIntf_GetPwd. The authenticator calls method
       HttpSession::setMaxInactiveInterval if this variable has a
       non-zero value.
   */
   BaTime maxInactiveInterval;

   U32 seed;
   U32 seedKey;

   /** Maximum concurrent logins for the user; initially 3. UserIntf_GetPwd may set a positive value. Values less than 1 prevent a new login. */
   int maxUsers;

   /** Tracker-derived login counter difference when access is denied; initially zero. It is not updated on every callback path. */
   int loginAttempts;

   /** Initially FALSE; set TRUE when the tracker validation callback denies a cached address. */
   BaBool denied;

   /** Initially FALSE. UserIntf_GetPwd may set TRUE to permit removal of an existing unlocked session when the login limit is reached. This does not guarantee that an eligible session exists. */
   BaBool recycle;

   /** Output credential storage, initially empty. For Password, copy a nonempty NUL-terminated password of at most 99 bytes. For HA1, store the 32 hexadecimal MD5 characters and a terminator. Leave empty for a missing user. Basic/Form ct=Valid can instead report a successful comparison without returning the stored password. Do not infer user existence solely from this buffer in every credential mode. */
   U8 password[100];
} AuthInfo;

/** Initialize an authentication record with zeroed optional fields.
    @param o Required writable record.
    @param trackerMA Borrowed tracker, or NULL.
    @param cmdMA Borrowed command, or NULL for lookup without a request.
    @param typeMA AuthenticatedUserType describing the lookup.
    Sets maxUsers=3 and ct=AuthInfoCT_Password; stores no owned pointers.
 */
#define AuthInfo_constructor(o, trackerMA, cmdMA, typeMA) do {\
   memset(o, 0, sizeof(AuthInfo));\
   (o)->tracker=trackerMA;\
   (o)->cmd=cmdMA;\
   (o)->type=typeMA;\
   (o)->maxUsers=3;\
   (o)->ct=AuthInfoCT_Password;\
   (o)->password[0]=0;\
} while(0)

/** Prototype for the validate callback method.  The validate callback
method is called before attempting to authorize a user. The validate
callback method can keep track of the login counter in the
LoginTrackerNode and either accepts or denies the user. The method
should return true if the request is accepted and false if the request
is denied. Attribute info.denied is set by the LoginTracker if this
method returns false.
 
    @param o Required callback interface.
    @param info Required borrowed authentication input/output record.
    @param node Required cached address node, borrowed for this call.
    @return TRUE to permit the attempt, FALSE to deny it. This callback is called
    only for an address already present in the cache.
*/
typedef BaBool (*LoginTrackerIntf_Validate)(
   struct LoginTrackerIntf* o,
   AuthInfo* info,
   struct LoginTrackerNode* node);


/** Prototype for the Login tracker method.
    The Login method is called when a user is authenticated.
    \param o the object
    \param info The AuthInfo container object.
    \param node is borrowed and may be NULL if the address is not cached. This object
    is automatically terminated as soon as this callback returns;
    i.e., the terminate callback is called.
 */
typedef void (*LoginTrackerIntf_Login)(
   struct LoginTrackerIntf* o,
   AuthInfo* info,
   struct LoginTrackerNode* node);

/** Prototype for the LoginFailed callback method.
The LoginFailed method is called when a user attempts to log in and
the user name, password, or both are incorrect.

One can potentially
<a href="https://en.wikipedia.org/wiki/Tarpit_(computing)">tarpit</a>
the failed login attempt if you run the HTTP server in threaded mode,
but a short "login window" is probably more than sufficient in most
applications. The "login window" length is controlled in the
LoginTrackerIntf_Validate callback method.

 
    @param o Required callback interface.
    @param info Required borrowed authentication record.
    @param node Required cached node after its counter and time are updated.
    The callback returns no value and does not own the node.
*/
typedef void (*LoginTrackerIntf_LoginFailed)(
   struct LoginTrackerIntf* o,
   AuthInfo* info,
   struct LoginTrackerNode* node);

/** Prototype for the TerminateNode callback method.
The TerminateNode method is called when the LoginTracker reuses a node
in the internal node cache. The TerminateNode method can be used for
clearing/releasing any data set with method
LoginTrackerNode::setUserData.
 
    @param o Required callback interface.
    @param node Required node about to leave the cache or be reused. Release any
    application-owned userData here, but do not free the tracker-owned node.
    Also called by clearCache() and the tracker destructor.
*/
typedef void (*LoginTrackerIntf_TerminateNode)(
   struct LoginTrackerIntf* o,
   struct LoginTrackerNode* node);


/** The interface between the LoginTracker and the application code.
You must inherit and implement the callback methods required for the
LoginTrackerIntf.
 */
typedef struct LoginTrackerIntf
{
#ifdef __cplusplus
      /** Install four required callbacks; none may be NULL.
          \param validate validate a user.
          \param login A user successfully logged in.
          \param loginFailed The login attempt failed.
          \param terminateNode The LoginTrackerNode is recycled.
      */
      LoginTrackerIntf(LoginTrackerIntf_Validate validate,
                       LoginTrackerIntf_Login login,
                       LoginTrackerIntf_LoginFailed loginFailed,
                       LoginTrackerIntf_TerminateNode terminateNode);
#endif
      LoginTrackerIntf_Validate validate;
      LoginTrackerIntf_Login login;
      LoginTrackerIntf_LoginFailed loginFailed;
      LoginTrackerIntf_TerminateNode terminateNode;
} LoginTrackerIntf;

/** Install required tracker callbacks; no callback may be NULL.
    @param o Required interface storage.
    @param validateMA LoginTrackerIntf_Validate callback.
    @param loginMA LoginTrackerIntf_Login callback.
    @param loginFailedMA LoginTrackerIntf_LoginFailed callback.
    @param terminateNodeMA LoginTrackerIntf_TerminateNode callback.
 */
#define LoginTrackerIntf_constructor(\
 o, validateMA, loginMA, loginFailedMA, terminateNodeMA) do {\
   (o)->validate=validateMA;\
   (o)->login=loginMA;\
   (o)->loginFailed=loginFailedMA;\
   (o)->terminateNode=terminateNodeMA;\
} while(0)
/** Invoke the corresponding LoginTrackerIntf callback synchronously.
    @param o Required initialized callback interface.
    @param request Required AuthInfo pointer (not HttpRequest).
    @param node Required cached LoginTrackerNode.
    @return Callback result: TRUE permits, FALSE denies.
 */
#define LoginTrackerIntf_validate(o, request, node) \
  (o)->validate(o, request, node)
/** Invoke the corresponding LoginTrackerIntf callback synchronously.
    @param o Required initialized callback interface.
    @param request Required AuthInfo pointer.
    @param user Borrowed LoginTrackerNode pointer, or NULL, despite the argument name.
 */
#define LoginTrackerIntf_login(o, request, user) \
  (o)->login(o, request, user)
/** Invoke the corresponding LoginTrackerIntf callback synchronously.
    @param o Required initialized callback interface.
    @param node Required AuthInfo pointer, despite the argument name.
    @param loginName Required LoginTrackerNode pointer, not a string.
 */
#define LoginTrackerIntf_loginFailed(o, node, loginName) \
  (o)->loginFailed(o, node, loginName)
/** Invoke the corresponding LoginTrackerIntf callback synchronously.
    @param o Required initialized callback interface.
    @param node Required node whose application data must be released.
 */
#define LoginTrackerIntf_terminateNode(o, node) \
  (o)->terminateNode(o, node)

#ifdef __cplusplus
inline LoginTrackerIntf::LoginTrackerIntf(
   LoginTrackerIntf_Validate validate,
   LoginTrackerIntf_Login login,
   LoginTrackerIntf_LoginFailed loginFailed,
   LoginTrackerIntf_TerminateNode terminateNode) {
   LoginTrackerIntf_constructor(this,validate,login,loginFailed,terminateNode);
}
#endif


/** A LoginTrackerNode keeps track of how many times a user using a
specific IP address has attempted to login to the server.  The
LoginTracker stores LoginTrackerNodes internally in a cache.
 */
typedef struct LoginTrackerNode
{
#ifdef __cplusplus
      /** Query the address failure/denial counter.
    @return U32 count maintained by the tracker, initially zero for a new node.
 */
      U32 getCounter();

      /** Query the application auxiliary counter.
    @return Last stored U32 value, initially zero for a new node.
 */
      U32 getAuxCounter();

      /** Set the application auxiliary counter.
    @param count U32 value, used as the baseline subtracted from loginCounter when populating denied-attempt information.
 */
      void setAuxCounter(U32 count);

      /** Access the cached peer IP address.
    @return Borrowed pointer valid while this node remains cached. Do not modify the address used as the tree key.
 */
      HttpSockaddr* getAddr();

      /** Associate application data with the node.
    @param data Borrowed application pointer, or NULL. Replacing it does not free the old value. Release owned data through the terminateNode callback.
 */
      void setUserData(void* data);

      /** Query application data.
    @return Last stored pointer, initially NULL; ownership remains with the application.
 */
      void* getUserData();

      /** Query the latest recorded failed or denied attempt.
    @return Unix time in seconds, as provided by baGetUnixTime().
 */
      BaTime getTime();
#endif
      SplayTreeNode super;
      DoubleLink dlink;
      HttpSockaddr addr;
      void* userData;
      BaTime t;
      U32 loginCounter;
      U32 auxCounter;
} LoginTrackerNode;

/** @copydoc LoginTrackerNode::getCounter
    @param o Required live tracker node.
 */
#define LoginTrackerNode_getCounter(o) (o)->loginCounter
/** @copydoc LoginTrackerNode::getAuxCounter
    @param o Required live tracker node.
 */
#define LoginTrackerNode_getAuxCounter(o) (o)->auxCounter
/** @copydoc LoginTrackerNode::setAuxCounter
    @param o Required live tracker node.
 */
#define LoginTrackerNode_setAuxCounter(o, count) (o)->auxCounter=count
/** @copydoc LoginTrackerNode::getAddr
    @param o Required live tracker node.
 */
#define LoginTrackerNode_getAddr(o) (&(o)->addr)
/** @copydoc LoginTrackerNode::setUserData
    @param o Required live tracker node.
 */
#define LoginTrackerNode_setUserData(o, data) (o)->userData=data
/** @copydoc LoginTrackerNode::getUserData
    @param o Required live tracker node.
 */
#define LoginTrackerNode_getUserData(o) (o)->userData
/** @copydoc LoginTrackerNode::getTime
    @param o Required live tracker node.
 */
#define LoginTrackerNode_getTime(o) (o)->t

#ifdef __cplusplus
inline U32 LoginTrackerNode::getCounter() {
   return LoginTrackerNode_getCounter(this);
}
inline U32 LoginTrackerNode::getAuxCounter() {
   return LoginTrackerNode_getAuxCounter(this);
}

inline void LoginTrackerNode::setAuxCounter(U32 count) {
   LoginTrackerNode_setAuxCounter(this, count);
}

inline HttpSockaddr* LoginTrackerNode::getAddr() {
   return LoginTrackerNode_getAddr(this);
}
inline void LoginTrackerNode::setUserData(void* data) {
   LoginTrackerNode_setUserData(this, data);
}
inline void* LoginTrackerNode::getUserData() {
   return  LoginTrackerNode_getUserData(this);
}
inline BaTime LoginTrackerNode::getTime() {
   return LoginTrackerNode_getTime(this);
}
#endif

/** The LoginTracker class is an optional security enhancement that
    can be installed in an instance of one of the authenticator
    classes.

The tracker caches failed attempts by peer IP address and delegates the decision
    to allow another attempt to application callbacks. It does not provide a
    built-in retry policy. A full cache reuses its oldest inserted active node.
    Serialize access with the server mutex and keep callbacks/dependencies alive.
*/
typedef struct LoginTracker
{
#ifdef __cplusplus
      /** Allocate a fixed cache of address records.
    @param noOfLoginTrackerNodes Requested positive cache capacity.
    @param intf Required borrowed interface containing four non-NULL callbacks.
    @param allocator Required allocation interface; the C++ default is
    AllocatorIntf::getDefault(). NULL does not select a default in the C function.
    Allocation failure calls baFatalE(FE_MALLOC, 0); there is no error return.
    @warning The current implementation allocates one fewer node than the capacity
    it uses and frees node storage with baFree rather than the supplied allocator.
    These implementation limitations require a separate source repair.
    The C++ interface has no automatic cleanup destructor; after detaching users,
    call LoginTracker_destructor() once.
 */
      LoginTracker(U32 noOfLoginTrackerNodes,
                   LoginTrackerIntf* intf,
                   AllocatorIntf* allocator = AllocatorIntf::getDefault());

      /** Remove all active cached addresses.
    Invokes terminateNode for each active node and retains storage for reuse.
    Previously returned node pointers must no longer be used as cached entries.
 */
      void clearCache();

      /** Start iteration over active cached addresses in insertion order.
    @return Borrowed first node, or NULL when empty. Keep the cache unchanged
    during iteration; pointers may be reused after login, clearing, or recycling.
 */
      LoginTrackerNode* getFirstNode();

      /** Advance through active cached addresses.
    @param n Required node currently in this tracker's active list.
    @return Borrowed next node, or NULL at the end. Do not pass NULL or a stale node.
 */
      LoginTrackerNode* getNextNode(LoginTrackerNode* n);

      /** Find a cached address using the current connection's peer IP.
    @param request Required request with its current connection.
    @return Borrowed node, or NULL when not cached or peer lookup fails.
 */
      LoginTrackerNode* find(HttpRequest* request);
#endif
      SplayTree tree;
      DoubleList dInUseList;
      DoubleList dFreeList;
      LoginTrackerIntf* loginTrackerIntf;
      U32 cursor;
      U32 noOfLoginTrackerNodes;
      LoginTrackerNode* nodes;
} LoginTracker;


#ifdef __cplusplus
extern "C" {
#endif
/** @copydoc LoginTracker::LoginTracker
    @param o Required storage to initialize.
 */
BA_API void LoginTracker_constructor(LoginTracker* o,
                                     U32 noOfLoginTrackerNodes,
                                     LoginTrackerIntf* intf,
                                     AllocatorIntf* allocator);
/** Release a tracker after detaching all users.
    @param o Required initialized tracker. Calls clearCache(), then baFree on its node storage. Does not free callback interfaces.
 */
BA_API void LoginTracker_destructor(LoginTracker* o);
/** @copydoc LoginTracker::clearCache
    @param o Required initialized tracker.
 */
BA_API void LoginTracker_clearCache(LoginTracker* o);
/** @copydoc LoginTracker::getFirstNode
    @param o Required initialized tracker.
 */
BA_API LoginTrackerNode* LoginTracker_getFirstNode(LoginTracker* o);
/** @copydoc LoginTracker::getNextNode
    @param o Required initialized tracker.
 */
BA_API LoginTrackerNode* LoginTracker_getNextNode(
   LoginTracker* o, LoginTrackerNode* n);
/** C form of LoginTracker::find.
    @param o Required tracker.
    @param req Required request.
    @return Borrowed node, or NULL if absent or peer lookup fails.
 */
BA_API LoginTrackerNode* LoginTracker_find(LoginTracker*o, HttpRequest* req);
/** Record a failed login, inserting or recycling an address node as needed.
    @param o Required initialized tracker.
    @param info Required authentication record with non-NULL cmd.
    Updates the counter/time before calling loginFailed. If peer lookup fails,
    marks the connection terminated. No error value is returned.
 */
BA_API void LoginTracker_loginFailed(
   LoginTracker* o, AuthInfo* info);
/** Check whether a cached peer may attempt authentication.
    @param o Required initialized tracker.
    @param info Required authentication record with non-NULL cmd.
    @return TRUE for an uncached address or callback acceptance; FALSE for callback
    denial or peer lookup failure. Denial updates denied/loginAttempts and the
    node counter/time. Peer lookup failure sends an HTTP 501 response.
 */
BA_API BaBool LoginTracker_validate(LoginTracker* o, AuthInfo* info);
/** Notify successful authentication and remove any cached peer entry.
    @param o Required initialized tracker.
    @param info Required authentication record with non-NULL cmd.
    Calls login with a node or NULL, then terminateNode for an existing node
    before recycling it. Callback pointers are borrowed and must not be retained.
 */
BA_API void LoginTracker_login(LoginTracker* o, AuthInfo* info);
#ifdef __cplusplus
}
inline LoginTracker::LoginTracker(U32 noOfLoginTrackerNodes,
                                  LoginTrackerIntf* intf,
                                  AllocatorIntf* allocator) {
   LoginTracker_constructor(this, noOfLoginTrackerNodes, intf, allocator);
}
inline void LoginTracker::clearCache() {
   LoginTracker_clearCache(this);
}
inline LoginTrackerNode* LoginTracker::getFirstNode() {
   return LoginTracker_getFirstNode(this);
}
inline LoginTrackerNode* LoginTracker::getNextNode(LoginTrackerNode* n) {
   return LoginTracker_getNextNode(this,n);
}
inline LoginTrackerNode* LoginTracker::find(HttpRequest* request) {
      return LoginTracker_find(this,request);
}
#endif


/** @} */ /* end of Authentication */

/* Internal func */
void calculateHA1Hex(
   const char* realm, const char* uname, const char* pwd, U8 hexbuf[33]);

#endif
