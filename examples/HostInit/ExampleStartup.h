/* Copyright (C) 2026 Real Time Logic. All rights reserved.
   Shared startup contract for the standalone examples. */
#ifndef BAS_EXAMPLE_STARTUP_H
#define BAS_EXAMPLE_STARTUP_H

#ifdef __cplusplus
extern "C" {
#endif

/* Initialize the allocator, networking, tracing, and error handling before
 * barracuda. The allocator must not depend on SharkSSL randomness. Native
 * applications must seed SharkSSL before their first RNG/TLS/authentication
 * consumer; the shared ExampleRng helper is demonstration-only. PSA hosts
 * must initialize their provider before entering barracuda. */
void hostInit(void);
/* Application-provided server entry point, called once by the host startup. */
void barracuda(void);

#ifdef __cplusplus
}
#endif

#endif
