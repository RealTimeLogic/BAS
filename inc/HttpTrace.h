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
 *   $Id: HttpTrace.h 4915 2021-12-01 18:26:55Z wini $
 *
 *   COPYRIGHT:  Real Time Logic LLC, 2004-2021
 *
 *   This software is copyrighted by and is the sole property of Real
 *   Time Logic LLC.  All rights, title, ownership, or other interests in
 *   the software remain the property of Real Time Logic LLC.  This
 *   software may only be used in accordance with the terms and
 *   conditions stipulated in the corresponding license agreement under
 *   which the software has been supplied.  Any unauthorized use,
 *   duplication, transmission, distribution, or disclosure of this
 *   software is expressly forbidden.
                                                                        
 *   This Copyright notice may not be removed or modified without prior
 *   written consent of Real Time Logic LLC.
                                                                         
 *   Real Time Logic LLC. reserves the right to modify this software
 *   without notice.

 *               http://www.realtimelogic.com
 ****************************************************************************
 *
 */


#ifndef __HttpTrace_h
#define __HttpTrace_h

#include <HttpServer.h>

#ifdef __cplusplus
#undef printf
#endif

#ifndef NO_HTTP_TRACE
#ifndef HTTP_TRACE
#define HTTP_TRACE
#endif
#endif

#define HttpTrace_doRequestMask          0x01
#define HttpTrace_doRequestHeadersMask   0x02
#define HttpTrace_doResponseHeadersMask  0x04
#define HttpTrace_doResponseBodyMask     0x08
#define HttpTrace_doHttp11StateMask      0x10
#define HttpTrace_doReqBufOverflowMask   0x20


typedef void (*HttpTrace_Flush)(char* buf, int bufLen);

