/*
 *     ____             _________                __                _     
 *    / __ \___  ____ _/ /_  __(_)___ ___  ___  / /   ____  ____ _(_)____
 *   / /_/ / _ \/ __ `/ / / / / / __ `__ \/ _ \/ /   / __ \/ __ `/ / ___/
 *  / _, _/  __/ /_/ / / / / / / / / / / /  __/ /___/ /_/ / /_/ / / /__  
 * /_/ |_|\___/\__,_/_/ /_/ /_/_/ /_/ /_/\___/_____/\____/\__, /_/\___/  
 *                                                       /____/          
 *
 *                  Barracuda Application Server
 *
 ****************************************************************************
 *			      HEADER
 *
 *   $Id: NetIo.h 5392 2023-02-21 15:56:50Z wini $
 *
 *   COPYRIGHT:  Real Time Logic LLC, 2008 - 2020
 *               http://www.realtimelogic.com
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
 */
#ifndef __NetIo_h
#define __NetIo_h

#include <HttpServer.h>
#include <IoIntf.h>

struct SoDisp;
struct SharkSsl;
struct HttpClient;


/** The NetIo is similar to a network file system and makes it
    possible for the server to access resources on another Barracuda
    server. The NetIo is a web file manager client specifically
    designed to operate a remote HttpResMgr. In other words, the
    remote HttpResMgr's file system appears as a local file system to
    code using an instance of the NetIo class.

    The NetIo is typically used during development of LSP code in an
    embedded device without a file system. The device can access
    resources on for example a remote Windows computer as if the
    resources are locally on the device.

    It is recommended to increase the size of the Web server's default
    HTTP response buffer if the NetIo is configured as the I/O for a
    HttpResRdr, HttpResMgr, or a WebDAV instance. A size of 8Kbytes
    decreases the number of HTTP connections initiated when reading
    from the NetIo by a factor of 6. See
    HttpServerConfig::setResponseData for more information.

    The NetIo code, which can be found in xrc/misc/NetIo.c, is
    delivered as example code. You must therefore include this code in
    your build if you intend to use the NetIo.
 */
typedef struct NetIo
#ifdef __cplusplus
: public IoIntf
{
      void *operator new(size_t s) { return ::baMalloc(s); }
      void operator delete(void* d) { if(d) ::baFree(d); }
      void *operator new(size_t, void *place) { return place; }
      void operator delete(void*, void *) { }
      /** The NetIo constructor.
          A NetIo instance cannot be used before it is initialized by
          calling NetIo::setRootDir.
          \param disp is an optional parameter that makes the
          integrated NetIo HTTP client release the dispatcher mutex
          when reading from or writing to the socket. The 'disp'
          parameter is typically set if the NetIo class is used by a
          Barracuda Web Server object such as a HttpResRdr instance.
      */
      NetIo(struct SoDisp* disp=0);

      /** The NetIo destructor.
       */
      ~NetIo();

      /** Initialize/set the URL to the far side Barracuda server.

      The URL must be to either the root of a HttpResRdr or to a
      sub-directory within a HttpResRdr instance. As an example, a far
      side Barracuda server, with a HttpResRdr instance installed in
      the Virtual File System at URI /drive/ can be initialized as
      follows:

      \code
      Base url: http://domainname/drive/
      Sub-directory: http://domainname/drive/c/lsp/
      \endcode

      In the second example above, the far side HttpResMgr is mapped
      to the root of the Windows computer. The path /c/lsp/ is to a
      directory where the files to a LSP application can be found.

      The NetIo instance can also access a remote HttpResMgr protected
      by an authenticator as long as the far side authenticator
      accepts Basic authentication. In other words, the far side
      authenticator must be BasicAuthenticator or Authenticator.

      \param url the URL to the far side HttpResMgr.
      Example: http://192.168.1.100/fs/
      */
      int setRootDir(const char* url);

      /** Set HTTP basic authentication username and password.
       */
      int setUser(const char* user, const char* password);

      /** Set proxy.
          \param proxy is the proxy IP address or hostname.
          \param portNo is the proxy listen port number.
          \param useSocksProxy if false, use HTTPS proxy, otherwise use SOCKS5.
       */
      int setProxy(const char* proxy, U16 portNo, BaBool useSocksProxy);

      /** Set proxy username.
       */
      int setProxyUser(const char* user, const char* password);
      
      /** When connection, bind to intfName.
       */
      int setIntfName(const char* intfName);

      /** Use IPv6, default is IPv4.
       */
      void setIPv6(bool enable);

      /* Enable secure connections. The SharkSsl instance must have
         been initialized as a SharkSsl_Client.
      */
      void setSSL(struct SharkSsl* sharkSslClient);
#if 0
}
#endif
#else
{
      IoIntf super; /* Inherits from IoIntf */
#endif
      ThreadMutex netMutex;
      SoDisp* disp;

      struct SharkSsl* sharkSslClient; /* optional SSL client */

      /* Variable proxyPortNo and the 3 following variables are sent
       * to the HttpClient.
       */
      char* proxy; /* Proxy name/IP addr, if any */
      char* proxyUserPass; /* Format:  "user:password" */
      char* intfName; /* If 0: bind to any intf, or bind to intfName */
      struct HttpClient* cCon;
      char* userPass;
      char* rootPath; /* The base URL */
      int rootPathLen;

      U16 proxyPortNo; /* host port number */

      U8 httpClientMode; /* HttpClient_SocksProxy | HttpClient_IPv6 */
} NetIo;

#ifdef __cplusplus
extern "C" {
#endif

void NetIo_constructor(NetIo* o, struct SoDisp* disp);
void NetIo_destructor(NetIo* o);
int NetIo_setRootDir(NetIo* o, const char* url);


int NetIo_setUser(NetIo* o, const char* user, const char* password);
int NetIo_setProxy(
   NetIo* o, const char* proxy, U16 portNo, BaBool useSocksProxy);
int NetIo_setProxyUser(NetIo* o, const char* user, const char* password);
int NetIo_setIntfName(NetIo* o, const char* intfName);
int NetIo_setIPv6(NetIo* o, BaBool enable);
#define NetIo_setSSL(o, ssl) (o)->sharkSslClient=ssl


#ifdef __cplusplus
}

inline NetIo::NetIo(struct SoDisp* disp) {
   NetIo_constructor(this, disp);
}
inline NetIo::~NetIo() {
   NetIo_destructor(this);
}
inline int NetIo::setRootDir(const char* url) {
   return NetIo_setRootDir(this, url);
}
inline int NetIo::setUser(const char* user, const char* password) {
   return NetIo_setUser(this, user, password);
}
inline int NetIo::setProxy(const char* proxy,U16 portNo,BaBool useSocksProxy) {
   return NetIo_setProxy(this, proxy, portNo, useSocksProxy);
}
inline int NetIo::setProxyUser(const char* user, const char* password) {
   return NetIo_setProxyUser(this, user, password);
}
inline int NetIo::setIntfName(const char* intfName) {
   return NetIo_setIntfName(this, intfName);
}
inline void NetIo::setIPv6(bool enable) {
   NetIo_setIPv6(this, enable ? TRUE : FALSE);
}
inline void NetIo::setSSL(struct SharkSsl* sharkSslClient) {
   NetIo_setSSL(this, sharkSslClient);
}
#endif


#endif
