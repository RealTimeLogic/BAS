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
 *   $Id: HttpUpload.h 4915 2021-12-01 18:26:55Z wini $
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

#ifndef _HttpUpload_h
#define _HttpUpload_h

#include <HttpAsynchReq.h>
#include <MultipartUpload.h>
#include <AuthenticatedUser.h>
#include <IoIntf.h>

struct HttpConnection;

/** @defgroup HttpUpload HttpUpload
    @ingroup HttpStack

    HttpUpload is a collection of classes that makes it easy to
    implement a remote file manager. HttpUpload which is internally
    using MultipartUpload, HttpAsynchReq, and HttpAsynchResp enables
    you to easily design code for uploading files using HTTP PUT and
    multipart POST.

    The HttpResMgr in the example directory is a full file
    manager implementation that is internally using HttpUpload when
    the client requests a file upload. The following HttpDir C++
    example shows how to use HttpUpload:

    \verbinclude HttpUploadExample.cpp

    @{
*/

#ifdef __cplusplus
/** A HttpUploadNode is dynamically created by an HttpUpload instance
    for each concurrent upload. The HttpUploadNode is provided as one
    of the arguments to the HttpUploadCbIntf callback methods.
 */
struct HttpUploadNode
{
      /** Returns the file name including the relative path */
      const char* getName();

      /** Returns the full URL the client used when sending data */
      const char* getUrl();

      /** Fetch the response object. Please note that calling this
          function terminates the current upload if not completed. The
          method may return NULL if the socket connection is
          broken.
      */
      HttpAsynchResp* getResponse();

      struct HttpConnection* getConnection();

      IoIntfPtr getIoIntf();

      /** Returns the HttpSession object or null if no session object.

          The session object may expire at any time. See the
          explanation in the HttpSession for more information.

          \sa HttpSession::incrRefCntr
       */
      HttpSession* getSession();


      /** Returns true if HTTP POST. False for HTTP PUT */
      bool isMultipartUpload();
};
#else
struct HttpUploadNode;
#endif

#ifdef __cplusplus
extern "C" {
#endif
BA_API const char* HttpUploadNode_getName(struct HttpUploadNode* o);
BA_API const char* HttpUploadNode_getUrl(struct HttpUploadNode* o);
BA_API HttpAsynchResp* HttpUploadNode_getResponse(struct HttpUploadNode* o);
BA_API struct HttpConnection* HttpUploadNode_getConnection(
   struct HttpUploadNode* o);
BA_API IoIntfPtr HttpUploadNode_getIoIntf(struct HttpUploadNode* o);
BA_API void* HttpUploadNode_getdata(struct HttpUploadNode* o);
BA_API HttpSession* HttpUploadNode_getSession(struct HttpUploadNode* o);
BA_API BaBool HttpUploadNode_isMultipartUpload(struct HttpUploadNode* o);
BA_API BaBool HttpUploadNode_isResponseMode(struct HttpUploadNode* o);
BA_API BaBool HttpUploadNode_initial(struct HttpUploadNode* o);
BA_API int HttpUploadNode_decrRef(struct HttpUploadNode* o);
BA_API void HttpUploadNode_incRef(struct HttpUploadNode* o);
BA_API void set_inflategzip(IoIntf_InflateGzip ptr);
BA_API IoIntf_InflateGzip get_inflategzip(void);
#ifdef __cplusplus
}
inline const char* HttpUploadNode::getName() {
   return HttpUploadNode_getName(this); }
inline const char* HttpUploadNode::getUrl() {
   return  HttpUploadNode_getUrl(this); }
inline HttpAsynchResp* HttpUploadNode::getResponse() {
   return HttpUploadNode_getResponse(this); }
inline struct HttpConnection* HttpUploadNode::getConnection() {
   return HttpUploadNode_getConnection(this); }
inline IoIntfPtr HttpUploadNode::getIoIntf() {
   return HttpUploadNode_getIoIntf(this); }
inline  HttpSession* HttpUploadNode::getSession() {
   return HttpUploadNode_getSession(this); }
inline bool HttpUploadNode::isMultipartUpload() {
   return HttpUploadNode_isMultipartUpload(this) ? true : false; }
#endif



#ifndef __DOXYGEN__
struct HttpUploadCbIntf;
#endif

typedef void (*HttpUploadCbIntf_OnFile)(
   struct HttpUploadCbIntf* o, struct HttpUploadNode* node, BaBool completed);
