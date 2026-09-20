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
 *   $Id: ubjson.h 6056 2026-09-20 05:09:33Z wini $
 *
 *   COPYRIGHT:  Real Time Logic LLC, 2014 - 2026
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

 Universal Binary JSON http://ubjson.org/
 Core API and parser

*/

/** @file ubjson.h */

#ifndef __ubjson_h
#define __ubjson_h

#include <TargConfig.h>
#include <stdarg.h>

/** @addtogroup UBJSONRef
@{
*/


#ifndef UBJ_ALIGNMT
#define UBJ_ALIGNMT  uintptr_t
#endif
#define UBJ_POINTER_NOT_ALIGNED(p)                      \
   (0 != ((uintptr_t)p & (sizeof(UBJ_ALIGNMT) - 1)))


#define UBJPARS_STACK_LEN 3

/** UBJSON Value Type: See UBJVal::t and the
    [UBJSON type reference](http://ubjson.org/type-reference/)
    for more information
*/
typedef enum {
   UBJT_InvalidType=0, /**< Not a valid UBJSON type */
   UBJT_Null='Z', /**< Type UBJT_Null */
   UBJT_NoOp='N', /**< Type UBJT_NoOp */
   UBJT_Boolean, /**< Type UBJT_Boolean */
   UBJT_Int8='i', /**< Type UBJT_Int8 */
   UBJT_Uint8='U', /**< Type UBJT_Uint8 */
   UBJT_Int16='I', /**< Type UBJT_Int16 */
   UBJT_Int32='l', /**< Type UBJT_Int32 */
   UBJT_Int64='L', /**< Type UBJT_Int64 */
   UBJT_Float32='d', /**< Type UBJT_Float32 */
   UBJT_Float64='D', /**< Type UBJT_Float64 */
   UBJT_HNumber='H', /**< Raw numeric text; the parser does not validate its grammar. */
   UBJT_Char='C', /**< Type UBJT_Char */
   UBJT_String='S', /**< Type UBJT_String */
   UBJT_BeginObject='{', /**< Type UBJT_BeginObject */
   UBJT_BeginArray='[', /**< Type UBJT_BeginArray */
   UBJT_Count='#', /**< Type UBJT_Count */
   UBJT_EndObject='}', /**< Type UBJT_EndObject */
   UBJT_EndArray=']' /**< Type UBJT_EndArray */
} UBJT;


/** UBJSON Parser Status.
 */
typedef enum {

   /** The parser completed parsing a new UBJSON object.
    */
   UBJPStatus_DoneEOS=1,

   /** The parser completed parsing a new UBJSON object, but found the
       unread bytes in the supplied buffer, not yet validated as another container.
    */
   UBJPStatus_Done,

    /** The parser requires more data to complete. */
   UBJPStatus_NeedMoreData = 100,

   /** UBJSON Parse error
    */
   UBJPStatus_ParseErr = 200,

   /** The UBJParserIntf interface reported a problem.
    */
   UBJPStatus_IntfErr,

   /** Parsed data/array/object too big.
    */
   UBJPStatus_Overflow
} UBJPStatus; 

/** UBJParser Value (passed into the parser callback interface UBJPIntf) */
typedef struct {
   /** The value type is controlled by type (t) */
   union {
      U8 uint8;  /**< Use when 't' is \ref UBJT_Uint8 or \ref UBJT_Boolean */
      S8 int8;   /**< Use when 't' is \ref UBJT_Int8 */
      char ch;   /**< Use when 't' is  \ref UBJT_Char */
      S16 int16; /**< Use when 't' is \ref UBJT_Int16 */
      S32 int32; /**< Use when 't' is \ref UBJT_Int32 */
      S64 int64; /**< Use when 't' is \ref UBJT_Int64 */
#ifndef NO_DOUBLE
      double float64; /**< Use when 't' is \ref UBJT_Float64 */
      float float32; /**< Use when 't' is \ref UBJT_Float32 */
#endif
      const char* string; /**< Borrowed chunk for String/HNumber; not NUL-terminated. Use len and x. */
   } u;

   /** When t=UBJT_Count: container len, When t=UBJT_String: string chunk len */
   S32 len;

   /** When t=UBJT_Count: type UBJT. When t=UBJT_String: remaining string length */
   S32 x;

   /** Object member name: set when parent is an object */
   char* name; 

   /** UBJSON Type, t is set to one of the types in UBJT */
   U8 t;
} UBJVal;


