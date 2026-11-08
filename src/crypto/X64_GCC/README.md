# SharkSSL x86-64 GCC/Clang assembly {#port_x64_gcc}

This port uses GNU assembly syntax and the System V AMD64 ABI for
x86-64, LP64 ELF targets supported by the platform adapter. It provides
AES-GCM/GHASH, optional VAES, the AES block cipher for CBC, CTR and CCM,
ChaCha20, Poly1305, and X25519 through BMI2/ADX
integer instructions. The nine files below implement those operations; this directory does not
provide a generic BigInt backend.

## Build integration

Keep `SharkSslCrypto.c` and `SharkSslBigInt.c`, then add the files for the
algorithms you want to accelerate:

| File | Purpose |
| --- | --- |
| `SharkSslCrypto_X64.S` | AES-NI GCM and GHASH |
| `SharkSslCrypto_X64_VAES.S` | Optional VAES GCM |
| `SharkSslAes_X64.S` | AES-NI block cipher for ECB, CBC, CTR and CCM, with its AES-NI probe |
| `SharkSslCrypto_X64_AVX2.S` | ChaCha20 |
| `SharkSslCrypto_X64_Poly1305.S` | Poly1305 |
| `SharkSslCrypto_X64_CPUID.c` | CPU and OS feature probes for AES-NI, ChaCha20, and Poly1305 |
| `SharkSslCrypto_X64_VAES_CPUID.c` | CPU and OS feature probe for the VAES kernel |
| `SharkSslX25519_X64.S` | X25519 field arithmetic, scalar multiplication, and BMI2/ADX detection |
| `SharkSslX25519_X64_Adapter.c` | Connects the common X25519 hook to the assembly and preserves C fallback |

Enable the algorithms in the usual SharkSSL configuration and select the
corresponding hooks:

```c
/* Apply the same feature definitions to C and assembly sources. */
#define SHARKSSL_OPTIMIZED_GCM_ASM 1
#define SHARKSSL_OPTIMIZED_GHASH_ASM 1
#define SHARKSSL_OPTIMIZED_CHACHA_ASM 1
#define SHARKSSL_OPTIMIZED_POLY1305_ASM 1
#define SHARKSSL_OPTIMIZED_GCM_VAES_ASM 1 /* optional */
#define SHARKSSL_OPTIMIZED_AES_ASM 1
#define SHARKSSL_X25519_ASM_HOOK 1
```

Enable GCM and GHASH together for runtime dispatch; SharkSSL_cfg.h rejects one
without the other. A processor without SSSE3, AES-NI and PCLMULQDQ
uses the constant-time bitsliced AES-GCM of `SharkSslAesGcmCT.h`. Set an unwanted option to
0. If the assembler lacks VAES/VPCLMULQDQ support, omit the two VAES files and
set its option to 0.

| Build choice | Files to include |
| --- | --- |
| AES-GCM with AES-NI | `SharkSslCrypto_X64.S`, `SharkSslCrypto_X64_CPUID.c` |
| AES-GCM with VAES and AES-NI fallback | AES-NI files plus `SharkSslCrypto_X64_VAES.S`, `SharkSslCrypto_X64_VAES_CPUID.c` |
| AES block API, CBC, CTR and CCM with AES-NI | `SharkSslAes_X64.S` |
| ChaCha20-Poly1305 | `SharkSslCrypto_X64_AVX2.S`, `SharkSslCrypto_X64_Poly1305.S`, `SharkSslCrypto_X64_CPUID.c` |
| X25519 only | `SharkSslX25519_X64.S`, `SharkSslX25519_X64_Adapter.c` |

Include `SharkSslCrypto_X64_CPUID.c` only once.

The AES block cipher of `SharkSslAesCtx` (the block API, CBC, CTR and CCM) is a
separate option, `SHARKSSL_OPTIMIZED_AES_ASM`, in `SharkSslAes_X64.S`, which has its own
AES-NI probe and needs no AES layout definition: C passes the round keys and
the round count. It does not change AES-GCM, which keeps its own options. A
processor without AES-NI uses the constant-time bitsliced AES of
`SharkSslAesGcmCT.h` at run time; the constructor stores the key schedule for
the path the processor selects.

