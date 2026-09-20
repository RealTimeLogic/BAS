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
 *   $Id: BaServerLib.h 6056 2026-09-20 05:09:33Z wini $
 *
 *   COPYRIGHT:  Real Time Logic LLC, 2002 - 2026
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


/** @file BaServerLib.h */

#ifndef __httpServerLib_h
#define __httpServerLib_h

#include <TargConfig.h>
#include <AllocatorIntf.h>
#include <ctype.h>
#include  <stddef.h>


#ifdef __cplusplus
extern "C" {
#endif

/* Internal HTTP framing helper. Parse [ptr,end), excluding CRLF.
   Returns the chunk length, or -1 for invalid syntax/overflow. */
BA_API SBaFileSize httpParseChunkSize(const U8* ptr, const U8* end);

/* Internal HTTP-date parser. Returns TRUE and sets *result on success, including
   epoch zero; returns FALSE without changing *result for NULL/invalid input. */
BA_API BaBool httpParseDate(const char* str, BaTime* result);


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

/** Encode one byte as two lowercase hexadecimal characters.
 * @param[out] hexOutData Writable buffer of at least two bytes. No NUL is added.
 * @param[in] binIn Byte to encode. */
BA_API void baConvBin2Hex(void* hexOutData, U8 binIn);

#endif


/** Decode one hexadecimal digit.
 * @param[in] c Character from 0-9, a-f or A-F.
 * @return Numeric value 0-15; an invalid character also returns zero. */
BA_API U8 baConvHex2Bin( U8 c );
/** Encode four bytes as eight lowercase hexadecimal characters.
 * @param[out] to Writable buffer of at least eight bytes; no NUL is added.
 * @param[in] from Value to encode. The implementation visits its memory bytes
 * from offset 3 to offset 0, producing conventional most-significant-digit
 * first notation on little-endian targets. See baConvHexToU32. */
BA_API void baConvU32ToHex(void* to, U32 from);
/** Decode eight hexadecimal characters into a U32.
 * @param[in] from At least eight readable characters, or NULL. A terminator is
 * not read. Invalid characters decode as zero-valued nibbles.
 * @return Decoded value, or zero for NULL. Byte placement is the inverse of
 * baConvU32ToHex and therefore depends on the target byte order. */
BA_API U32 baConvHexToU32(const void* from);


/** Allocate a NUL-terminated copy of a string using baMalloc.
 * @param[in] str Source string or NULL; the source is not modified.
 * @return Owned copy including a NUL byte, or NULL for a NULL source or an
 * allocation failure. Release a successful result with baFree. */
BA_API char* baStrdup(const char* str);

/** Find a key in a sorted array without modifying it.
 * @param[in] key Search key passed as the comparator's first argument.
 * @param[in] base Array of num records sorted in ascending comparator order.
 * @param[in] num Nonnegative number of records.
 * @param[in] size Positive size of each record in bytes. Array indexing and
 * midpoint arithmetic must fit in int as used by this implementation.
 * @param[in] cmp Required comparison callback, called as cmp(key, record).
 * It returns negative, zero or positive when key is less than, equal to or
 * greater than the record's key. It must not reorder the array.
 * @return Borrowed pointer to a matching record, or NULL if no match exists
 * or num is zero. With duplicate keys, which record is returned is unspecified. */
BA_API const void* baBSearch(const void* key, const void* base, int num, 
                             int size, int (*cmp)(const void*,const void*));

/** Compare two NUL-terminated strings using bTolower for case folding.
 * @param[in] a Required first string.
 * @param[in] b Required second string. This is a byte comparison, not Unicode
 * case folding; character handling follows the platform bTolower definition.
 * @return Negative, zero or positive when a sorts before, equals or sorts
 * after b under that comparison. */
BA_API int baStrCaseCmp(const char *a, const char *b);

/** Compare at most len bytes, using bTolower for case folding.
 * @param[in] a First string, readable through its NUL or len bytes.
 * @param[in] b Second string, readable through its NUL or len bytes.
 * @param[in] len Maximum byte count; zero performs no character access.
 * @return Negative, zero or positive as in baStrCaseCmp. Equal prefixes of
 * len bytes compare equal even if the strings differ afterward. */
BA_API int baStrnCaseCmp(const char *a, const char *b, size_t len);

/** Locate a token separated by characters in set.
 * @param[in,out] str Required pointer to a nonempty NUL-terminated string.
 * Leading delimiter characters are skipped by advancing *str. On success
 * *str points to the first token byte; the function does not advance it to
 * the returned end pointer. The string itself is not modified.
 * @param[in] set NUL-terminated delimiter character set.
 * @return Borrowed pointer just past the token, at a delimiter or the NUL,
 * or NULL when only delimiters remain. The token is not NUL-terminated by
 * this function; its length is the returned pointer minus *str. */
const char* baGetToken(const char** str, const char* set);

/** Parse an HTTP date string as UTC.
 * @param[in] str NUL-terminated IMF-fixdate, RFC 850, or asctime date, or NULL.
 * Names are case-sensitive; the first two forms require GMT. Surrounding
 * SP/HTAB is accepted; invalid calendar fields, trailing data, and date lists
 * are rejected. RFC 850 years use the HTTP 50-year rule and the server clock.
 * A recognized weekday name is required but is not compared to the date.
 * @return Seconds since 1970-01-01 00:00:00 UTC, or zero for invalid, NULL,
 * or empty input. Zero is also a valid timestamp. A leap second (60) maps to
 * the following POSIX second; no historical leap-second table is consulted.
 * @see BaTime */
BA_API BaTime baParseDate(const char* str);


/** Decode base64 or base64url into a caller buffer.
 * @param[out] outStr Buffer with outStrSize writable bytes; it may alias
 * b64EncStr for in-place decoding. No NUL terminator is written.
 * @param[in] outStrSize Nonnegative capacity in bytes.
 * @param[in] b64EncStr Required NUL-terminated input. Bytes outside both
 * base64 alphabets, including padding, are ignored rather than rejected.
 * @param[out] overflow Optional pointer, set to TRUE if decoded bytes were
 * discarded because the output was full; otherwise FALSE. May be NULL.
 * @return Number of bytes actually written, not the full decoded length.
 * Invalid input is not reported separately. */
BA_API int baB64Decode(
   unsigned char* outStr,int outStrSize,const char* b64EncStr,
   BaBool* overflow);

/** Normalize a slash-separated path in place.
 * @param[in,out] str Required writable NUL-terminated path. Repeated slashes,
 * '.' components and matched parent components are removed without allocation.
 * @return Zero on completion, -1 if a parent component would escape the
 * starting level. The buffer may have been modified on failure. This function
 * does not resolve filesystem links or perform filesystem access checks. */
BA_API int baElideDotDot(char* str);

/** Decode selected XML entity spellings in place.
 * @param[in,out] f Required writable NUL-terminated string. The result remains
 * NUL-terminated and is no longer than the input. Numeric character references
 * and unrecognized entity spellings remain unchanged. The implementation
 * handles lt, gt, apos and amp, but currently spells the quote entity as
 * "qout" rather than the XML spelling "quot". No error status is returned. */
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
   /** day of the month [1,31], valid for the selected month */
   int       tm_mday;
   /** months since January [0,11] */
   int       tm_mon;
   /** Years since 0000 (baTime2tmEx) or since 1900 (baTime2tm) */
   int       tm_year;
   /** days since Sunday [0,6] */
   int       tm_wday;
   /** Days since January 1 [0,365]; output of the time-to-calendar functions. */
   int       tm_yday;

   /** Fractional second in nanoseconds [0,999999999]. */
   int       nsec;
   /** Minutes east of UTC [-1439,1439]; negative means west. */
   int       offset;
};

