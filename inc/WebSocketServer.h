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
 *   $Id: WebSocketServer.h 5978 2026-09-11 16:13:48Z wini $
 *
 *   COPYRIGHT:  Real Time Logic LLC, 2015 - 2023
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

/** @file WebSocketServer.h */

#ifndef _WebSocketServer_h
#define _WebSocketServer_h

#include <DynBuffer.h>


/** @defgroup WebSockets WebSockets 

    The Lua socket library (including WebSockets) provides three
    modes: blocking, asynchronous, and non-blocking (cosockets). The C
    WebSocket Server library provides one mode, the asynchronous
    mode. Asynchronous sockets means that receiving data is event
    driven and sending data is blocking (socket is in blocking
    mode). The concept of using asynchronous sockets is the same for
    Lua and C code thus
    <a href="../../../lua/SockLib.html#AsynchronousSockets">consult the Lua documentation</a>
    for an introduction to asynchronous sockets.

    Each frame payload is limited to 65535 bytes. Fragmented messages are not
    supported. Client frames must be masked. Hold the dispatcher mutex when
    using this API from application threads; callbacks run under that mutex.

    ### Example:

    The Barracuda App Server SDK includes one example using this
     WebSocket library and a copy of the example
    [is available on GitHub](https://github.com/RealTimeLogic/BAS/tree/main/examples/C-WebSockets)

    SDK example directory: examples/C-WebSockets/

 *  @{
 */



struct WSS;
struct WSSCB;

/** Receive a complete unfragmented text or binary frame.
    @param o Required borrowed callback interface.
    @param wss Connection delivering this callback.
    @param data Borrowed payload with a temporary trailing NUL, including for
    binary frames. Embedded NUL bytes are possible; use len.
    @param len Payload byte count, 0 through 65535, excluding the added NUL.
    @param text TRUE for text, FALSE for binary. The parser does not validate UTF-8.
    Copy data needed after return. Do not destroy wss or reuse its receive buffer
    from this callback; the parser continues using them afterward.
 */
typedef void (*WSSCB_Frame)(
   struct WSSCB* o,struct WSS* wss,void* data,int len,int text);


/** Optional notification after the internal pong send attempt succeeds.
    @param o Required borrowed callback interface.
    @param wss Connection delivering this callback.
    @param data Borrowed ping payload, or NULL for an empty ping. Not guaranteed
    NUL-terminated. Copy data needed after return.
    @param len Payload byte count, 0 through 125.
    Do not destroy wss or reuse its receive buffer during this callback.
    @warning The current automatic pong path does not correctly reproduce a
    nonempty ping payload. This implementation limitation is pending source repair.
 */
typedef void (*WSSCB_Ping)(struct WSSCB* o,struct WSS* wss,void* data,int len);


/** Notify closure after the underlying connection has been closed.
    @param o Required borrowed callback interface.
    @param wss Closed WebSocket object. Its storage still belongs to the application;
    this callback may arrange destruction/cleanup.
    @param status Peer close code when supplied, zero for a close without a usable
    code, a locally generated protocol close code, or a negative transport/allocation
    error. A positive value is not proof that the peer initiated a graceful close.
    Explicit WSS::close() and destruction do not invoke this callback.
 */
typedef void (*WSSCB_Close)(struct WSSCB* o,struct WSS* wss,int status);

/** WebSocket Server Connection Callback Interface: provides an
    interface between your application and the WebSocket server. The
    WebSocket Server connection calls the functions in this interface
    on "receive" events.
 */
typedef struct WSSCB
{
#ifdef __cplusplus
   /** Provide your callback event functions.
       \param frameFp Required callback for a complete text/binary frame.
       \param closeFp Required closure callback.
       \param pingFp Optional ping callback; NULL disables notification.
    */
   WSSCB(WSSCB_Frame frameFp, WSSCB_Close closeFp, WSSCB_Ping pingFp=0);
#endif 
   WSSCB_Frame frameFp;
   WSSCB_Close closeFp;
   WSSCB_Ping pingFp;
} WSSCB;


/** Initialize callback storage.
    @param o Required writable interface.
    @param frame Required WSSCB_Frame callback.
    @param close Required WSSCB_Close callback.
    @param ping Optional WSSCB_Ping callback, or NULL.
 */
#define WSSCB_constructor(o, frame, close, ping) \
   (o)->frameFp=frame,(o)->closeFp=close,(o)->pingFp=ping

#ifdef __cplusplus
inline WSSCB::WSSCB(
   WSSCB_Frame frameFp, WSSCB_Close closeFp, WSSCB_Ping pingFp) {
   WSSCB_constructor(this, frameFp, closeFp, pingFp);
}
#endif 


/** WebSocket Server (WSS)
 */
