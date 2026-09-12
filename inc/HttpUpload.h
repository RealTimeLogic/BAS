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
 *   $Id: HttpUpload.h 5978 2026-09-11 16:13:48Z wini $
 *
 *   COPYRIGHT:  Real Time Logic LLC, 2006-2026
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

/** @file HttpUpload.h */

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
      /** @return Borrowed NUL-terminated path within the I/O interface.
       * PUT returns the destination path; multipart POST returns the current
       * file path, or the destination directory before a file is selected.
       * Copy it before the next file callback or node destruction. */
      const char* getName();

      /** @return Borrowed URL generated from the request's current directory
       * using HttpResponse::encodeRedirectURL with an empty relative path.
       * It can include session encoding; it is not necessarily the original
       * request URI or a URL identifying the current multipart file.
       * Valid until node destruction. */
      const char* getUrl();

      /** Switch from receiving the upload to producing its response.
       * Calling before completion aborts reception. Send the desired response
       * from the callback, including on success; obtaining the object alone
       * does not establish that any response was delivered.
       * @return Borrowed embedded response object, never NULL for a valid node.
       * Its connection can be unusable; check the response operation results.
       * Do not destroy or free this object separately from the upload node.
       */
      HttpAsynchResp* getResponse();

      /** @return Borrowed receive connection only after reception has reached
       * a state that needs no lingering close and before response mode.
       * Returns NULL while an incomplete upload needs closing or after response
       * mode starts. The pointer is not an ownership transfer. */
      struct HttpConnection* getConnection();

      /** @return Borrowed destination I/O interface owned by the application. */
      IoIntfPtr getIoIntf();

      /** @return Borrowed session found by its saved ID, or NULL if no session
          exists or it has expired.

          The session object may expire at any time. See the
          explanation in the HttpSession for more information.

          \sa HttpSession::incrRefCntr
       */
      HttpSession* getSession();


      /** @return True for multipart POST, false for PUT. */
      bool isMultipartUpload();
};
#else
struct HttpUploadNode;
#endif

#ifdef __cplusplus
extern "C" {
#endif
/** @copydoc HttpUploadNode::getName
 * @param[in,out] o Live upload node. */
BA_API const char* HttpUploadNode_getName(struct HttpUploadNode* o);
/** @copydoc HttpUploadNode::getUrl
 * @param[in,out] o Live upload node. */
BA_API const char* HttpUploadNode_getUrl(struct HttpUploadNode* o);
/** @copydoc HttpUploadNode::getResponse
 * @param[in,out] o Live upload node. */
BA_API HttpAsynchResp* HttpUploadNode_getResponse(struct HttpUploadNode* o);
/** @copydoc HttpUploadNode::getConnection
 * @param[in,out] o Live upload node. */
BA_API struct HttpConnection* HttpUploadNode_getConnection(
   struct HttpUploadNode* o);
/** @copydoc HttpUploadNode::getIoIntf
 * @param[in,out] o Live upload node. */
BA_API IoIntfPtr HttpUploadNode_getIoIntf(struct HttpUploadNode* o);
/** @param[in] o Live upload node.
 * @return Borrowed userdata passed to service, possibly NULL. */
BA_API void* HttpUploadNode_getdata(struct HttpUploadNode* o);
/** @copydoc HttpUploadNode::getSession
 * @param[in,out] o Live upload node. */
BA_API HttpSession* HttpUploadNode_getSession(struct HttpUploadNode* o);
/** @copydoc HttpUploadNode::isMultipartUpload
 * @param[in,out] o Live upload node. */
BA_API BaBool HttpUploadNode_isMultipartUpload(struct HttpUploadNode* o);
/** @param[in] o Live upload node.
 * @return TRUE after an asynchronous response connection has been installed;
 * FALSE otherwise. This does not test whether sending will succeed. */
BA_API BaBool HttpUploadNode_isResponseMode(struct HttpUploadNode* o);
/** @param[in] o Live upload node.
 * @return Saved HttpResponse_initial state from the command at upload startup. */
BA_API BaBool HttpUploadNode_initial(struct HttpUploadNode* o);
/** Release one retained reference.
 * @param[in,out] o Live node with a nonzero reference count.
 * @return -1 if the count reached zero and the node was destroyed, otherwise 0.
 * Do not access o after -1. Do not decrement an internal callback reference. */
BA_API int HttpUploadNode_decrRef(struct HttpUploadNode* o);
/** Retain a node beyond the current callback.
 * @param[in,out] o Live node; increments its U8 reference counter. Do not exceed
 * 255 total references, including internal callback references. Pair each
 * external increment with decrRef; stop external use before parent destruction.
 * Serialize access with the server. This does not make the node thread-safe. */
BA_API void HttpUploadNode_incRef(struct HttpUploadNode* o);
/** Install the process-wide gzip upload adapter in builds with zlib support.
 * @param[in] ptr Callback used to open gzip-decoding output resources, or NULL
 * to disable that adaptation. Configure before concurrent uploads begin.
 * @sa IoIntf_InflateGzip */
BA_API void set_inflategzip(IoIntf_InflateGzip ptr);
/** @return Installed process-wide gzip upload adapter, or NULL.
 * Available in builds with zlib support. No ownership is transferred. */
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

/** Notify the application about a file or completed request.
 * @param[in,out] o Borrowed callback interface installed in HttpUpload.
 * @param[in,out] node Borrowed upload node, valid during the callback.
 * @param[in] completed FALSE before opening each multipart file; TRUE when
 * the complete upload request finishes and its last file closes successfully.
 * PUT has a completion callback but no FALSE start notification.
 * A multipart request can contain several files; TRUE is not a per-file event.
 * Call HttpUploadNode_getResponse to reject a file before opening it or to send
 * the final response. Callbacks can run synchronously during service, or later.
 * Do not destroy the parent HttpUpload from a callback. */
typedef void (*HttpUploadCbIntf_OnFile)(
   struct HttpUploadCbIntf* o, struct HttpUploadNode* node, BaBool completed);
/** Report upload failure or parent shutdown.
 * @param[in,out] o Borrowed installed callback interface.
 * @param[in,out] node Borrowed upload node, valid during the callback.
 * @param[in] ecode I/O error code, or FE_SOCKET for receive/parser failures.
 * Parent shutdown reports IOINTF_IOERROR.
 * @param[in] extraEcode Borrowed optional error text, valid during the callback;
 * copy it if retaining it. Do not interpret it as a stable machine-readable code.
 * Send any desired error response using HttpUploadNode_getResponse. The socket
 * may already be unusable. Files can be partially written; cleanup is the
 * application's responsibility. The library does not free userdata. */
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
      /** Uninitialized storage; install both callbacks before use. */
      HttpUploadCbIntf(){}

      /** Initialize a HttpUploadCbIntf interface
      \param of Required file/completion callback; see HttpUploadCbIntf_OnFile.
      \param oe Required failure callback; see HttpUploadCbIntf_OnError.
      */
      HttpUploadCbIntf(HttpUploadCbIntf_OnFile of,HttpUploadCbIntf_OnError oe);
#endif
      HttpUploadCbIntf_OnFile onFileFp;
      HttpUploadCbIntf_OnError onErrorFp;
} HttpUploadCbIntf;