/** Return the number of days in a Gregorian calendar month.
 * @param[in] y Calendar year used for leap-year calculation.
 * @param[in] m Month from 1 (January) through 12 (December); not validated.
 * @return Number of days, from 28 through 31. */
BA_API U8 baDaysInMonth(U16 y, U16 m);

/** Convert UTC epoch seconds to calendar fields.
 * @param[out] tmP Required output structure; valid only on success. tm_year
 * is years since 1900, tm_mon is zero-based, nsec and offset are zero.
 * @param[in] t UTC seconds from -62135596800 through 253402300799
 * (years 0001 through 9999).
 * @return Zero on success, -1 for a timestamp outside the supported range. */
BA_API int baTime2tm(struct BaTm *tmP, BaTime t);
/** Convert a timestamp to UTC or offset-adjusted calendar fields.
 * @param[in] tex Required timestamp. nsec must be 0-999999999 and offset must
 * be -1439..1439 minutes. sec + offset * 60 must fall in years 0001..9999;
 * callers must also keep the calendar value selected by local in that range.
 * @param[in] local TRUE applies tex->offset; FALSE produces UTC fields.
 * @param[out] tm Required output, valid only on success. tm_year is the full
 * calendar year, tm_mon is zero-based. nsec and offset are copied even when
 * local is FALSE. Weekday and year-day fields are populated.
 * @return Zero on success, -1 when timestamp validation fails. */