typedef struct WSS
{
#ifdef __cplusplus
   /** Initialize an unconnected WebSocket object.
    @param cb Required borrowed interface with non-NULL frame and close callbacks.
    @param disp Required borrowed dispatcher. Both dependencies must outlive wss.
    @param startSize Positive initial receive-buffer capacity in bytes.
    @param expandSize Positive growth increment in bytes; raised to startSize if
    smaller. The buffer grows to retain an entire frame before invoking frameFp.
    Storage is allocated as input arrives. Receive allocation failure reports
    E_MALLOC through closeFp.
 */
   WSS(WSSCB* cb, SoDisp* disp, int startSize, int expandSize);

   /** Close the transport and free receive storage, without a WebSocket close
    handshake or close callback. Stop other users first; borrowed cb/disp remain alive.
 */
   ~WSS();

   /** Perform the server handshake and take over the request's connection.
    @param req Required current uncommitted WebSocket upgrade request.
    @return Zero on successful handoff, -1 on handshake or invalid-connection
    failure. A rejected handshake attempts an HTTP 400 response. On success,
    further I/O belongs to WSS; do not continue ordinary HTTP response output.
 */
   int upgrade(HttpRequest* req);

   /** Take an HTTP connection after its WebSocket handshake is complete.
    @param con Required live connection to move into this object. Ownership of
    the socket transfers, leaving con without it. An existing WSS socket is closed.
    @return Zero on success, -1 if con has no valid socket. This call does not
    perform or verify the handshake; use upgrade() for normal request handling.
 */
   int connect(HttpConnection* con);

   /** Send one complete, unmasked server frame using blocking transport writes.
    @param data Readable payload buffer for this call; required for positive len.
    @param len Payload byte count, 0 through 65535. The caller must enforce this range.
    @param isTxt True for UTF-8 text supplied by the application; false for binary.
    @return Zero when sent, negative on transport failure. No partial byte count
    is returned. This call does not itself deliver the receive-side close callback.
 */
   int write(const void* data, int len, bool isTxt);

   /** Attempt to send a close frame, then close the transport immediately.
    @param statusCode WebSocket close status valid for transmission; defaults to
    1000. Nonpositive values are also replaced with 1000. The caller supplies a
    valid 16-bit protocol value; this function does not validate it.
    @return Zero if the socket was valid and is now closed, -1 if already invalid.
    Zero does not confirm that the close frame was sent or acknowledged; send
    errors are not returned. The close callback is not invoked.
 */
   int close(int statusCode=1000);

      /** Check the local transport handle.
    @return True while a socket is locally valid, false after it is closed.
    True does not prove that the peer is still reachable or that a write will succeed.
 */
      bool isValid();

#endif 
   SoDispCon super;
   DynBuffer db;
   WSSCB* cb;
   int endOfPacketIx;
} WSS;


#ifdef __cplusplus
extern "C" {
#endif
/** @copydoc WSS::WSS
    @param o Required storage to initialize.
 */
BA_API void WSS_constructor(
   WSS* o, WSSCB* cb, SoDisp* disp, int startSize, int expandSize);
/** @copydoc WSS::~WSS
    @param o Required initialized WebSocket.
 */
BA_API void WSS_destructor(WSS* o);
/** @copydoc WSS::upgrade
    @param o Required initialized WebSocket.
 */
BA_API int WSS_upgrade(WSS* o, HttpRequest* req);
/** @copydoc WSS::connect
    @param o Required initialized WebSocket.
 */
BA_API int WSS_connect(WSS* o, HttpConnection* con);
/** Send one final server frame with a caller-selected opcode.
    @param o Required connected WebSocket.
    @param data Readable payload buffer, required when len is positive.
    @param len Byte count, 0 through 65535 for text/binary or 0 through 125 for
    control frames. The caller validates length and payload semantics.
    @param opCode Valid WebSocket opcode, normally 1 text, 2 binary, 8 close,
    9 ping, or 10 pong. The FIN bit is always set; no mask is added.
    @return Zero on success, negative transport error. No partial count is returned.
    This low-level call performs no opcode, length, or UTF-8 validation.
 */
BA_API int WSS_rawWrite(WSS* o, const void* data, int len, int opCode);
/** @copydoc WSS::write
    @param o Required connected WebSocket.
 */
#define WSS_write(o, data, len, isTxt) WSS_rawWrite(o, data, len, isTxt?1:2)
/** @copydoc WSS::close
    @param o Required initialized WebSocket.
 */
BA_API int WSS_close(WSS* o, int statusCode);
/** @copydoc WSS::isValid
    @param o Required initialized WebSocket.
 */
#define WSS_isValid(o) SoDispCon_isValid((SoDispCon*)o)
#ifdef __cplusplus
}

inline WSS::WSS(WSSCB* cb, SoDisp* disp, int startSize, int expandSize) {
   WSS_constructor(this, cb, disp, startSize, expandSize);
}
inline WSS::~WSS() {
   WSS_destructor(this);
}
inline int WSS::upgrade(HttpRequest* req) {
   return WSS_upgrade(this, req);
}
inline int WSS::connect(HttpConnection* con) {
   return WSS_connect(this, con);
}
inline int WSS::write(const void* data, int len, bool isTxt) {
   return WSS_write(this, data, len,isTxt);
}
inline int WSS::close(int statusCode) {
   return  WSS_close(this, statusCode);
}
inline bool WSS::isValid() {
	return WSS_isValid(this) ? true : false; }
#endif


/** @} */ /* end of WebSockets */

#endif
