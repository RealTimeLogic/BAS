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
 *   $Id: HttpResRdr.h 5355 2022-11-16 10:33:45Z wini $
 *
 *   COPYRIGHT:  Real Time Logic LLC, 2006-2008
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

#ifndef _HttpResRdr_h
#define _HttpResRdr_h

#include "IoIntf.h"
#include "BaServerLib.h"
#include "HttpServer.h"


struct HttpRdFilter;
typedef void (*HttpRdFilter_Service)(struct HttpRdFilter* o,
                                     const char* name,
                                     IoStat* st,
                                     HttpCommand* cmd);

/** Superclass for HttpResRdr filters. This class is used when
 * designing plugins such as the lspfilter, the LSP plugin.
 */
typedef struct HttpRdFilter
{
      DoubleLink super;
      HttpRdFilter_Service serviceFp;
      const char* ext; /* File Extension */
} HttpRdFilter;

BA_API void HttpRdFilter_constructor(
   HttpRdFilter* o, const char* ext, HttpRdFilter_Service serviceFp);

BA_API void HttpRdFilter_destructor(HttpRdFilter* o);

/* Designed for LSP function dir:header(table)
   The key and value indexes point to the key and value embedded as
   part of the memory region below the struct.
*/
typedef struct {
   U16 keyIx;
   U16 valIx;
} HttpResRdrHeader;


/** The HTTP resource reader searches and presents IoIntf resources to
    HTTP clients requesting such information.

The HttpResRdr class makes it possible for HTTP clients to fetch
resources from an IoIntf implementation. The HttpResRdr is a bridge
between HTTP request/response and the IoIntf implementation. As an
example, the HttpResRdr class can fetch files such as HTML files, gif
images, etc from a file system and send the file to a HTTP client
requesting the information.

The HttpResRdr can only download files to a client, not upload files
to the server; thus, the HttpResRdr is only using the "read" methods
in the IoIntf. HTTP Methods supported by HttpResRdr are: OPTIONS,
GET, or HEAD. HttpResRdr accepts any HTTP method for include/forward
delegations and files not found. This is explained below.

The HttpResRdr searches the IoIntf for the requested resource and
automatically handles the response. If the resource cannot be found,
HttpResRdr delegates the request to the original HttpDir service
function. The HTTP Method type is ignored if the resource is not
found. This means that you can insert CSP or HttpDir/HttpPage
instances in the HttpResRdr by using HttpDir::insertPage and
HttpDir::insertDir.

A client requesting a directory ending with '/' makes the HttpResRdr
search for index.X files, where X is 'htm', 'html', and extensions as
specified by installed filters. The HttpDir service method returns -1
if no index.X file can be found or the directory does not end with
'/'.

The HttpResRdr treats files with extension 'shtml' as hidden files
that can only be accessed via HttpResponse::forward or
HttpResponse::include.

The HttpResRdr is specifically designed to be used with
HttpResponse::include and HttpResponse::forward. The HTTP method is
ignored and is assumed to be a GET if the HttpResRdr detects that
this is a forward or include call. This makes it possible to design
model view controller applications by using HttpResponse::forward from
a CSP page or an HttpPage.

Response data is sent with a HTTP size header for direct delegations
and HttpResponse::forward delegations. HttpResponse::include
delegations require special handling and data is sent using chunk
encoding for HTTP 1.1 clients.

The HttpResRdr automatically handles caching and sends a "304 not
modified" response to a client if it detects that the client version
is the same as the server version. The cache handling defaults to
etags and downgrades to time handling if the client does not support
etags.

<b>Limitations:</b><br>
A HttpResRdr instance is normally used for downloading small HTML
files, gif images etc, but may also be used for downloading large
files. The HttpResRdr instance blocks the current thread when
downloading a file and a large file may take considerable time to
download. A Barracuda Web-Server running in the default single
threaded mode will block all other requests while downloading a large
file. It is, therefore, recommended to run the server in multi
threaded mode if you use a HttpResRdr instance to download large
files. See class HttpCmdThreadPool for more information.
*/
typedef struct HttpResRdr
#ifdef __cplusplus
: public HttpDir
{
      HttpResRdr() {}
      /** Initializes a HttpResRdr.
          \param io is a IoIntf implementation such as ZipIo.
          \param dirName is the HttpDir name. You must allocate
          persistent memory for this string if not constant.
          See \ref HttpDirVolatileMem "HttpDir" for an example.
          \param alloc is the allocator used for allocating temporary
          string objects.
          \param priority is the HttpDir priority.
      */
      HttpResRdr(IoIntf* io, const char* dirName,
                  AllocatorIntf* alloc=0, S8 priority=0);

      /** Initialize a HttpResRdr for
          <a href="https://en.wikipedia.org/wiki/Virtual_hosting">
          name based virtual hosting</a>.

          A HttpResRdr instance initialized for virtual hosting must
          be installed as a root directory using method
          HttpServer::insertRootDir.

          The C constructor name is: HttpResRdr_constructor2.

          Please note that you must allocate persistent memory for the
          string 'domain' and 'p404' if not constant.
          See \ref HttpDirVolatileMem "HttpDir" for an example.

          \param io is a IoIntf implementation such as ZipIo.
          \param domain is the domain name for this HttpResRdr instance.
          \param p404 is the path to a 404 resource. The HttpResRdr
          instance automatically forwards the request to this resource if
          the resource requested for this domain was not found. You
          can set this parameter to NULL, thus making the web-server
          continue searching for the page in another resource with the
          same domain or in a resource that accepts any domain. The
          404 resource can be a CSP page, an LSP page, a static HTML
          page, etc..
          \param alloc is the allocator used for allocating temporary
          string objects.
          \param priority is the HttpDir priority.

       */
      HttpResRdr(IoIntf* io, const char* domain, const char* p404,
                 AllocatorIntf* alloc=0, S8 priority=0);

      /** Insert a prologue HttpDir.
         prologue directories are activated (called) before the
         HttpResRdr service function is run, but after
         authentication. The the HttpResRdr service function is not
         activated if a prologue directory finds the
         resource. Epilogue directories can be inserted by using
         method HttpDir::insertDir. prologue and epilogue directories
         are chained instead of added as sub directories if the
         directory name for the added directories is NULL.  The
         priorities for sub directories and/or chained directories
         are: prologue > HttpResRdr > epilogue.
      */
      int insertPrologDir(HttpDir* dir);

      /* Terminate a HttpResRdr instance */
      ~HttpResRdr();

      /** Install a filter. The HttpRdFilter Documentation is not complete.
       */
      int installFilter(HttpRdFilter* filter);

      /** Makes the HttpResRdr instance set the HTTP header
          "Cache-Control: max-age" for all resources. See
          HttpResponse::setMaxAge for more information.
      */
      void  setMaxAge(BaTime maxAge);

      /** Send resource to a client.

      The sendFile method is internally used by the HttpResRdr when
      sending a resource to a client. The method automatically handles
      caching and HTTP method type handling as explained above.

      The method is made public as it may be useful for specialized
      HttpDir implementations.  The method cannot be used with
      HttpResponse:include calls or if data is committed. No data can
      be sent on the response object prior to calling this method.
      */
      static void sendFile(
         IoIntf* io,const char* name, IoStat* st, HttpCommand* cmd);

      /** Returns the IoIntf used by the HttpResRdr.
       */
      IoIntf* getIo();
#if 0
}
#endif
#else
{
      HttpDir super; /* Inherits from HttpDir */
#endif
      DoubleList filterList;
      AllocatorIntf* alloc;
      IoIntf* io;
      HttpDir_Service superServiceFunc;
      HttpDir* prologDirRoot;
      HttpResRdrHeader* headers;
      const char* domain;
      const char* p404;
      BaTime maxAge;
      int maxFilterLen;
} HttpResRdr;


