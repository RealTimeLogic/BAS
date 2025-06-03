/*
 * This software may only be used in accordance with the terms and
 * conditions stipulated in the corresponding license agreement under
 * which it has been supplied, or, at your option, under the terms of
 * the MIT License.
 *
 * The MIT License text follows below.
 *
 * No attribution or contribution is required when copying, modifying,
 * or redistributing this file.
 *
 * MIT License:
 * Copyright (c) 2025 Real Time Logic
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */



/* 
  Example Overview:
   This code demonstrates how to:
    1. Create Lua bindings to interface with C code
    2. Utilize the custom Xedge configuration file API for
       reading/writing the Xedge configuration file
    3. Add additional secrets for TPM key generation
    4. Integrate an embedded, read-only ZIP file system
    5. Send the time event 'sntp' to the Xedge Lua code
*/

#include "xedge.h"
#include <sys/stat.h>

/* The LThreadMgr configured in xedge.c */
extern LThreadMgr ltMgr;


/*
  Ex 1. Create Lua bindings to interface with C code.

  The LED_ functions show how to create a very simple Lua binding for
  controlling one LED.

   Code below copied from:
        https://tutorial.realtimelogic.com/Lua-Bindings.lsp

  Lua binding tutorials:
    https://tutorial.realtimelogic.com/Lua-Bindings.lsp
    https://realtimelogic.com/ba/doc/en/introduction.html#UsingLSP
    https://realtimelogic.com/ba/doc/en/C/reference/html/md_en_C_md_LuaBindings.html

 Auto generation:
    https://realtimelogic.com/swig/
*/

/********* Simulated HW **********/
static int ledState=0;
static void setLed(int val)
{
   ledState=val;
   printf("Set simulated LED %s\n", ledState ? "on" : "off");
}


static int getLed(void)
{
   printf("Get simulated LED state (%s)\n", ledState ? "on" : "off");
   return ledState;
}
/********* End simulated HW **********/


static int LED_setLed(lua_State* L)
{
   int on = lua_toboolean(L, 1); /* val at stack position 1 */
   setLed(on); /* Call the LED function that is interfacing the HW */
   return 0; /* No Lua return value */
}

static int LED_getLed(lua_State* L)
{
   /* Call the LED function that get's the LED state from the HW */
   int on = getLed();
   /* Push the on/off state onto the Lua stack */
   lua_pushboolean(L, on);
   return 1; /* Inform Lua that we have one return value */
}

/**************  END LED API CODE (Ex 1) **************************/


/* 
   Ex 2. Utilize the custom Xedge configuration file API for
      reading/writing the Xedge configuration file.

   This optional feature can be utilized when the code is compiled
   with NO_BAIO_DISK (no file system API) or to enhance the security
   of the Xedge configuration file. When a file system is included
   with Xedge, the configuration file is stored as a backup on the
   file system. Enabling this security enhancement is recommended,
   particularly when an Xedge-based firmware release includes the
   Xedge IDE. If both the Xedge config file API and the file system
   are enabled, the Xedge Lua code ensures that the configuration file
   is replicated and automatically restores either file if it becomes
   corrupt or missing. In this example, we store the configuration
   file on the file system; however, for a firmware release build, we
   recommend using a separate flash partition, ideally within the
   microcontroller's flash memory, to maximize security.
   Note: this file can be read only.
 */
static int xedgeCfgFile(lua_State* L)
{
   FILE* fp;
   static const char* fn = "Xedge-configuration-file";
   if(lua_isstring(L, 1)) /* Write (optional feature) */
   {
      size_t size;
      const char* data=lua_tolstring(L, 1, &size);
      fp = fopen(fn, "wb");
      if(fp)
      {
         fwrite(data, size, 1, fp);
         fclose(fp);
         lua_pushboolean(L, TRUE);
         return 1;
      }
   }
   else  /* Read */
   {
      struct stat st;
      if( ! stat(fn, &st) )
      {
         fp = fopen(fn, "rb");
         if(fp)
         {
            luaL_Buffer b;
            char* data = luaL_buffinitsize(L,&b,st.st_size);
            fread(data, st.st_size, 1, fp);
            fclose(fp);
            luaL_addsize(&b, st.st_size);
            luaL_pushresult(&b);
            return 1;
         }
      }
   }
   /* Failed: return nil,error message */
   lua_pushnil(L);
   lua_pushliteral(L,"OOPS");
   return 2;
}


