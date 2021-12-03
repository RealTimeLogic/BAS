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
 *   $Id: HttpMuxDir.h 4915 2021-12-01 18:26:55Z wini $
 *
 *   COPYRIGHT:  Real Time Logic, 2008
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

#ifndef __HttpMuxDir_h
#define __HttpMuxDir_h

#include <HttpServer.h>


/** HttpMuxDir HttpDir Multiplexer captures many muxResource requests and
    feeds all muxResource requests to one page.
    The HttpMuxDir class makes it possible to have an LSP or CSP page
    handle multiple URL's. The HttpMuxDir class is typically used in
    Representational State Transfer (REST) design. Another use for this
    class is to design a template based page design, where all pages
    use a common header and footer.

   The page can get the full URI by calling:

   C/C++ code HttpResponse::getRequestURI.

   LSP: request:uri

   The URI is typically used as a path for opening HTML page fragments
   or for searching in a database.

   The page can signal that it did not handle the request by not
   writing anything to the response. The virtual
   file system will, in this case, continue searching for the muxResource.

*/
typedef struct HttpMuxDir
#ifdef __cplusplus
: public HttpDir
{
 /**
   \param name the HttpDir name.

   \param startDir the directory where we search for the mux muxResource.

   \param muxResource is the relative path from 'startDir' to the mux
   muxResource, which is normally a CSP or LSP page.
 
   \param priority the page priority. See HttpDir for more information.
*/
      HttpMuxDir(const char* name,
                 HttpDir* startDir,
                 const char* muxResource,
                 S8 priority=0);

#else
#if 0
}
#endif
{
   HttpDir super; /* Inherits from HttpDir */
#endif
   HttpDir_Service orgService;
   HttpDir* startDir;
   const char* muxResource;
} HttpMuxDir;

#ifdef __cplusplus
extern "C" {
#endif 
void HttpMuxDir_constructor(
   HttpMuxDir* o,
   const char* name,
   HttpDir* startDir,
   const char* muxResource,
   S8 priority);
#ifdef __cplusplus
}
inline HttpMuxDir::HttpMuxDir(const char* name,
                              HttpDir* startDir,
                              const char* muxResource,
                              S8 priority) {
   HttpMuxDir_constructor(this,name,startDir,muxResource,priority);
}
#endif

#endif
