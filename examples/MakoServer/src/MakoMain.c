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
 *            PROGRAM MODULE
 *
 *   $Id: MakoMain.c 5387 2023-02-20 22:50:13Z wini $
 *
 *   COPYRIGHT:  Real Time Logic LLC, 2012 - 2023
 *
 *   This software is copyrighted by and is the sole property of Real
 *   Time Logic LLC.  All rights, title, ownership, or other interests in
 *   the software remain the property of Real Time Logic LLC.  This
 *   software may only be used in accordance with the terms and
 *   conditions stipulated in the corresponding license agreement under
 *   which the software has been supplied.  Any unauthorized use,
 *   duplication, transmission, distribution, or disclosure of this
 *   software is expressly forbidden.
 *                                                                        
 *   This Copyright notice may not be removed or modified without prior
 *   written consent of Real Time Logic LLC.
 *                                                                         
 *   Real Time Logic LLC. reserves the right to modify this software
 *   without notice.
 *
 *               http://realtimelogic.com
 ****************************************************************************
 *
 */

#ifdef _WIN32
#define _CRT_RAND_S
#endif

#ifndef MAKO
#error The macro MAKO must be defined when you compile the Mako Server (for all BAS files)
#endif

#include "mako.h"



/* Either remove the macro definition below or define macro
   BA_FILESIZE64 when compiling the code. You probably want to
   enable 64 bit file system support.

   NOTE: All BAS code must be compiled with this macro.
 */
#ifndef BA_FILESIZE64
#error You should probably compile with -DBA_FILESIZE64
#endif



/* If not using: https://realtimelogic.info/amalgamator/ */
#ifndef USE_AMALGAMATED_BAS
#include <barracuda.h>
#include <HttpTrace.h>
#include <HttpCmdThreadPool.h>
#include <HttpResRdr.h>
#include <IoIntfZipReader.h>
#include <lualib.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>


/* Version info */
#include "MakoVer.h"

/* Global pointer to dispatcher declared on 'main' stack */
static SoDisp* dispatcher;
static lua_State* L; /* pointer to the Lua virtual machine */
static IoIntfZipReader* makoZipReader; /* When using the ZIP file */


/* Your optional Custom mods */
#include "MakoExt.ch"

#define MAKOEXNAME ""

#ifndef MAKO_VNAME
#define MAKO_VNAME      "Mako Server" MAKOEXNAME   ". Version " MAKO_VER
#endif
#ifndef MAKO_CPR
#define MAKO_CPR "Copyright (c) Real Time Logic."
#endif


/* SharkTrustEx

   Create your own tokengen.h header file as follows:

   1: Sign up for Real Time Logic's DNS testing service or preferably,
      set up your own DNS service (one of:)
        A: Testing service: https://acme.realtimelogic.com/
        B: Your own service: https://github.com/RealTimeLogic/SharkTrustEx
   2: Wait for the email received from the DNS service
   3: Navigate to your DNS zone https://your-domain/login
   4: Login using your credentials
   5: Navigate to https://your-domain/cgen
   6: Download the generated C code, rename to tokengen.h,
      and place generated file in this directory
  
   The code can be excluded by defining NO_SHARKTRUST or by removing code below
*/
#ifdef NO_SHARKTRUST
#define balua_tokengen(L)
#else
#define EMBEDDED_ZONE_KEY
#include "tokengen.c"
#endif


#ifdef CUSTOM_PLAT

/* Include custom header files */

#else
#ifdef _WIN32
#pragma warning(disable : 4996)
#include "Windows/servutil.h"
#include <direct.h>
#define xgetcwd _getcwd
#else
#include <pwd.h>
#include <syslog.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <grp.h>

#define xgetcwd getcwd
#endif
#endif

#if USE_LPEG
extern int luaopen_lpeg(lua_State *L);
#endif
#if USE_PROTOBUF
extern int luaopen_pb(lua_State* L);
#endif

#if USE_OPCUA
#include <opcua_module.h>
#endif

#ifndef AUX_LUA_BINDINGS
#ifdef myCustomBindings
extern void myCustomBindings(lua_State *L);
#else
#define myCustomBindings(L)
#endif
#endif

#ifdef __APPLE__
#pragma GCC diagnostic ignored "-Wpointer-sign"
#endif

typedef struct {
   HttpServerConfig scfg;
   U32 timerTick;
} MakoServerConfig;

typedef struct
{
   U32 trackerNodes,maxlogin;
   BaTime bantime;
} ConfigParams;
ConfigParams* cfgParams;

int daemonMode=0;
#ifdef MAKO_HOME_DIR
static IoIntfPtr homeIo;
#endif

static ThreadMutex mutex;
static HttpServer server;
static BaTimer timer;
static DiskIo diskIo; /* The IO named "disk" */


#if USE_EMBEDDED_ZIP
extern ZipReader* getLspZipReader(void); /* ../obj/debug|release/LspZip.c */
#endif

/* xrc/sql/ */
#if USE_SQL == 2
#define luaopen_SQL luaopen_luasql_ittiadb
extern int luaopen_luasql_ittiadb(lua_State *L);
#elif USE_SQL == 1
#define luaopen_SQL luaopen_luasql_sqlite3
extern int luaopen_luasql_sqlite3(lua_State *L);
#else
#define luaopen_SQL(L)
#endif

static FILE* logFp; /* Log file */

void
setDispExit(void)
{
   SoDisp_setExit(dispatcher);
}

#if USE_DBGMON
static BaBool restart;
static void
dbgExitRequest(void* na, BaBool rstart)
{
   (void)na;
   restart=rstart;
   setDispExit();
}
#endif

/* Change log file location/name */
static int lSetTraceFile(lua_State *L)
{
   const char* name = luaL_checkstring(L,1);
   FILE* fp = fopen(name, "wb");
   if(fp)
   {
      if(logFp) fclose(logFp);
      logFp=fp;
      lua_pushboolean(L, TRUE);
   }
   else
      lua_pushboolean(L, FALSE);
   return 1;
}

#ifndef va_copy
#define va_copy(d,s) ((d) = (s))
#endif

