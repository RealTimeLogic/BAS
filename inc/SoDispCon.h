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
 *   $Id: SoDispCon.h 5978 2026-09-11 16:13:48Z wini $
 *
 *   COPYRIGHT:  Real Time Logic LLC, 2006 - 2026
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

/** @file SoDispCon.h */

#ifndef __SoDispCon_h
#define __SoDispCon_h

#include <ThreadLib.h>
#include <SoDisp.h>

struct SharkSslCon;

/*===========================================================================
 *
 *       SoDispCon
 *---------------------------------------------------------------------------
 */



/* SoDispCon_ExType -> SoDispCon Execute Type.
   The callback function SoDispCon_Execute in SoDispCon is
   like having 6 callback functions. SoDispCon_ExType specifies the
   current callback type.
   This construction preserves memory as we have 1 callback function pointer
   instead of 5.

   ExStateRead:     read socket data
   ExStateWrite:    write socket data
   GetSharkSslCon: return the internal SharkSsslCon object
   ExStateClose:    Close connection.
   ExStateMoveCon:  Moving the active connection to another SoDispCon obj.
   ExStateDispEv:   The socket dispatcher signals that data is ready.
*/
typedef enum {
   SoDispCon_ExTypeRead=1,
   SoDispCon_ExTypeWrite,
   SoDispCon_GetSharkSslCon,
   SoDispCon_ExTypeClose,
   SoDispCon_ExTypeMoveCon,
   SoDispCon_ExTypeAllocAsynchBuf,
   SoDispCon_ExTypeAsyncReady,
   SoDispCon_ExTypeIdle
} SoDispCon_ExType;


/*
The arguments to the SoDispCon_Execute callback function:
   SoDispCon_ExTypeRead:
      d1 pointer to data, d2 length of data, ret #of bytes read, < 0 if failed
      or 0 if no data.
   SoDispCon_ExTypeWrite:
      d1 pointer to data, d2 length, ret < 0 if failed or #of bytes written.
   SoDispCon_GetSharkSslCon:
      d1 is  pointer pointer to SharkSslCon, d2 not used, ret TRUE of FALSE. 
   SoDispCon_ExTypeClose:
      d1 not used, d2 not used, ret is void.
   SoDispCon_ExTypeMoveCon:
      d1 is a pointer to a SoDispCon object.
*/
 
struct SoDispCon;
struct SharkSsl;

typedef int (*SoDispCon_Execute)(
   struct SoDispCon* con, ThreadMutex* m,SoDispCon_ExType s,void* d1,int d2);


/** Dispatcher receive notification, invoked with its mutex held.
 * @param[in,out] con Borrowed registered connection. Check operation results;
 * readiness does not guarantee successful I/O. Stop events before freeing it. */
typedef void (*SoDispCon_DispRecEv)(struct SoDispCon* con);
/** Dispatcher send-ready notification, invoked with its mutex held.
 * @param[in,out] con Borrowed registered connection. Check operation results;
 * readiness does not guarantee successful I/O. Stop events before freeing it. */
typedef void (*SoDispCon_DispSendEv)(struct SoDispCon* con);


/** Contains information about the physical socket connection.  This
    object is used internally by HttpServer when sending and receiving
    data on either an insecure or secure SSL connection.
 */