typedef void (*HttpUploadCbIntf_OnError)(
   struct HttpUploadCbIntf* o, struct HttpUploadNode* node,
   int ecode, const char* extraEcode);

/** The HttpUploadCbIntf interface is an abstract class that must be
    implemented by code using the HttpUpload. The HttpUploadCbIntf
    methods are called at start of upload, end of upload, and if the
    upload failed.
 */
typedef struct HttpUploadCbIntf
{
#ifdef __cplusplus
      HttpUploadCbIntf(){}

      /** Initialize a HttpUploadCbIntf interface
      \param of is the callback called at start or end of an upload.
      \param oe is the error callback method called if the upload failed.
      */
      HttpUploadCbIntf(HttpUploadCbIntf_OnFile of,HttpUploadCbIntf_OnError oe);
#endif
      HttpUploadCbIntf_OnFile onFileFp;
      HttpUploadCbIntf_OnError onErrorFp;
} HttpUploadCbIntf;

#define HttpUploadCbIntf_constructor(o, onFile, onError) do { \
   (o)->onFileFp=onFile; \
   (o)->onErrorFp=onError; \
} while(0)

#ifdef __cplusplus
      inline HttpUploadCbIntf::HttpUploadCbIntf(
         HttpUploadCbIntf_OnFile of,HttpUploadCbIntf_OnError oe) {
         HttpUploadCbIntf_constructor(this, of, oe);
      }
#endif


/** The HttpUpload node is responsible for creating and starting
    HttpUploadNode instances. The class can create N concurrent
    HttpUploadNodes, where N is controlled by the 'maxUploads'
    attribute.
 */
typedef struct HttpUpload
{
#ifdef __cplusplus
      HttpUpload() {}

      /** Initialize an HttpUpload instance.

          \param io is a IoIntf implementation such as DiskIo.

          \param alloc is the allocator used when creating
          HttpUploadNode instances.

          \param uploadCb is the HttpUploadCbIntf implementation.

          \param maxUploads is the maximum number of concurrent
          uploads. The HttpUpload::service method sends a 503 HTTP
          response if the maximum number of concurrent uploads are
          reached.
      */
      HttpUpload(IoIntfPtr io, AllocatorIntf* alloc,
                 HttpUploadCbIntf* uploadCb, int maxUploads);

      /** Terminate the HttpUpload instance and all active
       * HttpUploadNode instances. */
      ~HttpUpload();

      /** The HttpUpload service method. This method is typically
          called from a HttpDir or HttpPage service method.
          \param name is:
          \li if PUT: path + name relative to the I/O intf.
          \li if POST: path relative to the I/O intf. The full
          path+name is constructed from the name in the multipart
          message.
          \param cmd is the request/response container object.
          \param userdata is an optional reference that is set in the HttpUploadNode.
       */
      int service(const char* name, HttpCommand* cmd, void* userdata=0);

      /** Return a pointer to the IoIntf implementation.
       */
      IoIntfPtr getIoIntf();
#endif

      DoubleList uploadNodeList;
      IoIntfPtr io;
      AllocatorIntf* alloc;
      HttpUploadCbIntf* uploadCb;
      int uploadsLeft;
} HttpUpload;

#ifdef __cplusplus
extern "C" {
#endif
BA_API void HttpUpload_constructor(
   HttpUpload* o, IoIntfPtr io, AllocatorIntf* alloc,
   HttpUploadCbIntf* uploadCb, int maxUploads);
BA_API void HttpUpload_destructor(HttpUpload* o);
BA_API int HttpUpload_service(
   HttpUpload* o, const char* name, HttpCommand* cmd, void* userdata);
#define HttpUpload_getIoIntf(o) (o)->io
#ifdef __cplusplus
}
inline HttpUpload::HttpUpload(IoIntfPtr io, AllocatorIntf* alloc,
                              HttpUploadCbIntf* uploadCb, int maxUploads) {
   HttpUpload_constructor(this, io, alloc, uploadCb, maxUploads); }
inline HttpUpload::~HttpUpload() {
    HttpUpload_destructor(this); }
inline int HttpUpload::service(const char* name, HttpCommand* cmd, void* userdata) {
   return  HttpUpload_service(this, name, cmd,userdata); }
inline IoIntfPtr HttpUpload::getIoIntf() {
   return HttpUpload_getIoIntf(this); }
#endif

/** @} */ /* end of HttpUpload */

#endif