/** Trace the communication between a client and the server.  This
    class is enabled if the Barracuda Web-Server library is compiled
    with macro HTTP_TRACE enabled. The methods in this class have no
    effect if the trace functionality is not enabled by macro
    HTTP_TRACE. You must also provide a console callback function
    before the trace functionality is activated. See
    HttpTrace::setFLushCallback for more information.

    Most user functions in the web-server return 0 on success and an
    error number if any of the parameters were incorrect. The web-server also
    prints the error to the trace buffer, and it is for this reason
    recommended that you always provide a trace callback function when
    doing development.

    The trace library is typically used by your code when
    debugging. The printf and write member functions take the message
    priority as argument. You can, therefore, design the code such that
    more important functions use a higher priority. See
    HttpTrace::setPrio for more information.

    The web-server is using the trace library to write information
    about user requests and response data. This is by default
    disabled. If enabled, the web-server writes the data to the trace
    buffer by using priority 5. See the setXXX functions for more
    information.

    The trace callback function can write to internal memory, your
    screen (if you have one) or use an SMTP library.

    Here is a trace callback function example that is using the SMTP library:
    \code
    //The flushTrace function is called by HttpTrace when trace buffer is full.
    static void flushTrace(char* buf, int bufLen)
    {
       const char* from = "device1@realtimelogic.com";
       const char* to = "bill@mycompany.com";
       const char* subject = "DEVICE 1 TRACE DATA";
       const char* smtpServer = "192.168.0.1";
       SMTP smtp(from, to, subject, smtpServer);
       smtp.write(buf, bufLen);
    }

    //This function should be called at startup
    void myInit(void)
    {
       // The default HttpTrace buffer is 80 bytes,
       // which would lead to a lot of emails.
       HttpTrace::setBufSize(2000);
       HttpTrace::setFLushCallback(flushTrace);
       //By default, print all web-server access to the trace buffer.
       HttpTrace::setRequest(true);
    }
    \endcode

    The methods in the HttpTrace class are re-entrant, except for
    HttpTrace::setFLushCallback and HttpTrace::setBufSize. These two
    functions, if used, must be called at system startup.

*/
typedef struct HttpTrace
{
#ifdef __cplusplus
      /**

      The HTTP_TRACE compile time macro adds the trace library to the
      Barracuda library, but the trace functions will have no effect
      if you do not provide a callback function.

      This function is not re-entrant; therefore, you should call
      this function at system startup.

      <p>
      The following example dumps data to the console:
      </p>
      \code
      void flush2Console(char* buf, int bufLen)
      {
         // The trace buffer is always > bufLen
         buf[bufLen] = 0; // convert to string
         printf("%s",buf);
      }
      .
      .
      HttpTrace_setFLushCallback(flush2Console);
      \endcode

      Please note that data written to the trace is not
      automatically flushed. It may be a good idea to run the
      web-server in poll mode and flush the data manually after the
      web-server is run.

      \code
      for(;;)
      {
         myDispatcher->run(1000);
         HttpTrace::flush();
      }
      \endcode
      */
      static void setFLushCallback(HttpTrace_Flush fcb);

      /** Write data to the trace buffer.
          Works just like the regular vprintf function.
          \param prio See HttpTrace::setPrio.
          \param fmt See BufPrint::vprintf
          \param argList See BufPrint::vprintf
          more information.
       */
      static void vprintf(int prio, const char* fmt, va_list argList);

      /** Write data to the trace buffer.
          Works just like the regular printf function.
          \param prio See HttpTrace::setPrio.
          \param fmt See BufPrint::printf
       */
      static void printf(int prio, const char* fmt, ...);

      /** Write data to the trace buffer.
          \param prio See HttpTrace::setPrio.
          \param buf a pointer to the buffer/string.
          \param len is optional, not needed for strings.
       */
      static void write(int prio, const char* buf, int len = -1);

      /** Set the trace message priority filter. Priority 0 is the
          highest priority.  Setting the priority to say 10 means that
          only trace messages with a priority greater than or equal to
          10 will be printed to the trace buffer.

	  \param prio The priority [default=5]
          \return previous priority.
      */
      static int setPrio(int prio);

      /** Get and lock the trace BufPrint object.
          \sa releaseWriter
          \sa HttpTraceWriteLock
       */
      static BufPrint* getWriter();

      /** Release the trace. Use in combination with getWriter.
          \sa HttpTraceWriteLock
      */
      static void releaseWriter(void);

      /**
         If enabled, the web-server prints the first line in the
         request header to the trace buffer.
         <p>
         Example:
         </p>
         \code
         68.5.99.169 GET "intro.html" Mozilla/5.0 (Macintosh; U; PPC Mac OS X Mach-O; en-US; rv:1.7) Gecko/20040623 Camino/0.8
         \endcode
      */
      static void setRequest(bool cmd);

      /** If enabled, the web-server dumps the content of the request
          header to the trace buffer.
      */
      static void setRequestHeaders(bool cmd);

      /** If enabled, the web-server dumps the content of the response
          header to the trace buffer.
      */
      static void setResponseHeaders(bool cmd);

      /** If enabled, the web-server dumps the content of the response
          body to the trace buffer.
          Warning: this generates an enormous amount of trace data.
      */
      static void setResponseBody(bool cmd);

      /** If enabled, prints the status for each active client connection of
          the internal "HTTP 1.1 persistent connection" state machine
          to the trace buffer.
          <p>
          The state machine can be in one of 5 states. As an
          example, a non-persistent connection will go through the
          following states for each request:
          </p>
          \code
          Connection 56c1f0 3944 trans: Free -> Connected
          Connection 56c1f0 3944 trans: Connected -> Running
          Connection 56c1f0 3944 trans: Running -> Terminated
          Connection 56c1f0 -001 trans: Terminated -> Free
          \endcode
          A persistent HTTP connection is in one of the Connected or
          Running states.
          \code
          Connection 56c290 3944 trans: Connected -> Running
          Connection 56c290 3944 trans: Running -> Connected
          Connection 56c290 3944 trans: Connected -> Running
          Connection 56c290 3944 trans: Running -> Connected
          Connection 56c290 3944 trans: Connected -> Running
          Connection 56c290 3944 trans: Running -> Connected
          \endcode
      */
      static void setHttp11State(bool cmd);

      /** Prints "ReqBufOverflow" if enabled and the HttpRequest
          buffer overflows. A buffer overflow occurs if the client
          request stream overflows the request buffer. This could
          potentially be an indication of a Buffer Overflow
          Attack. Buffer Overflow Attacks are very common against
          web servers. Barracuda is immune to Buffer Overflow
          Attacks. See HttpServerConfig::setRequest for more
          information.
       */
      static void setReqBufOverflow(bool cmd);

      /** Set trace buffer. The default buffer is 80 characters long.
      This function is not re-entrant, and you should therefore call
      this function at system startup.
       */
      static int setBufSize(int size);

      /** Force a flush on data in trace buffer; i.e., call the flush callback.
      */
      static void flush();

      static bool isRequestSet();
      static bool isRequestHeadersSet();
      static bool isResponseHeadersSet();
      static bool isResponseBodySet();
      static bool isHttp11StateSet();

   private:
      /**
         You cannot create an instance of this class.
      */
      inline HttpTrace() {}
#endif
      BufPrint print; /* As if inherited */
      int prio;
      U8 traceCmds;
}HttpTrace;