typedef struct SoDispCon
{
#ifdef __cplusplus
   void *operator new(size_t, void *place) { return place; }
   void operator delete(void*, void *) { }
   /** Uninitialized storage; call SoDispCon_constructor before use. */
   SoDispCon() {}


      /** Connect an initialized, empty connection to a remote endpoint.
 * @param[in] host Required NUL-terminated hostname or numeric IP address.
 * @param[in] port Remote TCP/UDP port in host byte order, 1..65535.
 * @param[in] bindIntfName Optional borrowed platform local interface/address
 * used during the call; NULL selects the default.
 * @param[in] bindPort Local port in host byte order; zero lets the OS choose.
 * @param[in] timeout Positive connect timeout in milliseconds, default 1500.
 * The legacy resolver path ignores this value; USE_ADDRINFO applies it to
 * individual address attempts, not the entire DNS-and-connect operation.
 * @param[in] dgram TRUE selects UDP, FALSE TCP (default).
 * @param[in] ipv6 TRUE requests IPv6, FALSE IPv4 (default); resolution depends
 * on the selected network port.
 * @param[out] errinfo Optional output for borrowed platform resolver text;
 * initialized to NULL. Do not free it or rely on text as a stable error code.
 * @return Zero on success, negative socket/resolver error on failure.
 * The owned dispatcher mutex is released around resolution/connect and acquired
 * again before return. Do not use the connection concurrently during this call.
 * Failure can require cleanup; no socket ownership is returned to the caller. */


   int connect(const char* host,
               U16 port,
               const void* bindIntfName=0,
               U16 bindPort=0,
               U32 timeout=1500,
               BaBool dgram=false, BaBool ipv6=false,
               char** errinfo=0);

     /** Deprecated: Use getSharkSslCon(NULL).
      */
     bool isSecure();

      /** Inspect a connected transport's TLS implementation.
 * @param[out] sc Optional output for a borrowed SharkSslCon pointer. Use it
 * only when the call reports a SharkSSL connection; no ownership is transferred.
 * NULL requests a secure-transport test, which can also succeed for OpenSSL.
 * @return True for the requested TLS test, false when unsupported. Call only
 * after a transport has been successfully installed; this is not a constructor
 * success test. A returned TLS pointer is invalidated by close or transfer. */

     bool getSharkSslCon(SharkSslCon** sc);


      /** @return True if a socket handle is installed, false otherwise.
       * This does not establish peer liveness or TLS handshake completion.
       */
      bool isValid();

      /** @return True for a connection recorded as IPv6, false for IPv4. */
      bool isIP6();

      /** Read the remote peer endpoint.
 * @param[out] addr Required address output, valid only on success.
 * @param[out] port Optional port output in host byte order; NULL omits it.
 * @return Zero on success, nonzero platform socket status on failure. */

      int getPeerName(HttpSockaddr* addr,U16* port=0);

      /** Read the local socket endpoint.
 * @param[out] addr Required address output, valid only on success.
 * @param[out] port Optional port output in host byte order; NULL omits it.
 * @return Zero on success, nonzero platform socket status on failure. */

      int getSockName(HttpSockaddr* addr,U16* port=0);

      /** Format a numeric address without a port number.
 * @param[in] addr Required address with the same family as this connection.
 * @param[out] buf Required writable storage for a NUL-terminated result.
 * @param[in] len Capacity in bytes including NUL; at least 16 for IPv4 or
 * 46 for IPv6. Always provide writable storage even if capacity is rejected.
 * @return buf on success, NULL on invalid storage, family mismatch or conversion
 * failure. No allocation is performed; do not use the output after failure. */

      char* addr2String(HttpSockaddr* addr, char* buf, int len);

      /** Compare an address with the current peer address, ignoring port numbers.
 * @param[in] addr2 Required address to compare; IPv4-mapped IPv6 is recognized.
 * @return True if equal, false if different or the peer address lookup fails. */

      bool cmpAddr(HttpSockaddr* addr2);

      /** Disable the TCP delay.
       * What TCP normally does is queue up data so it only sends full
       * packets out when possible. (This is called Nagle's algorithm.)
       * Call this function if you do not want that behavior.
       * @param[in] enable True disables Nagle; false enables it.
       * No socket-option error status is returned; errors can be traced.
       */
      void setTCPNoDelay(bool enable);
      
      /** @return Borrowed associated dispatcher, possibly NULL.
       */
      struct SoDisp* getDispatcher();

      /** @return Recorded receive readiness, not a byte count or a live socket probe.
       */
      bool hasMoreData();

      /** @return True if the dispatcher registration flag is set.
       */
      bool dispatcherHasCon();

      /** @return True if receive events are enabled, false otherwise.
       */
      bool recEvActive();

      /** @return True if send-ready events are enabled, false otherwise.
       */
      bool sendEvActive();

      /** Install a send-ready callback before enabling send events.
 * @param[in] ev Required callback while send events are active; borrowed. */
      void setDispSendEvent(SoDispCon_DispSendEv ev);
      /** Install a receive callback before enabling receive events.
 * @param[in] ev Required callback while receive events are active; borrowed. */
      void setDispRecEvent(SoDispCon_DispRecEv ev);
      /** Read through the installed transport using its recorded readiness.
 * @param[out] data Required writable buffer; returned bytes are not NUL-terminated.
 * @param[in] len Positive capacity in bytes.
 * @param[in] relmutex True allows the transport to release the owned dispatcher
 * mutex while waiting, then reacquire it; false (default) keeps it held.
 * @return Positive byte count (possibly short), zero for no data, or negative
 * transport/timeout error. Do not run concurrent reads on one connection. */
      int readData(void* data, int len, bool relmutex=false);
      /** Select nonblocking socket mode.
 * @return Zero success, -1 if already nonblocking, or a platform failure status.
 * The recorded mode changes only when the platform operation succeeds. */
      int setNonblocking();
      /** Select blocking socket mode.
 * @return Zero success, -1 if already blocking, or a platform failure status. */
      int setBlocking();
      /** Send an entire buffer through the installed transport.
 * @param[in] data Borrowed source containing len readable bytes.
 * @param[in] len Nonnegative byte count.
 * @return Zero only if the transport reports the complete byte count;
 * -1 for an invalid socket, E_SOCKET_WRITE_FAILED for a failed or short send.
 * No partial count is supplied. May release/reacquire the dispatcher mutex.
 * Use the async buffer API for nonblocking partial-send handling. */
      int sendData(const void* data, int len);
      /** Begin or advance a send from the transport's asynchronous buffer.
 * @param[in] len Positive payload byte count within the capacity returned by
 * allocAsynchBuf for a new send. Do not alter the buffer until completion;
 * use asyncReady to continue a pending send.
 * @return 1 complete, 0 pending, negative transport error. For a plain socket
 * with no asynchronous buffer, the implementation returns 1 without sending;
 * always allocate and check the buffer before use. */
      int asyncSend(int len);
      /** Send one HTTP chunk, including its hexadecimal size and CRLF delimiters.
 * @param[in] data Borrowed source containing len readable bytes.
 * @param[in] len Byte count from 0 through 65535. Zero sends the final empty chunk.
 * @return Zero on success, -1 on header, payload or delimiter send failure.
 * Partial wire output can already exist on failure. */
      int sendChunkData(const void* data, int len);
      /** Advance pending output and inspect completion.
 * @return The current C++ wrapper converts the result to 0 or 1, so both
 * completion and a negative error become 1. Use SoDispCon_asyncReady or
 * SoDispCon_asyncReadyF when error handling must distinguish these cases. */
      int asyncReady();
      /** Obtain transport-owned writable storage for asynchronous sending.
 * @param[in,out] size Required positive requested capacity in bytes; receives
 * the actual capacity, which may differ. Always use the returned capacity.
 * @return Borrowed writable storage, or NULL if allocation fails. Requires a
 * valid connected transport. Do not free it; close or transport buffer release
 * invalidates it. Do not reallocate while an earlier send is pending. */
      void* allocAsynchBuf(int* size);
#endif
      CONNECTION_DISPATCHER_OBJ /*Owned and handled by the SoDisp obj.*/

      /* Owned and handled by SSL. */
      void* sslData;
      SoDispCon_Execute exec;

      HttpSocket httpSocket;
      SoDispCon_DispRecEv dispRecEv;
      SoDispCon_DispSendEv dispSendEv;
      struct SoDisp* dispatcher;
      BaBool* sendTermPtr;
      BaBool* recTermPtr;
      U16 rtmo; /* Read timeout in 50 millisec intervals */
      U8 dataBits;
      U8 isSending;
} SoDispCon;

