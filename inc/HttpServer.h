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
 *   $Id: HttpServer.h 5412 2023-03-17 01:49:04Z wini $
 *
 *   COPYRIGHT:  Real Time Logic LLC, 2003 - 2023
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

#define BASLIB_VER_NO 5583
#define BASLIB_VER_M(x) #x
#define BASLIB_VER BASLIB_VER_M(5583)

/*! \page HttpDirVolatileMem Volatile/temporary memory used as name in a HttpDir/HttpPage

The name passed in as argument to a HttpDir/HttpPage object cannot be
changed since the constructor only stores a pointer to the name. The
virtual file system is designed to use as little memory as possible.
Thus, the name passed in as argument to the HttpDir/HttpPage
constructor is not copied; only the pointer reference is copied.

One would normally use constants as the name argument, but if you need
to create a name dynamically, you can allocate space for the structure
and the name as one piece as follows:

\code
HttpDir* createDir(char* name)
{
   HttpDir* dir = baMalloc(sizeof(HttpDir) + strlen(name) + 1);
   if(dir)
   {
      char* ptr = (char*)(dir+1);
      strcpy(ptr, name);
      HttpDir_constructor(dir, ptr, 0);
   }
   return dir;
}
\endcode

We use a HttpDir in the example above, but it also applies to a
HttpPage, HttpResRdr, AuthenticateDir, EvDir or any class derived
from HttpPage or HttpDir.
*/


#ifndef __HttpServer_h
#define __HttpServer_h
#include "TargConfig.h"
#include "HttpConnection.h"
#include "DoubleList.h"
#include "SoDisp.h"
#include "SplayTree.h"
#include "BaErrorCodes.h"
#include "SplayTree.h"
#include "BufPrint.h"
#include "IoIntf.h"
#include <string.h>
#include <stddef.h>

/* Used by examples */
#ifndef BA_STACKSZ
#define BA_STACKSZ 24000
#endif


/* This ID must match the ID in LoginKey.java */
#define BA_COOKIE_ID "z9ZAqJtI"

#ifndef __DOXYGEN__
struct HttpRequest;
struct HttpResponse;
struct HttpPage;
struct HttpServer;
struct HttpSession;
struct HttpLinkCon;
struct HttpDir;
struct AuthenticatedUser;
struct UserIntf;
struct CspReader;
struct HttpCmdThreadPoolIntf;
struct AuthUserList;
struct AuthorizerIntf;
struct AuthenticatorIntf;
struct LHttpCommand;
#endif


/*  NonBlockingSendBuf is used when sending asynchronous (non blocking
    data).  A pointer to this buffer is stored in SoDispCon.sslData
    for HTTP cons.: this pointer is not used for anything else for a
    non secure socket con..  */
typedef struct
{
      int cursor;
      int bufLen;
      int maxBufLen;
      char buf[1];
} NonBlockingSendBuf;


#ifdef __cplusplus
extern "C" {
#endif
BA_API struct AuthenticatedUser* AuthenticatedUser_get2(
   struct HttpSession* session);
/** Formats the pointer 'buf' with date/time according to RFC 1123
 */
BA_API void httpFmtDate(char* buf, U16 bufLen, BaTime t);
/** Decodes a string encoded with httpEscape.
    Decodes a string containing %[hex][hex].
    Returns the last character -- i.e. the character before \0.
*/
BA_API char* httpUnescape(char* from);
/** Encodes characters that cannot be in a http URL.
 * "out" should be 3 times the size of "in"
 */
BA_API char* httpEscape(char* out, const char* in);
#ifdef __cplusplus
}
#endif


typedef void (*UserDefinedErrHandler)(BaFatalErrorCodes ecode1,
                                      unsigned int ecode2,
                                      const char* file,
                                      int line);


/** @defgroup CSP C Server Pages

    C Server Pages make it possible to insert C or C++ code into HTML
    template files by using special tags. The technology is similar to
    other server-side technologies such as JSP, PHP, ASP, etc.

    See the <a href="../../CspTools.html">CSP user manual</a> for more
    information on CSP.

\sa <a href="../../index.html" _target="top"> Barracuda Introduction </a>
@{
*/


/** Function prototype for the "initialize function" generated by CspLink

<a href="../../CspTools.html#CspLink">CspLink</a> groups together CSP
pages and generates a CspInit function. You must call the method at
system startup.

\param cspRoot the position in the virtual files system where you want
the CSP to be installed.
\param reader the interface between the CSP data file and the
generated CSP engine.
*/
typedef void (*CspInit)(
   struct HttpDir* cspRoot,struct CspReader* reader);

/** @} */ /* end of CSP */

/* no longer used. included for compatibility.
 */
struct z_stream_s;
typedef int (*ZlibInflateInit2)(struct z_stream_s* s, int  windowBits,
                     const char *version, int stream_size);

typedef int (*ZlibInflate)(struct z_stream_s* s, int flush);
typedef int (*ZlibInflateEnd)(struct z_stream_s* s);


#ifndef __DOXYGEN__

typedef struct
{
      char* buf;
      U16 size;
      U16 index;
} HttpAllocator;


/** HttpHeader iterator: see HttpRequest::getHeaders for more information.
 */
typedef struct HttpHeader
{
#ifdef __cplusplus
      const char* name(HttpRequest* req);
      const char* value(HttpRequest* req);
#endif
      U16 nameI;
      U16 valueI;
}HttpHeader;

#ifdef __cplusplus
extern "C" {
#endif
BA_API const char* HttpHeader_name(HttpHeader* o, struct HttpRequest* req);
BA_API const char* HttpHeader_value(HttpHeader* o, struct HttpRequest* req);
#ifdef __cplusplus
}
inline const char* HttpHeader::name(HttpRequest* req) {
   return HttpHeader_name(this, req); }
inline const char* HttpHeader::value(HttpRequest* req) {
   return HttpHeader_value(this, req); }
#endif

#endif





/*===========================================================================
 *
 *       HttpInData
 *---------------------------------------------------------------------------
 *  Description:
 */

#ifndef __DOXYGEN__

typedef enum {
   HttpInData_ParseHeader,
   HttpInData_ReadBody,
   HttpInData_ReadBodyAndParseUrlEncData
} HttpInData_ParseState;
#endif


/** The web-server "Request Data" container.
 */
typedef struct HttpInData
{
#ifdef __cplusplus
      /** Get pointer to start of PUT/POST data.
       */
      const char* getBuf();

      /** Get size of internal buffer.  Use
          HttpStdHeaders::getContentLength to get the length of the
          PUT/POST message.
          You must use HttpAsynchRec if getBufSize() < content-length or if
          chunk encoding.
       */
      S32 getBufSize();
#endif
      HttpAllocator allocator;
      struct HttpRequest* request;
      U16 lineStartI;
      U16 lineEndI;
      S16 maxRequest;
      U8 parseState; /* HttpInData_ParseState */
      U8 overflow; /* Used for pipelined requests */
} HttpInData;

#define HttpAllocator_2Ptr(o, index) ((&(o)->buf[index]))
#define HttpInData_lineStartPtr(o) \
  HttpAllocator_2Ptr(&(o)->allocator,(o)->lineStartI)

#define HttpInData_getBufSize(o) \
  ((o)->lineStartI < (o)->allocator.index ? \
  (o)->allocator.index-(o)->lineStartI : 0)

#define HttpInData_getBuf(o) HttpInData_lineStartPtr(o)
BaBool HttpInData_hasMoreData(HttpInData* o);

#ifdef __cplusplus
inline const char* HttpInData::getBuf() {
   return HttpInData_getBuf(this); }
inline S32 HttpInData::getBufSize() {
   return HttpInData_getBufSize(this); }
#endif



/** A persistent container object for HTTP parameters.
    The parameters in an HttpRequest object are valid only for the
    duration of the call. The HttpParameter is a container object for
    the parameters. One can clone the parameters in the request object
    and save the parameters in an HttpParameter object.

    C example:
    \code
    HttpParameter* param=
      HttpParameter_clone(
         baMalloc(HttpParameter_calculateSize(request)),
         request);
    if(param)
       //OK
    else
      // baMalloc failed
    \endcode

    C++ example:
    \code
    HttpParameter* param=
      HttpParameter::clone(
         baMalloc(HttpParameter::calculateSize(request)),
         request);
    if(param)
       //OK
    else
      // baMalloc failed
    \endcode

    All parameters are contained within one memory unit and one can
   simply call baFree(param) when the object is no longer needed.

    You use the HttpParameterIterator for iterating the elements in the object.

    C example:
    \code
    HttpParameterIterator i;
    HttpParameter* param=
      HttpParameter_clone(baMalloc(HttpParameter_calculateSize(req)), req);
    if(param)
    {
       HttpParameterIterator_constructor2(&i, param);
       for(;
          HttpParameterIterator_hasMoreElements(&i) ;
          HttpParameterIterator_nextElement(&i))
       {
          HttpTrace_printf(0,"Param:  %s = %s\n", i.name, i.value);
       }
       baFree(param);
    }
    \endcode

    C++ Example: See HttpParameterIterator.
*/
typedef struct HttpParameter
{
#ifdef __cplusplus
      /** Calculate the HttpParameter size.
       */
      static U32 calculateSize(struct HttpRequest* req);

      /** Copy HTTP parameters to buf and return as a HttpParameter
          object.
      */
      static HttpParameter* clone(void* buf, struct HttpRequest* req);

      /** Returns the value of a request parameter as a const char*,
          or null if the parameter does not exist. Request parameters are
          extra information sent with the request, which are contained in
          the query string or posted form data.

          If you use this method with a multivalued parameter, the value
          returned is equal to the first value in a HttpParameterIterator.
      */
      const char* getParameter(const char* paramName);

   private:
      HttpParameter() {}
#endif
      U16 formLen;
} HttpParameter;


#ifdef __cplusplus
extern "C" {
#endif
BA_API U32 HttpParameter_calculateSize(struct HttpRequest* req);
BA_API HttpParameter* HttpParameter_clone(void* buf, struct HttpRequest* req);
BA_API const char* HttpParameter_getParameter(
   HttpParameter* o,const char* paramName);

#ifdef __cplusplus
}
inline U32 HttpParameter::calculateSize(struct HttpRequest* req) {
   return HttpParameter_calculateSize(req); }
inline HttpParameter* HttpParameter::clone(void* buf, struct HttpRequest* req){
   return HttpParameter_clone(buf, req); }
inline const char* HttpParameter::getParameter(const char* paramName) {
   return HttpParameter_getParameter(this, paramName); }
#endif


/** @defgroup StdWebServer Standard Web-Server classes

A collection of common classes typically used working with server side
scripting.
\sa CSP and HttpResRdr
\sa <a href="../../index.html" _target="top"> Barracuda Introduction </a>
   @{
 */



/** The HttpParameterIterator is used for iterating through the form elements
 * parsed by the HttpServer object.
 * <P>
 * C++ Example:
 * \code
 * HttpParameterIterator i(request);
 * for( ; i.hasMoreElements() ; i.nextElement())
 * {
 *    if( ! strcmp(i.getName(), "visa number") )
 *       visaNumber = i.getValue();
 * }
 * \endcode
 *
 * C Example: See HttpParameter.
 * \sa HttpRequest::getParameter HttpParameter
 */
typedef struct HttpParameterIterator
{
#ifdef __cplusplus
      HttpParameterIterator(){}
      /** This HttpParameterIterator constructor takes a HttpRequest
          object as argument. */
      HttpParameterIterator(HttpRequest* req);

      /** This HttpParameterIterator constructor takes a HttpParameter
          object as argument.

          c name: HttpParameterIterator_constructor2
      */
      HttpParameterIterator(HttpParameter* param);

      /** Advance to the next element */
      void nextElement();
      /** Returns true if more elements. */
      bool hasMoreElements();
      /** Return the form name */
      const char* getName() const { return name; }
      /** Returns the form value */
      const char* getValue() const { return value; }
   private:
#endif
      void* formElemBase;
      U8* dataEntry;
      U16 pos;
      U16 formLen;
      const char* name;
      const char* value;

} HttpParameterIterator;

#ifdef __cplusplus
extern "C" {
#endif

BA_API int HttpParameterIterator_constructor(
   HttpParameterIterator* o, struct HttpRequest* req);
BA_API int HttpParameterIterator_constructor2(HttpParameterIterator* o,
                                              HttpParameter* param);
BA_API void HttpParameterIterator_nextElement(HttpParameterIterator* o);
#define HttpParameterIterator_hasMoreElements(o) ((o)->name != 0)
#define HttpParameterIterator_getName(o) (o)->name
#define HttpParameterIterator_getValue(o) (o)->value


#ifdef __cplusplus
}
inline HttpParameterIterator::HttpParameterIterator(HttpRequest* req) {
   HttpParameterIterator_constructor(this, req); }
inline HttpParameterIterator::HttpParameterIterator(HttpParameter* param) {
   HttpParameterIterator_constructor2(this, param); }
inline void HttpParameterIterator::nextElement() {
   HttpParameterIterator_nextElement(this); }
#endif


/** A cookie is used for exchanging a small amount of information between a
 * HttpPage and a web browser.
 * <P> A cookie's value can uniquely identify a client, so cookies are
 * commonly used for session management.
 * <P>
 * A cookie has a name, a single value, and optional attributes such as: a
 * comment, path and domain qualifiers, a maximum age, and a version number.
 * <P>
 * The cookie implementation is almost identical to the cookie
 * implementation in the <A HREF=
 * "http://java.sun.com/j2ee/sdk_1.3/techdocs/api/javax/servlet/http/Cookie.html">
 * JavaTM  Enterprise Edition</A>
 * <P> <B>Typical usage:</B>
 * \code
 * HttpCookie* cookie = request->getCookie("myCookie");
 * if( ! cookie ) //If no cookie set for this page
 * {  //Create a session cookie
 *    cookie = reply->createCookie("myCookie");
 *    //Active cookie i.e. send cookie to client
 *    cookie->activate();
 * }
 * //This will never fail
 * baAssert(cookie == request->getCookie("myCookie"));
 * \endcode
 */
