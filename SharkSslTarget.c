/* Preprocess only, with the selected compiler and target flags. Never execute.
 * Copyright (c) Real Time Logic LLC, 2026. All rights reserved.
 */
/* Respect explicit command-line and SharkSSL_opts.h optimization settings. */
#include "SharkSSL_opts.h"
#ifdef SHARKSSL_OPTIMIZED_GCM_ASM
SHARKSSL_MAKE_GCM_DEFINED
#if SHARKSSL_OPTIMIZED_GCM_ASM
SHARKSSL_MAKE_GCM_REQUIRED
#endif
#endif
#ifdef SHARKSSL_OPTIMIZED_GHASH_ASM
SHARKSSL_MAKE_GHASH_DEFINED
#if SHARKSSL_OPTIMIZED_GHASH_ASM
SHARKSSL_MAKE_GHASH_REQUIRED
#endif
#endif
#ifdef SHARKSSL_OPTIMIZED_GCM_VAES_ASM
SHARKSSL_MAKE_GCM_VAES_DEFINED
#if SHARKSSL_OPTIMIZED_GCM_VAES_ASM
SHARKSSL_MAKE_GCM_VAES_REQUIRED
#endif
#endif
#ifdef SHARKSSL_OPTIMIZED_CHACHA_ASM
SHARKSSL_MAKE_CHACHA_DEFINED
#if SHARKSSL_OPTIMIZED_CHACHA_ASM
SHARKSSL_MAKE_CHACHA_REQUIRED
#endif
#endif
#ifdef SHARKSSL_OPTIMIZED_POLY1305_ASM
SHARKSSL_MAKE_POLY1305_DEFINED
#if SHARKSSL_OPTIMIZED_POLY1305_ASM
SHARKSSL_MAKE_POLY1305_REQUIRED
#endif
#endif
#ifdef SHARKSSL_X25519_ASM_HOOK
SHARKSSL_MAKE_X25519_DEFINED
#if SHARKSSL_X25519_ASM_HOOK
SHARKSSL_MAKE_X25519_REQUIRED
#endif
#endif
#include "SharkSSL_cfg.h"

#if !SHARKSSL_USE_ECC || !SHARKSSL_ECC_USE_CURVE25519 || !SHARKSSL_X25519_DEDICATED || SHARKSSL_BIGINT_WORDSIZE != 32 || SHARKSSL_X25519_ASM
SHARKSSL_MAKE_X25519_UNAVAILABLE
#endif

#if defined(_MSC_VER) && !defined(_M_ARM64EC) && defined(_M_X64)
SHARKSSL_MAKE_TARGET_X64_MSVC
#elif defined(_MSC_VER) && !defined(_M_ARM64EC) && defined(_M_IX86)
SHARKSSL_MAKE_TARGET_X86_MSVC
#elif (defined(__GNUC__) || defined(__clang__)) && (defined(__linux__) || defined(__QNXNTO__)) && defined(__ELF__) && defined(__x86_64__) && defined(__LP64__) && !defined(__ILP32__)
SHARKSSL_MAKE_TARGET_X64_GCC
#elif (defined(__GNUC__) || defined(__clang__)) && (defined(__linux__) || defined(__QNXNTO__)) && !defined(__ANDROID__) && defined(__ELF__) && defined(__aarch64__) && defined(__LP64__) && !defined(__ILP32__) && defined(__AARCH64EL__) && !defined(B_BIG_ENDIAN)
SHARKSSL_MAKE_TARGET_ARM64_GCC
#endif

#if SHARKSSL_USE_AES_256 || SHARKSSL_NOPACK
SHARKSSL_MAKE_M0_244
#elif SHARKSSL_USE_AES_192
SHARKSSL_MAKE_M0_212
#else
SHARKSSL_MAKE_M0_180
#endif