#define SoDispCon_socketHasNonBlockDataBitMask 0x01
#define SoDispCon_hasMoreDataDataBitMask 0x02

#define SoDispCon_dispatcherHasConDataBitMask 0x04
#define SoDispCon_recEvActiveDataBitMask 0x08
#define SoDispCon_sendEvActiveDataBitMask 0x10
#define SoDispCon_isNonBlockingDataBitMask 0x20
#define SoDispCon_IP6DataBitMask 0x40
#define SoDispCon_DGramBitMask 0x80

#define SoDispCon_setDispHasRecData(o)\
  ((o)->dataBits |= \
   (SoDispCon_hasMoreDataDataBitMask | SoDispCon_socketHasNonBlockDataBitMask))
#define SoDispCon_setDispatcherHasCon(o)\
  ((o)->dataBits |= SoDispCon_dispatcherHasConDataBitMask)
#define SoDispCon_setRecEvActive(o)\
  ((o)->dataBits |= SoDispCon_recEvActiveDataBitMask)
#define SoDispCon_setSendEvActive(o)\
  ((o)->dataBits |= SoDispCon_sendEvActiveDataBitMask)
#define SoDispCon_setIP6(o)\
  ((o)->dataBits |= SoDispCon_IP6DataBitMask)

#define SoDispCon_clearSocketHasNonBlockData(o)\
  ((o)->dataBits &= ~(U8)SoDispCon_socketHasNonBlockDataBitMask)
