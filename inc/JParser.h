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
 *   $Id: JParser.h 4915 2021-12-01 18:26:55Z wini $
 *
 *   COPYRIGHT:  Real Time Logic LLC, 2006-2018
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

#ifndef __JParser_h
#define __JParser_h


#include <BaAtoi.h>
#include <AllocatorIntf.h>

/** @addtogroup JSONCB
@{
*/

struct JParserIntf;
struct JParserVal;

/** The parser callback function.
    \param o, the interface object
    \param v, the parsed value
    \param recLevel goes from 0 to N and represents object nesting level
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
       \param s the parser service callback function.
   */
   JParserIntf(JParserIntf_Service s) { service = s; }

   /** Calling the JParserIntf service function is typically performed
       in a design using a chained parser callback interface.
       \param v the parsed value provided by JParser
       \param recLevel must be adjusted by the chained callback
   */
   int serviceCB(JParserVal* v, int recLevel);

   ~JParserIntf(){}
   JParserIntf() {}
#endif
      JParserIntf_Service service;
} JParserIntf;

#define JParserIntf_constructor(o,serviceMA) (o)->service=serviceMA

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
      /** Create a new object. */
      JErr();
      /** Reset -- remove any error so the object can be reused. */
      void reset();
      /** Returns true if the error flag is set. */
      bool isError();
      /** Returns true if no error. */
      bool noError();
      /** Returns the error type. */
      JErrT getErrT();
      /** Returns the expected type if this is a parse error.
          \sa getErrT getRecT
       */
      JVType getExpT();
      /** Returns the parsed type if this is a parse error.
          \sa getErrT getExpT
       */
      JVType getRecT();
      int setTooFewParams();
      int setTypeErr(JVType expT, JVType recT);
      int setError(JErrT err,const char* msg);
#endif
      const char* msg;
      JErrT err;
      JVType expType;
      JVType recType;
} JErr;
#ifdef __cplusplus
extern "C" {
#endif
#define JErr_constructor(o) (o)->err=JErrT_NoErr
#define JErr_reset(o) (o)->err=JErrT_NoErr, (o)->msg=0
#define JErr_isError(o) (o)->err!=JErrT_NoErr
#define JErr_noError(o) (o)->err==JErrT_NoErr
#define JErr_getErrT(o) (o)->err
#define JErr_getExpT(o) (o)->expType
#define JErr_getRecT(o) (o)->recType
BA_API int JErr_setTooFewParams(JErr* o);
BA_API int JErr_setTypeErr(JErr* o, JVType expT, JVType recT);
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
inline int JErr::setError(JErrT err,const char* msg) {
   return JErr_setError(this,err,msg);}
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
   JParserT_Long = 'l',   /**< 64 bit (long long) */
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
      char* s; /**< If string */
#ifndef NO_DOUBLE
      double f; /**< If number of type double */
#endif
      S32 d; /**< If integer */
      U64 l; /**< If long integer */
      BaBool b;
   } v;

   /** object member name is set for objects. Use the following
       construction to differentiate between an object/array:
       \code
       if(*val->memberName)
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
   JParserT t; /**< The type controlling 'v' */
} JParserVal;


/** JSON Parser Status.
 */
typedef enum {

   /** The parser completed parsing a new JSON object.
    */
   JParsStat_DoneEOS=1,

   /** The parser completed parsing a new JSON object, but found the
       start of a new object in the provided buffer.
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

       \param intf is the parser callback/interface. Provide your own
       callback or use one of JParserValFact or JDecoder.

       \param nameBuf is a buffer required for temporarily storing
       parsed object member names.

       \param namebufSize sizeof(nameBuf)

       \param alloc is the allocator used for internal buffer management.

       \param extraStackLen is a non documented feature and must be set to 0.
   */
   JParser(JParserIntf* intf, char* nameBuf, int namebufSize,
           AllocatorIntf* alloc, int extraStackLen=0);

   /** Parse a JSON text chunk.
       \param buf a pointer to the JSON text chunk buffer.
       \param size is the buffer length.
       \returns
       \li 0: Needs more data. The JSON chunk parsed was only a
       partial object.
       \li > 0: A complete JSON object is assembled.
       \li < 0: Parse error.
       \sa getStatus
   */
   int parse(const U8* buf, U32 size);

   /** Terminate and release the internal buffers. */
   ~JParser();

   /** Returns the parser status. Typically used when method parse
       returns a non zero value.
       \sa parse
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
BA_API void JParser_constructor(JParser* o, JParserIntf* intf, char* nameBuf,
                                int namebufSize, AllocatorIntf* alloc,
                                int extraStackLen);
BA_API int JParser_parse(JParser* o, const U8* buf, U32 size);
BA_API void JParser_destructor(JParser* o);
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
