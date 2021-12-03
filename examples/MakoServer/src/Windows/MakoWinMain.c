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
 *   $Id: MakoWinMain.c 4914 2021-12-01 18:24:30Z wini $
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
#include "../mako.h"
//#include "messages.h" /* Ref-EV */
#include <stdio.h>
#include <io.h>
#include <stdlib.h>
#include <shlobj.h>   
#include <direct.h>
#include <sys/stat.h>
#include <process.h>
#include <shellapi.h>


static SERVICE_STATUS_HANDLE statusHandle;
static SERVICE_STATUS	serviceStatus;
static BOOL isService=0;


static BOOL
RunElevated()
{
   TCHAR szPath[ MAX_PATH ];
   BOOL status;
   SHELLEXECUTEINFO shex;
   char* cmd=GetCommandLine();
   int len = strlen(cmd) + 100;
   char* params=malloc(len);
   basnprintf(params,len,"-pause %s",cmd);
   GetModuleFileName(NULL, szPath, MAX_PATH);
   memset(&shex, 0, sizeof( shex));
   shex.cbSize        = sizeof( SHELLEXECUTEINFO );
   shex.fMask        = 0;
   shex.hwnd        = 0;
   shex.lpVerb        = "runas";
   shex.lpFile        = szPath;
   shex.lpParameters    = params;
   shex.lpDirectory    = 0;
   shex.nShow        = SW_NORMAL;
   status=ShellExecuteEx(&shex);
   free(params);
   return status;
}


static int
extractArgv(char*** xargv)
{
   SC_HANDLE   schSCManager;
   int argc=0;
   if((schSCManager = OpenSCManager(NULL,NULL,SC_MANAGER_CONNECT))!=0)
   {
      SC_HANDLE   schService;
      if((schService =
          OpenService(schSCManager, SERVICENAME,SERVICE_ALL_ACCESS)) !=0)
      {
         LPQUERY_SERVICE_CONFIG lpServiceConfig;
         DWORD pcbBytesNeeded;
         lpServiceConfig=(LPQUERY_SERVICE_CONFIG)malloc(1024*8);
         if(QueryServiceConfig(
               schService,
               lpServiceConfig,
               1024*8,
               &pcbBytesNeeded))
         {
            char** argv;
            char* start;
            char* end;
            //printf("%s\n",lpServiceConfig->lpBinaryPathName);
            for(argc=0,start=lpServiceConfig->lpBinaryPathName;;argc++)
            {
               if(!(start=strchr(start,'"')))
                  break;
               if(!(end=strchr(++start,'"')))
               {
                  argc=0;
                  break;
               }
               start=end+1;
            }
            if(argc>0)
            {
               argv=(char**)malloc(sizeof(void*)*argc);
               for(argc=0,start=lpServiceConfig->lpBinaryPathName;;argc++)
               {
                  char* ptr;
                  int len;
                  if(!(start=strchr(start,'"')))
                     break;
                  end=strchr(++start,'"');
                  baAssert(end);
                  len=(int)(end-start);
                  ptr=malloc(len+1);
                  memcpy(ptr,start,len+1);
                  ptr[len]=0;
                  argv[argc]=ptr;
                  start=end+1;
               }
               *xargv=argv;
            }
         }
         else
            serviceErrPrintf("QueryServiceConfig failed");
         CloseServiceHandle(schService);
      }
      CloseServiceHandle(schSCManager);
   }
   return argc;
}


static void
startWinSock(void)
{
   WORD wVersionRequested;
   WSADATA wsaData;
   wVersionRequested = MAKEWORD( 2, 0 ); 
   if(WSAStartup( wVersionRequested, &wsaData ))
   {
      serviceErrPrintf("Error: winsock start err\n");
      exit(999);
   }
}


