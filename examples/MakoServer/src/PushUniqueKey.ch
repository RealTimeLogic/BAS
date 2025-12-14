/*
   PushUniqueKey()
  
   Pushes a device-unique key onto the Lua stack. This value is combined with
   the TPM master key so that each device ends up with a unique TPM root,
   even when all devices share the same key in EncryptionKey.h. In other words,
   the returned string is not a security secret by itself, but it provides
   per-device diversification for the TPM implementation.
  
   Supported platforms
     - Linux / Windows / Mac / QNX:
  
   The function gathers a stable per-device hardware identifier from
   the underlying platform. The C code can then feed this value into
   the TPM master-key derivation logic, ensuring that the resulting
   TPM key material becomes unique per device.
*/  

#ifdef _WIN32
static int pushUniqueKey(lua_State*L)
{
   int retVal=-1;
   HKEY hKey;
   LONG result;
   DWORD dwType = REG_SZ;
   char guid[256];
   DWORD dwSize = sizeof(guid);
   result = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                         "SOFTWARE\\Microsoft\\Cryptography",
                         0,
                         KEY_READ | KEY_WOW64_64KEY,
                         &hKey);
   if(result == ERROR_SUCCESS)
   {
      result = RegQueryValueEx(hKey,
                               "MachineGuid",
                               NULL,
                               &dwType,
                               (LPBYTE)guid,
                               &dwSize);
      if (result == ERROR_SUCCESS)
      {
         retVal=0;
         lua_pushstring(L,guid);
      }
      RegCloseKey(hKey);
   }
   return retVal;
}
#elif defined(_OSX_)
#include <IOKit/IOKitLib.h>
#include <CoreFoundation/CoreFoundation.h>
static int pushUniqueKey(lua_State*L)
{
   int retVal=-1;
   io_registry_entry_t ioRegistryRoot = IORegistryEntryFromPath(
      kIOMainPortDefault, "IOService:/");
   if(ioRegistryRoot != MACH_PORT_NULL)
   {
      CFStringRef uuidString=IORegistryEntryCreateCFProperty(
         ioRegistryRoot, CFSTR("IOPlatformUUID"), kCFAllocatorDefault, 0);
      if(uuidString)
      {
         CFIndex length = CFStringGetLength(uuidString);
         CFIndex maxLength = CFStringGetMaximumSizeForEncoding(
            length, kCFStringEncodingUTF8) + 1;
         char* binaryData = (char*)malloc(maxLength);
         if (CFStringGetCString(
                uuidString,(char*)binaryData,maxLength,kCFStringEncodingUTF8))
         {
            retVal=0;
            /* -1: Exclude the null terminator */
            lua_pushlstring(L,binaryData,maxLength - 1);
         }
         free(binaryData);
         CFRelease(uuidString);
      }
      IOObjectRelease(ioRegistryRoot);
   }
   return retVal;
}
#elif defined(__QNX__)
#include <ifaddrs.h>
#include <net/if.h>
#include <net/if_dl.h>
#include <net/if_types.h>
static int pushUniqueKey(lua_State*L)
{
   struct ifaddrs *ifap, *ifa;
   int retVal=-1;
   if(0 == getifaddrs(&ifap))
   {
      for (ifa = ifap; ifa != NULL; ifa = ifa->ifa_next)
      {
         if (ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_LINK)
         {
            struct sockaddr_dl* sdl = (struct sockaddr_dl *)ifa->ifa_addr;      
            if (sdl->sdl_type == IFT_ETHER && sdl->sdl_alen == 6)
            {
               unsigned char *mac = (unsigned char *)LLADDR(sdl);
               lua_pushlstring(L,mac,6);
               retVal=0;
               break; /* use first interface */
            }
         }
      }
      freeifaddrs(ifap);
   }
   if(retVal)
      lua_pushliteral(L,"");
   return retVal;
}
#else /* Linux */
static int pushUniqueKey(lua_State*L)
{
   int retVal=-1;
   FILE *file = fopen("/etc/machine-id", "r");
   if(file)
   {
      char guid[64];
      if(fgets(guid, sizeof(guid), file) != NULL)
      {
         retVal=0;
         lua_pushstring(L,guid);
      }
      fclose(file);
   }
   return retVal;
}
#endif
