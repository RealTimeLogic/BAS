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
 *   $Id: BufPrint.h 5978 2026-09-11 16:13:48Z wini $
 *
 *   COPYRIGHT:  Real Time Logic LLC, 2008 - 2026
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

/** @file BufPrint.h */

#ifndef __BufPrint_h
#define __BufPrint_h

#include <TargConfig.h>

#ifndef BA_API
#define BA_API
#endif

#include <stdarg.h>
#include <string.h>

struct BufPrint;

#ifdef __cplusplus
extern "C" {
#endif

/** Format into caller-owned storage using BufPrint::printf conversions.
 * @param[out] buf Writable buffer large enough for all output and NUL.
 * No capacity check is possible with this interface.
 * @param[in] fmt NUL-terminated format; following arguments must match it.
 * @return Output byte count excluding NUL on success, or a negative error.
 * Failure may leave partial output without a terminator.
 */

BA_API int basprintf(char* buf, const char* fmt, ...);

/** Format into a bounded caller-owned buffer.
 * @param[out] buf Writable storage of at least len bytes.
 * @param[in] len Positive capacity including space for the terminating NUL.
 * @param[in] fmt NUL-terminated format; following arguments must match it.
 * See BufPrint::printf for supported conversions.
 * @return Output byte count excluding NUL on success; -1 on insufficient
 * capacity or another formatting error. This is not the required length.
 * Overflow can assert in debug builds. Failure does not guarantee a
 * NUL-terminated result.
 */

BA_API int basnprintf(char* buf, int len, const char* fmt, ...);

#ifdef __cplusplus
#undef printf
}
#endif 

/** @defgroup BufPrint printf compatible function
   @{

\sa <a href="../../index.html" _target="top"> Barracuda Introduction </a>
 */

/** BufPrint flush callback function.

A BufPrint instance calls the flush callback function when the buffer
is full or when #BufPrint::flush is called. The callback can either
extend the buffer or flush and reset the buffer.

The following default callback is set if no callback is installed when
calling the BufPrint constructor:

\code
static int BufPrint_defaultFlush(struct BufPrint* bp, int sizeRequired)
{
   bp->cursor=0; // Reset
   baAssert(sizeRequired == 0); // Program error in code calling BufPrint_xxx
   return sizeRequired ? -1 : 0;
}
\endcode

\param o Borrowed writer. Update its buffer, capacity and cursor when
providing more space or consuming pending bytes.
\param sizeRequired Additional space requested in bytes. Zero denotes
an explicit flush, or initial buffer creation by printf/vprintf. A callback
that grows storage must preserve pending bytes. A callback that sends data
must reset the cursor and leave usable storage for continued output.
\return Zero on success; a negative status on failure. Use negative errors
consistently: some operations propagate the status, others return -1.
Earlier bytes may already have been sent when a later operation fails.

*/
typedef int (*BufPrint_Flush)(struct BufPrint* o, int sizeRequired);

/** The BufPrint class, which implements a compact printf-style
    formatter, is a base class used by several other classes.

    This class does not allocate memory for the buffer. Thus, any
    class using BufPrint must provide a buffer BufPrint can use. The
    output from printf is formatted in the buffer passed into the
    constructor. BufPrint calls the callback function BufPrint_Flush
    when the buffer is full. See #BufPrint_Flush for additional details.
 */