/** Install upload callbacks without allocating memory.
 * @param[out] o Caller-owned callback interface, valid throughout upload use.
 * @param[in] onFile Required file/completion callback.
 * @param[in] onError Required failure callback. */
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
      /** Uninitialized storage; initialize before use or destruction. */
      HttpUpload() {}

      /** Initialize an HttpUpload instance.

          \param io Required borrowed writable I/O interface, valid until all
          uploads are destroyed.

          \param alloc Required borrowed allocator, valid until all uploads are
          destroyed. Pass AllocatorIntf_getDefault() to use the default.

          \param uploadCb Required borrowed callback interface with both callbacks
          installed; must outlive this uploader and every active node.

          \param maxUploads Nonnegative maximum number of concurrent requests,
          not a byte limit or a limit on files within one multipart request.
          Zero disables uploads. The HttpUpload::service method sends a 503 HTTP
          response if the maximum number of concurrent uploads are
          reached.
      */
      HttpUpload(IoIntfPtr io, AllocatorIntf* alloc,
                 HttpUploadCbIntf* uploadCb, int maxUploads);

      /** Terminate the HttpUpload instance and all active
       * HttpUploadNode instances. Calls the error callback for each active
       * node. Do not call from an upload callback or while external node
       * references remain. The I/O interface, allocator and callback interface
       * are not destroyed; caller-provided userdata is not freed. */
      ~HttpUpload();

      /** The HttpUpload service method. This method is typically
          called from a HttpDir or HttpPage service method.
          \param name Required NUL-terminated destination, copied before return:
          \li if PUT: path + name relative to the I/O interface.
          \li if POST: directory path relative to the I/O interface, ending in a slash
          when nonempty. The full
          path+name is constructed from the name in the multipart
          message.
          \param cmd Borrowed current command; call from its service handler.
          An accepted upload takes over asynchronous request processing.
          \param userdata Optional borrowed context (default NULL), retrievable
          with HttpUploadNode_getdata. Keep it valid until callbacks finish.
          The uploader does not free it.
          \return -1 for an unsupported request, 0 when accepted, or 1 on
          startup failure. See #HttpUpload_service for callback-state ownership.
       */
      int service(const char* name, HttpCommand* cmd, void* userdata=0);

      /** @return Borrowed destination I/O interface passed to construction.
       */
      /** @return Borrowed destination I/O interface owned by the application. */
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
/** @copydoc HttpUpload::HttpUpload(IoIntfPtr, AllocatorIntf*, HttpUploadCbIntf*, int)
 * @param[out] o Caller-owned uploader storage. */
BA_API void HttpUpload_constructor(
   HttpUpload* o, IoIntfPtr io, AllocatorIntf* alloc,
   HttpUploadCbIntf* uploadCb, int maxUploads);
/** Abort active uploads and release their storage.
 * @param[in,out] o Initialized uploader; not in a callback and no external node
 * references outstanding. See HttpUpload::~HttpUpload for callback/lifetime rules. */
BA_API void HttpUpload_destructor(HttpUpload* o);
/** Start an upload for a PUT or multipart POST request.
    @param[in,out] o Initialized uploader.
    @param[in] name Required copied destination path. For multipart POST,
    use a directory path ending in a slash when nonempty.
    @param[in,out] cmd Borrowed current command; accepted requests become asynchronous.
    @param[in] userdata Borrowed callback context, or NULL; never freed by the uploader.
    @return -1 unsupported request, 0 accepted, 1 startup failure.
    Returns -1 for an unsupported request, 0 when accepted (callbacks may
    already have run), or 1 when startup failed.
    On startup failure the caller must release userdata unless a synchronous
    callback already released it. Existing HTTP error responses are preserved.
 */
BA_API int HttpUpload_service(
   HttpUpload* o, const char* name, HttpCommand* cmd, void* userdata);
/** @param[in] o Initialized uploader.
 * @return Borrowed destination I/O interface. */
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
