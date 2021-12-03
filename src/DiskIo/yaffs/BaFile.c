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
 *   COPYRIGHT:  Real Time Logic, 2006
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
This file contains DiskIo and BaFile implementations for YAFFS.
http://en.wikipedia.org/wiki/YAFFS

This implementation sets the root directory to '/' by default.  The
root directory can be change with DiskIo_setRootDir, which can be
called at any time to change the root directory.

Please see the BaDiskIo.h, ResIntf.h, and BaFile.h documentation in the
reference manual for information on the API's that are implemented in
this file.
*/


#define ROOTPT char*
#include <BaDiskIo.h>
#include <BaFile.h>
#include <BaServerLib.h>


#include <yaffs_guts.h>
#include <yaffsfs.h>




/* Read 'errno' and convert to a IOINTF_XXX error code.
   Parameter status is a pointer to where to store the error type.
   Parameter emsg is a optional pointer for storing the 'strerror' message.
*/
static void
setErrCode(int* status, const char** emsg)
{
   int ecode = errno;
   switch(ecode)
   {
      case ENOSPC:
         *status = IOINTF_NOSPACE;
         break;

      case ENOTDIR:
      case EEXIST:
         *status = IOINTF_EXIST;
         break;

      case ENOTEMPTY:
         *status = IOINTF_NOTEMPTY;
         break;

      case ENOENT:
         *status = IOINTF_ENOENT;
         break;

      case EBADF:
      case EIO:
      case EINVAL:
      case EFBIG:
         *status = IOINTF_IOERROR;
         break;

      case ENAMETOOLONG:
         *status = IOINTF_INVALIDNAME;
         break;

      case EMFILE:
      case ENFILE:
      case ENOMEM:
         *status = IOINTF_MEM;
         break;

      case EACCES:
      case EPERM:
      default:
         *status = IOINTF_NOACCESS;
         break;
   }
   if(emsg) /* optional, may be NULL */
      *emsg=strerror(ecode);
}


/* DirIter maintains state information for DiskIo_openDir,
 * DirIter_read, DirIter_getName, DirIter_stat, and DiskIo_closeDir.
 * A DirIter instance is created in DiskIo_openDir and released in
 * DiskIo_closeDir
 */
typedef struct
{
      DirIntf super; /* Implements the abstract DirIntf class */

      yaffs_DIR* dp;
      struct yaffs_dirent* ep;
      char* dname;
} DirIter;


static int
DirIter_read(DirIntfPtr super)
{
   DirIter* o = (DirIter*)super;
   o->ep = yaffs_readdir(o->dp);
   if(o->ep < 0)
   {
      int status;
      setErrCode(&status, 0);
      return status;
   }
   return 0;
}

static const char*
DirIter_getName(DirIntfPtr super)
{
   DirIter* o = (DirIter*)super;
   return o->ep ? o->ep->d_name : 0;
}

static int
DirIter_stat(DirIntfPtr super, IoStat* st)
{
   DirIter* o = (DirIter*)super;
   int status;
   if(o->ep)
   {
      int len = strlen(o->dname)+strlen(o->ep->d_name)+2;
      char* name = (char*)baMalloc(len);
      if(name)
      {
         struct yaffs_stat statBuf;
         if(o->dname[0] != '/' || o->dname[1])
            basnprintf(name,len,"%s/%s", o->dname, o->ep->d_name);
         else
            basnprintf(name,len,"/%s", o->ep->d_name);
         if(yaffs_stat(name, &statBuf) < 0)
            setErrCode(&status, 0);
         else
         {
            status=0;
            st->isDir = (S_IFDIR & statBuf.st_mode) ? TRUE : FALSE;
            st->lastModified = statBuf.yst_mtime;
            st->size = statBuf.st_size;
         }
         baFree(name);
      }
      else
         status=IOINTF_MEM;
   }
   else
      status=IOINTF_MEM;
   return status;
}


static void
DirIter_constructor(DirIter* o, yaffs_DIR* dp, char* dname)
{
   DirIntf_constructor((DirIntf*)o,
                       DirIter_read,
                       DirIter_getName,
                       DirIter_stat);
   o->dp=dp;
   o->dname=dname;
}


static void
DirIter_destructor(DirIter* o)
{
   yaffs_closedir(o->dp);
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
      int fp;
} DiskRes;


static int
DiskRes_read(ResIntfPtr super, void* buf, size_t maxSize, size_t* size)
{
   int ssize;
   DiskRes* o = (DiskRes*)super; /* upcast */
   ssize=yaffs_read(o->fp, buf, maxSize);
   if(ssize < 0)
   {
      *size=0;
      return IOINTF_IOERROR;
   }
   *size=ssize;
   return 0;
}


static int
DiskRes_write(ResIntfPtr super, const void* buf, size_t size)
{
   DiskRes* o = (DiskRes*)super; /* upcast */
   int status;
   if(yaffs_write(o->fp, buf, size) < 0)
      setErrCode(&status, 0);
   else
      status=0;
   return status;
}


static int
DiskRes_seek(ResIntfPtr super, size_t offset)
{
   DiskRes* o = (DiskRes*)super; /* upcast */
   int status;
   if(yaffs_lseek(o->fp, offset, SEEK_SET) < 0)
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
#ifdef MISSING_FUNCTION_IN_NETOS /* fixme */
   if(yaffs_file_flush(o->fp) < 0)
      setErrCode(&status, 0);
   else
#endif
      status=0;
   return status;
}