typedef struct HttpCookie
{
#ifdef __cplusplus
      ~HttpCookie();

      /** Returns the comment set for this cookie or null if the
       *  cookie comment is not set. */
      const char* getComment() const { return comment; }

      /** Returns the domain name set for this cookie. */
      const char* getDomain()  const { return domain; }

      /** Returns the maximum age of the cookie, specified in seconds;
       * by default, 0 indicating the cookie will persist until browser
       * shutdown. */
      BaTime      getMaxAge()  const { return maxAge; }

      /** Returns the name of the cookie. */
      const char* getName()    const { return name; }

      /** Returns the path on the server to which the browser returns this
       * cookie. */
      const char* getPath()    const { return path; }

      /** Returns true if the browser is sending cookies only over a secure
       * protocol, or false if the browser can send cookies using
       * any protocol. */
      bool        getSecure()  const { return secure ? true : false; }

      /** Return the HttpOnly attribute.
          \sa setHttpOnly.
      */
      bool        getHttpOnly()  const { return httpOnly ? true : false; }

      /** Returns the value of the cookie. */
      const char* getValue()   const { return value; }

      /** Specifies a comment that describes a cookie's purpose. */
      void setComment(const char* purpose);

      /** Specifies the domain within which this cookie should be presented. */
      void setDomain(const char* pattern);

      /** Sets the maximum age of the cookie in seconds.
          The cookie is by default not stored persistently and will be
          deleted when the web browser exits. This function turns the
          cookie into a persistent cookie, which the browser keeps in
          its cookie container. A persistent cookie can later be
          deleted with function HttpCookie::deleteCookie.
      */
      void setMaxAge(BaTime expiry);
      /** Specifies a path for the cookie to which the client should
       * return the cookie. */

      /** This method sets the maximum age to zero. Calling this method
       * has the same effect as calling setMaxAge(0). You must make
       * sure the cookie path is set and is identical to the path you
       * used when creating the cookie.
       */
      void deleteCookie();

      /** Set the cookie path */
      void setPath(const char* uri);

      /** Inform the browser whether the cookie should be
       * sent only using a secure protocol such as HTTPS -- i.e. using SSL.
       */
      void setSecure(bool flag);

   /** Marks or unmarks this Cookie as HttpOnly.

       If isHttpOnly is set to true, this cookie is marked as
       HttpOnly, by adding the HttpOnly attribute to it.

       HttpOnly cookies are not supposed to be exposed to client-side
       scripting code, and may therefore help mitigate certain kinds
       of cross-site scripting attacks.
       */
      void setHttpOnly(bool flag);

      /** Assigns a new value to a cookie after the cookie is created. */
      void setValue(const char* newValue);

#if 0
      /** Returns the version of the protocol this cookie complies with. */
      int getVersion() const { return version; }
      /* Sets the version of the cookie protocol this cookie complies with. */
      void setVersion(int v);
#endif

      /** Activates the cookie. The cookie will not be sent to the client
       * unless this function is called.
       * You cannot activate a cookie if the data is committed.
       * See HttpResponse::committed for more information.
       */
      void activate();
   private:
      HttpCookie();
#endif
      struct HttpCookie* next;
      char* comment;
      char* domain;
      char* name;
      char* path;
      char* value;
      BaTime maxAge;
      int   version;
      BaBool  secure;
      BaBool  httpOnly;
      BaBool  deleteCookieFlag;
      BaBool  activateFlag; /* The cookie will be sent to the client
                                * if this flag is true */
} HttpCookie;

#ifdef __cplusplus
extern "C" {
#endif
BA_API void HttpCookie_destructor(HttpCookie* o);
BA_API const char* HttpCookie_getComment(HttpCookie* o);
BA_API const char* HttpCookie_getDomain(HttpCookie* o);
BA_API BaTime HttpCookie_getMaxAge(HttpCookie* o);
BA_API const char* HttpCookie_getName(HttpCookie* o);
BA_API const char* HttpCookie_getPath(HttpCookie* o);
BA_API BaBool HttpCookie_getSecure(HttpCookie* o);
BA_API BaBool HttpCookie_getHttpOnly(HttpCookie* o);
BA_API const char* HttpCookie_getValue(HttpCookie* o);
BA_API void HttpCookie_setComment(HttpCookie* o, const char* purpose);
BA_API void HttpCookie_setDomain(HttpCookie* o, const char* pattern);
BA_API void HttpCookie_setMaxAge(HttpCookie* o, BaTime expiry);
#define HttpCookie_deleteCookie(o) (o)->deleteCookieFlag = TRUE;
BA_API void HttpCookie_setPath(HttpCookie* o, const char* uri);
BA_API void HttpCookie_setSecure(HttpCookie* o, BaBool flag);
BA_API void HttpCookie_setHttpOnly(HttpCookie* o, BaBool flag);
BA_API void HttpCookie_setValue(HttpCookie* o, const char* newValue);

#if 0
int HttpCookie_getVersion(HttpCookie* o);
void HttpCookie_setVersion(HttpCookie* o, int v);
#endif

BA_API void HttpCookie_activate(HttpCookie* o);
#ifdef __cplusplus
}
inline HttpCookie::HttpCookie() { baAssert(0); }
inline HttpCookie::~HttpCookie() { HttpCookie_destructor(this); }
inline void HttpCookie::setComment(const char* purpose) {
   HttpCookie_setComment(this, purpose); }
inline void HttpCookie::setDomain(const char* pattern) {
   HttpCookie_setDomain(this, pattern); }
inline void HttpCookie::setMaxAge(BaTime expiry) {
   HttpCookie_setMaxAge(this, expiry); }
inline void HttpCookie::deleteCookie() {
   HttpCookie_deleteCookie(this); }
inline void HttpCookie::setPath(const char* uri) {
   HttpCookie_setPath(this, uri); }
inline void HttpCookie::setSecure(bool flag) {
   HttpCookie_setSecure(this, flag ? TRUE : FALSE); }
inline void HttpCookie::setHttpOnly(bool flag) {
   HttpCookie_setHttpOnly(this, flag ? TRUE : FALSE); }
inline void HttpCookie::setValue(const char* newValue) {
   HttpCookie_setValue(this, newValue); }
#if 0
inline void HttpCookie::setVersion(int v) {
   HttpCookie_setVersion(this, v); }
#endif
inline void HttpCookie::activate() {
   HttpCookie_activate(this); }
#endif


/** Standard HTTP header values.
 \sa HttpRequest::getHeaderValue
 */
typedef struct HttpStdHeaders
{
#ifdef __cplusplus
      /** Returns the connection type for HTTP 1.1 connections,
          returns "Close" or "Keep-Alive.
       */
      const char* getConnection();

      /** Returns the host header.

         The host header includes the port number if the request is of
         the form http://myserver.com:8080

          \sa getDomain
       */
      const char* getHost();

      /** Returns the host header without any port number.
          Typically used with name-based virtual hosting.
          The returned value is always lower-case.
          \sa getHost.
      */
      const char* getDomain();

      /** Returns the content type, for example:
          "application/x-www-form-urlencoded".
       */
      const char* getContentType();

      /** Returns the content length if request contains a body.
       */
      SBaFileSize getContentLength();
#endif
      HttpInData* inData;
      char* domain;
      BaFileSize contentLength;
      U16 connectionHOffs;
      U16 hostHOffs;
      U16 contentTypeHOffs;
} HttpStdHeaders;

#ifdef __cplusplus
extern "C" {
#endif
BA_API const char* HttpStdHeaders_zzGetValFromOffs(
   HttpStdHeaders* o, U16 offset);
BA_API const char* HttpStdHeaders_getDomain(HttpStdHeaders* o);
#define HttpStdHeaders_getConnection(o) \
  HttpStdHeaders_zzGetValFromOffs(o,(o)->connectionHOffs)
#define HttpStdHeaders_getHost(o) \
  HttpStdHeaders_zzGetValFromOffs(o,(o)->hostHOffs)
#define HttpStdHeaders_getContentType(o) \
  HttpStdHeaders_zzGetValFromOffs(o,(o)->contentTypeHOffs)
#define HttpStdHeaders_getContentLength(o) (o)->contentLength

#ifdef __cplusplus
}
inline const char* HttpStdHeaders::getConnection() {
   return  HttpStdHeaders_getConnection(this); }
inline const char* HttpStdHeaders::getHost() {
   return  HttpStdHeaders_getHost(this); }
inline const char* HttpStdHeaders::getDomain() {
   return  HttpStdHeaders_getDomain(this); }
inline const char* HttpStdHeaders::getContentType() {
   return  HttpStdHeaders_getContentType(this); }
inline SBaFileSize HttpStdHeaders::getContentLength() {
   return  HttpStdHeaders_getContentLength(this); }

#endif



 /*Do not change the HttpMethod type without checking tables in Httpserver.c*/

/** HTTP method types.
  The most common headers are:
   HttpMethod_Get,
   HttpMethod_Head,
   HttpMethod_Options,
   HttpMethod_Post,
   HttpMethod_Put and
   HttpMethod_Trace.
 */

typedef enum {
   HttpMethod_Connect   =0x00001,
   HttpMethod_Get       =0x00002,
   HttpMethod_Head      =0x00004,
   HttpMethod_Options   =0x00008,

   HttpMethod_Patch     =0x00010,
   HttpMethod_Post      =0x00020,
   HttpMethod_Put       =0x00040,
   HttpMethod_Trace     =0x00080,

   /* WebDAV */
   HttpMethod_Copy      =0x00100,
   HttpMethod_Delete    =0x00200,
   HttpMethod_Lock      =0x00400,
   HttpMethod_Move      =0x00800,

   HttpMethod_Mkcol     =0x01000,
   HttpMethod_Propfind  =0x02000,
   HttpMethod_Proppatch =0x04000,
   HttpMethod_Unlock    =0x08000,

   HttpMethod_Unknown   =0x10000 /* Must be last */
} HttpMethod;


/* Convert a HTTP method from string value to the type HttpMethod */
BA_API HttpMethod HttpMethod_a2m(const char* str);



/** The HttpServer creates an HttpRequest object when the HttpServer parses
 *  a client request. The object is passed in as the first argument to the
 *  HttpPage service function.
 */
typedef struct HttpRequest
{
#ifdef __cplusplus

      /** Checks the HTTP method type and sends a response message if
          condition met.

          RFC26216 specifies that each resource in a web-server can
          have its own set of allowed HTTP methods.

          The checkMethod method checks if the requested HTTP method
          is one of the allowed HTTP methods as specified in the
          argument list to checkMethod. The function returns 0 if the
          condition is met; otherwise, status code is set to 405 and a
          nonzero value is returned.

          The method checkMethod also checks if the requested HTTP
          method is OPTIONS. If the requested HTTP method is OPTIONS, a
          nonzero value is returned.

          A response message is automatically sent to the client when
          method checkMethod returns a nonzero number; thus, the
          service function should not send a response message.

          This method is typically used by HttpDir and HttpPage
          objects prior to sending a response message. For example, a
          service function that only accepts GET requests would do the
          following:


          HttpMethod_Options and HttpMethod_Head should not be added
          to the argument list since the two HTTP methods are added
          automatically if 'addDefault' is TRUE. It is assumed that a
          resource always can accept HEAD and OPTIONS. The OPTIONS
          request is handled automatically by checkMethod, and HEAD is
          automatically handled by the HttpResponse write methods;
          thus, the two methods are transparent to users.

          A CSP page should not use the checkMethod method as the
          auto generated CSP code automatically sets the HttpMethod_Get,
          HttpMethod_Post, HttpMethod_Head and HttpMethod_Options.

          <a href="http://www.w3.org/Protocols/rfc2616/rfc2616-sec9.html#sec9.2">
          Se RFC2616 section 9.2</a> for more information.

          \param resp The response object.

          \param methods The methods added. Separate multiple methods
          by using character |.

          \param addDefault set to true if you would like to add
          OPTION and HEADER.

          \sa HttpRequest::getMethodType
       */
      int checkMethods(HttpResponse* resp, U32 methods, bool addDefault=TRUE);

      /** Parses and checks if the "If-Modified-Since" time is equal or greater
         than "time".
         <p>
         Sends a "304 Not modified" response and returns true if the condition
         is true; otherwise, false is returned.
         </p>
         <p><b>
         ! You cannot write to the output stream if this function returns true.
         </b></p>
         \param resp the response object is used if sending a 304 response.
         \param time is the GMT time.
      */
      BaBool checkTime(struct HttpResponse* resp, BaTime time);

      /** Returns the authenticated user or NULL if user is not authenticated.
       */
      AuthenticatedUser* getAuthenticatedUser();

      /** Returns the pathname. The pathname will be "/myDir/index.html" if
       * the URL is "http://myServer.net/myDir/index.html" */
      const char* getRequestURI();

      /** Reconstructs the URL the client used to make the
          request. The returned URL contains a protocol, server name,
          port number, and server path, but it does not include query
          string parameters.

          \param forceHttps makes the function return an URL that
          starts with HTTPS if forceHttps is set to true. The default
          is to return a URL that starts with HTTP for a non secure
          connection and HTTPS for a secure connection.

          \sa HttpResponse::forceHttps
       */
      const char* getRequestURL(bool forceHttps=false);

      /** Returns an object containing standard HTTP headers.
          \sa HttpStdHeaders HttpRequest::getHeaderValue
       */
      HttpStdHeaders* getStdHeaders();

      /** Returns the HTTP version as a string, normally "1.1" */
      const char* getVersion();

      /** Returns the value of the specified request header.
          If the request did not include a header of the specified name,
          this method returns null. The header name is case insensitive.
          You can use this method with any request header.
          \param name a string specifying the header name.
          \sa HttpRequest::getStdHeaders HttpStdHeaders
       */
      const char* getHeaderValue(const char* name);

      /** Returns the requested cookie or NULL if no cookie matches the name.
       * \param name the name of the cookie.
       */
      HttpCookie* getCookie(const char* name);

      /** Returns the method type.
          Common method types are HttpMethod_Get, HttpMethod_Post and
          HttpMethod_Head.
          \sa HttpRequest::getMethod HttpRequest::checkMethods
       */
      HttpMethod getMethodType();

      /** Returns a string representation of the value returned by
          HttpRequest::getMethodType
          \sa HttpRequest::getMethodType
       */
      const char* getMethod();

      /** Returns a string representation of the argument.
          \sa HttpRequest::getMethodType
       */
      static const char* getMethod(HttpMethod method);

      /** Returns the value of a request parameter as a <i>const char*</i>
          or null if the parameter does not exist. Request parameters is
          extra information sent with the request that is contained in
          the query string or posted form data.

          If you use this method with a multivalued parameter, the value
          returned is equal to the first value in a HttpParameterIterator.

          \sa HttpParameterIterator HttpParameter
       */
      const char* getParameter(const char* paramName);

      /** Activate a WebSocket handshake. The function returns 0 on
          successful handshake and a negative value if the client is
          not a WebSocket client.
      */
      int wsUpgrade();

      /** Return a HTTP header iterator that can iterate and fetch all
          the HTTP headers.

          <b>CSP page Example:</b>
          \code
         int len;
         HttpHeader* hIter = request->getHeaders(&len);
         for(int i = 0 ; i < len ; i++, hIter++)
         {
            response->printf("%s = %s\n",
                             hIter->name(request),
                             hIter->value(request));
         }
          \endcode
       */
      HttpHeader* getHeaders(int* len);



      /** Returns the current HttpSession associated with this request,
        or if there is no current session and create is true, returns
        a new session.


        If create is false and the request has no valid HttpSession,
        this method returns NULL.


        To make sure the session is properly maintained, you must call
        this method before the response is committed.


        \param create TRUE to create a new session for this request if
        necessary; FALSE to return NULL if there is no current session.

        \return the HttpSession associated with this request or null
        if create is false and the request has no valid session.


        <B>Typical C++ usage:</B>
        \code
        MyPage::service(HttpRequest* request, HttpResponse* response)
        {
           HttpSession* session = request->getSession(true);
           ShoppingCart* sc = (ShoppingCart*)session->getAttribute("Cart");
           if ( ! sc )
           {// User has no shopping cart, create one.
              sc = new ShoppingCart();
              session->setAttribute("Cart", sc);
           }
        }
        \endcode
       */
#ifndef NO_HTTP_SESSION
      HttpSession* getSession(BaBool create=true);
#endif
      /** Returns the connection object associated with this request
       */
      HttpConnection* getConnection();

      /** Returns the web server object.
       */
      HttpServer* getServer();

      /** Returns the HttpResponse object.
       */
      HttpResponse* getResponse();

      /** Get the HttpCommand object
       */
      HttpCommand* getCommand();

      /** Get the internal rec buffer */
      HttpInData* getBuffer();


      int setUserObj(void* userObj, bool overwrite=false);
      void* getUserObj();

#endif
      HttpStdHeaders stdH;
      HttpAllocator headerAlloc;
      HttpAllocator formAlloc;
      HttpInData inData;
      struct HttpServer* server;
      void* userObj;
      struct HttpSession* session;
      HttpMethod methodType;
      U16 pathI;
      U16 versionI;
      U16 headersI;
      U16 headerLen;
      U16 formsI;
      U16 formLen;
      BaBool postDataConsumed; /* Set by MultipartUpload and HttpRecData */
}HttpRequest;

