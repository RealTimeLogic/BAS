/*
 *     ____             _________                __                _     
 *    / __ \___  ____ _/ /_  __(_)___ ___  ___  / /   ____  ____ _(_)____
 *   / /_/ / _ \/ __ `/ / / / / / __ `__ \/ _ \/ /   / __ \/ __ `/ / ___/
 *  / _, _/  __/ /_/ / / / / / / / / / / /  __/ /___/ /_/ / /_/ / / /__  
 * /_/ |_|\___/\__,_/_/ /_/ /_/_/ /_/ /_/\___/_____/\____/\__, /_/\___/  
 *                                                       /____/          
 *
 *                  Barracuda Embedded Web-Server 
 ****************************************************************************
 *            HEADER
 *
 *   $Id: HttpRecData.h 4915 2021-12-01 18:26:55Z wini $
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
 */

#ifndef _HttpRecData_h
#define _HttpRecData_h


#include <HttpServer.h>

/** @addtogroup HttpStack
    @{
 */

/** The HttpRecData class makes it easy to upload packets or large
    data chunks when using Barracuda in multithread mode. This class
    gives similar functionality to HttpAsynchReq and makes it
    possible to receive any kind of data sent from a client in a CSP
    page or in a HttpDir/HttpPage.

<b>CSP example</b>
\code
<%p
if(HttpRecData::valid(req) >=0)
{
   HttpRecData rd(request); //Create on stack inside a CSP page.
   char buf[1500];
   S32 len;
   while( (len = rd.read(buf, sizeof(buf))) > 0)
   {
      //Save buf
   }
   if(len < 0)
      response->printf("failed");
   else
      response->printf("OK");
}
else
{
   response->printf("Bugger off");
}
%>
\endcode
 */
typedef struct HttpRecData
{
#ifdef __cplusplus

      /** This method validates the client HTTP request. The data
          received from the client must have a content-length or be
          chunk encoded.

          \returns the content length if this is a valid request. The
          return value can be zero if the client sends chunk encoded
          data. <b>This method returns the following error codes:</b>
          \li -1: Missing content-length and not chunk encoding.
          \li -2: URL encoded data consumed by web-server.
          \li -3: Received multipart/form-data. Use MultipartUpload.
       */
      static SBaFileSize valid(HttpRequest* req);


      /** The constructor is typically run by creating an object on the stack.
          \param req is the page HttpRequest object.
       */
      HttpRecData(HttpRequest* req);

      /** Destructor */
      ~HttpRecData();

      /** Read data from socket.
          \param buf is the buffer to where the data is saved.
          \param bufSize is the size of the buffer.
          \returns the length received. The method returns 0 when all
          data is received. The method returns a negative value if the
          socket connection fails.
      */
      S32 read(void* buf, S32 bufSize);
#endif
      HttpRequest* req;
      HttpConnection* con;
      SBaFileSize sizeLeft;
      SBaFileSize bufSize;
      SBaFileSize readPos;
      SBaFileSize chunkSize;
} HttpRecData;

#ifdef __cplusplus
extern "C" {
#endif
BA_API SBaFileSize HttpRecData_valid(HttpRequest* req);
BA_API void HttpRecData_constructor(HttpRecData* o, HttpRequest* req);
BA_API void HttpRecData_destructor(HttpRecData* o);
BA_API S32 HttpRecData_read(HttpRecData* o, void* buf, S32 bufSize);
#ifdef __cplusplus
}
inline SBaFileSize HttpRecData::valid(HttpRequest* req) {
   return HttpRecData_valid(req);
}
inline HttpRecData::HttpRecData(HttpRequest* req) {
   HttpRecData_constructor(this, req);
}
inline S32 HttpRecData::read(void* buf, S32 bufSize) {
   return HttpRecData_read(this, buf, bufSize);
}
inline HttpRecData::~HttpRecData() {
   HttpRecData_destructor(this);
}
#endif

/** @} */ /* end of HttpStack */

#endif