#define SoDispCon_clearHasMoreData(o)\
  ((o)->dataBits &= ~(U8)SoDispCon_hasMoreDataDataBitMask)
#define SoDispCon_clearDispatcherHasCon(o)\
  ((o)->dataBits &= ~(U8)SoDispCon_dispatcherHasConDataBitMask)
#define SoDispCon_setRecEvInactive(o)\
  ((o)->dataBits &= ~(U8)SoDispCon_recEvActiveDataBitMask)
#define SoDispCon_setSendEvInactive(o)\
  ((o)->dataBits &= ~(U8)SoDispCon_sendEvActiveDataBitMask)
#define SoDispCon_clearIP6(o)\
  ((o)->dataBits &= ~(U8)SoDispCon_IP6DataBitMask)

/** @param[in] o Initialized connection.
 * @return TRUE if socket readiness has been recorded, FALSE otherwise. */
#define SoDispCon_socketHasNonBlockData(o)\
  (((o)->dataBits & SoDispCon_socketHasNonBlockDataBitMask) ? TRUE : FALSE)
/** @param[in] o Initialized connection.
 * @return Recorded receive readiness; not a live probe or a byte count. */
#define SoDispCon_hasMoreData(o)\
  (((o)->dataBits & SoDispCon_hasMoreDataDataBitMask) ? TRUE : FALSE)
/** @param[in] o Initialized connection.
 * @return TRUE if registered with the dispatcher, FALSE otherwise. */
#define SoDispCon_dispatcherHasCon(o)\
  (((o)->dataBits & SoDispCon_dispatcherHasConDataBitMask)?TRUE:FALSE)
/** @param[in] o Initialized connection.
 * @return TRUE if receive events are enabled. */
#define SoDispCon_recEvActive(o)\
  (((o)->dataBits & SoDispCon_recEvActiveDataBitMask)?TRUE:FALSE)
/** @param[in] o Initialized connection.
 * @return TRUE if send-ready events are enabled. */
#define SoDispCon_sendEvActive(o)\
  (((o)->dataBits & SoDispCon_sendEvActiveDataBitMask)?TRUE:FALSE)
/** @param[in] o Initialized connection.
 * @return TRUE if the connection records nonblocking mode. */
#define SoDispCon_isNonBlocking(o)\
  (((o)->dataBits & SoDispCon_isNonBlockingDataBitMask) ?  TRUE : FALSE)
#ifndef SoDispCon_isIP6
/** @param[in] o Initialized connection.
 * @return TRUE if recorded as IPv6, FALSE for IPv4. */
#define SoDispCon_isIP6(o)\
  (((o)->dataBits & SoDispCon_IP6DataBitMask) ? TRUE : FALSE)
/** @param[in] o Initialized connection.
 * @return TRUE if recorded as datagram/UDP. */
#define SoDispCon_isDGRAM(o)                                    \
  (((o)->dataBits & SoDispCon_DGramBitMask) ? TRUE : FALSE)
#endif
/** Test a successfully installed transport for TLS.
 * @param[in] o Connected object with an installed transport.
 * @return TRUE if the transport reports secure, FALSE otherwise.
 * Does not establish handshake completion, certificate trust or peer identity. */
#define SoDispCon_isSecure(o) \
   ((o)->exec(o,0,SoDispCon_GetSharkSslCon,0,0) > 0)
/** Inspect a successfully installed transport.
 * @param[in] o Connected object with a valid transport implementation.
 * @param[out] sharkSslConPtrPtr Optional SharkSslCon** output, borrowed on
 * success; NULL requests only the secure-transport test.
 * @return Transport test result; see SoDispCon::getSharkSslCon. The C macro
 * preserves negative transport errors instead of converting them to bool. */