#ifdef __cplusplus
extern "C" {
#endif
BA_API void HttpResRdr_constructor(
   HttpResRdr* o, IoIntf* io, const char* dirName,
   AllocatorIntf* alloc, S8 priority);
BA_API void HttpResRdr_constructor2(
   HttpResRdr* o,IoIntf* io,const char* domain,
   const char* p404,AllocatorIntf* alloc,
   S8 priority);
BA_API int HttpResRdr_insertPrologDir(HttpResRdr* o, HttpDir* dir);
BA_API void HttpResRdr_destructor(HttpResRdr* o);
BA_API int HttpResRdr_installFilter(HttpResRdr* o, HttpRdFilter* filter);
#define HttpResRdr_setMaxAge(o, maxAgeMA) (o)->maxAge=maxAgeMA
#define HttpResRdr_setAuthenticator(o, authenticator, realm) \
   HttpDir_setAuthenticator((HttpDir*)o, authenticator, realm)
BA_API void HttpResRdr_sendFile(IoIntf* io,const char* name,
   IoStat* st,HttpCommand* cmd);
BA_API void set_deflategzip(IoIntf_DeflateGzip ptr);
BA_API IoIntf_DeflateGzip get_deflategzip(void);
BA_API void HttpResRdr_setHeader(HttpResRdr* o, HttpResRdrHeader* headers);


#define HttpResRdr_getIo(o) (o)->io
#ifdef __cplusplus
}
inline HttpResRdr::HttpResRdr(IoIntf* io, const char* domain, const char* p404,
                              AllocatorIntf* alloc, S8 priority) {
   HttpResRdr_constructor2(this, io, domain, p404, alloc, priority);
}
inline HttpResRdr::HttpResRdr(IoIntf* io, const char* dirName,
                              AllocatorIntf* alloc, S8 priority) {
   HttpResRdr_constructor(this, io, dirName, alloc, priority);
}

inline int HttpResRdr::insertPrologDir(HttpDir* dir) {
   return HttpResRdr_insertPrologDir(this, dir);
}

inline HttpResRdr::~HttpResRdr(){
   HttpResRdr_destructor(this);
}
inline int HttpResRdr::installFilter(HttpRdFilter* filter){
   return HttpResRdr_installFilter(this, filter);
}
inline void  HttpResRdr::setMaxAge(BaTime maxAge){
   HttpResRdr_setMaxAge(this, maxAge);
}
inline void HttpResRdr::sendFile(
   IoIntf* io, const char* name, IoStat* st, HttpCommand* cmd){
   HttpResRdr_sendFile(io,name,st,cmd);
}
inline IoIntf* HttpResRdr::getIo() {
   return HttpResRdr_getIo(this);
}
#endif


#endif
