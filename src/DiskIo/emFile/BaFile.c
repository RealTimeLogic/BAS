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
 *            PROGRAM MODULE
 *
 *   $Id: BaFile.c 4914 2021-12-01 18:24:30Z wini $
 *
 *   COPYRIGHT:  Real Time Logic, 2011
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


/*
This file contains DiskIo and BaFile implementations for emFile.

This implementation sets the root directory to '' by default.

The root directory can be change with DiskIo_setRootDir. This function
can be called at any time to change the root directory.

*/

#include <BaServerLib.h>
#define ROOTPT char*
#include <BaDiskIo.h>

#include <FS.h>



/* Converts 1980 format to the Unix style time in Barracuda.
 */
static BaTime
dateTime2UnixTime(U32 t)
{
   /* Quick and dirty, no leap years :-) */
   return t+60*60*24*365*10;
}


static char*
unix2DosPath(char* s)
{
   char* ptr;
#if FS_DIRECTORY_DELIMITER == '\\'
   for(ptr = s; *ptr; ptr++)
   {
      if(*ptr == '/')
         *ptr=FS_DIRECTORY_DELIMITER;
   }
#endif
   return s;
}


static int
doErr(int err)
{
   switch(err)
   {
      case FS_ERROR_ALLOC:
      case FS_ERROR_NOMEMORY:
         err = IOINTF_MEM;

      case FS_ERR_INVALIDPAR:
      case FS_ERR_CMDNOTSUPPORTED:
      case FS_ERROR_ILLEGAL_FORMAT_STRING:
      case FS_ERROR_UNKNOWN_DEVICE:
         err = IOINTF_INVALIDNAME;

      case FS_ERR_DISKFULL:
         err = IOINTF_NOSPACE;

      case FS_ERR_WRITEONLY:
      case FS_ERR_READONLY:
      case FS_ERR_READERROR:
      case FS_ERR_WRITEERROR:
         err = IOINTF_NOACCESS;

      #if 0  /* how to implement? */   
      case FS_ERR_FILE_ALREADY_OPENED:
         err = IOINTF_EXIST; 
      #endif   

      default:
         err = IOINTF_IOERROR; 
   }
   return err;
}


typedef struct DirIter
{
      DirIntf super;
      FS_FIND_DATA fileinfo;
      int notFound;
      char filename[512];
      BaBool isFirstTime;
}DirIter;


static int
DirIter_read(DirIntfPtr super)
{
   DirIter* o = (DirIter*)super;
   const char* an = o->fileinfo.sFileName;
   if(o->notFound)
      return IOINTF_NOTFOUND;
   if(o->isFirstTime)
   {
      o->isFirstTime=FALSE;
      if( ! (an[0]== '.' && (!an[1] || (an[1]=='.' && !an[2]))) )
         return 0;
   }
   for(;;)
   {
      o->notFound = FS_FindNextFile(&o->fileinfo);
      if(o->notFound)
         return IOINTF_NOTFOUND;
      an = o->fileinfo.sFileName;
      if(an[0]== '.' && (!an[1] || (an[1]=='.' && !an[2])))
         continue;
      return 0;
   }
}


static const char*
DirIter_getName(DirIntfPtr super)
{
   DirIter* o = (DirIter*)super;
   if(o->notFound)
      return 0;
   return (const char*)o->fileinfo.sFileName;
}




static int
DirIter_stat(DirIntfPtr super, IoStat* st)
{
   DirIter* o = (DirIter*)super;
   if(o->notFound)
      return IOINTF_NOTFOUND;
   st->lastModified = dateTime2UnixTime(o->fileinfo.LastWriteTime);
   st->isDir = o->fileinfo.Attributes & FS_ATTR_DIRECTORY ?
      TRUE : FALSE;
   st->size = o->fileinfo.FileSize;
   return 0;
}


static void
DirIter_constructor(DirIter* o, char* aname)
{
   DirIntf_constructor((DirIntf*)o,
                       DirIter_read,
                       DirIter_getName,
                       DirIter_stat);
   o->isFirstTime=TRUE;
   o->notFound = FS_FindFirstFile(
      &o->fileinfo, aname, o->filename, sizeof(&o->filename));
}


static void
DirIter_destructor(DirIter* o)
{
   if(o->notFound != -100)
   {
      FS_FindClose(&o->fileinfo);
      o->notFound=-100;
   }
}


typedef struct
{
   ResIntf super;
   FS_FILE* fh;
} DiskRes;