#ifdef __cplusplus
extern "C" {
#endif

/* Sets the value type (t) and the union (u) to the smallest number
   representation for 'in'
*/
/** Choose the smallest supported integer representation.
    @param o Required output record; sets t and its selected union member only.
    @param in Signed 64-bit input value. Other record fields remain unchanged.
 */
void UBJVal_setMinInteger(UBJVal* o, S64 in);

#ifdef __cplusplus
}
#endif


/** The event based parser uses a stack instead of recursion and the
 * UBJPStackNode type represent one entry in the stack.
 */
typedef struct
{
   S32 count; /* Set if optimized format */
   S32 ix; /* Current index goes from 0 to count-1 */
   U8 isObj; /* TRUE for object, FALSE for Array */
   U8 stronglyTyped; /* Set to token type if a strongly typed container */
   U8 state; /* Container header, body, member value, or pending count */
} UBJPStackNode;


struct UBJPIntf;

/** Receive a parsed UBJSON event synchronously.
    @param o Required callback interface.
    @param v Borrowed value event; inspect t before its union. Strings/HNumber are
    delivered in chunks: len is this chunk's bytes and x is bytes remaining.
    Neither chunk data nor member-name storage may be retained without copying.
    @param recLevel Top-level begin/end depth is zero; primitive children use
    one greater than their containing level. Count events use the container level.
    @return Zero to continue, nonzero to stop with UBJPStatus_IntfErr. Do not
    destroy the parser during its callback.
 */
typedef int (*UBJPIntf_Service)(struct UBJPIntf* o, UBJVal* v, int recLevel);


/** The UBJPIntf interface class is the interface between the parser and
    an object that implements the UBJPIntf interface.
    \sa UBJPIntf_Service
 */
typedef struct UBJPIntf
{
#ifdef __cplusplus
   /** Create the callback interface object.
       \param s Required callback, callable while installed.
    */
   UBJPIntf(UBJPIntf_Service s);
   ~UBJPIntf();
   UBJPIntf() {}
#endif
   UBJPIntf_Service service;
} UBJPIntf;

#define UBJPIntf_constructor(o,serviceMA) (o)->service=serviceMA
#define UBJPIntf_destructor(o)
#define UBJPIntf_service(o,v,recLev) (o)->service(o,v,recLev)

#ifdef __cplusplus
inline UBJPIntf::UBJPIntf(UBJPIntf_Service s) {
   UBJPIntf_constructor(this, s);
}
inline UBJPIntf::~UBJPIntf() {
   UBJPIntf_destructor(this);
}
#endif

/** The UBJSON parser parses a binary UBJSON stream and calls the UBJPIntf
    callback interface for each parsed object/primitive type.
    @note This implementation may not match the latest UBJSON specification.
    Specification compatibility is reserved for a separate review. The contracts
    here describe the current implementation, not a conformance certification.

    Only object/array roots are supported. String payloads must be UTF-8;
    the parser does not validate this encoding. High-precision H values are
    delivered as raw text for the application to interpret or reject.

    \sa UBJVal
    \sa JParser
 */
