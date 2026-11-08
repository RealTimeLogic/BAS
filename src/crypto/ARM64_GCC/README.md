# SharkSSL 64-bit Cortex-A GNU assembly {#port_arm64_gcc}

This directory provides SharkSSL assembly for GCC/Clang on little-endian
AArch64 targets using the LP64 data model and ELF format. It uses the existing
SharkSSL interfaces without changing public headers or context sizes.

Compile the selected files alongside `SharkSslCrypto.c` and `SharkSslBigInt.c`:

```text
SharkSslCrypto_ARM64.c          CPU feature detection and hook adapter
SharkSslGcm_ARM64.S             AES-GCM with AES and PMULL, four blocks per pass
SharkSslGhash_ARM64.S           PMULL and NEON PMUL GHASH
SharkSslAes_ARM64.S             AES block cipher for ECB, CBC, CTR and CCM, with AES
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
| `SHARKSSL_OPTIMIZED_AES_ASM` | `SharkSslAes_ARM64.S` |
| `SHARKSSL_OPTIMIZED_CHACHA_ASM` | `SharkSslChaCha_ARM64.S` |
| `SHARKSSL_OPTIMIZED_POLY1305_ASM` | `SharkSslPoly1305_ARM64.S` |
| `SHARKSSL_ARM64_SHA256_INTRINSICS` | `SharkSslSha256_ARM64.S` |
| `SHARKSSL_OPTIMIZED_SHA384_ASM` | `SharkSslSha512_ARM64.S` |
| `SHARKSSL_X25519_ASM_HOOK` | `SharkSslX25519_ARM64.S`, `SharkSslX25519_ARM64_Adapter.c` |
| `SHARKSSL_OPTIMIZED_BIGINT_ASM` | `SharkSslBigInt_ARM64.S` |
| `SHARKSSL_ECC_P256_ASM` | `SharkSslP256_ARM64.S` |
| `SHARKSSL_ECC_P384_ASM` | `SharkSslP384_ARM64.S` |

Compile `SharkSslCrypto_ARM64.c` in every build that uses the AES, AES-GCM, GHASH,
ChaCha20 or SHA-256 options. Enable GCM and GHASH together; SharkSSL_cfg.h rejects
one without the other. VAES is not part of this backend.

`SHARKSSL_OPTIMIZED_AES_ASM` accelerates `SharkSslAesCtx`, the block API, CBC, CTR
and CCM, independently of the AES-GCM options. `SharkSslAes_ARM64.S` needs no AES
layout definition: C passes the round keys and the round count. It loads the keys
once per call and processes four blocks per pass for ECB, CBC decryption and CTR;
CCM encrypts the CBC-MAC block and the counter block of each complete block together.

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
instructions at runtime. Hardware AES-GCM and the AES block cipher require ASIMD,
AES and PMULL; the constructor of an AES context stores the key schedule for the
path the CPU selects. Without
them, AES-GCM runs the software AES-GCM of `SharkSslCrypto.c`: the bitsliced AES of
`SharkSslAesGcmCT.h` with the NEON GHASH of the same file when `SHARKSSL_AES_CT_NEON`
is on (below), otherwise two counter blocks per call with the NEON PMUL GHASH of
`SharkSslGhash_ARM64.S` under ASIMD and the constant-time C GHASH without it.
ChaCha20 uses ASIMD when exposed and integer assembly otherwise. SHA-256 keeps its
C routine on CPUs without SHA2. The
Poly1305 NEON path, SHA-512, the BigInt, P-256 and P-384 code, and the two-lane
X25519 need nothing beyond baseline AArch64, where ASIMD is part of the Linux ABI.

Without AES-CE the bitsliced AES also runs on NEON: `SHARKSSL_AES_CT_NEON` (in
`SharkSSL_cfg.h`, default 1 for AArch64 compilers with Advanced SIMD) evaluates the
same fixsliced circuit on the four 32-bit lanes of the NEON registers, eight blocks
per call, for CTR, CBC decryption and the software AES-GCM when there are three or
more blocks. The software AES-GCM also hashes on NEON: a carry-less multiplier built
from PMULL.8B (`vmull_p8`, base Advanced SIMD, not the crypto extension), Karatsuba,
and H to H^4 in the context so that four blocks share one reduction. It is C with
`arm_neon.h` intrinsics inside `SharkSslCrypto.c`: no file to add, and it does not
depend on `SHARKSSL_OPTIMIZED_*_ASM`. ECB single blocks, CBC encryption and CCM are
serial and keep the two-block scalar code.

The hardware AES-GCM computes the powers of H from `M0[8]` on every call, so the
context layout does not change. The AES-GCM constructor stores the FIPS 197 or the
bitsliced round keys, and H or the basis of the C GHASH, for the path the CPU
selects: as with the AES block cipher, a context must stay on the kind of processor
that built it. The tag mask and partial blocks use the same AES instructions
when the CPU has them. A Pi 5 build needs no `-mcpu` flag: `-mcpu=cortex-a76+crypto`
for the whole library did not improve measured GCM throughput, and such a binary
does not run on older ARM64 processors.

## Constant-time behavior

Branches and memory addresses in these kernels depend only on public lengths, round
counts and loop counters. The AES-GCM key schedule and the C blocks of the same
contexts use the bitsliced S-box. Checked on a Pi 5 (Cortex-A76), with the kernels
reached directly and through the public API:

- Valgrind Memcheck with secret inputs marked undefined: AES-GCM (hardware path,
  and the one-block NEON and integer paths since replaced by the software AES-GCM),
  the PMULL, NEON and integer GHASH routines (the integer one since removed),
  ChaCha20, Poly1305, SHA-256, SHA-512,
  X25519, P-256, P-384 and the Montgomery product report no secret-dependent branch
  or address; a positive control reports both of its errors.
- Welch t-tests of fixed against random secrets, 20,000 to 60,000 batches: X25519,
  P-256, P-384 and the Montgomery product stay within the usual threshold, with an
  isolated excursion on one run that did not repeat.
- `MUL` and `UMULH` take the same time over 18 operand pairs from 0 to all ones.

Other CPUs may time their instructions differently; this was measured on one core.
Armv8-A does not promise data-independent multiply timing without `DIT`, which the
Cortex-A76 does not implement. AES-CBC, CTR and CCM use AES-CE through
`SharkSslAes_ARM64.S` with `SHARKSSL_OPTIMIZED_AES_ASM`, otherwise the
constant-time bitsliced AES of `SharkSslAesGcmCT.h`. The Valgrind and t-test
results above predate `SharkSslAes_ARM64.S` and do not cover it; its branches
depend only on the round count and the length, and it clears v0-v7 and v16-v31.

The AES-GCM, GHASH, ChaCha20, Poly1305, SHA and X25519 kernels clear the registers
and stack that held keys, keystream or field values before they return. The BigInt,
P-256 and P-384 routines are called per field operation: they clear their stack
but leave caller-saved registers to the next call, like the Cortex-M and RX ports.

The software AES-GCM, with AES and PMULL cleared and with ASIMD also cleared, was
checked again with Valgrind Memcheck (key, plaintext, AAD and tag undefined): the
only report is the accept/reject branch on the tag check, as in the portable C.
The same holds with `SHARKSSL_AES_CT_NEON` for AES-GCM, CTR and CBC decryption
(key and data undefined), with the NEON AES and the NEON GHASH: fixed shifts,
bitwise operations, PMULL.8B and fixed addresses only. PMULL.8B is assumed to take
the same time for every operand, like the PMUL of the assembler GHASH.

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

Without AES and PMULL (HWCAP cleared on the same Pi 5), AES-128-GCM runs at 135 MB/s
(NEON bitsliced AES and NEON GHASH), CTR at 179 MB/s and CBC decryption at 156 MB/s.
The NEON code is chosen at compile time, so clearing ASIMD changes nothing here.
Before `SHARKSSL_AES_CT_NEON` these were 44, 65 and 56 MB/s, and the one-block
assembler loop before r6794 took 20 MB/s for AES-GCM.

Raspberry Pi 4 (Cortex-A72 at 1.5 GHz, no AES, PMULL or SHA2), GCC 13 `-O2 -static`,
one core, MB/s for 16 KB:

| Operation | C, NEON off | C (default, NEON AES) | This backend |
| --- | --- | --- | --- |
| AES-128-CTR | 29.0 | 71.8 | 72.2 |
| AES-128-CBC decrypt | 19.3 | 60.9 | 61.2 |
| AES-128-CBC encrypt, CCM | 15.6, 15.1 | 15.6, 15.1 | 15.5, 15.0 |
| AES-128-GCM | 13.7 | 53.2 | 53.1 |
| AES-256-GCM | 11.9 | 42.2 | 42.2 |
| ChaCha20, Poly1305 | 142, 340 | 142, 340 | 291, 681 |
| SHA-256 | 128 | 128 | 127 (no SHA2) |
| SHA-384 | 55 | 55 | 171 |

Per block on the A72: NEON AES-128 182 ns (two-block scalar code 509 ns); GHASH
68 ns with the NEON GHASH over four blocks, 85 ns for a single block, against 284 ns
for the assembler PMUL GHASH and 594 ns for the C GHASH. Before r6794 the backend ran
AES-128-GCM at 8.8 MB/s on this board.

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

## AES block cipher checklist (`SHARKSSL_OPTIMIZED_AES_ASM`)

- It covers `SharkSslAesCtx`: the block API, CBC, CTR and CCM. SharkSSL and BAS use
  these for encrypted PEM keys (CBC), encrypted ZIP files and the form authenticator
  (CTR), `ba.crypto.symmetric` in Lua (CBC, CCM) and the PSA provider (cipher, CCM,
  key wrap). TLS records do not use it: SharkSSL has no CBC or CCM cipher suites, and
  TLS uses AES-GCM and ChaCha20-Poly1305.
- AES-GCM is a separate choice (`SHARKSSL_OPTIMIZED_GCM_ASM` and
  `SHARKSSL_OPTIMIZED_GHASH_ASM`, always together). Both can be enabled; neither
  needs the other.
- Link `SharkSslAes_ARM64.S` whenever the option is 1, otherwise the `sharkssl_aes_hw_*`
  symbols are unresolved. Linking it with the option at 0 is harmless.
- The file needs no `SHARKSSL_GCM_M0` or other definition: C passes the round keys
  and the round count. Passing `SHARKSSL_GCM_M0` anyway is harmless.
- The CPU check is `sharkssl_arm64_capabilities()` in `SharkSslCrypto_ARM64.c`
  (compile it, as for the other ARM64 options). The AES path needs ASIMD, AES
  and PMULL, the same capability as hardware AES-GCM: a CPU with AES but without
  PMULL uses the C AES. On QNX the flags of all CPUs are intersected.
- Without the AES instructions the constant-time bitsliced AES of
  `SharkSslAesGcmCT.h` runs instead. The constructor stores the key schedule for
  the path the processor selects, so a context must stay on the kind of processor
  that built it; never copy context bytes to a machine with different AES support.
- `SHARKSSL_PSA_CONSUMER_STANDALONE_AES` and this option exclude each other
  (`#error`).
- `SHARKSSL_AES_SOFTWARE_CT` and `SHARKSSL_AES_GCM_SOFTWARE_CT` no longer exist:
  defining either as 0 stops the build with an `#error`. Remove them from old
  project files.
- The file sets `.arch armv8-a+crypto` itself: compile common C for baseline
  Armv8-A, without `+crypto`, so the image still runs on CPUs without AES.
- Little-endian LP64 only. The functions use v0-v7 and v16-v31 (never the
  callee-saved v8-v15), load the round keys once per call and clear the
  registers before they return.
- In BAS, `SharkSslBackends.mk` adds the file and the option for ARM64_GCC.
- Checked on a Raspberry Pi 5: differential test with AES-CE and with the AES
  capability cleared, ASan/UBSan, and Mako TLS with BAS.
