/*
 *     ____             _________                __                _     
 *    / __ \___  ____ _/ /_  __(_)___ ___  ___  / /   ____  ____ _(_)____
 *   / /_/ / _ \/ __ `/ / / / / / __ `__ \/ _ \/ /   / __ \/ __ `/ / ___/
 *  / _, _/  __/ /_/ / / / / / / / / / / /  __/ /___/ /_/ / /_/ / / /__  
 * /_/ |_|\___/\__,_/_/ /_/ /_/_/ /_/ /_/\___/_____/\____/\__, /_/\___/  
 *                                                       /____/          
 *
 *                  Barracuda Embedded Web-Server 
 ****************************************************************************
 *            HEADER
 *
 *   $Id: FixedSizeAllocator.h 5978 2026-09-11 16:13:48Z wini $
 *
 *   COPYRIGHT:  Real Time Logic LLC, 2005-2008
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
 */

/** @file FixedSizeAllocator.h */

#ifndef __FixedSizeAllocator_h
#define __FixedSizeAllocator_h

#include <BaServerLib.h>
#include <SingleList.h>


/** This is a fixed size allocator implementation for the abstract
    interface class AllocatorIntf.

    @ingroup DynamicMemory

    The FixedSizeAllocator takes a buffer and splits the buffer up into
    equally sized chunks. Allocating memory larger than the chunk size
    or using realloc results in an error; i.e., NULL returned. One can
    allocate a smaller size than the chunk size, but the size will be
    adjusted to the chunk size. A zero-size request or exhausted pool returns
    NULL and sets the requested size to zero. Free only live blocks returned by
    this allocator, once each; NULL is not accepted. The backing buffer remains
    owned by the caller and must outlive the allocator and all allocated blocks.
    This allocator does not lock access; callers must serialize concurrent use.
*/
typedef struct FixedSizeAllocator
#ifdef __cplusplus
: public AllocatorIntf
{
      /** Leave storage uninitialized; call FixedSizeAllocator_constructor before use. */
      FixedSizeAllocator() {}

      /** Initialize a fixed-size allocation pool without allocating memory.
       * @param[in,out] buffer Writable backing storage, aligned for SingleLink
       * and for the objects to be allocated from it. Initialization writes free
       * list links into the buffer.
       * @param[in] bufSize Available bytes. Only complete blocks are used;
       * trailing bytes smaller than blockSize are unused.
       * @param[in] blockSize Positive bytes per block, at least sizeof(SingleLink)
       * and a multiple of the required alignment. The buffer-size arithmetic
       * must fit in size_t. No argument-validation status is returned.
       */
      FixedSizeAllocator(void* buffer, size_t bufSize, size_t blockSize);
      
#else
{
   AllocatorIntf super;
#endif
   SingleList freeList;
   size_t blockSize;
} FixedSizeAllocator;


#ifdef __cplusplus
extern "C" {
#endif 
/** Initialize a fixed-size allocator in caller-provided storage.
 * @param[out] o Allocator object to initialize.
 * @param[in,out] buffer Writable, suitably aligned backing buffer retained by o.
 * @param[in] bufSize Byte size of buffer; incomplete trailing blocks are unused.
 * @param[in] blockSize Positive block size. See
 * FixedSizeAllocator::FixedSizeAllocator(void*,size_t,size_t) for alignment,
 * minimum-size and lifetime requirements. */
BA_API void FixedSizeAllocator_constructor(FixedSizeAllocator* o,
                                    void* buffer,
                                    size_t bufSize,
                                    size_t blockSize);
#ifdef __cplusplus
}
inline FixedSizeAllocator::FixedSizeAllocator(void* buffer,
                                              size_t bufSize,
                                              size_t blockSize) {
   FixedSizeAllocator_constructor(this, buffer, bufSize, blockSize);
}
#endif 


#endif 
