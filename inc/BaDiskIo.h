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
 *   $Id: BaDiskIo.h 5978 2026-09-11 16:13:48Z wini $
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

/** @file BaDiskIo.h */

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

/** The DiskIo class makes it possible for the web server to work with
    resources on a hard drive.

    A directory separator is always '/'. DOS based file systems that
    cannot handle forward slash must internally convert to and from
    '/'.

    A DiskIo instance can directly work on the root of the file
    system, but it is more common to give the DiskIo instance an
    offset value. The offset value is set with method
    DiskIo::setRootDir

    Root syntax and default roots are platform-specific. Desktop ports accept
    absolute paths and selected relative paths; embedded ports may require a
    mounted volume or a platform-specific prefix. See xrc/DiskIo for the port
    selected by the build. Call setRootDir explicitly before serving files.

    This generic interface does not mount media, enforce filesystem permissions,
    or replace the platform's path and symbolic-link rules. Stop dependent
    readers and serialize operations before changing its root or destroying it.
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

      /** Release the root configuration and notify an attached I/O interface.
          Close resources and directory iterators, and stop dependent users,
          before destruction. Does not unmount media or free this object.
       */
      ~DiskIo();

      /** Select the filesystem location exposed as this I/O interface's root.
       * @param[in] root NUL-terminated UTF-8 path using forward slashes.
       * The path is copied. Accepted prefixes, relative paths, NULL, and empty
       * strings vary by port; use an explicit valid path for portable code.
       * @return Zero on success, nonzero platform/I/O status on failure.
       * Success does not universally verify that the directory exists.
       * A failed change need not preserve the previous root. Stop using the
       * instance until a valid root has been established again.
       */
      int setRootDir(const char* root);

      /** Copy the configured root representation.
       * @param[out] buf Caller-owned writable buffer. On success it contains
       * a NUL-terminated UTF-8 path, which may include a trailing slash.
       * @param[in] len Positive capacity in bytes, including the terminator.
       * Allocate enough for the entire path plus NUL; do not use this function
       * as a size query. Some ports do not check the terminator byte correctly.
       * @return Nonnegative on success, -1 on insufficient storage or conversion
       * failure. Most ports return strlen(buf); HCC_UNICODE returns zero for a
       * converted configured root. Use strlen(buf) when the length is needed.
       * Output is unspecified on failure. This reports configuration, not
       * filesystem existence or a universally canonical absolute path.
       */
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
/** Initialize the platform DiskIo implementation.
 * @param[out] o Caller-owned instance; explicitly set its root before use.
 * @sa DiskIo::DiskIo */
BA_API void DiskIo_constructor(DiskIo* o);
/** Release configuration after dependent users and open handles have stopped.
 * @param[in,out] o Initialized instance. Does not free o or unmount media.
 * @sa DiskIo::~DiskIo */
BA_API void DiskIo_destructor(DiskIo* o);
/** @copydoc DiskIo::setRootDir
 * @param[in,out] o Initialized instance, not concurrently in use. */
BA_API int DiskIo_setRootDir(DiskIo* o, const char* root);
/** @copydoc DiskIo::getRootDir
 * @param[in] o Initialized instance with a valid root configuration. */
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
