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
 *   $Id: JDecoder.h 5392 2023-02-21 15:56:50Z wini $
 *
 *   COPYRIGHT:  Real Time Logic LLC, 2014
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
*/

#ifndef __jdecode_h
#define __jdecode_h

#include <JVal.h>

/** @addtogroup JSONCB
@{
*/

#ifndef J_ALIGNMT
#define J_ALIGNMT  uintptr_t
#endif
#define J_POINTER_NOT_ALIGNED(p)  (0 != ((uintptr_t)p & (sizeof(J_ALIGNMT)-1)))


/** JDecoder Status
 */
typedef enum {
   /** Parsed data does not match the value tree that was created when
       method JDecoder::get was called
    */
   JDecoderS_FormatErr = -100,

   /** Parsed data includes a member whose name is not in value tree. */
   JDecoderS_NameNotFound,

   /** Received more array or object member values from parsed data than
    * found in value tree.
    */
   JDecoderS_Overflow,

   /** Parsed string longer than buffer provided */
   JDecoderS_StringOverflow,

   /** Incorrect use of '{', '}', '[', or '[' in JDecoder::get */
   JDecoderS_Unbalanced,

   /** Received less array or object member values from parsed data than
    * found in value tree.
    */
   JDecoderS_Underflow,

   /** Unkown error */
   JDecoderS_Unknown,

   /** Memory buffer(s) provided in JDecoder constructor must be word aligned */
   JDecoderS_BufNotAligned,

   /** A chained parser callback, provided via the 'X' format flag in
    * JDecoder::get, reported an error.
    */
   JDecoderS_ChainedErr,

   /** OK, no errors
    */
   JDecoderS_OK = 0
} JDecoderS;


/** Used internally in JDecoder for nested data structures */
typedef struct
{
   U16 contIx; /* container (object or array) index */
   U8 isObj; /* 1 if object, 0 if array */
} JDecoderStackNode;


/** Used internally in JDecoder, where one instance represents one
    node in the pointer value tree.
*/
typedef struct
{
   union
   {
      U8* b;
      S32* d;
      S64* l;
#ifndef NO_DOUBLE
      double* f;
#endif
      char* s;
      struct /* For obj/array */
      {
         U16 firstIx;
         U16 lastIx; /* used via JDecoder_vget only */
      } child; /* if t == container (object or array) */
      JParserIntf* pIntf; /* for 'X' */
   } u;
   const char* name; /* Member name, if parent is an object */
   S32 sSize; /* Size (len+1) of s buffer ( if t == s ) */
   U16 nextIx;
   U8 t; /* JVType */
} JDecoderV;


/** JDecoder implements the parser callback API JParserIntf and builds
    a pointer value tree when JDecoder::get is called. The pointer
    value tree is used by the parser callback when the parser feeds the
    callback with parsed data.

    See the [JSON and UBJSON tutorial](@ref JDecoderUBJDecoder) for more
    information on using JDecoder.
 */
typedef struct JDecoder
#ifdef __cplusplus
: public JParserIntf
{
   /** See JDecoder::get for details */
   int vget(const char* fmt, va_list* argList);

   /** Build a pointer value tree that is used by the integrated
       parser callback function when the parser feeds elements to the
       JDecoder instance.

       \param fmt format flags:
      <table>
      <tr><th>JSON type</th><th>Format flag</th><th>C type</th></tr>
      <tr><td>Number</td><td> d </td><td>S32*</td></tr>
      <tr><td>Number</td><td> l </td><td>S64*</td></tr>
      <tr><td>Number</td><td> f </td><td>double*</td></tr>
      <tr><td>boolean</td><td> b </td><td>BaBool* or U8*</td></tr>
      <tr><td>string</td><td> s </td><td>char**</td></tr>
      <tr><td>Start object</td><td> { </td><td>n/a</td></tr>
      <tr><td>End object</td><td> } </td><td>n/a</td></tr>
      <tr><td>Start array</td><td> [ </td><td>n/a</td></tr>
      <tr><td>End array</td><td> ] </td><td>n/a</td></tr>
      <tr><td>N/A</td><td> X </td><td>JParserIntf*</td></tr>
      </table>

       See the [JSON and UBJSON tutorial](@ref JDecoderUBJDecoder) for more
       information on using this method.

      \sa JD_MNUM
      \sa JD_MSTR
      \sa JD_ASTR
      \sa JEncoder::set
    */
   int get(const char* fmt, ...);

   /** Create/initialize a JDecoder instance.

       \param buf is a pointer to a buffer used internally for memory
       storage when building the pointer value tree. The minimum size
       must be sizeof(JDecoderV) * N, where N is the number of format
       flags minus the end of array/object flags (] or }).

       \param bufSize the size of 'buf'

       \param extraStackLen is a non documented value and must be set to 0.
   */
   JDecoder(U8* buf, int bufSize, int extraStackLen=0);
#if 0
}
#endif
#else
{
      JParserIntf super; /* Inherits from JParserIntf */
#endif
   JDecoderS status;
   JParserIntf* pIntf;
   int startServiceLevel;
   U8* buf;
   int bufIx;
   int bufSize;
   int stacklen;
   JDecoderStackNode stack[JPARSER_STACK_LEN];
} JDecoder;


/** JDecoder::get helper macro, used when setting a number pointer in an object.
    encoder.set("{d}", JD_MNUM(structval, membername));
*/
#define JD_MNUM(o, m) #m, &(o)->m

/** JDecoder::get helper macro, used when setting a string pointer in an object.
    encoder.set("{s}", JD_MSTR(structval, membername));
*/
#define JD_MSTR(o, m) #m, &(o)->m, sizeof((o)->m)

/** JDecoder::get helper macro, used when setting a string pointer in an array.
    encoder.set("[s]", JD_ASTR(structval, membername));
*/
#define JD_ASTR(o, m) &(o)->m, sizeof((o)->m)

#ifdef __cplusplus
extern "C" {
#endif

int JDecoder_vget(JDecoder* o, const char* fmt, va_list* argList);
int JDecoder_get(JDecoder* o, const char* fmt, ...);
void JDecoder_constructor(
   JDecoder* o, U8* buf, int bufSize, int extraStackLen);
#ifdef __cplusplus
}
inline int JDecoder::vget(const char* fmt, va_list* argList){
   return JDecoder_vget(this, fmt,  argList);
}
inline int JDecoder::get(const char* fmt, ...){
   int stat;
   va_list argList;
   va_start(argList, fmt);
   stat = JDecoder_vget(this, fmt, &argList);
   va_end(argList);
   return stat;
}
inline JDecoder::JDecoder(U8* buf, int bufSize, int extraStackLen){
   JDecoder_constructor(this, buf, bufSize, extraStackLen);
}
#endif

/** @} */ /* end of JSONCB */


#endif
