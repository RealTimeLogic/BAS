# SharkSSL x86-64 GCC/Clang assembly {#port_x64_gcc}

This port uses GNU assembly syntax and the System V AMD64 ABI for
x86-64, LP64 ELF targets supported by the platform adapter. It provides
AES-GCM/GHASH, optional VAES, ChaCha20, and
Poly1305. This directory does not provide an X25519 or generic BigInt backend.

## Build integration

For a standalone SharkSSL build, keep `SharkSslCrypto.c` and add:

| File | Purpose |
| --- | --- |
| `SharkSslCrypto_X64.S` | AES-NI GCM and GHASH |
| `SharkSslCrypto_X64_AVX2.S` | ChaCha20 |
| `SharkSslCrypto_X64_Poly1305.S` | Poly1305 |
| `SharkSslCrypto_X64_VAES.S` | Optional VAES GCM |
| `SharkSslCrypto_X64_CPUID.c` | CPU and OS feature probes |

Enable the algorithms in the usual SharkSSL configuration and select the
corresponding hooks:

```c
#define SHARKSSL_OPTIMIZED_GCM_ASM 1
#define SHARKSSL_OPTIMIZED_GHASH_ASM 1
#define SHARKSSL_OPTIMIZED_CHACHA_ASM 1
#define SHARKSSL_OPTIMIZED_POLY1305_ASM 1
#define SHARKSSL_OPTIMIZED_GCM_VAES_ASM 1 /* optional */
```

Enable GCM and GHASH together for runtime dispatch. Set an unwanted option to
0. If the assembler lacks VAES/VPCLMULQDQ support, omit the VAES file and set
its option to 0.

Assemble uppercase `.S` files through GCC or Clang with the same configuration
definitions and include paths as C. Pass `-DSHARKSSL_GCM_M0=N` to assembly:
244 for AES-256 or `SHARKSSL_NOPACK`, 212 when AES-192 is the largest enabled
key size, otherwise 180 for AES-128.
Compile common C and the CPU probe for the baseline CPU, without globally
enabling AVX2 or using `-march=native` for a portable executable.

## Runtime selection

AES-NI GCM requires SSSE3, AES-NI, and PCLMULQDQ. AVX2 paths also require OS
support for XMM/YMM state. VAES additionally requires VAES and VPCLMULQDQ.
The shared dispatcher checks the features before selecting accelerated paths
and retains C fallback. CPU brand alone does not determine support.

## Measure application performance

Measure the selected assembly and C paths with your target CPU, compiler,
message sizes, and algorithm configuration. Operation throughput does not
directly predict TLS or application throughput. Runtime dispatch retains
the available fallback when the required processor features are absent.
