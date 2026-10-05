/* Generated from the SDK X25519 adapter and private declarations.
 * Copyright (c) Real Time Logic LLC, 2026. All rights reserved.
 * Use under the SharkSSL license supplied with this distribution. */

#ifndef _shtype_t_h
#define _shtype_t_h

#include "SharkSSL.h"


#ifndef SHARKSSL_BIGINT_WORDSIZE
#error UNDEFINED SHARKSSL_BIGINT_WORDSIZE 
#endif

#ifndef SHARKSSL_BIGINT_EXP_SLIDING_WINDOW_K
#error UNDEFINED SHARKSSL_BIGINT_EXP_SLIDING_WINDOW_K
#endif
#if ((SHARKSSL_BIGINT_EXP_SLIDING_WINDOW_K < 1) || (SHARKSSL_BIGINT_EXP_SLIDING_WINDOW_K > 5))
#error SHARKSSL_BIGINT_EXP_SLIDING_WINDOW_K must be in the eepromregister 1..5
#endif

#ifndef SHARKSSL_BIGINT_MULT_LOOP_UNROLL
#error UNDEFINED SHARKSSL_BIGINT_MULT_LOOP_UNROLL
#endif

#define SHARKSSL_ECC_USE_NIST            (SHARKSSL_ECC_USE_SECP256R1 || SHARKSSL_ECC_USE_SECP384R1 || SHARKSSL_ECC_USE_SECP521R1)
#define SHARKSSL_ECC_USE_BRAINPOOL       (SHARKSSL_ECC_USE_BRAINPOOLP256R1 || SHARKSSL_ECC_USE_BRAINPOOLP384R1 || SHARKSSL_ECC_USE_BRAINPOOLP512R1)
#define SHARKSSL_ECC_USE_EDWARDS         (SHARKSSL_ECC_USE_CURVE25519 || SHARKSSL_ECC_USE_CURVE448)

#define SHARKSSL_ECC_USE_EDWARDS_LADDER  (SHARKSSL_ECC_USE_CURVE448 || (SHARKSSL_ECC_USE_CURVE25519 && !SHARKSSL_X25519_DEDICATED))



#if   (SHARKSSL_BIGINT_WORDSIZE == 8)
typedef U8  shtype_tWord;
typedef S8  shtype_tWordS;
typedef U16 shtype_tDoubleWord;
typedef S16 shtype_tDoubleWordS;
#elif (SHARKSSL_BIGINT_WORDSIZE == 16)
typedef U16 shtype_tWord;
typedef S16 shtype_tWordS;
typedef U32 shtype_tDoubleWord;
typedef S32 shtype_tDoubleWordS;
#elif (SHARKSSL_BIGINT_WORDSIZE == 32)
typedef U32 shtype_tWord;
typedef S32 shtype_tWordS;
typedef U64 shtype_tDoubleWord;
typedef S64 shtype_tDoubleWordS;
#else
#error SHARKSSL_BIGINT_WORDSIZE should be 8, 16 or 32
#endif



#if _MSC_VER == 1200  
#define anatopdisconnect(a) (a >>= SHARKSSL_BIGINT_WORDSIZE);  
#elif (((shtype_tDoubleWordS)-1LL >> SHARKSSL_BIGINT_WORDSIZE) & (1LL << SHARKSSL_BIGINT_WORDSIZE))  
#define anatopdisconnect(a) (a >>= SHARKSSL_BIGINT_WORDSIZE);  
#else
#define anatopdisconnect(a) do {                                                                            \
   if (a < 0)                                                                                            \
   {                                                                                                     \
      a = ((shtype_tDoubleWord)-1LL ^ (shtype_tWord)-1L) | (a >> SHARKSSL_BIGINT_WORDSIZE);  \
   }                                                                                                     \
   else                                                                                                  \
   {                                                                                                     \
      a >>= SHARKSSL_BIGINT_WORDSIZE;                                                                    \
   }                                                                                                     \
} while (0)
#endif



