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
 *   $Id: IoIntf.h 5978 2026-09-11 16:13:48Z wini $
 *
 *   COPYRIGHT:  Real Time Logic LLC, 2006-2018
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

/** @file IoIntf.h */

#ifndef __IoIntf_h
#define __IoIntf_h
 
#include "GenPrimT.h"
#include "ThreadLib.h"

/** @ingroup IO
   @{
 */
 


/** Error codes (status)
 */
#define IOINTF_OK 0

/** End of file from ResIntf_Read
 */
#define IOINTF_EOF 1


/** Invalid name or name not accepted by IOINTF implementation.
    A DOS 8.3 file system may return this code for long file names.
*/
#define IOINTF_INVALIDNAME -11

/** Resource not found.
 */
#define IOINTF_NOTFOUND -12

/** Resource exists and cannot be overwritten.
 */
#define IOINTF_EXIST -13

/** Path (parent directory) not found.
 */
#define IOINTF_ENOENT -14

/** No access or resource locked by file system.
 */
#define IOINTF_NOACCESS -15

/** A directory resource is not empty.
 */
#define IOINTF_NOTEMPTY -16

/** No space left on device.
 */
#define IOINTF_NOSPACE -17

/** Some kind of IO error. The extra error code may contain more information.
 */
#define IOINTF_IOERROR -18

/** Memory allocation error when working with resource.
*/
#define IOINTF_MEM -19

#define IOINTF_LOCKED -20


/** Method not implemented.
 */
#define IOINTF_NOIMPLEMENTATION -50

/** The provided buffer is too small.
 */
#define IOINTF_BUFTOOSMALL -51


/** IoIntf_OpenRes cannot uncompress the file since no NO_ZLIB is defined
 */
#define IOINTF_NOZIPLIB -100

/** IoIntf_OpenResGzip is not willing to compress the data. This
 * informs the caller that IoIntf_OpenRes must be used instead.
 */
#define IOINTF_NOTCOMPRESSED -101

/** Error in compressed data.
 */
#define IOINTF_ZIPERROR -102



/** Encrypted ZIP file requires AES, but AES is not enabled in
 * ZipIo.c. Recompile without NO_SHARKSSL.
 */
#define IOINTF_NOAESLIB -200

/** Unknown AES encryption or not an AES encrypted ZIP file.
 */ 
#define IOINTF_AES_NO_SUPPORT -201
/** File is AES encrypted, but password was not entered.
 */
#define IOINTF_NO_PASSWORD -202

/** Wrong password for AES encrypted file.
 */
#define IOINTF_WRONG_PASSWORD -203

/** Password does not match password in the file being accessed in the
    ZIP file. The most likely cause is corrupted ZIP file or a
    compromised ZIP file.
 */
#define IOINTF_AES_WRONG_AUTH -204

/** The file being accessed in the ZIP file is changed from an AES
    encrypted file to a non-encrypted file. Detection for this error is
    enabled by the 'passwordRequired' argument to function
    IoIntf_setPassword.
 */
#define IOINTF_AES_COMPROMISED -205

/* (Forward) Declarations
 */ 

#ifndef __DOXYGEN__
struct IoIntf;
typedef struct IoIntf* IoIntfPtr;
struct DirIntf;
typedef struct DirIntf* DirIntfPtr;
struct ResIntf;
typedef struct ResIntf* ResIntfPtr;
#endif

/** Resource information.
 */
typedef struct
{
      /** Last modification time in Unix seconds; availability and precision depend on the filesystem. */
      BaTime lastModified;
      /** File size in bytes. A directory size has no portable meaning. */
      BaFileSize size;
      /** TRUE for a directory, FALSE for a file. Read metadata only after successful stat. */
      BaBool isDir;
} IoStat;

/** Open resource read */
#define OpenRes_READ 1

/** Open resource write */
#define OpenRes_WRITE 2

/** Open resource and append. Default is to truncate */
#define OpenRes_APPEND 4