#ifdef __cplusplus
extern "C" {
#endif
BA_API int HttpRequest_checkMethods(HttpRequest* o, struct HttpResponse* resp,
                                    U32 methods, BaBool addDefault);
BA_API int HttpRequest_checkOptions(
   HttpRequest* o, struct HttpResponse* resp, int optLen, ...);
#define HttpRequest_getAuthenticatedUser(o) \
  AuthenticatedUser_get2(HttpRequest_getSession(o,FALSE))
#define HttpRequest_getConnection(o) HttpRequest_getCommand(o)->con
#define HttpRequest_getMethodType(o) (o)->methodType
#define HttpRequest_getMethod(o) HttpRequest_getMethod2((o)->methodType)
BA_API const char* HttpRequest_getMethod2(HttpMethod method);
#define HttpRequest_getServer(o) (o)->server
#define HttpRequest_getResponse(o) (&HttpRequest_getCommand(o)->response)
BA_API struct HttpCommand* HttpRequest_getCommand(HttpRequest*);
BA_API BaBool HttpRequest_checkTime(
   HttpRequest* o, struct HttpResponse* resp, BaTime time);
BA_API const char* HttpRequest_getRequestURI(HttpRequest* o);
BA_API const char* HttpRequest_getRequestURL(HttpRequest* o,BaBool forceHttps);
#define HttpRequest_getStdHeaders(o) (&(o)->stdH)
BA_API const char* HttpRequest_getVersion(HttpRequest* o);
BA_API const char* HttpRequest_getHeaderValue(HttpRequest* o,const char* name);
#define HttpRequest_getNoOfParameters(o) (o)->formLen
BA_API HttpCookie* HttpRequest_getCookie(HttpRequest* o, const char* name);
#define HttpRequest_getUserObj(o) (o)->userObj
BA_API int HttpRequest_setUserObj(
   HttpRequest* o, void* userObj, BaBool overwrite);
#define HttpRequest_getBuffer(o) (&(o)->inData)
BA_API const char* HttpRequest_getParameter(
   HttpRequest* o, const char* paramName);
BA_API HttpHeader* HttpRequest_getHeaders(HttpRequest* o, int* len);
BA_API int HttpRequest_wsUpgrade(HttpRequest* o);
BA_API BaBool HttpRequest_enableKeepAlive(HttpRequest* o);
BA_API int HttpRequest_pushBackData(HttpRequest* o);
#ifndef NO_HTTP_SESSION
BA_API struct HttpSession* HttpRequest_getSession(
   HttpRequest* o, BaBool create);
BA_API struct HttpSession* HttpRequest_session(
   HttpRequest* o, const char* val, size_t len, int set);
#endif
#ifdef __cplusplus
}

inline int HttpRequest::checkMethods(HttpResponse* resp,
                        U32 methods, bool addDefault){
   return HttpRequest_checkMethods(this,resp,
      methods, addDefault ? TRUE : FALSE);}

inline BaBool HttpRequest::checkTime(struct HttpResponse* resp, BaTime time) {
   return HttpRequest_checkTime(this, resp, time); }
inline AuthenticatedUser* HttpRequest::getAuthenticatedUser() {
   return HttpRequest_getAuthenticatedUser(this); }
inline HttpMethod HttpRequest::getMethodType() {
   return HttpRequest_getMethodType(this); }
inline const char* HttpRequest::getMethod() {
   return HttpRequest_getMethod(this); }
inline const char* HttpRequest::getMethod(HttpMethod method) {
   return HttpRequest_getMethod2(method); }
inline HttpServer* HttpRequest::getServer() {
   return HttpRequest_getServer(this); }
inline const char* HttpRequest::getRequestURI() {
   return HttpRequest_getRequestURI(this); }
inline const char* HttpRequest::getRequestURL(bool forceHttps) {
   return HttpRequest_getRequestURL(this, forceHttps); }
inline HttpStdHeaders* HttpRequest::getStdHeaders() {
   return HttpRequest_getStdHeaders(this); }
inline const char* HttpRequest::getVersion() {
   return HttpRequest_getVersion(this); }
inline const char* HttpRequest::getHeaderValue(const char* name) {
   return HttpRequest_getHeaderValue(this, name); }
inline HttpCookie* HttpRequest::getCookie(const char* name) {
   return HttpRequest_getCookie(this, name); }
inline const char* HttpRequest::getParameter(const char* paramName) {
   return HttpRequest_getParameter(this, paramName); }
inline int HttpRequest::wsUpgrade() {
   return HttpRequest_wsUpgrade(this);
}
inline HttpHeader* HttpRequest::getHeaders(int* len) {
   return HttpRequest_getHeaders(this, len); }
inline int HttpRequest::setUserObj(void* userObj, bool overwrite) {
   return HttpRequest_setUserObj(this, userObj, overwrite); }
inline void* HttpRequest::getUserObj() {
   return HttpRequest_getUserObj(this); }
#ifndef NO_HTTP_SESSION
inline HttpSession* HttpRequest::getSession(BaBool create) {
   return HttpRequest_getSession(this, create); }
#endif
inline bool HttpParameterIterator::hasMoreElements() {
   return HttpParameterIterator_hasMoreElements(this); }
inline HttpInData* HttpRequest::getBuffer() {
   return HttpRequest_getBuffer(this); }
#endif


#ifndef __DOXYGEN__

typedef struct
{
      HttpAllocator data;
      U16 maxResponseHeader;
} NameValMM;


typedef struct
{
      U8 major;
      U8 minor;
} HttpProtocol;

#endif

