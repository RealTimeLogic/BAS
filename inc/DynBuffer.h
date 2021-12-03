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
 *   $Id: DynBuffer.h 4915 2021-12-01 18:26:55Z wini $
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
typedef void (*DynBuffer_OnAllocError)(struct DynBuffer* o, int eCode);

/** A dynamic buffer. You either Subclass and implement the
    DynBuffer_OnAllocError method or set OnAllocError to NULL.
 */
typedef struct DynBuffer
#ifdef __cplusplus 
: public BufPrint
{
      DynBuffer() {}

      /** Create a dynamic buffer.
          \param startSize the size allocated by calling malloc.
          \param expandSize chunk size used when calling realloc. Set
          to 0 if you do not want the buffer to dynamically grow.
          \param alloc the allocator used by the dynamic buffer.
          \param onAllocError pointer to function called on allocation error.
       */
      DynBuffer(int startSize, int expandSize, AllocatorIntf* alloc=0,
                DynBuffer_OnAllocError onAllocError=0);

      /** destructor. release memory by calling method DynBuffer::release.
      */
      ~DynBuffer();

      /** terminate the internal dynamic buffer by calling free */
      void release();

      /** Returns a pointer to the internal buffer. This pointer is
       * invalid after the DynBuffer reallocates the internal
       * buffer. Unlike the BufPrint::getBuf method, the buffer
       * returned by this method is zero terminated.
       */
      char* getBuf();

      /** Returns current size of internal formatted data */
      U32 getBufSize();

      /** Returns the error code if memory allocation failed. This
          method is typically used when the DynBuffer_OnAllocError is
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
       \param sizeNeeded the required expand size.
       \returns 0 on success. A non zero value is returned if it is not
       enough memory to expand the buffer i.e. if the AllocatorIntf
       provided in the constructor cannot re-allocate the buffer.
      */
      int expand(int sizeNeeded);

      /** Return a pointer to the internal cursor position in the
          internal dynamic buffer.
          \sa expand incrementCursor
       */
      char* getCurPtr();

      /** Increments the internal cursor position.

      It is possible to manually format data in the internal
      buffer. This method advances the internal cursor by N bytes.

      example
      \code
      char data[]={"My data"};
      if(myBuf->expand(sizeof(data)-1) == 0) // -1: no need to store null term.
      {
         memcpy(myBuf->getCurPtr(), data, sizeof(data)-1);
         myBuf->incrementCursor(sizeof(data)-1);
      \endcode
      */
      void incrementCursor(int nBytes);

      /** Convert error code to string. Used by the onAllocError callback.
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
BA_API void DynBuffer_constructor(DynBuffer* o, int startSize, int expandSize,
                           AllocatorIntf* alloc,
                           DynBuffer_OnAllocError onAllocError);
#define DynBuffer_destructor(o) DynBuffer_release(o)
BA_API void DynBuffer_release(DynBuffer* o);
BA_API char* DynBuffer_getBuf(DynBuffer* o);
#define DynBuffer_getBufSize(o) BufPrint_getBufSize((BufPrint*)(o))
#define DynBuffer_getCurPtr(o) (((BufPrint*)(o))->buf+((BufPrint*)(o))->cursor)
#define DynBuffer_incrementCursor(o,nBytes) ((BufPrint*)(o))->cursor+=nBytes
#define DynBuffer_getECode(o) ((o)->expandSize < 0 ? (o)->expandSize : 0)
BA_API int DynBuffer_expand(DynBuffer* o, int sizeNeeded);
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