Assemble uppercase `.S` files through GCC or Clang with the same configuration
definitions and include paths as C. Pass `-DSHARKSSL_GCM_M0=N` to the two AES-GCM
assembly files: 244 for AES-256 or `SHARKSSL_NOPACK`, 212 when AES-192 is the
largest enabled key size, otherwise 180 for AES-128. The ChaCha20, Poly1305,
and X25519 files need no AES layout definition.
Compile common C and the CPU probes for the baseline CPU, without globally
enabling AVX2, BMI2, or ADX, or using `-march=native` for a portable executable.

## Configure X25519

The adapter requires `SHARKSSL_USE_ECC`, `SHARKSSL_ECC_USE_CURVE25519`, and
`SHARKSSL_X25519_DEDICATED` set to 1 (the shipped defaults), and
`SHARKSSL_X25519_ASM` undefined or 0; that option selects the Cortex-M4 backend.
Compile the adapter with SharkSSL's `src` and `inc` directories in the include
path. The X25519 hook is independent of the full BigInt backend. Keep the full
selector at 0 unless linking SharkSslBigInt_X64.S as well.

`SharkSslBigInt_X25519_mult` calls `SharkSslX25519_tryAsm` when
`SHARKSSL_X25519_ASM_HOOK` is 1. The adapter handles representation conversion,
the CPU check, and its scratch cleanup. A CPU without BMI2/ADX continues through
the existing C implementation.

Defining `SHARKSSL_X25519_TEST` for the assembler exports internal field
arithmetic entry points for regression tests. Leave it undefined in application
builds.

## Runtime selection

AES-NI GCM requires SSSE3, AES-NI, and PCLMULQDQ; the AES block cipher
requires AES-NI. AVX2 paths also require OS
support for XMM/YMM state. VAES additionally requires VAES and VPCLMULQDQ.
X25519 requires BMI2 and ADX and no AVX OS state.
The shared dispatcher checks the features before selecting accelerated paths
and retains C fallback. CPU brand alone does not determine support.

Each kernel clears its stack frame and the volatile XMM/YMM registers that held
keys, keystream or field values before it returns. Branches and memory addresses
depend only on public lengths, round counts and loop counters.

## Measure application performance

Measure the selected assembly and C paths with your target CPU, compiler,
message sizes, and algorithm configuration. Operation throughput does not
directly predict TLS or application throughput. Runtime dispatch retains
the available fallback when the required processor features are absent.

## AES block cipher checklist (`SHARKSSL_OPTIMIZED_AES_ASM`)

- It covers `SharkSslAesCtx`: the block API, CBC, CTR and CCM. SharkSSL and BAS use
  these for encrypted PEM keys (CBC), encrypted ZIP files and the form authenticator
  (CTR), `ba.crypto.symmetric` in Lua (CBC, CCM) and the PSA provider (cipher, CCM,
  key wrap). TLS records do not use it: SharkSSL has no CBC or CCM cipher suites, and
  TLS uses AES-GCM and ChaCha20-Poly1305.
- AES-GCM is a separate choice (`SHARKSSL_OPTIMIZED_GCM_ASM` and
  `SHARKSSL_OPTIMIZED_GHASH_ASM`, always together). Both can be enabled; neither
  needs the other.
- Link `SharkSslAes_X64.S` whenever the option is 1, otherwise the `sharkssl_aes_hw_*`
  symbols are unresolved. Linking it with the option at 0 is harmless.
- The file needs no `SHARKSSL_GCM_M0` or other definition: C passes the round keys
  and the round count. Passing `SHARKSSL_GCM_M0` anyway is harmless.
- The file has its own AES-NI probe (`sharkssl_aes_hw_probe`, CPUID leaf 1),
  whose result C caches; `SharkSslCrypto_X64_CPUID.c` is not needed for this option.
- Without the AES instructions the constant-time bitsliced AES of
  `SharkSslAesGcmCT.h` runs instead. The constructor stores the key schedule for
  the path the processor selects, so a context must stay on the kind of processor
  that built it; never copy context bytes to a machine with different AES support.
- `SHARKSSL_PSA_CONSUMER_STANDALONE_AES` and this option exclude each other
  (`#error`).
- `SHARKSSL_AES_SOFTWARE_CT` and `SHARKSSL_AES_GCM_SOFTWARE_CT` no longer exist:
  defining either as 0 stops the build with an `#error`. Remove them from old
  project files.
- The functions use only xmm0-xmm5 and the volatile System V registers, and clear
  the XMM registers before they return.
