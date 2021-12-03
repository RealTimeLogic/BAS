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
 *
 *   $Id: servutil.c 4914 2021-12-01 18:24:30Z wini $
 *
 *   COPYRIGHT:  Real Time Logic LLC, 2012
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
 *               http://www.realtimelogic.com
 ****************************************************************************
 *
 */

#include "servutil.h"
#include <stdio.h>


void
servicePrintf(const char* fmt, ...)
{
   va_list varg;
   va_start(varg, fmt);
   serviceVPrintf(FALSE,fmt, varg);
   va_end(varg);  
}


static void
internalErrPrintf(const char* fmt, ...)
{
   va_list varg;
   va_start(varg, fmt);
   serviceVPrintf(TRUE,fmt, varg);
   va_end(varg);  
}


void
serviceErrPrintf(const char* fmt, ...)
{
   va_list varg;
   char* buf=(char*)malloc(4000);
   if(buf)
   {
      char* ptr;
      DWORD ecode;
      DWORD len=200;
      strcpy(buf,"Error: ");
      ptr=buf+strlen(buf);
      va_start(varg, fmt);
      vsprintf(ptr, fmt, varg);
      va_end(varg);
      if( (ecode=GetLastError()) != 0)
      {
         strcat(ptr,": ");
         ptr=ptr+strlen(ptr);
         len = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM ,
                             NULL,
                             ecode,
                             MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                             ptr, len, NULL);
         if(len)
         {
            while(len>0 && (iscntrl(ptr[len-1]) || isspace(ptr[len-1]))) len--;
            ptr[len]=0;
         }
      }
      internalErrPrintf("%s\n", buf);
      free(buf);
   }
}


typedef unsigned long ulong;


int
serviceStart(const char* servname, int argc, const char ** argv)
{
   SC_HANDLE   schService;
   SC_HANDLE   schSCManager;
   SERVICE_STATUS ServStatus;
   char modname[MAX_PATH+1];
   int lasterr=0;
   char** xargv=0;

   ZeroMemory(&ServStatus,sizeof(ServStatus));
   if(!(schSCManager = OpenSCManager(
           NULL,                   // machine (NULL == local)
           NULL,                   // database (NULL == default)
           SC_MANAGER_CONNECT      // access required
           )))
   {
      return 0;
   }
   if(!(schService = OpenService(schSCManager, servname,
                                 SERVICE_START | SERVICE_QUERY_STATUS)) )
   {
      serviceErrPrintf("OpenService failed");
      CloseServiceHandle(schSCManager);
      return 0;
   }
   if( !QueryServiceStatus( schService, &ServStatus ) )
   {
      serviceErrPrintf("QueryServiceStatus failed");
      CloseServiceHandle(schService);
      CloseServiceHandle(schSCManager);
      return 0;
   }
   if(ServStatus.dwCurrentState == SERVICE_RUNNING ||
      ServStatus.dwCurrentState == SERVICE_START_PENDING)
   {
      CloseServiceHandle(schService);
      CloseServiceHandle(schSCManager);
      return ServStatus.dwCurrentState;
   }
   if(argv && argc > 1)
   {
      int i;
      xargv=(char**)malloc((argc)*sizeof(void*));
      xargv[0]=modname;
      GetModuleFileName(NULL, modname, MAX_PATH);
      for(i=1; i < argc; i++) xargv[i]=(char*)argv[i];
      argv=xargv;
   }
   else
   {
      argc=0;
      argv=0;
   }
   if(!StartService(schService, argc, argv))
   {
      serviceErrPrintf("StartService failed");
      CloseServiceHandle(schService);
      CloseServiceHandle(schSCManager);
      return 0;
   }
   if(xargv) free(xargv);
   {
      int cnt = 30;
      Sleep( 33 );
      while ( cnt--)
      {
         if(!QueryServiceStatus( schService, &ServStatus) )
         {
            lasterr=GetLastError();
            break;
         }
         if( ServStatus.dwCurrentState == SERVICE_START_PENDING ){
            Sleep( 33 );
         }
         else  break;
      }
   }
   if(ServStatus.dwCurrentState == SERVICE_START_PENDING)
   {
      lasterr = ERROR_SERVICE_REQUEST_TIMEOUT;
   }

   CloseServiceHandle(schService);
   CloseServiceHandle(schSCManager);
   if(lasterr)
   {
      SetLastError(lasterr);
      serviceErrPrintf("serviceStart failed");
   }
   return ServStatus.dwCurrentState;
}


