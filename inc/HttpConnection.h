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
 *   $Id: HttpConnection.h 4915 2021-12-01 18:26:55Z wini $
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
      HttpConnection() {}

      /** Fetch the HttpServer object
       */
      struct HttpServer* getServer();

      /** Close connection after request completed. */
      void setKeepAlive();

      /** Keep connection after request completed, i.e. a persistent
       * 1.1 connection.
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

#define HttpConnection_setKeepAlive(o) (o)->keepAlive=TRUE
#define HttpConnection_clearKeepAlive(o) (o)->keepAlive=FALSE
#define HttpConnection_keepAlive(o) (o)->keepAlive
#define HttpConnection_getState(o) (o)->state
#define HttpConnection_getServer(o) (o)->server
#define HttpConnection_getDispatcher(o) SoDispCon_getDispatcher((SoDispCon*)o)
#define HttpConnection_dispRecEvent(o) SoDispCon_dispRecEvent((SoDispCon*)o)
#define HttpConnection_newConnectionIsReady(con) \
  SoDispCon_newConnectionIsReady(con)

#ifdef __cplusplus
extern "C" {
#endif
BA_API void HttpConnection_constructor(HttpConnection* o,
                                       struct HttpServer* server,
                                       struct SoDisp* dispatcher,
                                       SoDispCon_DispRecEv e);
BA_API void HttpConnection_setState(
   HttpConnection* o, HttpConnection_State state);
BA_API int HttpConnection_moveCon(HttpConnection* o, HttpConnection*  newCon);
int HttpConnection_sendChunkData6bOffs(
   HttpConnection* o,const void* data,int len);
BA_API void HttpConnection_destructor(HttpConnection* o);
BA_API int HttpConnection_pushBack(HttpConnection* o, const void* d, int s);
#define HttpConnection_isSecure(o)\
   SoDispCon_isSecure((SoDispCon*)o)
#define HttpConnection_isValid(o)\
   SoDispCon_isValid((SoDispCon*)o)
#define HttpConnection_sendData(o, data, len)\
   SoDispCon_sendData((SoDispCon*)o, data, len)
#define HttpConnection_sendDataNT(o, data, len)\
   SoDispCon_sendDataNT((SoDispCon*)o, data, len)
#define HttpConnection_sendChunkData(o,data, len)\
   SoDispCon_sendChunkData((SoDispCon*)o,data, len)
#define HttpConnection_allocAsynchBuf(o, sizePtr)\
   SoDispCon_allocAsynchBuf((SoDispCon*)o, sizePtr)
#define HttpConnection_asyncSend(o,len) SoDispCon_asyncSend((SoDispCon*)o,len)
#define HttpConnection_asyncReady(o) SoDispCon_asyncReady((SoDispCon*)o)
BA_API int HttpConnection_readData(HttpConnection* con, void* data, int len);
BA_API int HttpConnection_blockRead(HttpConnection* con, void* data, int len);
#define HttpConnection_setNonblocking(o)\
   SoDispCon_setNonblocking((SoDispCon*)o)
#define HttpConnection_setBlocking(o)\
   SoDispCon_setBlocking((SoDispCon*)o)
#define HttpConnection_peek(o)\
   SoDispCon_peek((SoDispCon*)o)
#define HttpConnection_setTCPNoDelay(o, enable)\
   SoDispCon_setTCPNoDelay((SoDispCon*)o, enable)
#define HttpConnection_getPeerName(o, addr,port)\
   SoDispCon_getPeerName((SoDispCon*)o, addr,port)
#define HttpConnection_getSockName(o, addr,port)\
   SoDispCon_getSockName((SoDispCon*)o, addr,port)
#define HttpConnection_addr2String(o, addr, buf, len)\
   SoDispCon_addr2String((SoDispCon*)o, addr, buf, len)
#define HttpConnection_cmpAddr(o, addr2)\
   SoDispCon_cmpAddr((SoDispCon*)o, addr2)
#define HttpConnection_recEvActive(o)\
   SoDispCon_recEvActive((SoDispCon*)o)
#define HttpConnection_sendEvActive(o)\
   SoDispCon_sendEvActive((SoDispCon*)o)
#define HttpConnection_hasMoreData(o)\
   (SoDispCon_hasMoreData((SoDispCon*)o) || (o)->pushBackData)
#define HttpConnection_dispatcherHasCon(o)\
   SoDispCon_dispatcherHasCon((SoDispCon*)o)
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