- In BAS, `examples/MakoServer/make/SharkSslBackends.mk` adds the file and the
  option with `SHARKSSL_ASM=auto`; `SHARKSSL_ASM=off` or an explicit
  `-DSHARKSSL_OPTIMIZED_AES_ASM=0` disables it.
- Checked: differential test against the C AES and OpenSSL (also with the probe
  forced to 0, ASan/UBSan, Clang) and Mako TLS with BAS on WSL x64.

## SHA-256 with SHA-NI

Set `SHARKSSL_OPTIMIZED_SHA256_ASM=1` in the C build and link
`SharkSslSha256_X64.S`. Its CPU probe is self-contained; the other cipher CPUID
files are not needed for this option. Keep the C build at the baseline
instruction set: the SHA instructions are confined to the assembly file.

The native SHA-256 compressor selects SHA-NI only when CPUID reports SHA,
SSSE3 and SSE2. It uses legacy XMM instructions, not AVX, so it needs no
AVX/XGETBV check. Otherwise it keeps the constant-time C compressor, including
the `SHARKSSL_SHA256_SMALL_FOOTPRINT` variant. The public context and API do
not change. PSA consumers continue to use their selected provider.

This is an original implementation derived from FIPS 180-4 section 6.2.2
and the published Intel instruction semantics for SHA256RNDS2, SHA256MSG1
and SHA256MSG2. No third-party SHA implementation was imported. Round
constants are the existing SharkSSL FIPS constants passed by C. The kernel
updates only `state[8]`; a C compile-time check verifies its offset.
Memory offsets and the round schedule are fixed; dispatch depends only
on public CPU capabilities. Temporary SIMD registers and scratch storage
are cleared before returning. The implementation does not claim resistance
to power or electromagnetic analysis, or formal FIPS module validation.

Specifications:
- https://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.180-4.pdf
- https://www.intel.com/content/dam/develop/external/us/en/documents/intel-sha-extensions-white-paper-402097.pdf

### SHA-NI verification

On 7 November 2026, GCC 13.3 and MSVC 19.51 x64/x86 passed 18 host profiles:
pure C, SHA-NI, forced C fallback, small-footprint/alignment variants, and
x64 AddressSanitizer (GCC also UndefinedBehaviorSanitizer). Each profile
passed 4823 independent SHA-256/HMAC/Hash_DRBG cases; hardware profiles
also passed 256 arbitrary-state compression cases and dispatch counters.
The C/ASM MSVC PSA provider projects also passed their SHA-256/384 tests.

For this GCC x64 backend on an AMD Ryzen 7 5700U, hashing 1 MiB
through the public API measured 352.7 MB/s in C and 1050.6 MB/s
with SHA-NI (2.98 times). These are medians of five alternating pairs,
not instrumented or sanitizer builds, with SMALL_FOOTPRINT=0 and
UNALIGNED_ACCESS=1. Other CPUs and workloads need their own measurements.

All four compression kernels have fixed instruction/address sequences.
Valgrind 3.22 could verify the C and forced fallback with secret state/input
but could not decode SHA256RNDS2; no dynamic Valgrind pass is claimed for
SHA-NI. These checks do not constitute formal module certification.


## SHA-384 and SHA-512 with BMI2 (committed r6795)

Set `SHARKSSL_OPTIMIZED_SHA384_ASM=1` and link `SharkSslSha512_X64.S`.
The existing option remains 0 by default. The native compressor checks
CPUID leaf 7 subleaf 0 EBX bit 8 and caches the public BMI2 result; without
BMI2 it retains the constant-time C compressor. RORX operates on integer
registers: no AVX/OSXSAVE/XGETBV requirement or global compiler ISA flag is
needed. The object includes its own probe. Both algorithms share this kernel;
the public API and context are unchanged. Other architectures retain their
existing paths; PSA consumers delegate to their selected provider.

Original implementation from FIPS 180-4 section 6.4.2 and Intel SDM RORX
semantics, with no third-party SHA implementation imported:
https://doi.org/10.6028/NIST.FIPS.180-4
https://www.intel.com/content/www/us/en/support/articles/000102715/processors/intel-xeon-processors.html

The 80 rounds are unrolled, with a 16-word schedule, fixed addresses and no
secret-dependent branch. Written schedule storage (128 bytes) and working
registers are erased before restoring the caller's nonvolatile registers.
The kernel is about 13 KiB of code and uses a 200-byte frame below entry rsp.
GCC System V and MSVC Windows x64 ABI/unwind metadata are supplied.