typedef struct UBJParser
{
#ifdef __cplusplus
   /** Create the callback interface object.
       \param intf Required borrowed interface with a non-NULL callback.
       \param name is a buffer used for storing an object member name
       during parsing. Required writable storage retained for the parser lifetime.
       \param memberNameLen is the length of the object member name
       buffer. The length must be no less than the largest member name
       expected, plus one byte for its NUL terminator. Empty member names
       are rejected. Names containing NUL bytes are unsupported.
       \param extraStackLen informs the parser that it can use a stack
       larger than the default depth of 3. Use zero for an ordinary object.
       Positive values require the additional writable storage shown below and
       must fit the integer stack-length representation. The memory for the
       UBJParser object must be constructed as follows:
       malloc(sizeof(UBJParser) + extraStackLen *
       sizeof(UBJPStackNode))
   */
   UBJParser(UBJPIntf* intf,char* name,int memberNameLen,int extraStackLen=0);

   /** Feed or resume parsing a top-level UBJSON object/array.
    @param buf Borrowed binary input chunk. Supply new input after DoneEOS or
    NeedMoreData. After Done, this argument is ignored while the parser consumes
    unread bytes retained from the previous chunk; keep that storage valid.
    @param size New chunk byte count, ignored during continuation after Done.
    @return 0 needs more bytes, 1 completed one container, -1 reports a parse,
    callback, or capacity error. Inspect getStatus(). NeedMoreData at actual end
    of input means incomplete input. No whole-document allocation is made.
    After failure, reinitialize before reuse. Callback effects are not rolled back.
    A complete first container does not validate trailing bytes or another document.
 */ 
   int parse(const U8* buf, U32 size);

   /** No-op destructor. Input, name storage, and callback interface are borrowed
    and never freed by the parser. Stop using the parser before releasing them.
 */
   ~UBJParser();

   /** Query the current optimized-container position.
    @return Zero-based current index when a count is active, otherwise -1.
    Intended for callbacks while the container is active; do not interpret stale
    stack state after completion/error as a current position.
 */
   int getIndex();

   /** Query the current container's declared count.
    @return Nonnegative count for an optimized container, otherwise -1.
    Meaningful only while the container is active, typically during a callback.
 */
   int getCount();

   /** @return Current UBJPStatus as an int, initially DoneEOS. This query does
    not advance parsing or clear the status.
 */
   int getStatus();

#endif
   const U8* lxBufEnd;
   const U8* lxTokenPtr;
   U8* valPtr;
   S32 lxBytes2Read;
   U8 lxParseX; /* Parsing one of: S, H # */
   U8 lxState; /* UBJLxState */
   U8 status; /* UBJPStatus */
   U8 pState; /* parser state */
   UBJVal val;
   UBJPIntf* intf;
   int memberNameLen;
   int stackIx; /* index in 'stack' */
   int stackLen; /* min UBJPARS_STACK_LEN */
   U8 stringFragment;
   UBJPStackNode stack[UBJPARS_STACK_LEN];
} UBJParser;

#ifdef __cplusplus
extern "C" {
#endif
/** @copydoc UBJParser::UBJParser
    @param o Required storage to initialize.
 */
void UBJParser_constructor(UBJParser* o, UBJPIntf* intf, char* name,
                           int memberNameLen, int extraStackLen);
/** @copydoc UBJParser::~UBJParser
    @param o Required initialized parser.
 */
#define UBJParser_destructor(o)
/** @copydoc UBJParser::parse
    @param o Required initialized parser.
 */
int UBJParser_parse(UBJParser* o, const U8* buf, U32 size);
/** @copydoc UBJParser::getIndex
    @param o Required initialized parser.
 */
#define UBJParser_getIndex(o)  (o)->stack[(o)->stackIx].ix
/** @copydoc UBJParser::getCount
    @param o Required initialized parser.
 */
#define UBJParser_getCount(o)  (o)->stack[(o)->stackIx].count
/** @copydoc UBJParser::getStatus
    @param o Required initialized parser.
 */
#define UBJParser_getStatus(o) (o)->status
#ifdef __cplusplus
}
inline UBJParser::UBJParser(
   UBJPIntf* intf, char* name, int memberNameLen, int extraStackLen) {
   UBJParser_constructor(this, intf, name, memberNameLen, extraStackLen);
}
inline UBJParser::~UBJParser() {
   UBJParser_destructor(this);
}
inline int UBJParser::parse(const U8* buf, U32 size) {
   return UBJParser_parse(this, buf, size);
}
inline int UBJParser::getIndex() {
   return UBJParser_getIndex(this);
}
inline int UBJParser::getCount() {
   return UBJParser_getCount(this);
}
inline int UBJParser::getStatus() {
   return UBJParser_getStatus(this);
}
#endif

struct UBJEBuf;

/** Make room in an encoder buffer by flushing or expanding it.
    @param o Required borrowed buffer object. Preserve unflushed bytes when growing.
    After flushing, reset cursor to zero; when growing, update data and dlen.
    @param sizeRequired Additional capacity requested in bytes.
    @return Zero after providing usable space, nonzero on failure. A successful
    callback must leave writable capacity; the encoder does not retry indefinitely.
    Buffer ownership remains with the application, and prior output is not rolled back.
 */
typedef int (*UBJEBuf_FlushCB)(struct UBJEBuf* o, int sizeRequired);

