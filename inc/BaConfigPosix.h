/* Copyright (C) Real Time Logic LLC, 2026.
 * POSIX feature-test settings must precede every system header.
 * Used by the POSIX target and separately compiled POSIX DiskIo sources.
 */
#ifndef _BaConfigPosix_h
#define _BaConfigPosix_h
#include <BaConfigDefs.h>

#ifndef BA_INTERNAL_CONFIG_OPTS_LOADED
#define BA_INTERNAL_CONFIG_OPTS_LOADED 1
#include <BaConfigOpts.h>
#endif

#if defined(BA_FILE_OFFSET_BITS) && BA_FILE_OFFSET_BITS == 64
#if defined(_FILE_OFFSET_BITS) && _FILE_OFFSET_BITS != 64
#error BA_FILE_OFFSET_BITS=64 requires _FILE_OFFSET_BITS=64
#endif
#ifndef _FILE_OFFSET_BITS
#define _FILE_OFFSET_BITS 64
#endif
/* fseeko uses off_t; fseek still takes a 32-bit long on ILP32 systems. */
#ifndef _LARGEFILE_SOURCE
#define _LARGEFILE_SOURCE 1
#endif
#ifndef _LARGEFILE64_SOURCE
#define _LARGEFILE64_SOURCE 1
#endif
#endif
#endif
