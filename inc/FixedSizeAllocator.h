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
 *   $Id: FixedSizeAllocator.h 4915 2021-12-01 18:26:55Z wini $
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
    adjusted to the chunk size.
*/
typedef struct FixedSizeAllocator
#ifdef __cplusplus
: public AllocatorIntf
{
      FixedSizeAllocator() {}

      /** Create a fixed size allocator.
          \param buffer a pointer to the memory location for the fixed
          size allocator.
          \param bufSize the size of the buffer. The size should be n*blockSize.
          \param blockSize the size of the chunks.
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
