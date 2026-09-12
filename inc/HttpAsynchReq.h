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
 *   $Id: HttpAsynchReq.h 5978 2026-09-11 16:13:48Z wini $
 *
 *   COPYRIGHT:  Real Time Logic LLC, 2004 - 2012
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

/** @file HttpAsynchReq.h */

#ifndef __HttpAsynchReq_h
#define __HttpAsynchReq_h

#include <HttpAsynchResp.h>

#ifndef __DOXYGEN__
struct HttpAsynchReq;
#endif

/** @addtogroup HttpStack
    @{
 */

/** Receive data, completion, or an error.
 * @param[in,out] super Borrowed receiver; upcast to the application's containing
 * type when appropriate. Calls can occur within start or later in the dispatcher.
 * @param[in] data Borrowed bytes, valid only during the callback. Copy retained
 * data. NULL with size zero means completion. Non-NULL with size zero is the
 * preliminary Expect notification before the library sends 100 Continue.
 * @param[in] size Positive byte count, zero as described above, or a negative
 * error with NULL data: -1 receive failure, -10 malformed chunk line ending,
 * -11 chunk header does not fit the receive buffer.
 * Fixed-length bodies are delivered when the buffer fills or the body ends;
 * chunked bodies can produce smaller callbacks. A completion notification is
 * separate from the final positive-length data callback.
 * On completion or error, destroy the receiver or use getCon to transfer its
 * connection to response handling. Do not merely retain an idle receiver:
 * some chunked paths require that the callback terminates receive processing.
 * Destruction from the callback is supported; do not access it afterward. */

typedef void (*HttpAsynchReq_OnData)(struct HttpAsynchReq* super,
                                      void* data, S32 size);

