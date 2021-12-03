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
 *   COPYRIGHT:  Real Time Logic, 2019
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
This file contains DiskIo and BaFile implementations for FreeRTOS-Plus-FAT.

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
#include <ff_stdio.h>
#include <string.h>


/* Read 'errno' and convert to a IOINTF_XXX error code.
   Parameter status is a pointer to where to store the error type.
   Parameter emsg is a optional pointer for storing the 'strerror' message.
*/
static int
setErrCode(int* status, const char** emsg)
{
   switch(*status == 0 ? stdioGET_ERRNO() : *status)
   {
      case pdFREERTOS_ERRNO_ENXIO:
      case pdFREERTOS_ERRNO_ENOENT:
         *status = IOINTF_ENOENT;
         break;
      case pdFREERTOS_ERRNO_ENOMEM:
      case pdFREERTOS_ERRNO_ENOBUFS:
         *status = IOINTF_MEM;
         break;
      case pdFREERTOS_ERRNO_EACCES:
      case pdFREERTOS_ERRNO_EFAULT:
         *status = IOINTF_NOACCESS;
         break;
      case pdFREERTOS_ERRNO_EEXIST:
      case pdFREERTOS_ERRNO_ENOTDIR:
      case pdFREERTOS_ERRNO_EISDIR:
         *status = IOINTF_EXIST;
         break;
      case pdFREERTOS_ERRNO_ENOSPC:
         *status = IOINTF_NOSPACE;
         break;
      case pdFREERTOS_ERRNO_EROFS:
         *status = IOINTF_EOF;
         break;
      case pdFREERTOS_ERRNO_ENOTEMPTY:
         *status = IOINTF_NOTEMPTY;
         break;
      case pdFREERTOS_ERRNO_ENAMETOOLONG:
         *status = IOINTF_INVALIDNAME;
         break;
      case pdFREERTOS_ERRNO_EADDRINUSE:
         *status = IOINTF_NOACCESS;
         break;
      case pdFREERTOS_ERRNO_ENODEV:
      case pdFREERTOS_ERRNO_ENMFILE:
         *status = IOINTF_NOTFOUND;
         break;
      case pdFREERTOS_ERRNO_EAGAIN:
      case pdFREERTOS_ERRNO_EBADF:
      case pdFREERTOS_ERRNO_EIO:
      case pdFREERTOS_ERRNO_EBUSY:
      case pdFREERTOS_ERRNO_EXDEV:
      case pdFREERTOS_ERRNO_EINVAL:
      case pdFREERTOS_ERRNO_ESPIPE:
      case pdFREERTOS_ERRNO_EUNATCH:
      case pdFREERTOS_ERRNO_EBADE:
      case pdFREERTOS_ERRNO_EFTYPE:
      case pdFREERTOS_ERRNO_EOPNOTSUPP:
      case pdFREERTOS_ERRNO_ENOPROTOOPT:
      default:
         *status = IOINTF_IOERROR;
         break;
   }
   return *status;
}


/* DirIter maintains state information for DiskIo_openDir,
 * DirIter_read, DirIter_getName, DirIter_stat, and DiskIo_closeDir.
 * A DirIter instance is created in DiskIo_openDir and released in
 * DiskIo_closeDir
 */
typedef struct
{
   DirIntf super; /* Implements the abstract DirIntf class */
   FF_FindData_t fdata;
   char* dname;
   int dnameLen;
   int status;
   BaBool isFindFirst;
} DirIter;


static int
DirIter_read(DirIntfPtr super)
{
   DirIter* o = (DirIter*)super;
   const char *n;
   if(o->status)
      return o->status;
   if(o->isFindFirst)
   {
      o->isFindFirst=FALSE;
      n=o->fdata.pcFileName;
      if( ! (n[0]== '.' && (!n[1] || (n[1]=='.' && !n[2]))) )
         return 0;
   }
   for(;;)
   {
      if( ! ff_findnext(&o->fdata) )
      {
         n=o->fdata.pcFileName;
         if(n[0]== '.' && (!n[1] || (n[1]=='.' && !n[2])))
            continue;
         return 0;
      }
      else
         break;
   }
   return setErrCode(&o->status, 0);
}