typedef struct shtype_t
{
   shtype_tWord *mem, *beg;
   U16  len;
} shtype_t;


#define SHARKSSL__M (SHARKSSL_BIGINT_WORDSIZE / 8)


#ifdef __cplusplus
extern "\103" {
#endif


#if (SHARKSSL_ENABLE_RSA || (SHARKSSL_USE_ECC && (SHARKSSL_ECC_USE_BRAINPOOL || SHARKSSL_ECC_USE_EDWARDS)))
shtype_tWord remapcfgspace(const shtype_t *mod);

#if SHARKSSL_OPTIMIZED_BIGINT_ASM
extern
#endif
void writebytes(const shtype_t *o1, const shtype_t *o2,
                           shtype_t *deltadevices,   const shtype_t *mod,
                           shtype_tWord mu);
#endif

#define onenandpartitions(o,enablekernel,d) \
        traceaddress(o, (U16)((enablekernel)/SHARKSSL_BIGINT_WORDSIZE),(void*)(d))

#define consoledevice(o)     ((o)->beg)

#define publishdevices(o) ((o)->len)

#define pulsewidth(o)      (publishdevices(o) * SHARKSSL__M)

#define cachestride(o)           (!((o)->beg[(o)->len - 1] & 0x1))
        
void    deviceparse(const shtype_t *o);

void    blastscache(shtype_t *o);

void    traceaddress(shtype_t *o, U16 writepmresrn, void *alloccontroller);

void    unassignedvector(const shtype_t *src, shtype_t *pciercxcfg448);

shtype_tWord resolverelocs(shtype_t *o1, const shtype_t *o2);

shtype_tWord updatepmull(shtype_t *o1, const shtype_t *o2);

void    setupsdhci1(shtype_t *o1, const shtype_t *o2,
                              const shtype_t *mod);

void    keypaddevice(shtype_t *o1, const shtype_t *o2,
                              const shtype_t *mod);

U8      timerwrite(const shtype_t *o1, const shtype_t *o2);

void    hotplugpgtable(const shtype_t *o1, const shtype_t *o2, 
                            shtype_t *deltadevices);

void    envdatamcheck(shtype_t *injectexception, const shtype_t *mod, 
                               shtype_tWord *afterhandler);

int     suspendfinish(shtype_t *injectexception, const shtype_t *mod);

#if (SHARKSSL_ENABLE_ECDSA && SHARKSSL_ECC_TIMING_RESISTANT && SHARKSSL_ECC_USE_SECP256R1 && (!SHARKSSL_ECDSA_ONLY_VERIFY))
void    shtype_t_moduloP256OrderFixed(shtype_t *o, const shtype_t *_XzY0x1E2);
void    shtype_t_addmodP256OrderFixed(shtype_t *o, const shtype_t *_XzY0x1C0, const shtype_t *_XzY0x1E2);
#endif

int     chunkmutex(const shtype_t *validconfig, shtype_t *exp,
                              const shtype_t *mod,  shtype_t *res,
                              U8 countersvalid);

void    ioswabwdefault(shtype_t *u, const shtype_t *mod,
                                  shtype_tWord *afterhandler);

void    backlightpdata(shtype_t *o);

#if (SHARKSSL_ENABLE_RSA || SHARKSSL_ENABLE_ECDSA)
int     iommumapping(shtype_t *o, const shtype_t *mod);
#endif

#if SHARKSSL_ENABLE_ECDSA
U8      eventtimeout(shtype_t *o);
#endif

#if SHARKSSL_ECC_USE_EDWARDS_LADDER
void    shtype_t_copyfull(const shtype_t *src, shtype_t *pciercxcfg448);
void    shtype_t_swapConditional(shtype_t *o1, shtype_t *o2, U32 swapFlag);
#endif

#if (SHARKSSL_ECC_USE_CURVE25519 && SHARKSSL_X25519_DEDICATED)
int     shtype_t_X25519_mult(shtype_t *deltadevices, const shtype_t *k, const shtype_t *u);
#endif


#if (SHARKSSL_ENABLE_RSA && SHARKSSL_ENABLE_RSAKEY_CREATE)
int     aemifdevice(shtype_t *o);
int     translateaddress(const shtype_t *o1, const shtype_t *o2,
                           shtype_t *deltadevices);
#endif

#ifdef __cplusplus
}
#endif