static int
tryTraceFile(const char* name)
{
   logFp=fopen(name,"wb");
   if(logFp)
   {
      time_t t = time(0);
      struct tm tm = *localtime(&t);
      fprintf(logFp, "\n\nStarting Mako Server: %d-%02d-%02d %02d:%02d:%02d\n",
              tm.tm_year + 1900, tm.tm_mon + 1,
              tm.tm_mday, tm.tm_hour,
              tm.tm_min, tm.tm_sec);
      fprintf(stderr,"Trace file: %s\n",name);
      return 0;
   }
   return -1;
}


static void
openTraceFile(const char* name)
{
   if(!logFp)
   {
#ifdef BA_POSIX
      if( ! tryTraceFile("/var/log/mako.log") )
         return;
#endif
      if(tryTraceFile(name ? name : "mako.log"))
      {
         char* tmp=getenv("TMP");
         if(!tmp)
            tmp=getenv("TEMP");
         if(tmp)
         {
            char* ptr=malloc(strlen(tmp)+10);
            if(ptr)
            {
               sprintf(ptr,"%s/mako.log",tmp);
               tryTraceFile(ptr);
               free(ptr);
            }
         }
      }
      if(!logFp) return;
   }
}


void
makovprintf(int isErr,const char* fmt, va_list argList)
{
   va_list xList;
   if(daemonMode || logFp)
   {
#if !defined(_WIN32) && !defined(CUSTOM_PLAT)
      if(isErr)
      {
         va_copy(xList, argList);
         vsyslog(LOG_CRIT, fmt, xList);
         va_end(xList);
      }
#endif
      if(!logFp) return;
      if(daemonMode < 1) /* not -d, but non daemon or -s */
      {
         va_copy(xList, argList);
         vfprintf(logFp,fmt,xList);
         va_end(xList);
         vfprintf(stderr,fmt,argList);
      }
      else
         vfprintf(logFp,fmt,argList);
      fflush(logFp);
   }
   else
      vfprintf(stderr,fmt,argList);
}


void
makoprintf(int isErr,const char* fmt, ...)
{
   va_list varg;
   va_start(varg, fmt);
   makovprintf(isErr, fmt, varg);
   va_end(varg); 
}


void
errQuit(const char* fmt, ...)
{
   va_list varg;
   va_start(varg, fmt);
   makovprintf(TRUE, fmt, varg);
   va_end(varg);
   HttpTrace_flush();
   if(logFp) fclose(logFp);
   exit(1);
}


static void
serverErrHandler(BaFatalErrorCodes ecode1,
                 unsigned int ecode2,
                 const char* file,
                 int line)
{
   sendFatalError("FE",ecode1,ecode2,file,line);
#ifndef NDEBUG
   if(FE_ASSERT == ecode1) return;
#endif
   errQuit("Fatal err: %d:%d, %s %d\n",ecode1,ecode2,file,line);
}


static void
writeHttpTrace(char* buf, int bufLen)
{
   buf[bufLen]=0; /* Safe. See documentation. */
   makoprintf(FALSE,"%s",buf);
}


#ifdef MAKO_HOME_DIR
static void
setHomeIoPath(const char* homedir)
{
   int status;
   static union {
      DiskIo dio;
      NetIo nio;
   } u;
   if(strncmp("http://",homedir,7))
   {
      DiskIo_constructor(&u.dio);
      status=DiskIo_setRootDir(&u.dio, homedir);
      homeIo=(IoIntfPtr)&u.dio;
   }
   else
   {
      NetIo_constructor(&u.nio, dispatcher);
      status=NetIo_setRootDir(&u.nio,homedir);
      homeIo=(IoIntfPtr)&u.nio;
   }
   if(status)
      errQuit("Cannot set home IO path to %s: %s\n", homedir,baErr2Str(status));
   if(*homedir)
      makoprintf(FALSE,"Home dir: %s\n",homedir);
   if(!logFp && *homedir)
   {
      char* rname = IoIntf_getAbspath(homeIo, "");
      if(rname)
      {
         char* ptr=malloc(strlen(rname)+10);
         if(ptr)
         {
            sprintf(ptr,"%smako.log",rname);
            openTraceFile(ptr);
            free(ptr);
         }
         baFree(rname);
      }
   }
}
#endif

static int
findFlag(int argc, char** argv, const char flag, const char** data)
{
   int i;
   for(i = 1; i < argc ; i++)
   {
      char* ptr = argv[i];
      if(*ptr == '-' && ptr[1] == flag && !ptr[2])
      {
         if(data)
            *data = ((i+1) < argc) ? argv[i+1] : NULL;
         return TRUE;
      }
   }
   if(data)
      *data = NULL;
   return FALSE;
}

#ifndef CUSTOM_PLAT

/******************************** WIN32 **********************************/
#ifdef _WIN32
#define BA_WIN32
static int
pxexit(int status, int pause)
{
   if(pause)
   {
      fprintf(stderr,"\nPress <Enter> to continue.\n");
      getchar();
   }
   exit(status);
}


static char*
findExecPath(const char* argv0)
{
   char buf[2048];
   (void)argv0;
   if(GetModuleFileName(NULL, buf, sizeof(buf)))
   {
      char* ptr=strrchr(buf,'\\');
      if(ptr)
         ptr[0]=0;
      else
         buf[0]=0;
      return baStrdup(buf);
   }
   return NULL;
}


static BOOL WINAPI
sigTerm(DWORD x)
{
   int i;
   makoprintf(FALSE,"\nExiting...\n");
   setDispExit();
   for (i = 0; i < 60 && dispatcher; i++)
      Sleep(100); /* Ref-wait */
   return FALSE;
}


static void
setCtrlCHandler(void)
{
   SetConsoleCtrlHandler(sigTerm, TRUE);
}
#define setLinuxDaemonMode()
#define disableSignals()
#define setUser(argc, argv)

/******************************** LINUX **********************************/
#else /* #ifdef _WIN32 */

extern char** environ;