#ifdef __cplusplus
extern "C" {
#endif

/** wrapper for IoIntf_Property: 'pl'.  \param o a pointer to the
    IoIntf implementation.  \param password the required password for
    accessing the resources. ZipIo copies it; it must be non-NULL.
    \param passwordLen the length of the
    password (in bytes), can be zero when the password is ASCII
    format, and in this case, the length is calculated with strlen.
    ZipIo stores this length in U16, so keep the actual password at most 65535 bytes.
    \returns Zero when handled, nonzero if unsupported or failed. The current
    ZipIo text-password path does not report its copy-allocation failure.
*/
BA_API int IoIntf_setPassword(
   IoIntfPtr o, const char* password, size_t passwordLen);


/** wrapper for IoIntf_Property: 'pp'.
    \param o a pointer to the IoIntf implementation.
    \param passwordRequired Set to TRUE if a password must be set on
    all files in the ZIP file. This prevents a hacker from replacing a
    password protected file with a non password protected file.
    \param passwordBin Set to TRUE if the password is binary; it will
    be translated to an ASCII string.
    \returns 0 on success or a non zero value if setting password
	 properties not implemented by the IoIntf implementation.
*/
BA_API int IoIntf_setPasswordProp(
   IoIntfPtr o, BaBool passwordRequired, BaBool passwordBin);

/** wrapper for IoIntf_Property: 'abs'.
    Returns the physical absolute path for argument 'path' if the
    underlying IoIntf implementation is a DiskIo.
    \param o a pointer to the IoIntf implementation.
    \param path the path to convert.
    \returns the absolute path. The method returns NULL if the
    operation is not supported or if an error occurs. The returned
    pointer must be released by using baFree.
 */
BA_API char* IoIntf_getAbspath(IoIntfPtr o, const char* path);


/** Query the implementation's type property.
    @param o Required initialized filesystem with propertyFp.
    @param type Required output pointer for a borrowed NUL-terminated type string,
    such as "disk" or "zip". Do not free the returned string.
    @param platform Optional output pointer for a borrowed platform string; may
    equal the type where no separate platform is reported. NULL omits it.
    @return Zero on success, nonzero if unsupported or failed. Outputs are valid
    only on success and should be copied before filesystem destruction.
 */
BA_API int IoIntf_getType(IoIntfPtr o,const char** type,const char** platform);

/** wrapper for IoIntf_Property: 'aes'.
    \param o a pointer to the IoIntf implementation.
    \param name the file name.
    \param isEncrypted Required output pointer receiving TRUE or FALSE on success.
    \returns Zero when the encryption flag was obtained. Nonzero when
    unsupported, not found, or another error occurs; do not use the output then.
*/
BA_API int IoIntf_isEncrypted(
   IoIntfPtr o,const char* name,BaBool* isEncrypted);


/** Invoke the implementation's "destructor" property.
    @param o Required initialized filesystem implementing that property. Close
    resources/iterators and detach all users first. Implementation-owned storage
    is released; the caller still owns the IoIntf object itself.
    There is no error return; debug builds assert that the property succeeds.
 */
BA_API void IoIntf_destructor(IoIntfPtr o);


/** Open a destination that accepts gzip bytes and stores decompressed data.
    @param io Required writable filesystem, borrowed until the resource is closed.
    @param name Required NUL-terminated destination path, used during opening.
    @param status Required I/O status output.
    @param ecode Optional output for a borrowed implementation error string.
    @return Owned writable resource, or NULL on failure. Check write and close
    results for gzip/data errors. Opening may create or truncate the destination.
    Supplied by the optional BaGzip adapter.
 */
typedef ResIntfPtr (*IoIntf_InflateGzip)(
   IoIntfPtr io, const char* name, int* status, const char** ecode);


/** Optionally compress an open resource or range into temporary gzip storage.
    @param resPtr Required readable resource at the start of the desired input.
    @param name Required NUL-terminated resource name used for compression policy.
    @param m Optional caller-owned, already-locked mutex, released/reacquired while
    producing temporary data.
    @param size Required input/output byte count: input amount selected by the
    caller, output gzip size when compressed. The adapter applies its size policy.
    @param isCompressed Required output: TRUE when compression was selected,
    FALSE when declined. TRUE alone does not indicate successful compression.
    @return Original resource unchanged when declined, a new readable resource
    when compressed, or NULL on failure. Once compression is selected, the adapter
    closes resPtr even on failure. Close only the returned non-NULL resource.
 */
typedef ResIntfPtr (*IoIntf_DeflateGzip)(
   ResIntfPtr resPtr, const char* name, ThreadMutex* m,
   BaFileSize* size, BaBool* isCompressed);


#ifdef __cplusplus
}
#endif 