#ifdef __cplusplus
extern "C" {
#endif
BA_API int HttpTrace_setPrio(int prio);
BA_API void HttpTrace_setFLushCallback(HttpTrace_Flush fcb);
BA_API HttpTrace_Flush HttpTrace_getFLushCallback(void);
BA_API void HttpTrace_vprintf(int prio, const char* fmt, va_list argList);
BA_API void HttpTrace_printf(int prio, const char* fmt, ...);
BA_API void HttpTrace_write(int prio, const char* buf, int len);
BA_API void HttpTrace_flush(void);

BA_API void HttpTrace_setRequest(BaBool cmd);
BA_API void HttpTrace_setRequestHeaders(BaBool cmd);
BA_API void HttpTrace_setResponseHeaders(BaBool cmd);
BA_API void HttpTrace_setResponseBody(BaBool cmd);
BA_API void HttpTrace_setHttp11State(BaBool cmd);
BA_API void HttpTrace_setReqBufOverflow(BaBool cmd);
BA_API int HttpTrace_setBufSize(int size);
BA_API void HttpTrace_close(void);

/* Internal funtion. Used by macros below */
BA_API U8 HttpTrace_getTraceCmds(void);
#define HttpTrace_isRequestSet() \
  (HttpTrace_getTraceCmds() & HttpTrace_doRequestMask ? TRUE : FALSE)
#define HttpTrace_isRequestHeadersSet() \
  (HttpTrace_getTraceCmds() & HttpTrace_doRequestHeadersMask ? TRUE : FALSE)
#define HttpTrace_isResponseHeadersSet() \
  (HttpTrace_getTraceCmds() & HttpTrace_doResponseHeadersMask ? TRUE : FALSE)
#define HttpTrace_isResponseBodySet() \
  (HttpTrace_getTraceCmds() & HttpTrace_doResponseBodyMask ? TRUE : FALSE)
#define HttpTrace_isHttp11StateSet() \
  (HttpTrace_getTraceCmds() & HttpTrace_doHttp11StateMask ? TRUE : FALSE)

/* Private Barracuda functions */

#ifdef HTTP_TRACE
#define TRPR(data) HttpTrace_TRPR data
BA_API void HttpTrace_TRPR(const char* fmt, ...);
#else
#define TRPR(data)
#endif
BA_API HttpTrace* HttpTrace_get(void);
#define HttpTrace_doRequest() \
  (HttpTrace_get()->traceCmds & HttpTrace_doRequestMask)
#define HttpTrace_doRequestHeaders() \
  (HttpTrace_get()->traceCmds & HttpTrace_doRequestHeadersMask)
#define HttpTrace_doResponseHeaders() \
  (HttpTrace_get()->traceCmds & HttpTrace_doResponseHeadersMask)
#define HttpTrace_doResponseBody() \
  (HttpTrace_get()->traceCmds & HttpTrace_doResponseBodyMask)
#define HttpTrace_doHttp11State() \
  (HttpTrace_get()->traceCmds & HttpTrace_doHttp11StateMask)
#define HttpTrace_doReqBufOverflow() \
  (HttpTrace_get()->traceCmds & HttpTrace_doReqBufOverflowMask)
BA_API BufPrint* HttpTrace_getWriter(void);
BA_API void HttpTrace_releaseWriter(void);
#ifdef __cplusplus
}
inline int HttpTrace::setPrio(int prio) {
   return HttpTrace_setPrio(prio);
}
inline void HttpTrace::setFLushCallback(HttpTrace_Flush fcb) {
   HttpTrace_setFLushCallback(fcb); }