/* Allow service control start stop query etc
** This does not allow the creation/deletion etc of services
** action is one of the API defined values e.g.
**  SERVICE_START, SERVICE_START,
*/
int
serviceControl(const char* servname, int action, int access)
{
   SC_HANDLE   schService;
   SC_HANDLE   schSCManager;
   SERVICE_STATUS ServStatus;
   int lasterr=0;
   ZeroMemory(&ServStatus,sizeof(ServStatus));
   schSCManager = OpenSCManager(
      NULL,                   // machine (NULL == local)
      NULL,                   // database (NULL == default)
      SC_MANAGER_CONNECT      // access required
      );
   if( !schSCManager )
   {
      return -1;
   }
   schService = OpenService(schSCManager, servname,
                            access | SERVICE_QUERY_STATUS);
   if( !schService )
   {
      serviceErrPrintf("OpenService failed");
      CloseServiceHandle(schSCManager);
      return -1;
   }
   switch (action)
   {
      case SERVICE_CONTROL_STOP:
      case SERVICE_CONTROL_PAUSE:
      case SERVICE_CONTROL_CONTINUE:
         break;
      case 0:
         if( !QueryServiceStatus( schService, &ServStatus ) )
         {
            serviceErrPrintf("QueryServiceStatus failed");
            CloseServiceHandle(schService);
            CloseServiceHandle(schSCManager);
            return 0;
         }
         CloseServiceHandle(schService);
         CloseServiceHandle(schSCManager);
         SetLastError(lasterr);
         return ServStatus.dwCurrentState;
      default:
         if(action >= 128 && action <=255)
         {
            if( !ControlService( schService, action, &ServStatus ) )
            {
               lasterr=GetLastError();
            }
            ServStatus.dwCurrentState=0;
            if(!QueryServiceStatus( schService, &ServStatus) )
            {
               lasterr=GetLastError();
            }
         }
         CloseServiceHandle(schService);
         CloseServiceHandle(schSCManager);
         SetLastError(lasterr);
         return ServStatus.dwCurrentState;
         break;

   }
   if( ControlService( schService, action, &ServStatus ) )
   {
      int cnt = 33;
      Sleep(100);
      while ( cnt--)
      {
         if(!QueryServiceStatus( schService, &ServStatus) )
         {
            lasterr=GetLastError();
            break;
         }
         if( ServStatus.dwCurrentState != action ){
            Sleep(30);
         }
         else  break;
      }
   }
   else
      lasterr=GetLastError();

   CloseServiceHandle(schService);
   CloseServiceHandle(schSCManager);
   if(lasterr)
   {
      SetLastError(lasterr);
      serviceErrPrintf("serviceControl failed");
   }
   return ServStatus.dwCurrentState;
}


int
serviceStop(const char* servname)
{
   return serviceControl(servname, SERVICE_CONTROL_STOP, SERVICE_STOP);
}


int
servicePause(const char* servname)
{
   return serviceControl(
      servname, SERVICE_CONTROL_PAUSE, SERVICE_PAUSE_CONTINUE);
}


int
serviceContinue(const char* servname)
{
   return serviceControl(
      servname, SERVICE_CONTROL_CONTINUE, SERVICE_PAUSE_CONTINUE);
}


int
serviceUsercode(const char* servname, int action)
{
   if(action < 128 || action > 255) return 0;
   return serviceControl(servname, action,
                         SERVICE_USER_DEFINED_CONTROL);
}


int
serviceState(const char* servname)
{
   return serviceControl(servname, 0,0);
}


const char*
serviceStateDesc(int state)
{
   static const char* st[] =
      {
         "state unknown",
         "stopped",
         "start pending",
         "stop pending",
         "running",
         "continue pending",
         "pause pending",
         "paused","\0"};

   if(state < 1 || state > 7) state=0;
   return st[state];
}

int
serviceGetStartup(const char* servname)
{
   SC_HANDLE   schService;
   SC_HANDLE   schSCManager;
   LPQUERY_SERVICE_CONFIG pq;
   char* pb;
   ulong sz, dwBytesNeeded;
   int startType;

   schSCManager = OpenSCManager(
      NULL,                   // machine (NULL == local)
      NULL,                   // database (NULL == default)
      SC_MANAGER_CONNECT      // access required
      );
   if( !schSCManager )
   {
      serviceErrPrintf("OpenSCManager failed");
      return -1;
   }

   schService = OpenService(schSCManager, servname,
                            SERVICE_QUERY_CONFIG);
   if( !schService )
   {
      serviceErrPrintf("OpenService failed");
      CloseServiceHandle(schSCManager);
      return -1;
   }

   if(QueryServiceConfig(schService, NULL, 0L, &dwBytesNeeded)
      == ERROR_INSUFFICIENT_BUFFER) 
   {
      pb = malloc(dwBytesNeeded);
      if(!pb) return -1;
   }
   else
      return -1;
   sz = dwBytesNeeded;
   pq = (LPQUERY_SERVICE_CONFIG)pb ;
   if(QueryServiceConfig(schService, pq, sz, &dwBytesNeeded)) 
   {
      serviceErrPrintf("QueryServiceConfig failed");
      free(pb);
      CloseServiceHandle(schService);
      CloseServiceHandle(schSCManager);
      return -1;
   }

   startType = pq->dwStartType;

   free(pb);


   CloseServiceHandle(schService);
   CloseServiceHandle(schSCManager);
   return startType;
}