typedef struct BufPrint
{
#ifdef __cplusplus

      /** BufPrint constructor. When using this constructor, make sure
          to also call setBuf(). C constructor name: BufPrint_constructor

          \param userData Optional borrowed pointer (default NULL) stored in the BufPrint
          object and accessible in the flush callback.
          \param flush Optional callback; NULL selects the fixed-buffer callback. See
          #BufPrint_Flush for details.

          \sa setBuf(), getUserData()
      */
      BufPrint(void* userData=0, BufPrint_Flush flush=0);

   /** Initialize a writer using caller-owned storage.
       C constructor name: BufPrint_constructor2
       
       \param buf Borrowed writable storage; must remain valid while installed.
       \param size Nonnegative capacity in bytes. Supply usable storage before writing.
       \param userData Optional borrowed pointer (default NULL) stored in the BufPrint
       object and accessible in the flush callback.
       \param flush Optional callback; NULL selects the fixed-buffer callback. See
       #BufPrint_Flush for details.
       
       \sa setBuf(), getUserData()
   */
   BufPrint(char* buf,int size,void* userData=0,BufPrint_Flush flush=0);

      /** @return Borrowed user data pointer set in the constructor, possibly NULL.
       */
      void* getUserData();

      /** Format arguments into this writer.
 * @param[in] fmt NUL-terminated format using printf's supported conversions.
 * @param[in] argList Initialized argument list matching fmt. Its state after
 * this call is platform dependent; use va_copy if it is needed again.
 * @return Zero on success, negative status on failure; not a byte count.
 * Output is not automatically NUL-terminated or flushed. Partial output may
 * remain buffered or already have been sent on failure.
 * @sa printf
 */

      int vprintf(const char* fmt, va_list argList);

      /** Format values using the compact BAS formatter.
 * Supports integer conversions %%d, %%i, %%u, %%o, %%x, %%X; %%p, %%c, %%s and %%;
 * flags -, +, space, # and 0; width and precision (including *); and the
 * h, l and ll integer length modifiers. %%lld and %%llu format S64 and U64.
 * %%f, %%e, %%E, %%g and %%G require floating-point support in the build.
 * This is not a complete implementation of the C library printf.
 *
 * %%j takes a NUL-terminated UTF-8 string and emits a quoted JSON string using
 * jsonString. Precision limits input bytes and must not split a UTF-8 sequence.
 * %%s and %%j require complete NUL-terminated input even with precision.
 * NULL string pointers become "(null)" (%%j also adds JSON quotes).
 * Avoid %%n: this implementation does not maintain an output byte count.
 *
 * @param[in] fmt Required NUL-terminated format. Following arguments must
 * match the conversions and promoted C types. Keep widths and precisions
 * within signed-short range; literal fields are limited by the parser.
 * @return Zero on success, negative status on failure, not a byte count.
 * Partial output can remain on failure. Does not append NUL or flush.
 * @sa vprintf jsonString
 */

      int printf(const char* fmt, ...);

      /** Append one byte (C function: BufPrint_putc).
 * @param[in] c Character value converted to char.
 * @return Zero on success, -1 if obtaining buffer space fails.
 */

      int baputc(int c);

      /** Append bytes, flushing or expanding through the callback as needed.
 * @param[in] data Borrowed readable source; must not overlap destination
 * storage. With negative len, supply a NUL-terminated string.
 * @param[in] len Byte count, or a negative value to use strlen(data).
 * Length and cursor arithmetic must fit int.
 * @return Zero on success, negative callback status on failure. Earlier bytes
 * may already have been buffered or sent. Does not append NUL.
 */

      int write(const void* data, int len);

      /** Append a NUL-terminated string (C macro: BufPrint_write2).
 * @param[in] buf Required readable string, not overlapping writer storage.
 * Its length must fit int; the terminating NUL is not copied.
 * @return Zero on success, negative callback status on failure. Partial output
 * may already have been produced.
 */

      int write(const char* buf);


   /** Replace borrowed storage and reset the cursor, discarding pending data.
    Neither the old nor new storage is freed by BufPrint.
    \param buf Borrowed writable storage; must remain valid while installed.
    \param size Nonnegative capacity in bytes. Supply usable storage before writing.
   */
   void setBuf(char* buf, int size);


      /** Returns a pointer to the internal buffer. Please note that
       * the buffer returned by this method is not zero terminated.
       * @return Borrowed buffer pointer, possibly NULL before setup. The pointer
       * can change when the callback replaces or reallocates storage.
       * \sa BufPrint::getBufSize
       */
      char* getBuf();

      /** @return Pending byte count, not capacity or total bytes previously sent. */
      U32 getBufSize();


      /** resets the cursor, thus erasing the data in the buffer */
      void erase();

      /** Deliver pending bytes to the callback with sizeRequired zero.
       * @return Zero if empty; otherwise the callback status (zero success,
       * negative failure). The cursor is reset even if the callback fails. */
      int flush();

      /** Append standard Base64 with = padding, without a NUL terminator.
 * @param[in] data Borrowed source containing at least slen readable bytes.
 * @param[in] slen Nonnegative source length in bytes; zero produces no output.
 * @return Zero on success, -1 on output failure. Partial output is possible.
 * @sa baB64Decode
 */

      int b64Encode(const void* data, S32 slen);

      /** Append Base64url using - and _, without a NUL terminator.
 * @param[in] source Borrowed source containing at least slen readable bytes.
 * @param[in] slen Nonnegative source length in bytes; zero produces no output.
 * @param[in] padding True appends = padding, false omits it.
 * @return Zero on success, -1 on output failure. Partial output is possible.
 * @sa baB64Decode
 */

   int b64urlEncode(const void* source, S32 slen, bool padding);

      /** Append a complete quoted JSON string.
 * Escapes ASCII control characters, quotes, slash and backslash. UTF-8 is
 * validated and encoded as \\uxxxx sequences, using surrogate pairs for
 * characters outside the Basic Multilingual Plane. Does not append NUL.
 * @param[in] str Borrowed ASCII or UTF-8 bytes. Embedded NUL bytes are escaped.
 * @param[in] len Exact readable byte count; no NUL terminator is required.
 * @return Zero on success; negative status on invalid UTF-8 or output failure.
 * Failure can leave a partial quoted string in the buffer or output stream.
 * @sa printf
 */

      int jsonString(const char* str, size_t len);
#endif
      BufPrint_Flush flushCB;
      void *userData;
      char* buf;
      int cursor;
      int bufSize;
}BufPrint;

