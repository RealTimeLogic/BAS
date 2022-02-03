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
 *   $Id$
 *
 *   COPYRIGHT:  Real Time Logic, 2016 - 2022
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

FatFs port: http://elm-chan.org/

DiskIo_setRootDir must be called by using the following syntax:
   "drive:/[directory]"
   http://elm-chan.org/fsw/ff/en/filename.html
*/



#include <ff.h>

#define ROOTPT char*
#include <BaDiskIo.h>
#include <BaFile.h>
#include <BaServerLib.h>

/* Read 'errno' and convert to a IOINTF_XXX error code.
   Parameter status is a pointer to where to store the error type.
   Parameter emsg is a optional pointer for storing the 'strerror' message.
*/
static int
setErrCode(int* status, const char** emsg)
{
   switch(*status)
   {
      case FR_NO_FILE:
         *status=IOINTF_ENOENT;
         break;
      case FR_NO_PATH:
         *status=IOINTF_NOTFOUND;
         break;
      case FR_INVALID_NAME:
         *status=IOINTF_INVALIDNAME;
         break;
      case FR_DENIED:
         *status=IOINTF_NOACCESS;
         break;
      case FR_EXIST:
         *status=IOINTF_EXIST;
         break;
      case FR_WRITE_PROTECTED:
         *status=IOINTF_NOACCESS;
         break;
      case FR_INVALID_DRIVE:
         *status=IOINTF_ENOENT;
         break;
      case FR_LOCKED:
         *status=IOINTF_LOCKED;
         break;
      case FR_NOT_ENOUGH_CORE:
         *status=IOINTF_MEM;
         break;
      case FR_TOO_MANY_OPEN_FILES:
         *status=IOINTF_NOACCESS;
         break;

      case FR_INVALID_PARAMETER:
      case FR_TIMEOUT:
      case FR_MKFS_ABORTED:
      case FR_NO_FILESYSTEM:
      case FR_NOT_ENABLED:
      case FR_NOT_READY:
      case FR_INVALID_OBJECT:
      case FR_INT_ERR:
      case FR_DISK_ERR:
      default:
         *status=IOINTF_NOACCESS;
         break;
   }
   if(emsg)
      *emsg=0;
   return *status;
}


static void
remEndingSlash(char* name)
{
   int len=strlen(name)-1;
   if(name[len]=='/')
      name[len]=0; /* remove ending / */
}

/*
  Returns: https://en.wikipedia.org/wiki/Unix_time
*/
static BaTime
finfo2UnixTime(FILINFO* fi)
{
   BaTime t = /* days since 1970 */
      ((fi->fdate >> 9) + 1980 - 1970)*365 + 
      (fi->fdate >> 5 & 15)*31+
      (fi->fdate & 31);
   t *= 24*60*60; /* Convert to seconds */
   return t + 
      (fi->ftime >> 11)*60*60 + 
      (fi->ftime >> 5 & 63)*60 +
      (fi->ftime & 31) * 2;
}


/* DirIter maintains state information for DiskIo_openDir,
 * DirIter_read, DirIter_getName, DirIter_stat, and DiskIo_closeDir.
 * A DirIter instance is created in DiskIo_openDir and released in
 * DiskIo_closeDir
 */
typedef struct
{
   DirIntf super; /* Implements the abstract DirIntf class */
   FILINFO fno;
#ifdef ESP_PLATFORM
   FF_DIR dir;
#else
   DIR dir;
#endif
   char* dname;
   int dnameLen;
   int status;
#if _USE_LFN
   char lfn[_MAX_LFN];
#endif
} DirIter;


static int
DirIter_read(DirIntfPtr super)
{
   DirIter* o = (DirIter*)super;
   if(o->status)
      return o->status;
   for(;;)
   {
      if((o->status = f_readdir(&o->dir,&o->fno)) != FR_OK)
      {
         return setErrCode(&o->status, 0);
      }
      if( ! *o->fno.fname ) /* End of directory */
      {
         o->status = IOINTF_NOTFOUND;
         break;
      }
      /* Do not include dot entry */
      if( ! (o->fno.fname[0] == '.' && !o->fno.fname[0]) )
         break;
   }
   return o->status;
}