#define SoDispCon_getSharkSslCon(o,sharkSslConPtrPtr) \
   (o)->exec(o,0,SoDispCon_GetSharkSslCon,sharkSslConPtrPtr,0)
/** @param[in] o Initialized connection.
 * @return TRUE if a socket handle is installed; not a peer-liveness test. */
#define SoDispCon_isValid(o) HttpSocket_isValid(&(o)->httpSocket)
#define SoDispCon_invalidate(o) HttpSocket_invalidate(&(o)->httpSocket)
/** @param[in] o Initialized connection.
 * @return Borrowed associated dispatcher, possibly NULL. */
#define SoDispCon_getDispatcher(o) (o)->dispatcher
#define SoDispCon_dispRecEvent(o) (o)->dispRecEv(o)
#define SoDispCon_dispSendEvent(o) (o)->dispSendEv(o)
/** Install the send-ready callback before enabling events.
 * @param[in,out] o Initialized connection.
 * @param[in] dispSendEvFp Required callback while send events are active. */
#define SoDispCon_setDispSendEvent(o, dispSendEvFp) \
   (o)->dispSendEv=dispSendEvFp

/** Install the receive callback before enabling events.
 * @param[in,out] o Initialized connection.
 * @param[in] dispRecEvFp Required callback while receive events are active. */
#define SoDispCon_setDispRecEvent(o, dispRecEvFp) \
   (o)->dispRecEv=dispRecEvFp

/** @copydoc SoDispCon::readData
 * @param[in,out] o Connected object with an installed transport. */
#define SoDispCon_readData(o, data, len, relmutex) \
   (o)->exec(o, (relmutex) ? SoDisp_getMutex((o)->dispatcher) : 0, \
           SoDispCon_ExTypeRead,data,len)

/* The HttpServCon (and classes inherited from HttpServCon) calls the
 * newConnectionIsReady function after a successful call to
 * HttpSocket_accept and after the new connection is installed in the
 * web-server. This is a platform dependent function and is, by default,
 * disabled as few platforms need this.
 */
#ifndef SoDispCon_newConnectionIsReady
#define SoDispCon_newConnectionIsReady(con)
#endif

/** Read the current SharkSSL alert fields.
 * @param[in] o Connection known to use SharkSSL, not a plain socket with an
 * asynchronous buffer or a pending connect operation.
 * @param[out] alertLevel Required U8 output, valid on success.
 * @param[out] alertDescription Required U8 output, valid on success.
 * @return Zero if TLS state exists, -1 otherwise. Does not clear the alert.
 * Available with the SharkSSL transport wrapper. */
BA_API int SoDispCon_getSharkAlert(
   SoDispCon* o, U8* alertLevel, U8* alertDescription);

