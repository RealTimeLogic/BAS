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
 *   $Id: DynBuffer.h 5978 2026-09-11 16:13:48Z wini $
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

/** @file DynBuffer.h */

#ifndef _DynBuffer_h
#define _DynBuffer_h
 
#include <HttpServer.h>
#include <BaServerLib.h>

/*
  eCode:
    -1: No allocator
    -2: Malloc failed.
    -3: Need to realloc buffer, but no realloc provided.
    -4: Realloc failed.
    -5: Buffer too large.
*/
struct DynBuffer;
/** Report an allocation error synchronously from a buffer operation.
 * @param[in,out] o Buffer that encountered the error; borrowed, not transferred.
 * @param[in] eCode Negative status stored by DynBuffer_getECode: -1 missing
 * allocation callback, -2 allocation failure, -3 no resize callback, -4 resize
 * failure, -5 size arithmetic failure. This callback cannot supply a new buffer
 * or turn the failed operation into a success. */
typedef void (*DynBuffer_OnAllocError)(struct DynBuffer* o, int eCode);

/** A dynamic buffer. You either Subclass and implement the
    DynBuffer_OnAllocError method or set OnAllocError to NULL.
 */
typedef struct DynBuffer
#ifdef __cplusplus 
: public BufPrint
{
      /** Leave storage uninitialized; call DynBuffer_constructor before use. */
      DynBuffer() {}

      /** Create a dynamic buffer.
          \param startSize Nonnegative initial payload capacity in bytes. Zero
          defers allocation until data requires space. Allocations reserve an
          additional NUL byte. Keep capacities and cursor arithmetic within int.
          \param expandSize Nonnegative growth increment in bytes. Zero prevents
          an existing buffer from growing; a larger individual write may request
          a larger increment. Growth requires a realloc callback.
          \param alloc Borrowed allocator, or NULL for AllocatorIntf_getDefault().
          It must outlive the buffer and its storage.
          \param onAllocError Optional callback invoked synchronously on allocation
          errors; NULL disables notification. Inspect getECode after construction.
       */
      DynBuffer(int startSize, int expandSize, AllocatorIntf* alloc=0,
                DynBuffer_OnAllocError onAllocError=0);

      /** destructor. release memory by calling method DynBuffer::release.
      */
      ~DynBuffer();

      /** Free the current storage, reset the cursor and capacity, and invalidate
       * all borrowed buffer pointers. Does not destroy the allocator or reset
       * a previously recorded allocation error. Repeated release is harmless. */
      void release();

      /** Returns a pointer to the internal buffer. This pointer is
       * invalid after the DynBuffer reallocates the internal
       * buffer. Unlike the BufPrint::getBuf method, the buffer
       * returned by this method is zero terminated.
       * @return Borrowed NUL-terminated storage, or NULL if no allocation
       * exists. The pointer is also invalidated by release or destruction.
       */
      char* getBuf();

      /** @return Current payload byte count (cursor), excluding the NUL; this
       * is not the allocation capacity. */
      U32 getBufSize();

      /** Returns the error code if memory allocation failed. This
          @return Zero or a recorded negative error code as listed below.
          A fixed-size capacity failure may return -1 from a write without
          setting this code, so always check the operation result as well.
          This method is typically used when the DynBuffer_OnAllocError is
          set to NULL in the constructor.
          \code
          0: No error.
         -1: No allocator.
         -2: Malloc failed.
         -3: Need to realloc buffer, but no realloc provided.
         -4: Realloc failed.
         -5: Buffer too large.
         \endcode
       */
      int getECode();

      /** force buffer to expand.
       \param sizeNeeded Nonnegative number of additional bytes required after
       the current cursor, not a new total capacity. The cursor is unchanged.
       \returns 0 when sufficient space is available, -1 when growth fails or
       is disabled. A nonzero value is returned if there is not
       enough memory to expand the buffer i.e. if the AllocatorIntf
       provided in the constructor cannot re-allocate the buffer.
      */
      int expand(int sizeNeeded);

      /** Return a pointer to the internal cursor position in the
          internal dynamic buffer.
          @return Borrowed pointer at the cursor. Requires an allocated buffer;
          call expand with a positive size and check success first. The pointer
          is invalidated by reallocation or release.
          \sa expand incrementCursor
       */
      char* getCurPtr();

      /** Increments the internal cursor position.

      It is possible to manually format data in the internal
      buffer. This method advances the internal cursor by N bytes.
      @param[in] nBytes Nonnegative bytes already written at getCurPtr().
      Reserve enough space with expand first; this macro does not check bounds.

      example
      \code
      char data[]={"My data"};
      if(myBuf->expand(sizeof(data)-1) == 0) // -1: no need to store null term.
      {
         memcpy(myBuf->getCurPtr(), data, sizeof(data)-1);
         myBuf->incrementCursor(sizeof(data)-1);
      }
      \endcode
      */
      void incrementCursor(int nBytes);

      /** Convert a recognized allocation error code to a static message.
       * @param[in] eCode One of -2, -3, -4 or -5. Other values, including
       * -1 and zero, trigger an assertion in debug builds.
       * @return Borrowed static message; do not free it.
       */
      static const char* ecode2str(int eCode);
#else
{
   BufPrint super; /* inherits from BufPrint */
#if 0
}
#endif
#endif

      AllocatorIntf* alloc;
      DynBuffer_OnAllocError onAllocError;
      int startSize;
      int expandSize;
} DynBuffer;

