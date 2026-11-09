/**
 *     ____             _________                __                _
 *    / __ \___  ____ _/ /_  __(_)___ ___  ___  / /   ____  ____ _(_)____
 *   / /_/ / _ \/ __ `/ / / / / / __ `__ \/ _ \/ /   / __ \/ __ `/ / ___/
 *  / _, _/  __/ /_/ / / / / / / / / / / /  __/ /___/ /_/ / /_/ / / /__
 * /_/ |_|\___/\__,_/_/ /_/ /_/_/ /_/ /_/\___/_____/\____/\__, /_/\___/
 *                                                       /____/
 *
 *                 SharkSSL Embedded SSL/TLS Stack
 ****************************************************************************
 *   PROGRAM MODULE
 *
 *   $Id: SharkSslBigInt.h 6818 2026-11-08 21:18:02Z gianluca $
 *
 *   COPYRIGHT:  Real Time Logic LLC, 2010 - 2026
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
 *               http://www.sharkssl.com
 ****************************************************************************
 *
 */
#ifndef _SharkSslBigInt_h
#define _SharkSslBigInt_h

#include "SharkSSL.h"

/**
 * The following are #defined in SharkSSL_cfg.h
 */
#ifndef SHARKSSL_BIGINT_EXP_SLIDING_WINDOW_K
#error UNDEFINED SHARKSSL_BIGINT_EXP_SLIDING_WINDOW_K
#endif
#if ((SHARKSSL_BIGINT_EXP_SLIDING_WINDOW_K < 1) || (SHARKSSL_BIGINT_EXP_SLIDING_WINDOW_K > 5))
#error SHARKSSL_BIGINT_EXP_SLIDING_WINDOW_K must be in the range 1..5
#endif

#ifndef SHARKSSL_BIGINT_MULT_LOOP_UNROLL
#error UNDEFINED SHARKSSL_BIGINT_MULT_LOOP_UNROLL
#endif

#define SHARKSSL_ECC_USE_NIST            (SHARKSSL_ECC_USE_SECP256R1 || SHARKSSL_ECC_USE_SECP384R1 || SHARKSSL_ECC_USE_SECP521R1)
#define SHARKSSL_ECC_USE_BRAINPOOL       (SHARKSSL_ECC_USE_BRAINPOOLP256R1 || SHARKSSL_ECC_USE_BRAINPOOLP384R1 || SHARKSSL_ECC_USE_BRAINPOOLP512R1)
#define SHARKSSL_ECC_USE_EDWARDS         (SHARKSSL_ECC_USE_CURVE25519 || SHARKSSL_ECC_USE_CURVE448)
/**
 * The generic BigInt Montgomery ladder is reachable only when Curve448 is
 * enabled or when the dedicated Curve25519 implementation is disabled.
 */
#define SHARKSSL_ECC_USE_EDWARDS_LADDER  (SHARKSSL_ECC_USE_CURVE448 || (SHARKSSL_ECC_USE_CURVE25519 && !SHARKSSL_X25519_DEDICATED))


/**
 * do not modify the code below
 */

/**
 * BigInt word size in bits. This is not a configuration option: the 8- and
 * 16-bit implementations have been removed. A definition left in a user
 * configuration must be 32.
 */
#ifndef SHARKSSL_BIGINT_WORDSIZE
#define SHARKSSL_BIGINT_WORDSIZE                         32
#elif (SHARKSSL_BIGINT_WORDSIZE != 32)
#error SHARKSSL_BIGINT_WORDSIZE must be 32 (8- and 16-bit BigInt words are no longer supported)
#endif
typedef U32 SharkSslBigIntWord;
typedef S32 SharkSslBigIntWordS;
typedef U64 SharkSslBigIntDoubleWord;
typedef S64 SharkSslBigIntDoubleWordS;