/** Asynchronously receive data in a resource such as HttpDir or
    HttpPage.  
    An instance of the HttpAsynchReq class can be used as a method to
    receive any kind of data from a client as long as the client
    does not send the data as a multipart/form-data. A resource
    receiving multipart/form-data must use the MultipartUpload class.

    You typically create an instance of this class in a resource when
    receiving a request. You can optionally keep instances of this
    class in a pool. The object is started by calling method start. 

    HttpAsynchReq is an abstract base class; thus, you must inherit and
    implement the socket event method. See constructor for
    how to install the event function.

    The detached socket object is installed directly into the
    SoDisp object, and any socket event is directed to this
    object. The callback installed by the constructor receives body data and
    terminal notifications; see HttpAsynchReq_OnData.

    \sa HttpRecData
*/
typedef struct HttpAsynchReq
{
#ifdef __cplusplus
      void *operator new(size_t s) { return ::baMalloc(s); }
      void operator delete(void* d) { if(d) ::baFree(d); }
      void *operator new(size_t, void *place) { return place; }
      void operator delete(void*, void *) { }

      /** Uninitialized storage; initialize before use or destruction. */
      HttpAsynchReq(){}

      /** Create an instance of HttpAsynchReq.
          \param server Required borrowed server, valid throughout receiver use.
          \param data Required callback, valid throughout receiver use. HttpAsynchReq is an abstract class and you must,
          therefore, implement the OnData socket event method.
      */
      HttpAsynchReq(HttpServer* server, HttpAsynchReq_OnData data);

      /** Stop dispatcher reception and destroy the owned connection. Does not
       * free this object or the borrowed buffer. May be called from OnData;
       * after a connection handoff, it does not close the moved connection. */
      ~HttpAsynchReq();

      /** Inspect receive progress after successful start.
 * @return For fixed-length bodies, remaining bytes not yet received, including
 * zero at completion. For chunked bodies this is internal parsing state, not
 * the remaining body size; use isChunkEncoding and completion callbacks instead.
 * Before start or after failure, do not interpret it as progress. */

      SBaFileSize getPacketSizeLeft();

      /** Returns a pointer to the internal buffer where the received
          data is stored. The buffer is set in method
          HttpAsynchReq::start.
          @return Borrowed buffer passed to start, or NULL before start/after stop.
          It is not NUL-terminated; use the callback byte count.
       */
      void* getBuffer();

      /** @return Installed receive capacity in bytes, zero before start/after stop. */
      SBaFileSize getBufferSize();


      /** @return True if the active request is received as chunked data. */
      bool isChunkEncoding();

      /** Inspect the request's body framing without consuming it.
 * @param[in] req Required borrowed current request.
 * @return Content-Length (zero if absent or zero); -1 if a Transfer-Encoding
 * header is present; -2 for multipart/form-data; -3 for URL-encoded form data
 * already handled by the server. Transfer-Encoding takes precedence in this
 * check. The caller must ensure it denotes chunked encoding.
 * Use MultipartUpload for multipart bodies. Even if the receive buffer fits
 * the complete body, expect a separate completion callback after its data. */

      static SBaFileSize calcPacketSize(HttpRequest* req);

      /** Take over the request connection and start asynchronous reception.
 * @param[in,out] req Borrowed current request with an uncommitted response.
 * @param[in] buffer Caller-owned writable storage, valid until reception stops.
 * @param[in] bufferSize Capacity in bytes, at least 255. Chunk header lines must
 * fit this buffer. Chunked trailer fields are not supported.
 * @return Zero when started, including when a callback already completed or
 * destroyed the receiver. Negative on failure: -30 invalid buffer/capacity,
 * -20 already started, -1 multipart input or committed response, -2 form input,
 * invalid connection or failed 100 Continue, E_MALLOC pushback allocation
 * failure, -100 other rejected body sizing. -1 can also follow a completion
 * callback that did not destroy or hand off the receiver.
 * Callbacks may run before return. Arrange ownership accordingly and never
 * dereference an object destroyed by such a callback. On failure, clean up a
 * still-live receiver; the connection may already have been taken over.
 * Do not read the body through another API or continue using the original
 * response after takeover. Initialize again before reusing a stopped object. */

      int start(HttpRequest* req,void* buffer, S32 bufferSize);

      /** Remove active dispatcher reception, mark an active connection terminated,
      and clear receive-buffer state. Does not free borrowed buffer storage.
      Use destruction for final connection cleanup.
      */
      void stop();

      /** Stop dispatcher reception and obtain the connection for handoff.
      @param[in] o Unused compatibility argument in the C++ method; this method
      acts on its own instance. The C function uses o as its receiver.
      @return Borrowed embedded connection pointer, never NULL for a live
      receiver. The connection may be invalid; use its status checks.
      Calling this method tells the active callback path that receive processing
      has ended. It does not itself move or free the connection.

      This method is typically called from within the OnData event
      method when all data is received and you are moving the active
      connection object to an HttpAsynchResp object.

      C code example:
      \code
      HttpAsynchResp_constructor2(
         resp, buf, bufSize, HttpAsynchReq_getCon(req));
      \endcode
      */
      HttpConnection* getCon(HttpAsynchReq* o);
#endif
   HttpConnection super; /* Inherits from HttpConnection */
   SBaFileSize bufferSize;
   SBaFileSize packetSizeLeft;
   SBaFileSize offset;
   HttpAsynchReq_OnData data;
   BaBool* isTerminatedPtr;
   U8* buffer;
   BaBool chunkEncoding;
} HttpAsynchReq;

