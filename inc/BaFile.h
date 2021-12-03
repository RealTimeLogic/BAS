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
 *   $Id: BaFile.h 4915 2021-12-01 18:26:55Z wini $
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
 *
 */
#ifndef __BaFile_h
#define __BaFile_h

/** @defgroup BaFile Standard I/O functions
    @ingroup IO

    The BaFile API specifies a number of standard I/O functions for
    working with files. The BaFile API is used by some of the examples
    in the example directory. The BaFile API is not used by any of the
    web-server code.

    The BaFile API provides functions similar to standard ANSI I/O functions.

    @{
 */

#include <IoIntf.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Opens a directory for reading. Sub-directories may include '.' and '..'
 */
BA_API DirIntfPtr baOpenDir(const char* dirname, int* status, const char** ecode);

/** Close a BaDir object.
*/
BA_API int baCloseDir(DirIntfPtr* dirIntf);

/** Get information for a directory or file.
*/
BA_API int baStat(const char* name, IoStat* st);


/** Open a file for binary read.
*/
BA_API ResIntfPtr baOpenRes(const char* name,U32 mode,int* status,const char** ecode);

/** Close a file descriptor.
*/
BA_API int baCloseRes(ResIntfPtr* fp);


/** Create directory.
 */ 
BA_API int baMkDir(const char* dname, const char** ecode);

/** Remove empty directory.
 */
BA_API int baRmdDir(const char* dname, const char** ecode);


/** Remove file.
 */
BA_API int baRemove(const char* fname, const char** ecode);


/* Rename file or directory.
 */
BA_API int baRename(const char* from, const char* to, const char** ecode);

#ifdef __cplusplus
}
#endif

/** @} */ /* end of BaFile */ 

#endif
