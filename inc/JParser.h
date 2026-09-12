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
 *   $Id: JParser.h 5978 2026-09-11 16:13:48Z wini $
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

/** @file JParser.h */

#ifndef __JParser_h
#define __JParser_h


#include <BaAtoi.h>
#include <AllocatorIntf.h>

/** @addtogroup JSONCB
@{
*/

struct JParserIntf;
struct JParserVal;

/** Receive one parsed value or container boundary synchronously.
    @param o Required application callback interface.
    @param v Borrowed event record. Its strings/member name and lengths are valid
    only during this call; copy anything needed later. Do not retain v or free
    the parser from its callback.
    @param recLevel Container depth: top-level begin/end events use zero,
    their immediate primitive children use one, and nested levels increase it.
    @return Zero to continue, nonzero to stop parsing with JParsStat_IntfErr.
    The callback's numeric error is not returned unchanged by JParser::parse.
 */
typedef int (*JParserIntf_Service)(
   struct JParserIntf* o, struct JParserVal* v, int recLevel);


/** The JParserIntf interface class is the interface between the parser and
    an object that implements the JParserIntf interface.
 */
typedef struct JParserIntf
{
#ifdef __cplusplus
   /** Create the callback interface object.
       \param s Required callback, callable while the interface is installed.
   */
   JParserIntf(JParserIntf_Service s) { service = s; }

   /** Calling the JParserIntf service function is typically performed
       in a design using a chained parser callback interface.
       \param v the parsed value provided by JParser
       \param recLevel Nonnegative nesting level adjusted for the chained callback.
       @return The callback result unchanged; zero continues, nonzero reports failure.
   */
   int serviceCB(JParserVal* v, int recLevel);

   ~JParserIntf(){}
   JParserIntf() {}
#endif
      JParserIntf_Service service;
} JParserIntf;

/** Initialize the callback interface.
    @param o Required storage.
    @param serviceMA Required JParserIntf_Service callback.
 */
#define JParserIntf_constructor(o,serviceMA) (o)->service=serviceMA

/** Invoke the callback synchronously.
    @param o Required initialized interface.
    @param v Required borrowed event.
    @param recLev Nesting depth relative to this callback.
    @return Callback result unchanged; zero continues, nonzero stops.
 */
#define JParserIntf_serviceCB(o, v, recLev) (o)->service(o,v,recLev)

#ifdef __cplusplus

inline int JParserIntf::serviceCB(JParserVal* v, int recLevel) {
   return JParserIntf_serviceCB(this,v, recLevel); }
#endif

/** @} */ /* end of JSONCB */

/** @addtogroup JSONRef
@{
*/

/** The JSON types.
 */
typedef enum {
   /** Invalid JSON type.
    */
   JVType_InvalidType,
   /** JSON string.
    */
   JVType_String,
   /** JSON Number encoded as a float.
    */
   JVType_Double,
   /** JSON Number encoded as an integer.
    */
   JVType_Int,
   /** JSON Number encoded as a long integer.
    */
   JVType_Long,
   /** JSON boolean. */
   JVType_Boolean,
   /** JSON NULL value. */
   JVType_Null,
   /** The JVAL is of type JSON object. */
   JVType_Object,
   /** The JVAL is of type JSON array. */
   JVType_Array
} JVType;

/** JSON error codes
 */
typedef enum
{
   /** No error. */
   JErrT_NoErr=0,
   /** Generic error. */
   JErrT_JsonErr,
   /** Unexpected JSON type when parsing.
       \sa JErr::getExpT JErr::getRecT
    */
   JErrT_WrongType,
   /** Invalid RPC method parameters. */
   JErrT_InvalidMethodParams,
   /** A format error in JEncoder or JVal::get
    */
   JErrT_FmtValErr,
   /** Memory allocation error. */
   JErrT_MemErr,
   /** I/O error. */
   JErrT_IOErr
} JErrT;

/** The JSON error container object
 */