static int
DiskRes_read(ResIntfPtr super, void* buf, size_t maxSize, size_t* size)
{
   DiskRes* o = (DiskRes*)super; /* Downcast */
   if( (*size = FS_FRead(buf, 1, maxSize, o->fh)) == 0)
   {
      return FS_FEof(o->fh) ? IOINTF_EOF : doErr(FS_FError(o->fh));
   }
   return 0;
}


static int
DiskRes_write(ResIntfPtr super, const void* buf, size_t size)
{
   DiskRes* o = (DiskRes*)super; /* Downcast */
   return FS_FWrite((void*)buf, 1, size, o->fh) == size ?
      0 : doErr(FS_FError(o->fh));
}


static int
DiskRes_seek(ResIntfPtr super, BaFileSize offset)
{
   DiskRes* o = (DiskRes*)super; /* Downcast */
   return FS_FSeek(o->fh, offset, FS_SEEK_SET) ? doErr(FS_FError(o->fh)) : 0;
}


static int
DiskRes_flush(ResIntfPtr super)
{
   (void)super;
   return 0;
}


static int
DiskRes_close(ResIntfPtr super)
{
   FS_FILE* hndl;
   baAssert(super);
   if(!super) return -1;
   hndl = ((DiskRes*)super)->fh;
   baFree(super);
   super=0;
   return FS_FClose(hndl) ? doErr(FS_FError(hndl)) : 0;
}



static void
DiskRes_constructor(DiskRes* o, FS_FILE* hndl)
{
   ResIntf_constructor((ResIntf*)o,
                       DiskRes_read,
                       DiskRes_write,
                       DiskRes_seek,
                       DiskRes_flush,
                       DiskRes_close);
   o->fh=hndl;
}


static int DiskIo_stat(IoIntfPtr super, const char* name, IoStat* st);


/* Make absolute path from 'rootPath' and 'name'.
   Space to hold the returned string is obtained from baMalloc.
*/
static char*
DiskIo_mkAbsPath(DiskIo* o, const char* name, int extraChars, int* status)
{
   int len;
   char* aptr;

   /* The char \ is not seen as a dir sep by BA, thus a security problem */ 
   if(strchr(name, '\\'))
   {
      *status=IOINTF_INVALIDNAME;
      return 0;
   }
   if(*name == '/') name++;
   len = strlen(name)+1;
   aptr = baMalloc((o->rootPathLen+len+extraChars));
   if(aptr)
   {
      if(o->rootPath)
      {
         strcpy(aptr, o->rootPath);
         strcpy(aptr+o->rootPathLen, name);
      }
      else
         strcpy(aptr, name);
      return unix2DosPath(aptr);
   }
   *status=E_MALLOC;
   return 0;
}



static DirIntfPtr
DiskIo_openDir(IoIntfPtr super, const char* dirname,
               int* status, const char** ecode)
{
   DiskIo* o = (DiskIo*)super;
   *status=0;
   if(ecode) *ecode=0;

   IoStat st;
   if(DiskIo_stat(super, dirname, &st))
      *status=IOINTF_NOTFOUND;
   else if( ! st.isDir )
      *status=IOINTF_EXIST;
   else
   {
      char* aname = DiskIo_mkAbsPath(o, dirname, 1, status);
      if(aname)
      {
         DirIter* di = (DirIter*)baMalloc(sizeof(DirIter));
         if(di)
         {
            int len = strlen(aname)-1;
            if(len > 0 && aname[len] != '\\')
               strcat(aname, "\\");
            DirIter_constructor(di,aname);
         }
         else
            *status=IOINTF_MEM;
         baFree(aname);
         return (DirIntfPtr)di;
      }
      else
         *status=IOINTF_MEM;
   }
   return 0;
}


static int
DiskIo_mkDir(IoIntfPtr super, const char* name, const char** ecode)
{
   DiskIo* o = (DiskIo*)super;
   char* aname;
   int status;
   if(ecode) *ecode=0;
   if( (aname = DiskIo_mkAbsPath(o, name, 0, &status)) != 0)
   {
      status = FS_MkDir(aname) ? IOINTF_NOACCESS : 0;
      baFree(aname);
   }
   return status;
}


static int
DiskIo_rename(
   IoIntfPtr super, const char* rfrom, const char* rto, const char** ecode)
{
   DiskIo* o = (DiskIo*)super;
   char* afrom;
   char* ato;
   int status;

   if(ecode) *ecode=0;

   if( (ato = DiskIo_mkAbsPath(o, rto, 0, &status)) != 0)
   {
      if( (afrom = DiskIo_mkAbsPath(o, rfrom, 0, &status)) != 0)
      {
         status = FS_Rename(afrom, ato) ? IOINTF_NOACCESS : 0;
         baFree(afrom);
      }
      baFree(ato);
   }
   return status;
}