/* Required API methods
 */

/** Implementation-specific property operation.
    @param o Required initialized filesystem.
    @param name Required NUL-terminated property name, borrowed for the call.
    @param a Property-specific pointer/value; see the contracts below.
    @param b Property-specific pointer/value, or NULL where unused/optional.
    @return Zero when handled successfully, nonzero if unsupported or failed.
    Do not assume output arguments are initialized on failure.

    Common properties:
    - "type": a is required const char** output; b is optional const char**
      platform output. Returned strings are borrowed.
    - "movedir": a is required U32* output, TRUE/FALSE for moving directories.
    - "hidden": a is a borrowed resource-name string; b is required U32* input,
      TRUE to set the hidden flag or FALSE to clear it.
    - "abs": a is a borrowed path string; b is char** output for a baFree-owned
      absolute path. Prefer IoIntf_getAbspath().
    - "destructor": a and b are NULL. Prefer IoIntf_destructor().
    - "SeekAndRead": a is IoIntf_SeekAndRead* output; b is unused.
    - "attach": a is a borrowed attached IoIntf; b points to an
      IoIntf_OnTerminate callback to copy. a=NULL detaches. Implementations may
      notify the previous attachment when replacing it.
    ZIP password/encryption properties have typed wrappers: IoIntf_setPassword(),
    IoIntf_setPasswordProp(), and IoIntf_isEncrypted(). Availability depends on
    the implementation; do not call optional operations through NULL pointers.
 */
typedef int (*IoIntf_Property)(IoIntfPtr o,const char* name,void* a,void* b);

/** Open a directory iterator before its first entry.
    @param o Required initialized filesystem.
    @param dirname Required borrowed NUL-terminated directory path.
    @param status Required output receiving zero on success or an I/O error.
    @param ecode Optional output for a borrowed implementation error string;
    NULL omits it. Copy if needed beyond the operation and do not assume it is set.
    @return Owned iterator on success, NULL on failure. Advance with readFp
    before accessing an entry; close it through this filesystem's closeDirFp.
    Keep the filesystem alive until the iterator is closed.
 */
typedef DirIntfPtr (*IoIntf_OpenDir)(IoIntfPtr o, const char* dirname,
                                   int* status, const char** ecode);

/** Fetch metadata for a file or directory.
    @param o Required initialized filesystem.
    @param name Required borrowed NUL-terminated path relative to this filesystem.
    @param st Required writable output record, valid only on success.
    @return Zero on success, nonzero I/O status on failure.
 */
typedef int (*IoIntf_Stat)(IoIntfPtr o, const char* name, IoStat* st);