static const char*
DirIter_getName(DirIntfPtr super)
{
   DirIter* o = (DirIter*)super;
   if(o->status)
      return 0;
   return o->fdata.pcFileName;
}

static int
DirIter_stat(DirIntfPtr super, IoStat* st)
{
   FF_Stat_t statBuf;
   char* name;
   int len;
   DirIter* o = (DirIter*)super;
   if(o->status)
      return 0;
   len = o->dnameLen+strlen(o->fdata.pcFileName)+2;
   name = (char*)baMalloc(len);
   if(name)
   {
      if(o->dname[0] != '/' || o->dname[1])
      {
         if(o->dname[o->dnameLen-1] == '/')
            basnprintf(name,len,"%s%s", o->dname, o->fdata.pcFileName);
         else
            basnprintf(name,len,"%s/%s", o->dname, o->fdata.pcFileName);
      }
      else
         basnprintf(name,len,"/%s", o->fdata.pcFileName);
      if(ff_stat(name, &statBuf))
         setErrCode(&o->status, 0);
      else
      {
         st->isDir = (FF_IFDIR & statBuf.st_mode) ? TRUE : FALSE;
         st->lastModified = statBuf.st_mtime;
         st->size = statBuf.st_size;
      }
      baFree(name);
   }
   else
      o->status=IOINTF_MEM;
   return o->status;
}


static void
DirIter_constructor(DirIter* o, char* dname)
{
   DirIntf_constructor((DirIntf*)o,
                       DirIter_read,
                       DirIter_getName,
                       DirIter_stat);
   o->dname=dname;
   o->dnameLen=strlen(dname);
   o->isFindFirst=TRUE;
   o->status = 0;
}


static void
DirIter_destructor(DirIter* o)
{
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
      FF_FILE* fp;
} DiskRes;


static int
DiskRes_read(ResIntfPtr super, void* buf, size_t maxSize, size_t* size)
{
   DiskRes* o = (DiskRes*)super; /* upcast */
   int status=0;
   *size=ff_fread(buf, 1, maxSize, o->fp);
   if(*size != maxSize)
   {
      if(ff_feof(o->fp))
      {
         return *size == 0 ? IOINTF_EOF : 0;
      }
      setErrCode(&status, 0);
   }
   return status;
}


static int
DiskRes_write(ResIntfPtr super, const void* buf, size_t size)
{
   DiskRes* o = (DiskRes*)super; /* upcast */
   int status=0;
   if(ff_fwrite(buf, size, 1, o->fp) != 1)
      setErrCode(&status, 0);
   return status;
}


static int
DiskRes_seek(ResIntfPtr super, BaFileSize offset)
{
   DiskRes* o = (DiskRes*)super; /* upcast */
   int status=0;
   if(ff_fseek(o->fp, offset, SEEK_SET))
      setErrCode(&status, 0);
   return status;
}


static int
DiskRes_flush(ResIntfPtr super)
{
   DiskRes* o = (DiskRes*)super; /* upcast */
   int status=0; /* fixme no ff_flush? */
   return status;
}

static int
DiskRes_close(ResIntfPtr super)
{
   FF_FILE* fp;
   baAssert(super);
   if(!super) return -1;
   fp = ((DiskRes*)super)->fp;
   baFree(super);
   if(ff_fclose(fp))
   {
      int status=0;
      setErrCode(&status, 0);
      return status;
   }
   return 0;
}



