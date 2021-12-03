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
 *   $Id: IoIntf.h 4915 2021-12-01 18:26:55Z wini $
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
      /** Get lastModified time. */
      BaTime lastModified;
      /** Get size. */
      BaFileSize size;
      /** Resource type */
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
    accessing the resources.  \param passwordLen the length of the
    password (in bytes), can be zero when the password is ASCII
    format, and in this case, the length is calculated with strlen.
    \returns 0 on success or a non zero value if setting password not
    implemeted by the IoIntf implementation.
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
	 properties not implemeted by the IoIntf implementation.
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


/** wrapper for IoIntf_Property: 'type'.
    \param o a pointer to the IoIntf implementation.
    \param type is where the type is stored. Common types are 'disk' and 'zip'.
    \param platform is where the platform type is stored. Common types
    are 'windows' and 'POSIX'.
*/
BA_API int IoIntf_getType(IoIntfPtr o,const char** type,const char** platform);

/** wrapper for IoIntf_Property: 'aes'.
    \param o a pointer to the IoIntf implementation.
    \param name the file name.
    \param isEncrypted return value set to TRUE or FALSE.
    \returns zero if found. Non-zero if not found.
*/
BA_API int IoIntf_isEncrypted(
   IoIntfPtr o,const char* name,BaBool* isEncrypted);


/** Virtual destructor.
    Releases dynamically allocated resources.
*/
BA_API void IoIntf_destructor(IoIntfPtr o);


/** Open a file for writing.
    Inflates the gzip file as it is saved.
    Function in BaGzip.c.
    Function pointer in HttpUpload.c
    See BaGzip.c for more info.
*/
typedef ResIntfPtr (*IoIntf_InflateGzip)(
   IoIntfPtr io, const char* name, int* status, const char** ecode);


/** Deflate a file or portion of a file to temporary storage.
    A ResIntfPtr is returned to the compressed file or file fragment
    Function pointer in HttpResRdr.c
    See BaGzip.c for more info.
*/
typedef ResIntfPtr (*IoIntf_DeflateGzip)(
   ResIntfPtr resPtr, const char* name, ThreadMutex* m,
   BaFileSize* size, BaBool* isCompressed);


#ifdef __cplusplus
}
#endif 

/* Required API methods
 */

/** Set or get properties. The method returns a non zero value if
    the property is not supported.

   name: type
   a:    pointer to 'const char**'. The type is returned as a string in 'a'.
         Common values: "disk", "zip". Variable 'b' contains a pointer to
         the platform type such as 'windows', 'POSIX', etc.
   
   name: movedir
   a:    Return value: pointer to U32 set to TRUE or FALSE.
   b:    n/a
   
   name: hidden
   a:    pointer to 'const char*'. The resource name.
   b:    Pointer to U32 is set to TRUE for hidden and FALSE if hidden
         attribute is to be cleared.

  name: pwd
  a:    pointer to 'const char*', the password.

*/
typedef int (*IoIntf_Property)(IoIntfPtr o,const char* name,void* a,void* b);

/** Open a directory for reading.
    \returns DirIntfPtr directory iterator
 */
typedef DirIntfPtr (*IoIntf_OpenDir)(IoIntfPtr o, const char* dirname,
                                   int* status, const char** ecode);

/** Fetch resource information.
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
 */
typedef ResIntfPtr (*IoIntf_OpenRes)(IoIntfPtr o, const char* name,
                                     U32 mode, int* status,
                                     const char** ecode);

/** Close an open directory iterator.
 */
typedef int (*IoIntf_CloseDir)(IoIntfPtr o, DirIntfPtr* dirIntf);

/** Returns data as a compressed gzip file.
 */
typedef ResIntfPtr (*IoIntf_OpenResGzip)(IoIntfPtr o, const char* name,
                                         ThreadMutex* m, BaFileSize* size,
                                         int* status, const char** ecode);

/** Create a new directory.
 */
typedef int (*IoIntf_MkDir)(IoIntfPtr o, const char* name, const char** ecode);

/** Close an open directory iterator.
 */
typedef int (*IoIntf_Rename)(IoIntfPtr o, const char* from, const char* to,
                           const char** ecode);
/** Delete a file.
 */
typedef int (*IoIntf_Remove)(IoIntfPtr o,const char* name, const char** ecode);

/** delete an empty directory.
 */
typedef int (*IoIntf_RmDir)(IoIntfPtr o, const char* name, const char** ecode);

typedef int (*IoIntf_SeekAndRead)(
   ResIntfPtr super, BaFileSize offset,void* buf,size_t maxSize,size_t* size);

/* Called by I/O when terminating if property:attach is set */
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



/** Iterate to next resource in directory.
 */
typedef int (*DirIntf_Read)(DirIntfPtr o);

/** Returns the resource name. 
 */
typedef const char* (*DirIntf_GetName)(DirIntfPtr o);

/** Returns resource information.
 */
typedef int (*DirIntf_Stat)(DirIntfPtr o, IoStat* st);

/** Directory handle for a directory opened with IoIntf_OpenDir.
Example:
\code
DirIntfPtr dir = io->openDirFp(io, relPath, &status, 0);
if(dir)
{
   while ( ! dir->readFp(dir) )
   {
      IoStat st;
      const char* name = dir->getNameFp(dir);
      dir->statFp(dir, &st);
   }
}
io->closeDirFp(io, &dir);
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


#define DirIntf_constructor(o, read, getName, st) do {\
  (o)->readFp=read;\
  (o)->getNameFp=getName;\
  (o)->statFp=st;\
}while(0)

/** Abstract Resource Interface.
 */
typedef int (*ResIntf_Read)(
   ResIntfPtr o, void* buf, size_t maxSize, size_t* size);
typedef int (*ResIntf_Write)(ResIntfPtr o, const void* buf, size_t size); 
typedef int (*ResIntf_Seek)(ResIntfPtr o, BaFileSize offset);
typedef int (*ResIntf_Flush)(ResIntfPtr o);
typedef int (*ResIntf_Close)(ResIntfPtr o);


/** Resource handle for a directory opened with IoIntf_OpenRes.
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

#define ResIntf_constructor(o, read, write, seek, flush, close) do {\
  (o)->readFp=read;\
  (o)->writeFp=write;\
  (o)->seekFp=seek;\
  (o)->flushFp=flush;\
  (o)->closeFp=close;\
}while(0)

/** @} */ /* end of IO */ 

#endif
