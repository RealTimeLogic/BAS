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
 *   $Id: SoDispCon.h 5387 2023-02-20 22:50:13Z wini $
 *
 *   COPYRIGHT:  Real Time Logic LLC, 2006 - 2023
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


typedef void (*SoDispCon_DispRecEv)(struct SoDispCon* con);
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
   SoDispCon() {}


      /** Connect to host/IP address
          \param host the host/IP address.
          \param port the port number or service.
          \param bindIntfName the interface to bind this socket to.
          \param dgram defaults to false (TCP).
          \param bindPort optionally bind to specific port
          \param timeout in milliseconds
          \param ipv6 defaults to false.
          \param errinfo may be set to platform error.

          \returns 0 on success and < 0 on error: sock error code.
      */

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

      /** Returns true if this is an SSL connection.
          \param sc is set to the SharkSslCon object.
       */
     bool getSharkSslCon(SharkSslCon** sc);


      /** Returns true if the socket connection is valid.
       */
      bool isValid();

      /** Returns true if this is an IP version 6 connection. */
      bool isIP6();

      /** Returns the 'peer' IP address.
          \returns the IP address in the 'addr' argument.
       */
      int getPeerName(HttpSockaddr* addr,U16* port=0);

      /** Returns the 'sock' IP address.
          \returns the IP address in the 'addr' argument.
       */
      int getSockName(HttpSockaddr* addr,U16* port=0);

      /** Converts IP address to string.
       */
      char* addr2String(HttpSockaddr* addr, char* buf, int len);

      /** Compare addr with address returned by getPeerName.
       */
      bool cmpAddr(HttpSockaddr* addr2);

      /** Disable the TCP delay.
       * What TCP normally does is queue up data so it only sends full
       * packets out when possible. (This is called Nagle’s algorithm.)
       * Call this function if you do not want that behavior.
       */
      void setTCPNoDelay(bool enable);
      
      /** Fetch the SoDisp object.
       */
      struct SoDisp* getDispatcher();

      /** Returns true if more data on the socket.
       */
      bool hasMoreData();

      /** Returns true if connection is in the SoDisp object.
       */
      bool dispatcherHasCon();

      /** Returns true if the connection is active in the
       * SoDisp object.
       */
      bool recEvActive();

      /** Used for non blocking send.
       */
      bool sendEvActive();

      void setDispSendEvent(SoDispCon_DispSendEv ev);
      void setDispRecEvent(SoDispCon_DispRecEv ev);
      int readData(void* data, int len, bool relmutex=false);
      int setNonblocking();
      int setBlocking();
      int sendData(const void* data, int len);
      int asyncSend(int len);
      int sendChunkData(const void* data, int len);
      int asyncReady();
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

#define SoDispCon_socketHasNonBlockData(o)\
  (((o)->dataBits & SoDispCon_socketHasNonBlockDataBitMask) ? TRUE : FALSE)
#define SoDispCon_hasMoreData(o)\
  (((o)->dataBits & SoDispCon_hasMoreDataDataBitMask) ? TRUE : FALSE)
#define SoDispCon_dispatcherHasCon(o)\
  (((o)->dataBits & SoDispCon_dispatcherHasConDataBitMask)?TRUE:FALSE)
#define SoDispCon_recEvActive(o)\
  (((o)->dataBits & SoDispCon_recEvActiveDataBitMask)?TRUE:FALSE)
#define SoDispCon_sendEvActive(o)\
  (((o)->dataBits & SoDispCon_sendEvActiveDataBitMask)?TRUE:FALSE)
#define SoDispCon_isNonBlocking(o)\
  (((o)->dataBits & SoDispCon_isNonBlockingDataBitMask) ?  TRUE : FALSE)
#ifndef SoDispCon_isIP6
#define SoDispCon_isIP6(o)\
  (((o)->dataBits & SoDispCon_IP6DataBitMask) ? TRUE : FALSE)
#define SoDispCon_isDGRAM(o)                                    \
  (((o)->dataBits & SoDispCon_DGramBitMask) ? TRUE : FALSE)
#endif
#define SoDispCon_isSecure(o) \
   ((o)->exec(o,0,SoDispCon_GetSharkSslCon,0,0) > 0)
#define SoDispCon_getSharkSslCon(o,sharkSslConPtrPtr) \
   (o)->exec(o,0,SoDispCon_GetSharkSslCon,sharkSslConPtrPtr,0)