static const char*
DirIter_getName(DirIntfPtr super)
{
   DirIter* o = (DirIter*)super;
#if _USE_LFN
      char* fn = *o->fno.lfname ? o->fno.lfname : o->fno.fname;
#else
      char* fn = o->fno.fname;
#endif
   return o->status ? 0 : fn;
}

static int
DirIter_stat(DirIntfPtr super, IoStat* st)
{
   DirIter* o = (DirIter*)super;
   if( ! o->status )
   {
      int len;
      char* name;
#if _USE_LFN
      char* fn = *o->fno.lfname ? o->fno.lfname : o->fno.fname;
#else
      char* fn = o->fno.fname;
#endif
      len = strlen(o->dname)+strlen(fn)+2;
      name = (char*)baMalloc(len);
      if(name)
      {
         FILINFO finfo;
         if(o->dname[0] != '/' || o->dname[1])
         {
            if(o->dname[o->dnameLen-1] == '/')
               basnprintf(name,len,"%s%s", o->dname, fn);
            else
               basnprintf(name,len,"%s/%s", o->dname, fn);
         }
         else
            basnprintf(name,len,"/%s", fn);
         remEndingSlash(name);
         if( (o->status=f_stat(name, &finfo)) != FR_OK)
            setErrCode(&o->status, 0);
         else
         {
            st->isDir = (AM_DIR & finfo.fattrib) ? TRUE : FALSE;
            st->lastModified = finfo2UnixTime(&finfo);
            st->size = finfo.fsize;
         }
         baFree(name);
      }
      else
         o->status=IOINTF_MEM;
   }
   return o->status;
}


static void
DirIter_destructor(DirIter* o)
{
   f_closedir(&o->dir);
   baFree(o->dname);
}


/* DiskRes maintains state information for DiskIo_openRes,
 * DiskRes_read, DiskRes_write, DiskRes_seek, DiskRes_flush, and
 * DiskIo_closeRes.
 * A DiskRes instance is created in DiskIo_openRes and released in
 * DiskIo_closeRes.
 */
typedef struct
{
      ResIntf super; /* Implements the abstract ResIntf class */
      FIL fp;
} DiskRes;


static int
DiskRes_read(ResIntfPtr super, void* buf, size_t maxSize, size_t* size)
{
   DiskRes* o = (DiskRes*)super; /* upcast */
   int status;
   UINT br;
   if( (status=f_read(&o->fp, buf, maxSize, &br)) != FR_OK)
      return setErrCode(&status, 0);
   *size = br;
   if(br == 0)
      return IOINTF_EOF;
   return status;
}


static int
DiskRes_write(ResIntfPtr super, const void* buf, size_t size)
{
   DiskRes* o = (DiskRes*)super; /* upcast */
   int status;
   UINT bw;
   if( (status=f_write(&o->fp, buf, size, &bw)) != FR_OK)
      return setErrCode(&status, 0);
   if(size == bw)
      return 0;
   status = IOINTF_IOERROR;
   return setErrCode(&status, 0);
}


static int
DiskRes_seek(ResIntfPtr super, BaFileSize offset)
{
   DiskRes* o = (DiskRes*)super; /* upcast */
   int status;
   if( (status=f_lseek(&o->fp, offset)) != FR_OK)
      setErrCode(&status, 0);
   else
      status=0;
   return status;
}


static int
DiskRes_flush(ResIntfPtr super)
{
   DiskRes* o = (DiskRes*)super; /* upcast */
   int status;
   if( (status=f_sync(&o->fp)) != FR_OK)
      setErrCode(&status, 0);
   else
      status=0;
   return status;
}