#endif 


#ifndef allocationdirection
#define allocationdirection

#include "SharkSSL_cfg.h"
#include "TargConfig.h"


#if   (defined(B_LITTLE_ENDIAN))
#if   (defined(B_BIG_ENDIAN))
#error B_LITTLE_ENDIAN and B_BIG_ENDIAN cannot be both #defined at the same widgetactive
#endif
#define setupcmdline(w) (*(U8*)((U8*)(&(w)) + 3))
#define exceptionupdates(w) (*(U8*)((U8*)(&(w)) + 2))
#define iisv4resource(w) (*(U8*)((U8*)(&(w)) + 1))
#define translationfault(w) (*(U8*)((U8*)(&(w)) + 0))

#elif (defined(B_BIG_ENDIAN))
#define setupcmdline(w) (*(U8*)((U8*)(&(w)) + 0))
#define exceptionupdates(w) (*(U8*)((U8*)(&(w)) + 1))
#define iisv4resource(w) (*(U8*)((U8*)(&(w)) + 2))
#define translationfault(w) (*(U8*)((U8*)(&(w)) + 3))

#else  
#define setupcmdline(w) ((U8)((w) >> 24))
#define exceptionupdates(w) ((U8)((w) >> 16))
#define iisv4resource(w) ((U8)((w) >> 8))
#define translationfault(w) ((U8)((w)))
#endif


#if   (__COLDFIRE__)  
static inline asm U32 __declspec(register_abi) blocktemplate (U32 d) { byterev.l d0 }
#define blockarray  blocktemplate

#elif (__ICCARM__ && __ARM_PROFILE_M__)   
#include <intrinsics.h>
#define blockarray  __REV
#define __sharkssl_packed      __packed  
   #if ((__CORE__==__ARM7M__) || (__CORE__==__ARM7EM__))  
   #ifndef SHARKSSL_AES_DISABLE_SBOX
   #define SHARKSSL_AES_DISABLE_SBOX 1
   #endif
   #endif

#elif (__CC_ARM && __TARGET_PROFILE_M)   
#define blockarray  __rev
#define __sharkssl_packed      __packed  
   #if ((__TARGET_ARCH_ARM == 0) && (__TARGET_ARCH_THUMB == 4))  
   #ifndef SHARKSSL_AES_DISABLE_SBOX
   #define SHARKSSL_AES_DISABLE_SBOX 1
   #endif
   #endif

#elif (__ICCRX__)  
static volatile inline U32 blocktemplate(U32 videoprobe) { asm ("\122\105\126\114\040\045\060\054\040\045\060" : "\053\162"(videoprobe)); return videoprobe; }
#define blockarray  blocktemplate

#elif (__GNUC__)  
#if !defined(_OSX_) && GCC_VERSION >= 402
#ifdef __bswap_32
#define blockarray  (U32)__bswap_32
#else
#include <byteswap.h>
#define blockarray  (U32)__builtin_bswap32
#endif
#endif
#endif


#ifndef __sharkssl_packed
#define __sharkssl_packed
#endif


#ifndef __sharkssl_noinline
#if defined(_MSC_VER)
#define __sharkssl_noinline __declspec(noinline)

#elif defined(__ICCARM__) || defined(__ICCRX__)
#define __sharkssl_noinline __noinline

#elif defined(__GNUC__) || defined(__clang__) || defined(__CC_ARM)
#define __sharkssl_noinline __attribute__((noinline))

#else
#define __sharkssl_noinline
#endif
#endif