/** The UBJSON Encoder buffer is used by the encoder UBJEncoder */
typedef struct UBJEBuf
{
#ifdef __cplusplus
   /** Initialize the UBJSON Encoder buffer.
       \param cb Required expansion/flush callback; remains callable while used.
       \param buf Required borrowed writable encoder buffer, retained while used.
       \param bufLen Positive buffer capacity in bytes, at least 14. No storage is
       allocated and the initial cursor is zero.
   */
   UBJEBuf(UBJEBuf_FlushCB cb, U8* buf, S32 bufLen);
/** No-op destructor; neither flushes nor frees application buffer storage.
 */
   ~UBJEBuf();
#endif
   UBJEBuf_FlushCB flushCB;
   U8* data;
   S32 dlen;
   S32 cursor;
} UBJEBuf;

#define UBJEBuf_constructor(o, flushCBM, dataM, dlenM) \
   (o)->flushCB=flushCBM,(o)->data=dataM,(o)->dlen=dlenM,(o)->cursor=0
#define UBJEBuf_destructor(o)

#ifdef __cplusplus
inline UBJEBuf::UBJEBuf(UBJEBuf_FlushCB cb, U8* buf, S32 bufLen) {
   UBJEBuf_constructor(this, cb,buf,bufLen);
}
inline UBJEBuf::~UBJEBuf() {
   UBJEBuf_destructor(this);
}
#endif

/** UBJSON Encoder status */
typedef enum
{
   /** UBJEBuf_FlushCB returned a non zero value */
   UBJEStatus_FlushErr = -100,

   /**  Length required for Optimized Strongly Typed Container */
   UBJEStatus_LengthRequired,

   /** Object depth exceeding internal stack */
   UBJEStatus_StackOverflow,

   /** The value set does not match the type set for Optimized Strongly Typed */
   UBJEStatus_TypeMismatch,

   /** (curly) bracket start/end mismatch */
   UBJEStatus_Unbalanced,

   /** Unknown type (Must be a type from UBJT) */ 
   UBJEStatus_Unknown,

   /** Empty object member names are unsupported. */
   UBJEStatus_EmptyName,
   
   /** No error */
   UBJEStatus_ok=0
} UBJEStatus;


/** UBJSON Encoder.
    The encoder performs limited error checking and you can produce
    incorrect UBJSON data if used incorrectly. Supply matching containers and
    exact optimized counts. Output is incremental, and an error can leave partial
    data. Final buffered bytes must be consumed/flushed by the application; the
    destructor does not flush. The current implementation has not been verified
    against the complete UBJSON specification. Supply UTF-8 strings and
    nonempty member names without embedded NUL bytes. UTF-8 is not validated.
    Empty member names return UBJEStatus_EmptyName. Native floating-point
    values must be finite; the caller must explicitly encode null for
    nonfinite values. H numeric text must follow JSON number syntax;
    the encoder does not validate that syntax.
 */
