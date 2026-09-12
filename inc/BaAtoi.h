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
 *   $Id: BaAtoi.h 5978 2026-09-11 16:13:48Z wini $
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
 *               http://www.realtimelogic.com
 ****************************************************************************
 *
 *
 */


/** @file BaAtoi.h */

#ifndef __BaAtoi_h
#define __BaAtoi_h

#include <TargConfig.h>

#ifndef BA_API
#define BA_API
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** Convert a decimal string to a 64-bit integer representation.
 * @param[in] s NUL-terminated decimal string, optionally starting with '-';
 * NULL is allowed. No whitespace or '+' prefix is accepted by this contract.
 * The caller must provide valid digits and a value within the target range.
 * @return The U64 value, or zero for NULL or an empty string. A negative value
 * is returned in its unsigned representation; use S64_atoll for an S64 result.
 * @note This helper does not validate digits or report overflow. Zero is also
 * a valid conversion result and is not a distinguishable error indication.
 */
BA_API U64 U64_atoll(const char* s);

/** Convert a bounded decimal character sequence to a 64-bit representation.
 * @param[in] s Start of the sequence, or NULL to return zero. If non-NULL,
 * s must be readable even when the sequence is empty.
 * @param[in] e One past the last character, in the same buffer as s, with
 * e >= s. Ignored when s is NULL. A terminating NUL is not required.
 * @return The U64 representation of the converted integer, or zero for NULL
 * or an empty sequence. The same digit, sign and range requirements as
 * U64_atoll apply. This function does not round decimal fractions.
 */
BA_API U64 U64_atoll2(const char* s, const char* e);

/** Convert a decimal string and cast the result to S64.
 * @param[in] s NUL-terminated decimal string or NULL. See U64_atoll for the
 * input requirements; the value must be representable as S64.
 * @return The signed value, or zero for NULL or an empty string.
 */
#define S64_atoll(s) ((S64)U64_atoll(s))

/** Negate a 32-bit integer representation.
 * @param[in] n Value to negate, interpreted as S32. The S32 minimum value
 * must not be supplied, since its signed negation is not representable.
 * @return The negated value represented as U32.
 */
BA_API U32 U32_negate(U32 n);

/** Convert a decimal string, rounding an optional fraction.
 * @param[in] s NUL-terminated decimal string, optionally starting with '-';
 * NULL is allowed. The caller must supply valid decimal digits, at most one
 * '.', and a value whose conversion and optional signed negation are
 * representable. Whitespace and '+' are not supported by this contract.
 * @return The rounded value represented as U32, or zero for NULL or an empty
 * string. A first fractional digit of 5 through 9 rounds the magnitude up;
 * other fractional digits are ignored. Negative values round away from zero
 * at a half and are returned in unsigned representation.
 * @note Invalid input and overflow are not reported. This is not an input
 * validation function.
 */
BA_API U32 U32_atoi(const char* s);

/** Convert a bounded decimal sequence with the rounding used by U32_atoi.
 * @param[in] s Start of the sequence, or NULL to return zero.
 * @param[in] e End of the integer scan, in the same buffer as s, with e >= s.
 * Ignored when s is NULL. The implementation also reads the character at e;
 * if that character is '.', it reads the following character for rounding.
 * These positions must be readable. A '.' found before e also causes the
 * following character to be inspected.
 * @return The U32 result described by U32_atoi. No input-validation or
 * overflow status is returned.
 */
BA_API U32 U32_atoi2(const char* s, const char* e);

/** Convert an unsigned hexadecimal string.
 * @param[in] s NUL-terminated string containing at most eight hexadecimal
 * digits (0-9, a-f or A-F), or NULL. Do not include a sign or a 0x prefix.
 * @return The U32 value, or zero for NULL, an empty string or a string longer
 * than eight characters. Zero is also a valid result. Callers must validate
 * the character set themselves; malformed strings are not reliably rejected.
 */
BA_API U32 U32_hextoi(const char *s);
#ifdef __cplusplus
}
#endif


#endif
