# SharkSSL 64-bit Cortex-A GNU assembly {#port_arm64_gcc}

Experimental backend for GCC/Clang, little-endian AArch64, LP64 ELF targets
supported by the platform adapter.
It uses the existing SharkSSL hooks and an optional SHA-256 dispatch in
SharkSslCrypto.c. Public headers, context sizes and the GHASH table
representation are unchanged.
Files to compile alongside SharkSslCrypto.c:

```text
SharkSslCrypto_ARM64.c             CPU feature detection and hook adapter
SharkSslCrypto_ARM64_Gcm.S         AES-GCM bulk operation, four AES blocks
SharkSslCrypto_ARM64_Ghash.S       PMULL, NEON PMUL and integer GHASH
SharkSslCrypto_ARM64_ChaCha.S      Four-block NEON, short/integer scalar path
SharkSslCrypto_ARM64_Poly1305.S    Integer 5x26-limb multiplication/reduction
SharkSslCrypto_ARM64_Sha256.c      Optional SHA2 instruction block compressor
```

Compile `SharkSslBigInt.c` with `SHARKSSL_X25519_ASM_HOOK=0` for X25519.

Enable the algorithms normally, then set the desired integer options to 1:

```text
SHARKSSL_OPTIMIZED_GCM_ASM
SHARKSSL_OPTIMIZED_GHASH_ASM
SHARKSSL_OPTIMIZED_CHACHA_ASM
SHARKSSL_OPTIMIZED_POLY1305_ASM
```

Set an option to 0 to retain that C implementation. Normally enable GCM and
GHASH together. GCM-only and GHASH-only builds are also link-tested.
VAES is not part of this backend.

The former GNU X25519 assembly and its adapter were removed after paired,
CPU-pinned Pi 5 GCC tests measured 126.4 us for C versus 131.0 us for assembly
per shared-secret operation. Set `SHARKSSL_X25519_ASM_HOOK=0` for this backend.
The IAR X25519 assembly remains available because it beats IAR-generated C.

To enable hardware SHA-256, set SHARKSSL_ARM64_SHA256_INTRINSICS=1 for
SharkSslCrypto.c, SharkSslCrypto_ARM64.c and SharkSslCrypto_ARM64_Sha256.c.
Compile only the SHA-256 file with `-march=armv8-a+crypto`; keep the common C
and feature adapter at baseline `-march=armv8-a`. GCC 11 declares its SHA
intrinsics under `+crypto`, so `+sha2` alone does not compile with that
compiler. The SHA-256 helper uses SHA2 instructions only; this file-specific
flag does not require AES or PMULL at runtime. The adapter selects the
SHA2 instructions through the operating system's CPU capability flags and
retains the existing C compression routine on CPUs without SHA2. The SHA-256
context layout and public API do not change. Leave the option undefined or
set it to 0 when the SHA-256 file is not part of the build.

Pass identical SharkSSL configuration definitions to C and .S compilation.
Use the compiler driver to assemble .S, so preprocessing reads SharkSSL_cfg.h.
For GCM, pass -DSHARKSSL_GCM_M0=N to assembly:

```text
N=244 when AES-256 or SHARKSSL_NOPACK is enabled;
N=212 when AES-192 is the largest enabled key size;
N=180 otherwise (AES-128).
```

The C adapter derives the offset from its headers and checks it at compile
time. A layout-specific symbol makes an assembly/C offset mismatch fail to
link. Rebuild all objects when configuration changes, including AES unrolling.

Use SHARKSSL_AES_SMALL_FOOTPRINT=0 (the normal configuration). This backend
does not support small-footprint AES; the adapter rejects that configuration
at compile time.

The adapter uses operating-system CPU-feature information to select supported
instructions at runtime. Hardware AES-GCM requires ASIMD, AES and PMULL.
For GCM calls of at least 128 bytes, it encrypts four counters in parallel
and combines four carry-less GHASH products before one polynomial reduction.
Powers of H are prepared on the stack once per bulk call; no context or table
layout changes are needed. Short calls retain the single-block path.
With ASIMD alone, GHASH uses NEON byte-polynomial PMUL and
AES uses the existing C implementation. ChaCha uses ASIMD/NEON when exposed.
Without ASIMD, GHASH and ChaCha use integer assembly. Poly1305 remains integer
assembly.
Keep common C and the adapter compiled for baseline armv8-a. Optional crypto
instructions are confined to guarded assembly entry points. The C AES key
schedule, GCM setup and final partial block remain in the existing C code.
This does not establish that all AES processing is constant-time.

On a Pi 5, the baseline build selects the AES/PMULL path at runtime when
Linux reports all three capabilities. A Pi 5-only build may use
`-mcpu=cortex-a76+crypto` for C and assembly, but measured GCM throughput
did not improve with that flag. Do not use this whole-library flag for a
binary intended to run on older ARM64 processors without these extensions.

## Build the PSA provider

Build the
optimized provider on AArch64 Linux with:

```sh
make -C build PSA_CRYPTO=1 PSA_ARM64_GCC=1 ODIR=obj-psa-arm64
```

This option compiles the adapter and SHA-256 module with the common C, and the
GCM, GHASH, ChaCha20 and Poly1305 assembly with the same feature definitions.
Only the SHA-256 module uses `-march=armv8-a+crypto`; the other C sources use
baseline `-march=armv8-a`. The provider's relocatable link includes all six
ARM64 objects before section collection. The option assumes the default
AES-256 context layout (`SHARKSSL_GCM_M0=244`); pass
`SHARKSSL_GCM_M0=212` or `180` when using the corresponding AES layout and
rebuild all objects. Use a separate `ODIR` for portable C and ARM64 builds.
