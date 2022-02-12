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
 *   $Id: WebSocketServer.h 5079 2022-02-11 20:41:10Z wini $
 *
 *   COPYRIGHT:  Real Time Logic LLC, 2015 - 2022
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

#ifndef _WebSocketServer_h
#define _WebSocketServer_h

#include <DynBuffer.h>


/** @defgroup WebSockets WebSockets 

    The Lua socket library (including WebSockets) provides three
    modes: blocking, asynchronous, and non blocking (cosockets). The C
    WebSocket Server library provides one mode, the asynchronous
    mode. Asynchronous sockets means that receiving data is event
    driven and sending data is blocking (socket is in blocking
    mode). The concept of using asynchronous sockets is the same for
    Lua and C code thus
    <a href="../../../lua/SockLib.html#AsynchronousSockets">consult the Lua documentation</a>
    for an introduction to asynchronous sockets.

    The library is limited to sending and receiving data less to or
    equal 0xFFFF.

    ### Example:

    The Barracuda App Server SDK includes one example using this
     WebSocket library and a copy of the example
    [is available on GitHub](https://github.com/RealTimeLogic/BAS/blob/main/examples/C-WebSockets/README.md)

    SDK example dir: examples/WebSocket-Chat/

 *  @{
 */



struct WSS;
struct WSSCB;

/** WebSocket callback: a WebSocket frame is received.
    \param o the WSSCB interface instance.
    \param wss the WebSocket server instance.
    \param data the data received. The data is conveniently zero
    terminated for strings.
    \param len data length.
    \param text a boolean value set to TRUE for text frames and FALSE
    for binary frames.
 */
typedef void (*WSSCB_Frame)(
   struct WSSCB* o,struct WSS* wss,void* data,int len,int text);


/** Optional webSocket callback: the function is called when the
    client sends a ping message. A Pong message is sent automatically.
    \param o the WSSCB interface instance.
    \param wss the WebSocket server instance.
    \param data a ping message may include payload data.
    \param len data length.
*/
typedef void (*WSSCB_Ping)(struct WSSCB* o,struct WSS* wss,void* data,int len);


/** WebSocket callback: the client closed the connection or the
    connection closed unexpectedly.
    \param o the WSSCB interface instance.
    \param wss the WebSocket server instance.
    \param status. A positive value means that the client sent a
    graceful close message to the server. The value 1000 and 1001
    indicates normal closure. Any other value sent by the client
    indicates an error condition. See RFC6455:7.4.1 for details. A
    negative error means the socket connection unexpectedly dropped.
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
       \param frameFp called when a complete frame has been received.
       \param closeFp called when connection closes.
       \param pingFp called when client sends a ping message (optional).
    */
   WSSCB(WSSCB_Frame frameFp, WSSCB_Close closeFp, WSSCB_Ping pingFp=0);
#endif 
   WSSCB_Frame frameFp;
   WSSCB_Close closeFp;
   WSSCB_Ping pingFp;
} WSSCB;


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
   /** Create and initialize a WebSocket Server instance.
      \param cb your callback interface
      \param disp the server's socket dispatcher
      \param startSize received data is buffered internally until a
      complete WebSocket frame is received. This data is buffered in a
      #DynBuffer. The startSize and expandSize is used by the DynBuffer
      and enables a frame to dynamically grow from startSize and then
      at intervals of expandSize.
      \param expandSize see startSize for information.
    */
   WSS(WSSCB* cb, SoDisp* disp, int startSize, int expandSize);

   /** destructor.
    */
   ~WSS();

   /** Upgrades an HTTP server request to a persistent WebSocket
       connection. This method is typically called from within an
       HttpPage or HttpDir service function. The function performs a
       WebSocket handshake by calling HttpRequest::wsUpgrade. The
       function then calls WSS::connect if the WebSocket handshake was
       successful.
       \param req method HttpRequest::getConnection returns the
       connection object used as a parameter for this method.
       \returns 0 on success and a negative value on error.
    */
   int upgrade(HttpRequest* req);

   /** Upgrades (morphs) an HTTP request to a persistent WebSocket
       connection. You must complete the WebSocket handshake by calling
       HttpRequest::wsUpgrade prior to calling this method.

       \returns 0 on success and a negative value on error.
    */
   int connect(HttpConnection* con);

   /** Write/send a WebSocket frame.
       \param data the data to send
       \param len data length
       \param isTxt set to true for text frames and false for binary frames.
   */
   int write(const void* data, int len, bool isTxt);

   /** Gracefully close the WebSocket connection by sending WebSocket
       status code N to the client prior to closing the active socket
       connection.
       See https://tools.ietf.org/html/rfc6455#section-7.4
    */
   int close(int statusCode=1000);

      /** Returns true if the WebSocket connection is valid.
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
BA_API void WSS_constructor(
   WSS* o, WSSCB* cb, SoDisp* disp, int startSize, int expandSize);
BA_API void WSS_destructor(WSS* o);
BA_API int WSS_upgrade(WSS* o, HttpRequest* req);
BA_API int WSS_connect(WSS* o, HttpConnection* con);
BA_API int WSS_rawWrite(WSS* o, const void* data, int len, int opCode);
#define WSS_write(o, data, len, isTxt) WSS_rawWrite(o, data, len, isTxt?1:2)
BA_API int WSS_close(WSS* o, int statusCode);
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