typedef struct JErr
{
#ifdef __cplusplus
      /** Initialize only the error code to JErrT_NoErr. Other fields are not
    initialized; use reset() if a cleared message pointer is needed.
 */
      JErr();
      /** Clear the error code and message pointer so the object can be reused.
    Expected/received type fields are not reset.
 */
      void reset();
      /** @return True if err is not JErrT_NoErr, false otherwise.
 */
      bool isError();
      /** @return True if err is JErrT_NoErr, false otherwise.
 */
      bool noError();
      /** @return Stored error type; JErrT_NoErr means no recorded error.
 */
      JErrT getErrT();
      /** @return Expected JVType, meaningful only after a successful setTypeErr().
 */
      JVType getExpT();
      /** @return Received JVType, meaningful only after a successful setTypeErr().
 */
      JVType getRecT();
/** Record JErrT_InvalidMethodParams with a static message if no error exists.
    @return Zero if recorded, -1 if an earlier error is preserved.
 */
      int setTooFewParams();
/** Record JErrT_WrongType only when no earlier error exists.
    @param expT Expected JVType.
    @param recT Received JVType.
    @return Zero if recorded, -1 if an earlier error is preserved.
 */
      int setTypeErr(JVType expT, JVType recT);
/** Record an error only when no earlier error exists.
    @param err Error type to store.
    @param msg Borrowed message pointer, or NULL. Retain its storage while the error is used.
    @return Zero if recorded, -1 if an earlier error is preserved.
 */
      int setError(JErrT err,const char* msg);
#endif
      /** Borrowed message for the recorded error; constructor alone does not initialize it. */
      const char* msg;
      JErrT err;
      JVType expType;
      JVType recType;
} JErr;
#ifdef __cplusplus
extern "C" {
#endif
/** @copydoc JErr::JErr
    @param o Required storage to initialize.
 */
#define JErr_constructor(o) (o)->err=JErrT_NoErr
/** @copydoc JErr::reset
    @param o Required initialized error object.
 */
#define JErr_reset(o) (o)->err=JErrT_NoErr, (o)->msg=0
/** @copydoc JErr::isError
    @param o Required initialized error object.
 */
#define JErr_isError(o) ((o)->err!=JErrT_NoErr)
/** @copydoc JErr::noError
    @param o Required initialized error object.
 */
#define JErr_noError(o) ((o)->err==JErrT_NoErr)
/** @copydoc JErr::getErrT
    @param o Required initialized error object.
 */
#define JErr_getErrT(o) (o)->err
/** @copydoc JErr::getExpT
    @param o Required initialized error object.
 */
#define JErr_getExpT(o) (o)->expType
/** @copydoc JErr::getRecT
    @param o Required initialized error object.
 */
#define JErr_getRecT(o) (o)->recType
/** @copydoc JErr::setTooFewParams
    @param o Required initialized error object.
 */
BA_API int JErr_setTooFewParams(JErr* o);
/** @copydoc JErr::setTypeErr
    @param o Initialized error object, or NULL to return -1.
 */
BA_API int JErr_setTypeErr(JErr* o, JVType expT, JVType recT);
/** @copydoc JErr::setError
    @param o Required initialized error object.
 */
BA_API int JErr_setError(JErr* o,JErrT err,const char* msg);
#ifdef __cplusplus
}
inline JErr::JErr() {
   JErr_constructor(this); }
inline void JErr::reset() {
   JErr_reset(this); }
inline bool JErr::isError() {
   return JErr_isError(this) ? true : false;}
inline bool JErr::noError() {
   return JErr_noError(this) ? true : false;}
inline JErrT JErr::getErrT() {
   return JErr_getErrT(this);}
inline JVType JErr::getExpT() {
   return JErr_getExpT(this);}
inline JVType JErr::getRecT() {
   return JErr_getRecT(this);}
inline int JErr::setTooFewParams() {
   return JErr_setTooFewParams(this);}
inline int JErr::setTypeErr(JVType expT, JVType recT) {
   return JErr_setTypeErr(this, expT, recT);}
inline int JErr::setError(JErrT e,const char* message) {
   return JErr_setError(this,e, message);}
#endif



#ifndef __DOXYGEN__
typedef struct
{
      AllocatorIntf* alloc;
      U8* buf;
      U32 index;
      size_t size;
} JDBuf;


/* JLextT: JSON Lexer Types.
   The following types are used by the lexer and parser.
*/
typedef enum {
   JLexerT_Null,
   JLexerT_Boolean,
   JLexerT_Number,
   JLexerT_String,
   JLexerT_BeginObject,
   JLexerT_BeginArray,
   JLexerT_EndObject,
   JLexerT_EndArray,
   JLexerT_Comma, /* ',' Array or object list comma. */
   JLexerT_MemberSep, /* ':'  for string : value */
   JLexerT_NeedMoreData, /* Lexer not completed with current token. */
   JLexerT_ParseErr,
   JLexerT_MemErr
} JLexerT;