/** This object is used when sending response messages back to the client.
  The HttpServer creates an HttpResponse object and passes it as the second
  argument to the HttpPage Service Function.

  The HttpResponse struct contains a number of methods that can be used
  when sending messages back to the client from the HttpPage Service
  Function. Most of the response functions return an int. The methods
  return 0 on success or -1 on failure. A function might, for example,
  send -1 if the socket connection closes.
*/
typedef struct HttpResponse
{
#ifdef __cplusplus

      /** Returns number of bytes sent thus far.
       */
      U32 byteCount();


      /** Create a cookie.
       * \param name the name of the cookie.
       */
      HttpCookie* createCookie(const char* name);

      /** Searches the internal response header database for a header
          with the specified name. Returns the value of the {name,
          value} pair if the header is found or NULL if not found. The
          response header values are set with method
          HttpResponse::setHeader. This method is typically used by
          servlets either included by another servlet or when a
          servlet forwards the request to another servlet. See methods
          HttpResponse::forward and HttpResponse::include for more
          information.
      */
      const char* containsHeader(const char* name);

      /** Encodes the specified URL into an absolute URL, or if
       *  encoding is not needed, returns the URL unchanged.
       * \param pathName the absolute or relative URL to be encoded
       * \return the encoded absolute URL. The buffer returned by
       * encodeRedirectURL is valid until end of client request or until
       * encodeRedirectURL is called again.
       * \sa HttpResponse::sendRedirect
       */
      const char* encodeRedirectURL(const char* pathName);

      /** This method is similar to HttpResponse::encodeRedirectURL,
          but this method also includes all URL-encoded parameters in the
          request line and in the body if the client sent a POST message.
          \param pathName the absolute or relative URL to be encoded
          \return the encoded absolute URL. The buffer returned by
          encodeRedirectURL is valid until end of client request or until
          encodeRedirectURL is called again.
          \sa HttpResponse::sendRedirect
      */
      const char* encodeRedirectURLWithParam(const char* pathName);

      /** Encodes an absolute or relative URL, or if encoding is not needed,
       * returns the URL unchanged.
       *  This method escapes all symbols that cannot be in an URL.
       *  The method differs from encodeRedirectURL in that it only escapes
       *  non-URL compatible symbols.
       */
      const char* encodeUrl(const char* path);

      /** Forces any content in the buffer to be written to the client.
          A call to this method automatically commits the response, meaning
          the status code and headers will be written.
      */
      int flush();

      /** Forwards a request from a servlet to another resource (servlet, CSP
          file, or HTML file). This method allows one servlet/CSP file to do
          preliminary processing of a request and another resource to generate
          the response. See the introduction to
          <a href="../../misc/RequestDelegation.html">Request Delegation</a>
          for more information.

          Forward should be called before the response has been committed to
          the client (before response body output has been flushed). If the
          response already has been committed, this method returns a non-zero
          value.

          \param path is the path to the resource to execute. The path is
          assumed to be an absolute path value on the server if the string
          starts with "/". The path is otherwise assumed to be a relative path.

          This method is similar to method redirect, with the
          exception that forward bypasses any form of required
          authentication and/or authentication.

          \returns Zero on success. Returns a non-zero value if the
          response is committed.

          \sa redirect
      */
      int forward(const char* path);

      /** Internally redirects the request to another resource.

          Redirect should be called before the response has been committed to
          the client (before response body output has been flushed). If the
          response already has been committed, this method returns a non-zero
          value.

          \param path is the path to the resource to execute. The path is
          assumed to be an absolute path value on the server if the string
          starts with "/". The path is otherwise assumed to be a relative path.

          This method is similar to method forward, with the exception
          that forward bypasses any form of required authentication
          and/or authentication.

          \returns Zero on success. Returns a non-zero value if the
          response is committed.

          \sa forward
      */
      int redirect(const char* path);


      /** Get the HttpRequest object
       */
      HttpRequest* getRequest();

      /** Get the HttpCommand object
       */
      HttpCommand* getCommand();


      /** Includes the content of a resource (servlet, CSP page, HTML file)
          in the response. In essence, this method enables programmatic
          server-side includes. See the introduction to
          <a href="../../misc/RequestDelegation.html">Request Delegation</a>
          for more information.

          <p>The HttpResponse object's path elements and parameters
          remain unchanged from the caller's values. The included
          servlet cannot change the response status code or set
          headers; any attempt to make a change is ignored.</p>

          \param path is the path to the resource to include. The path is
          assumed to be an absolute path value on the server if the string
          starts with "/". The path is otherwise assumed to be a relative path.
      */
      int include(const char* path);

      /** Returns a boolean indicating if the response has been committed.
          A committed response has already had its status code and headers
          written.
      */
      bool committed() const;

      /** Returns true if this is a forward request from another
       * servlet or CSP file.
       */
      bool isForward() const;

      /** Returns true if this is an include from another servlet or CSP file.
       */
      bool isInclude() const;

      /** Returns true if this is the initial page.
          \return (!isForward() && !isInclude())
      */
      bool initial() const;

      /** Removes all HTTP headers. This method returns a non-zero
          value if the response has been committed.
      */
      int resetHeaders();

      /** Clears the content of the underlying buffer in the response without
          clearing headers or status code. If the response has been committed,
          this method returns a non-zero value.
      */
      int resetBuffer();

      /** Sends an error response as a simple HTML page to the client
          using the specified status code.  If the response has been
          committed, this method returns a non-zero value.

          \code
          C function name:
          HttpResponse_sendError1
          \endcode

          \param eCode the error code.
       */
      int sendError(int eCode);

      /** Sends an error response as a simple HTML page to the client
          using the specified status code.  If the response has been
          committed, this method returns a non-zero value.

          \code
          C function name:
          HttpResponse_sendError2
          \endcode

          \param eCode The HTTP error code.

          \param msg The error message.
       */
      int sendError(int eCode, const char* msg);

      /** Sends the data formatted into the HttpResponse buffer as an
          error message to the client. The mime type is set to
          'text/html'.
          \param eCode The HTTP error code.
      */
      int sendBufAsError(int eCode);

      /** Sends the data formatted into the HttpResponse buffer as an
          error message to the client. The mime type is set to
          'text/plain'.
          \param eCode The HTTP error code.
      */
      int sendBufAsTxtError(int eCode);


      /** This method implements a printf like implementation for
          formatting and sending an error message. The error message
          is sent as plain text to the client.
          \param eCode The HTTP error code.
          \param fmt See BufPrint::printf.
      */
      int fmtError(int eCode, const char* fmt, ...);

      /** Sends a temporary redirect (302) response to the client
         using the specified redirect location URL. The method is
         internally using HttpResponse::encodeRedirectURL if the URL
         does not start with http[s]:.
         \param url absolute or relative URL.
         \return zero on success or a negative value if the method fails.
         <B>Typical C++ usage:</B>
         \code
         response->sendRedirect("start.html");
         \endcode
         \code
         response->sendRedirect("https://127.0.0.1:9357/intro/start.html"));
         \endcode
         \sa HttpResponse::encodeRedirectURL
         \sa HttpResponse::encodeRedirectURLWithParam
         \sa HttpRequest::getRequestURI
       */
      int sendRedirect(const char* url);

      /** Converts the URL to HTTPS and sends a redirect
          (301) response to the client if this is a non secure
          connection. This function fails if the response is committed.

          \returns 0 if the connection is secure and nothing is
          sent. Returns 1 if a 301 HTTP response is sent. Returns < 0
          if the command fails.

          \sa HttpRequest::getRequestURL
          \sa HttpResponse::sendRedirect
      */
      int redirect2TLS();

      /** Sets the "Content-Length" parameter value.  If the header
        has already been set, the new value overwrites the previous
        one. The HttpResponse::containsHeader method can be used to
        test for the presence of a header before setting its value.
       */
      int setContentLength(BaFileSize len);

      /** Sets the "Content-Type" parameter value.  If the header has
        already been set, the new value overwrites the previous
        one. The HttpResponse::containsHeader method can be used to
        test for the presence of a header before setting its value.
      */
      int setContentType(const char* type);

      /** Sets a response header with the given name and
          date-value. The date is specified in terms of seconds. If
          the header has already been set, the new value overwrites
          the previous one. The HttpResponse::containsHeader method
          can be used to test for the presence of a header before
          setting its value.  \param name the name of the header to
          set.  \param time the assigned time value in number of
          seconds elapsed since midnight (00:00:00), January 1, 1970.
      */
      int setDateHeader(const char* name, BaTime time);

      /** Sets a
          <a href="http://www.w3.org/Protocols/rfc2616/rfc2616-sec14.html">
          HTTP response header</a> with the given name and value.

      If the header has already been set, the new value overwrites the
      previous one. The HttpResponse::containsHeader method can be
      used to test for the presence of a header before setting its
      value.

      setStatus should be called before the response has been
      committed to the client (before response body output has been
      flushed). If the response already has been committed, this
      method returns a non-zero value.

      \param name the name of the header to set.
      \param value the header value. Set to NULL if you want to erase
      any previous value, if any.
      \param replace set to false if you do not want to overwrite any
      previous value, if any.
      */
      int setHeader(const char* name, const char* value, bool replace=true);

      /** Sets header "Cache-Control: max-age=seconds".
       *  Can, for example, be used by CSP code to overide the
       *  default headers inserted by the CSP compiler.
       *  See HttpResponse::setDefaultHeaders for more information.
       */
      int setMaxAge(BaTime seconds);


      /** Pre-allocate memory for a {name, value} pair in the response
          header database.

          Sometimes when setting a header value with setHeader
          involves unnecessary copying and/or allocation of
          memory. The fmtHeader method makes it possible to
          pre-allocate a memory area in the internal {name, value}
          pair database in the response object. The function, if
          successful, returns a pointer to a memory area for the value
          in a {name, value} pair.  You can then use, for example,
          sprintf to format this memory area.

          <b> You must use extreme caution when using this
          function. Writing more data to the pointer returned than
          requested, will corrupt the internal data structures in the
          database.</b>

          <b>Example:</b><br>
          Setting the extended foobar header as per RFC 3092
          \code
          const char foo[] = {"foo"};
          const char bar[] = {"bar"};
          int len = strlen(bar);
          char* ptr = resp->fmtHeader(foo, len+1);
          if(ptr)
             strcpy(ptr, bar);
          \endcode

          \param name the name of the header to set.
          \param valueLen the lenght of the memory area returned by fmtHeader.
          \param replace the parameter if already set.
      */
      char* fmtHeader(const char* name, int valueLen, bool replace=true);


      /** Sets the status code for this response. This method is used
          to set the return status code when there is no error (for
          example, for the status code 304 Not Modified). If there is
          an error, the sendError method should be used instead.

          setStatus should be called before the response has been
          committed to the client (before response body output has
          been flushed). If the response already has been committed,
          this method returns a non-zero value.

          \param statusCode The
           <a href="http://www.w3.org/Protocols/HTTP/HTRESP.html">
           HTTP status code</a>
       */
      void setStatus(int statusCode);

      /** printf is used for sending formatted data to the client.
          \param fmt See BufPrint::printf
      */
      int printf(const char* fmt, ...);

      /** Returns a BufPrint object that can send any type of data to
          the client.

          The BufPrint object is also used implicitly when using
          method HttpResponse::printf or HttpResponse::write.
      */
      BufPrint* getWriter();

      /** Used for sending pre-formatted data to the client.
          \param data pointer to data.
          \param len size of data.
          \param useBuffering set this to true if the internal HttpResponse
          object should buffer the data before sending it to the client.
          The buffer will be automatically flushed when full.
          \sa BufPrint::write
       */
      int write(const void* data, int len, int useBuffering=TRUE);

      /** Used for sending a zero terminated string to the client.
         This method is only available if you use C++.
         \param data a reference to the string.
         \param useBuffering set this to true if the internal HttpResponse
         object should buffer the data before sending it to the client.
         The buffer will be automatically flushed when full.
       */
      int write(const char* data, int useBuffering=TRUE);

      /** Used when sending raw data to the client.
       * This function is typically used when sending binary data to the
       * client.
       */
      int send(const void* data, int len);

      /** Sets the most common header values in servlet and CSP files.
          Sets content type to "text/html" and sets the header
          "Cache-Control", "No-Cache".

          This method is automatically inserted by the CSP/JSSP compiler.
      */
      int setDefaultHeaders();

      /** This is an advanced function that makes it possible to
        redirect the output, which is normally sent to the client, to
        another buffer. The function must be called before any data
        is emitted.


        Method setDefaultHeaders is used internally by the Lua bindings for method
        <a href="../../../lua/lua.html#response_setresponse">response:setresponse</a>

        \sa removeResponseBuf

        \param buf the server emits response data to this buffer. The
        parameter buf and bufSize in struct BufPrint must be
        initialized if useDefBuffer is set to false.

        \param useDefBuffer The parameter buf and bufSize in struct
        BufPrint is set to the internal web-server buffer if this
        variable is set to true.
       */
      int setResponseBuf(BufPrint* buf, bool useDefBuffer=true);

      /** Remove buffer set by using setResponseBuf.
          \sa setResponseBuf
      */
      int removeResponseBuf();


      int setUserObj(void* userObj, bool overwrite=false);
#endif
      NameValMM nameValMM;
      BufPrint headerPrint;
      BufPrint defaultBodyPrint;
      BufPrint* bodyPrint;
      struct HttpDir* currentDir;
      char* encodedURL;
      char* encodedRedirectURL;
      HttpCookie* cookieList;
      void* userObj;
      int statusCode;
      U32 msgLen; /* Used if request is of type HEAD, count total msg len */
      HttpProtocol protocol;
      U16 includeCounter;
      U16 forwardCounter;
      char headerSent;
      char printAndWriteInitialized;
      char useChunkTransfer;
} HttpResponse;

#ifdef __cplusplus
extern "C" {
#endif
BA_API HttpCookie* HttpResponse_createCookie(
   struct HttpResponse* o,const char* name);
BA_API const char* HttpResponse_containsHeader(
   HttpResponse* o, const char* name);
BA_API int HttpResponse_dataAdded(HttpResponse* o, U32 size);
#define HttpResponse_byteCount(o) ((o)->msgLen + (o)->bodyPrint->cursor)
BA_API const char* HttpResponse_encodeRedirectURL(
   HttpResponse* o, const char* pathName);
BA_API const char* HttpResponse_encodeRedirectURLWithParamOrSessionURL(
   HttpResponse* o,const char* path,BaBool setUrlCookie);
#define HttpResponse_encodeRedirectURLWithParam(o, path) \
   HttpResponse_encodeRedirectURLWithParamOrSessionURL(o, path, FALSE)
#define HttpResponse_encodeSessionURL(o, path) \
   HttpResponse_encodeRedirectURLWithParamOrSessionURL(o, path, TRUE)
BA_API const char* HttpResponse_encodeUrl(HttpResponse* o, const char* path);
BA_API int HttpResponse_flush(HttpResponse* o);
#define HttpResponse_forward(o,path) HttpResponse_incOrForward(o,path,FALSE)
#define HttpResponse_getConnection(o) HttpResponse_getCommand(o)->con
#define HttpResponse_getRequest(o) (&HttpResponse_getCommand(o)->request)
BA_API struct HttpCommand* HttpResponse_getCommand(HttpResponse*);
#define HttpResponse_getBuf(o) (o)->bodyPrint->buf
#define HttpResponse_getBufSize(o) (o)->bodyPrint->bufSize
#define HttpResponse_getBufOffs(o) \
   ((o)->bodyPrint->buf + (o)->bodyPrint->cursor)
#define HttpResponse_getRemBufSize(o) \
   ((o)->bodyPrint->bufSize - (o)->bodyPrint->cursor)
#define HttpResponse_getUserObj(o) (o)->userObj
BA_API int HttpResponse_incOrForward(
   HttpResponse* o, const char* path, BaBool isInc);
BA_API int HttpResponse_redirect(HttpResponse* o, const char* path);
#define HttpResponse_include(o,path) HttpResponse_incOrForward(o,path,TRUE)
#define HttpResponse_isChunkTransfer(o) (o)->useChunkTransfer
#define HttpResponse_committed(o) (o)->headerSent
#define HttpResponse_isForward(o) ((o)->forwardCounter != 0)
#define HttpResponse_isInclude(o) ((o)->includeCounter != 0)
#define HttpResponse_initial(o) (!HttpResponse_isForward(o) && \
                                 !HttpResponse_isInclude(o))
BA_API int HttpResponse_setResponseBuf(
   HttpResponse* o,BufPrint* buf,BaBool useDefBuffer);
BA_API int HttpResponse_removeResponseBuf(HttpResponse* o);

BA_API int HttpResponse_resetHeaders(HttpResponse* o);
BA_API int HttpResponse_resetBuffer(HttpResponse* o);
BA_API int HttpResponse_sendError1(HttpResponse* o, int eCode);
BA_API int HttpResponse_sendError2(HttpResponse* o,int eCode,const char* msg);
BA_API int HttpResponse_sendBufAsError(HttpResponse* o,int eCode);
BA_API int HttpResponse_sendBufAsTxtError(HttpResponse* o,int eCode);
BA_API int HttpResponse_fmtVError(
   HttpResponse* response,
   int eCode,
   const char* fmt,
   va_list varg);
BA_API int HttpResponse_fmtError(
   HttpResponse* response,
   int eCode,
   const char* fmt, ...);
BA_API int HttpResponse_sendRedirect(HttpResponse* o, const char* url);
BA_API int HttpResponse_sendRedirectI(
   HttpResponse* o, const char* url, int status);
BA_API int HttpResponse_redirect2TLS(HttpResponse* o);
#define HttpResponse_forceHttps HttpResponse_redirect2TLS /* Backw. comp. */

#define HttpResponse_setBufPos(o, pos) (o)->bodyPrint->cursor = pos
BA_API int HttpResponse_setContentLength(HttpResponse* o, BaFileSize len);
BA_API int HttpResponse_setContentType(HttpResponse* o, const char* type);
BA_API int HttpResponse_checkContentType(HttpResponse* o, const char* type);
BA_API int HttpResponse_setDateHeader(
   HttpResponse* o, const char* name, BaTime t);
BA_API int HttpResponse_setHeader(
   HttpResponse* o,const char* name,const char* value, BaBool replace);
BA_API int HttpResponse_setMaxAge(HttpResponse* response, BaTime seconds);
BA_API char* HttpResponse_fmtHeader(
   HttpResponse* o, const char* name, int valueLen, BaBool replace);
BA_API int HttpResponse_setStatus(HttpResponse* o, int eCode);
BA_API int HttpResponse_vprintf(
   HttpResponse* o, const char* fmt, va_list argList);
BA_API int HttpResponse_printf(HttpResponse* o, const char* fmt, ...);
BA_API int HttpResponse_write(HttpResponse* o, const void* data, int len,
                              int useBuffering);
BA_API BufPrint* HttpResponse_getWriter(HttpResponse* o);
BA_API int HttpResponse_send(HttpResponse* o, const void* data, int len);
BA_API int HttpResponse_setDefaultHeaders(HttpResponse* o);
BA_API int HttpResponse_downgrade(HttpResponse* o);
BA_API int HttpResponse_setUserObj(
   HttpResponse* o,void* userObj,BaBool overwrite);
