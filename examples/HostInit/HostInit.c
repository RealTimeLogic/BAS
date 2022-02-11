/*
 *     ____             _________                __                _     
 *    / __ \___  ____ _/ /_  __(_)___ ___  ___  / /   ____  ____ _(_)____
 *   / /_/ / _ \/ __ `/ / / / / / __ `__ \/ _ \/ /   / __ \/ __ `/ / ___/
 *  / _, _/  __/ /_/ / / / / / / / / / / /  __/ /___/ /_/ / /_/ / / /__  
 * /_/ |_|\___/\__,_/_/ /_/ /_/_/ /_/ /_/\___/_____/\____/\__, /_/\___/  
 *                                                       /____/          
 *
 *                  Barracuda Embedded Web-Server 
 ****************************************************************************
 *
 *   $Id: HostInit.c 4914 2021-12-01 18:24:30Z wini $
 *
 *   COPYRIGHT:  Real Time Logic
 *               https://realtimelogic.com
 *
 *   The copyright to the program herein is the property of
 *   Real Time Logic. The program may be used or copied only
 *   with the written permission from Real Time Logic or
 *   in accordance with the terms and conditions stipulated in
 *   the agreement under which the program has been supplied.
 ****************************************************************************
 *
 */

/*
  This file contains code for initializing host (HLOS) specific parts required
  by the examples.

  This code also sets up a default error handler and a trace callback.

  Similar functions should be constructed for monolithic RTOS devices.
*/

#include <HttpTrace.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <zlib.h>
#include <BaServerLib.h>

#if defined(_POSIX) && !defined(BA_VXWORKS)
#define BAUSEPOSIXSIGS
#include "PosixSigs.h"
#endif

static FILE* traceFp=NULL;

static void
displayFatalError(const char* eMsg,
                  BaFatalErrorCodes ecode1,
                  unsigned int ecode2,
                  const char* file,
                  int line)
{
   printf("%s, ecode1=%d, ecode2=%d, file=%s, line=%d\n",
          eMsg,(int)ecode1,ecode2,file,line);
   printf("See trace for more info\n");
   HttpTrace_flush();
   if(traceFp)
      fclose(traceFp);
#ifdef _WIN32
   printf("Press enter to abort\n");
   getchar();
#endif
   abort();
}

static void
myErrHandler(BaFatalErrorCodes ecode1,
             unsigned int ecode2,
             const char* file,
             int line)
{
   displayFatalError("Fatal error in Barracuda", ecode1, ecode2, file, line);
}


static void
writeHttpTrace(char* buf, int bufLen)
{
   static int useConsole=1;
   if(!traceFp)
   {
      const char* c=getenv("BA_CONSOLE");
      if(c && !baStrCaseCmp(c, "FALSE") )
         useConsole=0;
      printf("Opening trace file HttpTrace.txt\n");
      if(useConsole)
      {
         printf("Trace is also echoed to console."
                " Disable by setting BA_CONSOLE=FALSE.\n");
      }
      traceFp = fopen("HttpTrace.txt", "a");
      if( !traceFp )
      {
         static int isRunning=0;
         if(isRunning)
            return;
         isRunning=1;
         displayFatalError("Cannot open HttpTrace.txt",
                           FE_USER_ERROR_1,
                           errno,
                           __FILE__,
                           __LINE__);
      }
      fprintf(traceFp, "------------  Started %lu\n", time(0));
   }
   if(useConsole)
   {
      buf[bufLen]=0; /* Safe. See documentation. */
      printf("%s",buf);
   }
   fwrite(buf,bufLen,1,traceFp);
   fflush(traceFp);
}


void
hostInit(void)
{
#ifdef _WIN32
   //If using VC.
   //Start windows socket layer.
   WORD wVersionRequested;
   WSADATA wsaData;
   wVersionRequested = MAKEWORD( 2, 0 ); 
   if(WSAStartup( wVersionRequested, &wsaData ))
   {
      displayFatalError("Cannot start windows socket library",
                        FE_USER_ERROR_1,
                        errno,
                        __FILE__,
                        __LINE__);
   }
#endif


#ifdef BAUSEPOSIXSIGS
   blocksigs();
#endif


   HttpTrace_setFLushCallback(writeHttpTrace);
/*
   HttpTrace_setRequest(TRUE);
*/
   HttpServer_setErrHnd(myErrHandler);
}