#ifdef _OSX_
#include <mach-o/dyld.h>
static char*
findExecPath(const char* argv0)
{
   char path[2048];
   uint32_t size = sizeof(path);
   (void)argv0;
   if ( ! _NSGetExecutablePath(path, &size) )
   {
      char* ptr=strrchr(path,'/');
      if(ptr)
         ptr[0]=0;
      else
         path[0]=0;
      ptr=baStrdup(path);
      if(ptr && realpath(ptr, path))
      {
         free(ptr);
         return baStrdup(path);
      }
      return ptr;
   }
   return NULL;
}
#else
static char*
findExecPath(const char* argv0)
{
   char* ptr;
   char buf[2048];
   ssize_t size = readlink("/proc/self/exe", buf, sizeof(buf));
   if(size < 0)
   {
      if( (size = readlink("/proc/curproc/file", buf, sizeof(buf))) < 0)
      {
         if( (size = readlink("/proc/self/path/a.out", buf, sizeof(buf))) < 0)
         {
            struct stat statBuf;
            if(argv0 && ! stat(argv0, &statBuf) && S_ISREG(statBuf.st_mode) &&
               (ptr = strrchr(argv0, '/')) != 0)
            {
               size = ptr - argv0;
               if('/' == *argv0)
               {
                  strncpy(buf, argv0, size);
                  buf[size]=0;
               }
               else if(xgetcwd(buf, sizeof(buf)))
               {
                  if(size != 1 || argv0[0] != '.')
                  {
                     if(buf[strlen(buf)-1] != '/' && size > 0)
                        strcat(buf,"/");
                     strncat(buf,argv0,size);
                  }
               }
               baElideDotDot(buf);
               if(!stat(buf, &statBuf) && S_ISDIR(statBuf.st_mode))
               {
                  strcat(buf, "/"); /* Required for final test in this func. */
                  size = strlen(buf);
               }
               else
                  size=-1;
            }
            if(size < 0)
            {
               char* name;
               char* path=getenv("PATH");
               while(path)
               {
                  int len;
                  ptr=strchr(path,':');
                  if(!ptr)
                     ptr=path+strlen(path);
                  len = (int)(ptr-path);
                  name=malloc(len+10);
                  if(!name)
                     return 0;
                  strncpy(name, path, len);
                  name[len]=0;
                  if(name[len-1] != '/')
                     strcat(name,"/");
                  strcat(name,"mako.zip");
                  if( ! stat(name, &statBuf) )
                  {
                     ptr=strrchr(name,'/');
                     *ptr=0;
                     return name;
                  }
                  baFree(name);
                  if(*ptr != ':')
                     return 0;
                  path=ptr+1;
                  if( ! *path )
                     return 0;
               }
            }
         }
      }
   }
   if(size > 0)
   {
      buf[size]=0;
      ptr=strrchr(buf,'/');
      if(ptr)
         ptr[0]=0;
      else
         buf[0]=0;
      return baStrdup(buf);
   }
   return 0;
}
#endif

static void
sigTerm(int x)
{
   (void)x;
   static int oneshot=FALSE;
   if(oneshot)
   {
      makoprintf(FALSE,"\nGot SIGTERM again; aborting...\n");
      HttpTrace_flush();
      if(logFp) fclose(logFp);
      abort();
   }
   oneshot=TRUE;
   makoprintf(FALSE,"\nGot SIGTERM; exiting...\n");
   setDispExit();
}
static void
setCtrlCHandler(void)
{
   signal(SIGINT, sigTerm);
   signal(SIGTERM, sigTerm);
}

static void
cfignoreSignal(int sig, const char* signame)
{
   struct sigaction sa;
   sa.sa_handler = SIG_IGN;
   sigemptyset(&sa.sa_mask);
   sa.sa_flags = 0;
   if (sigaction(sig, &sa, NULL) < 0)
      errQuit("can't ignore %s.\n", signame);
}
#define ignoreSignal(x) cfignoreSignal((int)(x), #x)
#ifndef BA_VXWORKS
static void
cfblockSignal(int sig, const char* signame)
{
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, sig);
    if (sigprocmask(SIG_BLOCK, (const sigset_t*) &set, NULL) < 0)
       errQuit("can't block %s - %s.\n", signame,strerror(errno));
}
#define blockSignal(x) cfblockSignal((int)(x), #x)
#endif

static void
disableSignals(void)
{
   ignoreSignal(SIGPIPE);
}

#ifdef BA_VXWORKS
#define setLinuxDaemonMode()
#else
static void
setLinuxDaemonMode(void)
{
   int fd0, fd1, fd2;
   pid_t pid;

   /*
    * Clear file creation mask.
    */
   umask(0);

   /*
    * Become a session leader to lose controlling TTY.
    */
   if((pid = fork()) < 0)
      errQuit("%s: can't fork\n", APPNAME);
   else if(pid != 0) /* parent */
      exit(0);
   setsid();

   if((pid = fork()) < 0)
      errQuit("%s: can't fork\n", APPNAME);
   else if (pid != 0) /* parent */
      exit(0);

    ignoreSignal(SIGHUP);
    ignoreSignal(SIGTTOU);
    ignoreSignal(SIGTTIN);
    ignoreSignal(SIGTSTP);
    ignoreSignal(SIGPIPE);
    ignoreSignal(SIGXFSZ);
    ignoreSignal(SIGXCPU); /*Prevent server from terminating after a few days*/
    ignoreSignal(SIGXFSZ);
    blockSignal(SIGUSR1);
    blockSignal(SIGUSR2);

   /*
   ** Attach file descriptors 0, 1, and 2 to /dev/null.
   ** Note that these do not have cloexec set.
   ** The idea is that both parent and child processes that write to
   ** stdout/stderr will gracefully do nothing.
   **/
   close(0);
   close(1);
   close(2);
   fd0 = open("/dev/null", O_RDWR);
   fd1 = dup(0);
   fd2 = dup(0);

   /*
    * Initialize the log file.
    */
   openlog(APPNAME, LOG_CONS|LOG_PID, LOG_DAEMON);
   if (fd0 != 0 || fd1 != 1 || fd2 != 2)
   {
      errQuit("unexpected file descriptors %d %d %d\n",fd0, fd1, fd2);
   }
   syslog(LOG_INFO, "%s: daemon started...", APPNAME);
}
#endif

