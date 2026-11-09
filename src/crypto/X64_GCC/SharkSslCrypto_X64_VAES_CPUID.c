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
 *   $Id: SharkSslCrypto_X64_VAES_CPUID.c 6818 2026-11-08 21:18:02Z gianluca $
 *
 *   COPYRIGHT:  Real Time Logic LLC, 2026
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

/* Linux x86-64 CPU/OS probe for the VAES kernel.
 * Compile for the baseline ISA, without -march=native or -mavx2.
 */
#include <cpuid.h>

static int sharkssl_x64_vaes_leaf(unsigned leaf, unsigned *b, unsigned *c)
{
   unsigned a,d;
   /* GCC 4.x provides these primitives, but not __get_cpuid_count. */
   if (__get_cpuid_max(0,0) < leaf)
      return 0;
   __cpuid_count(leaf,0,a,*b,*c,d);
   return 1;
}

int sharkssl_x86_64_vaes_available(void)
{
   unsigned b,c,lo,hi;
   /* SSSE3, PCLMULQDQ and AES-NI for the kernel tails; OSXSAVE and AVX */
   const unsigned mask1=(1U<<1)|(1U<<9)|(1U<<25)|(1U<<27)|(1U<<28);
   if (!sharkssl_x64_vaes_leaf(1,&b,&c) || (c & mask1) != mask1)
      return 0;
   /* XMM and YMM state for the 256-bit kernel */
   __asm__ volatile ("xgetbv" : "=a"(lo), "=d"(hi) : "c"(0));
   if ((lo & 6) != 6)
      return 0;
   /* AVX2, then VAES and VPCLMULQDQ */
   return sharkssl_x64_vaes_leaf(7,&b,&c) && (b & (1U<<5)) &&
      (c & (1U<<9)) && (c & (1U<<10));
}
