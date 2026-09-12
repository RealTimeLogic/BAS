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
 *   $Id: NetIo.h 5978 2026-09-11 16:13:48Z wini $
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
/** @file NetIo.h */

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
    HTTP response buffer if NetIo is configured as the I/O for a
    HttpResRdr, HttpResMgr, or a WebDAV instance. A size of 8Kbytes
    can reduce remote read requests compared with a small buffer; the benefit
    depends on the resources and access pattern. See
    HttpServerConfig::setResponseData for more information.

    The NetIo code, which can be found in xrc/misc/NetIo.c, is
    delivered as example code. You must therefore include this code in
    your build if you intend to use NetIo. Serialize configuration changes
    with file operations; the internal I/O mutex does not protect all setters.
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
          \param disp Borrowed dispatcher (default NULL), valid throughout use.
          This optional parameter that makes the
          integrated NetIo HTTP client release the dispatcher mutex
          when reading from or writing to the socket. The 'disp'
          parameter is typically set if the NetIo class is used by a
          Barracuda Web Server object such as a HttpResRdr instance.
      */
      NetIo(struct SoDisp* disp=0);

      /** Close the cached HTTP client and free copied configuration and its
       mutex. Close open resources/iterators and stop callers first. Does not
       destroy the borrowed dispatcher or TLS client.
       */
      ~NetIo();

      /** Initialize/set the URL to the far side Barracuda server.

      The URL must identify the root of a HttpResMgr or a
      subdirectory within a HttpResMgr instance. As an example, a far
      side Barracuda server, with a HttpResMgr instance installed in
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

      \param url Required NUL-terminated HTTP(S) URL to the remote HttpResMgr
      directory, copied and normalized with a trailing slash. Example:
      http://192.168.1.100/fs/. Configure authentication and TLS first.
      @return Zero after the remote stat confirms a directory; E_INVALID_URL
      for an invalid URL, E_MALLOC for allocation failure, IOINTF_ENOENT for a
      nondirectory, or another I/O/network status. This operation performs
      network I/O. The previous root is discarded even if the change fails;
      a failed instance must receive a valid root before normal file operations.
      */
      int setRootDir(const char* url);

      /** Set HTTP Basic authentication credentials and discard the cached client.
       * @param[in] user NUL-terminated username, or NULL to clear credentials.
       * @param[in] password NUL-terminated password; if NULL, user must contain
       * the complete user:password string. Strings are copied.
       * @return Zero success, E_MALLOC allocation failure. Old credentials
       * are discarded before allocation, so failure is not a rollback.
       */
      int setUser(const char* user, const char* password);

      /** Set proxy.
          \param proxy Copied NUL-terminated proxy hostname/address, or NULL to disable.
          \param portNo Proxy TCP port in host byte order, 1..65535 when enabled.
          \param useSocksProxy FALSE selects the HTTP CONNECT proxy path; TRUE
          selects SOCKS5. This is a proxy protocol choice, not a promise of
          encrypted communication with the proxy itself.
          @return Zero success, E_MALLOC allocation failure. Discards the cached
          client and changes proxy mode before allocating the new address.
       */
      int setProxy(const char* proxy, U16 portNo, BaBool useSocksProxy);

      /** Set copied proxy authentication credentials; discard the cached client.
       * @param[in] user NUL-terminated username, or NULL to clear credentials.
       * @param[in] password NUL-terminated password, or NULL when user already
       * contains user:password. Configure before opening remote resources.
       * @return Zero success, E_MALLOC allocation failure; old credentials are
       * discarded before allocation.
       */
      int setProxyUser(const char* user, const char* password);
      
      /** Select the local interface for future HTTP clients.
       * @param[in] intfName Copied NUL-terminated platform interface/address,
       * or NULL to use the default. Does not replace an already cached client.
       * @return Zero success, E_MALLOC allocation failure. Old value is discarded.
       */
      int setIntfName(const char* intfName);

      /** Select the address family and discard the cached client.
       * @param[in] enable True selects IPv6, false IPv4 (initial default).
       */
      void setIPv6(bool enable);

      /** Configure TLS for future HTTP clients.
       * @param[in] sharkSslClient Borrowed initialized SharkSsl_Client, valid
       * until NetIo and its HTTP clients are destroyed; NULL disables TLS setup.
       * Configure before setRootDir for HTTPS. Does not replace a cached client
       * or transfer ownership. Certificate policy belongs to the TLS setup.
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

/** @copydoc NetIo::NetIo
 * @param[in,out] o Caller-owned NetIo instance. */
void NetIo_constructor(NetIo* o, struct SoDisp* disp);
/** @copydoc NetIo::~NetIo
 * @param[in,out] o Caller-owned NetIo instance. */
void NetIo_destructor(NetIo* o);
/** @copydoc NetIo::setRootDir
 * @param[in,out] o Caller-owned NetIo instance. */
int NetIo_setRootDir(NetIo* o, const char* url);


/** @copydoc NetIo::setUser
 * @param[in,out] o Caller-owned NetIo instance. */
int NetIo_setUser(NetIo* o, const char* user, const char* password);
/** @copydoc NetIo::setProxy
 * @param[in,out] o Caller-owned NetIo instance. */
int NetIo_setProxy(
   NetIo* o, const char* proxy, U16 portNo, BaBool useSocksProxy);
/** @copydoc NetIo::setProxyUser
 * @param[in,out] o Caller-owned NetIo instance. */
int NetIo_setProxyUser(NetIo* o, const char* user, const char* password);
/** @copydoc NetIo::setIntfName
 * @param[in,out] o Caller-owned NetIo instance. */
int NetIo_setIntfName(NetIo* o, const char* intfName);
/** Select the family for subsequent connections and discard the cached client.
 * @param[in,out] o Initialized NetIo.
 * @param[in] enable TRUE selects IPv6, FALSE IPv4.
 * @return Always zero. The C++ wrapper returns no value. */
int NetIo_setIPv6(NetIo* o, BaBool enable);
/** Assign TLS configuration for future clients without changing the cached client.
 * @param[in,out] o Initialized NetIo.
 * @param[in] ssl Borrowed initialized SharkSsl_Client or NULL, valid throughout use.
 * @sa NetIo::setSSL */
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