BA_API int HttpResponse_printAndWriteInit(HttpResponse* o);
BA_API int HttpResponse_send100Continue(HttpResponse* o);
BA_API int HttpResponse_setChunkEncoding(HttpResponse* o);
BA_API const char* HttpResponse_getRespData(HttpResponse* o, int* len);
#define HttpResponse_getStatus(o) (o)->statusCode
#ifdef __cplusplus
}
inline HttpCookie* HttpResponse::createCookie(const char* name) {
   return HttpResponse_createCookie(this, name); }
inline const char* HttpResponse::containsHeader(const char* name) {
   return HttpResponse_containsHeader(this, name); }
inline U32 HttpResponse::byteCount() {
   return HttpResponse_byteCount(this); }
inline const char* HttpResponse::encodeRedirectURL(const char* pathName) {
   return HttpResponse_encodeRedirectURL(this, pathName); }
inline const char* HttpResponse::encodeRedirectURLWithParam(const char* p) {
   return HttpResponse_encodeRedirectURLWithParam(this, p); }
inline const char* HttpResponse::encodeUrl(const char* path) {
   return HttpResponse_encodeUrl(this, path); }
inline int HttpResponse::flush() {
   return HttpResponse_flush(this); }
inline int HttpResponse::forward(const char* path) {
   return HttpResponse_forward(this, path); }
inline int HttpResponse::redirect(const char* path) {
   return HttpResponse_redirect(this, path); }
inline int HttpResponse::include(const char* path) {
   return HttpResponse_include(this, path); }
inline bool HttpResponse::committed() const {
   return HttpResponse_committed(this) ? true : false; }
inline bool HttpResponse::isForward() const {
   return HttpResponse_isForward(this) ? true : false; }
inline bool HttpResponse::isInclude() const {
   return HttpResponse_isInclude(this) ? true : false; }
inline bool HttpResponse::initial() const {
   return HttpResponse_initial(this) ? true : false; }
inline int HttpResponse::setResponseBuf(BufPrint* buf, bool useDefBuffer) {
   return HttpResponse_setResponseBuf(this, buf, useDefBuffer?TRUE:FALSE); }
inline int HttpResponse::removeResponseBuf() {
   return HttpResponse_removeResponseBuf(this); }
inline int HttpResponse::resetHeaders() {
   return HttpResponse_resetHeaders(this); }
inline int HttpResponse::resetBuffer() {
   return HttpResponse_resetBuffer(this); }
inline int HttpResponse::sendError(int eCode) {
   return HttpResponse_sendError1(this, eCode); }
inline int HttpResponse::sendError(int eCode, const char* msg) {
   return HttpResponse_sendError2(this, eCode, msg); }
inline int HttpResponse::sendBufAsError(int eCode) {
   return HttpResponse_sendBufAsError(this, eCode); }
inline int HttpResponse::sendBufAsTxtError(int eCode) {
   return HttpResponse_sendBufAsTxtError(this,eCode);}
inline int HttpResponse::fmtError(int eCode,const char* fmt,...) {
   int retv; va_list varg;
   va_start(varg, fmt);
   retv = HttpResponse_fmtVError(this, eCode, fmt, varg);
   va_end(varg);
   return retv; }
inline int HttpResponse::sendRedirect(const char* url) {
   return HttpResponse_sendRedirect(this, url); }
inline int HttpResponse::redirect2TLS() {
   return HttpResponse_redirect2TLS(this); }
inline int HttpResponse::setContentLength(BaFileSize len) {
   return HttpResponse_setContentLength(this, len); }
inline int HttpResponse::setContentType(const char* type) {
   return HttpResponse_setContentType(this, type); }
inline int HttpResponse::setDateHeader(const char* name, BaTime t) {
   return HttpResponse_setDateHeader(this, name, t); }
inline int HttpResponse::setHeader(
   const char* name, const char* value, bool replace) {
   return HttpResponse_setHeader(this,name, value, replace?TRUE:FALSE); }
inline int HttpResponse::setMaxAge(BaTime seconds) {
   return HttpResponse_setMaxAge(this, seconds); }
inline char* HttpResponse::fmtHeader(
   const char* name, int valueLen, bool replace) {
   return HttpResponse_fmtHeader(this, name, valueLen, replace?TRUE:FALSE); }
inline void HttpResponse::setStatus(int eCode) {
   HttpResponse_setStatus(this, eCode);}

inline int HttpResponse::printf(const char* fmt, ...) {
   int retv; va_list varg; va_start(varg, fmt);
   retv = HttpResponse_vprintf(this, fmt, varg); va_end(varg); return retv;}
inline int HttpResponse::write(const char* data, int useBuffering) {
   return HttpResponse_write(this, data, iStrlen(data), useBuffering); }
inline BufPrint* HttpResponse::getWriter() {
   return HttpResponse_getWriter(this); }
inline int HttpResponse::write(const void* data, int len, int useBuffering){
   return HttpResponse_write(this, data, len, useBuffering); }
inline int HttpResponse::send(const void* data, int len) {
   return HttpResponse_send(this, data, len); }
inline int HttpResponse::setDefaultHeaders() {
   return HttpResponse_setDefaultHeaders(this); }
inline int HttpResponse::setUserObj(void* userObj, bool overwrite) {
   return HttpResponse_setUserObj(this, userObj, overwrite); }

#endif

/** The HttpCommand class is a container class for the HttpRequest and
    HttpResponse command pair. An instance of the HttpServer class
    contains N HttpCommand instances, where N is set with method
    HttpServerConfig::setNoOfHttpCommands.
*/
typedef struct HttpCommand
{
#ifdef __cplusplus

      /** Get the request object.
       */
      HttpRequest* getRequest();

      /** Get the response object.
       */
      HttpResponse* getResponse();

      /** Get the current connection object that the HttpCommand instance
          is bound with.
      */
      struct HttpConnection* getConnection();

      /** Get the web-server object */
      struct HttpServer* getServer();

      HttpCommand() {}
#endif
      DoubleLink super;
#ifdef __cplusplus
   public:
#endif
      HttpRequest request;
      HttpResponse response;
      struct HttpConnection* con;
      struct LHttpCommand* lcmd; /* Used by LSP plugin */
	  BaTime requestTime;
      BaBool runningInThread;
}HttpCommand;

#define HttpCommand_getRequest(o) (&(o)->request)
#define HttpCommand_getResponse(o) (&(o)->response)
#define HttpCommand_getConnection(o) (o)->con
#define HttpCommand_getServer(o) HttpConnection_getServer((o)->con)
#ifdef __cplusplus
inline HttpRequest* HttpCommand::getRequest() {
   return HttpCommand_getRequest(this); }
inline HttpResponse* HttpCommand::getResponse() {
   return HttpCommand_getResponse(this); }
inline struct HttpConnection* HttpCommand::getConnection() {
   return HttpCommand_getConnection(this); }
inline struct HttpServer* HttpCommand::getServer() {
   return HttpCommand_getServer(this);
}
#endif


#ifndef NO_HTTP_SESSION

#ifndef __DOXYGEN__
struct HttpSessionAttribute;
struct HttpSessionContainer;
#endif

/** HttpSessionAttribute termination callback function.
    This function is called when the session object times out. The
    callback function should release the memory to the session
    attribute.
    \param o the session object. Typically upcasted to the derived class.
*/
typedef void (*HttpSessionAttribute_Destructor)(
   struct HttpSessionAttribute* o);

/** The interface to an HttpSession attribute.
 * <P>Each attribute added to an HttpSession must inherit from this class.
 * The HttpSession object calls the HttpSessionAttribute destructor when the
 * HttpSession object is terminated.</P>
 *
 * <P>The HttpSessionAttribute is an abstract class with a virtual destructor.
 * We cannot use a "real" virtual destructor since the Web-Server must be ANSI
 * C compatible. The function pointer passed in to the constructor is the
 * function being called when the attribute is scheduled to terminate.
 * The function pointer can be NULL if no cleanup is necessary.
 * </P>
 * <B>Typical C++ usage:</B>
 * \code
 * class MyAttribute : public HttpSessionAttribute
 * {
 *    public:
 *       MyAttribute() : HttpSessionAttribute("MyAttribute", destructor) {}
 *    private:
 *       static void destructor(HttpSessionAttribute* o);
 * };
 *
 * MyAttribute::destructor(HttpSessionAttribute* o)
 * {
 *    delete ((MyAttribute*)o); // Run destructor and free memory.
 * }
 * \endcode
 */
typedef struct HttpSessionAttribute
{
#ifdef __cplusplus
      /** Create a session attribute.
          \param name a unique attribute name.
          \param terminate a pointer to a function called when the
          session object times out.
      */
      HttpSessionAttribute(const char* name,
                           HttpSessionAttribute_Destructor terminate);

      /** Get the session object.
       */
      HttpSession* getSession();
#endif
      struct HttpSessionAttribute* next;
      struct HttpSession* session;
      HttpSessionAttribute_Destructor destructor;
      char* name;
} HttpSessionAttribute;

#ifdef __cplusplus
extern "C" {
#endif

BA_API void HttpSessionAttribute_constructor(
   HttpSessionAttribute* o,
   const char* name,
   HttpSessionAttribute_Destructor d);
BA_API void HttpSessionAttribute_destructor(HttpSessionAttribute* o);
#define HttpSessionAttribute_getSession(o) (o)->session
#ifdef __cplusplus
}
inline HttpSessionAttribute::HttpSessionAttribute(
   const char* name,
   HttpSessionAttribute_Destructor d) {
   HttpSessionAttribute_constructor(this, name, d); }
inline HttpSession* HttpSessionAttribute::getSession() {
   return HttpSessionAttribute_getSession(this); }

#endif

/** Provides a way to identify a user across more than one page request or
    visit to a web site, and to store information about that user.

    \htmlinclude Session.txt
*/
typedef struct HttpSession
{
#ifdef __cplusplus

      void *operator new(size_t s) { return ::baMalloc(s); }
      void operator delete(void* d) { if(d) ::baFree(d); }
      void *operator new(size_t, void *place) { return place; }
      void operator delete(void*, void *) { }

      /** Unbinds any objects bound to this session object, runs the
          HttpSession destructor, and frees the memory for this object.

          The web-server controls this object, and the object may
          linger for some time before the object is terminated and
          released. See HttpSession for more information.

          When the web-server destroys the object, all attached
          HttpSessionAttributes are terminated by calling method
          HttpSessionAttribute_Destructor for each attached
          HttpSessionAttribute.

          \sa AuthenticatedUser::logout
       */
      void terminate();

      /** Returns the object bound with the specified name in this session,
       * or null if no object is bound under the name.
       */
      HttpSessionAttribute* getAttribute(const char* name);

      /** Returns the time when this session was created, measured in seconds
       *  since midnight January 1, 1970 GMT.
       */
      BaTime getCreationTime();

      /** Returns the last time the client sent a request associated with this
       * session, as the number of seconds since midnight January 1, 1970 GMT,
       * and marked by the time the container received the request.
       */
      BaTime getLastAccessedTime();

      /** Returns the maximum time interval, in seconds, that the session
       * container will keep this session open between client accesses.
       */
      BaTime getMaxInactiveInterval();

      /** Get the server object */
      HttpServer* getServer();

      /** Removes the object bound with the specified name from this session.
       */
      int removeAttribute(const char* name);

      /** Binds an object to this session, using the name specified.
       */
      int setAttribute(HttpSessionAttribute* value);

      /** Specifies the time, in seconds, between client requests before the
       * session container will invalidate this session.
       */
      void setMaxInactiveInterval(BaTime interval);

      /** Increments the session reference counter.

          This method is used together with method
          decrRefCntr. See HttpSession for more information.
      */
      void incrRefCntr();

      /** Decrements the session reference counter.

          This method is used together with method
          incrRefCntr. See HttpSession for more information.
      */
      void decrRefCntr();

      /** Returns a unique identifier assigned to this session.
       * See HttpServer::getSession for more information.
       */
      U32 getId();

      /** Returns the AuthenticatedUser if user is authenticated.
       */
      AuthenticatedUser* getAuthenticatedUser();

      /** Get the session usage counter
       */
      U32 getUseCounter();


      void incrementLock();
      void decrementLock();

      HttpSession() {}
   private:
#endif
      SplayTreeNode super; /* inherits from SplayTreeNode */
      DoubleLink dlink;
      HttpSockaddr peer;
      HttpSessionAttribute* attrList;
      struct HttpSessionContainer* container;
      BaTime creationTime;
      BaTime lastAccessedTime;
      BaTime maxInactiveInterval;
      U32 sesrnd1; /* Session random number 1 & 2 */
      U32 sesrnd2; /* entropy id+sesrnd1+sesrnd2=96 bits */
      U32 useCounter;
      int refCounter;
      U16 lockCounter;
      U8 termPending; /* TRUE if in termination list */
} HttpSession;


#ifdef __cplusplus
extern "C" {
#endif
BA_API HttpSessionAttribute* HttpSession_getAttribute(HttpSession* o,
                                                      const char* name);
BA_API BaTime HttpSession_getCreationTime(HttpSession* o);
BA_API BaTime HttpSession_getLastAccessedTime(HttpSession* o);
BA_API BaTime HttpSession_getMaxInactiveInterval(HttpSession* o);
BA_API struct HttpServer* HttpSession_getServer(HttpSession* o);
BA_API void HttpSession_terminate(HttpSession* o);
BA_API BaBool HttpSession_isNew(HttpSession* o);
BA_API int HttpSession_removeAttribute(HttpSession* o, const char* name);
BA_API int HttpSession_setAttribute(HttpSession* o,
                                    HttpSessionAttribute* value);
BA_API void HttpSession_setMaxInactiveInterval(HttpSession* o,BaTime interval);
#define HttpSession_incrRefCntr(o) (o)->refCounter++
BA_API void HttpSession_decrRefCntr(HttpSession* o);
#define HttpSession_incrementLock(o) (o)->lockCounter++
#define HttpSession_decrementLock(o) do {\
      baAssert((o)->lockCounter > 0);\
      (o)->lockCounter--; } while(0)