#ifdef BA_VXWORKS
#define setUser(x,y)
#else
static void
setUser(int argc, char** argv)
{
   const char* userName;
   findFlag(argc, argv, 'u', &userName);
   if(userName)
   {
      gid_t gid=0;
      uid_t uid=0;
      if(getuid()!=0 || getgid()!=0)
      {
         errQuit("Cannot use command line option: -u %s. "
                 "You are not root.\n", userName);
      }
      if(isdigit(userName[1]))
      {
         gid=uid=U32_atoi(userName);
         makoprintf(FALSE,"Setting user to id '%d'\n", uid);
      }
      else
      {
         int ngroups;
         gid_t* groups;
         struct passwd* pwd;
         makoprintf(FALSE,"Setting user to '%s'\n", userName);
         pwd = getpwnam(userName);
         if(pwd)
         {
            ngroups=sysconf(_SC_NGROUPS_MAX);
            groups=(gid_t*)baMalloc(ngroups * sizeof(gid_t));
            if(!groups ||
               getgrouplist(userName, pwd->pw_gid, groups, &ngroups) < 0 ||
               setgroups(ngroups,groups) < 0)
            {
               errQuit("getgrouplist: %s\n",strerror(errno));
            }
            baFree(groups);
            setenv("HOME",pwd->pw_dir, TRUE);
            uid=pwd->pw_uid;
            gid=pwd->pw_gid;
         }
         else
            errQuit("Cannot find user %s.\n",userName);
      }
      if(gid==0 || uid == 0 || setgid(gid) || setuid(uid))
      {
         errQuit("setgid/setuid: %s\n",strerror(errno));
      }
   }
}
#endif

#endif /* #ifdef _WIN32 */
/***************************** END WIN/LINUX *******************************/
#endif /* #ifdef CUSTOM_PLAT */


/* Run the Lua 'onunload' handlers for all loaded apps when mako exits.
   onunload is an optional function applications loaded by mako can
   use if the applications require graceful termination such as
   sending a socket close message to peers.
 */
static void
onunload(int onunloadRef)
{
   /* Run the 'onunload' function in the .config Lua script, which in
    * turn runs the optional onunload for all loaded apps.
    */
   lua_rawgeti(L, LUA_REGISTRYINDEX, onunloadRef);
   baAssert(lua_isfunction(L, -1));
   if(lua_pcall(L, 0, 1, 0))
   {
      makoprintf(TRUE,"Error in 'onunload': %s\n",
                 lua_isstring(L,-1) ? lua_tostring(L, -1) : "?");
   }
   luaL_unref(L, LUA_REGISTRYINDEX, onunloadRef);
   balua_relsocket(L); /* Gracefully close all cosockets, if any */
}

static int
loadConfigData(lua_State* L)
{
   U16 val;
   MakoServerConfig* mcfg = (MakoServerConfig*)lua_touserdata(L,1);
#ifdef BA_VXWORKS
   int status = balua_loadfile(
      L, (const char*)lua_touserdata(L,2),(IoIntf*)&diskIo, 0);
#else
   int status = luaL_loadfile(L, (const char*)lua_touserdata(L,2));
#endif
   const char** paramPtr = lua_touserdata(L,3);
   if (status != 0)
      lua_error(L);
   else
      lua_call(L,0,0);
   *paramPtr="commandcnt";
   lua_getglobal(L, *paramPtr);
   HttpServerConfig_setNoOfHttpCommands(
      &mcfg->scfg,(U16)luaL_optinteger(L,-1,3));

   *paramPtr="sessioncnt";
   lua_getglobal(L, *paramPtr);
   HttpServerConfig_setMaxSessions(&mcfg->scfg,(U16)luaL_optinteger(L,-1,50));

   *paramPtr="conncnt";
   lua_getglobal(L, *paramPtr);
   HttpServerConfig_setNoOfHttpConnections(
      &mcfg->scfg,(U16)luaL_optinteger(L,-1,50));

   *paramPtr="rspsz";
   lua_getglobal(L, *paramPtr);
   HttpServerConfig_setResponseData(
      &mcfg->scfg,(U16)luaL_optinteger(L,-1,8*1024));

   *paramPtr="reqminsz";
   lua_getglobal(L, *paramPtr);
   val=(U16)luaL_optinteger(L,-1,4*1024);

   *paramPtr="reqmaxsz";
   lua_getglobal(L, *paramPtr);
   HttpServerConfig_setRequest(
      &mcfg->scfg,val,(U16)luaL_optinteger(L,-1,8*1024));

   *paramPtr="timertick";
   lua_getglobal(L, *paramPtr);
   mcfg->timerTick = (U32)luaL_optinteger(L,-1,50);

   *paramPtr="tracker_nodes";
   lua_getglobal(L, *paramPtr);
   cfgParams->trackerNodes=(int)luaL_optinteger(L,-1,100);

   *paramPtr="tracker_maxlogin";
   lua_getglobal(L, *paramPtr);
   cfgParams->maxlogin=(int)luaL_optinteger(L,-1,4);

   *paramPtr="tracker_bantime";
   lua_getglobal(L, *paramPtr);
   cfgParams->bantime=(int)luaL_optinteger(L,-1,10*60);

#ifdef MAKO_HOME_DIR
   *paramPtr="homeio";
   lua_getglobal(L, *paramPtr);
   setHomeIoPath(luaL_optstring(L, -1, MAKO_HOME_DIR));
#endif
   return 0;
}


static char*
openAndLoadConf(const char* path,  MakoServerConfig* mcfg, int isdir)
{
   char* name=0;
   char* abspath=0;
   if(path)
   {
      if(*LUA_DIRSEP == '/' ? (path[0] != '/') : (path[1] != ':'))
      {
         char cwd[1024];
         if(xgetcwd(cwd, sizeof(cwd)))
         {
            abspath=baMalloc(strlen(cwd)+strlen(path)+5);
            basprintf(abspath,"%s%s%s",cwd,LUA_DIRSEP,path);
            baElideDotDot(abspath);
            path=abspath;
         }
      }
      name=malloc(strlen(path)+15);
      if(name)
      {
         struct stat statBuf;
         if(isdir)
            basprintf(name,"%s%smako.conf",path,LUA_DIRSEP);
         else
            basprintf(name,"%s",path);
         if( ! stat(name, &statBuf) )
         {
            const char* param=0;
            lua_State* L = luaL_newstate();
            luaopen_base(L);
            lua_pushcfunction(L,loadConfigData);
            lua_pushlightuserdata(L, mcfg);
            lua_pushlightuserdata(L, name);
            lua_pushlightuserdata(L, (void*)&param);
            makoprintf(FALSE,"Loading %s\n",name);
            if(lua_pcall(L,3,0,0))
            {
               const char* msg=lua_tostring(L,-1);
               if (!msg) msg="(error with no message)";
               if(param)
               {
                  const char* ptr=strchr(msg, '(');
                  makoprintf(TRUE,"Invalid value for paramater '%s': %s\n",
                             param, ptr ? ptr : msg);
               }
               else
                  makoprintf(TRUE,"%s\n",msg);
               exit(1);
            }
            lua_close(L);
         }
         else
         {
            free(name);
            name=NULL;
         }
      }
   }
   if(abspath)
      baFree(abspath);
   return name;
}