BA_API int baTime2tmEx(const BaTimeEx* tex, const BaBool local, struct BaTm* tm);


/** Convert calendar fields to UTC epoch seconds.
 * @param[in,out] tmP Required structure with tm_year as years since 1900,
 * tm_mon in 0..11 and valid day/time fields. Initialize nsec and offset to
 * zero. The function changes tm_year and tm_mon, including on failure; pass
 * a copy if the original fields are needed afterward. tm_wday and tm_yday
 * are ignored.
 * @return Epoch seconds on success, or zero for a reported calendar error.
 * Zero is also a valid result; use baTm2TimeEx for a separate status value. */
BA_API BaTime baTm2Time(struct BaTm* tmP);
/** Convert calendar fields and an optional timezone offset to a timestamp.
 * @param[in,out] tm Required initialized structure. tm_year is the full year
 * (use 1..9999), tm_mon is 0..11, and day, hour, minute and second must form
 * a valid date/time. Supply valid nsec and offset values as described by
 * BaTimeEx; these two fields are copied without validation. tm_wday and
 * tm_yday are ignored. tm_mon and possibly tm_year are changed during
 * conversion, including on failure; use a copy to preserve the input.
 * @param[in] local TRUE subtracts tm->offset to obtain UTC seconds; FALSE
 * interprets the calendar fields as UTC.
 * @param[out] tex Required output timestamp, valid only on success.
 * @return Zero on success, -1 for a detected invalid date/time. */
BA_API int baTm2TimeEx(struct BaTm* tm, BaBool local, BaTimeEx* tex);

/** Parse an ISO 8601 calendar timestamp with an explicit timezone.
 * @param[in] str Required readable input, with format YYYY-MM-DDTHH:MM:SSZ
 * or YYYY-MM-DDTHH:MM:SS+HH:MM (also allowing a negative offset). A space
 * or lowercase t may replace T; lowercase z is accepted. An optional decimal
 * point and 1..9 fractional digits may precede the timezone.
 * @param[in] len Exact byte length, excluding any NUL; at least 20. Supply
 * a complete timezone suffix. No trailing characters are accepted.
 * @param[out] tex Required timestamp buffer, valid only on success. sec is
 * UTC, nsec is scaled to nanoseconds, and offset retains the supplied minutes.
 * @return Zero on success, -1 for a detected invalid format or date. Years
 * are 0001..9999; leap seconds and a missing timezone are not supported. */
BA_API int baISO8601ToTime(const char *str, size_t len, BaTimeEx *tex);


/** Format a timestamp as ISO 8601 using its timezone offset.
 * @param[in] tex Required timestamp satisfying baTime2tmEx requirements.
 * @param[out] str Writable output buffer; NUL-terminated on success. The
 * output uses T, a nine-digit fractional part when nsec is nonzero, and Z
 * for offset zero or a signed HH:MM offset otherwise.
 * @param[in] len Buffer capacity in bytes, including the NUL. At least 36
 * bytes are required even when the particular result would be shorter.
 * @return Character count excluding the NUL, or -1 for insufficient capacity
 * or an invalid timestamp. Output is valid only on success. */
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

/** Allocate uninitialized storage using the target's configured allocator.
 * @param[in] size Requested bytes. Zero-size behavior is platform-dependent.
 * @return Owned block or NULL on failure; release with baFree. */
void* baMalloc(size_t size);

/** Resize storage using the target's configured allocator.
 * @param[in] p Existing allocation from this allocator, or NULL.
 * @param[in] size Requested bytes. Use a positive value for portable behavior;
 * zero-size behavior follows the target's allocator.
 * @return Replacement block preserving the smaller old/new byte count, or
 * NULL on failure. For a positive size, failure preserves p; success consumes
 * the old block. Newly added bytes are uninitialized. */
void* baRealloc(void* p, size_t size);


/** Release storage using the target's configured allocator.
 * @param[in] p Live block from baMalloc or baRealloc. NULL handling follows
 * the target's allocator. Do not access the block after this call. */
void baFree(void* p);

#endif /*  __DOXYGEN__ */


/** @} */ /* end of DynamicMemory */

#endif