typedef enum {
   JLexerSt_StartComment,
   JLexerSt_EatComment,
   JLexerSt_EndComment,
   JLexerSt_EatCppComment,
   JLexerSt_TrueFalseNull,
   JLexerSt_String,
   JLexerSt_StringEscape,
   JLexerSt_StringUnicode,
   JLexerSt_StringSurrogateEscape,
   JLexerSt_StringSurrogateU,
   JLexerSt_StringUtf8,
   JLexerSt_Number,
   JLexerSt_GetNextToken
} JLexerSt;


typedef struct
{
      JDBuf* asmB;
      const U8* bufStart;
      const U8* bufEnd;
      const U8* tokenPtr;

      U32 unicode;
      S16 unicodeShift;
      U16 surrogate;
      U8 utf8Len;

      /* typeChkPtr and retVal is used if the Lexer finds the start of
         true, false, or null.
      */
      const U8* typeChkPtr;
      U8 retVal;

      U8 state; /* JLexerSt */
      
       /* state for string or number.
          If in state number, 0 means positive number and 255 means neg.
          If in state String, sn is ' or ".
          If in state Boolean: true or false.
       */
      U8 sn;
      U8 isDouble; /* Used when reading a number */
} JLexer;

#endif /* __DOXYGEN__ */

/** Type 't' in JParserVal
 */
typedef enum {
   JParserT_InvalidType=0, /**< InvalidType */
   JParserT_Null, /**< Null */
   JParserT_String = 's', /**< String */
   JParserT_Double = 'f', /**< Double */
   JParserT_Int = 'd', /**< Int */
   JParserT_Long = 'l',   /**< 64-bit (long long) */
   JParserT_Boolean = 'b', /**< Boolean */
   JParserT_BeginObject = '{', /**< BeginObject */
   JParserT_BeginArray = '[', /**< BeginArray */
   JParserT_EndObject = '}', /**< EndObject */
   JParserT_EndArray = ']' /**< EndArray */
} JParserT;

/** The parser sets a JParserVal before calling the parser callback
 * JParserIntf.
*/
typedef struct JParserVal
{
   /** A union controlled by the type 't'
    */
   union
   {
      char* s; /**< Borrowed UTF-8 bytes for String; use stringLen to preserve embedded NUL. */
#ifndef NO_DOUBLE
      double f; /**< If number of type double */
#endif
      S32 d; /**< If integer */
      U64 l; /**< Long integer bit pattern; signed JSON long values are interpreted as S64 by consumers. */
      BaBool b;
   } v;

   /** object member name is set for objects. Use the following
       construction to differentiate between an object/array:
       \code
       if(val->memberNameSet)
       {
          // object
       }
       else
       {
          // array
       }

       \endcode
   */
   char* memberName;
   size_t stringLen; /**< String length when t is JParserT_String. */
   size_t memberNameLen; /**< Object member-name length. */
   BaBool memberNameSet; /**< TRUE when this event carries an object member name, including an empty name. FALSE also applies to container-end/top-level events. */
   JParserT t; /**< The type controlling 'v' */
} JParserVal;


/** JSON Parser Status.
 */
typedef enum {

   /** The parser completed parsing a new JSON object.
    */
   JParsStat_DoneEOS=1,

   /** The parser completed parsing a new JSON object, but found the
       unread bytes in the provided buffer. They have not yet been validated
       as another object or array.
    */
   JParsStat_Done,


    /** The parser needs more data to complete. */
   JParsStat_NeedMoreData = 100,

   /** JSON Parse error
    */
   JParsStat_ParseErr = 200,

   /** The JParserIntf interface reported a problem.
    */
   JParsStat_IntfErr,

   /** Memory allocation error.
    */
   JParsStat_MemErr,

   /** Object nested too deep. Increase stack size in constructor */
   JParsStat_StackOverflow
} JParsStat;


typedef enum {
   JParserSt_StartObj,
   JParserSt_BeginArray,
   JParserSt_MemberName,
   JParserSt_MemberSep,
   JParserSt_Value,
   JParserSt_EndObj,
   JParserSt_Comma
} JParserSt;


#define JPARSER_STACK_LEN 8

/** The stack used internally by JParser */
typedef U8 JParserStackNode;

/** The JSON parser parses a JSON stream and calls the JParserIntf
    callback interface for each parsed object/primitive type.

    \sa UBJParser
 */
