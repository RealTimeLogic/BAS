/*
 *     ____             _________                __                _     
 *    / __ \___  ____ _/ /_  __(_)___ ___  ___  / /   ____  ____ _(_)____
 *   / /_/ / _ \/ __ `/ / / / / / __ `__ \/ _ \/ /   / __ \/ __ `/ / ___/
 *  / _, _/  __/ /_/ / / / / / / / / / / /  __/ /___/ /_/ / /_/ / / /__  
 * /_/ |_|\___/\__,_/_/ /_/ /_/_/ /_/ /_/\___/_____/\____/\__, /_/\___/  
 *                                                       /____/          
 *
 *                  Barracuda Application Server
 *
 ****************************************************************************
 *			      HEADER
 *
 *   $Id: HttpClient.h 6056 2026-09-20 05:09:33Z wini $
 *
 *   COPYRIGHT:  Real Time Logic LLC, 2009-2026
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
 */
/** @file HttpClient.h */

#ifndef __HttpClient_h
#define __HttpClient_h

#include <DynBuffer.h>
#ifndef NO_SHARKSSL
#include <SharkSslEx.h>
#endif

struct HttpClient;


/** @defgroup HttpClient The HTTP(S) client library.
    @ingroup Plugins

    The HttpClient library is the C side implementation for the Lua <a
    href="../../../lua/auxlua.html#httpc">httpc</a> library. The
    library is also used internally by the NetIo network file
    system. See the <a href="../../../lua/auxlua.html#httplib">Lua
    HTTP client</a> for an introduction to this library.

    This documentation is for the C++ API defined in the headers. See
    the introduction to <a href="../../introduction.html#oo_c">object
    oriented code in C</a> for an introduction to the C API's.

    The HTTP client library can also be compiled into a standalone
    library. The HTTP client source code requires a few runtime files
    from the Barracuda Server such as the SoDisp, BufPrint, DynBuffer,
    HttpConnection, HttpServCon, and HttpSocket.

    The HTTP client library can also be used standalone together with
    the SharkSSL SSL/TLS stack when secure HTTP (HTTPS) is required.
   @{
*/

/** A container for key/value pairs used when setting custom HTTP
    headers or URL encoded HTTP parameters.

    HttpClientKeyVal can be statically declared at compile time or be
    dynamically created during runtime. A dynamically created
    HttpClientKeyVal can be released as soon as
    HttpClient::request returns.

Example code:
\code
   HttpClientKeyVal myHeaders[]={
      {"the-header-name","the header value"},
      {"foo","bar"},
      {0,0} // Terminator
   };
\endcode
 */
typedef struct HttpClientKeyVal
{
      /** NUL-terminated name; NULL terminates an input array. */
      const char* key;
      /** Required NUL-terminated value for a non-NULL key. */
      const char* val;
} HttpClientKeyVal;



/** The response HTTP headers returned by HttpClient::getHeaders.
 */
typedef struct
{
#ifdef __cplusplus
      /** @param c Required client that supplied this header.
          @return Borrowed NUL-terminated header name; same lifetime as getHeaders().
       */
      const char* getKey(HttpClient* c);
      /** @param c Required client that supplied this header.
          @return Borrowed NUL-terminated header value; same lifetime as getHeaders().
       */
      const char* getVal(HttpClient* c);
#endif
      U16 key;
      U16 val;
} HttpClientHeader;

/** C accessor: c is the required owning client and o its header entry.
    @return Borrowed NUL-terminated name, with the lifetime documented by getHeaders(). */
#define HttpClientHeader_key(c,o) ((c)->db.super.buf + (o)->key)
/** C accessor: c is the required owning client and o its header entry.
    @return Borrowed NUL-terminated value, with the lifetime documented by getHeaders(). */
#define HttpClientHeader_val(c,o) ((c)->db.super.buf + (o)->val)


#define HTTP_CLIENT_MAX_HEADERS 64

#define HttpClient_SocksProxy   1
#define HttpClient_ProxyConnect 2
#define HttpClient_Persistent   4
#define HttpClient_IPv6         8
/* Reserved for future use */
#define HttpClient_NoHostHeader 16
#define HttpClient_NotUsed2    32
/* May be set by code using the HttpClient lib */
#define HttpClient_UserDef1    64
#define HttpClient_UserDef2   128


