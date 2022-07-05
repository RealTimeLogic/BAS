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
 *   $Id: AuthenticatedUser.h 5201 2022-07-04 13:57:29Z wini $
 *
 *   COPYRIGHT:  Real Time Logic LLC, 2003 - 2022
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

    \sa <a href="../../index.html" _target="top"> Barracuda Introduction </a>

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

    \param user. A reference to the authenticated user. The method
    must return false if user is NULL.

    \param httpMethod The HTTP method type: From HttpRequest::getMethodType

    \param path the path to the requested resource.
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
          \param authorize Pointer to authorize method.
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

#define AuthorizerIntf_constructor(o, authorize) (o)->authorizeFP=authorize
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
  The GetPwd callback function searches for info->username and sets
  AuthInfo::password if found and/or the AuthInfo::ct parameter.

  info->userObj is NULL, but can be set in this callback to signal
  information to the other callbacks such as
  LoginRespIntf_Service.

  info->user is NULL when this method is called.
  
  The method is allowed to set header values and work with the
  response object.  The authenticator stops authentication and returns
  FALSE if the response object is committed; i.e., the login fails.

  The authenticator checks if the response is committed on return.
  The authenticator assumes the user is not authenticated if the
  response is committed.
*/
typedef void (*UserIntf_GetPwd)(struct UserIntf* intf,struct AuthInfo* info);

/**  Interface class used by the Authentication classes.
 *   The getPwd function returns the user's password if user found.
 */
typedef struct UserIntf
{
#ifdef __cplusplus

      UserIntf() {}
      
      /** The UserIntf constructor.
          \param getPwd a pointer to the get password callback function.
      */
      UserIntf(UserIntf_GetPwd getPwd);

#endif
      UserIntf_GetPwd getPwdFp;
} UserIntf;

