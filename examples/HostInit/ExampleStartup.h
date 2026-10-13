/* Copyright (C) 2026 Real Time Logic. All rights reserved.
   Shared startup contract for the standalone examples. */
#ifndef BAS_EXAMPLE_STARTUP_H
#define BAS_EXAMPLE_STARTUP_H

#ifdef __cplusplus
extern "C" {
#endif

/* Initialize host networking, tracing, and error handling before barracuda. */
void hostInit(void);
/* Application-provided server entry point, called once by the host startup. */
void barracuda(void);

#ifdef __cplusplus
}
#endif

#endif