/**
 * ---------------------------------------------------------------------------------
 * http://std.dkuug.dk/JTC1/SC22/WG14/www/docs/n843.htm
 * 6.5.7 Bitwise shift operators
 * [#5] The result of E1 >> E2 is E1 right-shifted E2 bit positions.
 * If E1 has a signed  type and a negative  value, the resulting value is 
 * implementation-defined.
 * 
 * This macro shifts right a SharkSslBigIntDoubleWordS by SHARKSSL_BIGINT_WORDSIZE 
 * bits preserving the sign
 */
#if _MSC_VER == 1200  /* Visual Studio 6.0 */
#define signed_rshift(a) (a >>= SHARKSSL_BIGINT_WORDSIZE);  
#elif (((SharkSslBigIntDoubleWordS)-1LL >> SHARKSSL_BIGINT_WORDSIZE) & (1LL << SHARKSSL_BIGINT_WORDSIZE))  /* sign is preserved */
#define signed_rshift(a) (a >>= SHARKSSL_BIGINT_WORDSIZE);  
#else
#define signed_rshift(a) do {                                                                            \
   if (a < 0)                                                                                            \
   {                                                                                                     \
      a = ((SharkSslBigIntDoubleWord)-1LL ^ (SharkSslBigIntWord)-1L) | (a >> SHARKSSL_BIGINT_WORDSIZE);  \
   }                                                                                                     \
   else                                                                                                  \
   {                                                                                                     \
      a >>= SHARKSSL_BIGINT_WORDSIZE;                                                                    \
   }                                                                                                     \
} while (0)
#endif


/**
 * ---------------------------------------------------------------------------------
 * The Big Integer object
 *  -mem points to the allocated memory
 *  -beg points to the beginning of the number
 *  -len is the len in SharkSslBigIntWords
 *
 * A number is stored in 'len' SharkSslBigIntWords (see typedef's above)
 * starting from the memory location pointed by 'beg', most significant word first;
 * each word follows the machine endianness.
 * --------------------------------------------
 * SharkSslBigInt numbers are only non-negative
 * --------------------------------------------
 */
typedef struct SharkSslBigInt
{
   SharkSslBigIntWord *mem, *beg;
   U16  len;
} SharkSslBigInt;

/**
 * SHARKSSL__M  is the number of bytes for each SharkSslBigIntWord.
 * Used for internal operations.
 */
#define SHARKSSL__M (SHARKSSL_BIGINT_WORDSIZE / 8)


#define SHARKSSL_ECDSA_ORDER_CT (SHARKSSL_ENABLE_ECDSA && (!SHARKSSL_ECDSA_ONLY_VERIFY) && \
        (SHARKSSL_ECC_USE_SECP256R1 || SHARKSSL_ECC_USE_SECP384R1 || SHARKSSL_ECC_USE_SECP521R1 || SHARKSSL_ECC_USE_BRAINPOOL))

#ifdef __cplusplus
extern "C" {
#endif

/* the order reduction of NIST and Brainpool scalars uses Montgomery products */
#define SHARKSSL_ORDER_REDUCE_MONT ((SHARKSSL_USE_ECC || (SHARKSSL_ENABLE_ECDSA && (!SHARKSSL_ECDSA_ONLY_VERIFY))) && \
                                    (SHARKSSL_ECC_USE_NIST || SHARKSSL_ECC_USE_BRAINPOOL))

/* see SharkSslBigInt.c */
#if (SHARKSSL_ENABLE_RSA || SHARKSSL_ECDSA_ORDER_CT || SHARKSSL_ORDER_REDUCE_MONT || (SHARKSSL_USE_ECC && (SHARKSSL_ECC_USE_BRAINPOOL || SHARKSSL_ECC_USE_EDWARDS)))
SharkSslBigIntWord SharkSslBigInt_calcmu(const SharkSslBigInt *mod);

#if SHARKSSL_OPTIMIZED_BIGINT_ASM
extern
#endif
void SharkSslBigInt_monpro(const SharkSslBigInt *o1, const SharkSslBigInt *o2,
                           SharkSslBigInt *result,   const SharkSslBigInt *mod,
                           SharkSslBigIntWord mu);
