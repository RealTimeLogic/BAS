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
 *   $Id: HttpServCon.h 5978 2026-09-11 16:13:48Z wini $
 *
 *   COPYRIGHT:  Real Time Logic LLC, 2003-2008
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

/** @file HttpServCon.h */

#ifndef __HttpServCon_h
#define __HttpServCon_h

#include <HttpConnection.h>
#include <SoDisp.h>


struct HttpServer;
struct HttpServCon;

/** Handle a newly accepted connection while the dispatcher mutex is held.
 * @param[in] scon Borrowed listener that accepted the socket.
 * @param[in,out] newcon Temporary connection, valid only during this callback.
 * Move it with HttpConnection_moveCon into an initialized application-owned
 * connection before returning to accept it. Otherwise its socket is closed.
 * Do not memcpy the connection or retain the temporary pointer. A TLS listener
 * can invoke this callback before the TLS handshake has completed. */
typedef void (*HttpServCon_AcceptNewCon)(
   struct HttpServCon* scon, HttpConnection* newcon);


/** Create a server listen object.
    The object binds itself to the SoDisp object and makes the
    web-server listen for new connections on the port specified.
    The default port is 80. You can create several of these objects and bind
    to the dispatcher if you want the web-server to listen to more than one
    port.
 */
typedef struct HttpServCon
{
#ifdef __cplusplus

      void *operator new(size_t s) { return ::baMalloc(s); }
      void operator delete(void* d) { if(d) ::baFree(d); }
      void *operator new(size_t, void *place) { return place; }
      void operator delete(void*, void *) { }


      /** Create a Server Connection.
       \param server Borrowed server, valid throughout listener use. Required
       for the default HTTP handler and for setPort.
       \param dispatcher Required borrowed dispatcher, valid throughout use.
       \param port TCP port in host byte order, default 80. Zero requests an
       OS-assigned port if supported by the socket port.
       \param setIP6 TRUE selects IPv6, FALSE IPv4 (default). This parameter is ignored unless
       the underlying TCP/IP stack is a dual IP V4 and IP V6 stack.
       \param interfaceName Borrowed platform binding address/interface, used
        during construction; normally a NUL-terminated textual address. NULL
        binds the wildcard address. Accepted representation is port-specific.
       \param userDefinedAccept The default (argument is NULL) is to
       accept connections for the web-server. A custom callback is required
       in NO_BA_SERVER builds; omitting it calls baFatalE.

       A ServerConnection object is normally used for accepting new
       connections for the web-server. It is possible to
       redirect new connections to the "userDefinedAccept" callback
       function. This makes it possible to use the socket dispatcher
       logic in the web-server for implementing other services such as
       a telnet server.

       The "user defined accept" callback function is called when a
       new connection is established. The HttpConnection object passed
       in as the argument to the callback function is a temporary
       object that will be destroyed as soon as the callback function
       returns. You must, therefore, move the connection into an initialized application object.

       Example C code:

       \code
       typedef struct { HttpConnection con; } MyHttpCon;

       static void MyHttpCon_dispatchData(SoDispCon* socket)
       {
          MyHttpCon* o = (MyHttpCon*)socket;
          char buf[512];
          int len = HttpConnection_readData(&o->con, buf, sizeof(buf));
          if(len < 0)
          {
             HttpConnection_destructor(&o->con); // Unregister and close.
             baFree(o);
             return;
          }
          if(len > 0)
          {
             // Consume exactly len bytes here; buf is not NUL-terminated.
          }
       }

       static void MyHttpCon_myAccept(HttpServCon* listener,
                                      HttpConnection* temporary)
       {
          MyHttpCon* o = (MyHttpCon*)baMalloc(sizeof(MyHttpCon));
          SoDisp* disp = HttpConnection_getDispatcher(temporary);
          (void)listener;
          if(!o) return; // The listener closes the unclaimed socket.
          HttpConnection_constructor(&o->con,
             HttpConnection_getServer(temporary), disp,
             MyHttpCon_dispatchData);
          HttpConnection_moveCon(temporary, &o->con);
          SoDisp_addConnection(disp, (SoDispCon*)&o->con);
          SoDisp_activateRec(disp, (SoDispCon*)&o->con);
       }
       \endcode
       */
      HttpServCon(HttpServer* server,
                  SoDisp* dispatcher,
                  U16 port=80,
                  BaBool setIP6=FALSE,
                  const void* interfaceName=0,
                  HttpServCon_AcceptNewCon userDefinedAccept=0);

      /** @return TRUE if the listen socket is currently valid, FALSE otherwise.
          Check after construction, which returns no status. Error messages
          are printed to HttpTrace.
      */
      BaBool isValid();
      
      /** Open a replacement listening endpoint before closing the old one.
       * @param[in] portNumber New TCP port, in host byte order.
       * @param[in] setIp6 True selects IPv6, false IPv4 (default).
       * @param[in] interfaceName Platform binding address/interface, or NULL
       * for wildcard; borrowed for this call only.
       * @return Zero on success, -1 if creating/binding/listening fails. The
       * old listener remains on failure. Requires a non-NULL server and uses
       * that server's dispatcher. Existing accepted connections are unaffected.
       * Perform listener changes while holding the dispatcher mutex.
       */
      int setPort(U16 portNumber, bool setIp6=false,
                  const void* interfaceName=0);

      /** Close and unregister the listener; accepted connections are separate.
       * Does not destroy the borrowed server or dispatcher. */
      ~HttpServCon();
      /** Uninitialized storage; initialize before use or destruction. */
      HttpServCon() {}
   private:
#endif
      HttpConnection con;
      HttpServCon_AcceptNewCon userDefinedAccept;
} HttpServCon;