/** Open a file for reading or writing.
    The following compares POSIX mode with IoIntf mode flags.
    Note: Only the two first modes are guaranteed to work on all DiskIos
    Note2: The ZipIo only supports OpenRes_READ.
   +-------------+-------------------------------------------------+
   | POSIX mode  | IoIntf flags                                    |
   +-------------+-------------------------------------------------+
   |     r       | OpenRes_READ                                    |
   +-------------+-------------------------------------------------+
   |     w       | OpenRes_WRITE                                   |
   +-------------+-------------------------------------------------+
   |     a       | OpenRes_WRITE | OpenRes_APPEND                  |
   +-------------+-------------------------------------------------+
   |     w+      | OpenRes_READ | OpenRes_WRITE                    |
   +-------------+-------------------------------------------------+
   |     a+      | OpenRes_READ | OpenRes_WRITE | OpenRes_APPEND   |
   +-------------+-------------------------------------------------+
     @param o Required initialized filesystem.
    @param name Required borrowed NUL-terminated path relative to this filesystem.
    @param mode OpenRes_READ, OpenRes_WRITE, or a supported combination with
    OpenRes_APPEND. Query the implementation before relying on combined modes.
    @param status Required output receiving zero on success or an I/O error.
    @param ecode Optional output for a borrowed implementation error string;
    NULL omits it. Copy if needed beyond the operation and do not assume it is set.
    @return Owned resource handle on success, NULL on failure. Close through
    its closeFp and keep the filesystem alive until all handles are closed.
    Check optional resource function pointers before calling them.
 */
typedef ResIntfPtr (*IoIntf_OpenRes)(IoIntfPtr o, const char* name,
                                     U32 mode, int* status,
                                     const char** ecode);

/** Consume and close a directory iterator.
    @param o Required filesystem that opened the iterator.
    @param dirIntf Required pointer to a live non-NULL iterator. On completion
    the implementation releases it and clears the caller's pointer.
    @return Zero on success or nonzero I/O status. Do not retry using a consumed
    handle; NULL-handle acceptance is not portable across implementations.
 */
typedef int (*IoIntf_CloseDir)(IoIntfPtr o, DirIntfPtr* dirIntf);

/** Open a gzip representation for reading when supported.
    @param o Required initialized filesystem.
    @param name Required borrowed NUL-terminated path relative to this filesystem.
    @param m Optional mutex already held by the caller. A compressing adapter
    may release it during work and reacquire it before returning.
    @param size Required input/output byte count: supply the original file size;
    on success receives the gzip representation size including framing.
    @param status Required I/O status output. IOINTF_NOTCOMPRESSED indicates that
    the implementation declines compression; opening can also fail for other reasons.
    @param ecode Optional output for a borrowed implementation error string;
    NULL omits it. Copy if needed beyond the operation and do not assume it is set.
    @return Owned readable resource, or NULL on failure/declined compression.
    Always test the pointer; close a returned resource through closeFp.
 */
typedef ResIntfPtr (*IoIntf_OpenResGzip)(IoIntfPtr o, const char* name,
                                         ThreadMutex* m, BaFileSize* size,
                                         int* status, const char** ecode);

/** Create a directory.
    @param o Required initialized filesystem.
    @param name Required borrowed NUL-terminated path relative to this filesystem.
    @param ecode Optional output for a borrowed implementation error string;
    NULL omits it. Copy if needed beyond the operation and do not assume it is set.
    @return Zero on success, nonzero I/O status on failure. Missing operations
    are represented by NULL function pointers on read-only filesystems.
 */
typedef int (*IoIntf_MkDir)(IoIntfPtr o, const char* name, const char** ecode);

/** Rename or move a resource within this filesystem.
    @param o Required initialized writable filesystem.
    @param from Required borrowed NUL-terminated existing path.
    @param to Required borrowed NUL-terminated destination path.
    @param ecode Optional output for a borrowed implementation error string;
    NULL omits it. Copy if needed beyond the operation and do not assume it is set.
    @return Zero on success, nonzero I/O status on failure. Replacement and
    nonempty-directory move behavior are implementation-specific; check "movedir".
 */
typedef int (*IoIntf_Rename)(IoIntfPtr o, const char* from, const char* to,
                           const char** ecode);
/** Remove a file.
    @param o Required initialized filesystem.
    @param name Required borrowed NUL-terminated path relative to this filesystem.
    @param ecode Optional output for a borrowed implementation error string;
    NULL omits it. Copy if needed beyond the operation and do not assume it is set.
    @return Zero on success, nonzero I/O status on failure. Missing operations
    are represented by NULL function pointers on read-only filesystems.
 */
