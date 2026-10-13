/* Copyright (c) Real Time Logic LLC, 2026. All rights reserved.
 * Use is subject to the Barracuda license supplied with this software.
 */
#ifndef SharkTrustXExample_h
#define SharkTrustXExample_h

#include <SharkTrustX.h>
#include <HttpTrace.h>
#include "../../HostInit/ExampleStartup.h"

/* Keep the preferred name stable to reuse the saved portal enrollment. */
#ifndef EXAMPLE_NAME
#define EXAMPLE_NAME "sharktrustx-c"
#endif
#ifndef SHARKTRUSTX_PRIVATE_CA
#ifndef EXAMPLE_EMAIL
/* Replace John Doe's placeholder with your own ACME contact email. */
#define EXAMPLE_EMAIL "john.doe@example.com"
#endif
#endif
#ifndef EXAMPLE_HTTP_PORT
#define EXAMPLE_HTTP_PORT 8080
#endif
#ifndef EXAMPLE_HTTPS_PORT
#define EXAMPLE_HTTPS_PORT 8443
#endif
#ifndef EXAMPLE_STATE_DIR
/* Existing writable directory for private state. On a device, select a
 * directory on persistent storage using the target DiskIo port's syntax.
 * On Windows/Linux, "." is the shared startup's working directory. */
#define EXAMPLE_STATE_DIR "."
#endif

void installVirtualDir(HttpServer* server);
void certificateEvent(void* context, int code);
void certificateName(void* context, const char* name, size_t length);

#endif