#ifdef __cplusplus
extern "C" {
#endif
/** Initialize an empty connection without opening a socket.
 * @param[out] o Caller-owned storage.
 * @param[in] dispatcher Borrowed dispatcher, or NULL for independent use.
 * @param[in] e Receive callback, required before enabling receive events.
 * Install a transport by connecting or moving in an accepted connection. */
BA_API void SoDispCon_constructor(
   SoDispCon* o, struct SoDisp* dispatcher, SoDispCon_DispRecEv e);
BA_API int SoDispCon_upgrade(SoDispCon* o, struct SharkSsl* ssl,
   const char* alpn, const char* host, int port);
/** Wait for input using a blocking connection.
 * @param[in,out] o Connected object in blocking mode; no competing reader.
 * @param[out] data Required writable storage, not NUL-terminated on return.
 * @param[in] len Positive capacity in bytes.
 * @return Positive byte count, zero when the transport supplies no data,
 * negative error (including timeout). Does not guarantee a full buffer.
 * Allows the owned dispatcher mutex to be released and reacquired while waiting. */
BA_API int SoDispCon_blockRead(SoDispCon* o,void* data,int len);
/** @copydoc SoDispCon::sendData
 * @param[in,out] o Initialized connection. */
BA_API int SoDispCon_sendData(SoDispCon* o, const void* data, int len);
/** Send all bytes with the sendData contract, but omit body tracing.
 * @param[in,out] o Connected object.
 * @param[in] data Borrowed source containing len bytes.
 * @param[in] len Nonnegative byte count.
 * @return Zero complete, -1 invalid socket, E_SOCKET_WRITE_FAILED short/error.
 * No partial count is returned. May release/reacquire the dispatcher mutex. */
BA_API int SoDispCon_sendDataNT(SoDispCon* o, const void* data, int len);
/** Send all bytes without releasing the dispatcher mutex.
 * @param[in,out] o Connected object; caller manages synchronization.
 * @param[in] data Borrowed source containing len readable bytes.
 * @param[in] len Nonnegative byte count.
 * @return Zero complete, -1 invalid socket, E_SOCKET_WRITE_FAILED short/error. */
BA_API int SoDispCon_sendDataX(SoDispCon* o, const void* data, int len);
/** @copydoc SoDispCon::sendChunkData
 * @param[in,out] o Initialized connection. */
BA_API int SoDispCon_sendChunkData(SoDispCon* o,const void* data, int len);
/** Advance asynchronous sending; same status contract as SoDispCon_asyncReady.
 * @param[in,out] o Connected object with an allocated asynchronous buffer.
 * @return 1 complete, 0 pending, negative transport error. */
BA_API int SoDispCon_asyncReadyF(SoDispCon* o);
/** Advance any pending asynchronous send without modifying its payload.
 * @param[in,out] o Connected object with an allocated asynchronous buffer.
 * @return 1 complete, 0 pending, negative transport error. Does not itself
 * register a send-ready callback. */
#define SoDispCon_asyncReady(o)\
   (o)->exec(o,0,SoDispCon_ExTypeAsyncReady,0,0)
/** @copydoc SoDispCon::asyncSend
 * @param[in,out] o Connected object with an allocated asynchronous buffer. */
#define SoDispCon_asyncSend(o, len) \
   (o)->exec(o,0,SoDispCon_ExTypeAsyncReady,0,len)
struct AllocatorIntf;

#ifndef __DOXYGEN__
typedef struct
{
      void* retVal;
      int size;
} AllocAsynchBufArgs;
#endif

/** @copydoc SoDispCon::allocAsynchBuf
 * @param[in,out] o Initialized connection. */
BA_API void* SoDispCon_allocAsynchBuf(SoDispCon* o, int* size);
BA_API void SoDispCon_releaseAsyncBuf(SoDispCon* con);
BA_API void SoDispCon_internalAllocAsynchBuf(
   SoDispCon* con, AllocAsynchBufArgs* args);
/** @copydoc SoDispCon::setNonblocking
 * @param[in,out] o Initialized connection. */
BA_API int SoDispCon_setNonblocking(SoDispCon* o);
/** @copydoc SoDispCon::setBlocking
 * @param[in,out] o Initialized connection. */
BA_API int SoDispCon_setBlocking(SoDispCon* o);
BA_API int SoDispCon_peek(SoDispCon* o);
/** Move socket and transport ownership without copying object storage.
 * @param[in,out] o Source connection; receive/send events are deactivated and
 * it is removed from dispatcher registration during the move.
 * @param[in,out] newCon Initialized empty destination with dispatcher and event
 * callbacks already configured. Do not overwrite a live destination socket.
 * @return Zero. Register/activate the destination separately as required. */
BA_API int SoDispCon_moveCon(SoDispCon* o, SoDispCon*  newCon);
/** Shut down and close the socket, disable events and remove registration.
 * @param[in,out] o Initialized connection. Does not free object storage.
 * No close result is reported; transport-owned buffers are invalidated. */
#define SoDispCon_destructor(o) SoDispCon_zzCloseCon(o,1)
/** Abortively close the socket, disable events and remove registration.
 * @param[in,out] o Initialized connection. Does not free object storage.
 * No close result is reported; transport-owned buffers are invalidated. */
#define SoDispCon_hardClose(o) SoDispCon_zzCloseCon(o, 2)
/** Shut down and close the socket, disable events and remove registration.
 * @param[in,out] o Initialized connection. Does not free object storage.
 * No close result is reported; transport-owned buffers are invalidated. */
#define SoDispCon_shutdown(o) SoDispCon_zzCloseCon(o, 1)
/** Close the socket, disable events and remove registration.
 * @param[in,out] o Initialized connection. Does not free object storage.
 * No close result is reported; transport-owned buffers are invalidated. */
#define SoDispCon_closeCon(o) SoDispCon_zzCloseCon(o, 0)
/** Set the read wait timeout used by supported socket ports.
 * @param[in,out] o Connected object.
 * @param[in] timeout Nonnegative integer milliseconds, at most 3276750.
 * Stored as U16 units of 50 ms, rounding down; values below 50 disable the
 * timeout. Zero means no timeout. Some read paths consume/reset the setting;
 * SoDispCon_blockRead restores it during its internal TLS retries. */
#define SoDispCon_setReadTmo(o,timeout)  (o)->rtmo = (U16)((timeout)/50)
BA_API void SoDispCon_zzCloseCon(SoDispCon* o, int shutdown);
/** @copydoc SoDispCon::setTCPNoDelay
 * @param[in,out] o Initialized connection. */
BA_API void SoDispCon_setTCPNoDelay(SoDispCon* o, int enable);
/** @copydoc SoDispCon::getPeerName
 * @param[in,out] o Initialized connection. */
BA_API int SoDispCon_getPeerName(SoDispCon* o,HttpSockaddr* addr,U16* port);
/** @copydoc SoDispCon::getSockName
 * @param[in,out] o Initialized connection. */
BA_API int SoDispCon_getSockName(SoDispCon* o, HttpSockaddr* addr, U16* port);
/** @copydoc SoDispCon::addr2String
 * @param[in,out] o Initialized connection. */
BA_API char* SoDispCon_addr2String(
   SoDispCon* o, HttpSockaddr* addr, char* buf, int len);
/** @copydoc SoDispCon::cmpAddr
 * @param[in,out] o Initialized connection. */
BA_API BaBool SoDispCon_cmpAddr(SoDispCon* o, HttpSockaddr* addr2);
BA_API void SoDispCon_printSockErr(
   SoDispCon* o,const char* type,HttpSocket* s,int status);
/** @param[in] o Initialized connection.
 * @return Platform socket identifier; not a portable persistent connection ID. */
#define SoDispCon_getId(o) HttpSocket_getId(&(o)->httpSocket)
/* Internal */
#ifndef SoDispCon_platReadData
int SoDispCon_platReadData(SoDispCon* o, ThreadMutex* m, BaBool* isTerminated,
                           void* data, int len);
#endif

/** @copydoc SoDispCon::connect
 * @param[in,out] o Initialized empty connection. */
BA_API int
SoDispCon_connect(SoDispCon* o,
                  const char* host,
                  U16 port,
                  const void* bindIntfName,
                  U16 bindPort,
                  U32 timeout,
                  BaBool dgram, BaBool ipv6,
                  char** errinfo);
#ifndef NO_ASYNCH_RESP
/** Begin a TCP connection without waiting for TCP establishment.
 * DNS resolution can still block. Available unless NO_ASYNCH_RESP is defined.
 * @param[in,out] o Initialized empty connection with no transport buffer/TLS
 * state. Keep it alive until the attempt completes or is cleaned up.
 * @param[in] host Required NUL-terminated hostname or numeric address.
 * @param[in] port Remote port in host byte order, 1..65535.
 * @param[in] bindIntfName Borrowed local interface/address, or NULL for default;
 * copied when the resolver keeps state for additional address attempts.
 * @param[in] ipv6 TRUE requests IPv6, FALSE IPv4.
 * @param[out] errinfo Optional borrowed resolver-text output, initially NULL.
 * @return 1 connected immediately, 0 connection pending, negative error.
 * Register send-ready handling and check socket completion before sending.
 * With USE_ADDRINFO, release connect state before TLS upgrade, asynchronous
 * buffer allocation, or final socket cleanup. See asyncConnectRelease.
 * @warning The current USE_ADDRINFO allocation-failure path can also return
 * zero without retaining pending state. Do not assume that zero alone
 * establishes that another address attempt is available. */
BA_API int
SoDispCon_asyncConnect(SoDispCon* o,
                       const char* host,
                       U16 port,
                       const void* bindIntfName,
                       BaBool ipv6,
                       char** errinfo);
/** Try the next resolved address after a failed pending connection.
 * @param[in,out] o Connection with live USE_ADDRINFO pending-connect state.
 * Call only after the current attempt fails; do not use as a readiness poll.
 * @return 1 connected, 0 pending, negative error. Without USE_ADDRINFO,
 * returns E_CANNOT_CONNECT because no address-list state is retained. */
BA_API int SoDispCon_asyncConnectNext(SoDispCon* o);
#ifdef  USE_ADDRINFO
/** Release retained resolver state after an asynchronous connect attempt.
 * @param[in,out] o Connection that still holds pending-connect state, or none.
 * Does not close its socket. Call before installing TLS or an async send buffer;
 * never call after that storage has been repurposed for a transport. */
BA_API void SoDispCon_asyncConnectRelease(SoDispCon* o);
#else
#define SoDispCon_asyncConnectRelease(o)
#endif

#endif
#ifdef __cplusplus
}
inline int SoDispCon::readData(void* data, int len, bool relmutex) {
   return SoDispCon_readData(this, data, len, relmutex ? TRUE : FALSE); }
