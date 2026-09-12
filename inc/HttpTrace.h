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
 *   $Id: HttpTrace.h 5978 2026-09-11 16:13:48Z wini $
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


/** @file HttpTrace.h */

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


/** Consume buffered trace output synchronously.
 * @param[in,out] buf Borrowed trace buffer, valid only during the callback.
 * The callback may write a NUL at buf[bufLen], but must not retain or free buf.
 * @param[in] bufLen Number of payload bytes; the buffer has room for one more
 * byte. Data need not be NUL-terminated on entry. The callback runs while the
 * trace mutex is held; do not call trace output or writer-lock APIs from it.
 */
typedef void (*HttpTrace_Flush)(char* buf, int bufLen);

/** Buffered diagnostic output and optional HTTP protocol tracing.
 * Install a flush callback at startup to enable output. If the library is
 * built without HTTP_TRACE, output functions have no effect. This header
 * enables HTTP_TRACE by default unless NO_HTTP_TRACE is defined.
 *
 * Priority zero is highest. A message is emitted when its priority is less
 * than or equal to the configured filter (initially 5). Optional HTTP trace
 * categories are initially disabled and use priority 5 when enabled.
 *
 * Output is flushed when the buffer fills, after a write that leaves a
 * newline in the buffer, or by an explicit flush(). Partial lines may remain
 * buffered. The default payload buffer is approximately 80 bytes.
 *
 * Configure the callback and buffer size at startup, before concurrent use.
 * Do not change or close the trace while a writer is locked. Output functions
 * serialize access with the trace mutex; the callback executes under that
 * mutex and must not call back into trace output.
 * @see HttpTrace::setFLushCallback
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

      Newline-terminated output is flushed automatically. To deliver partial
      lines promptly, flush explicitly, for example after a dispatcher poll.

      \code
      for(;;)
      {
         myDispatcher->run(1000);
         HttpTrace::flush();
      }
      \endcode
      */
      /** @param[in] fcb Callback retained until replaced, or NULL to disable
       * output. Allocation failure during initialization leaves tracing
       * disabled; HttpTrace_getFLushCallback returns NULL in that case. */
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
          \param buf Required input buffer, borrowed for the duration of the call.
          \param len Byte count; any negative value uses strlen(buf), requiring
          a NUL-terminated string. Zero writes no bytes.
       */
      static void write(int prio, const char* buf, int len = -1);

      /** Set the trace message priority filter. Priority 0 is the
          highest priority.  Setting the priority to say 10 means that
          only trace messages with a priority less than or equal to
          10 will be printed to the trace buffer.

	  \param prio Maximum message priority to emit; use 0-255 [default=5]
          \return previous priority.
      */
      static int setPrio(int prio);

      /** Get and lock the trace BufPrint object.
          @return Borrowed locked writer, or NULL when tracing is disabled.
          Call releaseWriter exactly once only after a non-NULL result. Writing
          through this object bypasses the priority filter. Do not free it.
          \sa releaseWriter
          \sa HttpTraceWriteLock
       */
      static BufPrint* getWriter();

      /** Release a writer lock obtained by a successful getWriter call.
          Flushes pending output containing a newline before unlocking.
          \sa HttpTraceWriteLock
      */
      static void releaseWriter(void);

      /** Enable or disable request-line tracing.
         If enabled, the web-server prints the first line in the
         request header to the trace buffer.
         <p>
         Example:
         </p>
         \code
         68.5.99.169 GET "intro.html" Mozilla/5.0 (Macintosh; U; PPC Mac OS X Mach-O; en-US; rv:1.7) Gecko/20040623 Camino/0.8
         \endcode
         \param cmd TRUE to enable request-line tracing.
      */
      static void setRequest(bool cmd);

      /** If enabled, the web-server dumps the content of the request
          header to the trace buffer.
          \param cmd TRUE to enable request-header tracing.
      */
      static void setRequestHeaders(bool cmd);

      /** If enabled, the web-server dumps the content of the response
          header to the trace buffer.
          \param cmd TRUE to enable response-header tracing.
      */
      static void setResponseHeaders(bool cmd);

      /** If enabled, the web-server dumps the content of the response
          body to the trace buffer.
          Warning: this generates an enormous amount of trace data.
          \param cmd TRUE to enable response-body tracing.
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
      /** @param[in] cmd TRUE enables this trace category; FALSE disables it. */
      static void setHttp11State(bool cmd);

      /** Report when an HTTP request exceeds the configured request buffer.
       * @param[in] cmd TRUE enables reporting; FALSE disables it.
       * @see HttpServerConfig::setRequest
       */
      static void setReqBufOverflow(bool cmd);

      /** Set trace buffer size. The default buffer is 80 characters long.
      This function is not re-entrant, and you should therefore call
      this function at system startup.
      \param size Positive buffer allocation size in bytes; use at least 81.
      The payload capacity can be one byte smaller. Existing buffered data is
      discarded when replacing the buffer. Call only during startup.
      \return 0 on success, -1 on allocation failure (which disables tracing).
      Without HTTP_TRACE, the stub returns 0 without allocating storage.
       */
      static int setBufSize(int size);

      /** Force a flush on data in trace buffer; i.e., call the flush callback.
      */
      static void flush();

      /** Returns true if request-line tracing is enabled. */
      static bool isRequestSet();
      /** Returns true if request-header tracing is enabled. */
      static bool isRequestHeadersSet();
      /** Returns true if response-header tracing is enabled. */
      static bool isResponseHeadersSet();
      /** Returns true if response-body tracing is enabled. */
      static bool isResponseBodySet();
      /** Returns true if HTTP/1.1 connection-state tracing is enabled. */
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
/** @copydoc HttpTrace::setPrio */
BA_API int HttpTrace_setPrio(int prio);
/** @copydoc HttpTrace::setFLushCallback */
BA_API void HttpTrace_setFLushCallback(HttpTrace_Flush fcb);
/** @return Currently installed callback, or NULL when tracing is disabled.
 * Inspect during startup; this function does not acquire the trace mutex. */
