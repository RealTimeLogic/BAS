/* 
  This example shows how to create a very simple Lua binding for
  controlling one LED.

   Code below copied from:
        https://tutorial.realtimelogic.com/Lua-Bindings.lsp

  Lua binding tutorials:
    https://realtimelogic.com/ba/doc/?url=GettingStarted.html#UsingLSP
    https://realtimelogic.com/ba/doc/en/C/reference/html/md_en_C_md_LuaBindings.html

 Auto generation:
    https://realtimelogic.com/swig/
*/


#include "xedge.h"
#include <sys/stat.h>

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


/* Optional enhancement: Read or write the Xedge configuration file.

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


/*
  The function below is called by the Xedge startup code.
*/
int xedgeOpenAUX(XedgeOpenAUX* aux)
{
   static const luaL_Reg reg[] = {
      {"setLed", LED_setLed},
      {"getLed", LED_getLed},
      {NULL, NULL}
   };
   /* Install this API as a global variable */
   luaL_newlib(aux->L, reg);
   lua_setglobal(aux->L, "LED");


   /* Install the optional config file handler. See comments above */
   aux->xedgeCfgFile = xedgeCfgFile;

   /* softTPM settings: https://realtimelogic.com/ba/doc/en/lua/auxlua.html#TPM
    * When using the softTPM, in addition to the main secret you must
    * set in EncryptionKey.h, additional secrets can be added to the
    * logic in xedge.lua that calculates the pre-master key. At least
    * one key should be a fixed ID specific to the device. It could be
    * the Ethernet MAC address or any other unique ID. In the ESP32
    * reference port, we use "eFUSE Registers". Note that you cannot
    * use random generated data, as secret(s) must be persistent.
    */
#ifndef NO_ENCRYPTIONKEY
   const char secret[] = {'Q','W','E','R','T','Y'}; /* NO TRAILING ZERO EX. */
   lua_pushlstring(aux->L,secret,sizeof(secret));
   aux->initXedge(aux->L, aux->initXedgeFuncRef); /* Send secret to Lua code */

   lua_pushstring(aux->L,"You can add any number of secret values");
   aux->initXedge(aux->L, aux->initXedgeFuncRef);
#endif

   return 0;
}
