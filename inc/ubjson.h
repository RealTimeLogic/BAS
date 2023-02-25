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
 *   $Id: ubjson.h 5392 2023-02-21 15:56:50Z wini $
 *
 *   COPYRIGHT:  Real Time Logic LLC, 2014 - 2023
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
   UBJT_HNumber='H', /**< Type UBJT_HNumber */
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
       start of a new object in the provided buffer.
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
      S8 int8;   /**< Use when 't' is \ref UBJT_Uint8 */
      char ch;   /**< Use when 't' is  \ref UBJT_Char */
      S16 int16; /**< Use when 't' is \ref UBJT_Int16 */
      S32 int32; /**< Use when 't' is \ref UBJT_Int32 */
      S64 int64; /**< Use when 't' is \ref UBJT_Int64 */
#ifndef NO_DOUBLE
      double float64; /**< Use when 't' is \ref UBJT_Float64 */
      float float32; /**< Use when 't' is \ref UBJT_Float32 */
#endif
      const char* string; /**< Use when 't' is \ref UBJT_String */
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
} UBJPStackNode;


struct UBJPIntf;

/** Parser callback function used by interface class UBJPIntf
    \param o the interface object
    \param v the parsed value
    \param recLevel goes from 0 to N and represents object nesting level
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
       \param s the parser service callback function.
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

    \sa UBJVal
    \sa JParser
 */
