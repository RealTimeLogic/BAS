/*
 *     ____             _________                __                _     
 *    / __ \___  ____ _/ /_  __(_)___ ___  ___  / /   ____  ____ _(_)____
 *   / /_/ / _ \/ __ `/ / / / / / __ `__ \/ _ \/ /   / __ \/ __ `/ / ___/
 *  / _, _/  __/ /_/ / / / / / / / / / / /  __/ /___/ /_/ / /_/ / / /__  
 * /_/ |_|\___/\__,_/_/ /_/ /_/_/ /_/ /_/\___/_____/\____/\__, /_/\___/  
 *                                                       /____/          
 *
 *                  Barracuda Application Server
 *
 ****************************************************************************
 *			      HEADER
 *
 *   $Id: AllocatorIntf.h 5978 2026-09-11 16:13:48Z wini $
 *
 *   COPYRIGHT:  Real Time Logic LLC, 2014
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
 *               http://realtimelogic.com
 ****************************************************************************
 *
 */

/** @file AllocatorIntf.h */

#ifndef __AllocatorIntf_h
#define __AllocatorIntf_h
 
#include <TargConfig.h>
#include <stddef.h>

#ifndef BA_API
#define BA_API
#endif


struct AllocatorIntf;

/** Allocate a block using an allocator implementation.
 * @param[in,out] o Allocator instance supplied to the callback.
 * @param[in,out] size Required pointer to the requested byte count. On success
 * the implementation may increase it to the actual allocation size. Its value
 * on failure, and the handling of a zero request, depend on the implementation.
 * @return Uninitialized storage of at least the requested size, or NULL on
 * failure. Release successful allocations with the same allocator's free
 * callback. The allocator object must outlive its allocated blocks.
 */
typedef void* (*AllocatorIntf_Malloc)(struct AllocatorIntf* o, size_t* size);

/** Resize a block using the same allocator that allocated it.
 * @param[in,out] o Allocator instance supplied to the callback.
 * @param[in] memblock Existing allocation, or NULL to request a new block.
 * @param[in,out] size Required requested byte count. On success it may be
 * increased to the actual size. Use a positive size for portable behavior;
 * zero-size behavior depends on the allocator implementation.
 * @return Replacement block preserving the smaller of the old and new byte
 * counts, or NULL on failure. For a positive request, failure leaves memblock
 * owned by the caller; success invalidates the old pointer. New bytes are not
 * initialized. This callback is optional in AllocatorIntf.
 */
typedef void* (*AllocatorIntf_Realloc)(
   struct AllocatorIntf* o, void* memblock, size_t* size);

/** Release a block.
 * @param[in,out] o Allocator instance supplied to the callback.
 * @param[in] memblock Live block obtained from this allocator. NULL handling
 * depends on the implementation; FixedSizeAllocator requires a non-NULL block.
 * The pointer must not be used after the callback returns.
 */
typedef void (*AllocatorIntf_Free)(struct AllocatorIntf* o, void* memblock);

/** Memory allocation and deallocation Interface class.

    This abstract interface class is used by some of the Barracuda
    classes when allocating memory. The reason for using an interface
    class and not directly calling the global functions baMalloc(),
    baRealloc() and baFree() is to provide a finer control of
    allocated memory. For example, an implementation of the
    AllocatorIntf can work with blocks of memory allocated
    from static memory. Implementing realloc is optional and can be
    set to NULL if not implemented.
 */