BA_API int HttpSession_fmtSessionId(HttpSession* o, U8* buf, size_t bufSize);
#define HttpSession_getId(o) \
   ((U32)((ptrdiff_t)SplayTreeNode_getKey((SplayTreeNode*)(o))))
#define HttpSession_getAuthenticatedUser(o) AuthenticatedUser_get2((o))
#define HttpSession_getUseCounter(o) (o)->useCounter
#define HttpSession_getPeerName(o) (&(o)->peer)
#ifdef __cplusplus
}
inline HttpSessionAttribute* HttpSession::getAttribute(const char* name) {
   return HttpSession_getAttribute(this, name); }
inline BaTime HttpSession::getCreationTime() {
   return HttpSession_getCreationTime(this); }
inline BaTime HttpSession::getLastAccessedTime() {
   return HttpSession_getLastAccessedTime(this); }
inline BaTime HttpSession::getMaxInactiveInterval() {
   return HttpSession_getMaxInactiveInterval(this); }
inline HttpServer* HttpSession::getServer() {
   return HttpSession_getServer(this); }
inline void HttpSession::terminate() {
   HttpSession_terminate(this); }
inline int HttpSession::removeAttribute(const char* name) {
   return HttpSession_removeAttribute(this, name); }
inline int HttpSession::setAttribute(HttpSessionAttribute* value) {
   return HttpSession_setAttribute(this, value); }
inline void HttpSession::setMaxInactiveInterval(BaTime interval) {
   HttpSession_setMaxInactiveInterval(this, interval); }
inline void HttpSession::incrRefCntr() {
   HttpSession_incrRefCntr(this); }
inline void HttpSession::decrRefCntr() {
   HttpSession_decrRefCntr(this); }
inline U32 HttpSession::getId() { return HttpSession_getId(this); }
inline U32 HttpSession::getUseCounter() {
   return HttpSession_getUseCounter(this); }
inline AuthenticatedUser* HttpSession::getAuthenticatedUser() {
   return HttpSession_getAuthenticatedUser(this); }
inline void HttpSession::incrementLock() {HttpSession_incrementLock(this);}
inline void HttpSession::decrementLock() {HttpSession_decrementLock(this);}

#endif /* __cplusplus */

typedef enum {
   HttpSessionContainer_OK = 0,
   HttpSessionContainer_NoMemory,
   HttpSessionContainer_TooManySessions,
   HttpSessionContainer_NoPeerAddress
} HttpSessionContainer_ECode;


/** The HttpSession container class. You get a reference to this class from
    HttpServer::getSessionContainer.
 */
typedef struct HttpSessionContainer
{
#ifdef __cplusplus
      HttpSessionContainer() {}
      /** Set the maximum number of session objects. See
          HttpServerConfig::setMaxSessions for default value.
          See The <a href="../../authentication.html">
          authentication classes</a> for security considerations.
       */
      void setMaxSessions(int max);
   private:
#endif
      SplayTree sessionTree;
      DoubleList sessionList;
      DoubleList sessionTermList;
      DoubleLink* sessionLinkIter;
      struct HttpServer* server;
      int noOfSessions; /* Current number of active sessions */
      int maxSessions;
      HttpSessionContainer_ECode eCode;
} HttpSessionContainer;

#ifdef __cplusplus
extern "C" {
#endif
void HttpSessionContainer_constructor(struct HttpSessionContainer* o,
                                      struct HttpServer* server,
                                      U16 maxSessions);
void HttpSessionContainer_destructor(struct HttpSessionContainer* o);
void HttpSessionContainer_sessionTimer(struct HttpSessionContainer* o);
#define HttpSessionContainer_setMaxSessions(o,max) (o)->maxSessions = max
#ifdef __cplusplus
}
inline void HttpSessionContainer::setMaxSessions(int max) {
   HttpSessionContainer_setMaxSessions(this, max); }
#endif

#endif /* NO_HTTP_SESSION */


/** The HttpPage service function.
   All service functions must be of this type.
   \param page a pointer to the page object. This object can be typecasted
   to the overloaded type.
   \param request A pointer to the request object created by HttpServer.
   \param response A pointer to the response object created by HttpServer.
 */
typedef void (*HttpPage_Service)(struct HttpPage* page,
                                 HttpRequest* request,
                                 HttpResponse* response);


#define HttpPageType_HttpPageSE                0x00020

#ifndef __DOXYGEN__
typedef struct HttpPageNode
{
      struct HttpPageNode* next;
} HttpPageNode;
#endif

/** An HttpPage, which is typically created by the
    <a href = "../../CspTools.html#CspCompile">CSP compiler</a>, is
    similar to a
    <a href="https://en.wikipedia.org/wiki/Servlet">Java servlet</a>.

    Please see our <a href="https://realtimelogic.com/WP/intro/">
    introduction whitepaper</a> for an introduction to
    server side scripting using the CSP compiler. See the
    <a href="https://realtimelogic.com/WP/DeviceControl/">
    device control whitepaper</a> for an example of how to manually
    sub-class the HttpPage class.

   The page service function cannot be a virtual C++ function since
   the code must be compatible with C.  You must create a "static C++"
   callback method, which means that the method has no "this"
   pointer. The "this" pointer is passed into the static method as the
   first parameter in the callback function. The "static" callback
   function in the above code simply typecasts the HttpPage object to
   a MyPage object and calls the private service method, which has a
   "this" pointer.

   <B>Typical C usage:</B>
   \code
   struct MyPage
   {
         HttpPage page;
         int myData;
   };

   void MyPage_service(HttpPage* page,
                       HttpRequest* request,
                       HttpResponse* response)
   {
      MyPage* o = (MyPage*)page;
      o->myData++;
      HttpResponse_printf(response,
                          "<html><body>"
                          "Number of visits: %d"
                          "</body></html>",
                          myData);
   }

   MyPage_constructor(MyPage* o, const char* name)
   {
      HttpPage_constructor(&o->page, MyPage_service, name);
      o->myData = 0;
   }
   \endcode

   \sa HttpDir
*/
typedef struct HttpPage
{
#ifdef __cplusplus
      void *operator new(size_t s) { return ::baMalloc(s); }
      void operator delete(void* d) { if(d) ::baFree(d); }
      void *operator new(size_t, void *place) { return place; }
      void operator delete(void*, void *) { }
      HttpPage() {}
      /** The HttpPage constructor
       * \param service a pointer to a ::HttpPage_Service function.
       * \param name The page name, for example "index.html".
       * The pointer "name" must always be a valid pointer since the
       * constructor does not duplicate the string, but only stores
       * the pointer value. Note the name cannot be
       * \ref HttpDirVolatileMem "volatile".
       */
      HttpPage(HttpPage_Service service, const char* name);

      /** The HttpPage destructor unlinks the page from the parent directory.
       */
      ~HttpPage();

      /** Returns the page name.
       */
      const char* getName() const { return name; }

      /** The virtual service function (C callback function) is
          normally run by the parent directory when delegating the
          request to the page service method.
       */
      void service(HttpRequest* request, HttpResponse* response);

      /** Returns true if this page node is installed into a
       * parent directory.
       */
      bool isLinked();

      /** Unlinks/removes the page from the parent directory.
       */
      int unlink();
#endif
      HttpPageNode super; /* As if inherited */
      HttpPage_Service serviceCB;
      const char* name;
} HttpPage;

#ifdef __cplusplus
extern "C" {
#endif
void
BA_API HttpPage_constructor(
   HttpPage* o, HttpPage_Service service, const char* name);
BA_API void HttpPage_destructor(HttpPage* o);
#define HttpPage_getName(o) (o)->name
#define HttpPage_isLinked(o) ((HttpPageNode*)(o))->next
BA_API int HttpPage_unlink(HttpPage* o);
#define HttpPage_service(o, request, response) \
  (o)->serviceCB(o, request, response)
#ifdef __cplusplus
}
inline HttpPage::HttpPage(HttpPage_Service service, const char* name) {
   HttpPage_constructor(this, service, name); }
inline HttpPage::~HttpPage() {
   HttpPage_destructor(this); }
inline bool HttpPage::isLinked() {
   return HttpPage_isLinked(this) ? true : false;
}
inline int HttpPage::unlink() {return HttpPage_unlink(this); }
inline void HttpPage::service(HttpRequest* request, HttpResponse* response) {
   HttpPage_service(this, request, response);
}
#endif


/** The HttpDir service callback function.
    \param o the HttpDir instance
    \param relPath the relative path: absolute path - base path
    \param cmd the request/response object
 */
typedef int (*HttpDir_Service)(struct HttpDir* o,
                               const char* relPath,
                               HttpCommand* cmd);

#define HttpDirType_EhDir                    0x00001
#define HttpDirType_HttpResRdr               0x00002
#define HttpDirType_AuthenticateDir          0x00008
#define HttpDirType_AuthenticateDirWrapper   0x00010


/** An instance of the HttpDir class, which is a collection of zero or
    more resources, serves as one directory entry in the virtual file
    system.

    An instance of HttpDir class installed in the virtual file system
    searches for the next directory and delegates the request to the
    directory if found. The HttpDir class also delegates requests to
    HttpPage instances installed in the directory.

    The HttpDir class can be sub-classed and the functionality can be
    extended. The HttpDir instance is an ideal building block when
    designing
    <a href= "https://en.wikipedia.org/wiki/Representational_State_Transfer">
    Representational State Transfer</a> (RESTful) based applications.
    
    In a true RESTful application, every resource is uniquely
    addressable through the resource's URL. The URL can point to
    physical objects such as files handled by the HttpResMgr or to virtual
    resources created on demand from the URL itself.


    \sa HttpPage
 */
typedef struct HttpDir
{
#ifdef __cplusplus
      void *operator new(size_t s) { return ::baMalloc(s); }
      void operator delete(void* d) { if(d) ::baFree(d); }
      void *operator new(size_t, void *place) { return place; }
      void operator delete(void*, void *) { }
      /** Constructor for creating a root dir, a root dir has no name */
      HttpDir();

      /** Constructor for creating a sub-directory
          \param name the name of the directory.
          The pointer "name" must always be a valid pointer since the
          constructor does not duplicate the string, but only stores
          the pointer value. Note the name cannot be
          \ref HttpDirVolatileMem "volatile".
          \param priority The priority is used when you have <a
          href="../../CspTools.html#duplicateDir"> duplicate directory
          names</a>.
          Zero is the default priority and a higher value gives higher
          priority. Two directory branches with the same name, but with
          different pages will never conflict. You only have a conflict if
          you have two pages with the same name, one page in each directory
          branch. Range: min <= priority <=max, where min=-14 and
          max=14.
       */
      HttpDir(const char* name, S8 priority=0);
      ~HttpDir();

      /** Insert a sub-directory.
       * \param dir the directory to insert.
       */
      int insertDir(HttpDir* dir);

      /** Insert a page in the directory.
       * \param page the page to insert.
       */
      int insertPage(HttpPage* page);

      /** Returns the first page.
       */
      HttpPage* getFirstPage();

      /** Returns the first sub-directory.
       */
      HttpDir* getFirstDir();

      /** Returns the first directory with the name given or NULL if not found.
       */
      HttpDir* getDir(const char* name);

      /** Returns the page with the name given or NULL if not found.
       */
      HttpPage* getPage(const char* name);

      /** Returns the next dir in the parent list. (next sibling)
       */
      HttpDir* getNext();

      /** Searches for a page in this directory node.
       * \param iter a pointer to a reference in the page list.
       * \param name the name of the page to search for.
       * \return the page or NULL if not found.
       *<P><B>Typical usage:</B>
       * \code
       * HttpPage* page = dir->findPage(dir->getFirstPage(), "pageName");
       * \endcode
       */
      HttpPage* findPage(HttpPage* iter, const char* name);

      /** Searches for a sub-directory in this directory node.
       * \param iter a pointer to a reference in the directory list.
       * \param name the name of the directory to search for.
       * \param nameLen the length of name.
       * \return the directory or NULL if not found.
       *<P><B>Typical usage:</B>
       * \code
       * HttpDir* dir = dir->findDir(dir->getFirstDir(), "dirName",
       *                             strlen("dirName"));
       * \endcode
       */
      static HttpDir* findDir(HttpDir* iter, const char* name,
                              unsigned int nameLen);

      /** Returns a sub-directory with the given name. The directory will
          be created if not found.
          \code
          HttpDir* mySubDir3 = myDir->createOrGet("sub1/sub2/sub3");
          \endcode
       */
      HttpDir* createOrGet(const char* name);

      /** Returns the directory name.
       */
      const char* getName() const { return name; }

      /** Returns the parent directory or NULL if no parent.
       */
      HttpDir* getParent() const { return parent; }

      /** Makes an absolute path based on where the HttpDir instance is
          installed in the virtual file system and the provided
          relative path.

          This method is not fast. You should consider using method
          getRootPath and cache the root path. You can calculate the
          abs path from adding root path + relative path.

          absPath = installPath + relPath.

          \param relPath The relative path argument added to
          installPath. This path may include '..'
          i.e. makeAbsPath("..",2) returns the absolute path to the
          parent directory.

          Memory is allocated by using baMalloc. The
          caller must release the memory by using baFree.

          \param relPathLen length of relPath.

        \sa getRootPath
      */
      char* makeAbsPath(const char* relPath, int relPathLen);

      /** Calculates the root of where the HttpDir instance is
        installed in the virtual file system. This method is not fast
        and you should, therefore, cache the result. The path may,
        however, change if the HttpDir instance is dynamically moved to
        another location in the virtual file system.

        Memory is allocated by using baMalloc. The
        caller must release the memory by using baFree.

        \sa makeAbsPath
      */
      char* getRootPath();


      /** Replace the original service function in HttpDir with your own.
       * \return the original service function.
       */
      HttpDir_Service overloadService(HttpDir_Service s);

      /** Set a 403 denied request handler.
         The directory forwards the request to the page if an
         authorizer is installed and the user is denied access by the
         authorizer. The default for a directory is to send a basic
         403 message if a 403 denied request handler is not installed.
         \param p403 is the path to a page that can be accessed by
         response:forward. HttpDir makes a copy of the path provided
         and releases the path if/when the destructor is called.
       */
      void p403(const char* p403);

      /** Returns true if this directory node is installed into a
       * parent directory.
       */
      bool isLinked();

      /** Unlinks/removes the directory from the parent directory.
       */
      int unlink();

      /** Set the optional authenticator and/or the optional AuthorizerIntf.
          \param authenticator is one of the authenticator implementations.
          \param authorizer the authorizer.
      */
      void setAuthenticator(
         struct AuthenticatorIntf* authenticator,
         struct AuthorizerIntf* authorizer=0);

      /** Authenticate and authorize the user.

      This method provides a simple mechanism for
      authenticating and authorizing a user. The method takes care of
      the response message if the user is not authenticated and
      authorized -- i.e., you should not send a response message if this
      method returns false. See the DiskDir implementation for an
      example of how to use this method.

      \param cmd The parameter passed into the HttpDir service
      callback method.

      \param path The relative path (from the URL) at the current
      position in the virtual file system.
          
      \returns true if:
      \li The authenticator argument is NULL.
      \li The user is authenticated and the realm argument is NULL.
      \li The user is authenticated and authorized.
      */
      bool authenticateAndAuthorize(HttpCommand* cmd,const char* path);


#endif
      HttpDir_Service service;
      struct HttpDir* parent;      /* owner */
      struct HttpDir* next;        /* next sibling */
      const char* name;            /* e.g. /mydir/ */
      struct HttpDir* dirList;     /* list of httpdirs (children) */
      struct AuthorizerIntf* realm;  /* If we have authorization */
      struct AuthenticatorIntf* authenticator; /* If we have authentic. */
      char* _p403;          /* If we have a 403 denied response page. */
      HttpPageNode pageList;     /* pages for this dir */
      /* U8 type; */
      S8 priority;
} HttpDir;