static int
DiskIo_remove(IoIntfPtr super,const char* name, const char** ecode)
{
   DiskIo* o = (DiskIo*)super;
   char* aname;
   int status;
   if(ecode) *ecode=0;
   if( (aname = DiskIo_mkAbsPath(o, name, 0, &status)) != 0)
   {
      status = FS_Remove(aname) ? IOINTF_NOACCESS : 0;
      baFree(aname);
   }
   return status;
}


static int
DiskIo_rmDir(IoIntfPtr super, const char* name, const char** ecode)
{
   DiskIo* o = (DiskIo*)super;
   char* aname;
   int status;

   if(ecode) *ecode=0;

   if( (aname = DiskIo_mkAbsPath(o, name, 0, &status)) != 0)
   {
      status = FS_RmDir(aname) ? IOINTF_NOACCESS : 0;
      baFree(aname);
   }
   return status;
}


static int
DiskIo_stat(IoIntfPtr super, const char* name, IoStat* st)
{
   char* aname;
   int status;
   DiskIo* o = (DiskIo*)super;
   if( (aname = DiskIo_mkAbsPath(o, name, 0, &status)) != 0)
   {
      U32 ts;
      status=FS_GetFileTime(aname, &ts) ? IOINTF_NOTFOUND : 0;
      if(!status)
      {
         U8 attr;
         st->lastModified = dateTime2UnixTime(ts);
         attr=FS_GetFileAttributes(aname);
         if(attr == 0xFF)
            status=IOINTF_IOERROR;
         else
         {
            st->isDir = attr & FS_ATTR_DIRECTORY ? TRUE : FALSE;
            if(st->isDir)
               st->size=0;
            else
            {
               FS_FILE* fh = FS_FOpen(aname, "rb");
               if(fh)
               {
                  st->size=FS_GetFileSize(fh);
                  FS_FClose(fh);
               }
               else
                  status=IOINTF_IOERROR;
            }
         }
      }
      baFree(aname);
   }
   return status;
}


static ResIntfPtr
DiskIo_openRes(IoIntfPtr super, const char* name, U32 mode,
               int* status, const char** ecode)
{
   const char* access;
   char* aname;
   DiskIo* o = (DiskIo*)super; /* Downcast */
   ResIntfPtr retVal=0;
   if(ecode) *ecode=0;

   if(mode == OpenRes_READ)
   {
      access = "rb";
   }
   else if(mode == OpenRes_WRITE)
   {
      access = "wb";
   }
   else
   {
      *status = IOINTF_NOIMPLEMENTATION;
      if(ecode) *ecode = "Unknown mode";
      return 0;
   }

   if( (aname = DiskIo_mkAbsPath(o, name, 0, status)) != 0)
   {
      DiskRes* dr = (DiskRes*)baMalloc(sizeof(DiskRes));
      if(dr)
      {
         FS_FILE* hndl = FS_FOpen(aname, access);
         if(hndl)
         {
            *status=0;
            DiskRes_constructor(dr, hndl);
            retVal = (ResIntfPtr)dr; /* Cast to super class */
         }
         else
         {
            *status = mode == OpenRes_READ ?
               IOINTF_NOTFOUND : IOINTF_NOACCESS;
         }
      }
      else
         *status = IOINTF_MEM;
      baFree(aname);
   }
   return retVal;
}


static int
DiskIo_closeDir(IoIntfPtr super, DirIntfPtr* dirIntf)
{
   if(dirIntf)
   {
      DirIter_destructor((DirIter*)*dirIntf);
      baFree(*dirIntf);
      *dirIntf=0;
      return 0;
   }
   return IOINTF_IOERROR;
}