/* Ex 4: Add an embedded ZIP file (read only file system)
   The C code below was generated as follows:
   echo hello > hello.txt
   zip hello.zip hello.txt
   bin2c -z hello hello.zip hello.c

   Ref bin2c: https://realtimelogic.com/downloads/bin2c/

   The content from hello.c can be found below:
*/


/* The C array below and the two following functions were copied from
 * the generated file hello.zip.
*/
static const U8 cspPages[] = {
(U8)0x50,(U8)0x4B,(U8)0x03,(U8)0x04,(U8)0x14,(U8)0x00,(U8)0x00,(U8)0x00
,(U8)0x00,(U8)0x00,(U8)0x5C,(U8)0x75,(U8)0x50,(U8)0x59,(U8)0x86,(U8)0xA6
,(U8)0x10,(U8)0x36,(U8)0x05,(U8)0x00,(U8)0x00,(U8)0x00,(U8)0x05,(U8)0x00
,(U8)0x00,(U8)0x00,(U8)0x09,(U8)0x00,(U8)0x00,(U8)0x00,(U8)0x68,(U8)0x65
,(U8)0x6C,(U8)0x6C,(U8)0x6F,(U8)0x2E,(U8)0x74,(U8)0x78,(U8)0x74,(U8)0x68
,(U8)0x65,(U8)0x6C,(U8)0x6C,(U8)0x6F,(U8)0x50,(U8)0x4B,(U8)0x01,(U8)0x02
,(U8)0x14,(U8)0x00,(U8)0x14,(U8)0x00,(U8)0x00,(U8)0x00,(U8)0x00,(U8)0x00
,(U8)0x5C,(U8)0x75,(U8)0x50,(U8)0x59,(U8)0x86,(U8)0xA6,(U8)0x10,(U8)0x36
,(U8)0x05,(U8)0x00,(U8)0x00,(U8)0x00,(U8)0x05,(U8)0x00,(U8)0x00,(U8)0x00
,(U8)0x09,(U8)0x00,(U8)0x00,(U8)0x00,(U8)0x00,(U8)0x00,(U8)0x00,(U8)0x00
,(U8)0x00,(U8)0x00,(U8)0x20,(U8)0x00,(U8)0x00,(U8)0x00,(U8)0x00,(U8)0x00
,(U8)0x00,(U8)0x00,(U8)0x68,(U8)0x65,(U8)0x6C,(U8)0x6C,(U8)0x6F,(U8)0x2E
,(U8)0x74,(U8)0x78,(U8)0x74,(U8)0x50,(U8)0x4B,(U8)0x05,(U8)0x06,(U8)0x00
,(U8)0x00,(U8)0x00,(U8)0x00,(U8)0x01,(U8)0x00,(U8)0x01,(U8)0x00,(U8)0x37
,(U8)0x00,(U8)0x00,(U8)0x00,(U8)0x2C,(U8)0x00,(U8)0x00,(U8)0x00,(U8)0x00
,(U8)0x00
};


static int
DataZipReader_diskRead(
   CspReader* o,void* data,U32 offset,U32 size,int blockStart)
{
   (void)o;
   (void)blockStart;
   memcpy(data, cspPages+offset, size);
   return 0;
}
ZipReader* hello(void)
{
   static ZipReader zipReader;
   ZipReader_constructor(&zipReader,DataZipReader_diskRead,sizeof(cspPages));
   CspReader_setIsValid(&zipReader);
   return &zipReader;
}

/***************************  END Ex 4 **************************/


/* 
 Ex 5: This example shows how to send the SNTP event to Xedge. A
 device without a clock must use an SNTP client to update the system
 time. See the following for an introduction:
 https://realtimelogic.com/ba/examples/xedge/readme.html#time

 The SNTP client C code may include an event mechanism for the C
 code to use; however, in the following example, we wait for the time
 to be set in a thread. When the time is set, the code calls the Lua
 '_XedgeEvent' function with the argument sntp to signal that we have
 the correct time.

 The code below uses the LThreadMgr object:
 https://realtimelogic.com/ba/doc/en/C/reference/html/md_en_C_md_LuaBindings.html#fullsolution
*/