static char*
openAndLoadConfUsingExecPath(
   MakoServerConfig* cfg, const char* argv0, char** execpath)
{
   *execpath=findExecPath(argv0);
   if(*execpath)
   {
      char* cfgfname=openAndLoadConf(*execpath,cfg,TRUE);
      return cfgfname;
   }
   return NULL;
}


/* Initialize the HttpServer object by calling HttpServer_constructor.
   The MakoServerConfig object is only needed during initialization.
*/
static char*
createServer(int argc, char** argv, char** execpath)
{
   MakoServerConfig mcfg;
   const char* ccfgfname;
   char* cfgfname=0;
   *execpath=0;
   HttpServerConfig_constructor(&mcfg.scfg);
   mcfg.timerTick = 50;

   if(findFlag(argc, argv, 'c', &ccfgfname))
   {
      if( ! (cfgfname=openAndLoadConf(ccfgfname,&mcfg,FALSE)) )
      {
         errQuit("Error: cannot load configuration file %s.\n",ccfgfname);
      }
   }
   else if( ! (cfgfname=openAndLoadConfUsingExecPath(
                  &mcfg,argc>0?argv[0]:0,execpath)) &&
            ! (cfgfname=openAndLoadConf("mako.conf",&mcfg,FALSE)))
   {
      if( ! (cfgfname=openAndLoadConf(getenv("HOME"),&mcfg,TRUE)) )
      {
#ifdef BA_WIN32
         if( ! (cfgfname=openAndLoadConf(getenv("USERPROFILE"),&mcfg,TRUE)) )
            cfgfname=openAndLoadConf(getenv("HOMEPATH"),&mcfg,TRUE);
#endif
      }
   }
   if(!cfgfname)
   {
      HttpServerConfig_setNoOfHttpCommands(&mcfg.scfg,3);
      HttpServerConfig_setNoOfHttpConnections(&mcfg.scfg,50);
      HttpServerConfig_setMaxSessions(&mcfg.scfg,50);
      HttpServerConfig_setResponseData(&mcfg.scfg,8*1024);
      HttpServerConfig_setRequest(&mcfg.scfg,4*1024,8*1024);
#ifdef MAKO_HOME_DIR
      setHomeIoPath(MAKO_HOME_DIR);
#endif
   }
   mcfg.scfg.maxResponseHeader = 32 * 1024;

   /* Create and init the server, by using the above HttpServerConfig.
    */
   HttpServer_constructor(&server, dispatcher, &mcfg.scfg);

   /* The optional timer. Remove or change in MakoExt.ch */
#ifdef MAKO_CUSTOM_TIMER
   MAKO_CUSTOM_TIMER(&timer, &mutex, timerTick);
#else
   BaTimer_constructor(
      &timer, &mutex, 10000, mcfg.timerTick, ThreadPrioNormal, 0);
#endif

   return cfgfname;
}

static int
mako_exit(lua_State *L)
{
   exit((int)luaL_optinteger(L, 1, 0));
}


/* Create Lua global variables "argv" for the command line arguments and
 * "env" for the system environment variables.
 */
static void
createLuaGlobals(
   int argc, char** argv, char** envp, char* cfgfname, const char* execpath)
{
   int i;
   char* ptr;
   char buf[200];
   static const luaL_Reg funcs[] = {
      {"tracefile", lSetTraceFile},
#ifdef ENABLE_ZIP_PWD
      {"setzippwd", lSetZipPwd},
#endif
      {NULL, NULL}
   };

   lua_newtable(L);
   lua_pushstring(L, MAKO_VER);
   lua_setfield(L, -2, "version");
   lua_pushstring(L, __DATE__);
   lua_setfield(L, -2, "date");
#ifdef BASLIB_VER
   lua_pushstring(L, BASLIB_VER);
   lua_setfield(L, -2, "BAS");
#endif

   lua_newtable(L);
   for(i=1 ; i < argc; i++)
   {
      lua_pushstring(L, argv[i]);
      lua_rawseti(L, -2, i);
   }
   lua_setfield(L, -2, "argv");

   if(envp)
   {
      lua_newtable(L);
      for(ptr=*envp ; ptr; ptr=*++envp)
      {
         char* v = strchr(ptr, '=');
         if(v)
         {
            lua_pushlstring(L, ptr, v-ptr);
            lua_pushstring(L, v+1);
            lua_rawset(L, -3);
         }
      }
      lua_setfield(L, -2, "env");
   }
   if(cfgfname)
   {
      lua_pushstring(L, cfgfname);
      lua_setfield(L, -2, "cfgfname");
      free(cfgfname);
   }
   if(execpath)
   {
      lua_pushstring(L, execpath);
      lua_setfield(L, -2, "execpath");
   }
   if(getcwd(buf, sizeof(buf)))
   {
      lua_pushstring(L, buf);
      lua_setfield(L, -2, "cwd");
   }
   lua_pushboolean(L, daemonMode ? TRUE : FALSE);
   lua_setfield(L, -2, "daemon");

   balua_pushbatab(L);
   luaL_setfuncs(L, funcs, 1);

   if( daemonMode == 0 )
   {
      lua_pushliteral(L, "exit");
      lua_pushcfunction(L, mako_exit);
      lua_rawset(L, -3);
   }

   lua_setglobal(L, "mako");

};


static void
printUsage()
{
   static const char usage[]={
"Usage: " APPNAME " [options]\n"
"\nOptions:\n"
" -l[name]:[priority]:app  - Load one or multiple applications\n"
" -c configfile            - Load configuration file\n"
" -? -h                    - print this help message\n"
#ifdef BA_WIN32
" -install                 - Installs the service\n"
" -installauto             - Installs the service for autostart\n"
" -remove                  - Removes the service\n"
" -start                   - Starts the service\n"
" -stop                    - Stops the service\n"
" -restart                 - Stops and starts the service\n"
" -autostart               - Changes the service to automatic start\n"
" -manual                  - Changes the service to manual start\n"
" -state                   - Print the service state\n"
"-minimize                 - Minimize the console window\n"
"-elevate                  - Request elevated privileges\n"
#else
" -d                       - Run in daemon mode by detatching from the console\n"
" -s                       - Run in daemon mode without detatching from the console\n"
" -u username              - Username to run as\n"
#endif
   };
   fprintf(stderr,"%s",usage);
   exit(1);
}


