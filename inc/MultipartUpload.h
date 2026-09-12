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
 *   $Id: MultipartUpload.h 5978 2026-09-11 16:13:48Z wini $
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

/** @file MultipartUpload.h */

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

/** Report the final multipart boundary.
    @param o Parser whose request has completed. This callback returns no value.
    In asynchronous mode, destroy the parser or call MultipartUpload_getCon() to
    detach it and transfer the connection. In synchronous run() mode, leave the
    object alive until run() returns; getCon() is unavailable in that mode.
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
     All strings/data are borrowed and valid only during the callback. Copy data
    that must be retained. In synchronous run() mode, do not free the parser in
    a callback; return nonzero to stop and clean up after run() returns.
    A nonzero callback result becomes MultipartUpload_UserRetErr internally;
    it is not passed to onError as an additional notification.
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
    \param contentTransferEncoding is the part's Content-Transfer-Encoding
    header value. The parser does not decode this encoding.
    This parameter is NULL if not specified in the multipart data stream.
    \return This function must return 0 on success. Any other value
    signals an error condition to the MultiPartUpload class.
    This method must call the MultipartUpload destructor or call
    MultipartUpload::getCon if returning
    a non zero value.
     All strings/data are borrowed and valid only during the callback. Copy data
    that must be retained. In synchronous run() mode, do not free the parser in
    a callback; return nonzero to stop and clean up after run() returns.
    A nonzero callback result becomes MultipartUpload_UserRetErr internally;
    it is not passed to onError as an additional notification.
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
    \param len Byte count, at most 65535. data=NULL and len=0 mark the end
    of the current file, not the end of the complete request.
    \return This function must return 0 on success. Any other value
    signals an error condition to the MultiPartUpload class.  This
    method must call the MultipartUpload destructor or call
    MultipartUpload::getCon if returning a non zero value.
     All strings/data are borrowed and valid only during the callback. Copy data
    that must be retained. In synchronous run() mode, do not free the parser in
    a callback; return nonzero to stop and clean up after run() returns.
    A nonzero callback result becomes MultipartUpload_UserRetErr internally;
    it is not passed to onError as an additional notification.
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
     The callback returns no value. In synchronous run() mode, leave the object
    alive and let run() return before freeing it. Not every initialization failure
    invokes onError; always check the start()/run() result as well.
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


/** Parser for multipart/form-data POST requests.
    Each field is delivered through formData; files are delivered through
    fileBegin and fileData callbacks. File bytes are streamed rather than stored
    as one complete file. Field values must fit in the parser's working storage.
    The parser does not create files or decode Content-Transfer-Encoding values.

    Use start() for dispatcher-driven input or run() for blocking input. All five
    callbacks are required. Callback cleanup rules differ between these modes:
    asynchronous completion/error must detach or destroy the parser; synchronous
    callbacks must keep its storage alive until run() returns. Do not reuse one
    parser concurrently or switch a completed blocking instance to asynchronous mode.
*/
typedef struct MultipartUpload
{
#ifdef __cplusplus
     /** Uninitialized storage; call MultipartUpload_constructor before use. */
     MultipartUpload() {}
 
      /**
         \param server Required borrowed server; it and its dispatcher must
         outlive the parser.

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

         \param bufferSize Initial buffer size and growth increment in bytes.
         Initialization uses at least 1024 bytes and may grow to hold buffered
         request data. Zero disables later growth. Growth increments above 50000
         are not used by the current implementation. This is working storage,
         not a total file-upload size limit.

         \param allocator Borrowed allocator with allocation, reallocation, and
         free support. NULL selects AllocatorIntf_getDefault(). Keep it alive
         until parser destruction. The input buffer is allocated by start()/run(),
         not by this constructor.
      */
      MultipartUpload(HttpServer* server,
                      MultipartUpload_EndOfReq onEndOfReq,
                      MultipartUpload_FormData onFormData,
                      MultipartUpload_FileBegin onFileBegin,
                      MultipartUpload_FileData onFileData,
                      MultipartUpload_Error onError,
                      U32 bufferSize,
                      AllocatorIntf* allocator = 0);

      /** Detach from the dispatcher, free parsing storage, and close the embedded
    asynchronous connection if still owned. In blocking mode the request retains
    its separate connection. The server and allocator are borrowed and not freed.
 */
      ~MultipartUpload();

      /** Start asynchronous multipart input, taking the request connection.
    @param req Required uncommitted POST multipart/form-data request with a live
    connection. The parser enables keep-alive and can send 100 Continue.
    @return Zero when initialized or when buffered-input processing has already
    invoked a completion/error callback. Negative initialization results include
    -1 committed response, -2 invalid connection, -3 wrong method/content type,
    -4 boundary setup failure, -5 buffer allocation failure, and E_MALLOC for
    request-data pushback failure. A callback may already have handled cleanup.
    Callbacks can run before this function returns. Do not assume a zero result
    means the object is still alive or the upload succeeded.
 */
      int start(HttpRequest *req);


      /** Parse multipart input synchronously, blocking for additional bytes.
    @param req Required current POST multipart/form-data request.
    @param setKeepAlive True (default) enables keep-alive when possible; false
    does not enable it. This does not force a close or override later errors.
    @return Zero on completion while the connection remains valid; negative
    initialization errors as for start(), or -1 for later processing/connection
    failure. Callbacks receive data during this call and must not free the parser.
    The request retains the connection; getCon() returns NULL in this mode.
 */
      int run(HttpRequest *req, bool setKeepAlive=true);

      /** Detach the asynchronous parser and release parsing buffers for a handoff.
    @param o Unused argument retained by the C++ wrapper; the operation acts on this.
    @return Borrowed embedded connection after detachment, or NULL in blocking
    mode. Move the returned connection into its new owner before destroying this
    parser. Detachment alone does not close or free that connection.
    @code
    // In an asynchronous callback, transfer the connection to a response object.
    HttpAsynchResp_constructor2(resp, buf, bufSize, MultipartUpload_getCon(upload));
    @endcode
 */
      HttpConnection* getCon(MultipartUpload* o);
 
      /** Get the server supplied to the constructor.
    @return Non-NULL borrowed server pointer; ownership does not change.
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
/** Construct a parser as described by MultipartUpload::MultipartUpload.
    @param o Required storage to initialize.
    @param server Required borrowed server.
    @param endOfReq Required completion callback.
    @param formData Required form-field callback.
    @param fileBegin Required file-header callback.
    @param fileData Required file-data callback.
    @param onError Required error callback.
    @param bufferSize Initial working-buffer size and growth increment in bytes.
    @param allocator Borrowed allocator, or NULL for the default allocator.
 */
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
/** @copydoc MultipartUpload::~MultipartUpload
    @param o Required initialized parser.
 */
BA_API void MultipartUpload_destructor(MultipartUpload* o);
/** @copydoc MultipartUpload::start
    @param o Required initialized parser.
 */
BA_API int MultipartUpload_start(MultipartUpload* o, HttpRequest *req);
/** @copydoc MultipartUpload::run
    @param o Required initialized parser.
 */
BA_API int MultipartUpload_run(
   MultipartUpload* o, HttpRequest *req, BaBool setKeepAlive);
/** Detach and prepare an asynchronous connection handoff.
    @param o Required initialized parser on which to act.
    @return Borrowed embedded connection, or NULL in blocking mode; see MultipartUpload::getCon.
 */
BA_API HttpConnection* MultipartUpload_getCon(MultipartUpload* o); 
/** @copydoc MultipartUpload::getServer
    @param o Required initialized parser.
 */
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