typedef struct UBJEncoder
{
#ifdef __cplusplus

   /** Create/initialize an UBJEncoder instance.
       \param buf a buffer that either buffers all produced UBJSON
       data or small chunks, which are then flushed out to a stream
       when the buffer is full. Required borrowed initialized UBJEBuf,
       which must outlive the encoder.
    */
   UBJEncoder(UBJEBuf* buf);

   /** No-op destructor; does not flush output or free the borrowed buffer.
 */
   ~UBJEncoder();

   /** Store a name for the next object value; do not use for array elements.
    @param n Borrowed NUL-terminated name retained until the next value is emitted.
    @return The C macro yields the assigned pointer, not a status code.
    @warning The C++ wrapper declares int but returns that pointer expression;
    it requires a separate source correction. No output is produced by the setter.
 */
   int setName(const char* n);

   /** Emit UBJSON null.
    @return Zero on success, negative UBJEStatus on a detected error. The error
    remains in status and blocks later value output until reset/reinitialization.
 */
   int null();

   /** Emit one boolean value.
    @param b Value copied into output.
    @return Zero on success, negative UBJEStatus on a detected error. The error
    remains in status and blocks later value output until reset/reinitialization.
 */
   int boolean(bool b);

   /** Emit one uint8 value.
    @param v Value copied into output.
    @return Zero on success, negative UBJEStatus on a detected error. The error
    remains in status and blocks later value output until reset/reinitialization.
 */
   int uint8(U8 v);

   /** Emit one int8 value.
    @param v Value copied into output.
    @return Zero on success, negative UBJEStatus on a detected error. The error
    remains in status and blocks later value output until reset/reinitialization.
 */
   int int8(S8 v);

   /** Emit one character value.
    @param v Value copied into output. Supply a protocol-valid single-byte character.
    @return Zero on success, negative UBJEStatus on a detected error. The error
    remains in status and blocks later value output until reset/reinitialization.
 */
   int character(char v);

   /** Emit one int16 value.
    @param v Value copied into output.
    @return Zero on success, negative UBJEStatus on a detected error. The error
    remains in status and blocks later value output until reset/reinitialization.
 */
   int int16(S16 v);

   /** Emit one int32 value.
    @param v Value copied into output.
    @return Zero on success, negative UBJEStatus on a detected error. The error
    remains in status and blocks later value output until reset/reinitialization.
 */
   int int32(S32 v);

   /** Emit one int64 value.
    @param v Value copied into output.
    @return Zero on success, negative UBJEStatus on a detected error. The error
    remains in status and blocks later value output until reset/reinitialization.
 */
   int int64(S64 v);

   /** Emit one float64 value.
    @param v Value copied into output. Requires floating-point support.
    @return Zero on success, negative UBJEStatus on a detected error. The error
    remains in status and blocks later value output until reset/reinitialization.
 */
   int float64(double v);

   /** Emit one float32 value.
    @param v Value copied into output. Requires floating-point support.
    @return Zero on success, negative UBJEStatus on a detected error. The error
    remains in status and blocks later value output until reset/reinitialization.
 */
   int float32(float v);

   /** Emit a length-delimited string.
    @param s Readable UTF-8 bytes borrowed for this call, required for positive len.
    @param len Nonnegative byte count, excluding a terminator. The encoder does
    not validate UTF-8. Embedded NUL bytes are included.
    @return Zero on success, negative UBJEStatus on a detected error. The error
    remains in status and blocks later value output until reset/reinitialization.
 */
   int string(const char* s, S32 len);

   /** Begin formatting an array. Use the default values for the two
       parameters, or consult the UBJSON specification, section:
       Optimized Format.

       \param count must be set to the number of elements in the array
       when formatting an optimized array with count or to -1 if not
       used.

       \param type must be set to the type of the optimized array if
       formatting a strongly typed array, or to UBJT_InvalidType if
       not used.
    
    @return Zero on success, negative UBJEStatus on a detected error. The error
    remains in status and blocks later value output until reset/reinitialization.
    Count must equal the number of values/members emitted; the encoder does not
    count them for you. Keep nesting within 63 containers. General nested
    strongly typed combinations need separate compatibility verification.
    */
   int beginArray(S32 count=-1, UBJT type=UBJT_InvalidType);

   /** Begin formatting an object. Use the default values for the two
       parameters, or consult the UBJSON specification, section:
       Optimized Format.

       \param count must be set to the number of elements in the object
       when formatting an optimized object with count or to -1 if not
       used.

       \param type must be set to the type of the optimized object if
       formatting a strongly typed object, or to UBJT_InvalidType if
       not used.
    
    @return Zero on success, negative UBJEStatus on a detected error. The error
    remains in status and blocks later value output until reset/reinitialization.
    Count must equal the number of values/members emitted; the encoder does not
    count them for you. Keep nesting within 63 containers. General nested
    strongly typed combinations need separate compatibility verification.
    */
   int beginObject(S32 count=-1, UBJT type=UBJT_InvalidType);

   /** Close the matching current container. A counted container omits its
    end marker from the wire, but this call is still required for encoder state.
    @return Zero on success, negative UBJEStatus on a detected error. The error
    remains in status and blocks later value output until reset/reinitialization.
 */
   int endArray();

   /** Close the matching current container. A counted container omits its
    end marker from the wire, but this call is still required for encoder state.
    @return Zero on success, negative UBJEStatus on a detected error. The error
    remains in status and blocks later value output until reset/reinitialization.
 */
   int endObject();

   /** Discard buffered bytes and clear status and the pending member name.
    This does not flush output or reset nesting/strongly-typed state. Use only
    after a balanced complete value. Reinitialize to recover from a partial container.
 */
   void reset();

   /** Emit formatted values as for set(), advancing the format cursor.
    @param fmt Required pointer to a NUL-terminated format cursor.
    @param argList Required pointer to a matching initialized va_list, consumed.
    @param isObj True consumes a member-name argument before each value; false
    emits unnamed values. Use the matching container context.
    @return Zero on success, negative UBJEStatus on a detected error. The error
    remains in status and blocks later value output until reset/reinitialization.
 */
   int vset(const char** fmt, va_list* argList, bool isObj);

   /** Encode/serialize C structs/data to UBJSON using formatted output.

      The set method works in a similar fashion to the ANSI C
      function printf; thus, it performs formatted output conversion.

      The method internally uses the 'setXXX' methods in this class
      for setting the actual value(s), where XXX is uint8, float32, etc.. The
      following table shows the relationship between the format flags and
      the UBJSON values:

       \param fmt format flags:
       <table>
       <tr><th>UBJSON type</th><th>Format flag</th><th>C type</th></tr>
       <tr><td>bool</td><td>b</td><td>U8</td></tr>
       <tr><td>int8</td><td>i</td><td>S8</td></tr>
       <tr><td>uint8</td><td>U</td><td>U8</td></tr>
       <tr><td>int16</td><td>I</td><td>S16</td></tr>
       <tr><td>int32</td><td>l</td><td>S32</td></tr>
       <tr><td>int64</td><td>L</td><td>S64</td></tr>
       <tr><td>float32</td><td>d</td><td>float</td></tr>
       <tr><td>float64</td><td>D</td><td>double</td></tr>
       <tr><td>char</td><td>C</td><td>S8</td></tr>
       <tr><td>string</td><td>S</td><td>char*</td></tr>
       </table>

      \sa UBJE_MEMBER
      \sa UBJDecoder::get
          Format braces/brackets describe ordinary containers. Each object member
      takes a const char* name before its value. S takes a non-NULL NUL-terminated
      string. Arguments must follow C variadic promotions and the exact expected
      type; use primitive methods when avoiding variadic conversions.
      @return Zero on success, a negative status on detected format/output errors.
      @warning The current d (float32) variadic path reads float instead of the
      C-promoted double. Use the primitive float32() method for this value type.
      */
   int set(const char* fmt, ...);
#endif

   UBJVal val;
   struct  /* Used as bit field and set if the object/array has a 'count' */
   {
      S32 level;
      U8 data[8];
   } countStack;
   UBJEBuf* buf;
   int status;
   UBJT stronglyTyped;
} UBJEncoder;