#ifdef __cplusplus
extern "C" {
#endif
/** @copydoc HttpAsynchReq::HttpAsynchReq(HttpServer*, HttpAsynchReq_OnData)
 * @param[in,out] o Caller-owned receiver instance. */
BA_API void HttpAsynchReq_constructor(HttpAsynchReq* o,
                                HttpServer* server,
                                HttpAsynchReq_OnData data);
/** @param[in,out] o Initialized receiver; closes its owned connection and
 * releases dispatcher registration, but does not free o or the receive buffer.
 * May be called from OnData. See HttpAsynchReq::~HttpAsynchReq. */
BA_API void HttpAsynchReq_destructor(HttpAsynchReq* o);
/** @param[in] o Initialized receiver.
 * @return Remaining fixed-length bytes; chunked values are parsing state. */
#define HttpAsynchReq_getPacketSizeLeft(o) (o)->packetSizeLeft
/** @param[in] o Initialized receiver.
 * @return Borrowed receive buffer, or NULL before start/after stop. */
#define HttpAsynchReq_getBuffer(o) (o)->buffer
/** @param[in] o Initialized receiver.
 * @return Receive capacity in bytes, zero before start/after stop. */
#define HttpAsynchReq_getBufferSize(o) (o)->bufferSize
/** @param[in] o Initialized receiver.
 * @return TRUE for active chunked reception, FALSE otherwise. */
#define HttpAsynchReq_isChunkEncoding(o) (o)->chunkEncoding
/** @param[in] o Initialized receiver.
 * @return Borrowed server supplied at construction. */
#define HttpAsynchReq_getServer(o) \
  HttpConnection_getServer((HttpConnection*)(o))
/** @copydoc HttpAsynchReq::calcPacketSize */
BA_API SBaFileSize HttpAsynchReq_calcPacketSize(HttpRequest* req);
/** @copydoc HttpAsynchReq::start
 * @param[in,out] o Caller-owned receiver instance. */
BA_API int HttpAsynchReq_start(
   HttpAsynchReq* o,HttpRequest* req, void* buffer, S32 bufferSize);
/* Internal function used by HttpFiber */
/** @param[in,out] o Initialized receiver.
 * @copydetails HttpAsynchReq::stop */
BA_API void HttpAsynchReq_stop(HttpAsynchReq* o);
/** Stop receiving and expose the embedded connection for handoff.
 * @param[in,out] o Live receiver.
 * @return Borrowed non-NULL connection pointer; it may hold an invalid socket.
 * See HttpAsynchReq::getCon for callback and transfer semantics. */
BA_API HttpConnection* HttpAsynchReq_getCon(HttpAsynchReq* o);
#ifdef __cplusplus
}
inline HttpAsynchReq::HttpAsynchReq(HttpServer* server,
                               HttpAsynchReq_OnData data) {
   HttpAsynchReq_constructor(this, server, data);
}
inline HttpAsynchReq::~HttpAsynchReq() {
   HttpAsynchReq_destructor(this);
}
inline SBaFileSize HttpAsynchReq::getPacketSizeLeft() {
   return HttpAsynchReq_getPacketSizeLeft(this);
}
inline void* HttpAsynchReq::getBuffer() {
   return HttpAsynchReq_getBuffer(this);
}
inline SBaFileSize HttpAsynchReq::getBufferSize() {
   return HttpAsynchReq_getBufferSize(this);
}
inline bool HttpAsynchReq::isChunkEncoding() {
   return HttpAsynchReq_isChunkEncoding(this) ? true : false;
}
inline SBaFileSize HttpAsynchReq::calcPacketSize(HttpRequest* req) {
   return HttpAsynchReq_calcPacketSize(req);
}
inline int HttpAsynchReq::start(HttpRequest* req,void* buffer, S32 bufferSize){
   return HttpAsynchReq_start(this,req,buffer,bufferSize);
}
inline void HttpAsynchReq::stop() {
   HttpAsynchReq_stop(this);
}

inline HttpConnection* HttpAsynchReq::getCon(HttpAsynchReq* o){
   return HttpAsynchReq_getCon(this); }
#endif



/** Simultaneously receive and send a stream of data.
The HttpAsynchReqResp class makes it possible to asynchronously
receive and send data at the same time. The HttpAsynchReqResp can also
be used as a replacement for using a HttpAsynchReq and HttpAsynchResp
object. An HttpAsynchReqResp instance is easier to use than first
using a HttpAsynchReq object for receiving data and then later using a
HttpAsynchResp for sending a response message.
 */
