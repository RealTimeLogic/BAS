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
 *   $Id: ZipIo.h 5355 2022-11-16 10:33:45Z wini $
 *
 *   COPYRIGHT:  Real Time Logic LLC, 2006-2019
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
 *               http://www.realtimelogic.com
 ****************************************************************************
 *
 *
 */

#ifndef __ZipIo_h
#define __ZipIo_h 

#include "IoIntf.h"
#include "VirDir.h"
#include "ZipFileIterator.h"

/** @addtogroup IO
    @{
 */


/** The ZipIo class makes it possible for the web-server to work with
resources in a ZIP file as if the ZIP file is a read-only file system.

The most common archive format, ZIP-File, is an "archive" that can
contain one or more files. Usually the files "archived" in a ZIP-File
are compressed to save space.
                                   
A ZIP-File contains a central repository, which stores the original
directory structure that was archived. You can think of the central
repository as a "read only" file system.  The ZipIo class can decode
and interpret the central repository in a ZIP-File.

The ZipIo class can automatically convert the internal ZIP data to a
GZIP file without using an uncompressing library. Many browsers
support the GZIP format and a file inside a ZIP-File can easily be
transformed into a GZIP file.

Whenever a browser requests a file that is in a ZIP-File, the ZipIo
class extracts the file, appends a GZIP header to the compressed data,
and sends the file as a GZIP file to the browser.

A compression library is only needed if you have to support older
browsers or if you plan to use method HttpResponse::include with
Zip-Files. See <a href="../../csp.html#HiddenFiles"> Server
Side Include files </a> in the user manual for more information.

It is possible to create a non compressed ZIP file, which can
be used without the aforementioned restrictions. A non compressed ZIP
file is created by running: zip -0 args. See ZIP man page for more
info. On windows, open a bash shell (Cygwin) and type one of:<br>
info zip<br>
zip -h

Directory and file names in the ZIP file must be stored as ASCII or UTF8.

A ZIP-File can be made plug-able; that is, you can add and remove a
Zip-File from the virtual file system. The ZIP-File uses a ZipReader
class, which inherits from CspReader, as the interface to the
Zip-File.  The example directory contains the FileZipReader which is
an example implementation of a ZipReader.
 */
typedef struct ZipIo
#ifdef __cplusplus
: public IoIntf
{
      ZipIo() {}
      void *operator new(size_t s) { return ::baMalloc(s); }
      void operator delete(void* d) { if(d) ::baFree(d); }
      void *operator new(size_t, void *place) { return place; }
      void operator delete(void*, void *) { }

/** ZipIo constructor.

<b>Example</b>
\code
extern "C" ZipReader* getHtmlZipReader(void);
.
.
ZipIo io(getHtmlZipReader());
\endcode
Function getHtmlZipReader in the above example is auto
generated by using
<a href="../../CspTools.html#bin2c">bin2c</a>
and the -z flag.

\param reader a ZipReader instance such as a FileZipReader.
\param size is the length of the longest directory and path name to
resources in the ZIP file. Minimum value is 256. The ZipIo is
internally passing this information to a ZipContainer.
\param alloc The ZipIo scans the ZIP file and builds a small virtual
directory with information on the offset position to the internal data
in the ZIP file. The allocator is used when building these data
structures. The data structures are released when the ZipIo destructor
is run.
*/
      ZipIo(ZipReader* reader,size_t size=256,AllocatorIntf* alloc=0);

/** ZipIo destructor.
 */
      ~ZipIo();

/** Returns an error code if the ZipIo constructor failed.
 */
      ZipErr getECode();
#if 0
}
#endif
#else
{
      IoIntf super; /* Inherits from IoIntf. */
#endif
      VirDirNode root;
      ZipContainer zc;
      AllocatorIntf* alloc;
      char* password;
      U16   passwordLen;
      ZipErr ecode;
      BaBool passwordRequired;
      BaBool passwordBin;
} ZipIo;

#ifdef __cplusplus
extern "C" {
#endif
BA_API void ZipIo_constructor(
   ZipIo* o,
   ZipReader* reader,
   size_t size,
   AllocatorIntf* alloc);
BA_API void ZipIo_destructor(ZipIo* o);
#define ZipIo_getECode(o) (o)->ecode
#ifdef __cplusplus
}
inline ZipIo::ZipIo(ZipReader* reader,size_t size,AllocatorIntf* alloc) {
   ZipIo_constructor(this,reader,size,alloc);
}
inline ZipIo::~ZipIo() {
   ZipIo_destructor(this);
}
inline ZipErr ZipIo::getECode() {
   return ZipIo_getECode(this);
}
#endif

/** @} */ /* end of IO */ 

#endif