inline int SoDispCon::setNonblocking() {
   return SoDispCon_setNonblocking(this); }
inline int SoDispCon::setBlocking() {
   return SoDispCon_setBlocking(this); }
inline bool SoDispCon::isSecure() {
   return SoDispCon_isSecure(this) ? true : false; }
inline bool SoDispCon::getSharkSslCon(SharkSslCon** sc) {
   return SoDispCon_getSharkSslCon(this,sc) ? true : false; }
inline bool SoDispCon::isIP6() {
	return SoDispCon_isIP6(this) ? true : false; }
inline bool SoDispCon::isValid() {
	return SoDispCon_isValid(this) ? true : false; }
inline int SoDispCon::getPeerName(HttpSockaddr* addr,U16* port){
   return SoDispCon_getPeerName(this,addr,port); }
inline int SoDispCon::getSockName(HttpSockaddr* addr,U16* port){
   return SoDispCon_getSockName(this, addr,port); }
inline char* SoDispCon::addr2String(HttpSockaddr* addr,char* buf,int len){
   return SoDispCon_addr2String(this, addr, buf, len); }
inline bool SoDispCon::cmpAddr(HttpSockaddr* addr2) {
   return SoDispCon_cmpAddr(this, addr2) ? true : false;
}
inline void SoDispCon::setTCPNoDelay(bool enable) {
   SoDispCon_setTCPNoDelay(this, enable); }
