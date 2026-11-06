# SharkSSL x86-64 GCC/Clang assembly {#port_x64_gcc}

This port uses GNU assembly syntax and the System V AMD64 ABI for
x86-64, LP64 ELF targets supported by the platform adapter. It provides
AES-GCM/GHASH, optional VAES, ChaCha20, Poly1305, and X25519 through BMI2/ADX
integer instructions. The eight files below implement those operations; this directory does not
provide a generic BigInt backend.

## Build integration

Keep `SharkSslCrypto.c` and `SharkSslBigInt.c`, then add the files for the
algorithms you want to accelerate:

| File | Purpose |
| --- | --- |
| `SharkSslCrypto_X64.S` | AES-NI GCM and GHASH |
| `SharkSslCrypto_X64_VAES.S` | Optional VAES GCM |
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
#define SHARKSSL_X25519_ASM_HOOK 1
```

Enable GCM and GHASH together for runtime dispatch. Set an unwanted option to
0. If the assembler lacks VAES/VPCLMULQDQ support, omit the two VAES files and
set its option to 0.

| Build choice | Files to include |
| --- | --- |
| AES-GCM with AES-NI | `SharkSslCrypto_X64.S`, `SharkSslCrypto_X64_CPUID.c` |
| AES-GCM with VAES and AES-NI fallback | AES-NI files plus `SharkSslCrypto_X64_VAES.S`, `SharkSslCrypto_X64_VAES_CPUID.c` |
| ChaCha20-Poly1305 | `SharkSslCrypto_X64_AVX2.S`, `SharkSslCrypto_X64_Poly1305.S`, `SharkSslCrypto_X64_CPUID.c` |
| X25519 only | `SharkSslX25519_X64.S`, `SharkSslX25519_X64_Adapter.c` |

Include `SharkSslCrypto_X64_CPUID.c` only once.

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
path. Leave `SHARKSSL_OPTIMIZED_BIGINT_ASM=0`: it controls the generic BigInt
assembly interface, which this directory does not implement.

`SharkSslBigInt_X25519_mult` calls `SharkSslX25519_tryAsm` when
`SHARKSSL_X25519_ASM_HOOK` is 1. The adapter handles representation conversion,
the CPU check, and its scratch cleanup. A CPU without BMI2/ADX continues through
the existing C implementation.

Defining `SHARKSSL_X25519_TEST` for the assembler exports internal field
arithmetic entry points for regression tests. Leave it undefined in application
builds.

## Runtime selection

AES-NI GCM requires SSSE3, AES-NI, and PCLMULQDQ. AVX2 paths also require OS
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
