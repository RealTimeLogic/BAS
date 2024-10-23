/* 
  Example Overview:
   This code demonstrates how to:
    1. Create Lua bindings to interface with C code
    2. Utilize the custom Xedge configuration file API for
       reading/writing the Xedge configuration file
    3. Add additional secrets for TPM key generation
    4. Integrate an embedded, read-only ZIP file system
*/

#include "xedge.h"
#include <sys/stat.h>

/*
  Ex 1. Create Lua bindings to interface with C code.

  The LED_ functions show how to create a very simple Lua binding for
  controlling one LED.

   Code below copied from:
        https://tutorial.realtimelogic.com/Lua-Bindings.lsp

  Lua binding tutorials:
    https://realtimelogic.com/ba/doc/?url=GettingStarted.html#UsingLSP
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

/**************  END LED API CODE **************************/


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
   if(lua_isstring(L, 1)) /* Write */
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

ZipReader* hello(void); /* Ex 4: See auto generated code below */

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
   const char secret[] = {'Q','W','E','R','T','Y'}; /* NO TRAILING ZERO EX. */
   aux->addSecret(aux, secret, sizeof(secret)); /* Send secret to Lua code */
   aux->addSecret(aux, "You can add any number of secrets", 33);
#endif

   /* Ex 4: Add an embedded ZIP file (read only file system)
      The content of hello.txt (see embedded zip file below) can be
      printed using Lua as follows:
      print(ba.mkio"hello-handle":open"hello.txt":read"a")
   */
   balua_installZIO(aux->L, "hello-handle", hello());

   return 0; /* OK */
}


/* Ex 4: Add an embedded ZIP file (read only file system)
   The C code below was generated as follows:
   echo hello > hello.txt
   zip hello.zip hello.txt
   bin2c -z hello hello.zip hello.c

   Ref bin2c: https://realtimelogic.com/downloads/bin2c/

   The content from hello.c can be found below:
*/


/****** The C array below contains data from file: hello.zip *****/
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