void
mustBeAdmin(void)
{
   BOOL b;
   SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
   PSID AdministratorsGroup; 
   b = AllocateAndInitializeSid(
      &NtAuthority,
      2,
      SECURITY_BUILTIN_DOMAIN_RID,
      DOMAIN_ALIAS_RID_ADMINS,
      0, 0, 0, 0, 0, 0,
      &AdministratorsGroup); 
   if(b) 
   {
      if (!CheckTokenMembership( NULL, AdministratorsGroup, &b)) 
      {
         b = FALSE;
      } 
      FreeSid(AdministratorsGroup); 
   }

   if(!b)
   {
      if( ! RunElevated() )
      {
         fprintf(stderr,
                 "The requested command requires administrator privileges.\n");
         exit(1);
      }
      exit(0);
   }
}


/* Ref-EV
   Event logging tutorial:
   http://www.codeproject.com/Articles/4166/Using-MC-exe-message-resources-and-the-NT-event-lo
   Installs our app as a source of events
   under the name pszName into the registry
*/
static void
addEventSource(PCTSTR pszName, DWORD dwCategoryCount)
{
   HKEY    hRegKey = NULL; 
   DWORD   dwError = 0;
   DWORD dwTypes;
   TCHAR   szPath[ MAX_PATH ];
        
   basnprintf(szPath, MAX_PATH,
           "SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application\\%s", 
           pszName);

   /* Create the event source registry key */
   dwError = RegCreateKey(HKEY_LOCAL_MACHINE, szPath, &hRegKey);

   /* Name of the PE module that contains the message resource */
   GetModuleFileName(NULL, szPath, MAX_PATH);

   /* Register EventMessageFile */
   dwError = RegSetValueEx(hRegKey, 
                           "EventMessageFile", 0, REG_EXPAND_SZ, 
                           (PBYTE) szPath, (DWORD)(strlen( szPath) + 1)); 
        

   /* Register supported event types */
   dwTypes = EVENTLOG_ERROR_TYPE | 
      EVENTLOG_WARNING_TYPE | EVENTLOG_INFORMATION_TYPE; 
   dwError = RegSetValueEx(hRegKey, "TypesSupported", 0, REG_DWORD, 
                            (LPBYTE) &dwTypes, sizeof dwTypes);

   /* If we want to support event categories, we have also to register
    the CategoryMessageFile.  and set CategoryCount. Note that
    categories need to have the message ids 1 to CategoryCount!
   */
   if( dwCategoryCount > 0 )
   {
      dwError = RegSetValueEx(hRegKey, "CategoryMessageFile", 
                               0, REG_EXPAND_SZ, (PBYTE) szPath, 
                               (DWORD)strlen( szPath)+ 1);

      dwError = RegSetValueEx(hRegKey, "CategoryCount", 0, REG_DWORD, 
                               (PBYTE) &dwCategoryCount,
                               sizeof dwCategoryCount);
   }
   RegCloseKey(hRegKey);
} 


/* This function sould technically only be run one time, but it's ok
 * running it at service install.
 */
void
instalMakoEventSource(void)
{
   addEventSource(SERVICENAME, 0);
}



/* Used by servicePrintf and serviceErrPrintf.
 * Print to console or Windows event log.
 */
void
serviceVPrintf(int isErr, const char* fmt, va_list argList)
{
   if(isService)
   {
#if 0
      /* Ref-EV */
      HANDLE h=RegisterEventSource(NULL, SERVICENAME);
      if(h)
      {
         char* buf=(char*)malloc(4000);
         if(buf)
         {
            const char* str=buf;
            vsnprintf(buf, 4000, fmt, argList);
            ReportEvent(h,
                        isErr ? EVENTLOG_ERROR_TYPE : EVENTLOG_INFORMATION_TYPE,
                        0,                    /* event category */
                        isErr ? SERVICE_ERROR : SERVICE_INFO, /* event ID */
                        NULL,                 /* current user's SID */
                        1,                    /* strings in lpszStrings */
                        0,                    /* no bytes of raw data */
                        &str,                 /* array of error strings */
                        NULL);                /* no raw data */
            DeregisterEventSource(h);
            free(buf);
         }
      }
#endif
   }
   else
      vfprintf(stderr,fmt,argList);
}



