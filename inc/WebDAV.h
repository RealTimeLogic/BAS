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
 *   $Id: WebDAV.h 4915 2021-12-01 18:26:55Z wini $
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

For some unknown reason, some "drive" clients, such as Mac OS X,
require locking. The WebDAV plugin needs to store the locking
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

       \param io is a IoIntf implementation such as DiskIo. The WebDAV
       plugin can also work with "read only" IoIntf implementations
       such as the ZipIo. The WebDAV plugin allows method OPTIONS,
       HEAD, GET, and PROPFIND for a "read only" IoIntf
       implementation.

       \param maxUploads is the limit value for concurrent
       asynchronous uploads. Downloads are also using asynchronous
       sockets, but it is not possible to set a limit on the number of
       downloads.

       \param dirName is the HttpDir name.

       \param lockDir is the name of the lock directory. See explanation above.

       \param maxNumberOfLocks is the maximum number of WebDAV
       locks. See explanation above for max number of locks.

       \param alloc is the allocator used for all internal memory
       allocations and re-allocations. The WebDAV plugin relies on
       heavy use of dynamic memory.

       \param priority is the HttpDir priority.
 
      */
      WebDAV(IoIntf* io,
             int maxUploads,
             const char* dirName, 
             const char* lockDir,
             U32 maxNumberOfLocks=10,
             AllocatorIntf* alloc=0,
             S8 priority=0);

      /**  Terminate the WebDAV object. Please note that any
           upload in progress will be aborted.
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

BA_API void WebDAV_constructor(
   WebDAV* o,
   IoIntf* io,
   int maxUploads,
   const char* dirName, 
   const char* lockDir,
   U32 maxNumberOfLocks,
   AllocatorIntf* alloc,
   S8 priority);

BA_API void WebDAV_destructor(WebDAV* o);

#define WebDAV_setAuthenticator(o, authenticator, realm) \
   HttpDir_setAuthenticator((HttpDir*)o, authenticator, realm)

typedef struct
{
      /* 0:is-locked, 1:lock, 2:unlock, 3:lock-owner,
      */
      int action; 
      BaTime lockTime; /* in val for action 1 */
      const char* name; /* in val: the file */
      const char* owner; /* in val for action 1 */
      ResIntfPtr fp; /* out val: the owner XML file */
} WebDAVLockMgr;
 

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