#ifdef __cplusplus
extern "C" {
#endif
BA_API void HttpDir_constructor(HttpDir* o, const char* name, S8 priority);
BA_API void HttpDir_destructor(HttpDir* o);
BA_API char* HttpDir_makeAbsPath(
   HttpDir* o, const char* relPath, int relPathLen);
#define HttpDir_getRootPath(o) HttpDir_makeAbsPath(o,"",0)
BA_API int HttpDir_insertDir(HttpDir* o, HttpDir* dir);
#define HttpDir_getFirstPage(o) \
   (o)->pageList.next != &(o)->pageList ? ((HttpPage*)(o)->pageList.next) : 0
#define HttpDir_getFirstDir(o) (o)->dirList
BA_API HttpDir* HttpDir_getDir(HttpDir* o, const char* name);
BA_API HttpPage* HttpDir_getPage(HttpDir* o, const char* name);
#define HttpDir_getNext(o) (o)->next
#define HttpDir_getName(o) (o)->name
#define HttpDir_getParent(o) (o)->parent
BA_API int HttpDir_insertPage(HttpDir* o, HttpPage* page);
BA_API HttpPage* HttpDir_findPage(
   HttpDir* o, HttpPage* iter, const char* name);
BA_API HttpDir* HttpDir_findDir(
   HttpDir* iter, const char* name,unsigned int nameLen);
BA_API HttpDir* HttpDir_createOrGet(HttpDir* o, const char* name);
BA_API void HttpDir_p403(HttpDir* o, const char* p403);
BA_API HttpDir_Service HttpDir_overloadService(HttpDir*o, HttpDir_Service s);
#define HttpDir_isLinked(o) (o)->parent
BA_API int HttpDir_unlink(HttpDir* o);
#define HttpDir_setAuthenticator(o,authenticatorMA,authorizerMA) \
   (o)->authenticator = authenticatorMA,(o)->realm = authorizerMA
BA_API int HttpDir_authenticateAndAuthorize(
   HttpDir* o,HttpCommand* cmd,const char* path);
#define HttpDir_isAuthorized(o,user,method,path) \
  ((o)->realm ? \
  (user ? AuthorizerIntf_authorize((o)->realm,user,method,path) : FALSE) : \
  TRUE)
#ifdef __cplusplus
}
inline HttpDir::HttpDir() {
   HttpDir_constructor(this, 0, 0); }
inline HttpDir::HttpDir(const char* name, S8 priority) {
   HttpDir_constructor(this, name, priority); }
inline HttpDir::~HttpDir() {
   HttpDir_destructor(this); }
inline char* HttpDir::makeAbsPath(const char* relPath, int relPathLen) {
   return HttpDir_makeAbsPath(this, relPath, relPathLen); }
inline char* HttpDir::getRootPath() {
   return HttpDir_getRootPath(this); }
inline int HttpDir::insertDir(HttpDir* dir) {
   return HttpDir_insertDir(this, dir); }
inline int HttpDir::insertPage(HttpPage* page) {
   return HttpDir_insertPage(this, page); }
inline HttpPage* HttpDir::getFirstPage() { return HttpDir_getFirstPage(this); }
inline HttpDir* HttpDir::getFirstDir() { return HttpDir_getFirstDir(this); }
inline HttpDir* HttpDir::getDir(const char* name) {
   return HttpDir_getDir(this,name); }
inline HttpPage* HttpDir::getPage(const char* name) {
   return HttpDir_getPage(this,name); }
inline HttpDir* HttpDir::getNext() { return HttpDir_getNext(this); }
inline HttpPage* HttpDir::findPage(HttpPage* iter, const char* name) {
   return HttpDir_findPage(this, iter, name); }
inline HttpDir* HttpDir::findDir(HttpDir* iter, const char* name,
                                        unsigned int nameLen) {
   return HttpDir_findDir(iter, name, nameLen); }
inline HttpDir* HttpDir::createOrGet(const char* name) {
   return HttpDir_createOrGet(this, name); }
inline HttpDir_Service HttpDir::overloadService(HttpDir_Service s) {
   return HttpDir_overloadService(this, s); }
inline void HttpDir::p403(const char* p403) {
   HttpDir_p403(this, p403); }
inline int HttpDir::unlink() {return HttpDir_unlink(this); }
inline bool HttpDir::isLinked() {
   return HttpDir_isLinked(this) ? true : false; }
inline void HttpDir::setAuthenticator(struct AuthenticatorIntf* authenticator,
                                      struct AuthorizerIntf* authorizer){
   HttpDir_setAuthenticator(this,authenticator,authorizer);
}
inline bool HttpDir::authenticateAndAuthorize(
   HttpCommand* cmd, const char* path) {
   return HttpDir_authenticateAndAuthorize(this,cmd,path) ? true : false; }
#endif

#ifndef __DOXYGEN__

typedef struct HttpLinkCon
{
      HttpConnection con; /* Inherits from HttpConnection */
      DoubleLink link;
} HttpLinkCon;


typedef struct
{
      HttpDir super; /* inherit */
      HttpDir_Service superServiceFunc;
      char* page404;
      BaBool page404InProgress;
} HttpRootDir;

#endif

/**
   Use an instance of this class if you want to override the default
   web-server parameters. An instance of this class can be created on
   the stack in a function. The object is of no use after the call to
   the HttpServer constructor. You should not create an instance of
   this class unless you want to change the default parameters in an
   HttpServer instance.
*/
typedef struct HttpServerConfig
{
#ifdef __cplusplus
      /** The constructor sets up the default parameters. Use one or
          several of the methods in this class after you have created
          this object to change one or more of the configuration
          parameters.
      */
      HttpServerConfig();

      /** Set the size of the HTTP request buffer. This buffer is used by
          the web-server when parsing an incoming HTTP request. The
          buffer must be big enough to contain all HTTP headers, the
          request path, and all HTTP parameters.

          See HttpRequest::getHeaderValue for more information on HTTP
          headers.<BR>
          See HttpRequest::getParameter and HttpParameterIterator for
          more information on HTTP parameters.

          The buffer is used for storing HTTP headers and for storing
          URL encoded data from HTTP requests. The buffer is also used
          for storing POST data if the client sends
          x-www-form-urlencoded data.

          Default values: min= 1024, max= 2048. Set min = max if you
          do not want the buffer to dynamically grow if needed. The
          minimum value cannot be smaller than 1024.

          It is recommended to set the max size to at least 4096 bytes
          if the HttpCmdThreadPool is enabled. The reason for this is
          that the web-server is sending response data and reading
          pipelined data at the same time when using the thread
          pool. The web-server has no option but to terminate the
          connection if the buffer overflows. See
          HttpTrace::setReqBufOverflow and the
          <a href="../../misc/HttpCmdThreadPool.html"> Http Command Thread
          Pool </a> documentation for more information.
      */
      int setRequest(S16 min, S16 max);

      /** Set the size of the HTTP response header buffer. This buffer
          is used by the web-server for storing the HTTP response
          headers.

          See HttpResponse::setHeader and HttpResponse::committed
          for more information.

          Default values: min= 512, max= 1024. Set min = max if you do
          not want the buffer to dynamically grow if needed. The
          minimum value cannot be smaller than 512.
       */
      int setResponseHeader(U16 min, U16 max);

      /** The HttpResponse object stores formatted data in the
          response data buffer. You add data to this buffer when using
          HttpResponse::printf and HttpResponse::write. The web-server
          flushes the buffer automatically when full. It is sometimes
          convenient to have a large buffer if you implement rollback
          handling; i.e., you print response data, but later decide to
          erase the data in the buffer. See HttpResponse::reset,
          HttpResponse::resetBuffer and HttpResponse::committed for
          more information.

          Default value is 1400. The minimum value cannot be smaller than 512.
       */
      int setResponseData(U16 size);

      /** Set the size of the HTTP response commit buffer. This buffer
          is used by the web-server when formatting the HTTP response
          header data. This buffer should be bigger than the HTTP
          response header buffer for best performance. A smaller
          buffer makes the web-server call socket send every time the
          buffer is full. This might deteriorate the performance of
          the web-server. See your TCP/IP stack for internal TCP
          buffer and the "naggle" algorithm.

          See HttpResponse::committed for more information.

          Default value is 512. The minimum value cannot be smaller than 128.
       */
      int setCommit(U16 size);

      /** The number of HttpCommand instances created by the
          web-server. This is by default set to one. You should not
          change this value unless you use the HttpCmdThreadPool
          class.

          Creating more than one instance of this class is
          expensive. The total memory consumption is N*M, where N is
          the number of HttpCommand instances and M is the sum of all
          buffers in an HttpCommand. M = Request buffer +
          ResponseHeader buffer + ResponseData buffer + Commit buffer
          + the size of HttpCommand.
       */
      int setNoOfHttpCommands(U16 size);

      /** Number of HttpConnection instances. An HttpConnection object
          is the web-server's socket connection. The web-server
          supports HTTP1.1 persistent connections; thus, the
          web-server may maintain many idle client connections
          simultaneously. It is recommended to set the size to 3
          times the number of HttpCommand objects. An HttpConnection
          object can also queue incoming requests if the
          HttpCmdThreadPool class is used.

          The size of one HttpConnection object is roughly 40 bytes,
          but be aware that this object can potentially
          hold large amounts of data if the connection is secure. A
          secure SSL connection may have to buffer its data stream
          before coding/decoding the data. See your SSL stack for more
          information.

          Default value is 16. Minimum value is NoOfHttpCommands + 3.

          See the [HTTP Engine and Sockets](@ref MaxSockets) for more
          information on using this function.
       */
      int setNoOfHttpConnections(U16 size);

      /** Maximum allowed active HttpSession objects. The size can
          also be changed during runtime with method
          HttpSessionContainer::setMaxSessions.
          Default value is set equal to NoOfHttpConnections.
       */
      int setMaxSessions(U16 size);
#endif
      S16 minRequest;
      S16 maxRequest;
      U16 minResponseHeader;
      U16 maxResponseHeader;
      U16 commit;
      U16 responseData;
      U16 noOfHttpCommands;
      U16 noOfHttpConnections;
      U16 maxSessions;
} HttpServerConfig;

#ifdef __cplusplus
extern "C" {
#endif
BA_API void HttpServerConfig_constructor(HttpServerConfig* o);
BA_API int HttpServerConfig_setRequest(HttpServerConfig* o, S16 min, S16 max);
BA_API int HttpServerConfig_setResponseHeader(
   HttpServerConfig* o, U16 min, U16 max);
BA_API int HttpServerConfig_setResponseData(HttpServerConfig* o, U16 size);
BA_API int HttpServerConfig_setCommit(HttpServerConfig* o, U16 size);
BA_API int HttpServerConfig_setNoOfHttpCommands(HttpServerConfig* o, U16 size);
BA_API int HttpServerConfig_setNoOfHttpConnections(
   HttpServerConfig* o, U16 size);
BA_API int HttpServerConfig_setMaxSessions(HttpServerConfig* o, U16 size);
#ifdef __cplusplus
}
inline HttpServerConfig::HttpServerConfig() {
   HttpServerConfig_constructor(this); }
inline int HttpServerConfig::setRequest(S16 min, S16 max) {
   return HttpServerConfig_setRequest(this, min, max); }
inline int HttpServerConfig::setResponseHeader(U16 min, U16 max) {
   return HttpServerConfig_setResponseHeader(this, min, max); }
inline int HttpServerConfig::setResponseData(U16 size) {
   return HttpServerConfig_setResponseData(this, size); }
inline int HttpServerConfig::setCommit(U16 size) {
   return HttpServerConfig_setCommit(this, size); }
inline int HttpServerConfig::setNoOfHttpCommands(U16 size) {
   return HttpServerConfig_setNoOfHttpCommands(this, size); }
inline int HttpServerConfig::setNoOfHttpConnections(U16 size) {
   return HttpServerConfig_setNoOfHttpConnections(this, size); }