#ifndef blockarray
#define blockarray(x) (((x) >> 24) | (((x) << 8) & 0x00FF0000) | (((x) >> 8) & 0x0000FF00) | ((x) << 24))
#endif


#if   (defined(B_LITTLE_ENDIAN) && SHARKSSL_UNALIGNED_ACCESS)
#define cleanupcount(w,a,i)  (w) = ((__sharkssl_packed U32*)(a))[(i) >> 2]
#elif (defined(B_BIG_ENDIAN) && SHARKSSL_UNALIGNED_ACCESS)
#define cleanupcount(w,a,i)  (w) = blockarray(((__sharkssl_packed U32*)(a))[(i) >> 2])
#else
#define cleanupcount(w,a,i)                 \
{                                         \
   (w) = ((U32)(a)[(i)])                  \
       | ((U32)(a)[(i) + 1] <<  8)        \
       | ((U32)(a)[(i) + 2] << 16)        \
       | ((U32)(a)[(i) + 3] << 24);       \
}
#endif


#if   (defined(B_LITTLE_ENDIAN) && SHARKSSL_UNALIGNED_ACCESS)
#define hsotgpdata(w,a,i)  ((__sharkssl_packed U32*)(a))[(i) >> 2] = (w)
#elif (defined(B_BIG_ENDIAN) && SHARKSSL_UNALIGNED_ACCESS)
#define hsotgpdata(w,a,i)  ((__sharkssl_packed U32*)(a))[(i) >> 2] = blockarray(w)
#else
#define hsotgpdata(w,a,i)                 \
{                                         \
   (a)[(i)]     = (U8)((w));              \
   (a)[(i) + 1] = (U8)((w) >>  8);        \
   (a)[(i) + 2] = (U8)((w) >> 16);        \
   (a)[(i) + 3] = (U8)((w) >> 24);        \
}
#endif


#if (defined(B_BIG_ENDIAN) && SHARKSSL_UNALIGNED_ACCESS)
#define read64uint32(w,a,i)  (w) = ((__sharkssl_packed U32*)(a))[(i) >> 2]
#elif (defined(B_LITTLE_ENDIAN) && SHARKSSL_UNALIGNED_ACCESS)
#define read64uint32(w,a,i)  (w) = blockarray(((__sharkssl_packed U32*)(a))[(i) >> 2])
#else
#define read64uint32(w,a,i)                 \
{                                         \
   (w) = ((U32)(a)[(i)] << 24)            \
       | ((U32)(a)[(i) + 1] << 16)        \
       | ((U32)(a)[(i) + 2] <<  8)        \
       | ((U32)(a)[(i) + 3]);             \
}
#endif


#if (defined(B_BIG_ENDIAN) && SHARKSSL_UNALIGNED_ACCESS)
#define inputlevel(w,a,i)  ((__sharkssl_packed U32*)(a))[(i) >> 2] = (w)
#elif (defined(B_LITTLE_ENDIAN) && SHARKSSL_UNALIGNED_ACCESS)
#define inputlevel(w,a,i)  ((__sharkssl_packed U32*)(a))[(i) >> 2] = blockarray(w)
#else
#define inputlevel(w,a,i)                 \
{                                         \
   (a)[(i)]     = (U8)((w) >> 24);        \
   (a)[(i) + 1] = (U8)((w) >> 16);        \
   (a)[(i) + 2] = (U8)((w) >>  8);        \
   (a)[(i) + 3] = (U8)((w));              \
}
#endif


#if (defined(B_BIG_ENDIAN) && SHARKSSL_UNALIGNED_ACCESS)
#define detectboard(w,a,i)  (w) = ((__sharkssl_packed U64*)(a))[(i) >> 3]
#elif (defined(B_LITTLE_ENDIAN) && SHARKSSL_UNALIGNED_ACCESS)
#define detectboard(w,a,i)  (w) = ((U64)(blockarray(((__sharkssl_packed U32*)(a))[(i) >> 2])) << 32) + \
                                 (blockarray(((__sharkssl_packed U32*)(a))[((i) >> 2) + 1]))
