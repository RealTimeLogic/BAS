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
 *   $Id: HttpSharkSslServCon.h 6056 2026-09-20 05:09:33Z wini $
 *
 *   COPYRIGHT:  Real Time Logic LLC, 2004 - 2026
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
 *             SharkSSL for Barracuda Embedded Web-Server
 *
 */
/** @file HttpSharkSslServCon.h
    @brief SharkSSL listener and transport adapter.

    @section sharkssl_async_api Asynchronous sending: legacy and current API

    The adapter implements the existing SoDispCon asynchronous API for TLS
    client and server connections. HttpConnection uses the same operations.
    The newer SharkSSL handshake-output and KeyUpdate APIs are handled inside
    the adapter; application callers continue using the interfaces below.

    Use an established TLS connection with a completed handshake. Serialize
    access using the connection's dispatcher mutex and allow only one writer.
    Select nonblocking socket mode for sending from dispatcher callbacks.

    @subsection sharkssl_async_buffer Borrowed buffer and return values

    SoDispCon_allocAsynchBuf() returns a borrowed void* and writes the available
    capacity, in bytes, to the required int* size argument. The TLS adapter
    reports its existing encryption-buffer capacity; it does not allocate a
    new buffer or resize it to the requested size. A zero input size is also
    supported for TLS capacity queries. Always check the pointer and use the
    returned capacity. NULL with size zero can mean the connection is busy,
    including pending TLS output; it does not necessarily mean out of memory.

    Obtaining the pointer does not reserve the connection or start a send.
    Repeated lookups and capacity-only queries are allowed while idle, including
    before an ordinary synchronous write. The pointer belongs to the TLS
    connection and must not be freed or used after that connection is destroyed.
    TLS processing can change its contents. Fill it immediately before sending,
    without intervening operations that use the same connection's TLS buffers.

    SoDispCon_asyncSend(con, len) starts a new block when no send is pending.
    The int len is the payload length in bytes, from 1 through the returned
    capacity. Encryption occurs in place, so the plaintext contents need not
    survive the call. SoDispCon_asyncReady(con), or asyncSend with len zero,
    advances pending output without starting another application block.
    Both operations return an int status, not a byte count:
    - 1: Output is complete and the buffer can be refilled. This means transport
      completion, not acknowledgement by the peer application.
    - 0: Output remains pending. Keep the buffer unchanged and arrange a
      send-ready callback to continue. These calls do not register callbacks.
    - A negative value: Transport error. Stop using the connection for sending
      and perform the owning application's error/close handling.

    A block accepted by asyncSend must not be copied or submitted as a new block
    again when the result is zero. While that send is pending, a legacy call to
    asyncSend with a positive length also advances the existing output; its
    length does not describe additional data. Prefer asyncReady for continuation
    so the intent is explicit. Yield to the dispatcher when the result is zero;
    do not spin waiting for completion.

    @subsection sharkssl_async_legacy Legacy pattern: retain and reuse the buffer

    1. Obtain the buffer and capacity once with SoDispCon_allocAsynchBuf().
    2. Optionally call SoDispCon_asyncReady() before filling it. A successful
       readiness check does not invalidate the borrowed pointer.
    3. Fill at most the returned capacity and call SoDispCon_asyncSend().
    4. If the result is zero, keep the buffer unchanged and advance the send
       from send-ready callbacks until completion or error.
    5. After result 1, refill the same buffer and send the next block. A new
       buffer lookup or explicit release between blocks is not required.

    This supports callers such as the native file reader and tunnel code.
    Capacity lookup followed by synchronous output is also supported.

    @subsection sharkssl_async_current Current pattern: obtain the buffer per block

    Callers such as the Lua nonblocking writer may obtain the buffer again for
    each new block. Complete the previous send first, obtain the pointer and
    capacity, fill it, and call SoDispCon_asyncSend(). Continue pending output
    exactly as in the legacy pattern. Both patterns use the same public API;
    reacquisition is optional and does not allocate a new TLS buffer.

    During pending asynchronous output, buffer lookup and synchronous writes
    are rejected and TLS reads are deferred. Do not force the connection idle,
    release its send buffer, or alter its buffer contents to bypass this state.
    Completion permits the next operation; obtaining a pointer alone does not
    impose these restrictions.

    The C macros and SoDispCon_asyncReadyF() preserve negative error results.
    The current C++ SoDispCon::asyncReady() wrapper converts the result to bool,
    so an error also becomes true. Use the C macro or function from C++ when
    distinguishing completion from failure.

    @subsection sharkssl_control_api New SharkSSL control-output API

    TLS output can include handshake or KeyUpdate records in addition to
    application data. When asynchronous application output triggers a key
    update, result 1 is delayed until that control output has also been sent.
    A zero result can therefore remain after all application bytes have left
    the socket. Continue through the same send-ready path without refilling
    the buffer or resending the application block.

    The adapter obtains control bytes with SharkSslCon_getHandshakeData() and
    SharkSslCon_getHandshakeDataLen(). After each valid transport send result
    it calls SharkSslCon_setHandshakeDataSent() with the actual U16 byte count,
    including a partial send, a complete send, or zero bytes. The remaining
    pointer and length are obtained again before the next send. Applications
    using the BAS transport must leave this accounting to the adapter.

    SharkSslCon_isHandshakeComplete() returns U8: 0 means incomplete, 1 means
    complete, and 2 means complete with additional buffered input to process.
    The adapter continues processing buffered input for the latter case during
    handshake-only reads. These engine-level requirements do not introduce a
    per-block reservation requirement in the SoDispCon API.
 */