typedef struct HttpAsynchReqResp
#ifdef __cplusplus
   :public HttpAsynchReq
{
      /** Uninitialized storage; initialize before use or destruction. */
      HttpAsynchReqResp(){}

      /** Create a HttpAsynchReqResp instance.
       \param server Required borrowed server, valid throughout object use.
       \param onData Required asynchronous receive callback
       method. HttpAsynchReqResp inherits from HttpAsynchReq and it is
       the HttpAsynchReq object which calls the callback method when
       the internal buffer is full. End of stream is signaled by
       setting the two parameters to the callback to null.
      */
      HttpAsynchReqResp(HttpServer* server, HttpAsynchReq_OnData onData);

      /** Start the HttpAsynchReqResp object.
      The onData callback method is called directly if all data was
      received by the client; otherwise, the HttpAsynchReq object is
      installed into the SoDisp.  The method returns a
      negative value if the HttpAsynchReq object failed to start. The
      method returns 0 if the object started successfully or if the
      destructor was called from within the onData callback.
      @return The status from HttpAsynchReq::start, with the same synchronous
      callback and cleanup rules.
      \param req Required borrowed current request with an uncommitted response.
      \param recBuf Borrowed writable receive buffer, valid until reception stops. The onData callback is called
      when the buffer is full.
      \param recBufSize Receive capacity in bytes, at least 255.
      \param sendBuf Separate borrowed writable response buffer, valid until close.
      \param sendBufSize Response capacity in bytes; use at least 256.
      The buffers must not overlap while sending and receiving concurrently.
       */
      int start(HttpRequest* req,void* recBuf,S32 recBufSize,
                void* sendBuf,S32 sendBufSize);
      /** Start response if you know that all data was received by the
       web-server. This is a faster method than calling start if you
       know that the web-server received all data in its internal
       buffer before delegating the request to the requested service.
       @param[in,out] req Current request; caller must account for any unread
       body before selecting this response-only path. No receive callback runs.
       @param[in] sendBuf Borrowed writable response storage, valid until close.
       @param[in] sendBufSize Response capacity in bytes; use at least 256.
       @return Zero after initialization, -1 if the response was committed,
       -2 if the request connection is invalid. Zero does not validate the send
       buffer; satisfy its preconditions and check response isValid.
      */
      int startResp(HttpRequest* req,void* sendBuf,S32 sendBufSize);

#if 0
      /** Disable the onData callback. Do not call this method after
          end of stream is signalled.
          \sa enableRecEv
       */
      void disableRecEv();

      /** Enable the disabled onData callback. Do not call this method after
          end of stream is signalled.
          \sa disableRecEv
       */
      void enableRecEv();
#endif

      /** @return Borrowed dispatcher mutex for the active response connection.
       Requires a successfully initialized response; does not acquire the mutex.
       */
      ThreadMutex* getMutex();

      /** @return Borrowed server for the active response connection.
       * Requires a successfully initialized response. */
      HttpServer* getServer();

      /** @return Borrowed embedded response object, not separately owned.
       Use only after start/startResp initializes it; check its operation results.
       */
      HttpAsynchResp* getResponse();
#else
#if 0
}
#endif
{
   HttpAsynchReq super;
#endif
   HttpAsynchResp resp;
}HttpAsynchReqResp;

#ifdef __cplusplus
extern "C" {
#endif
/** Initialize combined receive/response state.
 * @param[out] o Caller-owned object.
 * @param[in] server Required borrowed server, valid throughout use.
 * @param[in] data Required callback; see HttpAsynchReq_OnData. */
BA_API void HttpAsynchReqResp_constructor(HttpAsynchReqResp* o,
                                          HttpServer* server,
                                          HttpAsynchReq_OnData data);
#if 0
void HttpAsynchReqResp_disableRecEv(HttpAsynchReqResp* o);
void HttpAsynchReqResp_enableRecEv(HttpAsynchReqResp* o);
#endif
/** @copydoc HttpAsynchReqResp::start
 * @param[in,out] o Caller-owned receiver instance. */
BA_API int HttpAsynchReqResp_start(HttpAsynchReqResp* o,
                                   HttpRequest* req,
                                   void* recBuf,
                                   S32 recBufSize,
                                   void* sendBuf,
                                   S32 sendBufSize);
/** @copydoc HttpAsynchReqResp::startResp
 * @param[in,out] o Caller-owned receiver instance. */
BA_API int HttpAsynchReqResp_startResp(HttpAsynchReqResp* o,
                                       HttpRequest* req,
                                       void* sendBuf,
                                       S32 sendBufSize);
/** Close the response and destroy receive state; free neither object nor buffers.
 * @param[in,out] o Initialized combined receiver. May be called from OnData. */
#define HttpAsynchReqResp_destructor(o) do {\
   HttpAsynchResp_destructor(&(o)->resp);\
   HttpAsynchReq_destructor((HttpAsynchReq*)(o));\
}while(0)
/** @copydoc HttpAsynchReq_getPacketSizeLeft */
#define HttpAsynchReqResp_getPacketSizeLeft(o)\
   HttpAsynchReq_getPacketSizeLeft((HttpAsynchReq*)o)