static int
DiskRes_close(ResIntfPtr super)
{
   int status;
   baAssert(super);
   if(!super) return -1;
   if( (status=f_close(&((DiskRes*)super)->fp)) != FR_OK)
      setErrCode(&status, 0);
   baFree(super);
   return status;
}



static void
DiskRes_constructor(DiskRes* o)
{
   ResIntf_constructor((ResIntf*)o,
                       DiskRes_read,
                       DiskRes_write,
                       DiskRes_seek,
                       DiskRes_flush,
                       DiskRes_close);
}


/* Make absolute path by adding rootPath + name.
 * Space to hold the returned string is obtained from baMalloc.
 */
static char*
DiskIo_mkAbsPath(DiskIo* o, const char* name, int* status)
{
   int len;
   char* retVal;

   const char* rootPath;
   if(o->rootPath)
   {
      rootPath = o->rootPath;
   }
   else
   {
      rootPath = "/";
      o->rootPathLen=1;
   }
   if(*name == '/') name++;
   len = o->rootPathLen+strlen(name)+1;
   retVal = baMalloc(len);
   if(retVal)
   {
      memcpy(retVal, rootPath, o->rootPathLen);
      strcpy(retVal+o->rootPathLen, name);
      *status=0;
   }
   else
      *status=E_MALLOC;
   return retVal;
}


static DirIntfPtr
DiskIo_openDir(IoIntfPtr super, const char* rname,
               int* status, const char** ecode)
{
   DiskIo* o = (DiskIo*)super;
   char* aname;
   if(ecode) *ecode=0;

   aname = DiskIo_mkAbsPath(o, rname, status);
   if(aname)
   {
      DirIter* di = (DirIter*)baMalloc(sizeof(DirIter));
      if(di)
      {
#if _USE_LFN
         di->fno.lfname = di->lfn;
         di->fno.lfsize = sizeof(di->lfn);
#endif
         remEndingSlash(aname);
         if( (*status=f_opendir(&di->dir, aname)) == FR_OK)
         {
            DirIntf_constructor((DirIntf*)di,
                                DirIter_read,
                                DirIter_getName,
                                DirIter_stat);
            di->dname=aname;
            di->dnameLen=strlen(aname);
            di->status=0;
            aname=0;
            *status=0;
         }
         else
         {
            setErrCode(status, ecode);
            baFree(di);
            di=0;
         }
      }
      else
         *status = E_MALLOC;
      if(aname)
         baFree(aname);
      return (DirIntfPtr)di;
   }
   return 0;
}


