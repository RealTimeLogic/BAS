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
 *   $Id: HttpClient.h 4914 2021-12-01 18:24:30Z wini $
 *
 *   COPYRIGHT:  Real Time Logic LLC, 2009-2020
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
#ifndef __HttpClient_h
#define __HttpClient_h

#include <DynBuffer.h>
#include <SharkSslEx.h>

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

/** A container for key/value pairs that is used when setting custom
    HTTP headers and/or when setting URL encoded HTTP parameters.

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
      const char* key;
      const char* val;
} HttpClientKeyVal;



/** The response HTTP headers returned by HttpClient::getHeaders.
 */
typedef struct
{
#ifdef __cplusplus
      /** Returns the key.
       */
      const char* getKey(HttpClient* c);
      /** Returns the value.
       */
      const char* getVal(HttpClient* c);
#endif
      U16 key;
      U16 val;
} HttpClientHeader;

#define HttpClientHeader_key(c,o) ((c)->db.super.buf + (o)->key)
#define HttpClientHeader_val(c,o) ((c)->db.super.buf + (o)->val)


#define HTTP_CLIENT_MAX_HEADERS 32

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


/** The HTTP(S) "C" client library implementation conforms to the
    HTTP/1.1 standard, RFC 2616.

    The following example shows how to create a function that loads
    the response from a server that either returns static content with
    a known content length or from a server that returns dynamically
    created data using chunk encoding.

    A server can send a content length or chunk encoding for
    dynamically generated data. We can trick the server into sending a
    content-length even for dynamically generated data by sending a
    head request. Knowing the content length makes it easier to
    allocate a storage buffer.

\code
char* loadURL(HttpClient* http, const char* url, int* contentLen)
{
   *contentLen = 0;
   if( ! http->request(HttpMethod_Head, url) )
   {
      long int len;
      const char* cl = http->getHeaderValue("Content-Length");
      if(cl && http->getStatus() == 200)
      {
         len = strtol(cl, NULL, 10);
         if(len > 0 && len < 30*1024 && //We do not want to alloc more than 30K
            ! http->request(HttpMethod_Get, url) && http->getStatus())
         {
            *contentLen = len;
            char* buf = (char*)malloc(len+1); // +1 for strings (REF-S) 
            if(buf)
            {
               char* ptr=buf;
               while(len)
               {
                  int chunkLen = http->readData(ptr, len);
                  if(chunkLen <= 0)
                     break; //Failed: -1 on err. 0 not accepted
                  len -= chunkLen;
                  ptr += chunkLen;
                  assert(len >= 0);
               }
               if( ! len ) // if OK 
               {
                   // REF-S: Zero terminate.
                   // Can be used by caller if buf is a string. 
                  *ptr=0;
                  return buf;
               }
            }
            free(buf);
         }
      }
   }
   return NULL; // failed 
}
\endcode
*/
typedef struct HttpClient
#ifdef __cplusplus
: public SoDispCon
{
      /** Create a HttpClient instance.

        The following example shows how to use one (non threaded)
        instance of the HttpClient library. We set the mutex in SoDisp
        to NULL since we do not use threading. You must set a mutex
        and lock this mutex prior to calling any HttpClient methods if
        you use the HttpClient library simultaneously in multiple
        threads.
        
        \code
          SoDisp disp(NULL);
          HttpClient http(&disp);
        \endcode

        \param disp the Barracuda platform's socket
        dispatcher is the interface between Barracuda code and the
        native TCP/IP stack.

        \param mode can be a combination of the following:
         HttpClient_Persistent, HttpClient_SocksProxy, HttpClient_IPv6

         HttpClient_Persistent: Use a persistent HTTP 1.1 connection

         HttpClient_IPv6: Force the use of hostname to IPv6 address
         translation if applicable to the TCP/IP stack.

         HttpClient_SocksProxy: Use socks proxy, not HTTPS proxy. 

         Proxy support is enabled by manually setting the following attributes:
         \li proxy: Proxy name/IP addr, if any
         \li proxyUserPass: Format:  "user:password"
       */
      HttpClient(SoDisp* disp, U8 mode=HttpClient_Persistent);

      /** Terminate the HttpClient */
      ~HttpClient();

      /** Set the SharkSSL client object to enable https: URL's.

        Example:
        \code
        //See the SharkSSL documentation for details on the SharkSSL API's.
        //The SharkSSL documentation is provided as a PDF file.
        SharkSsl* shark = allocate object or use a static object;
        SharkSsl_constructor(SharkSsl_Client,0,2,3072,3072,0);
        http->setSSL(shark);
        \endcode
       */
      void setSSL(SharkSsl* ssl);

      /** Set the read timeout (in milliseconds). The default value is
          20 secs.
       */
      void setReadTmo(BaTime timeout);

      /** Returns true if the URL is valid */
      static int isURL(const char* url);

      /** Returns the peer's "trust" status.
         See SharkSslConTrust in the
        [SharkSSL documentation](../../shark/group__SharkSslInfoAndCodes.html)
         for more information.
      */
      SharkSslConTrust trusted(void);

      /** Force method HttpClient::request to accept only trusted
          connections when connecting to a server. Setting this to
          boolean true makes HttpClient::request internally call
          HttpClient::trusted when connecting to a server and before
          sending any data to the server. The HttpClient::request will
          return the error code E_NOT_TRUSTED if HttpClient::trusted
          returns a value other than 1.
       */
      void setAcceptTrusted(bool acceptTrusted);

      /** 

        \param methodType is one of:
           - HttpMethod_Delete
           - HttpMethod_Get
           - HttpMethod_Head
           - HttpMethod_Patch
           - HttpMethod_Post
           - HttpMethod_Put

        \param url must be a valid URL such as https://realtimelogic.com/

        \param userPass use basic authentication if not NULL. Format:
        "user:password"

        \param query optional HTTP parameters. A table with key value
        pairs. Includes url encoded data in the query component of the
        request URL.

        \param headers optional (custom) HTTP headers.

        \param size is an optional length when sending data to a server
        using POST or PUT. The client sends data using chunked transfer
        encoding if no size is specified.

        \return zero on succsss.
       */
      int request(HttpMethod methodType,
                  const char* url,
                  const char* userPass=0,
                  const HttpClientKeyVal* query=0,
                  const HttpClientKeyVal* headers=0,
                  BaFileSize size=0);

      /** Send data to the server. The data is sent using chunk
          encoding if param size of method request was set to zero.

          \param data a chunk of data or the complete data you are
          sending to the server.

          \param len is the data length.

          \return 0 on success and a negative value on socket errors.
       */
      int sendData(const void* data, int len);

      int getBufSize();

      /** Read HTTP response data.
         \return the size read, 0 when no more data, and a negative
         value on socket errors.
       */
      int readData(void* buf, int bufSize);

      /** Returns the value for the header 'name' or NULL if the
          header is not in the response.
          \param name the header key.
       */
      const char* getHeaderValue(const char* name);

      /** Returns all HTTP response headers.
         Example:
         \code
         HttpClientHeader* header;
         int hlen;
         for(header=http.getHeaders(&hlen); hlen > 0 ; header++,hlen--)
         {
            printf("%s: %s\n",header->getKey(&http), header->getVal(&http));
         }
         \endcode
       */
      HttpClientHeader* getHeaders(int* hlen);

      /** Close a persisten HTTP 1.1 connection.
       */
      void close();

      /** Returns the server's HTTP response code or a negative value
          on socket errors.
       */
      int getStatus();

      /** Returns the last socket error code if any. The function is
          typically called if the response fails. The error codes are
          documented in BaErrorCodes.h.
       */
      int getError();

   /** Wrapper for SoDispCon:getSharkSslCon
    */
   SharkSslCon* getSharkSslCon();

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
      const char* proxy; /* Proxy name/IP addr, if any */
      const char* proxyUserPass; /* Format:  "user:password" */
      const char* intfName; /* If 0: bind to any intf, or bind to intfName */

      char* data; /* Pointer to start of payload data */
      char* host; /* Server host name */
      BaFileSize size; /* Send or receive data size */
      BaTime readTmo; /* Default read timeout is 20 seconds */
      int chunkSize;
      int lastError;
      int portNo; /* host port number */
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

void HttpClient_constructor(HttpClient* o, SoDisp* disp, U8 mode);
#define HttpClient_setSSL(o, ssl) (o)->sharkSslClient=ssl
#define HttpClient_setReadTmo(o, timeout) (o)->readTmo=timeout
SharkSslCon* HttpClient_getSharkSslCon(HttpClient* o);

void HttpClient_destructor(HttpClient* o);
int HttpClient_isURL(const char* url);
int HttpClient_request(HttpClient* o,
                       HttpMethod methodType,
                       const char* url,
                       const char* userPass,
                       const HttpClientKeyVal* params,
                       const HttpClientKeyVal* headers,
                       BaFileSize size);

int HttpClient_sendData(HttpClient* o, const void* data, int len);
int HttpClient_getBufSize(HttpClient* o);
int HttpClient_readData(HttpClient* o, void* buf, int bufSize);
const char* HttpClient_getHeaderValue(HttpClient* o, const char* name);
HttpClientHeader* HttpClient_getHeaders(HttpClient* o, int* hlen);
void HttpClient_close(HttpClient* o);
int HttpClient_getStatus(HttpClient* o);
#define HttpClient_getError(o) (o)->lastError
#define HttpClient_getSoDispCon(o) ((SoDispCon*)(o))
SharkSslConTrust HttpClient_trusted(HttpClient* o);
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