typedef struct AllocatorIntf
{
#ifdef __cplusplus
      /** Leave callbacks uninitialized; initialize them before use. */
      AllocatorIntf() {}
      /** Create an instance of a memory allocation class.
          This is an abstract base class and should, therefore, be sub-classed.
          \param malloc Required allocation callback, retained without copying.
          \param realloc Pointer to memory reallocation method. This
          method is optional and the argument can be set to NULL if not
          implemented.
          \param free Required deallocation callback, retained without copying.
      */
      AllocatorIntf(AllocatorIntf_Malloc malloc,
                    AllocatorIntf_Realloc realloc,
                    AllocatorIntf_Free free);

      /** Obtain the shared allocator backed by baMalloc, baRealloc and baFree.
       * @return Borrowed process-lifetime allocator; do not free it or change
       * its callbacks. */
      static AllocatorIntf* getDefault(void);

      /** Allocate uninitialized storage.
       * @param[in,out] size Requested byte count, possibly increased on success.
       * @return Allocated block or NULL; see AllocatorIntf_Malloc. */
      void* malloc(size_t* size);
      /** Allocate without returning the adjusted allocation size.
       * @param[in] size Requested byte count.
       * @return Allocated block or NULL; see AllocatorIntf_Malloc. */
      void* malloc(size_t size) { return malloc(&size); }

      /** Resize an allocation.
       * @param[in] p Block allocated by this allocator, or NULL.
       * @param[in,out] size Requested byte count, possibly increased on success.
       * @return Replacement block or NULL. If realloc is not implemented,
       * returns NULL without modifying p or size. See AllocatorIntf_Realloc. */
      void* realloc(void* p, size_t* size);
      /** Resize without returning the adjusted allocation size.
       * @param[in] p Block allocated by this allocator, or NULL.
       * @param[in] size Requested byte count.
       * @return Replacement block or NULL; see AllocatorIntf_Realloc. */
      void* realloc(void* p, size_t size) { return realloc(p, &size); }

      /** Release an allocation.
       * @param[in] p Live block from this allocator. NULL is allowed only if
       * the installed callback supports it; see AllocatorIntf_Free. */
      void free(void* p);
#endif
      AllocatorIntf_Malloc mallocCB;
      AllocatorIntf_Realloc reallocCB; /* optional */
      AllocatorIntf_Free freeCB;
} AllocatorIntf;

/** Install allocator callbacks without allocating any memory.
 * @param[out] o Caller-owned allocator object.
 * @param[in] m Required allocation callback.
 * @param[in] r Optional resize callback; NULL disables resizing.
 * @param[in] f Required deallocation callback. */
#define AllocatorIntf_constructor(o, m, r, f) do { \
   (o)->mallocCB=m; \
   (o)->reallocCB=r; \
   (o)->freeCB=f; \
} while(0)

/** Dispatch an allocation request.
 * @param[in,out] o Initialized allocator.
 * @param[in,out] size Requested byte count; see AllocatorIntf_Malloc.
 * @return New storage or NULL on failure. */
#define AllocatorIntf_malloc(o, size) (o)->mallocCB(o, size)
/** Dispatch a resize request.
 * @param[in,out] o Initialized allocator.
 * @param[in] memblock Existing block or NULL; see AllocatorIntf_Realloc.
 * @param[in,out] size Requested byte count; see AllocatorIntf_Realloc.
 * @return Replacement block or NULL. An absent callback returns NULL and
 * leaves the original block and size unchanged. */
#define AllocatorIntf_realloc(o, memblock, size) \
   ((o)->reallocCB ? (o)->reallocCB(o,memblock,size) : 0)
/** Dispatch a deallocation request.
 * @param[in,out] o Initialized allocator.
 * @param[in] memblock Block to release; see AllocatorIntf_Free. */
#define AllocatorIntf_free(o, memblock) (o)->freeCB(o,memblock)

#ifdef __cplusplus
extern "C" {
#endif
/** @return Borrowed process-lifetime allocator backed by baMalloc,
 * baRealloc and baFree. Do not free it or change its callbacks. */
BA_API AllocatorIntf* AllocatorIntf_getDefault(void);

/** Allocate a NUL-terminated copy of a string.
 * @param[in,out] a Required initialized allocator, used only when str is non-NULL.
 * @param[in] str NUL-terminated source string, or NULL. The source is not modified.
 * @return Independent copy, or NULL when str is NULL or allocation fails.
 * Release a successful result through a. The allocation includes the NUL byte.
 */
BA_API char* baStrdup2(struct AllocatorIntf* a, const char* str);

#ifdef __cplusplus
}
inline AllocatorIntf::AllocatorIntf(AllocatorIntf_Malloc malloc,
                                    AllocatorIntf_Realloc realloc,
                                    AllocatorIntf_Free free) {
   AllocatorIntf_constructor(this, malloc,realloc,free); }
inline AllocatorIntf* AllocatorIntf::getDefault(void) {
   return AllocatorIntf_getDefault(); }
inline void* AllocatorIntf::malloc(size_t* size) {
   return AllocatorIntf_malloc(this, size); }
inline void* AllocatorIntf::realloc(void* memblock, size_t* size) {
   return AllocatorIntf_realloc(this, memblock, size); }
inline void AllocatorIntf::free(void* memblock) {
   AllocatorIntf_free(this, memblock); }
#endif

#endif
