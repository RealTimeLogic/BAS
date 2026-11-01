/*
 *     ____             _________                __                _     
 *    / __ \___  ____ _/ /_  __(_)___ ___  ___  / /   ____  ____ _(_)____
 *   / /_/ / _ \/ __ `/ / / / / / __ `__ \/ _ \/ /   / __ \/ __ `/ / ___/
 *  / _, _/  __/ /_/ / / / / / / / / / / /  __/ /___/ /_/ / /_/ / / /__  
 * /_/ |_|\___/\__,_/_/ /_/ /_/_/ /_/ /_/\___/_____/\____/\__, /_/\___/  
 *                                                       /____/          
 *
 *                             Mako Server
 ****************************************************************************

 This file makes it easy to add your own Mako Server extensions
 without having to modify MakoMain.c

*/


/* The directory for the "home" IO
   You may remove the macro if you do not need this IO.
 */
#ifndef MAKO_HOME_DIR
#define MAKO_HOME_DIR ""
#endif

/* The "disk" IO root directory */
#ifndef MAKO_ROOT_DIR
#define MAKO_ROOT_DIR "/"
#endif

/* The Mako Server can optionally use an embedded version of
 * mako.zip. The embedded ZIP file is a smaller version of mako.zip
 * and includes just enough code to open the HTTP server listening port
 * and start LSP apps from command line params. This ZIP file is built
 * and converted to a C code array by the "BuildInternalZip.sh"
 * script. Note: The Mako Server will abort at startup if this macro
 * is 0 and if it is unable to open the external mako.zip.
 * Note: this feature is auto disabled when BA_ENABLE_ZIP_SIGNATURE is enabled.
 */


/* Remove: https://realtimelogic.com/ba/doc/?url=auxlua.html#reverseproxy
#define BA_ENABLE_REVERSE_PROXY 0
*/

/* Remove https://realtimelogic.com/ba/doc/?url=auxlua.html#forkptylib
   Linux only
#define BA_ENABLE_LUA_FORKPTY 0
*/

/* Enable http://www.inf.puc-rio.br/~roberto/lpeg/
   You must download the code and include the code in your build
   The mako Makefile attempts to download and compile this code if the
   makefile is compiled with the option BA_ENABLE_LUA_LPEG=1
*/


/*  Include SQL Database
    0: No database
    1: SQLite (default)
    2: ITTIA database
 */


/* Add your own Lua bindings.
   Supply the function named by MAKO_OPEN_BINDINGS in host-owned code.
   See the template below; this hook also runs in PSA builds.
*/

/* 
   The Mako Server uses a few operating system depended functions for
   Linux,Mac, QNX, and Windows. Defining the following macro makes it
   possible to compile the Mako Server for other platforms.
  
   You may compile Mako for operating systems such as VxWorks. All
   function listed as required in section MAKO_USE_CUSTOM_PLATFORM below must be
   implemented.

#define MAKO_USE_CUSTOM_PLATFORM 1
*/


/* Template for ordinary binding registration, independent of softTPM.
 * Define this token and function in MakoExt1.ch, or provide your own function
 * in another source file and define the token in the build.

#define MAKO_OPEN_BINDINGS myMakoBindings
static void myMakoBindings(lua_State* L)
{
   int top = lua_gettop(L);
   // Register your Lua bindings here.
   lua_settop(L, top); // Leave the stack balanced.
}

 */

/* 
   Enable signed ZIP file enforcement. Create 'zipPubKey; below using
   SharkSSLParseKey.exe myPubEccKey.pem
   Details:
    https://realtimelogic.com/ba/doc/en/C/reference/html/SignEncZip.html

#define BA_ENABLE_ZIP_SIGNATURE 1
const U8 zipPubKey[72] =
{
};

*/



/* Globally apply a password to all loaded ZIP files. This approach
   allows embedding an obfuscated password that will be automatically
   applied to all ZIP files. Use binpwd2str to generate the zipBinPwd
   array content below.
   Details:
    https://realtimelogic.com/ba/doc/en/C/reference/html/SignEncZip.html

#define BA_ENABLE_ZIP_PASSWORD 1
#define BA_REQUIRE_ZIP_PASSWORD FALSE

static const U8 zipBinPwd[] ={
};

*/
   
#include "MakoConfig.h"

/********************************* TPM ***********************************/
#if BA_ENABLE_SOFTTPM_KEY_MATERIAL

/* The platform-specific function 'pushUniqueKey()' attempts to inject
   a unique per-device key, further enhancing the security of the TPM
   by making it more difficult to transfer TPM-generated secrets to
   another machine.
*/
#include "PushUniqueKey.ch"


void makoOpenAUX(MakoOpenAUX* aux)
{
   if(pushUniqueKey(L)) /* Per device key */
      makoprintf(TRUE,"Warning: no device unique TPM ID\n");
   else
      aux->addSecret(aux, TRUE, 0, 0);
}

#endif /* BA_ENABLE_SOFTTPM_KEY_MATERIAL */



/*************************************************************************
 ******************************* MAKO_USE_CUSTOM_PLATFORM *****************************
 *************************************************************************/

#if MAKO_USE_CUSTOM_PLATFORM

/* The "get current directory" function is used when searching for the
 * Mako config file. This function is not required and can return NULL.
 *
 * In addition to the optional command line parameter "-c", the Mako
 * Server also looks for the config file in the directory returned
 * by function findExecPath.
 */
static char*
xgetcwd(char *buf, size_t size)
{
   return 0;
}


/* This function should return the path to the directory where
 * mako.zip is stored. The buffer must be dynamically allocated since
 * the caller frees the memory by calling baFree. This function must
 * be implemented when MAKO_ENABLE_EMBEDDED_ZIP_FALLBACK=0.
 */
static char*
findExecPath(const char* argv0)
{
   /* The following example code shows how you could potentially use
    * xgetcwd to return the path, assuming that the home directory is
    * where the Mako ZIP file is stored.
    */
   char* buf=baMalloc(1000);
   if(buf)
      return xgetcwd(buf,1000);
   return 0;
}

/* The following functions are not required and are defined to do nothing. */

/* Enter Linux daemon mode */
#define setLinuxDaemonMode()
#define disableSignals()

/* Set Linux user */
#define setUser(argc, argv)

/* Install a CTRL-C handler */
#define setCtrlCHandler()

#endif

