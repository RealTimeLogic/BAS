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
 *   $Id: JEncoder.h 5978 2026-09-11 16:13:48Z wini $
 *
 *   COPYRIGHT:  Real Time Logic LLC, 2006-2026
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

/** @file JEncoder.h */

#ifndef __JEncoder_h
#define __JEncoder_h

#include "JVal.h"
#include "BufPrint.h"

/** @addtogroup JSONRef
@{
*/
 

/** The JEncoder can serialize a JSON JVAL syntax tree to the JSON
    text format. The JEncoder can also be used for assembling JSON
    text by calling the primitive methods in this class. Supply a valid sequence
    of names, values, and matching container delimiters. Writes are incremental:
    failure can leave partial output and does not roll back prior bytes. The
    borrowed JErr retains errors until explicitly reset by the caller.

Example:
\code
   JErr err;
   char buf[40]; //Must be sufficiently large for the JSON string
   BufPrint jBuf(buf,sizeof(buf));
   JEncoder jEnc(&err,&jBuf);
   int status = jEnc.set("{d}", "The number of the day is", (S32)5);
   //jBuf.buf == buf
   if(status == 0 && jBuf.cursor < sizeof(buf))
   {
      buf[jBuf.cursor]=0; // The encoder does not append a C-string terminator.
      printf("%s\n",buf); // Prints: {"The number of the day is":5}
   }
\endcode
*/
typedef struct JEncoder
{
#ifdef __cplusplus
      /** Initialize an encoder without writing output or resetting err.
    @param err Required initialized JErr, borrowed for the encoder's lifetime.
    @param out Required initialized writer, borrowed for the same lifetime.
    Start with a clear JErr. The encoder never owns or frees these objects.
 */
      JEncoder(JErr* err, BufPrint* out);
/** Call flush() without freeing the borrowed writer or error object.
    The C++ destructor cannot report a flush failure; call flush() explicitly
    and check its result before destruction when the outcome matters.
 */
      ~JEncoder();

      /** Write a JSON number.
    @param val S32 signed integer value.
    @return Zero on success, -1 for a stored format/value or output error.
    Inspect getErr(); output may already be partially written.
 */
      int setInt(S32 val);

      /** Write a JSON number.
    @param val S64 signed integer value.
    @return Zero on success, -1 for a stored format/value or output error.
    Inspect getErr(); output may already be partially written.
 */
      int setLong(S64 val);
#ifndef NO_DOUBLE

      /** Write a JSON number.
    @param val finite double value. Uses BufPrint floating formatting (%%f), not a lossless
    round-trip conversion. Available only when double support is enabled.
    @return Zero on success, -1 for a stored format/value or output error.
    Inspect getErr(); output may already be partially written.
 */
      int setDouble(double val);
#endif

      /** Write a quoted JSON string through BufPrint::jsonString.
    @param val Borrowed UTF-8 bytes, or NULL to write JSON null.
    @param len Byte count, excluding any terminator; ignored when val is NULL.
    Embedded NUL bytes are encoded as part of the string.
    @return Zero on success, -1 for a stored format/value or output error.
    Inspect getErr(); output may already be partially written.
 */
      int setString(const char* val, size_t len);

      /** Write binary data as a quoted standard Base64 string.
    @param source Readable bytes borrowed for this call; required for positive slen.
    @param slen Nonnegative source byte count.
    @return The current implementation returns -1 even after successful output.
    A stored JErr indicates an actual format/write failure. This return-value
    limitation is pending implementation repair.
 */
      int b64enc(const void* source, S32 slen);

      /** Format text inside JSON quotation marks.
    @param fmt BufPrint format string, or NULL to write JSON null. Variadic
    arguments must match its conversions. Formatted text is not automatically
    JSON-escaped; use appropriate BufPrint conversions or setString() instead.
    @return Intended to follow vFmtString().
    @warning The C++ wrapper forwards its va_list to the variadic C declaration,
    for which no implementation is present in the reviewed source. Use
    vFmtString() with a va_list or setString() until this is repaired.
 */
      int fmtString(const char* fmt,...);

      /** Write formatted text between JSON quotes, or JSON null.
    @param fmt Borrowed BufPrint format string, or NULL for null. Ensure its output
    is properly escaped JSON string content; this function only adds outer quotes.
    @param argList Valid argument list matching fmt, consumed according to va_list
    rules. Use va_copy if the original list must be reused.
    @return Zero on success, -1 for a stored format/value or output error.
    Inspect getErr(); output may already be partially written.
 */ 
      int vFmtString(const char* fmt,va_list argList);

      /** Write a JSON boolean.
    @param val boolean value.
    @return Zero on success, -1 for a stored format/value or output error.
    Inspect getErr(); output may already be partially written.
 */
      int setBoolean(bool val);

      /** Write JSON null.
    @return Zero on success, -1 for a stored format/value or output error.
    Inspect getErr(); output may already be partially written.
 */
      int setNull();

      /** Serialize a JVal node and its child values.
    @param val Borrowed acyclic node/tree; NULL emits nothing.
    @param iterateNext False (default) handles this node only; true also serializes
    its next-linked siblings in the current container context. Member names are
    emitted for object-member nodes when needed. No tree storage is taken.
    @return Zero on success, -1 for a stored format/value or output error.
    Inspect getErr(); output may already be partially written.
    Not available when built with NO_JVAL_DEPENDENCY.
 */
      int setJV(struct JVal* val, bool iterateNext=false);

      /** Encode/serialize C structs/data to JSON using formatted output.

      The set method works in a similar fashion to the ANSI C
      function printf; thus, it performs formatted output conversion.

      The method internally uses the 'setXXX' methods in this class
      for setting the actual value(s), where XXX is setInt, setLong, etc.. The
      following table shows the relationship between the format flags and
      the JSON value:

      <table>
      <tr><th>JSON type</th><th>Format flag</th><th>C type</th>
      <th>Method</th></tr>
      <tr><td>Number</td><td> d </td><td>S32</td>
      <td>JEncoder::setInt</td></tr>
      <tr><td>Number</td><td> l </td><td>S64</td>
      <td>JEncoder::setLong</td></tr>
      <tr><td>Number</td><td> f </td><td>double</td>
      <td>JEncoder::setDouble</td></tr>
      <tr><td>boolean</td><td> b </td><td>BaBool</td>
      <td>JEncoder::setBoolean</td></tr>
      <tr><td>string</td><td> s </td><td>const char*</td>
      <td>JEncoder::setString</td></tr>
      <tr><td>Any type</td><td> J </td><td>JVal*</td>
      <td>All methods</td></tr>
      <tr><td>Start object</td><td> { </td><td>n/a</td>
      <td>JEncoder::beginObject</td></tr>
      <tr><td>End object</td><td> } </td><td>n/a</td>
      <td>JEncoder::endObject</td></tr>
      <tr><td>Start array</td><td> [ </td><td>n/a</td>
      <td>JEncoder::beginArray</td></tr>
      <tr><td>End array</td><td> ] </td><td>n/a</td>
      <td>JEncoder::endArray</td></tr>
      <tr><td>Array</td><td>A followed by b, d, f, s, or J</td><td>int count, then typed array pointer</td>
      <td>All setXXX methods</td></tr>
      </table>

      \sa JE_MEMBER
      \sa JVal::get
      \sa JDecoder::get
            @param fmt Required NUL-terminated format with matching braces/brackets.
      Pass an object member name (const char*) before each object's value arguments.
      Flag n writes null without a value argument. Flag A takes an int count and
      an array pointer of BaBool, S32, double, const char*, or JVal* elements,
      selected by its following flag; long-integer arrays are not supported.
      All strings, arrays, and JVal trees are borrowed only for this call.
      Floating and JVal support depend on the build configuration.
      @return Zero on success, -1 on format/value/output error. Check JErr as well;
      output is incremental and is not rolled back on failure.
      */
      int set(const char* fmt, ...);

      /** Write an object member name before its value.
    @param name Required borrowed NUL-terminated UTF-8 name, escaped as JSON.
    Call only inside an object, once before each value.
    @return Zero on success, -1 for a stored format/value or output error.
    Inspect getErr(); output may already be partially written.
 */
      int setName(const char* name);

      /** Begin a JSON object value; call setName() before each member value.
    @return Zero on success, -1 for a stored format/value or output error.
    Inspect getErr(); output may already be partially written.
    Keep container nesting within 511 levels and balance all begin/end calls.
 */
      int beginObject();

      /** Close the current object after its final complete member.
    @return Zero on success, -1 for a stored format/value or output error.
    Inspect getErr(); output may already be partially written.
    Keep container nesting within 511 levels and balance all begin/end calls.
 */
      int endObject();

      /** Begin a JSON array value; array elements have no member-name call.
    @return Zero on success, -1 for a stored format/value or output error.
    Inspect getErr(); output may already be partially written.
    Keep container nesting within 511 levels and balance all begin/end calls.
 */
      int beginArray();

      /** Close the current array after its final complete element.
    @return Zero on success, -1 for a stored format/value or output error.
    Inspect getErr(); output may already be partially written.
    Keep container nesting within 511 levels and balance all begin/end calls.
 */
      int endArray();

      /** Query error storage.
    @return Required borrowed JErr pointer supplied at construction; does not clear errors.
 */
      JErr* getErr();

      /** Flush the underlying writer if JErr is clear.
    @return BufPrint::flush result, or -1 if JErr already records an error.
    A flush failure is not automatically copied into JErr. This does not validate
    balanced containers or reset encoder state.
 */
      int flush();

      /** Permit a new top-level value and flush the writer.
    @return Same result as flush(). The new-value flag is set even if flushing
    fails. This does not reset JErr or close/reset nested containers; call only
    after completing the current top-level value.
 */
      int commit();

      /** Query output storage.
    @return Borrowed BufPrint pointer supplied at construction; no ownership transfer.
 */
      BufPrint* getBufPrint();

#endif
      JErr* err;
      struct
      {
            S32 level;
            U8 data[64];
      }objectStack;
      BufPrint* out;
      BaBool objectMember;
      BaBool startNewObj;
} JEncoder;
#ifdef __cplusplus
extern "C" {
#endif
/** @copydoc JEncoder::JEncoder
    @param o Required storage to initialize.
 */
BA_API void JEncoder_constructor(JEncoder* o, JErr* err, BufPrint* out);
/** @copydoc JEncoder::~JEncoder
    @param o Required initialized encoder.
 */
#define JEncoder_destructor(o) JEncoder_flush(o)
/** @copydoc JEncoder::flush
    @param o Required initialized encoder.
 */
BA_API int JEncoder_flush(JEncoder* o);
/** @copydoc JEncoder::commit
    @param o Required initialized encoder.
 */
BA_API int JEncoder_commit(JEncoder* o);
/** @copydoc JEncoder::getErr
    @param o Required initialized encoder.
 */
#define JEncoder_getErr(o) (o)->err
/** @copydoc JEncoder::setInt
    @param o Required initialized encoder.
 */
BA_API int JEncoder_setInt(JEncoder* o, S32 val);
/** @copydoc JEncoder::setLong
    @param o Required initialized encoder.
 */
BA_API int JEncoder_setLong(JEncoder* o, S64 val);
#ifdef NO_DOUBLE
#define JEncoder_setDouble JEncoder_setInt
#else
/** @copydoc JEncoder::setDouble
    @param o Required initialized encoder.
 */
BA_API int JEncoder_setDouble(JEncoder* o, double val);
#endif

/** @copydoc JEncoder::fmtString
    @param o Required initialized encoder.
 */
BA_API int JEncoder_fmtString(JEncoder* o, const char* fmt,...);
/** @copydoc JEncoder::vFmtString
    @param o Required initialized encoder.
 */
BA_API int JEncoder_vFmtString(
   JEncoder* o, const char* fmt,va_list argList);
/** @copydoc JEncoder::setString
    @param o Required initialized encoder.
 */
BA_API int JEncoder_setString(JEncoder* o, const char* val, size_t len);
/** @copydoc JEncoder::b64enc
    @param o Required initialized encoder.
 */
BA_API int JEncoder_b64enc(JEncoder* o, const void* source, S32 slen);
/** @copydoc JEncoder::setBoolean
    @param o Required initialized encoder.
 */
BA_API int JEncoder_setBoolean(JEncoder* o, BaBool val);
/** @copydoc JEncoder::setNull
    @param o Required initialized encoder.
 */
BA_API int JEncoder_setNull(JEncoder* o);
/** @copydoc JEncoder::setJV
    @param o Required initialized encoder.
 */
BA_API int JEncoder_setJV(
   JEncoder* o, struct JVal* val, BaBool iterateNext);
BA_API int JEncoder_vSetJV(
   JEncoder* o,const char** fmt,va_list* argList);
/** @copydoc JEncoder::set
    @param o Required initialized encoder.
 */
BA_API int JEncoder_set(JEncoder* o, const char* fmt, ...);
/** @copydoc JEncoder::setName
    @param o Required initialized encoder.
 */
BA_API int JEncoder_setName(JEncoder* o, const char* name);
/** @copydoc JEncoder::beginObject
    @param o Required initialized encoder.
 */
BA_API int JEncoder_beginObject(JEncoder* o);
/** @copydoc JEncoder::endObject
    @param o Required initialized encoder.
 */
BA_API int JEncoder_endObject(JEncoder* o);
/** @copydoc JEncoder::beginArray
    @param o Required initialized encoder.
 */
BA_API int JEncoder_beginArray(JEncoder* o);
/** @copydoc JEncoder::endArray
    @param o Required initialized encoder.
 */
BA_API int JEncoder_endArray(JEncoder* o);
/** @copydoc JEncoder::getBufPrint
    @param o Required initialized encoder.
 */
#define JEncoder_getBufPrint(o) (o)->out
#ifdef __cplusplus
}
inline JEncoder::JEncoder(JErr* err, BufPrint* out) {
   JEncoder_constructor(this,err, out); }