#define BufPrint_putcMacro(o, c) do { \
   if((o)->cursor == (o)->bufSize) \
   { \
      if((o)->flushCB(o, 1)) \
         return -1; \
   } \
   (o)->buf[(o)->cursor++] = c; \
} while(0)

#ifdef __cplusplus
extern "C" {
#endif
/** @param[in] o Initialized writer.
 * @return Borrowed callback context, possibly NULL. */
#define BufPrint_getUserData(o) (o)->userData
/** Discard pending bytes without flushing or releasing storage.
 * @param[in,out] o Initialized writer. */
#define BufPrint_erase(o) (o)->cursor=0
/** @param[in] o Initialized writer.
 * @return Borrowed buffer, possibly NULL; not automatically NUL-terminated.
 * Replacing or expanding storage invalidates this pointer. */
#define BufPrint_getBuf(o) (o)->buf
/** Replace borrowed storage and discard pending bytes.
 * @param[in,out] o Initialized writer.
 * @param[in] b Writable buffer, valid while installed; not freed by BufPrint.
 * @param[in] size Nonnegative capacity in bytes. */
#define BufPrint_setBuf(o, b, size) (o)->buf=b,(o)->bufSize=size,(o)->cursor=0
/** @param[in] o Initialized writer.
 * @return Pending byte count, not capacity or previously sent bytes. */
#define BufPrint_getBufSize(o) (o)->cursor
/** Initialize a writer without storage; install a buffer before writing.
 * @param[out] o Caller-owned writer.
 * @param[in] userData Optional borrowed callback context, or NULL.
 * @param[in] flush Callback or NULL for the default fixed-buffer callback.
 * @sa BufPrint_Flush */
BA_API void BufPrint_constructor(
   BufPrint* o,void* userData,BufPrint_Flush flush);
/** Initialize a writer with borrowed storage.
 * @param[out] o Caller-owned writer.
 * @param[in] buf Writable buffer, valid while installed.
 * @param[in] size Nonnegative buffer capacity in bytes.
 * @param[in] userData Optional borrowed callback context, or NULL.
 * @param[in] flush Callback or NULL for the default fixed-buffer callback.
 * @sa BufPrint_Flush */
BA_API void BufPrint_constructor2(
   BufPrint* o, char* buf,int size,void* userData,BufPrint_Flush flush);
/** No operation: BufPrint does not own its storage or context.
 * @param[in] o Writer being destroyed; release owned resources separately. */
#define BufPrint_destructor(o)
/** @copydoc BufPrint::vprintf
 * @param[in,out] o Initialized writer. */
BA_API int BufPrint_vprintf(BufPrint* o, const char* fmt, va_list argList);
/** @copydoc BufPrint::printf
 * @param[in,out] o Initialized writer. */
BA_API int BufPrint_printf(BufPrint* o, const char* fmt, ...);
/** @copydoc BufPrint::write(const void*, int)
 * @param[in,out] o Initialized writer. */
BA_API int BufPrint_write(BufPrint* o, const void* data, int len);
/** @copydoc BufPrint::baputc
 * @param[in,out] o Initialized writer. */
BA_API int BufPrint_putc(BufPrint* o, int c);
/** @copydoc BufPrint::flush
 * @param[in,out] o Initialized writer. */
BA_API int BufPrint_flush(BufPrint* o);
/** Append a NUL-terminated string without copying its terminator.
 * @param[in,out] o Initialized writer.
 * @param[in] data Required string, length fitting int, not overlapping storage.
 * @return Zero on success, negative callback error on failure; partial output
 * may have been produced. See BufPrint::write(const char*). */
#define BufPrint_write2(o, data) BufPrint_write(o, data, -1)
/** Append padded standard Base64; see BufPrint::b64Encode.
 * @param[in,out] o Initialized writer.
 * @param[in] source Borrowed readable bytes.
 * @param[in] slen Nonnegative source length in bytes.
 * @return Zero on success, -1 on output failure. Partial output is possible. */
BA_API int BufPrint_b64Encode(BufPrint* o, const void* source, S32 slen);
/** @copydoc BufPrint::b64urlEncode
 * @param[in,out] o Initialized writer. */
BA_API int BufPrint_b64urlEncode(
   BufPrint* o, const void* source, S32 slen, BaBool padding);

/** @copydoc BufPrint::jsonString
 * @param[in,out] o Initialized writer. */
BA_API int BufPrint_jsonString(BufPrint* o, const char* str, size_t len);
#ifdef __cplusplus
}
inline void* BufPrint::getUserData() {
   return BufPrint_getUserData(this);
}
inline BufPrint::BufPrint(void* userData, BufPrint_Flush flush) {
   BufPrint_constructor(this, userData,flush); }
