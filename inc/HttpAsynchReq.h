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
 *   $Id: HttpAsynchReq.h 4915 2021-12-01 18:26:55Z wini $
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

#ifndef __HttpAsynchReq_h
#define __HttpAsynchReq_h

#include <HttpAsynchResp.h>

#ifndef __DOXYGEN__
struct HttpAsynchReq;
#endif

/** @addtogroup HttpStack
    @{
 */

/**  Prototype for the HttpAsynchReq OnRequest callback.
     The callback is called when the internal buffer is full and at
     end of stream. End of stream is signaled by setting data = NULL
     and size=0.

     If a HTTP client sends a 100-continue message, the function is
     initially called with a non NULL data pointer, but with size set
     to zero. The callback should create the resource and manage the
     error if creating the resource fails.

    \param super The object pointer, which you must upcast to your class 
    implementation, i.e. MyAsynchReq* o = (MyAsynchReq*)super;
    \param data pointer to received data.
\param size is the size of the received data.
*/
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
    object. The callback method specified in HttpAsynchReq::start is
    triggered when the buffer is full.

    \sa HttpRecData
*/
typedef struct HttpAsynchReq
{
#ifdef __cplusplus
      void *operator new(size_t s) { return ::baMalloc(s); }
      void operator delete(void* d) { if(d) ::baFree(d); }
      void *operator new(size_t, void *place) { return place; }
      void operator delete(void*, void *) { }

      HttpAsynchReq(){}

      /** Create an instance of HttpAsynchReq.
          \param server is a pointer to the HttpServer object.
          \param data is a pointer to the socket event
          function. HttpAsynchReq is an abstract class and you must,
          therefore, implement the OnData socket event method.
      */
      HttpAsynchReq(HttpServer* server, HttpAsynchReq_OnData data);

      /** Terminate a HttpAsynchReq object. */
      ~HttpAsynchReq();

      /** Returns the remaining packet size if the packet size is
          known -- i.e., if the initial HTTP header had a Content-Length
          header. This method is typically used in the OnData callback
          method. The method initially returns the same length as
          returned by HttpAsynchReq::calcPacketSize.

          \returns
          \li A value > 0 if the packet size is known.
          \li -1: data is of type "chunk transfer encoding".
          This is OK, but application/callbacks cannot use method
          getPacketSizeLeft.
          \li -2: Error: Payload data is of type "multipart/form-data".
          \li -3: Error: Data is x-www-form-urlencoded. The data was
          consumed by the web-server.
       */
      SBaFileSize getPacketSizeLeft();

      /** Returns a pointer to the internal buffer where the received
          data is stored. The buffer is set in method
          HttpAsynchReq::start.
       */
      void* getBuffer();

      /** Returns the lenght of the internal buffer. */
      SBaFileSize getBufferSize();


      bool isChunkEncoding();

      /** Returns the packet size if the length is known.
      This method is typically used before calling method
      HttpAsynchReq::start. You can calculate the length of the
      required buffer such that the entire data being received fits
      into the buffer. The OnData event method fires when the buffer
      is full. Thus, it is possible to make the OnData event method 
      fire one time only if the buffer is large enough to contain all
      data being received.
      <ul>
      <li>Returns 0 if data is URL-encoded. URL-encoded data is
      handled directly by the web-server.</li>
      <li>Returns -1 if payload data is of type "chunk transfer
      encoding". The size is unknown. When the OnData event method "signals
      end of data", we know that this is the end of the data stream.</li>
      <li>Returns -2 if payload data is of type
      "multipart/form-data". This is an error. A resource receiving
      multipart/form-data must use the MultipartUpload class</li>
      </ul>
      */
      static SBaFileSize calcPacketSize(HttpRequest* req);

      /** Start receiving data asynchronously.
      The active socket object is detached from the web-server and
      moved into the HttpAsynchReq object. The HttpAsynchReq object is
      inserted into the SoDisp object, thus causing the socket
      dispatcher to fire receive events directly to the HttpAsynchReq
      object. The event method OnData is called when the buffer is
      full. Thus, the OnData event method can be called a number of
      times if the buffer is not large enough to contain the data
      being received.
      */
      int start(HttpRequest* req,void* buffer, S32 bufferSize);

      /** Aborts the operation and terminates the active socket connection.
      */
      void stop();

      /** Returns the internal active connection object.

      This method is typically called from within the OnData event
      method when all data is received and you are moving the active
      connection object to an HttpAsynchResp object.

      C code example:
      \code
      HttpAsynchResp_constructor2(
         resp, buf, bufSize, HttpAsynchReq_ getCon(req));
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
BA_API void HttpAsynchReq_constructor(HttpAsynchReq* o,
                                HttpServer* server,
                                HttpAsynchReq_OnData data);
BA_API void HttpAsynchReq_destructor(HttpAsynchReq* o);
#define HttpAsynchReq_getPacketSizeLeft(o) (o)->packetSizeLeft
#define HttpAsynchReq_getBuffer(o) (o)->buffer
#define HttpAsynchReq_getBufferSize(o) (o)->bufferSize
#define HttpAsynchReq_isChunkEncoding(o) (o)->chunkEncoding
#define HttpAsynchReq_getServer(o) \
  HttpConnection_getServer((HttpConnection*)(o))
BA_API SBaFileSize HttpAsynchReq_calcPacketSize(HttpRequest* req);
BA_API int HttpAsynchReq_start(
   HttpAsynchReq* o,HttpRequest* req, void* buffer, S32 bufferSize);
/* Internal function used by HttpFiber */
BA_API void HttpAsynchReq_stop(HttpAsynchReq* o);
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
      HttpAsynchReqResp(){}

      /** Create a HttpAsynchReqResp instance.
       \param server the HttpServer instance.
       \param onData the asynchronous receive callback
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
      \param req the HttpRequest object.
      \param recBuf the receive buffer. The onData callback is called
      when the buffer is full.
      \param recBufSize size of recBuf.
      \param sendBuf send buffer used by HttpAsynchResp.
      \param sendBufSize the size of the send buffer. Min size is 256.
       */
      int start(HttpRequest* req,void* recBuf,S32 recBufSize,
                void* sendBuf,S32 sendBufSize);
      /** Start response if you know that all data was received by the
       web-server. This is a faster method than calling start if you
       know that the web-server received all data in its internal
       buffer before delegating the request to the requested service.
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

      /** Get the dispatcher lock.
       */
      ThreadMutex* getMutex();

      /** Get the HttpServer object. */
      HttpServer* getServer();

      /** Get the response object. The object used when sending response data.
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
BA_API void HttpAsynchReqResp_constructor(HttpAsynchReqResp* o,
                                          HttpServer* server,
                                          HttpAsynchReq_OnData data);
#if 0
void HttpAsynchReqResp_disableRecEv(HttpAsynchReqResp* o);
void HttpAsynchReqResp_enableRecEv(HttpAsynchReqResp* o);
#endif
BA_API int HttpAsynchReqResp_start(HttpAsynchReqResp* o,
                                   HttpRequest* req,
                                   void* recBuf,
                                   S32 recBufSize,
                                   void* sendBuf,
                                   S32 sendBufSize);
BA_API int HttpAsynchReqResp_startResp(HttpAsynchReqResp* o,
                                       HttpRequest* req,
                                       void* sendBuf,
                                       S32 sendBufSize);
#define HttpAsynchReqResp_destructor(o) do {\
   HttpAsynchResp_destructor(&(o)->resp);\
   HttpAsynchReq_destructor((HttpAsynchReq*)(o));\
}while(0)
#define HttpAsynchReqResp_getPacketSizeLeft(o)\
   HttpAsynchReq_getPacketSizeLeft((HttpAsynchReq*)o)
#define HttpAsynchReqResp_getBuffer(o)\
   HttpAsynchReq_getBuffer((HttpAsynchReq*)o)
#define HttpAsynchReqResp_getBufferSize(o)\
   HttpAsynchReq_getBufferSize((HttpAsynchReq*)o)
#define HttpAsynchReqResp_isChunkEncoding(o)\
   HttpAsynchReq_isChunkEncoding((HttpAsynchReq*)o)
#define HttpAsynchReqResp_calcPacketSize(req)\
   HttpAsynchReq_calcPacketSize(req)

#define HttpAsynchReqResp_getCon(o) ((o))->resp.con
#define HttpAsynchReqResp_getMutex(o) \
   SoDisp_getMutex(HttpConnection_getDispatcher( \
     HttpAsynchReqResp_getCon(o)))
#define HttpAsynchReqResp_getServer(o) \
  HttpConnection_getServer(HttpAsynchReqResp_getCon(o))
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
