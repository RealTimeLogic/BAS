/*
  Declarations for xedgeInitPsa, xedgeInitDiskIo, and xedgeOpenAUX.
  See details below.
  In addition:
  API designed to simplify the two following options as detailed in
  the example file led.c:
    2. Utilize the custom Xedge configuration file API for
       reading/writing the Xedge configuration file
    3. Add additional secrets for TPM key generation
  These optional configuration options must be set in the xedgeOpenAUX function.
*/
#include "XedgeConfig.h"
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
   IoIntfPtr dio; /* The DiskIo instance unless compiled with XEDGE_ENABLE_DISK_IO=0 */
   AddSecret addSecret; /* Callback to addSecret() in xedge.c */
   int initXedgeFuncRef; /* Reference to the init function in Xedge's .config */
   XedgeCfgFile xedgeCfgFile; /* Set this in xedgeOpenAUX() to enable (2) */
} XedgeOpenAUX;


#ifdef xedgeWait4Network
/* Optional function you may enable if the system needs to wait for
 * the network to be up before starting the socket dispatcher
 * SoDisp
*/
void xedgeWait4Network(void);
#endif


/* External code can call this function for graceful shutdown
 */
void setDispExit(void);

#if SHARKSSL_PSA_CONSUMER
/* Required platform hook in PSA builds. No arguments; return int 0 on success
 * or a nonzero error code to stop startup. Initialize entropy, hardware, and
 * persistent storage in the provider's required order, including a successful
 * psa_crypto_init(). Platform prerequisites must be ready here.
 *
 * Called once per barracuda() invocation, before cryptography, resource
 * verification, Lua, or the dispatcher mutex. Not called on a Lua debugger
 * restart. Keep provider state alive through shutdown and safely reuse it
 * if barracuda() is invoked again sequentially, never concurrently.
 * xedgeInitDiskIo() and xedgeOpenAUX() run too late for provider setup.
 * Do not use Lua/server objects or reset persistent keys here.
 * See PsaSimInit.c for the Windows simulator implementation.
 */
int xedgeInitPsa(void);
#endif

#if XEDGE_ENABLE_DISK_IO
/* You must create this function unless compiled with XEDGE_ENABLE_DISK_IO=0.
   Some embedded operating systems require the IO to be configured,
   such as setting the root directory (base path).
*/
int xedgeInitDiskIo(DiskIo* dio);
#endif

/* You must create this function and add your custom Lua bindings in
 * this function.
 */
int xedgeOpenAUX(XedgeOpenAUX* aux);