BA_API HttpTrace_Flush HttpTrace_getFLushCallback(void);
/** @copydoc HttpTrace::vprintf */
BA_API void HttpTrace_vprintf(int prio, const char* fmt, va_list argList);
/** @copydoc HttpTrace::printf */
BA_API void HttpTrace_printf(int prio, const char* fmt, ...);
/** @copydoc HttpTrace::write */
BA_API void HttpTrace_write(int prio, const char* buf, int len);
/** @copydoc HttpTrace::flush */
BA_API void HttpTrace_flush(void);

/** @copydoc HttpTrace::setRequest */
BA_API void HttpTrace_setRequest(BaBool cmd);
/** @copydoc HttpTrace::setRequestHeaders */
BA_API void HttpTrace_setRequestHeaders(BaBool cmd);
/** @copydoc HttpTrace::setResponseHeaders */
BA_API void HttpTrace_setResponseHeaders(BaBool cmd);
/** @copydoc HttpTrace::setResponseBody */
BA_API void HttpTrace_setResponseBody(BaBool cmd);
/** @copydoc HttpTrace::setHttp11State */
BA_API void HttpTrace_setHttp11State(BaBool cmd);
/** @copydoc HttpTrace::setReqBufOverflow */
BA_API void HttpTrace_setReqBufOverflow(BaBool cmd);
/** @copydoc HttpTrace::setBufSize */
BA_API int HttpTrace_setBufSize(int size);
/** Release the trace buffer and disable output without flushing pending
 * bytes. Call at shutdown after all trace users and writer locks have stopped.
 * No output or error status is returned. */
BA_API void HttpTrace_close(void);

/* Internal funtion. Used by macros below */
BA_API U8 HttpTrace_getTraceCmds(void);
/** @return TRUE when this trace category is enabled, otherwise FALSE. */
#define HttpTrace_isRequestSet() \
  (HttpTrace_getTraceCmds() & HttpTrace_doRequestMask ? TRUE : FALSE)
/** @return TRUE when this trace category is enabled, otherwise FALSE. */
#define HttpTrace_isRequestHeadersSet() \
  (HttpTrace_getTraceCmds() & HttpTrace_doRequestHeadersMask ? TRUE : FALSE)
/** @return TRUE when this trace category is enabled, otherwise FALSE. */
#define HttpTrace_isResponseHeadersSet() \
  (HttpTrace_getTraceCmds() & HttpTrace_doResponseHeadersMask ? TRUE : FALSE)
/** @return TRUE when this trace category is enabled, otherwise FALSE. */
#define HttpTrace_isResponseBodySet() \
  (HttpTrace_getTraceCmds() & HttpTrace_doResponseBodyMask ? TRUE : FALSE)
/** @return TRUE when this trace category is enabled, otherwise FALSE. */
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
/** @copydoc HttpTrace::getWriter */
BA_API BufPrint* HttpTrace_getWriter(void);
/** @copydoc HttpTrace::releaseWriter */
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
      /** Attempt to acquire the trace writer; conversion returns NULL if disabled. */
      HttpTraceWriteLock() {
         _bufPrint = HttpTrace_getWriter();
      }
      /** Release any held writer lock. */
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

      /** @return Borrowed writer while locked, or NULL when disabled or released.
       */
      operator BufPrint*() { return _bufPrint; }

      /** @return Borrowed writer while locked. Check conversion to BufPrint* for
       * NULL before dereferencing; no writer exists when tracing is disabled.
       */
      BufPrint* operator -> () { return _bufPrint; }
   private:
      BufPrint* _bufPrint;
};
#endif



#endif
