/* 
  This example shows how to create a very simple Lua binding for
  controlling one LED.

   Code below copied from:
        https://embedded-app-server.info/Lua-Bindings.lsp

  Lua binding tutorials:
    https://realtimelogic.com/ba/doc/?url=GettingStarted.html#UsingLSP
    https://realtimelogic.com/ba/doc/en/C/reference/html/md_en_C_md_LuaBindings.html

 Auto generation:
    https://realtimelogic.com/swig/
*/


#include <barracuda.h>

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

/*
  The function below is called by the LspAppMgr startup code.
*/
void luaopen_LED(lua_State* L)
{
   static const luaL_Reg reg[] = {
      {"setLed", LED_setLed},
      {"getLed", LED_getLed},
      {NULL, NULL}
   };

   luaL_newlib(L, reg);
   lua_setglobal(L, "LED");
}
