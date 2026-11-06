# SharkSSL 64-bit Cortex-A GNU assembly {#port_arm64_gcc}

This directory provides SharkSSL assembly for GCC/Clang on little-endian
AArch64 targets using the LP64 data model and ELF format. It uses the existing
SharkSSL interfaces without changing public headers or context sizes.

Compile the selected files alongside `SharkSslCrypto.c` and `SharkSslBigInt.c`:

```text
SharkSslCrypto_ARM64.c          CPU feature detection and hook adapter
SharkSslGcm_ARM64.S             AES-GCM with AES and PMULL, four blocks per pass
SharkSslGhash_ARM64.S           PMULL, NEON PMUL and integer GHASH
SharkSslChaCha_ARM64.S          Four-block NEON, short/integer scalar path
SharkSslPoly1305_ARM64.S        Two-lane NEON, integer 5x26-limb path
SharkSslSha256_ARM64.S          Optional SHA2 instruction block compressor
SharkSslSha512_ARM64.S          SHA-384 and SHA-512 on integer registers
SharkSslX25519_ARM64.S          X25519, two-lane NEON ladder
SharkSslX25519_ARM64_Adapter.c  Connects the X25519 hook to the assembly
SharkSslBigInt_ARM64.S          BigInt add, subtract, shift, multiply, Montgomery product
SharkSslP256_ARM64.S            P-256 field arithmetic
SharkSslP384_ARM64.S            P-384 field arithmetic
```

Enable the algorithms normally, then set the desired integer options to 1. Each
option selects its own files; set it to 0 to retain that C implementation.

| Option | Files |
| --- | --- |
| `SHARKSSL_OPTIMIZED_GCM_ASM` | `SharkSslGcm_ARM64.S`, `SharkSslGhash_ARM64.S` |
| `SHARKSSL_OPTIMIZED_GHASH_ASM` | `SharkSslGhash_ARM64.S` |
| `SHARKSSL_OPTIMIZED_CHACHA_ASM` | `SharkSslChaCha_ARM64.S` |
| `SHARKSSL_OPTIMIZED_POLY1305_ASM` | `SharkSslPoly1305_ARM64.S` |
| `SHARKSSL_ARM64_SHA256_INTRINSICS` | `SharkSslSha256_ARM64.S` |
| `SHARKSSL_OPTIMIZED_SHA384_ASM` | `SharkSslSha512_ARM64.S` |
| `SHARKSSL_X25519_ASM_HOOK` | `SharkSslX25519_ARM64.S`, `SharkSslX25519_ARM64_Adapter.c` |
| `SHARKSSL_OPTIMIZED_BIGINT_ASM` | `SharkSslBigInt_ARM64.S` |
| `SHARKSSL_ECC_P256_ASM` | `SharkSslP256_ARM64.S` |
| `SHARKSSL_ECC_P384_ASM` | `SharkSslP384_ARM64.S` |

Compile `SharkSslCrypto_ARM64.c` in every build that uses the AES-GCM, GHASH,
ChaCha20 or SHA-256 options. Normally enable GCM and GHASH together; GCM-only and
GHASH-only configurations are also supported. VAES is not part of this backend.

Pass identical SharkSSL configuration definitions to C and .S compilation.
Use the compiler driver to assemble .S, so preprocessing reads SharkSSL_cfg.h.
Keep common C and the adapter at baseline `-march=armv8-a`; the assembly files
select their instructions themselves. For GCM, pass -DSHARKSSL_GCM_M0=N to assembly:

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

## CPU selection

The adapter uses operating-system CPU-feature information to select supported
instructions at runtime. Hardware AES-GCM requires ASIMD, AES and PMULL. Without
them, AES-GCM uses the bitsliced AES of `SharkSslAesGcmCT.h`, with NEON PMUL GHASH
under ASIMD and integer GHASH otherwise. ChaCha20 uses ASIMD when exposed and
integer assembly otherwise. SHA-256 keeps its C routine on CPUs without SHA2. The
Poly1305 NEON path, SHA-512, the BigInt, P-256 and P-384 code, and the two-lane
X25519 need nothing beyond baseline AArch64, where ASIMD is part of the Linux ABI.