int
serviceSetStartup(const char* servname, int startType)
{
   SC_HANDLE   schService;
   SC_HANDLE   schSCManager;
   SC_LOCK sclLock;
   SERVICE_STATUS ServStatus;

   switch (startType)
   {
      case SERVICE_DISABLED :
      case SERVICE_DEMAND_START:
      case SERVICE_AUTO_START:
      case SERVICE_BOOT_START:
      case SERVICE_SYSTEM_START:
         break;
      default:
         return -1;
   }

   ZeroMemory(&ServStatus,sizeof(ServStatus));
   schSCManager = OpenSCManager(
      NULL,                   // machine (NULL == local)
      NULL,                   // database (NULL == default)
      SC_MANAGER_ALL_ACCESS   // access required
      );
   if( !schSCManager )
   {
      return -1;
   }

   sclLock = LockServiceDatabase(schSCManager);

   // If the database cannot be locked, return with an error
   if(sclLock == NULL)
   {
      return -1;
   }


   schService = OpenService(schSCManager, servname,
                            SERVICE_CHANGE_CONFIG);
   if( !schService )
   {
      serviceErrPrintf("OpenService failed");
      UnlockServiceDatabase(sclLock);
      CloseServiceHandle(schSCManager);
      return -1;
   }

   if(!ChangeServiceConfig(
         schService,        // handle of service
         SERVICE_NO_CHANGE, // service type: no change
         startType,        // change service start type
         SERVICE_NO_CHANGE, // error control: no change
         NULL,              // binary path: no change
         NULL,              // load order group: no change
         NULL,              // tag ID: no change
         NULL,              // dependencies: no change
         NULL,              // account name: no change
         NULL,              // password: no change
         NULL) )            // display name: no change
   
   {
      serviceErrPrintf("ChangeServiceConfig failed");
      UnlockServiceDatabase(sclLock);
      CloseServiceHandle(schService);
      CloseServiceHandle(schSCManager);
      return -1;
   }
   UnlockServiceDatabase(sclLock);
   CloseServiceHandle(schService);
   CloseServiceHandle(schSCManager);
   return startType;
}



int
serviceInstall(
   const char* servicename, int autostart, int argc, const char ** argv)
{
   char modname[MAX_PATH+1];
   int retVal=-1;
   if(GetModuleFileName(NULL, modname, MAX_PATH))
   {
      char* path;
      int size;
      int i;
      for(i=1,size=0 ; i < argc; i++) size += (int)(strlen(argv[i])+3);
      path=malloc(strlen(modname) + size + 3);
      if(path)
      {
         SC_HANDLE schSCManager =
            OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
         sprintf(path,"\"%s\"",modname);
         for(i=1 ; i < argc; i++)
            sprintf(path+strlen(path)," \"%s\"",argv[i]);
         if(schSCManager)
         {
            SC_HANDLE schService = CreateService(
               schSCManager,              /* SCManager database */
               servicename,               /* name of service */
               servicename,               /* name to display */
               SERVICE_ALL_ACCESS,        /* desired access */
               SERVICE_WIN32_OWN_PROCESS, /* service type */
               autostart ?
               SERVICE_AUTO_START :
               SERVICE_DEMAND_START,      /* start type */
               SERVICE_ERROR_NORMAL,      /* error control type */
               path,                    /* service's binary */
               NULL,                      /* no load ordering group */
               NULL,                      /* no tag identifier */
               "Tcpip\0\0",               /* dependencies */
               NULL,                      /* LocalSystem account */
               NULL);                     /* no password */
            if(schService)
            {
               servicePrintf("%s was installed.\n",servicename);
               CloseServiceHandle(schService);
               retVal=0;
            }
            else
               serviceErrPrintf("CreateService failed");
            CloseServiceHandle(schSCManager);
         }
         else
            serviceErrPrintf("OpenSCManager() failed");
         free(path);
      }
   }
   return retVal;
}


int
serviceRemove(const char* servicename)
{
   SC_HANDLE   schService;
   SC_HANDLE   schSCManager;
   int ret = -1;
   int st = serviceState(servicename);
   if(st < 0) return -1;
   if (st && st == SERVICE_RUNNING)
   {
      servicePrintf("Stopping %s...\n",servicename);
      if(serviceStop(servicename) < 0) return -1;
   }
   schSCManager = OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
   if(schSCManager)
   {
      schService = OpenService(schSCManager, servicename, SERVICE_ALL_ACCESS);
      if(schService)
      {
         if( DeleteService(schService) )
         {
            servicePrintf("%s was removed.\n", servicename);
            ret = 0;
         }
         else
            serviceErrPrintf("DeleteService failed");
         CloseServiceHandle(schService);
      }
      else
         serviceErrPrintf("OpenService failed");
      CloseServiceHandle(schSCManager);
   }
   else
      serviceErrPrintf("OpenSCManager failed");
   return ret;
}
