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
 *   $Id: BaFile.h 5978 2026-09-11 16:13:48Z wini $
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
/** @file BaFile.h */

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

/** Open a directory through the platform's default DiskIo.
 * @param[in] dirname NUL-terminated directory path. Path syntax and the
 * starting directory depend on the selected platform implementation.
 * @param[out] status Required status pointer: zero on success, otherwise an
 * IOINTF error code.
 * @param[out] ecode Optional pointer for a borrowed platform error description;
 * may be NULL. Inspect it only on failure; do not free the returned string.
 * @return Owned directory iterator, or NULL on failure. Close it with
 * baCloseDir. Entries may include '.' and '..' on some platforms.
 */
BA_API DirIntfPtr baOpenDir(const char* dirname, int* status, const char** ecode);

/** Close a directory iterator.
 * @param[in,out] dirIntf Required pointer to a live iterator returned by
 * baOpenDir. The iterator is released and *dirIntf is set to NULL.
 * @return Zero on success, otherwise an IOINTF error code. */
BA_API int baCloseDir(DirIntfPtr* dirIntf);

/** Read file or directory metadata.
 * @param[in] name NUL-terminated path using the platform's default DiskIo.
 * @param[out] st Required metadata buffer, valid only on success.
 * @return Zero on success, otherwise an IOINTF error code. */
BA_API int baStat(const char* name, IoStat* st);

/** Open a binary file for reading or writing.
 * @param[in] name NUL-terminated path using the platform's default DiskIo.
 * @param[in] mode OpenRes_READ, OpenRes_WRITE or a supported combination with
 * OpenRes_APPEND. See IoIntf_OpenRes for the mode table and portability limits.
 * Write mode without append can truncate an existing file.
 * @param[out] status Required status pointer: zero on success, otherwise an
 * IOINTF error code.
 * @param[out] ecode Optional pointer for a borrowed platform error description;
 * may be NULL. Inspect only on failure and do not free it.
 * @return Owned resource handle, or NULL on failure. Close it with baCloseRes.
 */
BA_API ResIntfPtr baOpenRes(const char* name,U32 mode,int* status,const char** ecode);

/** Close a resource handle, including when the underlying close reports failure.
 * @param[in,out] fp Required pointer to a live handle returned by baOpenRes.
 * The handle is consumed and *fp is set to NULL; do not retry with that handle.
 * @return Zero on success, otherwise the resource's close error code. */
BA_API int baCloseRes(ResIntfPtr* fp);

/** Create a directory.
 * @param[in] dname NUL-terminated directory path.
 * @param[out] ecode Optional borrowed platform error description on failure;
 * may be NULL. Do not free it.
 * @return Zero on success, otherwise an IOINTF error code. */
BA_API int baMkDir(const char* dname, const char** ecode);

/** Remove an empty directory.
 * @param[in] dname NUL-terminated directory path.
 * @param[out] ecode Optional borrowed platform error description on failure;
 * may be NULL. Do not free it.
 * @return Zero on success, otherwise an IOINTF error code. */
BA_API int baRmdDir(const char* dname, const char** ecode);

/** Remove a file.
 * @param[in] fname NUL-terminated file path.
 * @param[out] ecode Optional borrowed platform error description on failure;
 * may be NULL. Do not free it.
 * @return Zero on success, otherwise an IOINTF error code. */
BA_API int baRemove(const char* fname, const char** ecode);

/** Rename a file or directory.
 * @param[in] from NUL-terminated existing path.
 * @param[in] to NUL-terminated destination path. Replacement and cross-device
 * behavior depend on the platform's DiskIo implementation.
 * @param[out] ecode Optional borrowed platform error description on failure;
 * may be NULL. Do not free it.
 * @return Zero on success, otherwise an IOINTF error code. */
BA_API int baRename(const char* from, const char* to, const char** ecode);

#ifdef __cplusplus
}
#endif

/** @} */ /* end of BaFile */ 

#endif