static IoIntf*
checkMakoIo(IoIntf* io, const char* name)
{
   IoStat sb;
   if( ! io->statFp(io, ".config", &sb) &&
       ! io->statFp(io, ".openports", &sb))
   {
      makoprintf(FALSE,"Mounting %s\n",name);
      return io;
   }
   makoprintf(
      TRUE,"%s is missing .config or .openports\n",name);
   return 0;
}

static IoIntf*
createAndCheckMakoZipIo(ZipReader* zipReader, const char* name)
{
   if(CspReader_isValid((CspReader*)zipReader))
   {
      ZipIo* vmIo=(ZipIo*)baMalloc(sizeof(ZipIo));
      ZipIo_constructor(vmIo, zipReader, 2048, 0);
      if(ZipIo_getECode(vmIo) ==  ZipErr_NoError)
      {
         if(checkMakoIo((IoIntf*)vmIo,name))
            return (IoIntf*)vmIo;
      }
      ZipIo_destructor(vmIo);
      baFree(vmIo);
   }
   return 0;
}


static IoIntf*
openMakoZip(IoIntf* rootIo, const char* path)
{
   IoStat st;
   IoIntf* io=0;
   int plen=iStrlen(path);
   char* buf=(char*)baMalloc(plen+12);
   basprintf(buf,"%s%smako.zip",
             path, path[plen-1] == *LUA_DIRSEP ? "" : LUA_DIRSEP);
   if(*LUA_DIRSEP == '\\')
   {
      char* ptr=buf;
      while(*ptr)
      {
         if(*ptr == '\\')
            *ptr='/';
         ptr++;
      }
      if(buf[1]==':')
      {
         buf[1] = buf[0];
         buf[0]='/';
      }
   }
   baElideDotDot(buf);
   if( ! rootIo->statFp(rootIo, buf, &st) )
   {
      makoZipReader = baMalloc(sizeof(IoIntfZipReader));
      IoIntfZipReader_constructor(makoZipReader, rootIo, buf);
      io=createAndCheckMakoZipIo((ZipReader*)makoZipReader, buf);
      if(!io)
      {
         IoIntfZipReader_destructor(makoZipReader);
         baFree(makoZipReader);
         makoZipReader=0;
      }
   }
   baFree(buf);
   return io;
}



static IoIntf*
openVmIo(IoIntf* rootIo,const char* execpath, const char* cfgfname)
{
   int status;
   IoIntf* io=0;
   char* path=getenv("MAKO_ZIP");
   if(path)
   {
      const char* nd="non-deployed mako.zip dir";
      DiskIo* dio = baMalloc(sizeof(DiskIo));
      DiskIo_constructor(dio);
      status=DiskIo_setRootDir(dio,path);
      if(!status)
      {
         if(checkMakoIo((IoIntf*)dio,path))
            return (IoIntf*)dio;
      }
      else
         makoprintf(TRUE,"Cannot open %s %s.\n",nd,path);
      baFree(dio);
   }
   if(execpath)
   {
      io=openMakoZip(rootIo,execpath);
      if(io)
         return io;
   }
   if(cfgfname)
   {
      char* ptr;
      path=baStrdup(cfgfname);
      ptr=strrchr(path,*LUA_DIRSEP);
      if(ptr) *ptr=0;
      else path=".";
      io=openMakoZip(rootIo,path);
      baFree(path);
      if(io)
         return io;
   }

   /*
     The following code opens the internal (embedded ZIP file), if
     finding/opening mako.zip fails.
   */
   {
#if defined(BAIO_DISK)
      /* Test only: Use filesystem, not internal ZIP for lsp dir */
      static const char* dirname="../../lsp";
      int ecode;
      DiskIo* vmIo=(DiskIo*)baMalloc(sizeof(DiskIo));
      DiskIo_constructor(vmIo);
      if( (ecode=DiskIo_setRootDir(vmIo, dirname)) != 0 &&
          (ecode=DiskIo_setRootDir(vmIo, dirname)) != 0)
      {
         errQuit("Cannot set DiskIo directory %s: %s\n",
                 dirname,
                 baErr2Str(ecode));
      }
      io=checkMakoIo((IoIntf*)vmIo, dirname);
#elif USE_EMBEDDED_ZIP
      /* Use embedded ZIP file, which is Created by the the BA tool: bin2c */
      io=createAndCheckMakoZipIo(
         getLspZipReader(),
         "'the limited embedded mako.zip'. "
         "Warning: key features will be missing!");
#endif
   }
   if(!io)
      errQuit("Fatal error: cannot find mako.zip.\n");
   return io;
}


#if defined(LUA_USE_LINUX) || defined(LUA_USE_MACOSX)
static void
seedSharkSSL(void)
{
   int i;
   U32 rndData[4];
   sharkssl_entropy((U32)baGetUnixTime());
   sharkssl_entropy(baGetMsClock());
   int f = open("/dev/random", O_RDONLY);
   if(f < 0) return;
   if(read(f, rndData, sizeof(rndData)) > 0)
   {
      for(i = 0 ; i < 4 ; i++)
         sharkssl_entropy(rndData[i]);
   }
   close(f);
}
#elif defined LUA_USE_WINDOWS
static void
seedSharkSSL(void)
{
   int i;
   unsigned int rnd;
   sharkssl_entropy((U32)baGetUnixTime());
   sharkssl_entropy(baGetMsClock());
   for(i = 0 ; i < 4 ; i++)
   {
      if( ! rand_s(&rnd) )
         sharkssl_entropy(rnd);
   }
}
#else
static void
seedSharkSSL(void)
{
   sharkssl_entropy((U32)baGetUnixTime());
   sharkssl_entropy(baGetMsClock());
}
#endif

static void
checkEndian()
{
   U16 endian;
   U8* ptr = (U8*)&endian;
#ifdef B_LITTLE_ENDIAN
   ptr[1]=0x11;
   ptr[0]=0x22;
#elif defined(B_BIG_ENDIAN)
   ptr[0]=0x11;
   ptr[1]=0x22;
#else
#error NO_ENDIAN
#endif
   if(endian != 0x1122)
      errQuit("Panic!!! Wrong endian\n");
} 