static int
DiskRes_close(ResIntfPtr super)
{
   int status;
   int fp;
   baAssert(super);
   if(!super) return -1;
   fp = ((DiskRes*)super)->fp;
   baFree(super);
   if(yaffs_close(fp) < 0)
      setErrCode(&status, 0);
   else
      status=0;
   return status;
}



static void
DiskRes_constructor(DiskRes* o, int fp)
{
   ResIntf_constructor((ResIntf*)o,
                       DiskRes_read,
                       DiskRes_write,
                       DiskRes_seek,
                       DiskRes_flush,
                       DiskRes_close);
   o->fp=fp;
}


/* Make absolute path by adding rootPath + name.
 * Space to hold the returned string is obtained from baMalloc.
 */
static char*
DiskIo_mkAbsPath(DiskIo* o, const char* name, int* status)
{
   int len;
   char* retVal;
   const char* rootPath = o->rootPath ? o->rootPath : "/";
   if(*name == '/') name++;
   len = o->rootPathLen+strlen(name)+1;
   retVal = baMalloc(len);
   if(retVal)
   {
      memcpy(retVal, rootPath, o->rootPathLen);
      strcpy(retVal+o->rootPathLen, name);
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
         yaffs_DIR* dp = yaffs_opendir(aname);
         if(dp)
         {
            DirIter_constructor(di, dp, aname);
            *status=0;
            aname=0;
         }
         else
         {
            setErrCode(status, ecode);
            baFree(di);
            di=0;
         }
      }
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
      if(yaffs_mkdir(aname, S_IRWXU|S_IRGRP|S_IXGRP) < 0)
         setErrCode(&status, ecode);
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
         if((status=yaffs_rename(afrom, ato)) != 0)
            setErrCode(&status, ecode);
         else
            status=0;
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
   int status=-1;
   if(ecode) *ecode=0;
   if( (aname = DiskIo_mkAbsPath(o, rname, &status)) != 0)
   {
      if(yaffs_unlink(aname) < 0)
         setErrCode(&status, ecode);
      else
         status=0;
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
      if(yaffs_rmdir(aname) < 0)
         setErrCode(&status, ecode);
      else
         status=0;
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
      struct yaffs_stat statBuf;
      if((status=yaffs_stat(aname, &statBuf)) != 0)
      {
         setErrCode(&status, 0);
      }
      else
      {
         status=0;
         st->isDir = (S_IFDIR & statBuf.st_mode) ? TRUE : FALSE;
         st->lastModified = statBuf.yst_mtime;
         st->size = statBuf.st_size;
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
         int fp = yaffs_open(
            aname,
            O_CREAT | (mode == OpenRes_WRITE ? O_WRONLY : O_RDONLY),
            mode == OpenRes_WRITE ? S_IWRITE : S_IREAD);
         if(fp < 0)
         {
            baFree(dr);
            setErrCode(status, ecode);
         }
         else
         {
            *status=0;
            DiskRes_constructor(dr, fp);
            retVal = (ResIntfPtr)dr; /* Downcast */
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
            *platform="YAFFS";
         }
         return 0;
      }
   }
   if( ! strcmp(name, "abs") )
   {
      int status;
      char* path = DiskIo_mkAbsPath(o, (const char*)a, &status);
      if(path)
      {
         *((char**)b) = path;
         return 0;
      }
      return status;
   }
   if( ! strcmp(name, "destructor") )
   {
      DiskIo_destructor(o);
      return 0;
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
      o->rootPathLen=1;
      status = 0;
   }
   else
   {
      struct yaffs_stat statBuf;
      if(!*root)
         root=".";
      if(yaffs_stat(root, &statBuf) < 0)
      {
         setErrCode(&status, 0);
      }
      else if(S_IFDIR & statBuf.st_mode)
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
      strcpy(buf, "/");
      return 1;
   }
   strcpy(buf,o->rootPath);
   return strlen(buf);
}



/****************************************************************************
                                   BaFile functions
****************************************************************************/

static DiskIo io = {
   {0,0,0,0,0,0,0,0,0,0},
   0,
   0};

DirIntfPtr
baOpenDir(const char* dirname, int* status, const char** ecode)
{
   return DiskIo_openDir((IoIntfPtr)&io, dirname, status, ecode);
}


int
baCloseDir(DirIntfPtr* dirIntf)
{
   return DiskIo_closeDir((IoIntfPtr)&io, dirIntf);
}


int
baStat(const char* name, IoStat* st)
{
   return DiskIo_stat((IoIntfPtr)&io, name, st);
}


ResIntfPtr
baOpenRes(const char* name,U32 mode,int* status,const char** ecode)
{
   return DiskIo_openRes((IoIntfPtr)&io, name, mode, status, ecode);
}


int
baCloseRes(ResIntfPtr* fp)
{
   int status=DiskRes_close(*fp);
   *fp=0;
   return status;
}


int
baMkDir(const char* name, const char** ecode)
{
   return DiskIo_mkDir((IoIntfPtr)&io, name, ecode);
}


int
baRmdDir(const char* name, const char** ecode)
{
   return DiskIo_rmDir((IoIntfPtr)&io, name, ecode);
}


int
baRemove(const char* name, const char** ecode)
{
   return DiskIo_remove((IoIntfPtr)&io, name, ecode);
}


int
baRename(const char* from, const char* to, const char** ecode)
{
   return DiskIo_rename((IoIntfPtr)&io, from, to, ecode);
}
