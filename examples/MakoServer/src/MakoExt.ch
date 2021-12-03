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
 * is not defined and if it is unable to open the external mako.zip.
 */
#ifndef USE_EMBEDDED_ZIP
#define USE_EMBEDDED_ZIP 1
#endif


/* Remove: https://realtimelogic.com/ba/doc/?url=auxlua.html#reverseproxy
#define USE_REDIRECTOR 0
*/

/* Remove https://realtimelogic.com/ba/doc/?url=auxlua.html#forkptylib
   Linux only
#define USE_FORKPTY 0
*/

/* Enable http://www.inf.puc-rio.br/~roberto/lpeg/
   You must download the code and include the code in your build
   The mako Makefile attempts to download and compile this code if the
   makefile is compiled with the option USE_LPEG=1
*/
#ifndef USE_LPEG
#define USE_LPEG 0
#endif


/*  Include SQL Database
    0: No database
    1: SQLite (default)
    2: ITTIA database
 */
#ifndef USE_SQL
#define USE_SQL 1
#endif


/* Add your own Lua bindings.
   The function myCustomBindings below must be implemented by you.
#define AUX_LUA_BINDINGS
*/

/* The Lua ZIP file system supports AES encrypted ZIP files. You may
   password encrypt your deployed Lua applications loaded by the Mako
   Server. This macro enables the Lua function mako.setzippwd, a
   function that is used by the application loading mechanism in
   Mako's .preload script. The .preload script calls mako.setzippwd
   for all ZIP files if mako.setzippwd is defined. You must implement
   the template function lSetZipPwd below.
#define ENABLE_ZIP_PWD
*/

/* 
   The Mako Server uses a few operating system depended functions for
   Linux,Mac, QNX, and Windows. Defining the following macro makes it
   possible to compile the Mako Server for other platforms.
  
   You may compile Mako for operating systems such as VxWorks. All
   function listed as required in section CUSTOM_PLAT below must be
   implemented.

#define CUSTOM_PLAT
*/



/* Template function:
   Include your own Lua bindings and extend Mako.
 */
#ifdef AUX_LUA_BINDINGS
static void myCustomBindings(lua_State* L)
{
   int top = lua_gettop(L);
   /*

     Manually add additional bindings here.

    */
   lua_settop(L, top); /* Make sure stack is balanced. */
}
#endif


/* Template function:
   See 'ENABLE_ZIP_PWD' explanation above for details.
 */
#ifdef ENABLE_ZIP_PWD
static int lSetZipPwd(lua_State *L)
{
   /* You must come up with your own solution for password management */
   char* pass="1234";
   IoIntf* io = baluaENV_checkIoIntf(L, 1); /* the only argument */
   int ret = IoIntf_setPassword(io, pass, (U16)strlen(pass));
   lua_pushboolean(L, ret ? 0 : 1);
   return 1;
}
#endif




/*************************************************************************
 ******************************* CUSTOM_PLAT *****************************
 *************************************************************************/

#ifdef CUSTOM_PLAT

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
 * be implemented if the macro USE_EMBEDDED_ZIP is undefined.
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