/* This callback is called by one of the threads managed by LThreadMgr
 * when a job is taken off the queue and executed. The callback
 * attempts to find the global Lua function '_XedgeEvent', and if the
 * function is found, it will be executed as follows: _XedgeEvent("sntp")
 */
static void executeXedgeEvent(ThreadJob* job, int msgh, LThreadMgr* mgr)
{
   lua_State* L = job->Lt;
   lua_pushglobaltable(L); /* _G */
   lua_getfield(L, -1, "_XedgeEvent");
   if(lua_isfunction(L, -1))  /* Do we have _G._XedgeEvent */
   {
      /* Call _XedgeEvent("sntp") */
      lua_pushstring(L,"sntp"); /* Arg */
      lua_pcall(L, 1, 0, msgh); /* one arg, no return value */
   }
}


/* Thread started by xedgeOpenAUX() */
static void checkTimeThread(Thread* th)
{
   ThreadMutex* soDispMutex = HttpServer_getMutex(ltMgr.server);
   (void)th; /* not used */

   /* Use the compile time macros for date and time and convert the
    * date/time to a value that can be used by function baParseDate
    */
   const char* d = __DATE__;
   char buf[50];
   if (!(basnprintf(buf, sizeof(buf), "Mon, %c%c %c%c%c %s %s",
                    d[4],d[5], d[0],d[1],d[2], d + 7, __TIME__) < 0))
   {
      BaTime compileT = baParseDate(buf);
      if(compileT) /* If OK: Seconds since 1970 */
      {
         compileT -= 24*60*60; /* Give it one day for time zone adj. */
         /* Wait for time to be updated by NTP */
         while(baGetUnixTime() < compileT)
            Thread_sleep(500);
         /* Initiate executing the Lua func _XedgeEvent("sntp") */
         ThreadJob* job=ThreadJob_lcreate(sizeof(ThreadJob), executeXedgeEvent);
         ThreadMutex_set(soDispMutex);
         LThreadMgr_run(&ltMgr, job);
         ThreadMutex_release(soDispMutex);
      }
   }
   /* Exit thread */
}


/***************************  END Ex 5 **************************/

/*
  The function below is called by the Xedge startup code.
*/
int xedgeOpenAUX(XedgeOpenAUX* aux)
{
   static const luaL_Reg ledReg[] = {
      {"setLed", LED_setLed},
      {"getLed", LED_getLed},
      {NULL, NULL}
   };
   /* Ex1: Install the LED API as a global variable */
   luaL_newlib(aux->L, ledReg);
   lua_setglobal(aux->L, "LED");

   /* Ex2: Install the optional config file handler. See function and comments above */
   aux->xedgeCfgFile = xedgeCfgFile;

   /* Ex 3. Add additional secrets for TPM key generation
      Config TPM: https://realtimelogic.com/ba/examples/xedge/readme.html#security
      TPM API: https://realtimelogic.com/ba/doc/en/lua/auxlua.html#TPM
      When using the softTPM, in addition to the main secret you must
      set in EncryptionKey.h, additional secrets can be added to the
      logic in xedge.lua that calculates the pre-master key. At least
      one key should be a fixed ID specific to the device. It could be
      the Ethernet MAC address or any other unique ID. In the ESP32
      reference port, we use "eFUSE Registers". Note that you cannot
      use random generated data, as secret(s) must be persistent.
    */
#ifndef NO_ENCRYPTIONKEY
   const U8 secret[] = {'Q','W','E','R','T','Y'}; /* NO TRAILING ZERO EXAMPLE */
   aux->addSecret(aux, secret, sizeof(secret)); /* Send secret to Lua code */
   aux->addSecret(aux, "You can add any number of secrets", 33);
#endif

   /* Ex 4: Add an embedded ZIP file (read only file system)
      The content of hello.txt (see embedded zip file below) can be
      printed using Lua as follows:
      print(ba.mkio"hello-handle":open"hello.txt":read"a")
   */
   balua_installZIO(aux->L, "hello-handle", hello());

   /* Ex 5 */
   static Thread checkTime;
   Thread_constructor(&checkTime, checkTimeThread, ThreadPrioNormal, 2000);
   Thread_start(&checkTime);

   return 0; /* OK */
}