AES-GCM computes the powers of H from `M0[8]` on every call, so the context layout
does not change. The tag mask and partial blocks use the same AES instructions
when the CPU has them. A Pi 5 build needs no `-mcpu` flag: `-mcpu=cortex-a76+crypto`
for the whole library did not improve measured GCM throughput, and such a binary
does not run on older ARM64 processors.

## Constant-time behavior

Branches and memory addresses in these kernels depend only on public lengths, round
counts and loop counters. The AES-GCM key schedule and the C blocks of the same
contexts use the bitsliced S-box. Checked on a Pi 5 (Cortex-A76), with the kernels
reached directly and through the public API:

- Valgrind Memcheck with secret inputs marked undefined: AES-GCM (hardware, NEON and
  integer paths), the three GHASH routines, ChaCha20, Poly1305, SHA-256, SHA-512,
  X25519, P-256, P-384 and the Montgomery product report no secret-dependent branch
  or address; a positive control reports both of its errors.
- Welch t-tests of fixed against random secrets, 20,000 to 60,000 batches: X25519,
  P-256, P-384 and the Montgomery product stay within the usual threshold, with an
  isolated excursion on one run that did not repeat.
- `MUL` and `UMULH` take the same time over 18 operand pairs from 0 to all ones.

Other CPUs may time their instructions differently; this was measured on one core.
Armv8-A does not promise data-independent multiply timing without `DIT`, which the
Cortex-A76 does not implement. AES-CBC, CCM and the random-number generator still
use the table AES of `SharkSslCrypto.c`, which is outside this backend.

The AES-GCM, GHASH, ChaCha20, Poly1305, SHA and X25519 kernels clear the registers
and stack that held keys, keystream or field values before they return. The BigInt,
P-256 and P-384 routines are called per field operation: they clear their stack
but leave caller-saved registers to the next call, like the Cortex-M and RX ports.

## Performance

Measured on a Pi 5 at 2.4 GHz, one core, GCC 14 `-O2`, against the portable C of this
library and against OpenSSL 3.5 on the same board. Throughput is in MB/s for 16 KB
inputs; AES-GCM and ChaCha20-Poly1305 include the tag.

| Operation | C | This backend | OpenSSL 3.5 |
| --- | --- | --- | --- |
| AES-128-GCM | 38 | 1840 | 2286 |
| AES-256-GCM | 35 | 1537 | 1846 |
| ChaCha20-Poly1305 | 245 | 496 | 712 |
| SHA-256 | 246 | 1434 | 1456 |
| SHA-384 | 115 | 362 | 393 |

| Operation (microseconds) | C | This backend | OpenSSL 3.5 |
| --- | --- | --- | --- |
| X25519 shared secret | 126 | 98 | 164 |
| ECDSA P-256 sign, verify | 543, 1396 | 389, 840 | 56, 188 |
| ECDSA P-384 sign, verify | 1296, 3268 | 950, 2208 | 571, not measured |
| RSA-2048 public, private | 292, 9455 | 206, 5321 | 97, 3546 |

The Cortex-A76 executes a 32-bit multiply or a two-lane `UMLAL` per cycle, and needs
seven cycles for a `MUL` and `UMULH` pair. That is why Poly1305, X25519, P-256 and
P-384 use 32-bit products, and why only the long BigInt rows use 64-bit limbs.
Operation throughput does not directly predict TLS throughput.

## Build the PSA provider

The following command requires a full SharkSSL SDK checkout, including the
provider sources and its `build` directory. Run it from that checkout's root:

```sh
# Build the provider with the ARM64 backend in a separate object directory.
make -C build PSA_CRYPTO=1 PSA_ARM64_GCC=1 ODIR=obj-psa-arm64
```

This option compiles the adapters with the common C, and all the assembly above with
the same feature definitions, at baseline `-march=armv8-a`. The provider's
relocatable link includes the ARM64 objects before section collection. The option
assumes the default AES-256 context layout (`SHARKSSL_GCM_M0=244`); pass
`SHARKSSL_GCM_M0=212` or `180` when using the corresponding AES layout and rebuild
all objects. Use a separate `ODIR` for portable C and ARM64 builds.