typedef int (*IoIntf_Remove)(IoIntfPtr o,const char* name, const char** ecode);

/** Remove an empty directory.
    @param o Required initialized filesystem.
    @param name Required borrowed NUL-terminated path relative to this filesystem.
    @param ecode Optional output for a borrowed implementation error string;
    NULL omits it. Copy if needed beyond the operation and do not assume it is set.
    @return Zero on success, nonzero I/O status on failure. Missing operations
    are represented by NULL function pointers on read-only filesystems.
 */
typedef int (*IoIntf_RmDir)(IoIntfPtr o, const char* name, const char** ecode);

/** Perform a combined seek and read on a resource.
    @param super Required live resource belonging to the implementation.
    @param offset Absolute byte position from the beginning of the resource.
    @param buf Required writable buffer for maxSize bytes.
    @param maxSize Positive maximum byte count.
    @param size Required output for the number of bytes read.
    @return ResIntf_Read-style status and count. This combined operation is needed
    by readers whose I/O releases the dispatcher mutex between operations; obtain
    it through the filesystem's "SeekAndRead" property.
 */
typedef int (*IoIntf_SeekAndRead)(
   ResIntfPtr super, BaFileSize offset,void* buf,size_t maxSize,size_t* size);

/* Called by I/O when terminating if property:attach is set */
/** Notify a filesystem attachment of termination/replacement.
    @param o Borrowed attached interface supplied by the "attach" property.
    @param io Filesystem delivering the notification. Its storage is still present
    during this callback but must not be retained for later use.
    No return value or ownership transfer is implied by the callback itself.
 */
typedef void (*IoIntf_OnTerminate)(IoIntfPtr o, IoIntfPtr io);

/** The IoIntf class specifies an abstract file API, implementations
    include ZipIo, DiskIo, and NetIo.

<h3>References:</h3>

\sa IoIntf.h (Detailed information about the IoIntf methods and the arguments).
\sa DirIntf (Iterate directories)
 */
typedef struct IoIntf
{
      /** Set or get property.
       */
      IoIntf_Property propertyFp;

      /** Close directory.
       */
      IoIntf_CloseDir closeDirFp;

      /** Make directory. Null for read only resource collections.
       */
      IoIntf_MkDir mkDirFp;

      /** Rename resource. Null for read only resource collections.
       */
      IoIntf_Rename renameFp;

      /** Open directory.
       */
      IoIntf_OpenDir openDirFp;

      /** Open resource.
       */
      IoIntf_OpenRes openResFp;

      /** Open file as a compressed gzip file. Null if not implemented.
       */
      IoIntf_OpenResGzip openResGzipFp;

      /** Remove file. Null for read only resource collections.
       */
      IoIntf_Remove removeFp;

      /** Remove directory. Null for read only resource collections.
       */
      IoIntf_RmDir rmDirFp;

      /** Get resource information.
       */
      IoIntf_Stat statFp;

      /* Called by I/O when terminating if property:attach is set */
      struct IoIntf* attachedIo;
      IoIntf_OnTerminate onTerminate;
} IoIntf;


/* Constructor for Read And Write resource collections.
 */
/** Initialize an I/O interface with caller-supplied callbacks.
    @param o Required caller-owned IoIntf storage.
    @param property IoIntf_Property callback for implementation-specific properties.
    @param closeDir IoIntf_CloseDir callback that consumes directory handles.
    @param mkDir IoIntf_MkDir callback, or NULL when unsupported.
    @param rename IoIntf_Rename callback, or NULL when unsupported.
    @param openDir IoIntf_OpenDir callback.
    @param openRes IoIntf_OpenRes callback.
    @param openResGzip IoIntf_OpenResGzip callback, or NULL when unsupported.
    @param rm IoIntf_Remove callback, or NULL when unsupported.
    @param rmDir IoIntf_RmDir callback, or NULL when unsupported.
    @param st IoIntf_Stat callback.
    No allocation or return value. Callbacks must remain callable throughout
    the interface lifetime. Attachment fields are cleared.
 */
