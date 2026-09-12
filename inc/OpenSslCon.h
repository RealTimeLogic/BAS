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
 *   $Id: OpenSslCon.h 5978 2026-09-11 16:13:48Z wini $
 *
 *   COPYRIGHT:  Real Time Logic LLC, 2021
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
 *               https://realtimelogic.com
 ****************************************************************************
 *
 *             OpenSSL HTTP Server Porting Layer.
 *
 */

/** @file OpenSslCon.h */

#ifndef __OpenSslCon_h
#define __OpenSslCon_h
 
#include <HttpServCon.h>
#include <openssl/ssl.h>

/** OpenSSL-backed HTTP listener. Configure SSL_CTX before construction.
 * Check OpenSslCon_isValid after construction; socket setup has no direct
 * constructor status. Listener configuration requires the dispatcher mutex. */
typedef struct OpenSslCon
{
#ifdef __cplusplus

   /** Create an OpenSSL Server Connection Object.
       \param ctx Required borrowed SSL_CTX configured for server use. Keep it
       alive throughout listener use; configure certificates before listening.
      \param server Required borrowed server, valid throughout listener use.
      \param port TCP listen port in host byte order, normally 443. Zero
      initializes an inactive listener without opening a socket.
      \param setIP6 TRUE selects IPv6, FALSE IPv4. This parameter is ignored unless
      the underlying TCP/IP stack is a dual IP V4 and IP V6 stack.
      \param interfaceName Borrowed platform binding address/interface, used
      during the call; normally a NUL-terminated address string. NULL binds the
      wildcard address. Accepted representation is platform-specific.
   */
   OpenSslCon(
      SSL_CTX* ctx,
      struct HttpServer* server,
      U16 port,
      BaBool setIP6,
      const void* interfaceName);

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

   /** Close and unregister the listener. Accepted connections are separate.
    * Does not destroy the borrowed TLS configuration, server or dispatcher. */
   ~OpenSslCon(); 
   private:
#endif
   HttpServCon super;
   SSL_CTX* ctx;
} OpenSslCon;

#ifdef __cplusplus
extern "C" {
#endif

/** @copydoc OpenSslCon::OpenSslCon
 * @param[out] o Caller-owned listener storage. */
SHARKSSL_API void OpenSslCon_constructor(
   OpenSslCon* o,
   SSL_CTX* ctx,
   struct HttpServer* server,
   U16 port,
   BaBool setIP6,
   const void* interfaceName);

/** Close and unregister the listener. Accepted connections are separate.
 * @param[in,out] o Initialized listener; no borrowed dependency is freed. */
SHARKSSL_API void OpenSslCon_destructor(OpenSslCon* o);


/** @copydoc OpenSslCon::setPort
 * @param[in,out] o Initialized listener. */
SHARKSSL_API int OpenSslCon_setPort(OpenSslCon* o,
                                    U16 portNumber,
                                    BaBool setIp6,
                                    const void* interfaceName);
/** @param[in] o Initialized listener.
 * @return TRUE if its listening socket is valid, FALSE otherwise. */
#define OpenSslCon_isValid(o) HttpServCon_isValid((HttpServCon*)o)

#ifdef __cplusplus

inline OpenSslCon::OpenSslCon(
   SSL_CTX* ctx,
   struct HttpServer* server,
   U16 port,
   BaBool setIP6,
   const void* interfaceName) {
   OpenSslCon_constructor(this,ctx,server,port,setIP6,interfaceName);
}
   
inline int OpenSslCon::setPort(U16 portNumber, bool setIp6=false,
                               const void* interfaceName) {
   return OpenSslCon_setPort(this,portNumber,setIp6,interfaceName);
}

inline OpenSslCon::~OpenSslCon() {
   OpenSslCon_destructor(this);
}


}
#endif

#endif