#define SoDispCon_isValid(o) HttpSocket_isValid(&(o)->httpSocket)
#define SoDispCon_invalidate(o) HttpSocket_invalidate(&(o)->httpSocket)
#define SoDispCon_getDispatcher(o) (o)->dispatcher
#define SoDispCon_dispRecEvent(o) (o)->dispRecEv(o)
#define SoDispCon_dispSendEvent(o) (o)->dispSendEv(o)
#define SoDispCon_setDispSendEvent(o, dispSendEvFp) \
   (o)->dispSendEv=dispSendEvFp

#define SoDispCon_setDispRecEvent(o, dispRecEvFp) \
   (o)->dispRecEv=dispRecEvFp

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

BA_API int SoDispCon_getSharkAlert(
   SoDispCon* o, U8* alertLevel, U8* alertDescription);

#ifdef __cplusplus
extern "C" {
#endif
BA_API void SoDispCon_constructor(
   SoDispCon* o, struct SoDisp* dispatcher, SoDispCon_DispRecEv e);
BA_API int SoDispCon_upgrade(SoDispCon* o, struct SharkSsl* ssl,
   const char* alpn, const char* host, int port);
BA_API int SoDispCon_blockRead(SoDispCon* o,void* data,int len);
BA_API int SoDispCon_sendData(SoDispCon* o, const void* data, int len);
BA_API int SoDispCon_sendDataNT(SoDispCon* o, const void* data, int len);
BA_API int SoDispCon_sendDataX(SoDispCon* o, const void* data, int len);
BA_API int SoDispCon_sendChunkData(SoDispCon* o,const void* data, int len);
BA_API int SoDispCon_asyncReadyF(SoDispCon* o);
#define SoDispCon_asyncReady(o)\
   (o)->exec(o,0,SoDispCon_ExTypeAsyncReady,0,0)
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

BA_API void* SoDispCon_allocAsynchBuf(SoDispCon* o, int* size);
BA_API void SoDispCon_releaseAsyncBuf(SoDispCon* con);
BA_API void SoDispCon_internalAllocAsynchBuf(
   SoDispCon* con, AllocAsynchBufArgs* args);
BA_API int SoDispCon_setNonblocking(SoDispCon* o);
BA_API int SoDispCon_setBlocking(SoDispCon* o);
BA_API int SoDispCon_peek(SoDispCon* o);
BA_API int SoDispCon_moveCon(SoDispCon* o, SoDispCon*  newCon);
#define SoDispCon_destructor(o) SoDispCon_zzCloseCon(o,1)
#define SoDispCon_hardClose(o) SoDispCon_zzCloseCon(o, 2)
#define SoDispCon_shutdown(o) SoDispCon_zzCloseCon(o, 1)
#define SoDispCon_closeCon(o) SoDispCon_zzCloseCon(o, 0)
#define SoDispCon_setReadTmo(o,timeout)  (o)->rtmo = (U16)((timeout)/50)
BA_API void SoDispCon_zzCloseCon(SoDispCon* o, int shutdown);
BA_API void SoDispCon_setTCPNoDelay(SoDispCon* o, int enable);
BA_API int SoDispCon_getPeerName(SoDispCon* o,HttpSockaddr* addr,U16* port);
BA_API int SoDispCon_getSockName(SoDispCon* o, HttpSockaddr* addr, U16* port);
BA_API char* SoDispCon_addr2String(
   SoDispCon* o, HttpSockaddr* addr, char* buf, int len);
BA_API BaBool SoDispCon_cmpAddr(SoDispCon* o, HttpSockaddr* addr2);
BA_API void SoDispCon_printSockErr(
   SoDispCon* o,const char* type,HttpSocket* s,int status);
#define SoDispCon_getId(o) HttpSocket_getId(&(o)->httpSocket)
/* Internal */
#ifndef SoDispCon_platReadData
int SoDispCon_platReadData(SoDispCon* o, ThreadMutex* m, BaBool* isTerminated,
                           void* data, int len);
#endif

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
BA_API int
SoDispCon_asyncConnect(SoDispCon* o,
                       const char* host,
                       U16 port,
                       const void* bindIntfName,
                       BaBool ipv6,
                       char** errinfo);
BA_API int SoDispCon_asyncConnectNext(SoDispCon* o);
#ifdef  USE_ADDRINFO
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
                            dgram?TRUE:FALSE, dgram?TRUE:FALSE,errinfo);
}
#endif


#endif
