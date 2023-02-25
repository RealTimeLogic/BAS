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
 *   $Id: ubjsonex.h 5392 2023-02-21 15:56:50Z wini $
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

 Universal Binary JSON http://ubjson.org/
 Extended API: UBJDecoder and UBJEncoder_set
*/

#ifndef __ubjsonex_h
#define __ubjsonex_h

#include "ubjson.h"

/** @addtogroup UBJSONRef
@{
*/

/** UBJDecoder Status Codes
 */
typedef enum {

   /** Parsed data does not match the value tree that was created when
       method UBJDecoder::get was called
    */
   UBJDecoderS_FormatErr = -100,

   /** Parsed data includes a member whose name is not in value tree. */
   UBJDecoderS_NameNotFound,

   /** Received more array or object member values from parsed data than
    * found in value tree.
    */
   UBJDecoderS_Overflow,

   /** Parsed string longer than buffer provided */
   UBJDecoderS_StringOverflow,

   /** Incorrect use of '{', '}', '[', or '[' in JDecoder::get */
   UBJDecoderS_Unbalanced,

   /** Received less array or object member values from parsed data than
    * found in value tree.
    */
   UBJDecoderS_Underflow,

   /** Unkown error */
   UBJDecoderS_Unknown,

   /** Memory buffer(s) provided in UBJDecoder constructor must be
       word aligned
   */
   UBJDecoderS_BufNotAligned,

   /** A chained parser callback, provided via the 'X' format flag in
    * JDecoder::get, reported an error.
    */
   UBJDecoderS_ChainedErr,

   /** OK, no errors
    */
   UBJDecoderS_OK = 0
} UBJDecoderS;


/** Used internally in UBJDecoder for nested data structures */
typedef struct
{
   U16 contIx; /* container (object or array) index */
   U8 isObj; /* 1 if object, 0 if array */
} UBJDecoderStackNode;


/** Used internally in UBJDecoder, where one instance represents one
    node in the pointer value tree.
*/
typedef struct
{
   union
   {
      U8* uint8;
      S8* int8;
      S16* int16;
      S32* int32;
      S64* int64;
#ifndef NO_DOUBLE
      double* float64;
      float* float32;
#endif
      char* string;
      struct /* For obj/array */
      {
         U16 firstIx;
         U16 lastIx; /* used via UBJDecoder_vget only */
      } child; /* if t == container (object or array) */
      UBJPIntf* ubjpintf; /* for 'X' */
   } u;
   const char* name; /* Member name, if parent is an object */
   S32 sSize; /* Size (len+1) of string buffer ( if t == string ) */
   U16 nextIx;
   U8 t; /* UBJT */
} UBJDecoderV;



/** UBJDecoder implements the parser callback API UBJPIntf and builds
    a pointer value tree when UBJDecoder::get is called. The pointer
    value tree is used by the parser callback when the parser feeds the
    callback with parsed data.

    See the [JSON and UBJSON tutorial](@ref JDecoderUBJDecoder) for more
    information on using UBJDecoder.
 */
typedef struct UBJDecoder
#ifdef __cplusplus
: public UBJPIntf
{
   /** See UBJDecoder::get for details */
   int vget(const char* fmt, va_list* argList);

   /** Build a pointer value tree that is used by the integrated
       parser callback function when the parser feeds elements to the
       JDecoder instance.

       \param fmt format flags:
       <table>
       <tr><th>UBJSON type</th><th>Format flag</th><th>C type</th></tr>
       <tr><td>bool</td><td>b</td><td>U8*</td></tr>
       <tr><td>int8</td><td>i</td><td>S8*</td></tr>
       <tr><td>uint8</td><td>U</td><td>U8*</td></tr>
       <tr><td>int16</td><td>I</td><td>S16*</td></tr>
       <tr><td>int32</td><td>l</td><td>S32*</td></tr>
       <tr><td>int64</td><td>L</td><td>S64*</td></tr>
       <tr><td>float32</td><td>d</td><td>float*</td></tr>
       <tr><td>float64</td><td>D</td><td>double*</td></tr>
       <tr><td>char</td><td>C</td><td>S8</td></tr>
       <tr><td>string</td><td>S</td><td>char**</td></tr>
       </table>

       See the [JSON and UBJSON tutorial](@ref JDecoderUBJDecoder) for more
       information on using this method.

      \sa UBJD_MNUM
      \sa UBJD_MSTR
      \sa UBJD_ASTR
      \sa UBJEncoder::set
    */
   int get(const char* fmt, ...);

   /** Create/initialize a UBJDecoder instance.

       \param buf is a pointer to a buffer used internally for memory
       storage when building the pointer value tree. The minimum size
       must be sizeof(UBJDecoderV) * N, where N is the number of format
       flags minus the end of array/object flags (] or }).

       \param bufSize the size of 'buf'

       \param extraStackLen is a non documented value and must be set to 0.
   */
   UBJDecoder(U8* buf, int bufSize, int extraStackLen=0);
#if 0
}
#endif
#else
{
      UBJPIntf super; /* Inherits from UBJPIntf */
#endif
   UBJDecoderS status;
   UBJPIntf* ubjpintf;
   int startServiceLevel;
   char* buildStr;
   U8* buf;
   int buildStrLen;
   int bufIx;
   int bufSize;
   int stacklen;
   UBJDecoderStackNode stack[UBJPARS_STACK_LEN];
} UBJDecoder;

/** JDecoder::get helper macro, used when setting a number pointer in an object.
    encoder.set("{d}", UBJD_MNUM(structval, membername));
*/
#define UBJD_MNUM(o, m) #m, &(o)->m

/** JDecoder::get helper macro, used when setting a string pointer in an object.
    encoder.set("{s}", UBJD_MSTR(structval, membername));
*/
#define UBJD_MSTR(o, m) #m, &(o)->m, sizeof((o)->m)

/** JDecoder::get helper macro, used when setting a string pointer in an array.
    encoder.set("[s]", UBJD_ASTR(structval, membername));
*/
#define UBJD_ASTR(o, m) &(o)->m, sizeof((o)->m)


#ifdef __cplusplus
extern "C" {
#endif

int UBJDecoder_vget(UBJDecoder* o, const char* fmt, va_list* argList);
int UBJDecoder_get(UBJDecoder* o, const char* fmt, ...);
void UBJDecoder_constructor(
   UBJDecoder* o, U8* buf, int bufSize, int extraStackLen);
#ifdef __cplusplus
}
inline int UBJDecoder::vget(const char* fmt, va_list* argList){
   return UBJDecoder_vget(this, fmt,  argList);
}
inline int UBJDecoder::get(const char* fmt, ...){
   int status;
   va_list argList;
   va_start(argList, fmt);
   status = UBJDecoder_vget(this, fmt, &argList);
   va_end(argList);
   return status;
}
inline UBJDecoder::UBJDecoder(U8* buf, int bufSize, int extraStackLen){
   UBJDecoder_constructor(this, buf, bufSize, extraStackLen);
}
#endif

/** @} */ /* end of UBJSONRef */


#endif