typedef struct UBJParser
{
#ifdef __cplusplus
   /** Create the callback interface object.
       \param intf the callback interface object.
       \param name is a buffer used for storing an object member name
       during parsing.
       \param memberNameLen is the length of the object member name
       buffer. The length must be no less than the largest member name
       expected.
       \param extraStackLen informs the parser that it can use a stack
       larger than the default depth of 3. The memory for the
       UBJParser object must be constructed as follows:
       malloc(sizeof(UBJParser) + extraStackLen *
       sizeof(UBJPStackNode))
   */
   UBJParser(UBJPIntf* intf,char* name,int memberNameLen,int extraStackLen=0);

   /** Parse a UBJSON chunk.
       \param buf a pointer to the UBJSON chunk.
       \param size is the buffer length.
       \returns
       \li 0: Needs more data. The UBJSON chunk parsed was only a
       partial object.
       \li > 0: A complete UBJSON object is assembled.
       \li < 0: Parse error.
       \sa getStatus
   */ 
   int parse(const U8* buf, U32 size);

   /** Destructor */
   ~UBJParser();

   /** Returns the current container index position when parsing an
    [optimized container](http://ubjson.org/type-reference/container-types/#optimized-format).
    */
   int getIndex();

   /** Returns the length of the
       [optimized container](http://ubjson.org/type-reference/container-types/#optimized-format)
       when type (t) is \ref UBJT_Count
    */
   int getCount();

   /** Returns the parser status. Typically used when method parse
       returns a value less than zero.
       \sa parse
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
void UBJParser_constructor(UBJParser* o, UBJPIntf* intf, char* name,
                           int memberNameLen, int extraStackLen);
#define UBJParser_destructor(o)
int UBJParser_parse(UBJParser* o, const U8* buf, U32 size);
#define UBJParser_getIndex(o)  (o)->stack[(o)->stackIx].ix
#define UBJParser_getCount(o)  (o)->stack[(o)->stackIx].count
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

/** UBJSON Encoder buffer callback can be used for flushing the
    buffer or for expanding the buffer. The callback must set the
    cursor position to zero if the buffer is flushed.
    \param o UBJEBuf
    \param sizeRequired is the extra size required by the buffer (if
    expanding the buffer).
 */
typedef int (*UBJEBuf_FlushCB)(struct UBJEBuf* o, int sizeRequired);

/** The UBJSON Encoder buffer is used by the encoder UBJEncoder */
typedef struct UBJEBuf
{
#ifdef __cplusplus
   /** Initialize the UBJSON Encoder buffer.
       \param cb the expand or flush callback.
       \param buf the (initial) encoder buffer
       \param bufLen is length of buffer (min 14 bytes)
   */
   UBJEBuf(UBJEBuf_FlushCB cb, U8* buf, S32 bufLen);
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

   /** The value set does not mach type set for Optimized Strongly Typed  */
   UBJEStatus_TypeMismatch,

   /** (curly) bracket start/end mismatch */
   UBJEStatus_Unbalanced,

   /** Unknown type (Must be a type from UBJT) */ 
   UBJEStatus_Unknown,
   
   /** No error */
   UBJEStatus_ok=0
} UBJEStatus;


/** UBJSON Encoder.
    The encoder performs limited error checking and you can produce
    incorrect UBJSON data if you incorrectly use the methods in this
    class.
 */
typedef struct UBJEncoder
{
#ifdef __cplusplus

   /** Create/initialize an UBJEncoder instance.
       \param buf a buffer that either buffers all produced UBJSON
       data or small chunks, which are then flushed out to a stream
       when the buffer is full.
    */
   UBJEncoder(UBJEBuf* buf);

   /** Destructor
    */
   ~UBJEncoder();

   /** Set member name. This function must be called for objects, but
       must not be called for arrays.
    */
   int setName(const char* n);

   /** Set UBJSON NULL */
   int null();

   /** Set boolean */
   int boolean(bool b);

   /** Set uint8 */
   int uint8(U8 v);

   /** Set int8 */
   int int8(S8 v);

   /** Set 'char' */
   int character(char v);

   /** Set int16 */
   int int16(S16 v);

   /** Set int32 */
   int int32(S32 v);

   /** Set int64 */
   int int64(S64 v);

   /** Set float64 */
   int float64(double v);

   /** Set float32 */
   int float32(float v);

   /** Set string */
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
    */
   int beginObject(S32 count=-1, UBJT type=UBJT_InvalidType);

   /** End of array */
   int endArray();

   /** End of object */
   int endObject();

   /** Resets the UBJEBuf cursor (the buffer provided in the constructor) */
   void reset();

   /** See UBJEncoder:set for details */
   int vset(const char** fmt, va_list* argList, bool isObj);

   /** Encode/serialize C structs/data to UBJSON using formatted output.

      The set method works in a similar fashion to the ANSII C
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

#define UBJEncoder_constructor(o, ubjsBuf)      \
   memset(o,0,sizeof(UBJEncoder)),(o)->buf=ubjsBuf
#define UBJEncoder_destructor(o)
#define UBJEncoder_reset(o) ((o)->status=0,(o)->val.name=0,o->buf->cursor=0,0)

#ifdef __cplusplus
extern "C" {
#endif
int UBJEncoder_setStatus(UBJEncoder* o, UBJEStatus s);
int UBJEncoder_val(UBJEncoder* o);
int UBJEncoder_vset(UBJEncoder* o,const char** fmt,va_list* argList,int isObj);
int UBJEncoder_set(UBJEncoder* o, const char* fmt, ...);
#ifdef __cplusplus
}
#endif

#define UBJEncoder_setName(o,v) ((o)->val.name=(char*)v)
#define UBJEncoder_null(o) ((o)->val.t=UBJT_Null,UBJEncoder_val(o))
#define UBJEncoder_boolean(o,v)                                         \
   ((o)->val.t=UBJT_Boolean,(o)->val.u.uint8=v,UBJEncoder_val(o))
#define UBJEncoder_uint8(o,v)                                   \
   ((o)->val.t=UBJT_Uint8,(o)->val.u.uint8=v,UBJEncoder_val(o))
#define UBJEncoder_int8(o,v)                                    \
   ((o)->val.t=UBJT_Int8,(o)->val.u.int8=v,UBJEncoder_val(o))
#define UBJEncoder_character(o,v)                               \
   ((o)->val.t=UBJT_Char,(o)->val.u.int8=v,UBJEncoder_val(o))
#define UBJEncoder_int16(o,v)                                   \
   ((o)->val.t=UBJT_Int16,(o)->val.u.int16=v,UBJEncoder_val(o))
#define UBJEncoder_int32(o,v)                                   \
   ((o)->val.t=UBJT_Int32,(o)->val.u.int32=v,UBJEncoder_val(o))
#define UBJEncoder_int64(o,v)                                   \
   ((o)->val.t=UBJT_Int64,(o)->val.u.int64=v,UBJEncoder_val(o))
#define UBJEncoder_float64(o,v)                                         \
   ((o)->val.t=UBJT_Float64,(o)->val.u.float64=v,UBJEncoder_val(o))
#define UBJEncoder_float32(o,v)                                         \
   ((o)->val.t=UBJT_Float32,(o)->val.u.float32=v,UBJEncoder_val(o))
#define UBJEncoder_string(o,v,l)                                        \
   ((o)->val.t=UBJT_String,(o)->val.u.string=v,(o)->val.len=l,UBJEncoder_val(o))
#define UBJEncoder_beginArray(o,count, sType)           \
   ((o)->val.t=UBJT_BeginArray,(o)->val.len=count,      \
    (o)->val.x=sType,UBJEncoder_val(o))
#define UBJEncoder_beginObject(o,count, sType)          \
   ((o)->val.t=UBJT_BeginObject,(o)->val.len=count,     \
    (o)->val.x=sType,UBJEncoder_val(o))
#define UBJEncoder_endArray(o) ((o)->val.t=UBJT_EndArray,UBJEncoder_val(o))
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