#endif

#define SharkSslBigInt_constructor(o,bits,d) \
        SharkSslBigInt_constructor_w(o, (U16)((bits)/SHARKSSL_BIGINT_WORDSIZE),(void*)(d))

#define SharkSslBigInt_getDataPtr(o)     ((o)->beg)

#define SharkSslBigInt_getLengthWords(o) ((o)->len)

#define SharkSslBigInt_getLength(o)      (SharkSslBigInt_getLengthWords(o) * SHARKSSL__M)

#define SharkSslBigInt_even(o)           (!((o)->beg[(o)->len - 1] & 0x1))
        
void    SharkSslBigInt_zero(const SharkSslBigInt *o);

void    SharkSslBigInt_ltrim(SharkSslBigInt *o);

void    SharkSslBigInt_constructor_w(SharkSslBigInt *o, U16 words, void *data);

void    SharkSslBigInt_copy(const SharkSslBigInt *src, SharkSslBigInt *dest);

SharkSslBigIntWord SharkSslBigInt_add(SharkSslBigInt *o1, const SharkSslBigInt *o2);

SharkSslBigIntWord SharkSslBigInt_sub(SharkSslBigInt *o1, const SharkSslBigInt *o2);

void    SharkSslBigInt_addmod(SharkSslBigInt *o1, const SharkSslBigInt *o2,
                              const SharkSslBigInt *mod);

void    SharkSslBigInt_submod(SharkSslBigInt *o1, const SharkSslBigInt *o2,
                              const SharkSslBigInt *mod);

U8      SharkSslBigInt_ge(const SharkSslBigInt *o1, const SharkSslBigInt *o2);

void    SharkSslBigInt_mult(const SharkSslBigInt *o1, const SharkSslBigInt *o2, 
                            SharkSslBigInt *result);

void    SharkSslBigInt_modulo_(SharkSslBigInt *dividend, const SharkSslBigInt *mod, 
                               SharkSslBigIntWord *tmpBuf);

int     SharkSslBigInt_modulo(SharkSslBigInt *dividend, const SharkSslBigInt *mod);

#if ((SHARKSSL_USE_ECC || (SHARKSSL_ENABLE_ECDSA && (!SHARKSSL_ECDSA_ONLY_VERIFY))) && (SHARKSSL_ECC_USE_NIST || SHARKSSL_ECC_USE_BRAINPOOL))
#if SHARKSSL_ECC_USE_SECP521R1
#define SHARKSSL_ORDER_FIXED_WORDS ((521 + SHARKSSL_BIGINT_WORDSIZE - 1) / SHARKSSL_BIGINT_WORDSIZE)
#elif SHARKSSL_ECC_USE_BRAINPOOLP512R1
#define SHARKSSL_ORDER_FIXED_WORDS (512 / SHARKSSL_BIGINT_WORDSIZE)
#elif (SHARKSSL_ECC_USE_SECP384R1 || SHARKSSL_ECC_USE_BRAINPOOLP384R1)
#define SHARKSSL_ORDER_FIXED_WORDS (384 / SHARKSSL_BIGINT_WORDSIZE)
#else
#define SHARKSSL_ORDER_FIXED_WORDS (256 / SHARKSSL_BIGINT_WORDSIZE)
#endif
void    SharkSslBigInt_reduceOrderFixed(SharkSslBigInt *o, const SharkSslBigInt *operand, const SharkSslBigInt *order);
#endif

#if SHARKSSL_ECDSA_ORDER_CT
int     SharkSslBigInt_invmodOrderFixed(SharkSslBigInt *o, const SharkSslBigInt *order);
void    SharkSslBigInt_moduloOrderFixed(SharkSslBigInt *o, const SharkSslBigInt *order);
void    SharkSslBigInt_addmodOrderFixed(SharkSslBigInt *o, const SharkSslBigInt *addend, const SharkSslBigInt *order);
#endif

