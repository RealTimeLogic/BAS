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
 *   $Id: HttpSharkSslServCon.h 5978 2026-09-11 16:13:48Z wini $
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
/** @file HttpSharkSslServCon.h */

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