#ifndef __HttpSharkSslServCon_h
#define __HttpSharkSslServCon_h

#include <HttpServCon.h>
#include <BaServerLib.h>
#include <SharkSSL.h>
#include <string.h>
#include <DoubleList.h>






/** Create a SharkSSL server listen object.
    The object binds itself to the SoDisp object and makes the
    web-server listen for new connections on the port specified.
    The default port is 443. You can create several of these objects and bind
    to the dispatcher if you want the web-server to listen to more than one
    port.


 */
typedef struct HttpSharkSslServCon
{
#ifdef __cplusplus

      void *operator new(size_t s) { return ::baMalloc(s); }
      void operator delete(void* d) { if(d) ::baFree(d); }
      void *operator new(size_t, void *place) { return place; }
      void operator delete(void*, void *) { }

      /** Uninitialized storage; use a C constructor before use or destruction. */
      HttpSharkSslServCon() {}
      
      /** Create a SharkSSL Server Connection.

      \param sharkSsl is the SharkSsl instance required for operating
       the server connection. You must make sure that this object is
       valid for the lifetime of the listener and its accepted connections.
       It must have role SharkSsl_Server and be configured before listening.
       An incorrect role violates the constructor precondition.
      \param server Required borrowed server, valid throughout listener use.
      \param dispatcher Required borrowed dispatcher, valid throughout use.
      \param port TCP listen port in host byte order, normally 443. Zero
      initializes an inactive listener without opening a socket.
      \param setIP6 TRUE selects IPv6, FALSE IPv4. This parameter is ignored unless
      the underlying TCP/IP stack is a dual IP V4 and IP V6 stack.
      \param interfaceName Borrowed platform binding address/interface, used
      during the call; normally a NUL-terminated address string. NULL binds the
      wildcard address. Accepted representation is platform-specific.
      \param userDefinedAccept Same functionality as for HttpServCon,
      but this callback is for implementing secure servers.
      The temporary-connection ownership rules of HttpServCon_AcceptNewCon
      apply. The callback may run before the TLS handshake completes. It is
      required in NO_BA_SERVER builds; otherwise NULL selects HTTP handling.
       */
      HttpSharkSslServCon(SharkSsl* sharkSsl,
                          HttpServer* server,
                          SoDisp* dispatcher,
                          U16 port=443,
                          bool setIP6=false,
                          const void* interfaceName=0,
                          HttpServCon_AcceptNewCon userDefinedAccept=0);
      /** @return TRUE when the listening socket is valid, FALSE otherwise.
       * Check after construction, which returns no status. */
      BaBool isValid();

      /** Open a replacement listener, then close the old listener on success.
       * @param[in] portNumber New TCP port in host byte order, 1..65535.
       * @param[in] setIp6 True selects IPv6, false IPv4 (default).
       * @param[in] interfaceName Borrowed platform binding address/interface,
       * or NULL for wildcard; used during this call only.
       * @return Zero on success, -1 on failure. The old listener is retained
       * if replacement setup fails. Uses the associated server's dispatcher.
       * Hold the dispatcher mutex during this operation.
       */
      int setPort(U16 portNumber, bool setIp6=false,
                  const void* interfaceName=0);
      /** Close the listener and every accepted TLS connection still tracked by it.
    * Does not destroy the borrowed TLS configuration, server or dispatcher. */
   ~HttpSharkSslServCon();
   private:
#endif
      HttpServCon sCon;
      DoubleList sharkSslConList;
      SharkSsl* sharkSsl;
      BaBool requestClientCert;
      BaBool favorRSA;
} HttpSharkSslServCon;


