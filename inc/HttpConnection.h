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
 *   $Id: HttpConnection.h 5978 2026-09-11 16:13:48Z wini $
 *
 *   COPYRIGHT:  Real Time Logic LLC, 2003 - 2017
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

/** @file HttpConnection.h */

#ifndef __HttpConnection_h
#define __HttpConnection_h

#include <SoDispCon.h>

struct HttpServer;
struct HttpConnection;


/*===========================================================================
 *
 *       HttpConnection
 *---------------------------------------------------------------------------
 */


/** Server connection lifecycle states. Use setState for transitions that
 * must release buffered bytes or terminate a socket. */
typedef enum 
{ 
   HttpConnection_Free=0,        /* Resource not in use */
   HttpConnection_Connected,   /* connected, but no data */
   HttpConnection_Ready,       /* Data on socket */
   HttpConnection_Running,     /* Executing HTTP request/response */
   HttpConnection_Moved,       /* Moved to EventHandler */
   HttpConnection_Terminated,   /* Temporary state. Failed reading/writing */
   /* Not a valid state, but can be used in method HttpConnection_setState to
      do a hard (abortive) socket close. */
   HttpConnection_HardClose
} HttpConnection_State;



/** Contains information about the physical socket connection. This
    object is used internally by HttpServer when sending and receiving
    data on either an insecure or secure SSL connection.
 */
#ifdef __cplusplus
typedef struct HttpConnection : public SoDispCon
{
      void *operator new(size_t, void *place) { return place; }
      void operator delete(void*, void *) { }
      /** Uninitialized storage; use HttpConnection_constructor before use. */
      HttpConnection() {}

      /** @return Borrowed associated server, possibly NULL for a non-HTTP use.
       */
      struct HttpServer* getServer();

      /** Enable persistent connection reuse after a completed HTTP request. */
      void setKeepAlive();

      /** Disable persistent connection reuse after the request completes.
       * Does not immediately close the socket.
       */
      void clearKeepAlive();
#else
typedef struct HttpConnection
{
      SoDispCon super;
#if 0
}
#endif
#endif
      struct HttpServer* server;
      struct HttpCommand* cmd;
      void* pushBackData;
      int pushBackDataSize;
      U8 state; /* HttpConnection_State */
      U8 keepAlive;
} HttpConnection;

/** Enable persistent reuse after a completed HTTP request.
 * @param[in,out] o Initialized connection. */
#define HttpConnection_setKeepAlive(o) (o)->keepAlive=TRUE
/** Disable persistent reuse without immediately closing the socket.
 * @param[in,out] o Initialized connection. */
#define HttpConnection_clearKeepAlive(o) (o)->keepAlive=FALSE
/** @param[in] o Initialized connection.
 * @return TRUE if reuse is enabled, FALSE otherwise. */
#define HttpConnection_keepAlive(o) (o)->keepAlive
/** @param[in] o Initialized connection.
 * @return Current HttpConnection_State value. */
#define HttpConnection_getState(o) (o)->state
/** @param[in] o Initialized connection.
 * @return Borrowed associated server, possibly NULL. */
#define HttpConnection_getServer(o) (o)->server
/** @param[in] o Initialized connection.
 * @return Borrowed associated dispatcher, possibly NULL. */
#define HttpConnection_getDispatcher(o) SoDispCon_getDispatcher((SoDispCon*)o)
#define HttpConnection_dispRecEvent(o) SoDispCon_dispRecEvent((SoDispCon*)o)
#define HttpConnection_newConnectionIsReady(con) \
  SoDispCon_newConnectionIsReady(con)

