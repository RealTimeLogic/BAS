/* Copyright (c) Real Time Logic LLC, 2026. All rights reserved.
 * Linux x86-64 CPU/OS probes. Same license as SharkSslCrypto.c.
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
int sharkssl_x86_64_vaes_available(void)
{
   unsigned b,c;
   return sharkssl_x86_64_gcm_available() && sharkssl_x64_ymm() &&
      sharkssl_x64_leaf(7,&b,&c) && (b & (1U<<5)) && (c & (1U<<9)) && (c & (1U<<10));
}
