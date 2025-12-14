/*
  Declarations for xedgeInitDiskIo and xedgeOpenAUX. See details below.
  In addition:
  API designed to simplify the two following options as detailed in
  the example file led.c:
    2. Utilize the custom Xedge configuration file API for
       reading/writing the Xedge configuration file
    3. Add additional secrets for TPM key generation
  These optional configuration options must be set in the xedgeOpenAUX function.
*/
#include <barracuda.h>

struct XedgeOpenAUX;

/* Function pointer to addSecret() in xedge.c */
typedef void (*AddSecret)(
   struct XedgeOpenAUX* aux, BaBool unique, const U8* secret, size_t slen);

/* Set this in xedgeOpenAUX() to enable (2)
 */
typedef int (*XedgeCfgFile)(lua_State* L);

typedef struct XedgeOpenAUX {
   lua_State* L; /* The main Lua state */
   IoIntfPtr dio; /* The DiskIo instance unless compiled with NO_BAIO_DISK */
   AddSecret addSecret; /* Callback to addSecret() in xedge.c */
   int initXedgeFuncRef; /* Reference to the init function in Xedge's .config */
   XedgeCfgFile xedgeCfgFile; /* Set this in xedgeOpenAUX() to enable (2) */
} XedgeOpenAUX;

/* External code can call this function for graceful shutdown
 */
void setDispExit(void);

#ifndef NO_BAIO_DISK
/* You must create this function unless compiled with NO_BAIO_DISK.
   Some embedded operating systems require the IO to be configured,
   such as setting the root directory (base path).
*/
int xedgeInitDiskIo(DiskIo* dio);
#endif

/* You must create this function and add your custom Lua bindings in
 * this function.
 */
int xedgeOpenAUX(XedgeOpenAUX* aux);