#ifdef __cplusplus
extern "C" {
#endif
/** Initialize connection storage without opening a socket.
 * @param[out] o Caller-owned storage; initially Free and not keep-alive.
 * @param[in] server Borrowed server, or NULL for non-HTTP use.
 * @param[in] dispatcher Borrowed dispatcher, valid throughout registered use.
 * @param[in] e Receive-event callback; required before enabling receive events.
 * Establish or move in a socket separately before registering events. */
BA_API void HttpConnection_constructor(HttpConnection* o,
                                       struct HttpServer* server,
                                       struct SoDisp* dispatcher,
                                       SoDispCon_DispRecEv e);
/** Change lifecycle state, performing the associated cleanup.
 * @param[in,out] o Initialized connection; serialize with its dispatcher.
 * @param[in] state A HttpConnection_State value. Free/Terminated release
 * pushback storage and shut down the socket. HardClose performs an abortive
 * close and stores Terminated. Other states are intended for server lifecycle
 * handling; setting Running alone does not create a usable socket. */
BA_API void HttpConnection_setState(
   HttpConnection* o, HttpConnection_State state);
/** Move the socket, transport state and pending input to another object.
 * @param[in,out] o Live source associated with the same server as newCon.
 * Its socket and pushback ownership are removed and its state becomes Moved.
 * @param[in,out] newCon Initialized empty destination with the appropriate
 * dispatcher/callback setup. It must not own a socket or pending input.
 * @return Zero. Register and activate the destination as required by its owner;
 * the source object itself is not freed. This is not a copy operation. */
BA_API int HttpConnection_moveCon(HttpConnection* o, HttpConnection*  newCon);
int HttpConnection_sendChunkData6bOffs(
   HttpConnection* o,const void* data,int len);
/** Release pushback storage, unregister events, and close the connection.
 * @param[in,out] o Initialized object; its storage and borrowed server are not
 * freed. Stop other users before destruction. */
BA_API void HttpConnection_destructor(HttpConnection* o);
/** Prepend copied bytes to the input returned by subsequent reads.
 * @param[in,out] o Initialized connection.
 * @param[in] d Borrowed source containing s bytes.
 * @param[in] s Positive byte count; combined pending length must fit int.
 * @return Zero when pushback storage exists, -1 if an initial allocation fails.
 * @warning When older pushback data exists, an allocation failure while
 * prepending can return zero without adding the new bytes. Do not assume this
 * return proves that an additional prepend succeeded. */
BA_API int HttpConnection_pushBack(HttpConnection* o, const void* d, int s);
/** @copydoc SoDispCon_isSecure */
#define HttpConnection_isSecure(o)\
   SoDispCon_isSecure((SoDispCon*)o)
/** @copydoc SoDispCon_isValid */
#define HttpConnection_isValid(o)\
   SoDispCon_isValid((SoDispCon*)o)
/** @copydoc SoDispCon_sendData */
#define HttpConnection_sendData(o, data, len)\
   SoDispCon_sendData((SoDispCon*)o, data, len)
/** @copydoc SoDispCon_sendDataNT */
#define HttpConnection_sendDataNT(o, data, len)\
   SoDispCon_sendDataNT((SoDispCon*)o, data, len)
/** @copydoc SoDispCon_sendChunkData */
#define HttpConnection_sendChunkData(o,data, len)\
   SoDispCon_sendChunkData((SoDispCon*)o,data, len)
/** Obtain transport-owned asynchronous send storage.
 * @param[in,out] o Connected object.
 * @param[in,out] sizePtr Positive requested capacity; receives actual capacity.
 * @return Borrowed buffer or NULL. See SoDispCon::allocAsynchBuf for lifetime. */
#define HttpConnection_allocAsynchBuf(o, sizePtr)\
   SoDispCon_allocAsynchBuf((SoDispCon*)o, sizePtr)
/** @copydoc SoDispCon_asyncSend */
#define HttpConnection_asyncSend(o,len) SoDispCon_asyncSend((SoDispCon*)o,len)
/** @copydoc SoDispCon_asyncReady */
#define HttpConnection_asyncReady(o) SoDispCon_asyncReady((SoDispCon*)o)
/** Read pending pushback bytes first, otherwise perform an event-oriented read.
 * @param[in,out] con Initialized connection.
 * @param[out] data Writable storage of at least len bytes, not NUL-terminated.
 * @param[in] len Positive capacity in bytes.
 * @return Positive byte count, zero when no data is currently available, or a
 * negative transport error. A return may be short even with more bytes pending.
 * Readiness and mutex requirements follow SoDispCon_readData. */
BA_API int HttpConnection_readData(HttpConnection* con, void* data, int len);
/** Read buffered bytes or wait for socket data.
 * @param[in,out] con Initialized connection configured for blocking reads.
 * @param[out] data Writable storage of at least len bytes, not NUL-terminated.
 * @param[in] len Positive capacity in bytes.
 * @return Positive byte count, zero if the transport supplies no bytes, or a
 * negative transport/timeout error. Does not promise to fill the buffer.
 * Pushback bytes are returned immediately; otherwise uses SoDispCon_blockRead.
 * Do not run competing reads on the same connection. */
BA_API int HttpConnection_blockRead(HttpConnection* con, void* data, int len);
/** @copydoc SoDispCon_setNonblocking */
#define HttpConnection_setNonblocking(o)\
   SoDispCon_setNonblocking((SoDispCon*)o)
/** @copydoc SoDispCon_setBlocking */
#define HttpConnection_setBlocking(o)\
   SoDispCon_setBlocking((SoDispCon*)o)
#define HttpConnection_peek(o)\
   SoDispCon_peek((SoDispCon*)o)
/** @copydoc SoDispCon_setTCPNoDelay */
#define HttpConnection_setTCPNoDelay(o, enable)\
   SoDispCon_setTCPNoDelay((SoDispCon*)o, enable)
/** @copydoc SoDispCon_getPeerName */
#define HttpConnection_getPeerName(o, addr,port)\
   SoDispCon_getPeerName((SoDispCon*)o, addr,port)
/** @copydoc SoDispCon_getSockName */
#define HttpConnection_getSockName(o, addr,port)\
   SoDispCon_getSockName((SoDispCon*)o, addr,port)
/** @copydoc SoDispCon_addr2String */
#define HttpConnection_addr2String(o, addr, buf, len)\
   SoDispCon_addr2String((SoDispCon*)o, addr, buf, len)
/** @copydoc SoDispCon_cmpAddr */
#define HttpConnection_cmpAddr(o, addr2)\
   SoDispCon_cmpAddr((SoDispCon*)o, addr2)
/** @copydoc SoDispCon_recEvActive */
#define HttpConnection_recEvActive(o)\
   SoDispCon_recEvActive((SoDispCon*)o)
/** @copydoc SoDispCon_sendEvActive */
#define HttpConnection_sendEvActive(o)\
   SoDispCon_sendEvActive((SoDispCon*)o)
/** @param[in] o Initialized connection.
 * @return TRUE if pushback bytes or transport readiness are recorded; not a byte count. */
#define HttpConnection_hasMoreData(o)\
   (SoDispCon_hasMoreData((SoDispCon*)o) || (o)->pushBackData)
/** @copydoc SoDispCon_dispatcherHasCon */
#define HttpConnection_dispatcherHasCon(o)\
   SoDispCon_dispatcherHasCon((SoDispCon*)o)
/** @copydoc SoDispCon_setDispSendEvent */
#define HttpConnection_setDispSendEvent(o, dispSendEvFp)\
   SoDispCon_setDispSendEvent((SoDispCon*)o, dispSendEvFp)
#ifdef __cplusplus
}
inline void HttpConnection::setKeepAlive() {
   HttpConnection_setKeepAlive(this);
}
inline void HttpConnection::clearKeepAlive() {
   HttpConnection_clearKeepAlive(this);
}
#endif


#endif
