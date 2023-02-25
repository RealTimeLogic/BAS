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
 *   $Id: MultipartUpload.h 5387 2023-02-20 22:50:13Z wini $
 *
 *   COPYRIGHT:  Real Time Logic LLC, 2006 - 2023
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

#ifndef __HttpMultipartUpload_h
#define __HttpMultipartUpload_h

#include "HttpServer.h"
#include "BaServerLib.h"

/** @addtogroup HttpStack
    @{
 */


/** Error codes used by the callback function
    MultipartUpload_Error
    in interface class MultipartUpload.
 */
typedef enum
{
   MultipartUpload_NoError=0,
   MultipartUpload_ConnectionTerminated=-100,
   MultipartUpload_NoMemory,
   MultipartUpload_ParseError1,
   MultipartUpload_ParseError2,
   MultipartUpload_ParseError3,
   MultipartUpload_ParseError4,
   MultipartUpload_ParseError5,
   MultipartUpload_ParseError6,
   MultipartUpload_UserRetErr
} MultipartUpload_ErrorType;

#ifndef __DOXYGEN__
struct MultipartUpload;
#endif

/** Executed by MultipartUpload when a multipart upload is completed.
    \return This function must return 0 on success. Any other value
    signals an error condition to the MultiPartUpload class.  This
    method must call the MultipartUpload destructor or call
    MultipartUpload::getCon
 */
typedef void (*MultipartUpload_EndOfReq)(
   struct MultipartUpload* o);


/** Executed by MultipartUpload every time a form data element such as
    "text" is found in the multipart data stream. 
    The browser bundles all data into the multipart stream sent to the
    server. The MultipartUpload servlet calls the formData callback
    function when it detects form data.
    \param o the object
    \param name the name as specified in the HTML "input" type.
    \param value the text entered by the user.
    \return This function must return 0 on success. Any other value
    signals an error condition to the MultiPartUpload class.  This
    method must call the MultipartUpload destructor or call
    MultipartUpload::getCon if returning a non zero value.
 */
typedef int (*MultipartUpload_FormData)(
   struct MultipartUpload* o,
   const char* name,
   const char* value);

/** Executed by MultipartUpload when a file is found in the multipart data
    stream.
    \param o the object
    \param name the name as specified in the HTML "input" type.
    \param fileName is the name and possible path of the file entered by the
    user. The path separator is platform dependent.
    \param contentType is the content mime type such as "text/plain".
    This parameter is NULL if not specified in the multipart data stream.
    \param contentTransferEncoding is the transfer encoding such as "gzip".
    This parameter is NULL if not specified in the multipart data stream.
    \return This function must return 0 on success. Any other value
    signals an error condition to the MultiPartUpload class.
    This method must call the MultipartUpload destructor or call
    MultipartUpload::getCon if returning
    a non zero value.
 */
typedef int (*MultipartUpload_FileBegin)(
   struct MultipartUpload* o,
   const char* name,
   const char* fileName,
   const char* contentType,
   const char* contentTransferEncoding);

/** Executed by MultipartUpload when a data chunk is available. The servlet
    first calls MultipartUpload_FileBegin and thereafter calls this
    method repeatedly until all data is received. End of file is signalled
    by setting the two input parameters to 0.
    \param o the object
    \param data a pointer to the received data.
    \param len length of the data chunk.
    \return This function must return 0 on success. Any other value
    signals an error condition to the MultiPartUpload class.  This
    method must call the MultipartUpload destructor or call
    MultipartUpload::getCon if returning a non zero value.
 */
typedef int (*MultipartUpload_FileData)(
   struct MultipartUpload* o,
   const U8* data,
   U16 len);

/** Executed by MultipartUpload if any errors are detected during
    parsing of the multipart data stream. This method must call the
    MultipartUpload destructor or call MultipartUpload::getCon.
    \param o the object
    \param e is the error code.
 */
typedef void (*MultipartUpload_Error)(
   struct MultipartUpload* o,
   MultipartUpload_ErrorType e);



typedef enum {
   MultipartUpload_ReadBoundaryTag,
   MultipartUpload_ReadHeaders,
   MultipartUpload_ReadFormData,
   MultipartUpload_ReadFileData
} MultipartUpload_States;


