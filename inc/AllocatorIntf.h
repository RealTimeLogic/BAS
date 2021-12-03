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
 *   $Id: AllocatorIntf.h 4914 2021-12-01 18:24:30Z wini $
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

#ifndef __AllocatorIntf_h
#define __AllocatorIntf_h
 
#include <TargConfig.h>
#include <stddef.h>

#ifndef BA_API
#define BA_API
#endif


struct AllocatorIntf;

/** Malloc prototype.
    \param size the size required.  The allocator can change the
    size to a size larger than the one requested.
    \sa AllocatorIntf::AllocatorIntf
*/
typedef void* (*AllocatorIntf_Malloc)(struct AllocatorIntf* o, size_t* size);

/** Realloc prototype.
    \param size the size required.  The allocator can change the
    size to a size larger than the one requested.
    \sa AllocatorIntf::AllocatorIntf
*/
typedef void* (*AllocatorIntf_Realloc)(
   struct AllocatorIntf* o, void* memblock, size_t* size);

/** Free prototype.
    \sa AllocatorIntf::AllocatorIntf
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
      AllocatorIntf() {}
      /** Create an instance of a memory allocation class.
          This is an abstract base class and should, therefore, be sub-classed.
          \param malloc Pointer to memory allocation method.
          \param realloc Pointer to memory reallocation method. This
          method is optional and the argument can be set to NULL if not
          implemented.
          \param free Pointer to a memory deallocation method.
      */
      AllocatorIntf(AllocatorIntf_Malloc malloc,
                    AllocatorIntf_Realloc realloc,
                    AllocatorIntf_Free free);

      /** Returns a pointer to a predefined AllocatorIntf class. The
          default implementation uses method baMalloc(), baRealloc() and
          baFree().
      */
      static AllocatorIntf* getDefault(void);

      /** Returns pointer to uninitialized newly-allocated space for
         an object of size "size", or NULL on error.
         \param size the size required.  The allocator can change the
          size to a size larger than the one requested.
       */
      void* malloc(size_t* size);
      void* malloc(size_t size) { return malloc(&size); }

      /** Returns pointer to newly-allocated space for an object of
          size "size", initialized, to minimum of old and new sizes, to
          existing contents of p (if non-null), or NULL on error. On
          success, old object deallocated; otherwise unchanged.
      */
      void* realloc(void* p, size_t* size);
      void* realloc(void* p, size_t size) { return realloc(p, &size); }

      /** Deallocates space to which it points.
       */
      void free(void* p);
#endif
      AllocatorIntf_Malloc mallocCB;
      AllocatorIntf_Realloc reallocCB; /* optional */
      AllocatorIntf_Free freeCB;
} AllocatorIntf;

#define AllocatorIntf_constructor(o, m, r, f) do { \
   (o)->mallocCB=m; \
   (o)->reallocCB=r; \
   (o)->freeCB=f; \
} while(0)

#define AllocatorIntf_malloc(o, size) (o)->mallocCB(o, size)
#define AllocatorIntf_realloc(o, memblock, size) \
   ((o)->reallocCB ? (o)->reallocCB(o,memblock,size) : 0)
#define AllocatorIntf_free(o, memblock) (o)->freeCB(o,memblock)

#ifdef __cplusplus
extern "C" {
#endif
BA_API AllocatorIntf* AllocatorIntf_getDefault(void);

/** Calls method malloc in the allocator to allocate storage space for
    a copy of str and then copies src to the allocated space.
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