#define IoIntf_constructorRW(o, property, closeDir, mkDir, rename,\
                             openDir, openRes, openResGzip, rm,\
                             rmDir, st) do {\
   (o)->propertyFp=property;\
   (o)->closeDirFp=closeDir;\
   (o)->mkDirFp=mkDir;\
   (o)->renameFp=rename;\
   (o)->openDirFp=openDir;\
   (o)->openResFp=openRes;\
   (o)->openResGzipFp=openResGzip;\
   (o)->removeFp=rm;\
   (o)->rmDirFp=rmDir;\
   (o)->statFp=st;\
   (o)->attachedIo=0;\
   (o)->onTerminate=0;\
} while(0)



/* Constructor for Read Only resource collections.
 */
/** Initialize an I/O interface with caller-supplied callbacks.
    @param o Required caller-owned IoIntf storage.
    @param property IoIntf_Property callback for implementation-specific properties.
    @param closeDir IoIntf_CloseDir callback that consumes directory handles.
    @param openDir IoIntf_OpenDir callback.
    @param openRes IoIntf_OpenRes callback.
    @param openResGzip IoIntf_OpenResGzip callback, or NULL when unsupported.
    @param st IoIntf_Stat callback.
    No allocation or return value. Callbacks must remain callable throughout
    the interface lifetime. Attachment fields are cleared. Write/directory modification callbacks are set to NULL.
 */
#define IoIntf_constructorR(o, property, closeDir, openDir,\
                            openRes, openResGzip, st) do {\
   (o)->propertyFp=property;\
   (o)->closeDirFp=closeDir;\
   (o)->mkDirFp=0;\
   (o)->renameFp=0;\
   (o)->openDirFp=openDir;\
   (o)->openResFp=openRes;\
   (o)->openResGzipFp=openResGzip;\
   (o)->removeFp=0;\
   (o)->rmDirFp=0;\
   (o)->statFp=st;\
   (o)->attachedIo=0;\
   (o)->onTerminate=0;\
} while(0)



/** Advance to the next directory entry, including the first after open.
    @param o Required live iterator.
    @return Zero when an entry is available, IOINTF_NOTFOUND at the end, or another
    nonzero I/O status on failure. Stop after a nonzero result. Ordering and whether
    special dot entries are exposed depend on the implementation.
 */
typedef int (*DirIntf_Read)(DirIntfPtr o);

/** Access the current entry name after successful readFp.
    @param o Required live iterator positioned on an entry.
    @return Borrowed NUL-terminated entry name, valid until advance or close.
    Copy it if needed later; behavior outside a valid entry is not portable.
 */
typedef const char* (*DirIntf_GetName)(DirIntfPtr o);

/** Fetch metadata for the current entry after successful readFp.
    @param o Required live iterator positioned on an entry.
    @param st Required output record, valid only on success.
    @return Zero on success, nonzero I/O status on failure.
 */
typedef int (*DirIntf_Stat)(DirIntfPtr o, IoStat* st);

/** Directory handle for a directory opened with IoIntf_OpenDir.
Example:
\code
int status;
DirIntfPtr dir = io->openDirFp(io, relPath, &status, 0);
if(dir)
{
   while((status = dir->readFp(dir)) == 0)
   {
      IoStat st;
      const char* name = dir->getNameFp(dir);
      int statStatus = dir->statFp(dir, &st);
      if(statStatus == 0)
      {
         // Process name/st here before advancing the iterator.
      }
   }
   // IOINTF_NOTFOUND is normal end-of-directory; other values are errors.
   int closeStatus = io->closeDirFp(io, &dir);
   // Handle closeStatus; dir has been consumed even if closing failed.
}
\endcode
\sa IoIntf
 */