/** @copydoc HttpAsynchReq_getBuffer */
#define HttpAsynchReqResp_getBuffer(o)\
   HttpAsynchReq_getBuffer((HttpAsynchReq*)o)
/** @copydoc HttpAsynchReq_getBufferSize */
#define HttpAsynchReqResp_getBufferSize(o)\
   HttpAsynchReq_getBufferSize((HttpAsynchReq*)o)
/** @copydoc HttpAsynchReq_isChunkEncoding */
#define HttpAsynchReqResp_isChunkEncoding(o)\
   HttpAsynchReq_isChunkEncoding((HttpAsynchReq*)o)
/** @copydoc HttpAsynchReq_calcPacketSize */
#define HttpAsynchReqResp_calcPacketSize(req)\
   HttpAsynchReq_calcPacketSize(req)

/** @param[in] o Initialized combined receiver.
 * @return Borrowed response connection, possibly NULL before initialization. */
#define HttpAsynchReqResp_getCon(o) ((o))->resp.con
/** @param[in] o Initialized combined receiver.
 * @return Borrowed dispatcher mutex; requires an initialized response connection. */
#define HttpAsynchReqResp_getMutex(o) \
   SoDisp_getMutex(HttpConnection_getDispatcher( \
     HttpAsynchReqResp_getCon(o)))
/** @param[in] o Initialized combined receiver.
 * @return Borrowed server; requires an initialized response connection. */
#define HttpAsynchReqResp_getServer(o) \
  HttpConnection_getServer(HttpAsynchReqResp_getCon(o))
/** @param[in] o Initialized combined receiver.
 * @return Borrowed embedded response; use only after start/startResp. */
#define HttpAsynchReqResp_getResponse(o) (&(o)->resp)
#ifdef __cplusplus
}
inline HttpAsynchReqResp::HttpAsynchReqResp(HttpServer* server,
                                     HttpAsynchReq_OnData data) {
   HttpAsynchReqResp_constructor(this, server, data);
}
inline int HttpAsynchReqResp::start(HttpRequest* req,
                                    void* recBuf,
                                    S32 recBufSize,
                                    void* sendBuf,
                                    S32 sendBufSize) {
   return HttpAsynchReqResp_start(
      this,req,recBuf,recBufSize,sendBuf,sendBufSize); }
inline int HttpAsynchReqResp::startResp(HttpRequest* req,
                                        void* sendBuf,
                                        S32 sendBufSize) {
   return HttpAsynchReqResp_startResp(this,req,sendBuf,sendBufSize); }
#if 0
inline void HttpAsynchReqResp::disableRecEv() {
   HttpAsynchReqResp_disableRecEv(this); }
inline void HttpAsynchReqResp::enableRecEv() {
   HttpAsynchReqResp_enableRecEv(this); }
#endif
inline ThreadMutex* HttpAsynchReqResp::getMutex() {
   return HttpAsynchReqResp_getMutex(this);
}
inline struct HttpServer* HttpAsynchReqResp::getServer() {
   return HttpAsynchReqResp_getServer(this);
}
inline HttpAsynchResp* HttpAsynchReqResp::getResponse() {
   return HttpAsynchReqResp_getResponse(this);
}
#endif


/** @} */ /* end of HttpStack */
  

#endif