#ifdef __cplusplus
extern "C" {
#endif
/** @copydoc HttpServCon::HttpServCon
 * @param[in,out] o Caller-owned listener. */
BA_API void HttpServCon_constructor(HttpServCon* o,
                             struct HttpServer* server,
                             struct SoDisp* dispatcher,
                             U16 port,
                             BaBool setIP6,
                             const void* interfaceName,
                             HttpServCon_AcceptNewCon userDefinedAccept);
/** @param[in] o Initialized listener.
 * @return TRUE if its socket is valid, FALSE otherwise. */
#define HttpServCon_isValid(o) \
        SoDispCon_isValid((SoDispCon*)(o))
/** @copydoc HttpServCon::setPort
 * @param[in,out] o Caller-owned listener. */
BA_API int HttpServCon_setPort(HttpServCon* o, U16 portNumber,
                               BaBool setIp6, const void* interfaceName);
/** Close and unregister a listener; does not close its accepted connections.
 * @param[in,out] o Initialized listener; its storage is not freed. */
BA_API void HttpServCon_destructor(HttpServCon* o);
BA_API int HttpServCon_init(
   HttpServCon* o,
   struct HttpServer* server,
   U16 port,
   BaBool setIP6,
   const void* interfaceName);

/* Exclusively used by SoDispCon_connect */
BA_API void HttpServCon_bindExec(SoDispCon* con);

#ifdef __cplusplus
}
inline HttpServCon::HttpServCon(HttpServer* server,
                                SoDisp* dispatcher,
                                U16 port,
                                BaBool setIP6,
                                const void* interfaceName,
                                HttpServCon_AcceptNewCon userDefinedAccept) {
   HttpServCon_constructor(this,server,dispatcher,port,setIP6,interfaceName,
                           userDefinedAccept);
}
inline HttpServCon::~HttpServCon() {
   HttpServCon_destructor(this);
}
inline BaBool HttpServCon::isValid() { return HttpServCon_isValid(this); }
inline int HttpServCon::setPort(
   U16 portNumber,bool setIp6,const void* interfaceName) {
   return HttpServCon_setPort(this,portNumber,setIp6?TRUE:FALSE,interfaceName);
}


#endif

#endif