inline JEncoder::~JEncoder() {
   JEncoder_destructor(this); }
inline int JEncoder::setInt(S32 val) {
   return  JEncoder_setInt(this, val); }
inline int JEncoder::setLong(S64 val) {
   return  JEncoder_setLong(this, val); }
#ifndef NO_DOUBLE
inline int JEncoder::setDouble(double val) {
   return  JEncoder_setDouble(this, val); }
#endif
inline int JEncoder::fmtString(const char* fmt,...) {
   int retv; va_list argList;
   va_start(argList, fmt); 
   retv = JEncoder_fmtString(this, fmt, argList);
   va_end(argList);
   return retv;
}
inline int JEncoder::vFmtString(const char* fmt,va_list argList) {
   return JEncoder_vFmtString(this, fmt, argList); }
inline int JEncoder::setString(const char* val, size_t len) {
   return  JEncoder_setString(this, val, len); }
inline int JEncoder::b64enc(const void* source, S32 slen) {
   return  JEncoder_b64enc(this, source, slen); }
inline int JEncoder::setBoolean(bool val) {
   return  JEncoder_setBoolean(this, val ? TRUE : FALSE); }
inline int JEncoder::setNull() {
   return  JEncoder_setNull(this); }
inline int JEncoder::setJV(struct JVal* val, bool iterateNext) {
   return  JEncoder_setJV(this,val,iterateNext?TRUE:FALSE); }
inline int JEncoder::set(const char* fmt, ...) {
   int retv; va_list argList;
   va_start(argList, fmt); 
   retv=JEncoder_vSetJV(this,&fmt,&argList);
   va_end(argList);
   return retv;
}
inline int JEncoder::setName(const char* name) {
   return  JEncoder_setName(this, name); }
inline int JEncoder::beginObject() {
   return  JEncoder_beginObject(this); }
inline int JEncoder::endObject() {
   return  JEncoder_endObject(this); }
inline int JEncoder::beginArray() {
   return  JEncoder_beginArray(this); }
inline int JEncoder::endArray() {
   return  JEncoder_endArray(this); }
inline JErr* JEncoder::getErr() {
   return JEncoder_getErr(this); }
inline int JEncoder::flush() {
   return JEncoder_flush(this); }
inline int JEncoder::commit() {
   return JEncoder_commit(this); }
inline BufPrint* JEncoder::getBufPrint() {
   return JEncoder_getBufPrint(this); }
#endif

/** JEncoder::set helper macro, used when setting a value for an object.
    encoder.set("{d}", JE_MEMBER(structval, membername));
 */
#define JE_MEMBER(o, m) #m, (o)->m


/** @} */ /* end of JSONRef */ 

#endif