#ifdef __cplusplus
extern "C" {
#endif

#define UserIntf_constructor(o, getPwd) (o)->getPwdFp = getPwd

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

int AuthUserList_createOrCheck(struct AuthInfo* info,
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

      /** Returns a pointer to an instance of class AuthenticatedUser if
          a session object exists and if the user is authenticated,
          otherwise NULL is returned.

          \code
          AuthenticatedUser* user;
          user = AuthenticatedUser::get(reguest);
          \endcode

          C name: AuthenticatedUser_get1
      */ 
      static AuthenticatedUser* get(HttpRequest* request);


      /** Returns a pointer to an instance of class AuthenticatedUser if
          the user is authenticated, otherwise NULL is returned.

          \code
          AuthenticatedUser* user;
          user = AuthenticatedUser::get(reguest->getSession(false));
          \endcode

          C name: AuthenticatedUser_get2

          \sa HttpServer::getSession
      */
      static AuthenticatedUser* get(HttpSession* session);

      /** Get the session object.

      \sa HttpSession::getId
       */
      HttpSession* getSession();

      /** Returns the user's password
       */
      const char* getPassword();

      /** Returns the user's name
       */
      const char* getName();

      /** Logout user and terminate the session object. You should use
        this method and not HttpSession:terminate when logging out a
        user.

        <b>Example</b>
        \code
        // The following code works if AuthenticatedUser::get returns NULL.
        AuthenticatedUser::get(request)->logout();
        \endcode

        <b>Basic and Digest logout:</b>

        With Basic and Digest authentication, the browser remembers
        the user and password. This means that this method will not
        have any effect with Basic and Digest since the user will be
        automatically logged in as soon as the browser sends a request
        to the server.

        The only viable solution to logging out using Basic and Digest
        is to terminate the browser client.

        \param all Set to true if you want to terminate all of the
        user's active sessions. A user may be logged in using more
        than one client. One must typically set all=true when changing
        password as all clients must be terminated or the clients may
        get a 403 response.
       */
      void logout(bool all=false);

      /** Returns the authenticator type that was used to authenticate
          this user.
          \sa attribute type in AuthInfo.
       */
      AuthenticatedUserType getType();

      /** non public
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
BA_API AuthenticatedUser* AuthenticatedUser_get1(HttpRequest* request);
BA_API AuthenticatedUser* AuthenticatedUser_get2(HttpSession* session);
#define AuthenticatedUser_getName(o) \
  ((o) && (o)->authUserList && (o)->authUserList->username ? \
  (o)->authUserList->username : 0)
#define AuthenticatedUser_getDerivedType(o) (o)->derivedType
#define AuthenticatedUser_getSession(o) \
   HttpSessionAttribute_getSession((HttpSessionAttribute*)o)
#define AuthenticatedUser_getPassword(o) \
   ((o) && (o)->authUserList && (o)->authUserList->password ? \
    (o)->authUserList->password : 0)
BA_API void AuthenticatedUser_logout(AuthenticatedUser* o, BaBool all);
BA_API AuthenticatedUserType AuthenticatedUser_getType(AuthenticatedUser* o);
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
    \param cmd The HttpRequest HttpResponse container.
    \return The AuthenticatedUser if authenticated, otherwise NULL is returned.
 */
typedef AuthenticatedUser* (*AuthenticatorIntf_Authenticate)(
   struct AuthenticatorIntf* super,
   const char* relPath,
   HttpCommand* cmd);


/** Abstract interface class implemented by DigestAuthenticator,
 *  FormAuthenticator and DigestAuthenticator.
 */ 
typedef struct AuthenticatorIntf
{
#ifdef __cplusplus
      /*Only to be used as default constructor when sub-classing with C code*/
      AuthenticatorIntf(){}

      /** The constructor is used by one of the BasicAuthenticator,
       * DigestAuthenticator or FormAuthenticator constructors.
       */
      AuthenticatorIntf(AuthenticatorIntf_Authenticate authenticate);

      /** Authenticate the user.
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

#define AuthenticatorIntf_authenticate(o, relPath, cmd) \
  (o)->authenticateCB(o, relPath, cmd)

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
      /** The UserIntf constructor.
          \param service a pointer to the response service callback function.
      */
      LoginRespIntf(LoginRespIntf_Service service);
#endif
      LoginRespIntf_Service serviceFp;
} LoginRespIntf;

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
   /** The tracker object if any. */
   struct LoginTracker* tracker;

   /** The request/response container object. */
   HttpCommand* cmd;

   /** The user name if the client sends login information. */
   const char* username;

   /** The password provided by the user, if any. */
   const char* upwd;

   /** The authenticated user object if the user is
    * authenticated. */
   AuthenticatedUser* user;

   /** The AuthenticatedUser type. This is one of
       AuthenticatedUserType_Basic, AuthenticatedUserType_Digest, or
       AuthenticatedUserType_Form. You can use this type information
       if you need to upcast the "user" variable.
       \sa AuthenticatedUser::getType
   */
   AuthenticatedUserType type;
   
   /** The ct parameter can optionally be set by the #UserIntf_GetPwd
       function.  Must be set to AuthInfoCT_HA1 if *
       AuthInfo::password is set to a HA1 hash or to AuthInfoCT_Valid
       if the callback verifies (compares) the password *
       AuthInfo::upwd with the stored password.
   */
   AuthInfoCT ct;

   /** An object that can be set by the callbacks for exchanging
       information.
   */
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

   /** Max number of concurrent logins for this user. This value
    * defaults to 1. The UserIntf_GetPwd can change this
    * value.
    */
   int maxUsers;

   /** Shows the number of login attempts if the LoginTracker is active.
       \sa denied
   */
   int loginAttempts;

   /** Flag set if the user is denied access by the LoginTracker.
       \sa loginAttempts
   */
   BaBool denied;

   /** Force another client to log out.
       A user is normally prevented from logging in if the number of
       connected clients is larger than 'maxUsers'. This flag can be
       set by the UserIntf_GetPwd to force one of the other
       clients to automatically log out.
   */
   BaBool recycle;

   /**  Must be set by UserIntf_GetPwd if the user is
        found. This variable is by default set to '\\0' and should
        not be changed by UserIntf_GetPwd if the user is not
        found. The LoginRespIntf callback can from this variable
        detect if the username is not found or if the password is
        wrong. The LoginRespIntf callback can assume the password
        provided by the user was wrong if this variable is set.
        
        Warning: max password length is 99 characters + '\\0'
   */
   U8 password[100];
} AuthInfo;

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
 */
typedef BaBool (*LoginTrackerIntf_Validate)(
   struct LoginTrackerIntf* o,
   AuthInfo* info,
   struct LoginTrackerNode* node);


/** Prototype for the Login tracker method.
    The Login method is called when a user is authenticated.
    \param info The AuthInfo container object.
    \param node may be be NULL if the object was recycled. This object
    is automatically terminated as soon as this callback returns;
    i.e., the terminate callback is called.
 */
typedef void (*LoginTrackerIntf_Login)(
   struct LoginTrackerIntf* o,
   AuthInfo* info,
   struct LoginTrackerNode* node);

/** Prototype for the LoginFailed callback method.
The LoginFailed method is called when a user attempts to log in and
the user and/or password is incorrect.

One can potentially
<a href="http://en.wikipedia.org/wiki/Tarpit_(computing)">tarpit</a>
the failed login attempt if you run the HTTP server in threaded mode,
but a short "login window" is probably more than sufficient in most
applications. The "login window" length is controlled in the
LoginTrackerIntf_Validate callback method.

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
      /** Create a LoginTrackerIntf.
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

#define LoginTrackerIntf_constructor(\
 o, validateMA, loginMA, loginFailedMA, terminateNodeMA) do {\
   (o)->validate=validateMA;\
   (o)->login=loginMA;\
   (o)->loginFailed=loginFailedMA;\
   (o)->terminateNode=terminateNodeMA;\
} while(0)
#define LoginTrackerIntf_validate(o, request, node) \
  (o)->validate(o, request, node)
#define LoginTrackerIntf_login(o, request, user) \
  (o)->login(o, request, user)
#define LoginTrackerIntf_loginFailed(o, node, loginName) \
  (o)->loginFailed(o, node, loginName)
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
      /** Returns the number of login attempts.
       */
      U32 getCounter();

      /** Get auxiliary counter.
       */
      U32 getAuxCounter();

      /** Set auxiliary counter.
       */
      void setAuxCounter(U32 count);

      /** Return the HttpSockaddr.
       */
      HttpSockaddr* getAddr();

      /** Store application data in this node.
       */
      void setUserData(void* data);

      /** Fetch stored application data in this node.
       */
      void* getUserData();

      /** Returns the time for the latest login attempt.
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

#define LoginTrackerNode_getCounter(o) (o)->loginCounter
#define LoginTrackerNode_getAuxCounter(o) (o)->auxCounter
#define LoginTrackerNode_setAuxCounter(o, count) (o)->auxCounter=count
#define LoginTrackerNode_getAddr(o) (&(o)->addr)
#define LoginTrackerNode_setUserData(o, data) (o)->userData=data
#define LoginTrackerNode_getUserData(o) (o)->userData
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

A hacker may write an automated password cracker tool to exploit weak
passwords. This is often referred to as a
<a href=" http://en.wikipedia.org/wiki/Dictionary_attack" >
Dictionary attack</a>. 

The purpose with the LoginTracker is to make it virtually impossible
to perform dictionary attacks on the Barracuda authenticator
classes. The LoginTracker keeps track of IP addresses that failed to
supply the correct user and/or password.  The LoginTracker keeps a
cache of LoginTrackerNode instances where each LoginTrackerNode
stores information such as IP address and time of login attempt.

You can download and try the
<a href="https://FuguHub.com">FuguHub</a>
demo for a practical example of  using the LoginTracker. Enter an
invalid user/password when logging into the FuguHub Web Server and
observe the response message.
*/
typedef struct LoginTracker
{
#ifdef __cplusplus
      /** Create a LoginTracker instance.
          \param noOfLoginTrackerNodes size of internal LoginTrackerNode cache.
          \param intf the application interface used by the LoginTracker.
          \param allocator the allocator uses when allocating the LoginTracker
          nodes.
      */
      LoginTracker(U32 noOfLoginTrackerNodes,
                   LoginTrackerIntf* intf,
                   AllocatorIntf* allocator = AllocatorIntf::getDefault());

      /** Clear the LoginTrackerNode cache.
       */
      void clearCache();

      /** Return first LoginTrackerNode in the cache or NULL if cache empty.
       */
      LoginTrackerNode* getFirstNode();

      /** Return the next LoginTrackerNode in the cache or NULL if no
       * more nodes.
       */
      LoginTrackerNode* getNextNode(LoginTrackerNode* n);

      /** Find LoginTrackerNode in cache by using the IP address from
          the request object.
       \returns the LoginTrackerNode if the user is in the cache or
       NULL if the user either never has failed a login attempt or the
       user is authenticated.
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
BA_API void LoginTracker_constructor(LoginTracker* o,
                                     U32 noOfLoginTrackerNodes,
                                     LoginTrackerIntf* intf,
                                     AllocatorIntf* allocator);
BA_API void LoginTracker_destructor(LoginTracker* o);
BA_API void LoginTracker_clearCache(LoginTracker* o);
BA_API LoginTrackerNode* LoginTracker_getFirstNode(LoginTracker* o);
BA_API LoginTrackerNode* LoginTracker_getNextNode(
   LoginTracker* o, LoginTrackerNode* n);
BA_API LoginTrackerNode* LoginTracker_find(LoginTracker*o, HttpRequest* req);
BA_API void LoginTracker_loginFailed(
   LoginTracker* o, AuthInfo* info);
BA_API BaBool LoginTracker_validate(LoginTracker* o, AuthInfo* info);
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