Verification on 7 November 2026: 18 GCC 13.3/MSVC 19.51 x64 profiles PASS,
including C, real BMI2, forced C fallback, ASan (GCC also UBSan), alignment
0/1 and SHA-384-only/SHA-512-only. Full profiles check 18,258 independent
SHA/HMAC results; real ASM adds 1,024 arbitrary-state compressions with
input/context/constants canaries. Single-algorithm profiles check 10,153
results. A full real-ASM profile counts 889,314 kernel calls and one cached
probe; forced fallback counts zero kernel calls. GNU and MASM cores have
identical disassembled instructions after ABI setup.

Valgrind 3.22 on GCC x64: zero reports for C, direct BMI2 and forced fallback,
64 secret-state/block compressions each; positive control: 64 reports.
This checks executed branches/addresses, not every CPU's timing, power/EM or
formal FIPS module validation. No Welch campaign or physical BMI2-free CPU
test is claimed. Nine preprocessing profiles are unchanged on non-x64 or
disabled/consumer paths; these are not cross runtime tests.

All 48 test_all solution builds pass with zero warnings/errors; the kernel
is included only in the 12 x64 ASM builds. Four MSVC PSA provider C/ASM
x64/Win32 builds pass SHA-256/384 tests and 36 extra SHA-384/512 hash checks.
No new board or end-to-end TLS test is claimed.

Benchmark, Ryzen 7 5700U, medians of five alternating C/BMI2 pairs, GCC -O3
or MSVC /O2 /GS; 1 MiB messages: SHA384 168.75 -> 506.23 MB/s (3.00x); SHA512 169.18 -> 532.09 MB/s (3.15x).
Same hash-only harness, UNALIGNED_ACCESS=1, AES_GCM=0, without sanitizers or
functional instrumentation. Raw samples and runners are in the task's
sha512-asm artifact directory. The SHA-256 Hash_DRBG is unchanged.

## P-256 field arithmetic with BMI2 (committed r6799)

Set `SHARKSSL_ECC_P256_ASM=1` in the C build and link `SharkSslP256_X64.S`.
The existing option remains 0 by default. The object contains its own CPUID
probe. CPUID 7.0 EBX bit 8 selects BMI2 MULX; otherwise the existing C CT
field arithmetic runs. No ADX, AVX, OSXSAVE, XGETBV, public API/context change
or global compiler ISA option is needed. Keep C compiled for the baseline CPU.
The flag requires the object even on a CPU that will select C fallback.

