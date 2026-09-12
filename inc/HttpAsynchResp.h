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
 *   $Id: HttpAsynchResp.h 5978 2026-09-11 16:13:48Z wini $
 *
 *   COPYRIGHT:  Real Time Logic LLC, 2004-2022
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

/** @file HttpAsynchResp.h */

#ifndef __HttpAsynchResp_h
#define __HttpAsynchResp_h

#include <HttpServer.h>


#define RESPONSESTATE_IDLE 0
#define RESPONSESTATE_WRITEMODE 1
#define RESPONSESTATE_WRITEMODE_CHUNK 2
#define RESPONSESTATE_BODYSENT 3
#define RESPONSESTATE_CLOSED 4



/** @addtogroup HttpStack
    @{
 */


/** Send data asynchronously to a client.

Please see the
<a href="../../misc/HttpStack.html#HttpAsynchResp">
HTTP protocol stack</a> for an introduction to the HttpAsynchResp class.

You must lock the dispatcher prior to calling any methods in the
HttpAsynchResp class unless the object is used from within a Barracuda
event callback or you have called method HttpAsynchResp::asynchThreadMode.  See
<a href ="../../introduction.html#threads">
using multiple threads</a> in the Barracuda introduction for more
information on protecting the Barracuda code.

There is one exception to the above. You can call method
HttpAsynchResp::getMutex without locking the dispatcher
first. You cannot lock the dispatcher unless you first have a
reference to the dispatcher object.

example code
\code
while(sendData)
{
   ThreadLock lock(myAsynchResp->getMutex()); // Keep the lock until scope exit.
   BufPrint* writer = myAsynchResp->getWriter();
   if(!writer || writer->printf("Hi client") < 0)
      break; // Stop producing output after a failure.
} 
\endcode

Certain restrictions apply to how you can use this object. This is,
after all, a lightweight version of HttpResponse. The HttpAsynchResp
object does not have large buffers for storing out data. Thus, the
methods must be called in the following order:
 

 - HttpAsynchResp::setConClose
 - HttpAsynchResp::setStatus
 - HttpAsynchResp::setHeader
 - BufPrint | ( HttpAsynchResp::sendData [ & HttpAsynchResp::sendNextChunk ] )

Calling methods setConClose, setStatus, and setHeader are optional.

Calling method setHeader implicitly calls method setStatus with status
code 200 unless you already called setStatus.

Sending data using method sendData or the methods in BufPrint flushes
the HTTP header. Thus, you cannot call method setHeader after the HTTP
header is flushed.

You must use one of BufPrint or ( HttpAsynchResp::sendData [ &
HttpAsynchResp::sendNextChunk ] ) when sending response data.

*/
typedef struct HttpAsynchResp
#ifdef __cplusplus
: public HttpConnection
{
      void *operator new(size_t s) { return ::baMalloc(s); }
      void operator delete(void* d) { if(d) ::baFree(d); }
      void *operator new(size_t, void *place) { return place; }
      void operator delete(void*, void *) { }

      /** Uninitialized storage; initialize before use or destruction. */
      HttpAsynchResp() {}

      /** Initiate a HttpAsynchResp from a HttpRequest object.
          \param buf Required borrowed writable output buffer, valid until close.
          It is not freed by the response.
          \param size Output capacity in bytes. Use at least 256 (the internal
          minimum check is 200). For chunked writer output, usable payload
          capacity (size minus 8) must not exceed 65535.
          \param req Required current request with an uncommitted response. Its
          active connection is moved to this object; do not use the original
          response afterward. Check isValid after construction.
      */
      HttpAsynchResp(char* buf, int size, HttpRequest* req);

      /** Initiate a HttpAsynchResp from a HttpConnection object.
          \param buf Required borrowed writable output buffer, valid until close.
          It is not freed by the response.
          \param size Output capacity in bytes. Use at least 256 (the internal
          minimum check is 200). For chunked writer output, usable payload
          capacity (size minus 8) must not exceed 65535.
          \param con Source connection whose active socket is moved to this
          object. NULL creates no active socket. Check isValid before output.
          For HttpAsynchReq, obtain the source with its getCon handoff method.
      */
      HttpAsynchResp(char* buf,int size,HttpConnection* con);

      /** Check the installed buffer and connection.
 * @return True if storage is installed and the connection currently reports
 * valid, false otherwise. This cannot guarantee that a later send succeeds. */

      bool isValid();

      /** Set in asynchronous thread mode so you can call the methods in
      this class using its worker-thread send path. This does not serialize
      multiple application threads accessing the same response; use one owner. You must
      not call this method if you are calling the methods in this
      class from within a callback originating from the socket
      dispatcher SoDisp -- i.e., if the mutex is already locked.
      */
      void asynchThreadMode();

      /** Calls method close.
      */
      ~HttpAsynchResp();

      /** Flushes the response, if any, and checks the connection object:
      A valid and persistent HTTP 1.1 socket connection object is moved
      back into the Web-server's HTTP 1.1 HttpConnection pool such
      that the connection can be recycled.  A non-persistent HTTP
      connection such as an HTTP 1.0 connection is terminated.
      Completes the final chunk for writer mode. This void operation does not
      report flush/close failure; check preceding output results. It does not
      free borrowed storage. No further output is allowed after close.
      */
      void close();

      /** Get the dispatcher lock. This is the only method in this
          class that can be called without locking the dispatcher.
          @return Borrowed dispatcher mutex; NULL when the server has no mutex.
          Requires a properly initialized connection/dispatcher association.
       */
      ThreadMutex* getMutex();

      /** Close connection when the HttpAsynchResp object is done.
      A persistent HTTP 1.1 connection is recycled and sent back to
      the connection pool in the web-server when the HttpAsynchResp
      object is done. Calling this method makes sure the connection
      closes when the HttpAsynchResp object is done.
      @return Zero (the assigned keep-alive flag), not a delivery status. You should call
      this method if you stream data such as audio.
       */
      int setConClose();

      /** Request connection closure.
 * @return Zero, as for setConClose.
 * @warning The current C++ wrapper calls setConClose and does not enable the
 * lingering-close queue. When unread incoming data requires a lingering close,
 * use the C macro HttpAsynchResp_setLingeringClose(thisResponse) before headers
 * are sent. The macro and this wrapper currently have different behavior. */

      int setLingeringClose();


      /** Append the HTTP status line and standard Date/Server headers.
 * @param[in] statusCode Code recognized by HttpServer_getStatusCode; unknown
 * codes produce its fallback text rather than a valid numeric status line.
 * @param[in] protocol Borrowed NUL-terminated HTTP version for this call,
 * normally "1.1" (also selected by NULL); do not include "HTTP/" or line breaks.
 * @return Zero on success; -200 missing buffer, -100 status already selected,
 * -11 formatting/output failure. Partial headers may already have been sent.
 * Set status before any header or body output. The default is 200. */

      int setStatus(int statusCode, const char* protocol=0);

      /** Append a response header before starting the body.
 * @param[in] name Required NUL-terminated HTTP field name, used during this call.
 * @param[in] value Required NUL-terminated field value, used during this call.
 * Neither string may contain CR or LF. Validation is the caller's responsibility.
 * @return Zero on success; -110 after headers have ended, -2 output failure,
 * or a negative setStatus error when emitting the default 200 status.
 * Repeated calls append fields rather than replacing earlier fields. Leave
 * Content-Length, Transfer-Encoding and Connection framing to this response API. */

      int setHeader(const char *name, const char *value);

      /** Begin a fixed-length response body.
 * @param[in] data Borrowed source for this call, or NULL to send headers only.
 * @param[in] pktSize Nonnegative total body length in bytes, fitting int.
 * @param[in] chunkSize Initial byte count, between zero and pktSize; provide
 * this many readable bytes when data is non-NULL.
 * @return Zero on success, negative on header/send failure (-3 length-header
 * formatting, -4 body send, or a propagated header error). If body mode was
 * already selected, the call returns zero without sending anything.
 * Call once, then sendNextChunk until exactly pktSize bytes have been supplied.
 * Total length is the caller's responsibility; the implementation does not
 * track remaining bytes. Do not combine this mode with getWriter. */

      int sendData(const void* data, int pktSize, int chunkSize);

      /** Continue the fixed-length body begun with sendData.
 * @param[in] data Borrowed source containing chunkSize readable bytes.
 * @param[in] chunkSize Nonnegative byte count; cumulative output must not exceed
 * the pktSize declared by sendData. This is not HTTP chunked transfer coding.
 * @return -1 if fixed-length mode has not started; otherwise the connection's
 * send status (zero success, negative failure). No partial count is provided. */

      int sendNextChunk(const void* data,int chunkSize);

      /** BufPrint is used when sending data of unknown length. For a
          HTTP/1.1. connection, the data is sent in 
   <a href="http://www.w3.org/Protocols/rfc2616/rfc2616-sec3.html#sec3.6.1">
          chunk transfer mode.</a>

          This method returns NULL if HttpAsynchResp::sendData was called
          prior to this method.
          @return Borrowed embedded writer, or NULL if headers cannot be sent,
          fixed-length mode was selected, or the response is closed.
          Its storage is not separately owned. Check every writer operation.
          Obtaining the writer sends headers and selects body mode; it is not
          a read-only accessor. close finishes the response.
       */
      BufPrint* getWriter();
#else
{
   HttpConnection super;
#endif
   BufPrint bufPrint;
   HttpConnection* con; /* pointer to 'super' or to a HttpAsyncReqResp */
   ThreadMutex* mutex; /* Can be set so code automatically locks mutex */
   BaBool statusSent;
   BaBool headerSent;
   U8 responseState;
   U8 doLingeringClose;
} HttpAsynchResp;


#ifdef __cplusplus
extern "C" {
#endif
/** Initialize from a request and take over its connection.
 * @param[out] o Caller-owned response storage.
 * @param[in] buf Required borrowed writable buffer, valid until close.
 * @param[in] size Capacity in bytes; use at least 256. See the C++ constructor
 * for the chunked-writer capacity limit.
 * @param[in,out] req Current request with an uncommitted response.
 * Check HttpAsynchResp_isValid after construction. */
BA_API void HttpAsynchResp_constructor(
   HttpAsynchResp* o, char* buf, int size, HttpRequest* req);
/** Initialize from a connection and take over its socket.
 * @param[out] o Caller-owned response storage.
 * @param[in] buf Required borrowed writable buffer, valid until close.
 * @param[in] size Capacity in bytes; use at least 256. See the C++ constructor
 * for the chunked-writer capacity limit.
 * @param[in,out] con Source connection, or NULL for no active socket.
 * Check HttpAsynchResp_isValid after construction. */
BA_API void HttpAsynchResp_constructor2(
   HttpAsynchResp* o, char* buf, int size, HttpConnection* con);
/** Initialize the response half of a combined request/response object.
 * Prefer HttpAsynchReqResp_start to configure both halves together.
 * @param[out] o Caller-owned response storage.
 * @param[in] buf Required borrowed writable buffer, valid until close.
 * @param[in] size Capacity in bytes; use at least 256 and respect the
 * chunked-writer limit documented by HttpAsynchResp.
 * @param[in,out] con Required borrowed receive connection, valid until close;
 * this initializer shares it instead of moving its socket. */
BA_API void HttpAsynchResp_ReqRespInit(
   HttpAsynchResp* o, char* buf, int size, HttpConnection* con);
/** @copydoc HttpAsynchResp::isValid
 * @param[in,out] o Initialized response. */
BA_API BaBool HttpAsynchResp_isValid(HttpAsynchResp* o);
/** Select worker-thread send behavior; see HttpAsynchResp::asynchThreadMode.
 * @param[in,out] o Initialized response, used by one application thread. */
#define HttpAsynchResp_asynchThreadMode(o) \
   ((o)->mutex=HttpAsynchResp_getMutex(o))
/** @copydoc HttpAsynchResp::close
 * @param[in,out] o Initialized response. */
BA_API void HttpAsynchResp_close(HttpAsynchResp* o);
/** Close the response; no status is returned.
 * @param[in,out] o Initialized response. Does not free o or borrowed storage.
 * @sa HttpAsynchResp::close */
#define HttpAsynchResp_destructor(o) HttpAsynchResp_close(o)
/** @param[in] o Response with an initialized dispatcher association.
 * @return Borrowed dispatcher mutex, possibly NULL; does not acquire it. */
#define HttpAsynchResp_getMutex(o) \
   SoDisp_getMutex(HttpConnection_getDispatcher((HttpConnection*)(o)))
/** Disable persistent connection reuse before sending headers.
 * @param[in,out] o Initialized standalone response.
 * @return Zero (assigned flag), not an I/O status. */
#define HttpAsynchResp_setConClose(o)\
   HttpConnection_clearKeepAlive((HttpConnection*)(o))
/** Enable the lingering-close queue before sending headers.
 * Use when terminating an upload that still has unread incoming bytes.
 * @param[in,out] o Initialized response.
 * @return TRUE (assigned flag), not an I/O status. Unlike the current C++
 * wrapper, this macro sets the lingering-close flag. */
#define HttpAsynchResp_setLingeringClose(o) (o)->doLingeringClose=TRUE
/** @copydoc HttpAsynchResp::setStatus
 * @param[in,out] o Initialized response. */
BA_API int HttpAsynchResp_setStatus(
   HttpAsynchResp* o, int statusCode, const char* protocol);
/** @copydoc HttpAsynchResp::setHeader
 * @param[in,out] o Initialized response. */
BA_API int HttpAsynchResp_setHeader(
   HttpAsynchResp* o,const char *name,const char *value);
/** @copydoc HttpAsynchResp::sendData
 * @param[in,out] o Initialized response. */
BA_API int HttpAsynchResp_sendData(
   HttpAsynchResp* o, const void* data, int pktSize, int chunkSize);
/** @copydoc HttpAsynchResp::sendNextChunk
 * @param[in,out] o Initialized response. */
BA_API int HttpAsynchResp_sendNextChunk(
   HttpAsynchResp* o,const void* data,int chunkSize);
/** @copydoc HttpAsynchResp::getWriter
 * @param[in,out] o Initialized response. */
BA_API BufPrint* HttpAsynchResp_getWriter(HttpAsynchResp* o);
/** @param[in] o Initialized response.
 * @return TRUE when a worker-thread mutex pointer has been installed,
 * FALSE otherwise (including a server with no mutex). */
#define HttpAsynchResp_isAsynchThreadMode(o) ((o)->mutex ? TRUE : FALSE)
#ifdef __cplusplus
}
inline HttpAsynchResp::HttpAsynchResp(char* buf, int size, HttpRequest* req) {
    HttpAsynchResp_constructor(this, buf, size, req);
}
inline HttpAsynchResp::HttpAsynchResp(char* buf,int size,HttpConnection* con) {
    HttpAsynchResp_constructor2(this, buf, size, con);
}
inline bool HttpAsynchResp::isValid() {
      return HttpAsynchResp_isValid(this) ? TRUE : FALSE;
}
inline HttpAsynchResp::~HttpAsynchResp() {
   HttpAsynchResp_destructor(this);
}
inline void HttpAsynchResp::close() {
   HttpAsynchResp_close(this);
}
inline void HttpAsynchResp::asynchThreadMode() {
   HttpAsynchResp_asynchThreadMode(this);
}
inline ThreadMutex* HttpAsynchResp::getMutex() {
   return HttpAsynchResp_getMutex(this);
}
inline int HttpAsynchResp::setConClose() {
   return HttpAsynchResp_setConClose(this); 
}
inline int HttpAsynchResp::setLingeringClose() {
   return HttpAsynchResp_setConClose(this); 
}
inline int HttpAsynchResp::setStatus(int statusCode, const char* protocol) {
   return HttpAsynchResp_setStatus(this, statusCode, protocol); 
}
inline int HttpAsynchResp::setHeader(const char *name,const char *value) {
   return HttpAsynchResp_setHeader(this, name, value);
}
inline int HttpAsynchResp::sendData(
   const void* data, int pktSize, int chunkSize) {
   return HttpAsynchResp_sendData(this, data, pktSize, chunkSize);
}
inline int HttpAsynchResp::sendNextChunk(const void* data,int chunkSize) {
   return HttpAsynchResp_sendNextChunk(this,data,chunkSize); }
inline BufPrint* HttpAsynchResp::getWriter() {
   return HttpAsynchResp_getWriter(this);
}
#endif

/** @} */ /* end of HttpStack */
  

#endif
