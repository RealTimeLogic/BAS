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
 *   $Id: SharkSslCrypto_X64_CPUID.c 6730 2026-11-03 20:10:54Z gianluca $
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

/* Linux x86-64 CPU/OS probes.
 * Compile for the baseline ISA, without -march=native or -mavx2.
 */
#include <cpuid.h>

static int sharkssl_x64_leaf(unsigned leaf, unsigned *b, unsigned *c)
{
   unsigned a,d;
   /* GCC 4.x provides these primitives, but not __get_cpuid_count. */
   if (__get_cpuid_max(0,0) < leaf)
      return 0;
   __cpuid_count(leaf,0,a,*b,*c,d);
   return 1;
}

static int sharkssl_x64_ymm(void)
{
   unsigned b,c,lo,hi;
   if (!sharkssl_x64_leaf(1,&b,&c) || (c & ((1U<<27)|(1U<<28))) != ((1U<<27)|(1U<<28)))
      return 0;
   __asm__ volatile ("xgetbv" : "=a"(lo), "=d"(hi) : "c"(0));
   return (lo & 6) == 6;
}

int sharkssl_x86_64_gcm_available(void)
{
   unsigned b,c;
   const unsigned mask=(1U<<1)|(1U<<9)|(1U<<25);
   return sharkssl_x64_leaf(1,&b,&c) && (c & mask) == mask;
}
int sharkssl_x86_64_avx2_available(void)
{
   unsigned b,c;
   return sharkssl_x64_ymm() && sharkssl_x64_leaf(7,&b,&c) && (b & (1U<<5)) != 0;
}
