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
 *   $Id: HttpServCon.h 4915 2021-12-01 18:26:55Z wini $
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

#ifndef __HttpServCon_h
#define __HttpServCon_h

#include <HttpConnection.h>
#include <SoDisp.h>


struct HttpServer;
struct HttpServCon;

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
       \param server is the web-server object.
       \param dispatcher is SoDisp object.
       \param port the server port, normally port 80.
       \param setIP6 Set protcol version. This parameter is ignored unless
       the underlying TCP/IP stack is a dual IP V4 and IP V6 stack.
       \param interfaceName the name of the interface used for binding the
        server socket. If this is zero, any available interface will
        be selected.
       \param userDefinedAccept The default (argument is NULL) is to
       do "accept calls" for the web-server.

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
       returns. You must, therefore, copy the data in the connection
       object.

       Example C code:

       \code
       //Dispatcher calls this function when new data on my connection obj.
       static void MyHttpCon_dispatchData(MyHttpCon* o)
       {
          char buf[size];
          HttpConnection* con = (HttpConnection*)o; // Down cast
          int len = HttpConnection_readData(con, buf, size);
          if(len < 0)
          { // Socket closed
             SoDisp_removeConnection(
                HttpConnection_getDispatcher(con),con);
             free(o);
          }
          if(len)
          {
             // handle data
          }
       }
       // HttpServCon calls this function for all new connections
       static void MyHttpCon_myAccept(HttpConnection* tmpCon)
       {
          MyHttpCon* o = // MyHttpCon inherit from HttpConnection
             baMalloc(sizeof(MyHttpCon));
          if(o)
          {
             HttpConnection* newCon = (HttpConnection*)o; // Down cast

             HttpConnection_constructor(
                newCon,
                HttpConnection_getServer(tmpCon),
                (SoDispCon_DispRecEv)MyHttpCon_dispatchData);
              //Copy connection
             HttpConnection_moveCon(tmpCon,(HttpConnection*)con);
             SoDisp_addConnection(
                HttpConnection_getDispatcher(newCon),newCon);
          }
       }
       \endcode
       */
      HttpServCon(HttpServer* server,
                  SoDisp* dispatcher,
                  U16 port=80,
                  BaBool setIP6=FALSE,
                  const void* interfaceName=0,
                  HttpServCon_AcceptNewCon userDefinedAccept=0);

      /** Returns true if the constructor successfully opened the
          listen socket; otherwise, false is returned. Error messages
          are printed to HttpTrace.
      */
      BaBool isValid();
      
      /** Change the port number for the "listen" object.
       */
      int setPort(U16 portNumber, bool setIp6=false,
                  const void* interfaceName=0);

      ~HttpServCon();
      HttpServCon() {}
   private:
#endif
      HttpConnection con;
      HttpServCon_AcceptNewCon userDefinedAccept;
} HttpServCon;


#ifdef __cplusplus
extern "C" {
#endif
BA_API void HttpServCon_constructor(HttpServCon* o,
                             struct HttpServer* server,
                             struct SoDisp* dispatcher,
                             U16 port,
                             BaBool setIP6,
                             const void* interfaceName,
                             HttpServCon_AcceptNewCon userDefinedAccept);
#define HttpServCon_isValid(o) \
        SoDispCon_isValid((SoDispCon*)(o))
BA_API int HttpServCon_setPort(HttpServCon* o, U16 portNumber,
                               BaBool setIp6, const void* interfaceName);
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
