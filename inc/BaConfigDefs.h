/* Copyright (C) 2026 Real Time Logic. All rights reserved.
 * Named values shared by application overrides and BAS port defaults.
 */
#ifndef _BaConfigDefs_h
#define _BaConfigDefs_h

#define BA_LUA_OS_LIBRARY_FULL 0
#define BA_LUA_OS_LIBRARY_MINIMAL 1

#define BA_ALLOCATOR_DEFAULT 0
#define BA_ALLOCATOR_DLMALLOC 1
#define BA_ALLOCATOR_MQX 2

#ifndef BA_BYTE_ORDER_LITTLE_ENDIAN
#define BA_BYTE_ORDER_LITTLE_ENDIAN 1234
#elif BA_BYTE_ORDER_LITTLE_ENDIAN != 1234
#error BA_BYTE_ORDER_LITTLE_ENDIAN must be 1234
#endif
#ifndef BA_BYTE_ORDER_BIG_ENDIAN
#define BA_BYTE_ORDER_BIG_ENDIAN 4321
#elif BA_BYTE_ORDER_BIG_ENDIAN != 4321
#error BA_BYTE_ORDER_BIG_ENDIAN must be 4321
#endif

#define BA_TIMER_SOURCE_INTERNAL 0
#define BA_TIMER_SOURCE_EXTERNAL 1

#endif