static int
DiskIo_property(IoIntfPtr super,const char* name,void* a,void* b)
{
   DiskIo* o = (DiskIo*)super;
   if( ! strcmp(name, "movedir") )
   {
      *((U32*)a) = TRUE; /* emFile can move directories */
      return 0;
   }
   if( ! strcmp(name, "hidden") )
   {
      char* aname;
      int status;
      if( (aname = DiskIo_mkAbsPath(o, (const char*)a, 0, &status)) != 0)
      {
         U8 attr=FS_GetFileAttributes(aname);
         if(attr == 0xFF)
            status=IOINTF_NOTFOUND;
         else
         {
            attr |= FS_ATTR_HIDDEN;
            status = FS_SetFileAttributes(aname, attr) ? IOINTF_IOERROR : 0;
         }
         baFree(aname);
      }
      else
         status = IOINTF_MEM;
      return status;
   }
   if( ! strcmp(name, "type") )
   {
      if(a)
      {
         const char** type = (const char**)a;
         *type = "disk";
         if(b)
         {
            const char** platform = (const char**)b;
            *platform="emFile";
         }
         return 0;
      }
   }
   if( ! strcmp(name, "abs") )
   {
      int status;
      char* path = DiskIo_mkAbsPath(o, (const char*)a, 0, &status);
      if(path)
      {
         *((char**)b) = path;
         return 0;
      }
      return status;
   }

   if( ! strcmp(name, "attach") )
   {
      if(a)
      {
         if(super->onTerminate)
            super->onTerminate(super->attachedIo, super);
         super->attachedIo = (IoIntfPtr)a;
         super->onTerminate = *((IoIntf_OnTerminate*)b);
      }
      else
      {
         super->attachedIo = 0;
         super->onTerminate = 0;
      }
      return 0;
   }

   if( ! strcmp(name, "dupsize") )
   {
      *((size_t*)a) = sizeof(DiskIo);
      return 0;
   }
   if( ! strcmp(name, "dup") )
   {
      char* path;
      int status;
      DiskIo* di = (DiskIo*)a;
      const char* ptr = (const char*)b;
      if(!ptr) ptr="";
      path = DiskIo_mkAbsPath(o, ptr, 0, &status);
      DiskIo_constructor(di);
      if(path)
      {
         status=DiskIo_setRootDir(di, path);
         baFree(path);
         if(status == IOINTF_NOTFOUND)
         {
            /* The path could be an absolute path */
            status=DiskIo_setRootDir(di, (const char*)b);
            if(status)
               status = IOINTF_NOTFOUND; /* Set to original error */
         }
      }
      else
         status=E_MALLOC;
      if(status)
         DiskIo_destructor(di);
      return status;
   }

   if( ! strcmp(name, "destructor") )
   {
      DiskIo_destructor(o);
      return 0;
   }
   return -1;
}


void
DiskIo_constructor(DiskIo* o)
{
   IoIntf_constructorRW((IoIntf*)o,
                        DiskIo_property,
                        DiskIo_closeDir,
                        DiskIo_mkDir,
                        DiskIo_rename,
                        DiskIo_openDir,
                        DiskIo_openRes,
                        0, /* openResGzip */
                        DiskIo_remove,
                        DiskIo_rmDir,
                        DiskIo_stat);
   o->rootPath=0;
   o->rootPathLen=1;
}


void
DiskIo_destructor(DiskIo* o)
{
   IoIntfPtr super = (IoIntfPtr)o;
   if(super->onTerminate)
      super->onTerminate(super->attachedIo, super);
   if(o->rootPath)
      baFree(o->rootPath);
}


int
DiskIo_setRootDir(DiskIo* o, const char* root)
{
   int status;
   if( !root || (root[0] == '/' && !root[1]) )
   { /* Absolute root path */
      if(o->rootPath)
         baFree(o->rootPath);
      o->rootPath=0;
      o->rootPathLen=0;
      status = 0;
   }
   else
   {
      status=0;
      if(!root)
         root="";
      else
      {
         U8 attr;
         char* dp=baStrdup(root);
         if(!dp) return IOINTF_MEM;
         dp=unix2DosPath(dp);
         attr=FS_GetFileAttributes(dp);
         if(attr == 0xFF || !(attr & FS_ATTR_DIRECTORY))
            status=IOINTF_NOTFOUND;
         baFree(dp);
      }
      if(!status)
      {
         o->rootPath = baMalloc(strlen(root)+2);
         if(o->rootPath)
         {
            strcpy(o->rootPath, root);
            if(root[strlen(root)-1] != '/')
               strcat(o->rootPath, "/");
            o->rootPathLen = strlen(o->rootPath);
            status=0;
         }
         else
            status=IOINTF_MEM;
      }
      else
         status = IOINTF_NOACCESS;
   }
   return status;
}


int
DiskIo_getRootDir(DiskIo* o, char* buf, int len)
{
   if(len < o->rootPathLen || len < 2)
   {
      return -1;
   }
   if(!o->rootPath)
   {
      strcpy(buf, "");
      return 1;
   }
   strcpy(buf,o->rootPath);
   return strlen(o->rootPath);
}