inline int HttpServerConfig::setMaxSessions(U16 size) {
   return HttpServerConfig_setMaxSessions(this, size); }

#endif


typedef DoubleList HttpLinkConList;

/* Used by LSP plugin */
typedef void (*LspOnTerminateRequest)(struct LHttpCommand* lcmd);

/** The Web Server
 */
typedef struct HttpServer
{
#ifdef __cplusplus
      void *operator new(size_t s) { return ::baMalloc(s); }
      void operator delete(void* d) { if(d) ::baFree(d); }
      void *operator new(size_t, void *place) { return place; }
      void operator delete(void*, void *) { }
      /** Create a Web Server object.
         \param dispatcher The socket dispatcher object.
         The SoDisp object is platform specific.
         \param cfg is an optional parameter, which you use to
         override the default web-server configurations.
       */
      HttpServer(SoDisp* dispatcher, HttpServerConfig* cfg=0);

      ~HttpServer();

      /** Insert a root directory node. A root directory node is a
          special directory in which the name of the directory is
          ignored. The directory name can be NULL. For example, if you
          insert a directory node with a page, say "start.html", the
          URL to this page will be: http://{IP address}/start.html.

          \param dir the directory node to install. A root directory node
          does not need a name. See HttpDir::HttpDir for more information.
          \return 0 on success or -1 if the directory is already installed
          in the virtual file system.
       */
      int insertRootDir(HttpDir* dir);

      /** Insert a directory node into the virtual file system.  Here
          are some examples if you insert a directory node with a
          page, say "start.html":

<table border="1" style="text-align: left">
<tr><th>virtualDirRootPath</th> <th>HttpDir name</th>
<th>URL</th></tr> <tr><td>"D1/D2"</td><td>"D3"</td><td>http://{IP
address}/D1/D2/D3/start.html</td></tr>
<tr><td>"D1"</td><td>"D3"</td><td>http://{IP
address}/D1/D3/start.html</td></tr> <tr><td>NULL or
"/"</td><td>"D3"</td><td>http://{IP address}/D3/start.html</td></tr>
<tr><td>NULL or "/"</td><td>NULL</td><td>http://{IP
address}/start.html <b>(*)</b></td></tr>
<tr><td>"D1"</td><td>NULL</td><td>Invalid</td></tr>
</table>

          (*) Identical to HttpServer::insertRootDir

          \param virtualDirRootPath start path in the virtual file system.
          \param dir the directory node to insert.
      */
      int insertDir(const char* virtualDirRootPath, HttpDir* dir);

      /** Insert and initialize a CSP Virtual Directory.
          A CSP Virtual Directory structure is generated by
         <a href="../../CspTools.html#CspCompile"> CspCompile </a> and
         <a href="../../CspTools.html#CspLink"> CspLink </a>.

         \param cspInit is the function generated by CspLink. The default
         name is httpInitGeneratedCode, but this can be changed with
         the CspLink --init flag.

         \param reader is a reader such as FileCspReader or a reader
         generated by the
         <a href="../../CspTools.html#bin2c">bin2c
         </a>, if you embed the data file into the executable.

         \param virtualDirRootPath is the root path of the CSP pages.

         <b>Example:</b>

         \code
         extern "C" void //See HttpLink for more info.
           httpInitGeneratedCode(HttpDir* parent, CspReader* reader);
         void insertCspIntoRootDir(HttpServer* server, CspReader* reader)
         {
           server->insertCSP(httpInitGeneratedCode, "/", reader);
         }
         \endcode

         It is sometimes more convenient to populate a CSP Virtual
         File System directly on a HttpResRdr node.

         <b>Example:</b>

         \code
          DiskIo io;
          HttpResRdr myRootDir(&io, NULL);
          FileCspReader reader("/home/webserver/CspPages.dat");
          httpInitGeneratedCode(&myRootDir, &reader);
         \endcode
      */
      int insertCSP(CspInit cspInit,
                    const char* virtualDirRootPath,
                    struct CspReader* reader);

      struct AuthUserList* getAuthUserList(const char* name);

      /** Get the dispatcher mutex */
      ThreadMutex* getMutex();

      /** Returns the dispatcher object */
      SoDisp* getDispatcher();

      /** Returns the first root directory.
          Returns the first root directory or NULL if no root directory is
          installed.
       */
      HttpDir* getFirstRootDir();

      /** Get the HttpSessionContainer
       */
      HttpSessionContainer* getSessionContainer();

#ifndef NO_HTTP_SESSION
      /** Returns the HttpSession associated with id or NULL if not found.
          See HttpSession::getId for more information.

          It can be potentially dangerous to use this method in a
          multithread enabled web-server. See the explanation in the
          HttpSession for more information.

          \sa HttpSession::incrRefCntr
       */
      HttpSession* getSession(U32 id);
#endif

      /** Set a more user friendly 404 page. Have you ever noticed
          that some sites give you a nice looking page when you
          mistype an URL, as opposed to the default nasty 404 File Not
          Found error? This function lets you set your own user
          friendly 404 page.
         \param page404 is a URL to your user friendly 404 page.
         Example "/myUserFriendly404Page.html"
      */
      void set404Page(const char* page404);

      /** Returns a pointer to the current 404 page, if any. */
      const char* get404Page();

      int setUserObj(void* userObj, bool overwrite=false);

      /** The only purpose with this function is to clean all static
          variables that are in the BSS section; i.e., you do not need
          to call this function if you properly clear your static
          uninitialized variables.
      */
      static void initStatic(void);

      /**You can set your own user defined error handler for the web-server.
       * The web-server calls this function if any non-recoverable error is
       * detected. The error codes are defined in BaErrorCodes.h. See the
       * example directory for an example on how to write your own error
       * handler.
       * \param e is the name of your error handler function.
       * The prototype for this function should be:
       * void myError(BaErrorCodes ecode1,
       * unsigned int ecode2, const char* file, int line);
       */
      static void setErrHnd(UserDefinedErrHandler e);

      /* No longer used */
      static int setZlib(ZlibInflateInit2 init,
                         ZlibInflate inflate,
                         ZlibInflateEnd end){
                            (void)init; (void)inflate; (void)end;
                            return 0;                         }
      /** Return a short description for common HTTP error codes.
       */
      static const char* getStatusCode(int code);


#endif
      DoubleList commandPool;
      DoubleList cmdReqList;
      HttpConnection noOpCon;
      HttpLinkConList freeList;
      HttpLinkConList readyList;
      HttpLinkConList connectedList;
      HttpRootDir rootDirContainer;
      SplayTree authUserTree; /* Used by AuthenticatedUser.c */
      struct HttpCmdThreadPoolIntf* threadPoolIntf;
      HttpLinkCon* connections;
      SoDisp* dispatcher;
      void* userObj;
      void* waitForConClose; /* See HttpServer_doLingeringClose */
      LspOnTerminateRequest lspOnTerminateRequest;
      int commandPoolSize;
      U16 noOfConnections;
      S16 maxHttpRequestLen;
#ifndef NO_HTTP_SESSION
      HttpSessionContainer sessionContainer;
#endif
} HttpServer;


/* Used exclusively by HttpCmdThreadPool */
void HttpServer_AsynchProcessDir(HttpServer* o,
                                 HttpDir* dir,
                                 HttpCommand* cmd);
/* Used exclusively by HttpCmdThreadPool */
#define HttpServer_setThreadPoolIntf(o, intf) (o)->threadPoolIntf=intf
#define HttpServer_luaenv(o) (o)->luaenv

#ifdef __cplusplus
extern "C" {
#endif
BA_API void HttpServer_constructor(HttpServer*,SoDisp*, HttpServerConfig*);
BA_API void HttpServer_destructor(HttpServer* o);
BA_API int HttpServer_insertRootDir(HttpServer* o, HttpDir* dir);
BA_API int HttpServer_insertDir(HttpServer* o,
                                const char* virtualDirRootPath,
                                HttpDir* dir);
BA_API int HttpServer_insertCSP(HttpServer* o,
                                CspInit cspInit,
                                const char* virtualDirRootPath,
                                struct CspReader* reader);
#define HttpServer_getAuthUserList(o, name) \
   (AuthUserList*)SplayTree_find(&(o)->authUserTree, name)
#define HttpServer_getDispatcher(o) (o)->dispatcher
#define HttpServer_getFirstRootDir(o) \
     HttpDir_getFirstDir((HttpDir*)&(o)->rootDirContainer)
#define HttpServer_getUserObj(o) (o)->userObj
#define HttpServer_getSessionContainer(o) (&(o)->sessionContainer)
#define HttpServer_getMutex(o) SoDisp_getMutex((o)->dispatcher)
BA_API const char* HttpServer_getStatusCode(int code);
void HttpServer_addCon2ConnectedList(HttpServer* o, HttpConnection* con);
void HttpServer_doLingeringClose(
   HttpServer* o, HttpConnection* con, BaFileSize contLen);

#ifndef NO_HTTP_SESSION
BA_API HttpSession* HttpServer_getSession(HttpServer* o, U32 id);
#endif
BA_API HttpConnection* HttpServer_getFreeCon(HttpServer* o);
void HttpServer_returnFreeCon(HttpServer* o, HttpConnection* con);
BA_API void HttpServer_installNewCon(HttpServer* o, HttpConnection* con);
BA_API void HttpServer_setErrHnd(UserDefinedErrHandler e);
void HttpServer_initStatic(void);
int HttpServer_termOldestIdleCon(HttpServer* o);
BA_API void HttpServer_set404Page(HttpServer*o, const char* page404);
#define HttpServer_get404Page(o) (o)->rootDirContainer.page404
BA_API int HttpServer_setUserObj(
   HttpServer* o, void* userObj, BaBool overwrite);
#define HttpServer_termAllSessions(o) \
   HttpSessionContainer_destructor(&(o)->sessionContainer)
/* No longer used */
#define HttpServer_setZlib(init,inflate,end);
#ifdef __cplusplus
}
inline HttpServer::HttpServer(SoDisp* disp, HttpServerConfig* cfg){
   HttpServer_constructor(this, disp, cfg); }
inline HttpServer::~HttpServer()  {
   HttpServer_destructor(this); }
inline int HttpServer::insertRootDir(HttpDir* dir) {
   return HttpServer_insertRootDir(this, dir); }
inline ThreadMutex* HttpServer::getMutex() {
   return HttpServer_getMutex(this);
}
inline int HttpServer::insertDir(const char* virtualDirRootPath,HttpDir* dir){
   return HttpServer_insertDir(this, virtualDirRootPath, dir); }
inline int HttpServer::insertCSP(CspInit cspInit,
                                 const char* virtualDirRootPath,
                                 struct CspReader* reader) {
   return HttpServer_insertCSP(this, cspInit, virtualDirRootPath, reader); }
inline struct AuthUserList* HttpServer::getAuthUserList(const char* name) {
   return HttpServer_getAuthUserList(this,name); }
inline SoDisp* HttpServer::getDispatcher() {
   return HttpServer_getDispatcher(this); }
inline HttpDir* HttpServer::getFirstRootDir() {
   return HttpServer_getFirstRootDir(this); }
inline HttpSessionContainer* HttpServer::getSessionContainer() {
   return HttpServer_getSessionContainer(this); }
#ifndef NO_HTTP_SESSION
inline HttpSession* HttpServer::getSession(U32 id) {
   return HttpServer_getSession(this, id); }
inline void HttpServer::initStatic(void) {
   HttpServer_initStatic(); }
inline void HttpServer::setErrHnd(UserDefinedErrHandler e) {
   HttpServer_setErrHnd(e); }
inline void HttpServer::set404Page(const char* page404) {
   HttpServer_set404Page(this, page404); }
inline const char* HttpServer::get404Page() {
   return HttpServer_get404Page(this); }
inline int HttpServer::setUserObj(void* userObj, bool overwrite) {
   return HttpServer_setUserObj(this, userObj, overwrite); }
inline const char* HttpServer::getStatusCode(int code) {
   return HttpServer_getStatusCode(code);
}
#endif


inline HttpResponse* HttpRequest::getResponse() {
   return HttpRequest_getResponse(this); }
inline HttpConnection* HttpRequest::getConnection() {
   return HttpRequest_getConnection(this); }
inline HttpRequest* HttpResponse::getRequest() {
   return HttpResponse_getRequest(this); }
inline HttpCommand* HttpResponse::getCommand() {
   return HttpResponse_getCommand(this); }
inline HttpCommand* HttpRequest::getCommand() {
   return HttpRequest_getCommand(this); }
inline struct HttpServer* HttpConnection::getServer() {
   return HttpConnection_getServer(this); }

#endif /* __cplusplus */

/** @} */ /* end of StdWebServer */

/* Internal use */
#ifdef NO_SHARKSSL

#define SHARKSSL_MD5_HASH_LEN       16
#define SHARKSSL_SHA1_HASH_LEN      20

typedef struct SharkSslMd5Ctx
{
   U32 total[2];
   U32 state[4];
   U8  buffer[64];
} SharkSslMd5Ctx;

SHARKSSL_API void  SharkSslMd5Ctx_constructor(SharkSslMd5Ctx* ctx);
SHARKSSL_API void  SharkSslMd5Ctx_append(SharkSslMd5Ctx* ctx, const U8* data, U32 len);
SHARKSSL_API void  SharkSslMd5Ctx_finish(SharkSslMd5Ctx* ctx, U8 digest[]);
SHARKSSL_API int   sharkssl_md5(const U8*, U16, U8*); 

typedef struct SharkSslSha1Ctx
{
   U32 total[2];
   U32 state[5];
   U8  buffer[64];
} SharkSslSha1Ctx;

SHARKSSL_API void  SharkSslSha1Ctx_constructor(SharkSslSha1Ctx* ctx);
SHARKSSL_API void  SharkSslSha1Ctx_append(SharkSslSha1Ctx* ctx, const U8* data, U32 len);
SHARKSSL_API void  SharkSslSha1Ctx_finish(SharkSslSha1Ctx*, U8 digest[]);

typedef void SharkSsl;

#endif

#endif /* __httpServer_h */