inline BufPrint::BufPrint(
   char* buf,int size,void* userData,BufPrint_Flush flush) {
   BufPrint_constructor2(this,buf,size,userData,flush); }
inline int BufPrint::vprintf(const char* fmt, va_list argList) {
   return BufPrint_vprintf(this, fmt, argList); }
inline int BufPrint::printf(const char* fmt, ...) {
   int retv; va_list varg;
   va_start(varg, fmt);
   retv = BufPrint_vprintf(this, fmt, varg);
   va_end(varg);
   return retv;
}
inline char* BufPrint::getBuf() {
   return BufPrint_getBuf(this); }
inline void BufPrint::setBuf(char* buffer, int size) {
   BufPrint_setBuf(this, buffer, size); }
inline U32 BufPrint::getBufSize() {
   return BufPrint_getBufSize(this); }
inline void BufPrint::erase() {
   BufPrint_erase(this); }
inline int BufPrint::baputc(int c) {
   return BufPrint_putc(this, c); }
inline int BufPrint::write(const void* data, int len) {
   return BufPrint_write(this, data, len); }
inline int BufPrint::write(const char* data) {
   return BufPrint_write2(this, data); }
inline int BufPrint::flush() {
   return BufPrint_flush(this);
}
inline int BufPrint::b64Encode(const void* source, S32 slen){
   return BufPrint_b64Encode(this, source, slen);
}
inline int BufPrint::b64urlEncode(const void* source, S32 slen, bool padding){
   return BufPrint_b64urlEncode(this, source, slen, padding?TRUE:FALSE);
}
inline int BufPrint::jsonString(const char* str, size_t len){
   return BufPrint_jsonString(this, str, len);
}
#endif

/** @} */ /* end of BufPrint */


#endif
