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
 *   $Id: BaAtoi.h 4915 2021-12-01 18:26:55Z wini $
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


#ifndef __BaAtoi_h
#define __BaAtoi_h

#include <TargConfig.h>

#ifndef BA_API
#define BA_API
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** Converts characters of s to type U64. The number is negated if
    preceded by '-'. Returns 0 if unable to convert or s is NULL.
*/
BA_API U64 U64_atoll(const char* s);
BA_API U64 U64_atoll2(const char* s, const char* e);

#define S64_atoll(s) ((S64)U64_atoll(s))


   /** Negates a 32 bit number.
    */
BA_API U32 U32_negate(U32 n);

   /** Converts characters of s to type U32. The number is negated if
      preceded by '-'. Returns 0 if unable to convert or s is NULL.
    */
BA_API U32 U32_atoi(const char* s);

   /** Works like U32_atoi, but converts from start of string 's' to end 'e'.
    */
BA_API U32 U32_atoi2(const char* s, const char* e);

   /** Converts hex characters of s to type U32. 
       Returns 0 if unable to convert or s is NULL.
    */
BA_API U32 U32_hextoi(const char *s);
#ifdef __cplusplus
}
#endif


#endif
