/*
 *     ____             _________                __                _     
 *    / __ \___  ____ _/ /_  __(_)___ ___  ___  / /   ____  ____ _(_)____
 *   / /_/ / _ \/ __ `/ / / / / / __ `__ \/ _ \/ /   / __ \/ __ `/ / ___/
 *  / _, _/  __/ /_/ / / / / / / / / / / /  __/ /___/ /_/ / /_/ / / /__  
 * /_/ |_|\___/\__,_/_/ /_/ /_/_/ /_/ /_/\___/_____/\____/\__, /_/\___/  
 *                                                       /____/          
 *
 *                  Barracuda Embedded Web-Server
 *
 ****************************************************************************
 *			      HEADER
 *
 *   $Id: BaServerLib.h 5355 2022-11-16 10:33:45Z wini $
 *
 *   COPYRIGHT:  Real Time Logic LLC, 2002 - 2021
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
 *
 */


#ifndef __httpServerLib_h
#define __httpServerLib_h

#include <TargConfig.h>
#include <AllocatorIntf.h>
#include <ctype.h>
#include  <stddef.h>


#ifdef __cplusplus
extern "C" {
#endif


/** @defgroup UtilityFunctions Miscellaneous library functions
    This header file contains functions that are used by the web-server.
    The functions might also be useful for the code you design.
    Some of the functions are replacements for functions in the 
    <a href="../clib.html">
    C Standard Library</a>. 

    \sa <a href="../../index.html" _target="top"> Barracuda Introduction </a>

   @{
 */
 

#ifdef INL_baConvBin2Hex

extern const char baBin2HexTable[]; /* =
      {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
      'a', 'b', 'c', 'd', 'e', 'f'}; */

#define baConvBin2Hex(hexOutData, binIn) \
      hexOutData[0] = baBin2HexTable[((U8)binIn) >> 4], \
      hexOutData[1] = baBin2HexTable[((U8)binIn) & 0x0f]

#else

BA_API void baConvBin2Hex(void* hexOutData, U8 binIn);

#endif


BA_API U8 baConvHex2Bin( U8 c );
BA_API void baConvU32ToHex(void* to, U32 from);
BA_API U32 baConvHexToU32(const void* from);


/** Calls baMalloc to allocate storage space for a copy of str and
    then copies src to the allocated space.
 */
BA_API char* baStrdup(const char* str);

/** Returns a pointer to an occurrence of key in the array pointed to by base.
    If key is not found, the function returns NULL. If the array is not in
    ascending sort order or contains duplicate records with identical keys,
    the result is unpredictable.
*/
BA_API const void* baBSearch(const void* key, const void* base, int num, 
                             int size, int (*cmp)(const void*,const void*));

/** Compare two strings, ignoring case.
 */
BA_API int baStrCaseCmp(const char *a, const char *b);

/** Compare two strings with max length 'len', ignoring case.
 */
BA_API int baStrnCaseCmp(const char *a, const char *b, size_t len);

const char* baGetToken(const char** str, const char* set);

/** Parses the date string 'str' and returns the time with epoch of
    1970-01-01 00:00:10 GMT
    \sa BaTime
*/
BA_API BaTime baParseDate(const char* str);


/** Decodes a B64 encoded string.
    outStr and b64EncStr can be the same.
 */
BA_API int baB64Decode(
   unsigned char* outStr,int outStrSize,const char* b64EncStr);

/** Fix a path with ../ in it
 */
BA_API int baElideDotDot(char* str);

BA_API void baXmlUnescape(char* f);

#if defined(_WIN32) && !defined(NDEBUG)
/* Bug in VC++ */
#undef bIsspace
#define bIsspace(c)  (c ==  ' ' || c == '\f' || c == '\n' || c == '\r' ||\
                                c == '\t' || c == '\v')
#endif

#define httpEatWhiteSpace(ptr) while(bIsspace((U8)*ptr)) (ptr)++
#define httpEatNonWhiteSpace(ptr) while(!bIsspace((U8)*ptr)) (ptr)++
#define httpEatCharacters(ptr, delimiter) \
   while(*ptr && *ptr != delimiter) (ptr)++

/** Represents the components of calendar time.
    \sa baTime2tm
    \sa baTm2Time
    \sa baParseDate
 */
struct BaTm
{
   /** seconds after the minute [0,59] */
   int       tm_sec;
   /** minutes after the hour [0,59] */
   int       tm_min;
   /** hours since midnight [0,23] */
   int       tm_hour;
   /** day of the month [0,31] */
   int       tm_mday;
   /** months since January [0,11] */
   int       tm_mon;
   /** Years since 0000 (baTime2tmEx) or since 1900 (baTime2tm) */
   int       tm_year;
   /** days since Sunday [0,6] */
   int       tm_wday;
   /** days since January 1 */
   int       tm_yday;

   /** Nano seconds */
   int       nsec;
   /** Time zone offset in minutes */
   int       offset;
};

BA_API U8 baDaysInMonth(U16 y, U16 m);

/** Convert BaTime to a BaTm structure.
    \sa BaTm
    \sa BaTime
 */
BA_API int baTime2tm(struct BaTm *tmP, BaTime t);
BA_API int baTime2tmEx(const BaTimeEx* tex, const BaBool local, struct BaTm* tm);


/** Convert a BaTm structure to BaTime.
    \sa BaTm
    \sa BaTime
 */
BA_API BaTime baTm2Time(struct BaTm* tmP);
BA_API int baTm2TimeEx(struct BaTm* tm, BaBool local, BaTimeEx* tex);

/** Parse ISO860 time */
BA_API int baISO8601ToTime(const char *str, size_t len, BaTimeEx *tex);


/** Return ISO860 encoded string */
BA_API int baTime2ISO8601(const BaTimeEx* tex, char* str, size_t len);


/** @} */ /* end of UtilityFunctions */


#ifdef __cplusplus
}
#endif


/** @defgroup DynamicMemory Dynamic memory handling.

\sa <a href="../../index.html" _target="top"> Barracuda Introduction </a>

   @{
 */
#ifdef __DOXYGEN__
/* For documentation only */

/** Returns pointer to uninitialized newly-allocated space for
    an object of size "size", or NULL on error.
*/
void* baMalloc(size_t size);

/** Returns pointer to newly-allocated space for an object of
    size "size", initialized, to minimum of old and new sizes, to
    existing contents of p (if non-null), or NULL on error. On
    success, old object deallocated; otherwise unchanged.
*/
void* baRealloc(void* p, size_t size);


/** Deallocates space to which it points.
 */
void baFree(void* p);

#endif /*  __DOXYGEN__ */


/** @} */ /* end of DynamicMemory */

#endif
