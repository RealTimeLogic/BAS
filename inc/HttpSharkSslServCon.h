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
 *   $Id: HttpSharkSslServCon.h 4915 2021-12-01 18:26:55Z wini $
 *
 *   COPYRIGHT:  Real Time Logic LLC, 2004 - 2012
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

      // Default constructor is used as a dummy op with the C constructors
      HttpSharkSslServCon() {}
      
      /** Create a SharkSSL Server Connection.

      \param sharkSsl is the SharkSsl instance required for operating
       the server connection. You must make sure that this object is
       valid for the lifetime of the HttpSharkSslServCon instance.
      \param server is the web-server object.
      \param dispatcher is SoDisp object.
      \param port the server port; normally port 443
      \param setIP6 Set protcol version. This parameter is ignored unless
      the underlying TCP/IP stack is a dual IP V4 and IP V6 stack.
      \param interfaceName the name of the interface used when binding the
      server socket. If this is zero, any available interface will
      be selected.
      \param userDefinedAccept Same functionality as for HttpServCon,
      but this callback is for implementing secure servers.
      See HttpServCon for more information.
       */
      HttpSharkSslServCon(SharkSsl* sharkSsl,
                          HttpServer* server,
                          SoDisp* dispatcher,
                          U16 port=443,
                          bool setIP6=false,
                          const void* interfaceName=0,
                          HttpServCon_AcceptNewCon userDefinedAccept=0);
      BaBool isValid();

      /** Change the port number for the "listen" object.
       */
      int setPort(U16 portNumber, bool setIp6=false,
                  const void* interfaceName=0);
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
SHARKSSL_API int HttpSharkSslServCon_setPort(HttpSharkSslServCon* o,
                                             U16 portNumber,
                                             BaBool setIp6,
                                             const void* interfaceName);
#define HttpSharkSslServCon_isValid(o) HttpServCon_isValid((HttpServCon*)o)
#define HttpSharkSslServCon_requestClientCert(o,enable) \
        (o)->requestClientCert=enable
#define HttpSharkSslServCon_favorRSA(o,enable) \
        (o)->favorRSA=enable

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