/** UBJEncoder::set helper macro, used when setting a value for an object.
    encoder.set("{d}", UBJE_MEMBER(structval, membername));
 */
#define UBJE_MEMBER(o, m) #m, (o)->m

/** Initialize the encoder; no allocation, flush, or return value.
    @param o Required encoder storage.
    @param ubjsBuf Required borrowed initialized UBJEBuf.
 */
#define UBJEncoder_constructor(o, ubjsBuf)      \
   memset(o,0,sizeof(UBJEncoder)),(o)->buf=ubjsBuf
/** No-op; does not flush or free buffers.
    @param o Required initialized encoder.
    
 */
#define UBJEncoder_destructor(o)
/** Discard buffered bytes and clear status/name. Nesting state is not reset. Returns 0.
    @param o Required initialized encoder.
    
 */
#define UBJEncoder_reset(o) ((o)->status=0,(o)->val.name=0,o->buf->cursor=0,0)

#ifdef __cplusplus
extern "C" {
#endif
int UBJEncoder_setStatus(UBJEncoder* o, UBJEStatus s);
int UBJEncoder_val(UBJEncoder* o);
/** @copydoc UBJEncoder::vset
    @param o Required initialized encoder.
 */
int UBJEncoder_vset(UBJEncoder* o,const char** fmt,va_list* argList,int isObj);
/** @copydoc UBJEncoder::set
    @param o Required initialized encoder.
 */
int UBJEncoder_set(UBJEncoder* o, const char* fmt, ...);
#ifdef __cplusplus
}
#endif

/** Set the pending member name. Returns the assigned char pointer (not a status).
    @param o Required initialized encoder.
    @param v Borrowed nonempty NUL-terminated member name, valid until the next
    value. Embedded NUL bytes are unsupported. An empty name is rejected by
    the next value operation with UBJEStatus_EmptyName.
 */