/** Blocking HTTP/1.1 client with optional SharkSSL HTTPS support.

    Send a request, send its body if applicable, obtain response headers/status,
    and read the response body. Serialize access to each instance. When the
    dispatcher has a mutex, hold it when entering the API; blocking transport
    operations release and reacquire it internally.

    This example streams a GET response without relying on a prior HEAD request
    or allocating storage for the complete response. The destination writer is
    owned by the caller and is not flushed here. An error can leave partial output.
    @code
    int copyURL(HttpClient* http, const char* url, BufPrint* dest)
    {
       char buf[512];
       int status = http->request(HttpMethod_Get, url);
       if(status == 0)
       {
          int count;
          // This example accepts only HTTP 200 responses.
          status = http->getStatus();
          if(status == 200)
          {
             status = 0;
             while((count = http->readData(buf, sizeof(buf))) > 0)
             {
                status = dest->write(buf, count);
                if(status < 0) break;
             }
             if(count < 0) status = count;
          }
          else if(status >= 0) status = -1;
       }
       http->close();
       return status;
    }
    @endcode

    @note The parser accepts status codes 100 through 599. Informational
    responses are consumed until a final response, except 101 protocol switching.
    Chunked response trailers are validated and discarded, not exposed as headers.
    Chunk-size lines must be shorter than 256 bytes, excluding CRLF; the entire
    trailer section including its terminating CRLF is limited to 8192 bytes.
    Content-Encoding (for example gzip) is not decoded. Applications must check
    the HTTP status separately from transport success.
*/
typedef struct HttpClient
#ifdef __cplusplus
: public SoDispCon
{
      /** Construct an idle client; no connection is opened.
        @param disp Required borrowed dispatcher, which must outlive this client.
        @param mode Bitwise combination of HttpClient_Persistent (the C++ default),
        HttpClient_SocksProxy, and HttpClient_IPv6. Zero disables these options.
        Persistent mode permits connection reuse; IPv6 selects IPv6 resolution
        where supported. SOCKS selects SOCKS instead of an HTTP CONNECT proxy.
        Configure proxy, proxyPortNo, proxyUserPass, and intfName before connecting.
        Without a dispatcher mutex, use the client from a single thread.
        The initial read timeout is 20000 milliseconds. Trust enforcement is off.
       */
      HttpClient(SoDisp* disp, U8 mode=HttpClient_Persistent);

      /** Close the connection and release client-owned storage. Borrowed dispatcher,
        TLS context, and configuration strings are not destroyed. Stop all users first.
       */
      ~HttpClient();

      /** Select the TLS context for future HTTPS connections.
        @param ssl Borrowed, configured SharkSSL client context, or NULL to disable
        new HTTPS connections. Keep it alive until all connections using it close.
        Close an existing connection before replacing its TLS configuration.
        This setter does not configure certificates or enable trust enforcement;
        see setAcceptTrusted().
       */
      void setSSL(SharkSsl* ssl);

      /** Set the timeout applied to individual blocking reads.
        @param timeout Milliseconds, default 20000. Use values representable by
        SoDispCon::setReadTmo (50 through 3276750, in 50 ms steps for exact timing).
        Values are rounded down to that transport's 50 ms resolution.
        @note request() replaces zero with 100 ms. This is not a total request
        deadline; DNS lookup and separate reads do not share one time budget.
       */
      void setReadTmo(BaTime timeout);

      /** Classify a URL scheme; this does not validate the complete URL.
        @param url Required NUL-terminated URL string.
        @return 1 for http://, 2 for https://, 3 for ws://, 4 for wss://,
        or E_INVALID_URL for an unrecognized prefix. Prefix matching is case-sensitive.
       */
      static int isURL(const char* url);

      /** Query the current connection's certificate trust result.
        @return A SharkSslConTrust value, including SharkSslConTrust_NotSSL when
        no SharkSSL connection is available. Certificate, host-name, and date
        checks use the configured SharkSSL context and current host.
        See the [SharkSSL reference](../../shark/group__SharkSslInfoAndCodes.html).
       */
      SharkSslConTrust trusted(void);

      /** Choose whether new connections must pass the TLS trust check.
        @param acceptTrusted True requires SharkSslConTrust_CertCnDate before
        sending the HTTP request; failure returns E_NOT_TRUSTED. False (default)
        disables this requirement. Plain HTTP is rejected when enabled.
        Set this before connecting. Changing it does not recheck a reused connection.
       */
      void setAcceptTrusted(bool acceptTrusted);

      /** Send request headers, opening or reusing a connection.
        @param methodType HttpMethod_Delete, HttpMethod_Get, HttpMethod_Head,
        HttpMethod_Patch, HttpMethod_Post, or HttpMethod_Put.
        @param url Required NUL-terminated absolute URL, including host and path.
        HTTPS requires setSSL(). The ws/wss schemes are also recognized, but
        this call alone does not complete a WebSocket upgrade.
        @param userPass Optional NUL-terminated "user:password" for Basic
        authentication; NULL omits it.
        @param query Optional array of decoded query names and values. Both are
        URL-encoded. Terminate the array with a NULL key; each other value must
        be a non-NULL NUL-terminated string.
        @param headers Optional custom header array with the same terminator.
        Supply valid HTTP names/values without embedded CR or LF.
        @param size Body length in bytes for POST, PUT, PATCH, or DELETE. Zero
        selects chunked encoding. GET and HEAD ignore this argument.
        All argument strings/arrays need remain valid only during this call.
        @return Zero when request headers have been sent, or a negative error
        code (BaErrorCodes.h). Zero does not indicate an HTTP success response.
        Finish any fixed-length body with sendData() before reading the response.
        Calling getHeaders(), getHeaderValue(), or readData() finishes a chunked
        upload and reads the response headers. getStatus() does so when no status
        is already cached. Calling request() again before managing the prior
        response returns E_INCORRECT_USE; use close() to abandon it.
        @note Advanced HttpClient_ProxyConnect mode returns E_PROXY_READY when
        its proxy tunnel is ready, before sending an ordinary HTTP request.
       */
      int request(HttpMethod methodType,
                  const char* url,
                  const char* userPass=0,
                  const HttpClientKeyVal* query=0,
                  const HttpClientKeyVal* headers=0,
                  BaFileSize size=0);

      /** Send part or all of the request body after request().
        @param data Required readable buffer when len is positive; borrowed for
        this call only. Binary bytes are sent unchanged.
        @param len Nonnegative byte count. Zero is a no-op, not an end marker.
        For fixed-length requests, do not exceed the remaining declared length.
        @return Zero on success, a negative error code, or a positive HTTP status
        if an early server response is available after a send failure. Stop on
        any nonzero result; no partial byte count is returned.
        To finish chunked output, obtain the response rather than sending zero bytes.
       */
      int sendData(const void* data, int len);

      /** Query bytes already buffered beyond the response headers.
        @return Nonnegative buffered byte count, or zero when none are available.
        This does not read headers or report allocation capacity. For chunked
        responses, buffered bytes may include chunk framing.
       */
      int getBufSize();

      /** Read response body bytes, removing chunk framing when applicable.
        Completes the outgoing request and reads response headers if needed.
        @param buf Writable buffer; returned data is binary and not NUL-terminated.
        NULL only processes the response headers and returns zero without consuming
        the body, so that use cannot be interpreted as end-of-body.
        @param bufSize Positive buffer capacity in bytes when buf is non-NULL.
        @return Positive bytes stored, zero at end-of-body (including HEAD), or a
        negative error code. If a fixed-length read fails after copying buffered
        bytes, their count is returned and getError() records the failure; the
        next read returns that error. Other failures may modify the buffer without
        reporting a partial count. Reading before the request body is completely sent
        returns E_INCORRECT_USE.
        @note Closure or a read failure before Content-Length bytes arrive records
        E_INVALID_RESPONSE, even with Connection: close. The cached HTTP status
        is independent of this body error. For a body delimited by connection
        closure (without Content-Length), transport read failure
        is treated as end-of-body. This cannot distinguish every truncated response.
       */
      int readData(void* buf, int bufSize);

      /** Look up the first response header with this name, ignoring case.
        May finish the outgoing request and block while reading response headers.
        @param name Required NUL-terminated header name.
        @return Borrowed NUL-terminated value, or NULL if absent or response
        processing fails. Use getError() to inspect a stored failure. Copy a value
        needed beyond the next request, close(), or destruction of the client.
       */
      const char* getHeaderValue(const char* name);

      /** Obtain parsed response headers; may finish an upload and block for input.
        @param hlen Required output pointer receiving the number of array entries;
        receives zero on failure.
        @return Borrowed array on success (possibly with zero entries), or NULL
        on failure. Entries and their strings belong to this client; copy anything
        needed beyond the next request, close(), or destruction. Use
        HttpClientHeader::getKey() and HttpClientHeader::getVal() with this client.
       */
      HttpClientHeader* getHeaders(int* hlen);

      /** Shut down the connection and discard the current response state.
        The object can be reused by request(). This does not free the dispatcher
        or TLS context, report a close error, or reset cached status/error values.
       */
      void close();

      /** Obtain the HTTP status, reading response headers if no status is cached.
        @return Cached HTTP status, or a negative error from response processing.
        A cached status can include 100 Continue; getHeaders() completes response
        processing in that case. Check the actual status your application expects,
        rather than treating every positive result as success.
       */
      int getStatus();

      /** Query the stored client error without performing I/O.
        @return Zero when no error is stored, or a BaErrorCodes.h error value.
        Check each operation's own return value first: not every returned error
        is stored here, and close() does not clear this field.
       */
      int getError();

   /** Access the current SharkSSL connection.
        @return Borrowed connection pointer, or NULL when unavailable. Do not free
        it or keep it after the HTTP connection is closed or replaced.
       */
   SharkSslCon* getSharkSslCon();

      /** Access the embedded transport object.
        @return Non-NULL borrowed pointer with this client's lifetime. This does
        not indicate that a socket is connected. Do not destroy it independently.
       */
      SoDispCon* getSoDispCon();
#else
{
      SoDispCon super;
#endif
      DynBuffer db;
      HttpClientHeader headers[HTTP_CLIENT_MAX_HEADERS];

      struct SharkSsl* sharkSslClient; /* optional SSL client */

      /* Variable proxyPortNo and the following 3 variables must be
       * set and managed by the user of this class.
       */
      /** Borrowed proxy host string, or NULL for a direct connection. Set before connecting; retain while configured. */
      const char* proxy; /* Proxy name/IP addr, if any */
      /** Borrowed proxy credentials "user:password", or NULL. Set before connecting; retain while configured. */
      const char* proxyUserPass; /* Format:  "user:password" */
      /** Borrowed local interface name/address, or NULL for the default binding. Set before connecting; retain while configured. */
      const char* intfName; /* If 0: bind to any intf, or bind to intfName */

      char* data; /* Pointer to start of payload data */
      char* host; /* Server host name */
      BaFileSize size; /* Send or receive data size */
      BaTime readTmo; /* Default read timeout is 20 seconds */
      int chunkSize;
      int lastError;
      int portNo; /* host port number */
      /** Proxy TCP port, 1 through 65535 when proxy is configured. */
      U16 proxyPortNo; /* host port number */
      U16 headerLen;
      S16 httpStatus;
      BaBool chunkEncoding;
      BaBool respManaged; /* Func HttpClient_manageEndOfRequest */
      BaBool closeCon;
      BaBool acceptTrusted; /* Accept only trusted connections */
      /*
        HttpClient_SocksProxy
        HttpClient_Persistent
        HttpClient_IPv6
       */
      U8 mode;
      U8 methodType;
} HttpClient;

#ifdef __cplusplus
extern "C" {
#endif

/** @copydoc HttpClient::HttpClient
    @param o Required initialized client storage.
 */
void HttpClient_constructor(HttpClient* o, SoDisp* disp, U8 mode);
/** @copydoc HttpClient::setSSL
    @param o Required initialized client.
 */
#define HttpClient_setSSL(o, ssl) (o)->sharkSslClient=ssl
/** @copydoc HttpClient::setReadTmo
    @param o Required initialized client.
 */
#define HttpClient_setReadTmo(o, timeout) (o)->readTmo=timeout
#ifndef NO_SHARKSSL
/** @copydoc HttpClient::getSharkSslCon
    @param o Required initialized client.
 */
SharkSslCon* HttpClient_getSharkSslCon(HttpClient* o);
#endif

/** @copydoc HttpClient::~HttpClient
    @param o Required initialized client.
 */
void HttpClient_destructor(HttpClient* o);
/** @copydoc HttpClient::isURL
 */
int HttpClient_isURL(const char* url);
/** C form of HttpClient::request, with the same return values and lifecycle.
    @param o Required initialized client.
    @param methodType Supported HttpMethod value; see HttpClient::request.
    @param url Required absolute URL string.
    @param userPass Optional "user:password" string, or NULL.
    @param params Optional decoded query array terminated by a NULL key.
    @param headers Optional custom header array terminated by a NULL key.
    @param size Request-body byte count; zero selects chunked output for body methods.
    @return Zero when headers are sent, or a negative error; see HttpClient::request.
 */
int HttpClient_request(HttpClient* o,
                       HttpMethod methodType,
                       const char* url,
                       const char* userPass,
                       const HttpClientKeyVal* params,
                       const HttpClientKeyVal* headers,
                       BaFileSize size);

/** @copydoc HttpClient::sendData
    @param o Required initialized client.
 */
int HttpClient_sendData(HttpClient* o, const void* data, int len);
/** @copydoc HttpClient::getBufSize
    @param o Required initialized client.
 */
int HttpClient_getBufSize(HttpClient* o);
int HttpClient_readDataBuffered(HttpClient* o, void* buf, int bufSize);
/** @copydoc HttpClient::readData
    @param o Required initialized client.
 */
int HttpClient_readData(HttpClient* o, void* buf, int bufSize);
/** @copydoc HttpClient::getHeaderValue
    @param o Required initialized client.
 */
const char* HttpClient_getHeaderValue(HttpClient* o, const char* name);
/** @copydoc HttpClient::getHeaders
    @param o Required initialized client.
 */
HttpClientHeader* HttpClient_getHeaders(HttpClient* o, int* hlen);
/** @copydoc HttpClient::close
    @param o Required initialized client.
 */
void HttpClient_close(HttpClient* o);
/** @copydoc HttpClient::getStatus
    @param o Required initialized client.
 */
int HttpClient_getStatus(HttpClient* o);
/** @copydoc HttpClient::getError
    @param o Required initialized client.
 */
#define HttpClient_getError(o) (o)->lastError
/** @copydoc HttpClient::getSoDispCon
    @param o Required initialized client.
 */
#define HttpClient_getSoDispCon(o) ((SoDispCon*)(o))
#ifndef NO_SHARKSSL
/** @copydoc HttpClient::trusted
    @param o Required initialized client.
 */
SharkSslConTrust HttpClient_trusted(HttpClient* o);
#endif
/** C form of HttpClient::setAcceptTrusted.
    @param o Required initialized client.
    @param t TRUE to require trusted new connections, FALSE to disable the check.
 */
#define HttpClient_setAcceptTrusted(o, t) (o)->acceptTrusted=t

#ifdef __cplusplus
}

inline HttpClient::HttpClient(SoDisp* disp, U8 mode) {
   HttpClient_constructor(this,disp, mode);
}

inline HttpClient::~HttpClient() {
   HttpClient_destructor(this);
}

inline void HttpClient::setSSL(SharkSsl* ssl) {
   HttpClient_setSSL(this,ssl);
}

inline void HttpClient::setReadTmo(BaTime timeout) {
   HttpClient_setReadTmo(this,timeout);
}

inline int HttpClient::isURL(const char* url) {
   return HttpClient_isURL(url);
}


inline SharkSslConTrust HttpClient::trusted(void){
   return HttpClient_trusted(this);
}

inline void HttpClient::setAcceptTrusted(bool t) {
   HttpClient_setAcceptTrusted(this, t?TRUE:FALSE);
}


inline SharkSslCon* HttpClient::getSharkSslCon() {
   return HttpClient_getSharkSslCon(this);
}


inline int HttpClient::request(HttpMethod methodType,
                               const char* url,
                               const char* userPass,
                               const HttpClientKeyVal* params,
                               const HttpClientKeyVal* headers,
                               BaFileSize size)
{
   return HttpClient_request(this,methodType,url,userPass,params,headers,size);
}

inline int HttpClient::sendData(const void* data, int len) {
   return HttpClient_sendData(this, data, len);
}

inline int HttpClient::getBufSize() {
   return HttpClient_getBufSize(this);
}

inline int HttpClient::readData(void* buf, int bufSize) {
   return HttpClient_readData(this, buf, bufSize);
}

inline const char* HttpClient::getHeaderValue(const char* name) {
   return HttpClient_getHeaderValue(this, name);
}

inline HttpClientHeader* HttpClient::getHeaders(int* hlen) {
   return HttpClient_getHeaders(this, hlen);
}

inline void HttpClient::close() {
   HttpClient_close(this);
}

inline int HttpClient::getStatus() {
   return HttpClient_getStatus(this);
}


inline int HttpClient::getError() {
   return HttpClient_getError(this);
}

inline SoDispCon* HttpClient::getSoDispCon() {
   return HttpClient_getSoDispCon(this);
}

inline const char* HttpClientHeader::getKey(HttpClient* c) {
   return c->db.buf + key;
}
inline const char* HttpClientHeader::getVal(HttpClient* c) {
   return c->db.buf + val;
}
#endif

/** @} */ /* End group HttpClient */


#endif