#ifdef __cplusplus
extern "C" {
#endif
/** @copydoc HttpSharkSslServCon::HttpSharkSslServCon(SharkSsl*,HttpServer*,SoDisp*,U16,bool,const void*,HttpServCon_AcceptNewCon)
 * @param[out] o Caller-owned listener storage. */
SHARKSSL_API void HttpSharkSslServCon_constructor(
   HttpSharkSslServCon* o,
   SharkSsl* sharkSsl,
   struct HttpServer* server,
   struct SoDisp* dispatcher,
   U16 port,
   BaBool setIP6,
   const void* interfaceName,
   HttpServCon_AcceptNewCon userDefinedAccept);
SHARKSSL_API int HttpServCon_setPort(HttpServCon* o, U16 portNumber,
                               BaBool setIp6, const void* interfaceName);
/** @copydoc HttpSharkSslServCon::setPort
 * @param[in,out] o Initialized listener. */
SHARKSSL_API int HttpSharkSslServCon_setPort(HttpSharkSslServCon* o,
                                             U16 portNumber,
                                             BaBool setIp6,
                                             const void* interfaceName);
/** @param[in] o Initialized listener.
 * @return TRUE if its listening socket is valid, FALSE otherwise. */
#define HttpSharkSslServCon_isValid(o) HttpServCon_isValid((HttpServCon*)o)
/** Store the listener's client-certificate request flag.
 * @param[in,out] o Initialized listener.
 * @param[in] enable TRUE sets the flag, FALSE clears it.
 * @warning The current accept implementation does not read this field. This
 * macro alone does not request or require a client certificate. */
#define HttpSharkSslServCon_requestClientCert(o,enable) \
        (o)->requestClientCert=enable
/** Select the RSA certificate preference applied to newly accepted connections.
 * @param[in,out] o Initialized listener.
 * @param[in] enable TRUE favors RSA, FALSE keeps the default selection.
 * Existing TLS connections are unchanged. Set while holding the dispatcher mutex.
 * Availability and selection details depend on the configured TLS engine. */
#define HttpSharkSslServCon_favorRSA(o,enable) \
        (o)->favorRSA=enable

/** Close the listener and every accepted TLS connection still tracked by it.
 * @param[in,out] o Initialized listener; no borrowed dependency is freed. */
SHARKSSL_API void HttpSharkSslServCon_destructor(HttpSharkSslServCon* o);

SHARKSSL_API int HttpSharkSslServCon_bindExec(
   SoDispCon* con, SharkSsl* ssl,const char* alpn,const char* host,int port);

#ifdef __cplusplus
}
inline HttpSharkSslServCon::HttpSharkSslServCon(
   SharkSsl* sharkSsl,
   HttpServer* server,
   SoDisp* dispatcher,
   U16 port,
   bool setIP6,
   const void* interfaceName,
   HttpServCon_AcceptNewCon userDefinedAccept) {
   HttpSharkSslServCon_constructor(
      this,sharkSsl,server,dispatcher,
      port,setIP6?TRUE:FALSE,
      interfaceName,userDefinedAccept); }
inline BaBool HttpSharkSslServCon::isValid() {
   return HttpServCon_isValid(this); }
inline int HttpSharkSslServCon::setPort(
   U16 portNumber,bool setIp6,const void* interfaceName) {
   return HttpSharkSslServCon_setPort(
      this,portNumber,setIp6?TRUE:FALSE,interfaceName);
}

inline HttpSharkSslServCon::~HttpSharkSslServCon() {
   HttpSharkSslServCon_destructor(this);}
#endif

#endif