inline void HttpTrace::vprintf(int prio, const char* fmt, va_list argList) {
   HttpTrace_vprintf(prio, fmt, argList); }
inline void HttpTrace::printf(int prio, const char* fmt, ...) {
   va_list varg;
   va_start(varg, fmt);
   HttpTrace_vprintf(prio, fmt, varg);
   va_end(varg);
}
inline void HttpTrace::write(int prio, const char* buf, int len) {
   HttpTrace_write(prio, buf, len); }

inline BufPrint* HttpTrace::getWriter() {
   return HttpTrace_getWriter(); }
inline void HttpTrace::releaseWriter(void) {
   HttpTrace_releaseWriter(); }
inline void HttpTrace::setRequest(bool cmd) {
   HttpTrace_setRequest(cmd); }
inline void HttpTrace::setRequestHeaders(bool cmd) {
   HttpTrace_setRequestHeaders(cmd); }
inline void HttpTrace::setResponseHeaders(bool cmd) {
   HttpTrace_setResponseHeaders(cmd); }
inline void HttpTrace::setResponseBody(bool cmd) {
   HttpTrace_setResponseBody(cmd); }
inline void HttpTrace::setHttp11State(bool cmd) {
   HttpTrace_setHttp11State(cmd); }
inline void HttpTrace::setReqBufOverflow(bool cmd) {
   HttpTrace_setReqBufOverflow(cmd); }
inline int HttpTrace::setBufSize(int size) {
   return HttpTrace_setBufSize(size); }
inline void HttpTrace::flush() {
   HttpTrace_flush();
}
inline bool HttpTrace::isRequestSet() {
   return HttpTrace_isRequestSet() ? true : false;
}
inline bool HttpTrace::isRequestHeadersSet() {
   return HttpTrace_isRequestHeadersSet() ? true : false;
}
inline bool HttpTrace::isResponseHeadersSet() {
   return HttpTrace_isResponseHeadersSet() ? true : false;
}
inline bool HttpTrace::isResponseBodySet() {
   return HttpTrace_isResponseBodySet() ? true : false;
}
inline bool HttpTrace::isHttp11StateSet() {
   return HttpTrace_isHttp11StateSet() ? true : false;
}
#endif

#ifdef __cplusplus
/** HttpTraceWriteLock is a C++ class that makes it easier to lock the
 * trace by using method HttpTrace::getWriter and
 * HttpTrace::releaseWriter.
 * The constructor calls getWriter and the desctructor calls releaseWriter.
 */
struct HttpTraceWriteLock
{
      HttpTraceWriteLock() {
         _bufPrint = HttpTrace_getWriter();
      }
      ~HttpTraceWriteLock() {
         release();
      }

      /** Release the trace if not already released. */
      void release() {
         if(_bufPrint) {_bufPrint=0; HttpTrace_releaseWriter(); }
      }

      /** Lock the trace if not already locked. */
      void lock() {
         if(!_bufPrint) {_bufPrint = HttpTrace_getWriter(); }
      }

      /** Operator that returns the BufPrint object.
       */
      operator BufPrint*() { return _bufPrint; }

      /** Operator that makes it possible to do myLock->printf("hi");
       */
      BufPrint* operator -> () { return _bufPrint; }
   private:
      BufPrint* _bufPrint;
};
#endif



#endif