struct JParser
{
#ifdef __cplusplus
   /** Create a JSON parser object.

       \param intf Required borrowed interface with a non-NULL callback. Provide your own
       callback or use one of JParserValFact or JDecoder.

       \param nameBuf is a buffer required for temporarily storing
       parsed object member names. Required writable storage, retained for the
       parser lifetime; it must fit the longest name plus its terminator.

       \param namebufSize Positive capacity of nameBuf in bytes. This buffer does
       not grow; an oversized member name reports JParsStat_MemErr.

       \param alloc Borrowed allocator for string/number assembly, retained until
       destruction. Supply an allocation/reallocation/free-capable allocator for
       general input; NULL disables dynamic assembly rather than choosing a default.

       \param extraStackLen is an undocumented feature and must be set to 0.
   */
   JParser(JParserIntf* intf, char* nameBuf, int namebufSize,
           AllocatorIntf* alloc, int extraStackLen=0);

   /** Feed or resume parsing a top-level JSON object or array.
    @param buf Borrowed input chunk. Supply a new chunk after DoneEOS or
    NeedMoreData. After Done, the parser retains unread bytes from the previous
    chunk and ignores these new arguments; call again before replacing that buffer.
    @param size Byte length of a new chunk, excluding any terminator. Retain its
    storage until the parser has consumed it. No NUL termination is needed.
    @return 0 means more input is needed, 1 means one complete object/array was
    delivered, -1 means parsing, callback, allocation, or nesting failure.
    Done indicates unread bytes remain; DoneEOS indicates the chunk was exhausted.
    Neither proves that an entire external input stream contains just one value.
    At actual end of input, NeedMoreData means the document is incomplete.
    After failure, destroy and reinitialize before reuse; callbacks may already
    have changed application data. Top-level scalar values are not supported.
    @note This parser also accepts extensions such as comments and single-quoted
    strings. It is not a strict JSON conformance validator.
 */
   int parse(const U8* buf, U32 size);

   /** Free internal assembly storage. The callback interface, nameBuf, allocator,
       and input chunks are borrowed and are not freed. Stop parsing first. */
   ~JParser();

   /** Inspect the last parser result without changing state.
    @return JParsStat value; initially DoneEOS, meaning ready for the first chunk.
    Done/DoneEOS accompany parse()=1, NeedMoreData accompanies 0, and error states
    accompany -1. See parse() for input-buffer lifetime.
 */
   JParsStat getStatus();
#endif
   JLexer lexer;
   JParserVal val;
   JDBuf asmB; /* Assembling various values */
   JDBuf mnameB; /* Assembling object member names */
   JParserIntf* intf;
   S16 stackIx;
   S16 stackSize;
   U8 status; /* JParsStat */
   U8 state; /* JParserSt */
   /* It's possible to extend the stack size by reserving
    * N*JParserStackNode bytes immediately following the memory for
    * this struct instance. N is then used as 'extraStackLen' in constructor.
    */
   JParserStackNode stack[JPARSER_STACK_LEN];
};

typedef struct JParser JParser;

#ifdef __cplusplus
extern "C" {
#endif
/** @copydoc JParser::JParser
    @param o Required storage to initialize.
 */
BA_API void JParser_constructor(JParser* o, JParserIntf* intf, char* nameBuf,
                                int namebufSize, AllocatorIntf* alloc,
                                int extraStackLen);
/** @copydoc JParser::parse
    @param o Required initialized parser.
 */
BA_API int JParser_parse(JParser* o, const U8* buf, U32 size);
/** @copydoc JParser::~JParser
    @param o Required initialized parser.
 */
BA_API void JParser_destructor(JParser* o);
/** @copydoc JParser::getStatus
    @param o Required initialized parser.
 */
#define JParser_getStatus(o) ((JParsStat)(o)->status)
#ifdef __cplusplus
}
inline JParser::JParser(JParserIntf* intf, char* nameBuf,
                        int namebufSize, AllocatorIntf* alloc,
                        int extraStackLen) {
   JParser_constructor(this, intf, nameBuf, namebufSize, alloc, extraStackLen);
}
inline int JParser::parse(const U8* buf, U32 size) {
   return JParser_parse(this, buf, size);}
inline JParser::~JParser() {
   JParser_destructor(this);}
inline JParsStat JParser::getStatus() {
   return JParser_getStatus(this);}
#endif

/** @} */ /* end of JSONRef */ 

#endif