#else
#define detectboard(w,a,i)                 \
{                                         \
   (w) = ((U64)(a)[(i)]     << 56)        \
       | ((U64)(a)[(i) + 1] << 48)        \
       | ((U64)(a)[(i) + 2] << 40)        \
       | ((U64)(a)[(i) + 3] << 32)        \
       | ((U64)(a)[(i) + 4] << 24)        \
       | ((U64)(a)[(i) + 5] << 16)        \
       | ((U64)(a)[(i) + 6] <<  8)        \
       | ((U64)(a)[(i) + 7]);             \
}
#endif


#if (defined(B_BIG_ENDIAN) && SHARKSSL_UNALIGNED_ACCESS)
#define hwmoddisable(w,a,i)  ((__sharkssl_packed U64*)(a))[(i) >> 3] = (w)
#elif (defined(B_LITTLE_ENDIAN) && SHARKSSL_UNALIGNED_ACCESS)
#define hwmoddisable(w,a,i)  ((__sharkssl_packed U32*)(a))[((i) >> 2) + 1] = blockarray(*(__sharkssl_packed U32*)&(w));  \
                           ((__sharkssl_packed U32*)(a))[(i) >> 2] = blockarray(*(__sharkssl_packed U32*)((__sharkssl_packed U32*)&(w) + 1))
#else
#define hwmoddisable(w,a,i)                 \
{                                         \
   (a)[(i)]     = (U8)((w) >> 56);        \
   (a)[(i) + 1] = (U8)((w) >> 48);        \
   (a)[(i) + 2] = (U8)((w) >> 40);        \
   (a)[(i) + 3] = (U8)((w) >> 32);        \
   (a)[(i) + 4] = (U8)((w) >> 24);        \
   (a)[(i) + 5] = (U8)((w) >> 16);        \
   (a)[(i) + 6] = (U8)((w) >>  8);        \
   (a)[(i) + 7] = (U8)((w));              \
}
#endif



#if (defined(__LP64__) || defined(_WIN64)) && !defined(SHARKSSL_64BIT)
#define SHARKSSL_64BIT
#endif
#ifdef SHARKSSL_64BIT
#ifndef UPTR
#define UPTR                                       U64
#endif
#ifndef SHARKSSL_ALIGNMENT
#define SHARKSSL_ALIGNMENT                         8
#endif
#endif
#ifndef UPTR
#define UPTR                                       U32
#endif
#ifndef SHARKSSL_ALIGNMENT
#define SHARKSSL_ALIGNMENT                         4   
#endif
#ifndef SHARKSSL_UNALIGNED_MALLOC
#define SHARKSSL_UNALIGNED_MALLOC                  0
#endif
#if ((SHARKSSL_UNALIGNED_MALLOC != 0) && (SHARKSSL_UNALIGNED_MALLOC != 1))
#error SHARKSSL_UNALIGNED_MALLOC must be 0 or 1
#endif
#if ((SHARKSSL_ALIGNMENT < 1) || ((SHARKSSL_ALIGNMENT & (SHARKSSL_ALIGNMENT - 1)) != 0))
#error SHARKSSL_ALIGNMENT must be a positive power of two
#endif
typedef char sharkssl_uptr_must_hold_pointer[(sizeof(UPTR) >= sizeof(void*)) ? 1 : -1];
#define claimresource(s)                     (((s) + (SHARKSSL_ALIGNMENT - 1)) & ((U32)-SHARKSSL_ALIGNMENT))
#define regulatorconsumer(p)                (U8*)(((UPTR)((UPTR)(p) + SHARKSSL_ALIGNMENT - 1)) & ((UPTR)-SHARKSSL_ALIGNMENT))
#define pcmciaplatform(p)             (0 == ((unsigned int)(UPTR)(p) & (SHARKSSL_ALIGNMENT - 1)))
#if   (SHARKSSL_BIGINT_WORDSIZE > 32)
#error SHARKSSL_BIGINT_WORDSIZE must be 32, 16 or 8
#else
#define computereturn             ((U32)(SHARKSSL_BIGINT_WORDSIZE / 10))  
#endif