static void
DiskRes_constructor(DiskRes* o, FF_FILE* fp)
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

   const char* rootPath;
   if(o->rootPath)
   {
      rootPath = (char*)o->rootPath;
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
   *status=0;
   aname = DiskIo_mkAbsPath(o, rname, status);
   if(aname)
   {
      DirIter* di = (DirIter*)baMalloc(sizeof(DirIter));
      if(di)
      {
         memset(&di->fdata, 0, sizeof(FF_FindData_t));
         if( ! (*status=ff_findfirst(aname, &di->fdata)) )
         {
            DirIter_constructor(di, aname);
            aname=0;
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
   else
      *status = E_MALLOC;
   return 0;
}


static int
DiskIo_mkDir(IoIntfPtr super, const char* rname, const char** ecode)
{
   DiskIo* o = (DiskIo*)super;
   char* aname;
   int status=0;
   if(ecode) *ecode=0;
   if( (aname = DiskIo_mkAbsPath(o, rname, &status)) != 0)
   {
      if(ff_mkdir(aname))
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
   int status=0;
   if(ecode) *ecode=0;
   if( (ato = DiskIo_mkAbsPath(o, rto, &status)) != 0)
   {
      if( (afrom = DiskIo_mkAbsPath(o, rfrom, &status)) != 0)
      {
         if(ff_rename(afrom, ato, TRUE))
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
   int status=0;
   if(ecode) *ecode=0;
   if( (aname = DiskIo_mkAbsPath(o, rname, &status)) != 0)
   {
      if(ff_remove(aname))
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
   int status=0;
   if(ecode) *ecode=0;
   if( (aname = DiskIo_mkAbsPath(o, rname, &status)) != 0)
   {
      if(ff_rmdir(aname))
         setErrCode(&status, ecode);
      baFree(aname);
   }
   return status;
}


static int
DiskIo_stat(IoIntfPtr super, const char* rname, IoStat* st)
{
   DiskIo* o = (DiskIo*)super;
   char* aname;
   int status=0;
   if( (aname = DiskIo_mkAbsPath(o, rname, &status)) != 0)
   {
      FF_Stat_t statBuf;
      if(ff_stat(aname, &statBuf))
      {
         setErrCode(&status, 0);
      }
      else
      {
         st->isDir = (FF_IFDIR & statBuf.st_mode) ? TRUE : FALSE;
         st->lastModified = statBuf.st_mtime;
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
   *status=0;
   if( (aname = DiskIo_mkAbsPath(o, rname, status)) != 0)
   {
      DiskRes* dr = (DiskRes*)baMalloc(sizeof(DiskRes));
      if(dr)
      {
         const char* fmode;
         FF_FILE* fp;
         if(mode & OpenRes_WRITE)
         {
            if(mode & OpenRes_READ)
            {
               fmode = mode & OpenRes_APPEND ? "a+" : "r+";
            }
            else
            {
               fmode = mode & OpenRes_APPEND ? "a" : "w";
            }
         }
         else
         {
            fmode="r";
         }
         fp = ff_fopen(aname, fmode);
         if(fp)
         {
            DiskRes_constructor(dr, fp);
            retVal = (ResIntfPtr)dr;
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
      *((U32*)a) = FALSE; /* No, FreeRTOS+FAT cannot move directories */
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
            *platform="FreeRTOS";
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
   int status=0;
   if( !root || !*root || (root[0] == '/' && !root[1]) )
   { /* Absolute root path */
      if(o->rootPath)
         baFree(o->rootPath);
      o->rootPath=0;
      o->rootPathLen=1;
   }
   else
   {
      FF_Stat_t statBuf;
      if(ff_stat(root, &statBuf))
      {
         setErrCode(&status, 0);
      }
      else if(FF_IFDIR & statBuf.st_mode)
      {
         if(o->rootPath)
            baFree(o->rootPath);
         o->rootPath = baMalloc(strlen(root)+2);
         if(o->rootPath)
         {
            strcpy(o->rootPath, root);
            o->rootPathLen = strlen(root);
            if(o->rootPath[o->rootPathLen-1] != '/')
            {
               o->rootPath[o->rootPathLen++]='/';
               o->rootPath[o->rootPathLen]=0;
            }
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