typedef struct DirIntf
{
      /** Iterate the resources in the file system.
       */
      DirIntf_Read readFp;
      /** Get resource name */
      DirIntf_GetName getNameFp;
      /** Get resource information */
      DirIntf_Stat statFp;
}DirIntf;


/** Install directory handle callbacks; no allocation or return value.
    @param o Required initialized implementation storage.
    @param read Read callback; directory iteration or resource bytes respectively.
    @param getName Current-entry name callback.
    @param st Current-entry metadata callback.
    Callback storage and code must outlive the handle.
 */
#define DirIntf_constructor(o, read, getName, st) do {\
  (o)->readFp=read;\
  (o)->getNameFp=getName;\
  (o)->statFp=st;\
}while(0)

/** Read binary bytes at the current resource position.
    @param o Required live resource supporting reads.
    @param buf Required writable buffer for maxSize bytes.
    @param maxSize Positive capacity in bytes; zero-size behavior is not portable.
    @param size Required output receiving the byte count, no greater than maxSize.
    @return Zero for a successful read, IOINTF_EOF for end-of-file, or another
    nonzero I/O status. A successful read can be short or have zero bytes depending
    on the implementation. Inspect both status and count; no NUL is appended.
    A failure may leave partial bytes/count, whose availability is implementation-specific.
 */
typedef int (*ResIntf_Read)(
   ResIntfPtr o, void* buf, size_t maxSize, size_t* size);
/** Write binary bytes at the current resource position.
    @param o Required live resource supporting writes.
    @param buf Readable buffer, required when size is positive.
    @param size Number of bytes to write.
    @return Zero for a complete write, nonzero I/O status on failure. No partial
    count is returned; failure may already have changed the file.
 */
typedef int (*ResIntf_Write)(ResIntfPtr o, const void* buf, size_t size); 
/** Set the resource position relative to its beginning.
    @param o Required live resource supporting seek.
    @param offset Nonnegative absolute byte offset. Supported range and seeking
    beyond the current end depend on the implementation.
    @return Zero on success, nonzero I/O status on failure.
 */
typedef int (*ResIntf_Seek)(ResIntfPtr o, BaFileSize offset);
/** Flush buffered writes through the implementation.
    @param o Required live resource supporting flush.
    @return Zero on success, nonzero I/O status on failure. Success is not a
    portable guarantee of physical-media durability.
 */
typedef int (*ResIntf_Flush)(ResIntfPtr o);
/** Close and consume a resource handle.
    @param o Required live resource. Its storage is released by this call.
    @return Zero on success, nonzero I/O status if finalization/close fails.
    The handle is no longer usable even on failure; do not retry close.
 */
typedef int (*ResIntf_Close)(ResIntfPtr o);


/** Resource handle for a file opened with IoIntf_OpenRes.
 */
typedef struct ResIntf
{
      /** Read from resource */
      ResIntf_Read readFp;
      /** Write to resource */
      ResIntf_Write writeFp;
      /** Seek to position in resource */
      ResIntf_Seek seekFp;
      /** Flush resource */
      ResIntf_Flush flushFp;
      /** Close the resource and release any data */
      ResIntf_Close closeFp;
}ResIntf;

/** Install resource handle callbacks; no allocation or return value.
    @param o Required initialized implementation storage.
    @param read Read callback; directory iteration or resource bytes respectively.
    @param write Write callback, or NULL for a read-only resource.
    @param seek Seek callback, or NULL when unsupported.
    @param flush Flush callback, or NULL when unsupported.
    @param close Required close callback that releases the handle.
    Callback storage and code must outlive the handle.
 */
#define ResIntf_constructor(o, read, write, seek, flush, close) do {\
  (o)->readFp=read;\
  (o)->writeFp=write;\
  (o)->seekFp=seek;\
  (o)->flushFp=flush;\
  (o)->closeFp=close;\
}while(0)

/** @} */ /* end of IO */ 

#endif