#define UBJEncoder_setName(o,v) ((o)->val.name=(char*)v)
/** Encode null.
    @param o Required initialized encoder.
    
    @return 0 on success, negative encoder/output status on failure.
    Partial output is possible. See the corresponding UBJEncoder method.
 */
#define UBJEncoder_null(o) ((o)->val.t=UBJT_Null,UBJEncoder_val(o))
/** Encode a boolean.
    @param o Required initialized encoder.
    @param v Boolean value; zero false, nonzero true.
    @return 0 on success, negative encoder/output status on failure.
    Partial output is possible. See the corresponding UBJEncoder method.
 */
#define UBJEncoder_boolean(o,v)                                         \
   ((o)->val.t=UBJT_Boolean,(o)->val.u.uint8=v,UBJEncoder_val(o))
/** Encode a U8 value.
    @param o Required initialized encoder.
    @param v Value representable in U8.
    @return 0 on success, negative encoder/output status on failure.
    Partial output is possible. See the corresponding UBJEncoder method.
 */
#define UBJEncoder_uint8(o,v)                                   \
   ((o)->val.t=UBJT_Uint8,(o)->val.u.uint8=v,UBJEncoder_val(o))
/** Encode a S8 value.
    @param o Required initialized encoder.
    @param v Value representable in S8.
    @return 0 on success, negative encoder/output status on failure.
    Partial output is possible. See the corresponding UBJEncoder method.
 */
#define UBJEncoder_int8(o,v)                                    \
   ((o)->val.t=UBJT_Int8,(o)->val.u.int8=v,UBJEncoder_val(o))
/** Encode a char value.
    @param o Required initialized encoder.
    @param v Value representable in char.
    @return 0 on success, negative encoder/output status on failure.
    Partial output is possible. See the corresponding UBJEncoder method.
 */
#define UBJEncoder_character(o,v)                               \
   ((o)->val.t=UBJT_Char,(o)->val.u.int8=v,UBJEncoder_val(o))
/** Encode a S16 value.
    @param o Required initialized encoder.
    @param v Value representable in S16.
    @return 0 on success, negative encoder/output status on failure.
    Partial output is possible. See the corresponding UBJEncoder method.
 */
#define UBJEncoder_int16(o,v)                                   \
   ((o)->val.t=UBJT_Int16,(o)->val.u.int16=v,UBJEncoder_val(o))
/** Encode a S32 value.
    @param o Required initialized encoder.
    @param v Value representable in S32.
    @return 0 on success, negative encoder/output status on failure.
    Partial output is possible. See the corresponding UBJEncoder method.
 */
#define UBJEncoder_int32(o,v)                                   \
   ((o)->val.t=UBJT_Int32,(o)->val.u.int32=v,UBJEncoder_val(o))
/** Encode a S64 value.
    @param o Required initialized encoder.
    @param v Value representable in S64.
    @return 0 on success, negative encoder/output status on failure.
    Partial output is possible. See the corresponding UBJEncoder method.
 */
#define UBJEncoder_int64(o,v)                                   \
   ((o)->val.t=UBJT_Int64,(o)->val.u.int64=v,UBJEncoder_val(o))
/** Encode a double value.
    @param o Required initialized encoder.
    @param v Value representable in double.
    @return 0 on success, negative encoder/output status on failure.
    Partial output is possible. See the corresponding UBJEncoder method.
 */
#define UBJEncoder_float64(o,v)                                         \
   ((o)->val.t=UBJT_Float64,(o)->val.u.float64=v,UBJEncoder_val(o))
/** Encode a float value.
    @param o Required initialized encoder.
    @param v Value representable in float.
    @return 0 on success, negative encoder/output status on failure.
    Partial output is possible. See the corresponding UBJEncoder method.
 */
#define UBJEncoder_float32(o,v)                                         \
   ((o)->val.t=UBJT_Float32,(o)->val.u.float32=v,UBJEncoder_val(o))
/** Encode string bytes without requiring NUL termination.
    @param o Required initialized encoder.
    @param v Required readable string bytes, borrowed for the call.
    @param l Nonnegative byte count, excluding any optional NUL.
    @return 0 on success, negative encoder/output status on failure.
    Partial output is possible. See the corresponding UBJEncoder method.
 */
#define UBJEncoder_string(o,v,l)                                        \
   ((o)->val.t=UBJT_String,(o)->val.u.string=v,(o)->val.len=l,UBJEncoder_val(o))