void
runMako(int isWinService, int argc, char* argv[], char* envp[])
{
   int ecode;
   int onunloadRef;
   SoDisp disp;
   HttpCmdThreadPool pool;
   static LThreadMgr ltMgr;
   NetIo netIo;
   BaLua_param blp; /* configuration parameters */
   char* cfgfname;
   char* execpath;
#ifdef BA_WIN32
   int sMode;
#endif
#ifdef USE_DLMALLOC
   static char poolBuf[4*1024*1024]; /* Set your required size */
#endif

#if USE_DBGMON
  L_restart:
   restart=FALSE;
#endif

   HttpTrace_setPrio(7); /* block level 8 and 9 */

#ifdef BA_WIN32
   sMode=0; /* 0: nothing, 1: install, 2: installauto, 3: start now */ 
#endif
#ifdef USE_DLMALLOC
   init_dlmalloc(poolBuf, poolBuf + sizeof(poolBuf)); 
#endif

   memset(&blp, 0, sizeof(blp));
   checkEndian();
   seedSharkSSL();

   dispatcher=&disp; /* The global ptr used by the CTRL-C signal handler */
   HttpTrace_setFLushCallback(writeHttpTrace);
   HttpServer_setErrHnd(serverErrHandler);
#ifdef BA_WIN32
   /* For dumb WebDAV clients trying to access non existing disks in Windows
    * i.e. empty CD ROM. Prevent server from popping up dialog 'error'.
    */
   SetErrorMode(SEM_FAILCRITICALERRORS|SEM_NOOPENFILEERRORBOX);
#endif
   if( ! isWinService )
      fprintf(stderr,"\n%s\n%s\n%s\n\n",MAKO_VNAME,MAKO_DATE,MAKO_CPR);
   if(isWinService)
      daemonMode=1;
   else
   {
      int i;
      for(i=1; i < argc ; i++)
      {
         char* ptr=argv[i];
         if(*ptr == '-')
         {
            ptr++;
            if( (*ptr == '?' || *ptr=='h') && !ptr[1])
               printUsage();
#ifdef BA_WIN32
            if(!strcmp("autostart",ptr))
            {
               mustBeAdmin();
               pxexit(prtok(serviceSetStartup(SERVICENAME, SERVICE_AUTO_START)
                            == SERVICE_AUTO_START,"set to auto start"),TRUE);
            }
            else if(!strcmp("install",ptr))
            {
               sMode=1;
            }
            else if(!strcmp("installauto",ptr))
            {
               sMode=2;
            }
            else if(!strcmp("manual",ptr))
            {
               mustBeAdmin();
               pxexit(prtok(
                         serviceSetStartup(SERVICENAME, SERVICE_DEMAND_START)
                         == SERVICE_DEMAND_START,"set to manual start"),TRUE);
            
            }
            else if(!strcmp("remove",ptr))
            {
               mustBeAdmin();
               pxexit(serviceRemove(SERVICENAME),TRUE);
            }
            else if(!strcmp("restart",ptr))
            {
               if(serviceState(SERVICENAME) != SERVICE_STOPPED &&
                  serviceStop(SERVICENAME) < 0)
               {
                  pxexit(1,TRUE);
               }
               sMode=3;
            }
            else if(!strcmp("start",ptr))
            {
               if(serviceState(SERVICENAME) == SERVICE_RUNNING)
                  pxexit(1,TRUE);
               sMode=3;
            }
            else if(!strcmp("state",ptr))
            {
               fprintf(stderr,"%s %s\n", SERVICENAME,
                       serviceStateDesc(serviceState(SERVICENAME)));
               exit(0);
            
            }
            else if(!strcmp("stop",ptr))
            {
               if(serviceState(SERVICENAME) != SERVICE_STOPPED)
               {
                  mustBeAdmin();
                  pxexit(prtok(serviceStop(SERVICENAME) ==SERVICE_STOPPED,
                               "stopped"),TRUE);
               }
               else
                  pxexit(1,TRUE);
            }
            else if(!strcmp("elevate",ptr))
               mustBeAdmin();
            else if(!strcmp("minimize",ptr))
               ShowWindow(GetConsoleWindow(), SW_MINIMIZE);
#else
            if( ! strcmp(ptr,"d") && daemonMode == 0)
               daemonMode = 1; /* init.d or command line */
            if( ! strcmp(ptr,"s") && daemonMode == 0)
               daemonMode = -1; /* systemd */
#endif
         }
      }
   }

#ifdef BA_WIN32
   if(!isWinService && sMode)
   {
      int status=0;
      mustBeAdmin();
      if(sMode == 3)
      {
         status=prtok(serviceStart(SERVICENAME,argc,argv)
                      == SERVICE_RUNNING,"started");
      }
      else if(serviceInstall(SERVICENAME,sMode==2,argc,argv))
         status=1;
      else if(sMode==2)
      {
         status=prtok(serviceStart(SERVICENAME,argc,argv)
                      == SERVICE_RUNNING,"started");
      }
      pxexit(status,TRUE);
   }
#else
   if(daemonMode == 1) /* -d option */
      setLinuxDaemonMode();
   else
      disableSignals();
#endif
   if( ! isWinService )
      setCtrlCHandler();

   /* Create the Socket dispatcher (SoDisp), the SoDisp mutex, and the server.
    */
   cfgParams=baMalloc(sizeof(ConfigParams));
   cfgParams->trackerNodes=100;
   cfgParams->maxlogin=4;
   cfgParams->bantime=10*60;

   /* Root of local file system  -- io named "disk" */
   DiskIo_constructor(&diskIo);

   ThreadMutex_constructor(&mutex);
   SoDisp_constructor(&disp, &mutex);
   cfgfname=createServer(argc,argv,&execpath);
   if(!execpath)
      execpath=findExecPath(argc>0?argv[0]:0);
   if(0 != daemonMode)
      openTraceFile(0);

   /* Create an LSP virtual machine.
    */
   blp.vmio = openVmIo((IoIntf*)&diskIo,execpath,cfgfname);
   blp.server = &server;           /* pointer to a HttpServer */
   blp.timer = &timer;             /* Pointer to a BaTimer */
   L = balua_create(&blp);        /* create the Lua state */

   lua_gc(L, LUA_GCSTOP, 0);  /* stop collector during initialization */
   createLuaGlobals(argc,argv,envp,cfgfname,execpath);
   lua_gc(L, LUA_GCRESTART, 0);

   balua_usertracker_create(
      L, cfgParams->trackerNodes, cfgParams->maxlogin, cfgParams->bantime);
   baFree(cfgParams);

   /*** Add optional IO interfaces
        In Lua, use ba.openio(name), where name is net,disk,home from
        names below:
    ***/

   /* Install standard Mako I/O interfaces */
   NetIo_constructor(&netIo, &disp);
   balua_iointf(L, "net",  (IoIntf*)&netIo);
   balua_iointf(L, "disk",  (IoIntf*)&diskIo);
#ifdef MAKO_HOME_DIR
   balua_iointf(L, "home",  homeIo);
#endif

   balua_http(L); /* Install optional HTTP client library */
   LThreadMgr_constructor(&ltMgr,  /* Install optional Lua thread library */
      &server,
      ThreadPrioNormal,
      BA_STACKSZ,
      3, /* Number of threads */
      L,
      TRUE); /* allow creating more threads */
   balua_socket(L);  /* Install optional Lua socket library */
   balua_sharkssl(L);  /* Install optional Lua SharkSSL library */
   balua_crypto(L);  /* Install optional crypto library */
   balua_tracelogger(L); /* Install optional trace logger  */
   myCustomBindings(L);

   /* Install optional SQL bindings */
   luaopen_SQL(L);

#if USE_REDIRECTOR
   luaopen_ba_redirector(L); /* a type of reverse proxy */
#endif
#if USE_FORKPTY
   balua_forkpty(L); /* The optional forkpty Lua bindings for Linux */
#endif
#if USE_UBJSON
   balua_ubjson(L);
#endif
#if USE_LPEG
   luaL_requiref(L, "lpeg", luaopen_lpeg, FALSE);
   lua_pop(L,1); /* Pop lpeg obj: statically loaded, not dynamically. */
#endif
#if USE_PROTOBUF
   /* Google's protobuf for Lua: https://github.com/starwing/lua-protobuf */
   luaL_requiref(L, "pb", luaopen_pb, FALSE);
   lua_pop(L,1); /* Pop pb obj: statically loaded, not dynamically. */
#endif
#if USE_DBGMON
   /* Add Lua debugger */
   ba_ldbgmon(L, dbgExitRequest, 0);
#endif

   balua_tokengen(L); /* See  the "SharkTrustEx" comment above */
#if USE_REVCON
   /* Add reverse server connection. This requires SharkTrustEx.
    */
   balua_revcon(L);
#endif

#if USE_OPCUA
   luaopen_opcua_ns0_static(L);
#endif

   /* Dispatcher mutex must be locked when running the .openports script
    */
   ThreadMutex_set(&mutex);

   /* (Ref-ports)
      Open web server listen ports by calling the Lua script
      '.openports'. Notice that we do this on Linux before downgrading
      from user root to 'user'. Only root can open ports below 1024 on
      Linux (REF-user).
   */
   ecode=balua_loadconfig(L, blp.vmio, ".openports");

   ThreadMutex_release(&mutex);
   if(ecode)
      errQuit(".openports error: %s.\n", lua_tostring(L,-1)); 

   /* On Linux, optionally downgrade from root to 'user'
      When run as: sudo mako -u `whoami`
   */
   setUser(argc,argv);

   /* Dispatcher mutex must be locked when running the .config script
    */
   ThreadMutex_set(&mutex);
   ecode=balua_loadconfigExt(L, blp.vmio, 0, 1); /* Load and run .config  */
   ThreadMutex_release(&mutex);

#if USE_DBGMON
   if( ! restart )
   {
      if(ecode)
         errQuit(".config error: %s.\n", lua_tostring(L,-1));
      if(!lua_isfunction(L, -1))
         errQuit(".config error: no onunload");
      onunloadRef=luaL_ref(L,LUA_REGISTRYINDEX);
   }
   else
#endif
      onunloadRef=0;

   /* See (A) above */
   HttpCmdThreadPool_constructor(&pool, &server, ThreadPrioNormal, BA_STACKSZ);

   /*
     The dispatcher object waits for incoming HTTP requests. These
     requests are sent to the HttpServer object, where they are delegated to
     a Barracuda resource such as the WebDAV instance.
   */
   if( ! disp.doExit ) /* Can be set by debugger */
   {
      /* Arg -1: Never returns unless CTRL-C handler (or debugger monitor)
          sets exit
      */
      SoDisp_run(&disp,-1);
   }

   /*Dispatcher mutex must be locked when terminating the following objects.*/
   ThreadMutex_set(&mutex);
   /* Graceful termination of Lua apps. See function above. */
   if(onunloadRef)
      onunload(onunloadRef);
   LThreadMgr_destructor(&ltMgr); /* Wait for threads to exit */
   HttpCmdThreadPool_destructor(&pool);  /* Wait for threads to exit */

   /* Must cleanup all sessions before destroying the Lua VM */
   HttpServer_termAllSessions(&server);
   /* Destroy all objects, including server listening objects. */
   balua_close(L);

   IoIntf_destructor(blp.vmio); /* Virtual destr */
   baFree(blp.vmio);
#ifdef MAKO_HOME_DIR
   IoIntf_destructor(homeIo); /* Virtual destr */
#endif
   DiskIo_destructor(&diskIo);
   NetIo_destructor(&netIo);
   if(makoZipReader)
   {
      IoIntfZipReader_destructor(makoZipReader);
      baFree(makoZipReader);
   }
   BaTimer_destructor(&timer);

   HttpServer_destructor(&server);
   SoDisp_destructor(&disp);

   ThreadMutex_release(&mutex);
   ThreadMutex_destructor(&mutex);
   if(daemonMode)
      makoprintf(TRUE,"Terminating Mako Server\n");
   HttpTrace_flush();
   HttpTrace_close();
   if(execpath) baFree(execpath);
   dispatcher=0; /* Ref-wait */
   if(logFp)
   {
      fclose(logFp);
      logFp=0;
   }
#if USE_DBGMON
   if(restart)
      goto L_restart;
#endif


   return;
}



#if !defined(_WIN32) && !defined(CUSTOM_PLAT) && !defined(NO_MAIN)
int
main(int argc, char* argv[], char* envp[])
{
   runMako(FALSE,argc, argv, envp);
   return 0;
}
#endif