/** The MultipartUpload, which is an abstract base class, implements
    the functionality as specified in <a
    href="http://www.faqs.org/rfcs/rfc1867.html">RFC 1867</a>. A HTTP
    POST in multipart mode uses a rather complex scheme for
    transferring data, and we do not recommend using this class unless
    you use a browser for uploading data or need to send a stream of
    binary files and application/x-www-form-urlencoded data to the
    server. Transferring files from a client to the server is easier
    with HTTP PUT, but a browser can, unfortunately, not upload data
    using HTTP PUT.

    You must subclass the MultipartUpload class and implement a number
    of 'onEvent' callback methods. The 'onEvent' methods are called as
    the MultipartUpload decodes the stream of data uploaded from the
    client.

    The MultipartUpload class is designed such that any error detected
    in the 'onEvent' methods must make sure the MultipartUpload
    destructor or call MultipartUpload::getCon is called before
    returning a non-zero value. You may also have to release the
    memory associated with the MultipartUpload instance -- i.e. with the
    instance of the class sub-classing MultipartUpload. The
    MultipartUpload class is designed such that it allows the callback
    to destroy the object itself; that is, the MultipartUpload logic makes
    sure no data, except for stack variables, are used when the
    callback returns an error code.
*/
typedef struct MultipartUpload
{
#ifdef __cplusplus
     MultipartUpload() {}
 
      /**
         \param server is the Web-Server object.

         \param onEndOfReq is called when the multipart upload is completed.
         See \ref MultipartUpload_EndOfReq.

         \param onFormData the function where you process form data.
         See \ref MultipartUpload_FormData.

         \param onFileBegin is called when a file is found in the
         input stream.
         See \ref MultipartUpload_FileBegin.

         \param onFileData is repeatedly called until end of file.
         See \ref MultipartUpload_FileData.

         \param onError is called if an error is encountered when processing
         the input stream.
         See \ref MultipartUpload_Error.

         \param bufferSize is the size of the internal buffer used
         when parsing the received data stream. This size must be, at
         a minimum, the size of the largest form submitted. This size
         should probably be > 10K.

         \param allocator An allocator used for various small string
          allocations needed when parsing the incomming data stream
          and for allocating the internal buffer. The size of the
          internal buffer is set with parameter 'bufferSize'.
      */
      MultipartUpload(HttpServer* server,
                      MultipartUpload_EndOfReq onEndOfReq,
                      MultipartUpload_FormData onFormData,
                      MultipartUpload_FileBegin onFileBegin,
                      MultipartUpload_FileData onFileData,
                      MultipartUpload_Error onError,
                      U32 bufferSize,
                      AllocatorIntf* allocator = 0);

      /** Release buffers used during parsing the multipart stream. */
      ~MultipartUpload();

      /** Start receiving data asynchronously.
      The active socket object is detached from the web-server and
      moved into the MultipartUpload object. The MultipartUpload object is
      inserted into the SoDisp object, thus causing the socket
      dispatcher to fire receive events directly to the MultipartUpload
      object. The event methods are called: when the buffer is
      full, receiving form data, etc.
      */
      int start(HttpRequest *req);


      int run(HttpRequest *req, bool setKeepAlive=true);

      /** Returns the internal active connection object.

      This method is typically called from within the onEndOfReq event
      method when all data is received and you are moving the active
      connection object to an HttpAsynchResp object.

      C code example:
      \code
      HttpAsynchResp_constructor2(
         resp, buf, bufSize, MultipartUpload_getCon(req));
      \endcode
      */
      HttpConnection* getCon(MultipartUpload* o);
 
      /** Get the HttpServer instance.
       */
      HttpServer* getServer();

   private:
#endif
      HttpConnection super; /* Inherits from HttpConnection */

       /* List used as a stack, containing Boundary tags. */
      DoubleList boundaryStack;
      MultipartUpload_EndOfReq endOfReq;
      MultipartUpload_FormData formData;
      MultipartUpload_FileBegin fileBegin;
      MultipartUpload_FileData fileData;
      MultipartUpload_Error onError;
      HttpConnection* con; /* Set if in blocking mode. NULL for asynch mode. */
      char* dataBuffer;
      char* name;
      char* fileName;
      char* contentType;
      char* contentTransferEncoding;
      char* currBName;
      AllocatorIntf* alloc;
      MultipartUpload_States state;
      U32 currentI;
      U32 readI;
      U32 lineStartI;
      U32 lineEndI;
      U32 dataBufferSize;
      U32 expandSize;
      U32 maxFormSize;
      BaBool newBoundaryTag;
      BaBool copyingHttpReqData;
} MultipartUpload;


#ifdef __cplusplus
extern "C" {
#endif
void
BA_API MultipartUpload_constructor(
   MultipartUpload* o,
   HttpServer* server,
   MultipartUpload_EndOfReq endOfReq,
   MultipartUpload_FormData formData,
   MultipartUpload_FileBegin fileBegin,
   MultipartUpload_FileData fileData,
   MultipartUpload_Error onError,
   U32 bufferSize,
   AllocatorIntf* allocator);
BA_API void MultipartUpload_destructor(MultipartUpload* o);
BA_API int MultipartUpload_start(MultipartUpload* o, HttpRequest *req);
BA_API int MultipartUpload_run(
   MultipartUpload* o, HttpRequest *req, BaBool setKeepAlive);
BA_API HttpConnection* MultipartUpload_getCon(MultipartUpload* o); 
#define MultipartUpload_getServer(o) \
   HttpConnection_getServer((HttpConnection*)(o))
#ifdef __cplusplus
}
inline MultipartUpload::MultipartUpload(
   HttpServer* server,
   MultipartUpload_EndOfReq endOfReq,
   MultipartUpload_FormData formData,
   MultipartUpload_FileBegin fileBegin,
   MultipartUpload_FileData fileData,
   MultipartUpload_Error onError,
   U32 size,
   AllocatorIntf* allocator) {
   MultipartUpload_constructor(this, server, endOfReq, formData,
                               fileBegin, fileData, onError, size, allocator);
}
inline MultipartUpload::~MultipartUpload() {
   MultipartUpload_destructor(this);
}
inline int MultipartUpload::start(HttpRequest *req){
   return MultipartUpload_start(this,req);
}
inline int MultipartUpload::run(HttpRequest *req, bool setKeepAlive) {
   return MultipartUpload_run(this,req,setKeepAlive?TRUE:FALSE);
}
inline HttpConnection* MultipartUpload::getCon(MultipartUpload* o){
   return MultipartUpload_getCon(this); }
inline HttpServer* MultipartUpload::getServer(){
   return MultipartUpload_getServer(this);
}
#endif

/** @} */ /* end of HttpStack */
  

#endif
