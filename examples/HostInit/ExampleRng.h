/*
 * Copyright (c) Real Time Logic LLC, 2026.
 * Distributed under the same license as the SharkSSL examples.
 *
 * DEMONSTRATION ONLY: this helper supplies predictable seed bytes so the
 * examples can run without a platform-specific entropy source. Hashing a
 * function address adds no entropy. Never use this seed in a product.
 * Replace the native branch with your platform's cryptographic random source.
 */
#ifndef SHARKSSL_EXAMPLE_RNG_H
#define SHARKSSL_EXAMPLE_RNG_H

/* BAS examples include their configuration before this shared header. */
#if defined(BA_ENABLE_SHARKSSL) && !BA_ENABLE_SHARKSSL
/* Plain BWS examples have no cryptographic RNG dependency. */
static int sharkssl_exampleSeed(void)
{
   return 0;
}
#else

#include "SharkSslCrypto.h"
#include <string.h>

/* Call before the first RNG use and before starting worker threads.
 * Returns zero on success, nonzero on failure. Repeated calls do not add
 * fresh entropy. Production native initialization needs at least 48 bytes
 * of entropy; subsequent reseeds need at least 32 fresh bytes.
 */
static int sharkssl_exampleSeed(void)
{
#if SHARKSSL_PSA_CONSUMER
   /**
    * Initialize the PSA provider. Configure real entropy in the provider;
    * no demonstration seed is used.
    */
   return sharkssl_entropy();
#else
   int (*address)(void) = sharkssl_exampleSeed;
   U8 input[sizeof(address) + 1];
   U8 seed[2 * SHARKSSL_SHA256_HASH_LEN];
   int status;

   /* Copy the pointer value, not function code, without narrowing to U32. */
   memcpy(input, &address, sizeof(address));
   input[sizeof(address)] = 0;
   status = sharkssl_sha256(input, sizeof(input), seed);
   if (!status)
   {
      input[sizeof(address)] = 1;
      status = sharkssl_sha256(input, sizeof(input),
                              seed + SHARKSSL_SHA256_HASH_LEN);
      if (!status)
         status = sharkssl_entropyEx(seed, 48);
   }
   sharkssl_clear(input, sizeof(input));
   sharkssl_clear(seed, sizeof(seed));
   return status;
#endif
}

#endif /* standalone SharkSSL or BAS with SharkSSL enabled */
#endif /* SHARKSSL_EXAMPLE_RNG_H */
