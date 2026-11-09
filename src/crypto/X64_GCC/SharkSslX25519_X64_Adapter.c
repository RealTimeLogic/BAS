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
 *   $Id: SharkSslX25519_X64_Adapter.c 6818 2026-11-08 21:18:02Z gianluca $
 *
 *   Linux x86-64 (System V) X25519 backend adapter
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

#ifndef BA_LIB
#define BA_LIB
#endif
#include "SharkSslBigInt.h"
#include "SharkSslEndian.h"

#if defined(SHARKSSL_X25519_ASM_HOOK) && SHARKSSL_X25519_ASM_HOOK
#if (!defined(__GNUC__) || !defined(__x86_64__) || defined(_WIN32))
#error This X25519 adapter requires the System V x86-64 ABI
#endif
#if (!SHARKSSL_USE_ECC || !SHARKSSL_ECC_USE_CURVE25519 || !SHARKSSL_X25519_DEDICATED)
#error This X25519 adapter requires dedicated Curve25519 with ECC enabled
#endif
#if SHARKSSL_X25519_ASM
#error This X25519 adapter cannot be combined with the Cortex-M4 backend
#endif

extern int sharkssl_x25519_adx_available(void);
extern void sharkssl_x25519_adx(U8 out[32], const U8 scalar[32], const U8 u[32]);

/* Return 0 without touching inputs/result when the CPU lacks BMI2/ADX.
 * Otherwise process all scalar bits with SharkSSL's existing word placement,
 * including short inputs, and return 1 with a canonical eight-word result.
 * result may alias either input. Clamping and zero-secret rejection stay in
 * the public API; the assembly masks coordinate bit 255.
 */
int SharkSslX25519_tryAsm(SharkSslBigInt *result, const SharkSslBigInt *k, const SharkSslBigInt *u)
{
   U8 ub[32];
   int w;
   if (sharkssl_x25519_adx_available())
   {
      U8 kb[32], ob[32];
      for (w = 0; w < 8; ++w)
      {
         U32 v = (w < (int)u->len) ? u->beg[u->len - 1 - w] : 0;
         PUT_U32_LE(v, ub, 4 * w);
         v = (w < (int)k->len) ? k->beg[w] : 0;
         PUT_U32_LE(v, kb, 4 * (7 - w));
      }
      sharkssl_x25519_adx(ob, kb, ub);
      for (w = 0; w < 8; ++w)
      {
         U32 v;
         GET_U32_LE(v, ob, 4 * (7 - w));
         result->beg[w] = v;
      }
      result->len = 8;
      for (w = 0; w < 32; ++w)
      {
         ((volatile U8*)kb)[w] = 0;
         ((volatile U8*)ob)[w] = 0;
      }
      return 1;
   }
   return 0;
}
#endif