static int
DiskIo_mkDir(IoIntfPtr super, const char* rname, const char** ecode)
{
   DiskIo* o = (DiskIo*)super;
   char* aname;
   int status;

   if(ecode) *ecode=0;

   if( (aname = DiskIo_mkAbsPath(o, rname, &status)) != 0)
   {
      if( (status=f_mkdir(aname)) != FR_OK)
         return setErrCode(&status, ecode);
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

   if( (ato = DiskIo_mkAbsPath(o, rto, &status)) != 0)
   {
      if( (afrom = DiskIo_mkAbsPath(o, rfrom, &status)) != 0)
      {
         if( (status=f_rename(afrom, ato)) != FR_OK)
            setErrCode(&status, ecode);
         baFree(afrom);
      }
      baFree(ato);
   }
   return status;
}


static int
DiskIo_remove(IoIntfPtr super, const char* rname, const char** ecode)
{
   DiskIo* o = (DiskIo*)super;
   char* aname;
   int status;

   if(ecode) *ecode=0;

   if( (aname = DiskIo_mkAbsPath(o, rname, &status)) != 0)
   {
      if( (status=f_unlink(aname)) != FR_OK)
         setErrCode(&status, ecode);
      baFree(aname);
   }
   return status;
}


static int
DiskIo_rmDir(IoIntfPtr super, const char* rname, const char** ecode)
{
   DiskIo* o = (DiskIo*)super;
   char* aname;
   int status;

   if(ecode) *ecode=0;

   if( (aname = DiskIo_mkAbsPath(o, rname, &status)) != 0)
   {
      if( (status=f_unlink(aname)) != FR_OK)
      {
         if(status == FR_DENIED)
            status=IOINTF_NOTEMPTY;
         else
            setErrCode(&status, ecode);
      }
      baFree(aname);
   }
   return status;
}


static int
DiskIo_stat(IoIntfPtr super, const char* rname, IoStat* st)
{
   DiskIo* o = (DiskIo*)super;
   char* aname;
   int status;
   if( (aname = DiskIo_mkAbsPath(o, rname, &status)) != 0)
   {
      FILINFO finfo;
      remEndingSlash(aname);
      if( (status=f_stat(aname, &finfo)) != FR_OK)
      {
         if(strncmp(o->rootPath,aname,strlen(aname)))
            setErrCode(&status, 0);
         else
         {
            status=0;
            st->isDir = TRUE;
            st->lastModified = 0;
            st->size = 0;
         }
      }
      else
      {
         st->isDir = (AM_DIR & finfo.fattrib) ? TRUE : FALSE;
         st->lastModified = finfo2UnixTime(&finfo);
         st->size = finfo.fsize;
      }
      baFree(aname);
   }
   return status;
}


static ResIntfPtr
DiskIo_openRes(IoIntfPtr super, const char* rname, U32 mode,
               int* status, const char** ecode)
{
   DiskIo* o = (DiskIo*)super; /* Upcast */
   char* aname;
   ResIntfPtr retVal=0;
   if(ecode) *ecode=0;
   if( (aname = DiskIo_mkAbsPath(o, rname, status)) != 0)
   {
      DiskRes* dr = (DiskRes*)baMalloc(sizeof(DiskRes));
      if(dr)
      {
         *status = f_open(&dr->fp, aname,
                          mode == OpenRes_WRITE ? 
                          (FA_WRITE|FA_CREATE_ALWAYS) : FA_READ);
         if(*status == FR_OK)
         {
            DiskRes_constructor(dr);
            retVal = (ResIntfPtr)dr; /* Downcast */
         }
         else
         {
            baFree(dr);
            setErrCode(status, ecode);
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
   int status;
   DiskIo* o = (DiskIo*)super;
   if( ! strcmp(name, "movedir") )
   {
      *((U32*)a) = TRUE; /* Standard Posix file system can move directories */
      return 0;
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
            *platform="FatFs";
         }
         return 0;
      }
   }
   if( ! strcmp(name, "abs") )
   {
      char* path = DiskIo_mkAbsPath(o, (const char*)a, &status);
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
      DiskIo* di = (DiskIo*)a;
      const char* ptr = (const char*)b;
      if(!ptr) ptr="";
      path = DiskIo_mkAbsPath(o, ptr, &status);
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

   return IOINTF_NOIMPLEMENTATION;
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
   o->rootPathLen=0;
}


void
DiskIo_destructor(DiskIo* o)
{
   IoIntfPtr super = (IoIntfPtr)o;
   if(super->onTerminate)
      super->onTerminate(super->attachedIo, super);
   if(o->rootPath)
   {
      baFree(o->rootPath);
      o->rootPath=0;
      o->rootPathLen=0;
   }
}


int
DiskIo_setRootDir(DiskIo* o, const char* root)
{
   if( !root )
      return IOINTF_NOTFOUND;
   o->rootPath = baMalloc(strlen(root)+2);
   if(o->rootPath)
   {
      char* ptr;
      strcpy(o->rootPath, root);
      ptr = strrchr(o->rootPath, '/');
      if(!ptr || ptr[1])
         strcat(o->rootPath, "/");
      o->rootPathLen = strlen(o->rootPath);
      return 0;
   }
   return E_MALLOC;
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
      strcpy(buf, "/");
      return 1;
   }
   strcpy(buf,o->rootPath);
   return strlen(buf);
}
