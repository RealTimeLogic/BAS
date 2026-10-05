# SharkSSL 64-bit Cortex-A GNU assembly {#port_arm64_gcc}

Experimental backend for GCC/Clang, little-endian AArch64, LP64 ELF targets
supported by the platform adapter.
It uses the existing SharkSSL hooks. No changes to SharkSslCrypto.c, public
headers, context sizes, or the GHASH table representation are required.
Native Cortex-A72 testing on Raspberry Pi 4 passed with GCC 14.2. AES/PMULL
hardware validation is still pending: that board exposes neither extension.
Big-endian and ILP32 targets are not supported. The accompanying adapter must
support the target operating system's CPU-feature detection interface.

Files to compile alongside SharkSslCrypto.c:

```text
SharkSslCrypto_ARM64.c             CPU feature detection and hook adapter
SharkSslCrypto_ARM64_Gcm.S         AES-GCM bulk operation, four AES blocks
SharkSslCrypto_ARM64_Ghash.S       PMULL, NEON PMUL and integer GHASH
SharkSslCrypto_ARM64_ChaCha.S      Four-block NEON, short/integer scalar path
SharkSslCrypto_ARM64_Poly1305.S    Integer 5x26-limb multiplication/reduction
```

For X25519, compile alongside SharkSslBigInt.c:

```text
SharkSslX25519_ARM64.S            Integer four-limb X25519 ladder
SharkSslX25519_ARM64_Adapter.c    Existing generic X25519 hook adapter
```

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

For X25519, set SHARKSSL_X25519_ASM_HOOK=1 for C and assembly. This requires
ECC, Curve25519, SHARKSSL_X25519_DEDICATED=1, 32-bit BigInt words, and the
Cortex-M SHARKSSL_X25519_ASM option disabled. Set the hook to 0 for C X25519.
The assembly uses baseline Armv8-A MUL/UMULH; NEON and crypto extensions are
not required. Its private four-limb format does not change SharkSslBigInt.
All 256 scalar bits, short-input placement, aliases and coordinate masking
match the existing C hook. The public API retains clamping and zero-secret
checks. The kernel uses fixed loops and masked swaps, and clears its local
scalar/field storage. This is not a formal side-channel certification.
Do not define SHARKSSL_X25519_TEST in production. It exposes test-only helpers.

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
With ASIMD alone, GHASH uses NEON byte-polynomial PMUL and
AES uses the existing C implementation. ChaCha uses ASIMD/NEON when exposed.
Without ASIMD, GHASH and ChaCha use integer assembly. Poly1305 remains integer
assembly.
Keep common C and the adapter compiled for baseline armv8-a. Optional crypto
instructions are confined to guarded assembly entry points. The C AES key
schedule, GCM setup and final partial block remain in the existing C code.
This does not establish that all AES processing is constant-time.

## Performance and benefit

On Raspberry Pi 4 (Cortex-A72, native 64-bit Linux, GCC 14.2 with `-O2`),
16 KiB encryption benchmarks achieved about **1.9x the C throughput for
AES-128-GCM** and **1.7x for complete ChaCha20-Poly1305**. X25519 shared-secret
calculation improved from **270.78 us in C to 228.62 us with assembly**, about
**1.18x**. The cipher gains leave more CPU time for application work; X25519
offers a smaller reduction in key-exchange cost.

These operation timings do not predict whole-server speedups
or results on other ARM cores. The Pi lacks AES/PMULL extensions: the AES-GCM
result uses C AES with accelerated GHASH, not hardware AES. Performance on
processors with hardware AES/PMULL requires separate measurement.