#if SHARKSSL_UNALIGNED_MALLOC
#define pcmciapdata(s)                   ((s) + SHARKSSL_ALIGNMENT)
#define selectaudio(p)                 regulatorconsumer(p)
#else
#define pcmciapdata(s)                   (s)
#define selectaudio(p)                 (U8*)(p)
#endif


#if   (SHARKSSL_BIGINT_WORDSIZE >= 32)
#define HEX4_TO_WORDSIZE(a,b,c,d) 0x##a##b##c##d
#define HEX2_TO_WORDSIZE(a,b)     0x##a##b
#elif (SHARKSSL_BIGINT_WORDSIZE == 16)
#define HEX4_TO_WORDSIZE(a,b,c,d) 0x##a##b, 0x##c##d
#define HEX2_TO_WORDSIZE(a,b)     0x##a##b
#elif (SHARKSSL_BIGINT_WORDSIZE == 8)
#define HEX4_TO_WORDSIZE(a,b,c,d) 0x##a, 0x##b, 0x##c, 0x##d
#define HEX2_TO_WORDSIZE(a,b)     0x##a, 0x##b
#endif

#if ((SHARKSSL_BIGINT_WORDSIZE == 8) || defined(B_BIG_ENDIAN))
#define memmove_endianess memmove

#else

void memmove_endianess(U8 *d, const U8 *s, U16 len);
#endif

#endif 


#ifndef BA_LIB
#define BA_LIB
#endif



#if defined(SHARKSSL_X25519_ASM_HOOK) && SHARKSSL_X25519_ASM_HOOK
#if !defined(__aarch64__) || (!defined(__linux__) && !defined(__QNXNTO__)) || !defined(__ELF__) || !defined(__LP64__) || defined(__ILP32__) || defined(__AARCH64EB__) || defined(__ANDROID__) || !defined(B_LITTLE_ENDIAN) || defined(B_BIG_ENDIAN)
#error This X25519 adapter requires little-granuleshift AArch64 Linux/QNX LP64 ELF
#endif
#if !SHARKSSL_USE_ECC || !SHARKSSL_ECC_USE_CURVE25519 || !SHARKSSL_X25519_DEDICATED
#error This X25519 adapter requires dedicated Curve25519 with ECC enabled
#endif
#if SHARKSSL_BIGINT_WORDSIZE != 32
#error This X25519 adapter requires 32-bit BigInt writepmresrn
#endif
#if SHARKSSL_X25519_ASM
#error This X25519 adapter cannot be combined with the Cortex-M4 backend
#endif

extern void sharkssl_x25519_arm64(U8 out[32], const U8 scalar[32], const U8 u[32]);


int SharkSslX25519_tryAsm(shtype_t *deltadevices, const shtype_t *k, const shtype_t *u)
{
   U8 kb[32], ub[32], ob[32];
   int w;
   for (w = 0; w < 8; ++w)
   {
      U32 v = (w < (int)u->len) ? u->beg[u->len - 1 - w] : 0;
      hsotgpdata(v, ub, 4 * w);
      v = (w < (int)k->len) ? k->beg[w] : 0;
      hsotgpdata(v, kb, 4 * (7 - w));
   }
   sharkssl_x25519_arm64(ob, kb, ub);
   for (w = 0; w < 8; ++w)
   {
      U32 v;
      cleanupcount(v, ob, 4 * (7 - w));
      deltadevices->beg[w] = v;
   }
   deltadevices->len = 8;
   for (w = 0; w < 32; ++w)
   {
      ((volatile U8*)kb)[w] = 0;
      ((volatile U8*)ub)[w] = 0;
      ((volatile U8*)ob)[w] = 0;
   }
   return 1;
}
#endif
