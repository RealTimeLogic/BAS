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
 *   $Id: Main.c 5065 2022-01-31 23:53:26Z wini $
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
  This file contains the main entry point for HLOS builds. You must
  replace this code with a thread/task in an embedded system.
*/

#include <stdio.h>
#include <stdlib.h>
#ifdef _WIN32
#include <direct.h>
#elif !__CODEWARRIOR__
#include <unistd.h>
#endif
#include <BaServerLib.h>
#include<string.h>
#include <sys/stat.h>

extern void hostInit(void);
extern void barracuda(void);
#ifdef USE_DLMALLOC
extern void init_dlmalloc(char* heapstart, char* heapend);
#endif

/* Changes the current directory to the object directory.

   This functionality is strictly not needed, but simplfifies using
   the examples. The example programs, when using the DiskIo, are
   designed to run from the object directory and will fail if run from
   another directory.
*/
static void
change2ObjDir(const char* argv0)
{
   struct stat st;
#ifdef _WIN32
   char* path=0;
#else
   (void)argv0;
#endif
   const char* cpath="obj";
   if( ! stat(cpath,&st) )
   {
      const char* cpath2="obj/debug";
      if( ! stat(cpath2,&st) ) cpath = cpath2;
   }
   else
   {
      cpath="../obj";
      if( ! stat(cpath,&st) )
      {
         const char* cpath2="obj/debug";
         if( ! stat(cpath2,&st) ) cpath = cpath2;
      }
      else
      {
#ifdef _WIN32
         char* ptr;
         cpath=0;
         path = baStrdup(argv0);
         ptr = strrchr(path, '\\');
         if(ptr)
         {
            *ptr=0;
            if( ! stat(path,&st) ) cpath=path;
         }
#else
         cpath=0;
#endif
      }
   }
   if(cpath)
      printf("Changing directory to: %s%s\n",cpath,chdir(cpath) ? " FAILED!!!" : "");
   else
      printf("Warning: cannot find the object directory\n");
#ifdef _WIN32
   if(path) baFree(path);
#endif
}


int
main(int argc, char* argv[])
{
#ifdef USE_DLMALLOC
   static char poolBuf[3 * 1024 * 1024];
   init_dlmalloc(poolBuf, poolBuf + sizeof(poolBuf));
#endif

   change2ObjDir(argv[0]);

   hostInit(); /* See HostInit.c */
   barracuda(); /* See barracuda.c. Never returns */
   return 0; /* Remove compiler warning */
}