This is original arithmetic derived from the P-256 modulus in
[NIST SP 800-186](https://doi.org/10.6028/NIST.SP.800-186),
`p = 2^256 - 2^224 + 2^192 + 2^96 - 1`, and public MULX semantics.
No third-party implementation was imported. Four 64-bit limbs form the product;
fixed signed radix-2^32 folding and one conditional subtraction produce a
canonical result. Bounds and the two carry-fold argument are in the source;
an independent integer model covers all 65,536 coefficient extrema and
100,000 random 512-bit inputs.

The private raw entries are `sharkssl_p256_mulmod64`,
`sharkssl_p256_addmod64`, `sharkssl_p256_submod64`, and
`sharkssl_p256_hw_probe`. Operands/results are eight native U32 words,
most significant first, in [0,p). Exact r==a, r==b and a==b aliases work.
Applications use the normal SharkSSL APIs. C wrappers preserve the BigInt
result contract (beg=mem+8, len=8); public atomic CPU dispatch guards all four
field call sites, including the fixed Fermat inversion chain.

This accelerates P-256 arithmetic modulo the field prime, not modulo the
scalar order. Point formulas, scalar control and ECDSA order arithmetic remain
C. Full ECDSA/ECDHE API classifications retain their documented residuals.
P-384, Brainpool, x86 and other ports retain their existing implementations.

The arithmetic kernels have no jumps/calls and fixed memory addresses.
GNU/MASM cores disassemble identically after ABI setup. Multiplication uses
16 MULX, erases 128 written scratch bytes, and uses a 200-byte frame below
entry rsp; add/sub frames are 72 bytes. Working registers are cleared before
restoring saved registers. System V CFI is supplied.

Verification: 20 GCC/MSVC profiles PASS (83,084 checks): C, real BMI2 and forced C fallback, normal and sanitizer builds, no-RSA, standalone ECDSA-only, verify-only and P-256-only. Full profiles pass 4,157 checks: 4,096 field results from 1,024 pairs, 23 scalar points, 21 ECDH cases, five fixed-nonce signatures, ten valid/invalid verification cases and a randomized signature independently checked by Python. Standalone ECDSA-only passes 4,136 checks. Two independent P-384 2G KATs (GCC/MSVC) pass with P-256 ASM enabled and zero P-256 kernel calls. Counters confirm real ASM and forced fallback selection.

Direct GNU normal and ASan/UBSan runs each pass 948,447 operation/alias
checks (105,383 vectors x 3 operations x 3 aliases), including U32 alignment,
canaries and nonvolatile-register sentinels. Direct MASM normal and MSVC ASan
runs each pass 90,000 checks plus 939 full Win64 ABI checks; a deliberately
clobbered R12 is detected by the positive control.

Valgrind: zero reports over 576 secret-input raw GNU operations (positive
control: 576 reports). Through the C call sites, C/real ASM/forced fallback
each pass 64 secret cases for mul/sqr/add/sub, zero reports; positive control
256 reports. These checks cover executed branches and addresses. No Welch,
power/EM, physical CPU-without-BMI2, board or new end-to-end TLS claim is made.

All 48 test_all solution configurations rebuild without errors or warnings;
only the 12 x64 ASM builds link the new object. Four real MSVC PSA provider
projects (Win32/x64, C/ASM) each pass 60 ECDH P-256 and 78 ECDSA checks.
Ten synthetic preprocessing profiles preserve other paths, apart from four
`&& 1` identities; real old/new x86 GCC and MSVC compile checks pass. These
compatibility checks are not cross-architecture runtime tests.

Benchmark on Ryzen 7 5700U, uninstrumented optimized binaries, medians of
five alternating C/ASM pairs. GCC 13.3 -O3, MSVC 19.51 /O2 /GS, identical CT release flags and BIGINT_TIMING_RESISTANT=1. Each operation runs for at least 150 ms after warmup; field outputs feed the next operands. RNG initialization is outside timing; randomized signing and blinding costs are included. No sanitizer or call counters are timed. There are 160 raw samples across 16 comparisons; these are primitive/API timings, not TLS throughput.

| Compiler | Operation | C us/op | ASM us/op | Speedup |
| --- | --- | ---: | ---: | ---: |
| gcc | field_mul | 0.108 | 0.046 | 2.38x |
| gcc | field_sqr | 0.106 | 0.045 | 2.33x |
| gcc | field_add | 0.024 | 0.010 | 2.44x |
| gcc | field_sub | 0.017 | 0.008 | 2.21x |
| gcc | scalar_mult | 257.353 | 101.263 | 2.54x |
| gcc | ecdh | 580.234 | 234.305 | 2.48x |
| gcc | ecdsa_sign | 363.685 | 232.449 | 1.56x |
| gcc | ecdsa_verify | 997.856 | 384.151 | 2.60x |

Raw vectors, samples, runners and audit reports are in the task's
`p256-asm` artifact directory. These P-256 measurements precede the commit; implementation is committed in r6799 and test_all in r6800.

## P-384 field arithmetic with BMI2 (r6802)

Set `SHARKSSL_ECC_P384_ASM=1` for C and link `SharkSslP384_X64.S`. Default remains 0.
The object contains its own public CPUID probe (7.0 EBX bit 8); BMI2 MULX
is selected only when available, otherwise the existing C CT field paths
and inversion chain remain active. No ADX, AVX or vector OS-state check,
new API/context, or global compiler ISA flag is needed. Flag 1 requires
the object even on a fallback CPU. Compile surrounding C for the baseline ISA.

Original arithmetic from the P-384 modulus in
[NIST SP 800-186](https://doi.org/10.6028/NIST.SP.800-186),
`p = 2^384 - 2^128 - 2^96 + 2^32 - 1`, and published MULX semantics.
No third-party implementation was imported. Six U64 limbs form the product;
36 MULX are followed by signed radix-2^32 folding with
`x^12 = x^4 + x^3 - x + 1`. Normalized carry is in [-2,4]. For R=2^384,
Q=R-p and 5Q<R, two fixed carry folds eliminate the carry; one conditional
subtraction canonicalizes the value below R<2p. Source comments document
the coefficient bounds. An independent integer model checks 4,096 corner
assignments and 100,000 random 768-bit inputs.

Private raw ABI: `sharkssl_p384_mulmod64`, `sharkssl_p384_addmod64`,
`sharkssl_p384_submod64`, and `sharkssl_p384_hw_probe`. Operands/results
are twelve native U32 words, most significant first, canonical in [0,p).
Exact r==a, r==b and a==b aliases work. C wrappers preserve the BigInt
product contract (beg=mem+12, len=12), with an atomic public CPU cache at
all four field gates including the Fermat inversion chain. Applications
continue to call the public SharkSSL APIs, not these private entries.

This accelerates the P-384 field modulo p. Point formulas, scalar control
and ECDSA order-n arithmetic remain C. The documented full-API CT residuals
remain; P-256 and other curves keep their existing implementations.

The arithmetic kernels have fixed addresses and no jumps/calls/division;
GNU/MASM cores disassemble identically after ABI setup. Multiplication
erases 192 written scratch bytes; frames below entry rsp are 264/72/72
bytes for mul/add/sub. Working registers are cleared before restoring
saved registers; CFI and Win64 unwind metadata are supplied and audited.

20 GCC/MSVC profiles PASS (83,084 checks): C, real BMI2 and forced C fallback, normal and sanitizer builds, no-RSA, standalone ECDSA-only, verify-only and P-384-only. Full profiles pass 4,157 checks, including 4,096 field results, 23 scalar points, 21 ECDH cases, five fixed-nonce signatures, ten valid/invalid verification cases and a randomized signature independently verified by Python. Mixed P-256 regression confirms coexistence with both ASM flags enabled and distinct kernel/probe counters.

Direct GNU normal and ASan/UBSan: 972,639 checks per run, 108,071 vectors
with canaries, aliases, U32 alignment residues and ABI sentinels. MASM
normal and ASan: 186,444 arithmetic/alias checks and 1,875 full Win64 ABI
checks per run, including XMM6-XMM15 and an intentional R12-clobber control.
Valgrind raw GNU: 576 secret-input operations, zero errors, positive 576.
Integrated C/ASM/forced fallback taint checks are reported separately in
the task's validation/ct-results.json. These cover executed branches and
addresses; no Welch, power/EM, physical BMI2-free CPU, board or new TLS
runtime campaign is claimed. Sanitizers do not instrument raw assembly.

All 48 test_all builds pass with zero warnings/errors; only 12 x64 ASM
builds select the new object. Four actual PSA project builds each pass the
maintained P-256 ECDH/ECDSA regressions and 21 extra P-384 checks (independent
3G/9G ECDH fixtures, signing/verification, negative cases and lifecycle).
Ten synthetic preprocessing profiles and four real x86 old/new compile
passes preserve the other paths; these are not board runtime tests.

Ryzen 7 5700U. Medians of five alternating C/ASM pairs, at least 150 ms per operation after warmup; GCC13.3 -O3 and MSVC19.51 /O2 /GS, identical CT release flags and BIGINT_TIMING_RESISTANT=1. No sanitizer or counters timed; field outputs feed the next operands. RNG initialization is outside timing, randomized signing/blinding included. 160 raw samples over 16 comparisons; these are primitive/API timings, not TLS throughput.

| Compiler | Operation | C us/op | ASM us/op | Speedup |
| --- | --- | ---: | ---: | ---: |
| GCC | field_mul | 0.176 | 0.065 | 2.71x |
| GCC | field_sqr | 0.162 | 0.062 | 2.60x |
| GCC | field_add | 0.034 | 0.010 | 3.29x |
| GCC | field_sub | 0.023 | 0.010 | 2.40x |
| GCC | scalar_mult | 546.034 | 224.057 | 2.44x |
| GCC | ecdh | 1311.142 | 483.132 | 2.71x |
| GCC | ecdsa_sign | 782.125 | 432.503 | 1.81x |
| GCC | ecdsa_verify | 2371.540 | 826.422 | 2.87x |

Evidence and raw samples: task artifact directory `p384-asm`. P-256 is
committed r6799 (test_all r6800); P-384 is committed r6802 (test_all r6803).

## Full BigInt x64 backend

Select `SHARKSSL_OPTIMIZED_BIGINT_ASM=1` and link `SharkSslBigInt_X64.S`.
It covers `SharkSslBigInt_add`, `SharkSslBigInt_sub`,
`SharkSslBigInt_shiftr`, multiplication and Montgomery reduction. The
temporary multiplication-only selector has been removed, without an alias.
Default 0 retains C and excludes the full assembly object. Do not combine
the full selector with `SHARKSSL_OPTIMIZED_BIGINT_WORD_ASM` (MIPS word-only).
This is a build selection; no runtime CPU probe or optional ISA is needed.

The native U32 MSW-first representation is unchanged. Even product widths
use paired U64 digits/full U128 products; odd widths retain radix 2^32.
REDC requires odd modulus and T<mod*R; the existing C structure adapters
preserve `beg=mem+1,len=n`. These adapters are selected by a private,
compiler-derived guard, not another user configuration option.

New add/sub loops depend only on public lengths, with fixed carry/borrow
propagation and unchanged return values (carry or all-one borrow mask).
Right shift propagates each bit arithmetically, including in the C fallback.
The three new ASM functions are leaf routines using volatile registers only;
no local stack storage or nonvolatile-register changes. Work registers are
cleared before return, apart from the intended carry/borrow return value.
Raw multiplication/REDC retain their established 48/64-byte GNU/MASM saved
register frames, no local scratch and fixed schedules. MUL/IMUL timing
independence remains a hardware assumption, not a universal ISA guarantee.

Original arithmetic derives from HAC Chapter 14 Algorithms 14.12/14.32
(https://cacr.uwaterloo.ca/hac/about/chap14.pdf) and Intel SDM semantics.
No external crypto implementation was imported. The public SharkSSL APIs
and other architecture backend selections are unchanged.

Validation of this full-backend update:

- GCC and MSVC normal/sanitizer direct word tests: 2,720 Python-oracle
  vectors, 2,353,028 checks per profile; unequal/odd lengths, aliases,
  extrema, canaries and structure metadata. Four profiles pass.
- C/ASM word-operation Valgrind: zero reports; positive controls produce
  7,584 reports each. C contract is nonzero width; ASM zero-width defense
  is additionally tested. Integrated arithmetic taint probes also pass.
- 16 GCC/MSVC C/ASM profiles, 2,081,248 assertions: independent
  multiply/REDC/pow, RSA2048 and Brainpool256/384/512 checks, sanitizers,
  no-RSA and standalone ECDSA configurations. The C build does not link
  the full assembly object, as on existing full backends.
- 48 test_all builds and four actual PSA builds pass without warnings;
  60 maintained DHE-RSA/OpenSSL TLS cases pass C/ASM normal/sanitized.
- New word arithmetic cores match between GNU/MASM after argument setup;
  raw multiply/REDC instruction bodies are unchanged by selector unification.

Focused arithmetic builds disable AES-GCM for the existing GCC-O3 header
diagnostic; full project and separate DHE tests use their normal feature
configurations. Sanitizers do not instrument raw assembly. No new board,
Welch or power/EM campaign is claimed. Earlier direct raw/ABI/guard-page
evidence remains in `bigint-x64`; current full-backend evidence is in
`bigint-x64-full`. Backend committed r6811; test_all integration r6812. Validation preceded the commits; tested content was preserved apart from SVN keyword expansion. BAS r6806 still covers
the earlier SHA/NIST integration; enable/link this full BigInt backend
explicitly where required.

Ryzen 7 5700U, five alternating C/ASM pairs, median and at least 160 ms per
operation after warmup. GCC 13.3 -O3 / MSVC 19.51 /O2 /GS, same CT flags.
No counters/profiler/sanitizers in timed builds. Seed/key parsing excluded;
nonce and blinding included. Decrypt includes a 256-byte ciphertext copy
per iteration because the existing API modifies its input. Core expmod
includes R2 preparation and is not TLS throughput.

| Compiler | Operation | C us/op | ASM us/op | C / ASM |
| --- | --- | ---: | ---: | ---: |
| GCC | expmod2048 | 18204.014 | 8017.783 | 2.27x |
| GCC | RSA2048_sign | 5614.049 | 3018.965 | 1.86x |
| GCC | RSA2048_decrypt | 5439.721 | 3016.989 | 1.80x |
| GCC | BP256_sign | 1756.411 | 1049.152 | 1.67x |
| GCC | BP384_sign | 4825.753 | 2505.487 | 1.93x |
| GCC | BP512_sign | 9696.657 | 4854.484 | 2.00x |
