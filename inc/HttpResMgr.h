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
 *   $Id: HttpResMgr.h 4915 2021-12-01 18:26:55Z wini $
 *
 *   COPYRIGHT:  Real Time Logic, 2006 - 2019
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

#ifndef __HttpResMgr_h
#define __HttpResMgr_h

#include "HttpUpload.h"


/** The HttpResMgr class makes it possible for HTTP clients to
    use an IoIntf for uploading and downloading files.

The HttpResMgr is similar in functionality to an FTP server, but
uses HTTP for the transport of data. 

The HttpResMgr is implemented in C code, but we also provide a
<a href="../../../lua/lua.html#ba_create_wfs">Web File Manager</a>
implemented in Lua code. See the WFS documentation for more
information on the Lua version.

The HttpResMgr is designed to be used from both browser clients
and non-browser clients. The HttpResMgr detects the difference
between a browser and non-browser client by looking at the commands
sent to the HttpResMgr resource and the upload type.

The HttpResMgr can also work with "read only" IoIntf
implementations such as the ZipIo, but any "write" command type is denied.

Unlike the HttpResRdr class, the HttpResMgr does not do any
translations on downloaded files. The HttpResMgr should be used
for file transfer and not for serving general HTML files, images, etc,
to a client. A request for a directory returns a list of all resources
in the directory. This is different from the HttpResRdr class, which
attempts to locate the index.html file for directory requests.

For security reasons, a HttpResMgr should not be used without an
authenticator.

The HttpResMgr is an example application for how to make a class
that inherits from HttpDir. The HttpResMgr class can be
considered a resource collection. The HttpResMgr source code and
example can be found in the
<a href="../../../../../examples/HttpResMgr/readme.html">example/HttpResMgr</a> directory.
*/
typedef struct HttpResMgr
#ifdef __cplusplus
: public HttpDir
{ 
      /** Initializes a HttpResMgr.
          \param io is a IoIntf implementation such as DiskIo
          \param maxUploads is the limit value for concurrent uploads. The
          maximum numbers of downloads are controlled by the
          HttpCmdThreadPool. See the limitations section in
          HttpResRdr for more information on how the download works.
          \param dirName is the HttpDir name.
          \param alloc is the allocator used when allocating
          "concurrent upload" objects.
          \param priority is the HttpDir priority
      */
      HttpResMgr(IoIntf* io,
                 int maxUploads,
                 const char* dirName, 
                 AllocatorIntf* alloc=0,
                 S8 priority=0);

      /**  Terminate the HttpResMgr object. Please note that any
           upload in progress will be aborted.
       */
      ~HttpResMgr();
#if 0
}
#endif
#else
{
   HttpDir super; /* Inherits from HttpDir */
#endif
   HttpUploadCbIntf uploadCb;

   HttpUpload upload;
   IoIntf* io;
   AllocatorIntf* alloc;
} HttpResMgr;



#ifdef __cplusplus
extern "C" {
#endif

 void
HttpResMgr_constructor(HttpResMgr* o,
                       IoIntf* io,
                       int maxUploads,
                       const char* dirName, 
                       AllocatorIntf* alloc,
                       S8 priority);

 void HttpResMgr_destructor(HttpResMgr* o);

#define HttpResMgr_setAuthenticator(o, authenticator, realm) \
   HttpDir_setAuthenticator((HttpDir*)o, authenticator, realm)
#ifdef __cplusplus
}
inline HttpResMgr::HttpResMgr(IoIntf* io,
                              int maxUploads,
                              const char* dirName, 
                              AllocatorIntf* alloc,
                              S8 priority) {
   HttpResMgr_constructor(this,io,maxUploads,dirName, alloc,priority);
}

inline HttpResMgr::~HttpResMgr(){
   HttpResMgr_destructor(this);
}
#endif

#endif /* __HttpResMgr_h */