inline bool SoDispCon::hasMoreData() {
   return SoDispCon_hasMoreData(this) ? true : false;
}
inline bool SoDispCon::dispatcherHasCon() {
   return SoDispCon_dispatcherHasCon(this) ? true : false; }
inline bool SoDispCon::recEvActive() {
   return SoDispCon_recEvActive(this) ? true : false; }
inline bool SoDispCon::sendEvActive() {
   return SoDispCon_sendEvActive(this) ? true : false; }
inline struct SoDisp* SoDispCon::getDispatcher() {
   return SoDispCon_getDispatcher(this); }
inline void SoDispCon::setDispSendEvent(SoDispCon_DispSendEv ev) {
   SoDispCon_setDispSendEvent(this, ev); }
inline void SoDispCon::setDispRecEvent(SoDispCon_DispRecEv ev) {
   SoDispCon_setDispRecEvent(this, ev); }
inline int SoDispCon::sendData(const void* data, int len) {
   return SoDispCon_sendData(this,data, len); }
inline int SoDispCon::asyncSend(int len) {
   return SoDispCon_asyncSend(this, len); }
inline int SoDispCon::sendChunkData(const void* data, int len) {
   return SoDispCon_sendChunkData(this, data, len); }
inline int SoDispCon::asyncReady() {
   return SoDispCon_asyncReadyF(this) ? true : false; }
inline void* SoDispCon::allocAsynchBuf(int* size){
   return SoDispCon_allocAsynchBuf(this, size); }
inline int SoDispCon::connect(const char* host,
                              U16 port,
                              const void* bindIntfName,
                              U16 bindPort,
                              U32 timeout,
                              BaBool dgram, BaBool ipv6,
                              char** errinfo) {
   return SoDispCon_connect(this,host,port, bindIntfName,bindPort,timeout,
                            dgram?TRUE:FALSE, ipv6?TRUE:FALSE,errinfo);
}
#endif


#endif
