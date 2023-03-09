/*
The following example is part of the Advanced Lua Bindings tutorial
and demonstrates how to call Lua code asynchronously from C code. This
example receives UDP messages on ports 8080 and 8090, forwarding these
events to Lua if the relevant Lua functions have been created (example 1) and
installed (example 2).

Introduction:
https://realtimelogic.com/ba/doc/en/C/reference/html/md_en_C_md_LuaBindings.html#AsynchC2Lua

Example 1:
The example listening on port 8080, referred to as 'ex1' below,
expects a global Lua function to be defined. If the function is found,
the example will call this function. To create this function, add the
following Lua code to the .preload script of a Lua application using
Xedge:

function _G.udpmsg(msg)
    trace("Global func received:", msg)
end

You can also insert this code into an LSP page to experiment with the
incoming data. Repeatedly refreshing the LSP page will replace the
previous function with a new function.

Example 2:
The example listening on port 8090, referred to as 'ex2' below, expects
a Lua callback function to be installed. To install the callback
function, use the following code:

UDPTST.install(function(msg)
    trace("CB received:", msg)
end)

You can insert this code into an LSP page as well. Calling the
UDPTST.install() function repeatedly will replace the previous
callback with the new callback.

The following Python script can be used for sending UDP broadcast
messages on port 8080 and generate UDP events for example 1. To test
example 2, change the port number in the Python script to 8090.

Here is the Python script:

#!/usr/bin/env python3
import socket
import time
client = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
client.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEPORT, 1)
client.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
client.settimeout(0.2)
cnt=1
while True:
    message = "UDP Message %d" % cnt
    cnt=cnt+1
    client.sendto(message.encode('utf-8'), ("255.255.255.255", 8080))
    print(message, flush=True)
    time.sleep(1)
*/

#include <barracuda.h>
#include <stdlib.h>

#define PORT_EX1 8080 /* UDP listen port number for Example 1 */
#define PORT_EX2 8090
#define MAXLINE 1024

/* The LThreadMgr configured in xedge.c */
extern LThreadMgr ltMgr;
/* The Socket Dispatcher (SoDisp) mutex protecting everything. */
ThreadMutex* soDispMutex;
/* Lua REGISTRY index reference to callback installed when calling
 * UDPTST.install(). See function installUdpCallbackEx2() below.
 */
static int luaFuncRefEx2;

/* This callback is called by one of the threads managed by LThreadMgr
 * when a job is taken off the queue and executed. The callback
 * attempts to find the global Lua function 'udpmsg', and if the
 * function is found, it will be executed.
 */
static void runLuaEx1(ThreadJob* job, int msgh, LThreadMgr* mgr)
{
   lua_State* L = job->Lt;
   lua_pushglobaltable(L);
   lua_getfield(L, -1, "udpmsg");
   if (lua_isfunction(L, -1))
   {
      lua_pushstring(L, (char*)(job + 1));
      lua_pcall(L, 1, 0, msgh);
   }
   else
   {
      HttpTrace_printf(0, "Err: global lua function 'udpmsg' missing\n");
   }
}

/* This callback is similar to runLuaEx1() above, but instead of
 * looking up a global Lua function, it uses the Lua function
 * referenced by the luaFuncRefEx2 variable. This variable is set when
 * Lua code calls UDPTST.install().
 */
static void runLuaEx2(ThreadJob* job, int msgh, LThreadMgr* mgr)
{
   if(luaFuncRefEx2)
   {
      lua_State* L = job->Lt;
      lua_rawgeti(L, LUA_REGISTRYINDEX, luaFuncRefEx2);
      baAssert(lua_isfunction(L, -1));
      lua_pushstring(L, (char*)(job + 1));
      lua_pcall(L, 1, 0, msgh);
   }
   else
   {
      HttpTrace_printf(0,"Err: Lua callback function 'udpmsg' not installed\n");
   }
}


/* This is a socket dispatcher (SoDisp) callback function. The
 * function is called by the SoDisp instance when we receive an UDP
 * message. The function is responsible for reading socket data,
 * creating a job by calling ThreadJob_lcreate, and dispatching the
 * job by calling LThreadMgr_run. This function is used by both
 * example 1 and example 2 code.
 */
