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
 *   $Id: WebDAV.h 5978 2026-09-11 16:13:48Z wini $
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

/** @file WebDAV.h */

#ifndef __WebDAV_h
#define __WebDAV_h

#include <SingleList.h>
#include "HttpUpload.h"

struct WebDAV;

#ifndef __DOXYGEN__
typedef struct
{
      SplayTree lTree; /* Lock node tree. */
      SingleList lList; /* Lock node list. */
      struct WebDAV* webDAV;
      char* lockDir;
      char* lockDirFn;
      U32 nextLockFileNumber;
      U32 locksLeft; /* Max number of allowed locks left to allocate. */
} LockContainer;
#endif



/** The WebDAV plugin implements WebDAV version 1, as specified in
<a href="http://www.ietf.org/rfc/rfc2518.txt">RFC2518</a> and part of
the locking in DAV 2, making it interoperate with "drive" clients
requiring locking.

The purpose of the WebDAV plugin is to make it possible to use the
server as a remote disk. The locking in DAV 2 is partly implemented in
order to make it interoperate with "drive" clients requiring
locking. A "drive" client such as 
<a href="http://dav.sourceforge.net/">davfs2</a>
can then be mounted as a drive.

Some filesystem clients require locking for writable access. The WebDAV plugin needs to store the locking
information in files on the file system. The "lockDir" argument in the
constructor must point to an existing directory or to an existing
parent directory. The WebDAV plugin attempts to create this directory
if it does not exist and marks it as hidden for file systems
supporting hidden files.

Not all clients require locking. For example, Web Folders work without
locking. You can set the "lockDir" argument to NULL if you do not want
to support the simple locking implemented in the WebDAV plugin. As an
example, Mac OS X will mount the drive as read only.

The simple locking implemented in the WebDAV plugin also requires a
dynamic object per active lock. The maxNumberOfLocks constructor
argument controls how many concurrent locks the server allows. For
example, this can be set to 2-4 for a standard embedded system.

The WebDAV plugin creates a subdirectory ".DAV" if a client sends a "set
<a href="http://greenbytes.de/tech/webdav/rfc2518.html#rfc.iref.8">
dead property</a> request".
This sub directory contains XML files for storing the dead
properties. The .DAV directory is only created if the client creates
dead properties. The .DAV directory will be marked as hidden for
file systems supporting hidden files.

\sa HttpResMgr
*/
typedef struct WebDAV
#ifdef __cplusplus
: public HttpDir
{ 
      /** Create a WebDAV instance. You can create several instances
       of this class and install several instances into the virtual
       file system.

       \param io Required borrowed IoIntf implementation, which must outlive
       the WebDAV instance and all active transfers. For example, DiskIo. The WebDAV
       plugin can also work with "read only" IoIntf implementations
       such as the ZipIo. The WebDAV plugin allows method OPTIONS,
       HEAD, GET, and PROPFIND for a "read only" IoIntf
       implementation.

       \param maxUploads Maximum concurrent asynchronous uploads. Values less
       than or equal to zero are coerced to 1, matching HttpResMgr. This limits
       simultaneous requests, not uploaded bytes. It does not limit downloads.

       \param dirName Borrowed HttpDir name; retain its storage while installed.
       NULL creates an unnamed directory according to HttpDir rules.

       \param lockDir NUL-terminated lock-directory path relative to io, copied
       during construction. NULL disables locking. A missing directory is created
       when possible; read-only filesystems disable locking. Setup failure also
       leaves locking unavailable; this void constructor does not return that error.

       \param maxNumberOfLocks Maximum simultaneous locks, default 10. Zero
       disables locking. Requires a usable lockDir.

       \param alloc Borrowed allocator for internal allocation/reallocation/free;
       NULL selects AllocatorIntf_getDefault(). Retain it through destruction.

       \param priority Signed 8-bit HttpDir priority, default zero.
 
      */
      WebDAV(IoIntf* io,
             int maxUploads,
             const char* dirName, 
             const char* lockDir,
             U32 maxNumberOfLocks=10,
             AllocatorIntf* alloc=0,
             S8 priority=0);

      /**  Terminate the WebDAV object. Please note that any
           upload in progress will be aborted. Detach the directory and stop active
           users before destruction. The borrowed IoIntf and allocator are not freed.
       */
      ~WebDAV();

#if 0
}
#endif
#else
{
   HttpDir super; /* Inherits from HttpDir. */
#endif
   HttpUploadCbIntf uploadCb;
   LockContainer lock;
   HttpUpload upload;
   IoIntf* io;
   AllocatorIntf* alloc;
   char* vdRootPath;
   size_t vdRootPathLen;
   BaBool ioReadOnly; /* TRUE if the IoIntf is read only. */
   BaBool ioMoveDir; /* TRUE if the IoIntf can move directories. */
} WebDAV;