static void APIENTRY
serviceCtrlHandler(DWORD opcode)
{
   switch(opcode) {
      case SERVICE_CONTROL_STOP:
         setDispExit();
         Sleep(3000); /* wait for dispatchers */
         servicePrintf("%s stopped",SERVICENAME);
         serviceStatus.dwCurrentState  = SERVICE_STOPPED;
         break;

      case SERVICE_CONTROL_SHUTDOWN:
         setDispExit();
         Sleep(1000); /* wait for dispatchers */
         servicePrintf("%s stopping due to shutdown",SERVICENAME);
         serviceStatus.dwCurrentState  = SERVICE_STOPPED;
         break;

      case SERVICE_CONTROL_INTERROGATE:
      default:
         break;
   }

   /* Send current serviceStatus. */
   SetServiceStatus(statusHandle,  &serviceStatus);
}


/*
**	Called via enterService when we start the service
*/
static void APIENTRY
serviceMain(DWORD argc, char** argv)
{

   serviceStatus.dwServiceType        = SERVICE_WIN32;
   serviceStatus.dwCurrentState       = SERVICE_START_PENDING;
   serviceStatus.dwControlsAccepted   =
      SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
   serviceStatus.dwWin32ExitCode      = 0;
   serviceStatus.dwServiceSpecificExitCode = 0;
   serviceStatus.dwCheckPoint         = 0;
   serviceStatus.dwWaitHint           = 0;

   Sleep(30);	 /* thread breathing */

   statusHandle=RegisterServiceCtrlHandler(SERVICENAME,serviceCtrlHandler);
   if (statusHandle == (SERVICE_STATUS_HANDLE)0)
   {
      serviceErrPrintf("Failed to register service %s"SERVICENAME);
      return;
   }

   /* Initialization complete - report running status */
   serviceStatus.dwCurrentState       = SERVICE_RUNNING;
   serviceStatus.dwCheckPoint         = 0;
   serviceStatus.dwWaitHint           = 0;
   SetServiceStatus(statusHandle, &serviceStatus);
   
   /* Enable if you need the program to halt and wait for the debugger
    * to attach to the service.
    */
   //while(!IsDebuggerPresent()) Sleep(100);
   argc=extractArgv(&argv);
   startWinSock();
   runMako(TRUE,argc,argv,NULL);
   Sleep(100); /* thread breathing */
   
   /* Exiting service */
   if(serviceStatus.dwCurrentState != SERVICE_STOPPED)
   {
      serviceStatus.dwCurrentState = SERVICE_STOPPED;
      SetServiceStatus(statusHandle,  &serviceStatus);
   }
}


/* Called from "main" if running in "service" mode i.e. not console mode.
 */
static int
enterService(int argc, char **argv)
{
   SERVICE_TABLE_ENTRY dispatchTable[] = {
      {SERVICENAME, serviceMain},
      {NULL, NULL } };
   if(!StartServiceCtrlDispatcher(dispatchTable))
   {
      serviceErrPrintf("StartServiceCtrlDispatcher() failed");
      return -1;
   }
   return 0;
}


/* Returns true if running as a service and false if running in console mode.
 */
static int
isInServiceMode()
{
   int isService = FALSE;

   HWINSTA hWinStation = GetProcessWindowStation();
   if (hWinStation != NULL)
   {     
      USEROBJECTFLAGS uof = {0};     
      if (GetUserObjectInformation(
             hWinStation, UOI_FLAGS, &uof, sizeof(USEROBJECTFLAGS), NULL)
          && ((uof.dwFlags & WSF_VISIBLE) == 0))
      {
         isService = TRUE;
      }
   }
   return isService;
}


/* Prints a message if ok is true */
int
prtok(int ok, const char* okmsg)
{
   if(ok)
   {
      if (!okmsg) okmsg="";
      servicePrintf("%s %s\n",SERVICENAME,okmsg);
      return 0;
   }
   return ok ? 0 : -1;
}


/* Entry point for service and console mode */
int
main(int argc, char *argv[], char* envp[])
{
   isService=isInServiceMode();
   if(isService)
      return enterService(argc, argv);
   startWinSock();
   return runMako(FALSE,argc,argv,envp);
}