static void asyncDispRecEv(SoDispCon* con, int port, ThreadJob_LRun callback)
{
   char buffer[MAXLINE];
   struct sockaddr_in sin;
   socklen_t len = sizeof(sin);
   memset(&sin, 0, sizeof(sin));
   sin.sin_family = AF_INET;
   sin.sin_port = htons(port);
   sin.sin_addr.s_addr = INADDR_ANY;
   int n = recvfrom(con->httpSocket.hndl, (char *)buffer, MAXLINE-1, 
                    0, ( struct sockaddr *) &sin,
                    &len);
   if(n > 0)
   {
      ThreadJob* job;
      buffer[n] = 0;
      job=ThreadJob_lcreate(sizeof(ThreadJob)+n+1, callback);
      if(job)
      {
         strcpy((char*)(job + 1), buffer); /* copy received message to job */
         /* We do not need to lock the mutex since it was already
          * locked by SoDisp. This code validates that the SoDisp
          * thread is the owner.
          */
         baAssert(ThreadMutex_isOwner(soDispMutex));
         LThreadMgr_run(&ltMgr, job);
      }
   }
}

/* SoDisp callback for example 1
 */
static void asyncDispRecEvEx1(SoDispCon* con)
{
   asyncDispRecEv(con, PORT_EX1, runLuaEx1);
}

/* SoDisp callback for example 2
 */
static void asyncDispRecEvEx2(SoDispCon* con)
{
   asyncDispRecEv(con, PORT_EX2, runLuaEx2);
}

/* This function opens a UDP socket on a specified port number and
 * performs the necessary steps for installing the socket in the
 * socket dispatcher SoDisp. Although this function is required for
 * this example to function properly, you do not need to comprehend
 * the details of its implementation, since the main focus of this
 * example is to demonstrate how to use the LThreadMgr.
 */
static void openServerSock(int port,SoDispCon_DispRecEv callback)
{
   SoDispCon* con;
   struct sockaddr_in servaddr;
   int sockfd;
   if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
      perror("socket creation failed");
      exit(1);
   }
   memset(&servaddr, 0, sizeof(servaddr));
   servaddr.sin_family = AF_INET;
   servaddr.sin_addr.s_addr = INADDR_ANY;
   servaddr.sin_port = htons(port);
   if(bind(sockfd,(const struct sockaddr *)&servaddr,sizeof(servaddr)) < 0)
   {
      perror("bind failed");
      exit(2);
   }
   /* Quick and dirty; memory never released. */
   con = (SoDispCon*)baMalloc(sizeof(SoDispCon));
   SoDispCon_constructor(con, ltMgr.server->dispatcher, callback);
   con->httpSocket.hndl = sockfd;
   SoDisp_addConnection(ltMgr.server->dispatcher, con);
   SoDisp_activateRec(ltMgr.server->dispatcher, con);
}

/* Open and install the two UDP socket objects used in this example.
 */
static void initSocketServer()
{
   openServerSock(PORT_EX1, asyncDispRecEvEx1);
   openServerSock(PORT_EX2, asyncDispRecEvEx2);
}

/* This Lua binding is registered by the luaopen_AUX() function below
 * and is invoked when Lua code calls UDPTST.install(). The binding
 * saves a reference to the specified function (argument 1) in the
 * luaFuncRefEx2 variable, which is used by the runLuaEx2() function
 * to push the function onto the Lua stack.
 */
static int installUdpCallbackEx2(lua_State* L)
{
   if(lua_isfunction(L, 1))
   {
      if(luaFuncRefEx2)
      {  /* Release old callback */
         luaL_unref(L, LUA_REGISTRYINDEX, luaFuncRefEx2);
      }
      lua_settop(L, 1); /* Make sure we only have one arg. */
      /* Save reference to function in registry */
      luaFuncRefEx2=luaL_ref(L, LUA_REGISTRYINDEX);
   }
   else
      luaL_typeerror(L, 1, "function");
   return 0;
}


/* This function installs the Lua binding, enabling Lua code to call
 * UDPTST.install(). The code employs standard Lua syntax, which is
 * included in any literature that explains Lua bindings. The function
 * is called by the Xedge startup code.
*/
void luaopen_AUX(lua_State* L)
{
   soDispMutex = HttpServer_getMutex(ltMgr.server);
   initSocketServer();
   static const luaL_Reg reg[] = {
      {"install", installUdpCallbackEx2},
      {NULL, NULL}
   };

   luaL_newlib(L, reg);
   lua_setglobal(L, "UDPTST");
}