#ifdef __cplusplus
extern "C" {
#endif
/** Initialize a dynamic buffer and optionally allocate its starting storage.
 * @param[out] o Caller-owned buffer object.
 * @param[in] startSize Nonnegative initial payload capacity in bytes.
 * @param[in] expandSize Nonnegative growth increment; zero disables growth.
 * @param[in] alloc Borrowed allocator or NULL for the default.
 * @param[in] onAllocError Optional synchronous allocation-error callback.
 * No status is returned; inspect DynBuffer_getECode after construction.
 * @see DynBuffer::DynBuffer */
BA_API void DynBuffer_constructor(DynBuffer* o, int startSize, int expandSize,
                           AllocatorIntf* alloc,
                           DynBuffer_OnAllocError onAllocError);
/** Release the current storage.
 * @param[in,out] o Initialized buffer; see DynBuffer::release. */
#define DynBuffer_destructor(o) DynBuffer_release(o)
/** Release the current storage and invalidate borrowed pointers.
 * @param[in,out] o Initialized buffer; see DynBuffer::release. */
BA_API void DynBuffer_release(DynBuffer* o);
/** Terminate and obtain the current payload.
 * @param[in,out] o Initialized buffer.
 * @return Borrowed NUL-terminated storage or NULL if unallocated. */
BA_API char* DynBuffer_getBuf(DynBuffer* o);
/** Read the payload length.
 * @param[in] o Initialized buffer.
 * @return Payload bytes, excluding the NUL. */
#define DynBuffer_getBufSize(o) BufPrint_getBufSize((BufPrint*)(o))
/** Get the current write position.
 * @param[in] o Initialized buffer with allocated storage.
 * @return Borrowed cursor pointer, invalidated by reallocation or release. */
#define DynBuffer_getCurPtr(o) (((BufPrint*)(o))->buf+((BufPrint*)(o))->cursor)
/** Advance after writing reserved storage.
 * @param[in,out] o Initialized buffer.
 * @param[in] nBytes Nonnegative bytes written; must fit in the reserved space. */
#define DynBuffer_incrementCursor(o,nBytes) ((BufPrint*)(o))->cursor+=nBytes
/** Read the allocation-error state.
 * @param[in] o Initialized buffer.
 * @return Zero or a negative recorded error; see DynBuffer::getECode. */
#define DynBuffer_getECode(o) ((o)->expandSize < 0 ? (o)->expandSize : 0)
/** Reserve additional space without advancing the cursor.
 * @param[in,out] o Initialized buffer.
 * @param[in] sizeNeeded Nonnegative byte count required after the cursor.
 * @return 0 when available, -1 on failure or disabled growth. */
BA_API int DynBuffer_expand(DynBuffer* o, int sizeNeeded);
/** Translate an allocation error.
 * @param[in] eCode One of -2, -3, -4 or -5; other values assert.
 * @return Borrowed static message; see DynBuffer::ecode2str. */
BA_API const char* DynBuffer_ecode2str(int eCode);
#ifdef __cplusplus
}

inline DynBuffer::DynBuffer(
   int startSize, int expandSize, AllocatorIntf* alloc,
   DynBuffer_OnAllocError onAllocError) {
   DynBuffer_constructor(this, startSize, expandSize, alloc, onAllocError); }
inline DynBuffer::~DynBuffer() {
   DynBuffer_destructor(this); }
inline void DynBuffer::release() {
   DynBuffer_release(this); }
inline char* DynBuffer::getBuf() {
   return DynBuffer_getBuf(this); }
inline U32 DynBuffer::getBufSize() {
   return DynBuffer_getBufSize(this); }
inline int DynBuffer::getECode() {
   return DynBuffer_getECode(this); }
inline int DynBuffer::expand(int sizeNeeded) {
   return DynBuffer_expand(this, sizeNeeded); }
inline char* DynBuffer::getCurPtr() {
   return DynBuffer_getCurPtr(this); }
inline void DynBuffer::incrementCursor(int nBytes) {
   DynBuffer_incrementCursor(this, nBytes); }
inline const char* DynBuffer::ecode2str(int eCode) {
   return DynBuffer_ecode2str(eCode); }
#endif

#endif
