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
 *   $Id: OpenSslCon.h 5076 2022-02-10 16:59:48Z wini $
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

#ifndef __OpenSslCon_h
#define __OpenSslCon_h
 
#include <HttpServCon.h>
#include <openssl/ssl.h>

typedef struct OpenSslCon
{
#ifdef __cplusplus

   /** Create an OpenSSL Server Connection Object.
       \param ctx an initialized SSL_CTX object.
      \param server is the web-server object.
      \param port the server port; normally port 443
      \param setIP6 Set protcol version. This parameter is ignored unless
      the underlying TCP/IP stack is a dual IP V4 and IP V6 stack.
      \param interfaceName the name of the interface used when binding the
      server socket. If this is zero, any available interface will
      be selected.
   */
   OpenSslCon(
      SSL_CTX* ctx,
      struct HttpServer* server,
      U16 port,
      BaBool setIP6,
      const void* interfaceName);

   /** Change the port number for the "listen" object.
    */
   int setPort(U16 portNumber, bool setIp6=false,
               const void* interfaceName=0);

   ~OpenSslCon(); 
   private:
#endif
   HttpServCon super;
   SSL_CTX* ctx;
} OpenSslCon;

#ifdef __cplusplus
extern "C" {
#endif

SHARKSSL_API void OpenSslCon_constructor(
   OpenSslCon* o,
   SSL_CTX* ctx,
   struct HttpServer* server,
   U16 port,
   BaBool setIP6,
   const void* interfaceName);

SHARKSSL_API void OpenSslCon_destructor(OpenSslCon* o);


SHARKSSL_API int OpenSslCon_setPort(OpenSslCon* o,
                                    U16 portNumber,
                                    BaBool setIp6,
                                    const void* interfaceName);
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
