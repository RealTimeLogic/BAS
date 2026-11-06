POSIX poll dispatcher
====================

Use poll for QNX applications that need more simultaneous connections than
the generic select dispatcher allows. QNXBuild.sh selects poll by default;
set BA_USE_POLL=0 to use select. On Linux, epoll is recommended.

When building Mako directly, use make -f mako.mk BA_USE_POLL=1. The makefile
selects the poll source and header. Use BA_USE_EPOLL=1 for Linux instead.

For custom builds, compile this directory's SoDisp.c and use
inc/arch/NET/poll, inc/arch/Posix, and inc as include directories.
Define BA_USE_POLL=1 throughout the library and application. Use one
dispatcher and clean and rebuild all objects when switching.

The target must provide POSIX poll(), sockets, and a monotonic clock.
Allow enough operating-system sockets, file descriptors, and memory for
the desired connections, and configure the HTTP connection pool as well.
Increasing FD_SETSIZE is unnecessary for poll's socket paths.

See doc/compiling.md for build instructions and porting-layer choices.
