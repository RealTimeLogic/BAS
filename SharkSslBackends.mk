# Backend descriptions. Shared selection/recipes live in SharkSslAsm.mk.
# Copyright (c) Real Time Logic LLC, 2026. All rights reserved.
# Each entry supplies DIR, DRIVER (cc or masm), ASM, C, DEFAULTS and AUTO.
# Backends may also add common CFLAGS and source-specific SHA2 CFLAGS.
# AUTO=0 keeps a detected backend opt-in until target runtime acceptance.
SHARKSSL_BACKENDS ?= X64_MSVC X86_MSVC X64_GCC ARM64_GCC

SHARKSSL_X64_MSVC_DIR := X64_MSVC
SHARKSSL_X64_MSVC_DRIVER := masm
SHARKSSL_X64_MSVC_TOOL := ml64
SHARKSSL_X64_MSVC_ASFLAGS = /DSHARKSSL_GCM_M0=$(SHARKSSL_M0)
SHARKSSL_X64_MSVC_ASM := SharkSslCrypto_X64.asm SharkSslCrypto_X64_AVX2.asm SharkSslCrypto_X64_Poly1305.asm SharkSslCrypto_X64_VAES.asm
SHARKSSL_X64_MSVC_C := SharkSslCrypto_X64_CPUID.c SharkSslCrypto_X64_VAES_CPUID.c
SHARKSSL_X64_MSVC_DEFAULTS := CHACHA POLY1305 GCM GHASH GCM_VAES
SHARKSSL_X64_MSVC_AUTO := 1

SHARKSSL_X86_MSVC_DIR := X86_MSVC
SHARKSSL_X86_MSVC_DRIVER := masm
SHARKSSL_X86_MSVC_TOOL := ml
SHARKSSL_X86_MSVC_ASFLAGS = /coff /safeseh /DSHARKSSL_GCM_M0=$(SHARKSSL_M0)
SHARKSSL_X86_MSVC_ASM := SharkSslCrypto_X86.asm SharkSslCrypto_X86_AVX2.asm SharkSslCrypto_X86_Poly1305.asm
SHARKSSL_X86_MSVC_C := SharkSslCrypto_X86_CPUID.c
SHARKSSL_X86_MSVC_DEFAULTS := CHACHA POLY1305 GCM GHASH
SHARKSSL_X86_MSVC_AUTO := 1

SHARKSSL_X64_GCC_DIR := X64_GCC
SHARKSSL_X64_GCC_DRIVER := cc
SHARKSSL_X64_GCC_ASFLAGS = -DSHARKSSL_GCM_M0=$(SHARKSSL_M0)
SHARKSSL_X64_GCC_ASM := SharkSslCrypto_X64.S SharkSslCrypto_X64_AVX2.S SharkSslCrypto_X64_Poly1305.S SharkSslCrypto_X64_VAES.S
SHARKSSL_X64_GCC_C := SharkSslCrypto_X64_CPUID.c
SHARKSSL_X64_GCC_DEFAULTS := CHACHA POLY1305 GCM GHASH GCM_VAES
SHARKSSL_X64_GCC_AUTO := 1
SHARKSSL_X64_GCC_COMPILE_CHECK := 1
# Older assemblers support AES-NI/AVX2 but not 256-bit VAES/VPCLMULQDQ.
# Optional features are compile-probed with the selected target compiler.
SHARKSSL_X64_GCC_OPTIONAL := GCM_VAES
SHARKSSL_X64_GCC_GCM_VAES_ASM := SharkSslCrypto_X64_VAES.S

# AArch64 Linux/QNX: compile-check configured kernels and C adapters before inclusion.
# Optional CPU instructions remain behind runtime feature checks.
# Cortex-M ports remain standalone SharkSSL ports, not Mako build targets.
SHARKSSL_ARM64_GCC_DIR := ARM64_GCC
SHARKSSL_ARM64_GCC_DRIVER := cc
SHARKSSL_ARM64_GCC_ASFLAGS = -DSHARKSSL_GCM_M0=$(SHARKSSL_M0)
SHARKSSL_ARM64_GCC_ASM := SharkSslCrypto_ARM64_Gcm.S SharkSslCrypto_ARM64_Ghash.S SharkSslCrypto_ARM64_ChaCha.S SharkSslCrypto_ARM64_Poly1305.S
SHARKSSL_ARM64_GCC_C := SharkSslCrypto_ARM64.c SharkSslCrypto_ARM64_Sha256.c
SHARKSSL_ARM64_GCC_CFLAGS := -DSHARKSSL_ARM64_SHA256_INTRINSICS=1
SHARKSSL_ARM64_GCC_SHA2_C := SharkSslCrypto_ARM64_Sha256.c
# GCC 11 groups SHA intrinsics under +crypto; apply only to the SHA helper.
SHARKSSL_ARM64_GCC_SHA2_CFLAGS := -march=armv8-a+crypto
SHARKSSL_ARM64_GCC_DEFAULTS := CHACHA POLY1305 GCM GHASH
SHARKSSL_ARM64_GCC_AUTO := 1
SHARKSSL_ARM64_GCC_COMPILE_CHECK := 1