#ifdef __cplusplus
extern "C" {
#endif

/** @copydoc WebDAV::WebDAV
    @param o Required storage to initialize.
 */
BA_API void WebDAV_constructor(
   WebDAV* o,
   IoIntf* io,
   int maxUploads,
   const char* dirName, 
   const char* lockDir,
   U32 maxNumberOfLocks,
   AllocatorIntf* alloc,
   S8 priority);

/** @copydoc WebDAV::~WebDAV
    @param o Required initialized WebDAV instance.
 */
BA_API void WebDAV_destructor(WebDAV* o);

/** Configure inherited authentication and authorization.
    @param o Required initialized WebDAV instance.
    @param authenticator Borrowed AuthenticatorIntf pointer, or NULL.
    @param realm Borrowed AuthorizerIntf pointer, or NULL; the historical macro
    name does not describe a realm string. See HttpDir::setAuthenticator.
 */
#define WebDAV_setAuthenticator(o, authenticator, realm) \
   HttpDir_setAuthenticator((HttpDir*)o, authenticator, realm)

/** Input/output record for WebDAV_lockmgr(). */
typedef struct
{
      /* 0:is-locked, 1:lock, 2:unlock, 3:lock-owner,
      */
      /** Input: 0 tests a lock, 1 creates, 2 removes, 3 reads owner XML. */
      int action; 
      /** Action 1 input: duration in seconds. Action 3 output: absolute Unix expiry time, including the implementation's 60-second allowance. */
      BaTime lockTime; /* in val for action 1 */
      /** Required input resource path relative to the WebDAV IoIntf, NUL-terminated and borrowed for the call. */
      const char* name; /* in val: the file */
      /** Action 1 input: required NUL-terminated owner text inserted into an XML href element. Supply XML-escaped content; the implementation does not escape it. */
      const char* owner; /* in val for action 1 */
      /** Action 3 output only: opened owner-XML resource, or NULL if opening fails. Caller must close a non-NULL handle. */
      ResIntfPtr fp; /* out val: the owner XML file */
} WebDAVLockMgr;
 

/** Manage WebDAV locks without an HTTP request.
    @param o Required initialized WebDAV instance.
    @param mgr Required input/output record; initialize the fields for its action.
    @return Action 0: 1 if locked, 0 otherwise. Other actions: 0 when handled.
    Errors: -1 locking disabled, -2 already locked, -3 lock not found,
    -10 creation/general failure or unknown action.
    @note Action 1 does not report owner-file write/close errors. Action 3 can
    return zero with fp=NULL; always test fp before reading. No ownership of
    input strings transfers, and other output fields are not reset on error.
 */
BA_API int WebDAV_lockmgr(WebDAV* o, WebDAVLockMgr* mgr);



#ifdef __cplusplus
}
inline WebDAV::WebDAV(IoIntf* io,
                      int maxUploads,
                      const char* dirName, 
                      const char* lockDir,
                      U32 maxNumberOfLocks,
                      AllocatorIntf* alloc,
                      S8 priority) {
   WebDAV_constructor(this,io,maxUploads,dirName, lockDir,
                      maxNumberOfLocks, alloc,priority);
}

inline WebDAV::~WebDAV(){
   WebDAV_destructor(this);
}
#endif

#endif /* __WebDAV_h */