int     SharkSslBigInt_expmod(const SharkSslBigInt *base, SharkSslBigInt *exp,
                              const SharkSslBigInt *mod,  SharkSslBigInt *res,
                              U8 window);

int     SharkSslBigInt_expmodPublicCT(const SharkSslBigInt *base, const SharkSslBigInt *exp,
                                      const SharkSslBigInt *mod, SharkSslBigInt *res);

/* RSA private key and DHE operations without data-dependent branches or addresses */
#define SHARKSSL_RSA_PRIVATE_CT (SHARKSSL_ENABLE_RSA &&                                       \
        (SHARKSSL_ENABLE_DHE_RSA || SHARKSSL_SSL_SERVER_CODE || SHARKSSL_ENABLE_RSA_API ||    \
         (SHARKSSL_SSL_CLIENT_CODE && SHARKSSL_ENABLE_CLIENT_AUTH)))

#if (SHARKSSL_RSA_PRIVATE_CT || SHARKSSL_ENABLE_ECDHE_RSA || SHARKSSL_ENABLE_ECDHE_ECDSA || (SHARKSSL_USE_ECC && SHARKSSL_ECC_USE_CURVE25519 && SHARKSSL_ENABLE_X25519_API))
void    SharkSslBigInt_submodCT(SharkSslBigInt *o1, const SharkSslBigInt *o2, const SharkSslBigInt *mod);
#endif

#if SHARKSSL_RSA_PRIVATE_CT
int     SharkSslBigInt_invmodOddCT(SharkSslBigInt *o, const SharkSslBigInt *mod);
#endif

#if (SHARKSSL_ENABLE_RSA || SHARKSSL_ECDSA_ORDER_CT)
void    SharkSslBigInt_addmodCT(SharkSslBigInt *o1, const SharkSslBigInt *o2, const SharkSslBigInt *mod);
int     SharkSslBigInt_montR2CT(const SharkSslBigInt *mod, SharkSslBigInt *r2);

int     SharkSslBigInt_expmodCT(const SharkSslBigInt *base, const SharkSslBigInt *exp,
                                const SharkSslBigInt *mod,  const SharkSslBigInt *r2,
                                SharkSslBigInt *res);
#endif

void    SharkSslBigInt_invmod_buf(SharkSslBigInt *u, const SharkSslBigInt *mod,
                                  SharkSslBigIntWord *tmpBuf);

void    SharkSslBigInt_shiftr(SharkSslBigInt *o);

#if (SHARKSSL_ENABLE_RSA || SHARKSSL_ENABLE_ECDSA)
int     SharkSslBigInt_invmod(SharkSslBigInt *o, const SharkSslBigInt *mod);
#endif

#if SHARKSSL_ENABLE_ECDSA
U8      SharkSslBigInt_isZero(SharkSslBigInt *o);
#endif

#if SHARKSSL_ECC_USE_EDWARDS_LADDER
void    SharkSslBigInt_copyfull(const SharkSslBigInt *src, SharkSslBigInt *dest);
void    SharkSslBigInt_swapConditional(SharkSslBigInt *o1, SharkSslBigInt *o2, U32 swapFlag);
#endif

#if (SHARKSSL_ECC_USE_CURVE25519 && SHARKSSL_X25519_DEDICATED)
int     SharkSslBigInt_X25519_mult(SharkSslBigInt *result, const SharkSslBigInt *k, const SharkSslBigInt *u);
#endif


#if (SHARKSSL_ENABLE_RSA && SHARKSSL_ENABLE_RSAKEY_CREATE)
int     SharkSslBigInt_invmod65537CT(SharkSslBigInt *o, const SharkSslBigInt *mod);
int     SharkSslBigInt_genPrime(SharkSslBigInt *o);
int     SharkSslBigInt_GCD(const SharkSslBigInt *o1, const SharkSslBigInt *o2,
                           SharkSslBigInt *result);
#endif

#ifdef __cplusplus
}
#endif


#endif /* _SharkSslBigInt_h */
