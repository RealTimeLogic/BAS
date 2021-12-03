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
 *   $Id: BaDiskIo.h 4915 2021-12-01 18:26:55Z wini $
 *
 *   COPYRIGHT:  Real Time Logic, 2006
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
 *  DiskIo implements the abstract class IoIntf. See the reference
 *  manual for more information on the IoIntf (IO interface)
 *  requirements.
 *
 *  This is a generic header file for all file systems and
 *  platforms. See the sub-directories for platform specific
 *  implementations.
 */

#ifndef _DiskIo_h
#define _DiskIo_h
 
#include "HttpServer.h"
#include "IoIntf.h"

#ifndef ROOTPT
#define ROOTPT void*
#endif

#ifndef DiskIo_DATA
#define DiskIo_DATA void* data
#endif



/** @addtogroup IO
    @{
 */

/** The DiskIo class makes it possible for the web-server to work with
    resources on a hard drive.

    A directory separator is always '/'. DOS based file systems that
    cannot handle forward slash must internally convert to and from
    '/'.

    A DiskIo instance can directly work on the root of the file
    system, but it is more common to give the DiskIo instance an
    offset value. The offset value is set with method
    DiskIo::setRootDir

    The setRootDir method can accept an absolute or relative path
    argument. As an example, all example programs compiled in 'debug'
    mode executes from the 'obj/debug' directory. All 'debug mode'
    examples opens a DiskIo and sets the root path to '../../html' and
    binds the DiskIo to a HttpResRdr instance. This makes it
    possible for the web-server to access the HTML files in the 'html'
    directory.

    This is a generic header file for all file systems and
    platforms.

    Barracuda does not officially support native file systems as
    Barracuda can easily work without using a file system. However,
    the xrc/DiskIo directory contains implementation for common
    file systems. This directory also contains the BaFile API used by
    some of the example code.
 */
typedef struct DiskIo
#ifdef __cplusplus
: public IoIntf
{
      void *operator new(size_t s) { return ::baMalloc(s); }
      void operator delete(void* d) { if(d) ::baFree(d); }
      void *operator new(size_t, void *place) { return place; }
      void operator delete(void*, void *) { }
 
      /** Create a DiskIo instance and set the root directory to '/'.
          The meaning of '/' depends on the implementation. As an
          example, the Windows version of DiskIo interprets '/' as
          the root of everything and the C: drive will therefore be
          /c/. Please see the documentation in the example
          implementations for more information.
       */
      DiskIo();

      /** Terminate the DiskIo instance
       */
      ~DiskIo();

      /** Set the root directory.

         \param root is a UTF8 formated path using forward slash. The
         meaning of 'root' depends on the implementation. Please see
         the comments in the DiskIo implementation for your target
         platform for more information.

         \returns 0 on success. A non zero value indicates failure.
       */
      int setRootDir(const char* root);

      int getRootDir(char* buf, int len);
#if 0
}
#endif
#else
{
      IoIntf super; /* Inherits from IoIntf */
#endif
      ROOTPT rootPath;
      int rootPathLen;
      DiskIo_DATA; /* Used internally if needed by the implementation */
} DiskIo;


#ifdef __cplusplus
extern "C" {
#endif
BA_API void DiskIo_constructor(DiskIo* o);
BA_API void DiskIo_destructor(DiskIo* o);
BA_API int DiskIo_setRootDir(DiskIo* o, const char* root);
BA_API int DiskIo_getRootDir(DiskIo* o, char* buf, int len);
#ifdef __cplusplus
}

inline DiskIo::DiskIo() {
   DiskIo_constructor(this);
}
inline DiskIo::~DiskIo() {
   DiskIo_destructor(this);
}
inline int DiskIo::setRootDir(const char* root) {
   return DiskIo_setRootDir(this, root);
}
inline int DiskIo::getRootDir(char* buf, int len) {
   return DiskIo_getRootDir(this, buf, len);
}

#endif
 
/** @} */ /* end of IO */ 

#endif

