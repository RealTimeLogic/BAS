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
 *   $Id: HttpAsynchResp.h 5355 2022-11-16 10:33:45Z wini $
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
   ThreadLock(myAsynchResp->getMutex());
   myAsynchResp->getWriter()->printf("Hi client");
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

      HttpAsynchResp() {}

      /** Initiate a HttpAsynchResp from a HttpRequest object.
          \param buf the output buffer. Minimum value is 255 bytes.
          \param size the output buffer size.
          \param req the active connection object is moved from HttpRequest to
          this object.
      */
      HttpAsynchResp(char* buf, int size, HttpRequest* req);

      /** Initiate a HttpAsynchResp from a HttpConnection object.
          \param buf the output buffer. Minimum value is 255 bytes.
          \param size the output buffer size.
          \param con the active connection object is moved from
          HttpConnection to this object.
      */
      HttpAsynchResp(char* buf,int size,HttpConnection* con);

      /** Returns true if the HttpConnection object is valid -- i.e., if
          the socket connection is alive.
       */
      bool isValid();

      /** Set in asynchronous thread mode so you can call the methods in
      this class without having to lock the dispatcher mutex. You must
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
      connection such as a HTTP 1.0 connection is terminated.
      */
      void close();

      /** Get the dispatcher lock. This is the only method in this
          class that can be called without locking the dispatcher.
       */
      ThreadMutex* getMutex();

      /** Close connection when the HttpAsynchResp object is done.
      A persistent HTTP 1.1 connection is recycled and sent back to
      the connection pool in the web-server when the HttpAsynchResp
      object is done. Calling this method makes sure the connection
      closes when the HttpAsynchResp object is done. You should call
      this method if you stream data such as audio.
       */
      int setConClose();

      /** Used if a resource must close an active HttpAsynchReq and data
          is pending.

          If a server closes the input side of the connection while
          the client is sending data (or is planning to send data),
          then the server's TCP stack will signal an RST (reset) back
          to the client. This forces a disconnect before client can
          read response data.

          Please note that Barracuda is not implementing this by using
          SO_LINGER, but is instead using a lingering close queue.
       */
      int setLingeringClose();


      /** Set the response status code. The default return code is 200.
          \param statusCode The
           <a href="http://www.w3.org/Protocols/HTTP/HTRESP.html">
           HTTP status code</a>
           \param protocol set the protocol version. The protocol is
           set to "1.1" if this parameter is NULL.
       */
      int setStatus(int statusCode, const char* protocol=0);

      /** Sets a 
          <a href="http://www.w3.org/Protocols/rfc2616/rfc2616-sec14.html">
          HTTP response header</a> with the given name and value.
          \param name the name of the header to set.
          \param value the header value.
      */
      int setHeader(const char *name, const char *value);

      /** Send data of known size to client.
          \param data the data to send.
          \param pktSize The size of the data to send.
          \param chunkSize the size of parameter 'data'. Set this
          parameter to pktSize if all data is sent in the sendData
          call.  Otherwise, set chunkSize to the size of the chunk being
          sent, and then call method sendNextChunk.
      */
      int sendData(const void* data, int pktSize, int chunkSize);

      /** Send next chunk if not all data was sent with sendData.
          \param data the data to send.
          \param chunkSize the size of parameter 'data'.
      */
      int sendNextChunk(const void* data,int chunkSize);

      /** BufPrint is used when sending data of unknown length. For a
          HTTP/1.1. connection, the data is sent in 
   <a href="http://www.w3.org/Protocols/rfc2616/rfc2616-sec3.html#sec3.6.1">
          chunk transfer mode.</a>

          This method returns NULL if HttpAsynchResp::sendData was called
          prior to this method.
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
BA_API void HttpAsynchResp_constructor(
   HttpAsynchResp* o, char* buf, int size, HttpRequest* req);
BA_API void HttpAsynchResp_constructor2(
   HttpAsynchResp* o, char* buf, int size, HttpConnection* con);
BA_API void HttpAsynchResp_ReqRespInit(
   HttpAsynchResp* o, char* buf, int size, HttpConnection* con);
BA_API BaBool HttpAsynchResp_isValid(HttpAsynchResp* o);
#define HttpAsynchResp_asynchThreadMode(o) \
   ((o)->mutex=HttpAsynchResp_getMutex(o))
BA_API void HttpAsynchResp_close(HttpAsynchResp* o);
#define HttpAsynchResp_destructor(o) HttpAsynchResp_close(o)
#define HttpAsynchResp_getMutex(o) \
   SoDisp_getMutex(HttpConnection_getDispatcher((HttpConnection*)(o)))
#define HttpAsynchResp_setConClose(o)\
   HttpConnection_clearKeepAlive((HttpConnection*)(o))
#define HttpAsynchResp_setLingeringClose(o) (o)->doLingeringClose=TRUE
BA_API int HttpAsynchResp_setStatus(
   HttpAsynchResp* o, int statusCode, const char* protocol);
BA_API int HttpAsynchResp_setHeader(
   HttpAsynchResp* o,const char *name,const char *value);
BA_API int HttpAsynchResp_sendData(
   HttpAsynchResp* o, const void* data, int pktSize, int chunkSize);
BA_API int HttpAsynchResp_sendNextChunk(
   HttpAsynchResp* o,const void* data,int chunkSize);
BA_API BufPrint* HttpAsynchResp_getWriter(HttpAsynchResp* o);
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