/** Begin an array; emit exactly count children when count is nonnegative.
    @param o Required initialized encoder.
    @param count Number of children, or -1 for an uncounted array.
    @param sType Strong element type, or UBJT_InvalidType for mixed types.
    @return 0 on success, negative encoder/output status on failure.
    Partial output is possible. See the corresponding UBJEncoder method.
 */
#define UBJEncoder_beginArray(o,count, sType)           \
   ((o)->val.t=UBJT_BeginArray,(o)->val.len=count,      \
    (o)->val.x=sType,UBJEncoder_val(o))
/** Begin an object; emit exactly count named members when count is nonnegative.
    @param o Required initialized encoder.
    @param count Number of members, or -1 for an uncounted object.
    @param sType Strong member-value type, or UBJT_InvalidType for mixed types.
    @return 0 on success, negative encoder/output status on failure.
    Partial output is possible. See the corresponding UBJEncoder method.
 */
#define UBJEncoder_beginObject(o,count, sType)          \
   ((o)->val.t=UBJT_BeginObject,(o)->val.len=count,     \
    (o)->val.x=sType,UBJEncoder_val(o))
/** End the current array.
    @param o Required initialized encoder.
    
    @return 0 on success, negative encoder/output status on failure.
    Partial output is possible. See the corresponding UBJEncoder method.
 */
#define UBJEncoder_endArray(o) ((o)->val.t=UBJT_EndArray,UBJEncoder_val(o))
/** End the current object.
    @param o Required initialized encoder.
    
    @return 0 on success, negative encoder/output status on failure.
    Partial output is possible. See the corresponding UBJEncoder method.
 */
#define UBJEncoder_endObject(o) ((o)->val.t=UBJT_EndObject,UBJEncoder_val(o))

#ifdef __cplusplus
inline UBJEncoder::UBJEncoder(UBJEBuf* b) {
   UBJEncoder_constructor(this, b);
}
inline UBJEncoder::~UBJEncoder() {
   UBJEncoder_destructor(this);
}
inline int UBJEncoder::setName(const char* n) {
   return UBJEncoder_setName(this, n);
}
inline int UBJEncoder::null() {
   return UBJEncoder_null(this);
}
inline int UBJEncoder::boolean(bool b) {
   return UBJEncoder_boolean(this, b ? 1 : 0);
}
inline int UBJEncoder::uint8(U8 v) {
   return UBJEncoder_uint8(this, v);
}
inline int UBJEncoder::int8(S8 v) {
   return UBJEncoder_int8(this, v);
}
inline int UBJEncoder::character(char v) {
   return UBJEncoder_character(this, v);
}
inline int UBJEncoder::int16(S16 v) {
   return UBJEncoder_int16(this, v);
}
inline int UBJEncoder::int32(S32 v) {
   return UBJEncoder_int32(this, v);
}
inline int UBJEncoder::int64(S64 v) {
   return UBJEncoder_int64(this, v);
}
inline int UBJEncoder::float64(double v) {
   return UBJEncoder_float64(this, v);
}
inline int UBJEncoder::float32(float v) {
   return UBJEncoder_float32(this, v);
}
inline int UBJEncoder::string(const char* s, S32 len) {
   return UBJEncoder_string(this, s, len);
}
inline int UBJEncoder::beginArray(S32 count, UBJT t) {
   return UBJEncoder_beginArray(this, count, t);
}
inline int UBJEncoder::beginObject(S32 count, UBJT t) {
   return UBJEncoder_beginObject(this, count, t);
}
inline int UBJEncoder::endArray() {
   return UBJEncoder_endArray(this);
}
inline int UBJEncoder::endObject() {
   return UBJEncoder_endObject(this);
}
inline void UBJEncoder::reset() {
   UBJEncoder_reset(this);
}
inline int UBJEncoder::vset(const char** fmt, va_list* argList, bool isObj){
   return UBJEncoder_vset(this, fmt, argList, isObj ? 1 : 0);
}
inline int UBJEncoder::set(const char* fmt, ...){
   int status;
   va_list varg;
   va_start(varg, fmt);
   status = UBJEncoder_vset(this, &fmt, &varg, 0);
   va_end(varg);
   return status;
}
#endif

/** @} */ /* end of UBJSONRef */


#endif
