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
 *   $Id: BaFile.c 4919 2021-12-02 21:34:55Z wini $
 *
 *   COPYRIGHT:  Real Time Logic, 2006 - 2021
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
This file contains DiskIo and BaFile implementations for POSIX.

The POSIX implementation is compatible with POSIX compliant operating
systems such as UNIX, Linux, QNX, etc.

This implementation sets the root directory to '/' by default.  The
root directory can be change with DiskIo_setRootDir, which can be
called at any time to change the root directory.

Please see the BaDiskIo.h, ResIntf.h, and BaFile.h documentation in the
reference manual for information on the API's that are implemented in
this file.
*/

#if !defined(BA_VXWORKS) || _WRS_VXWORKS_MAJOR != 5
#if !defined(__GLIBC__) || (__GLIBC__ < 2) || ((__GLIBC__ == 2) && (__GLIBC_MINOR__ < 23))

/* Disabled for now, see:
   https://womble.decadent.org.uk/readdir_r-advisory.html
#define USE_READDIR_R
*/


#endif
#endif

#ifdef BA_FILESIZE64
/* Enable 64 bit support */
#ifndef _LARGEFILE64_SOURCE
#define _LARGEFILE64_SOURCE
#endif
/* Convert standard functions to 64 bit version */
#define _FILE_OFFSET_BITS 64
#endif

#include <BaDiskIo.h>
#include <BaFile.h>
#include <BaServerLib.h>


#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>

#ifdef __INTIME__
/* Use 64 bit version */
#undef stat
#define stat _stat64
#endif

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


#if defined(ESP_PLATFORM) || defined(__INTIME__)
static void
remEndingSlash(char* name)
{
   int len=strlen(name)-1;
#ifdef __INTIME__
   if(name[len]=='\\') name[len]=0; /* remove ending \ */
#else
   if(name[len]=='/') name[len]=0; /* remove ending / */
#endif
}
#else
#define remEndingSlash(name) /* Not needed */
#endif



/* DirIter maintains state information for DiskIo_openDir,
 * DirIter_read, DirIter_getName, DirIter_stat, and DiskIo_closeDir.
 * A DirIter instance is created in DiskIo_openDir and released in
 * DiskIo_closeDir
 */

#ifdef __INTIME__

typedef struct
{
   DirIntf super; /* Implements the abstract DirIntf class */
   struct _find64x64 xfind;
   BaBool eodir;
   BaBool isFirstTime;
} DirIter;


static int
DirIter_read(DirIntfPtr super)
{
   DirIter* o = (DirIter*)super;
   const char* n =  o->xfind.f_filename;
   if(o->isFirstTime)
   {
      o->isFirstTime = FALSE;
      if( ! (n[0]== '.' && (!n[1] || (n[1]=='.' && !n[2]))) )
         return 0;
   }
   if(o->eodir) return IOINTF_NOTFOUND;
   for(;;)
   {
      if(_findnext64x64(&o->xfind))
      {
         int status;
         setErrCode(&status, 0);
         o->eodir=TRUE;
         return status;
      }
      else
      {
         n =  o->xfind.f_filename;
         if( ! (n[0]== '.' && (!n[1] || (n[1]=='.' && !n[2]))) )
            return 0;
      }
   }
}


static const char*
DirIter_getName(DirIntfPtr super)
{
   DirIter* o = (DirIter*)super;
   if(o->eodir) return 0;
   return o->xfind.f_filename;
}



static int
DirIter_stat(DirIntfPtr super, IoStat* st)
{
   DirIter* o = (DirIter*)super;
   if(o->eodir) return IOINTF_NOTFOUND;
   st->lastModified = o->xfind.f_atime;
   st->isDir = S_ISDIR(o->xfind.f_mode) ? TRUE : FALSE;
   st->size = o->xfind.f_size;
   return 0;
}

 

static void
DirIter_constructor(DirIter* o)
{
   DirIntf_constructor((DirIntf*)o,
                       DirIter_read,
                       DirIter_getName,
                       DirIter_stat);
   o->eodir=FALSE;
   o->isFirstTime = TRUE;
}


static void
DirIter_destructor(DirIter* o)
{
   if( ! o->eodir )
   {
      _findclose64x64(&o->xfind);
      o->eodir=TRUE;
   }
}

#else /* not __INTIME__ */

typedef struct
{
   DirIntf super; /* Implements the abstract DirIntf class */

   DIR* dp;
   struct dirent* ep; /* Current entry: readdir_r() */
   char* fname; /* current file name, ref to: ep->d_name */
   char* dname; /* directory name */
   int dnameLen;
#ifdef USE_READDIR_R
   char direntbuf[2048];
#endif
} DirIter;


static int
DirIter_read(DirIntfPtr super)
{
   DirIter* o = (DirIter*)super;
   int status;
#ifdef USE_READDIR_R
   if( ! readdir_r(o->dp,(struct dirent *)o->direntbuf,&o->ep) && o->ep)
#else
   if((o->ep = readdir(o->dp)) != 0)
#endif
   {
#ifdef BA_VXWORKS
      o->fname=strrchr(o->ep->d_name, '/');
      o->fname = o->fname ? o->fname+1 : o->ep->d_name;
#else
      o->fname = o->ep->d_name;
#endif
      return 0; /* OK */
   }
   o->fname=0;
   setErrCode(&status, 0);
   return status;
}

static const char*
DirIter_getName(DirIntfPtr super)
{
   return ((DirIter*)super)->fname;
}

static int
DirIter_stat(DirIntfPtr super, IoStat* st)
{
   DirIter* o = (DirIter*)super;
   int status;
   if(o->fname)
   {
      int len = (int)(strlen(o->dname)+strlen(o->fname)+2);
      char* name = (char*)baMalloc(len);
      if(name)
      {
         struct stat statBuf;
         if(o->dname[0] != '/' || o->dname[1])
         {
            if(o->dname[o->dnameLen-1] == '/')
               basnprintf(name,len,"%s%s", o->dname, o->fname);
            else
               basnprintf(name,len,"%s/%s", o->dname, o->fname);
         }
         else
            basnprintf(name,len,"/%s", o->fname);
         remEndingSlash(name);
         if(stat(name, &statBuf))
            setErrCode(&status, 0);
         else
         {
            status=0;
            st->isDir = (S_IFDIR & statBuf.st_mode) ? TRUE : FALSE;
            st->lastModified = statBuf.st_mtime;
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
DirIter_constructor(DirIter* o, DIR* dp, char* dname)
{
   DirIntf_constructor((DirIntf*)o,
                       DirIter_read,
                       DirIter_getName,
                       DirIter_stat);
   o->dp=dp;
   o->dname=dname;
   o->dnameLen=(int)strlen(dname);
   o->fname=0;
}


static void
DirIter_destructor(DirIter* o)
{
   closedir(o->dp);
   baFree(o->dname);
}

#endif /* not __INTIME__ */




/* DiskRes maintains state information for DiskIo_openRes,
 * DiskRes_read, DiskRes_write, DiskRes_seek, DiskRes_flush, and
 * DiskIo_closeRes.
 * A DiskRes instance is created in DiskIo_openRes and released in
 * DiskIo_closeRes.
 */
typedef struct
{
      ResIntf super; /* Implements the abstract ResIntf class */
      FILE* fp;
} DiskRes;


static int
DiskRes_read(ResIntfPtr super, void* buf, size_t maxSize, size_t* size)
{
   DiskRes* o = (DiskRes*)super; /* upcast */
   int status;
   *size=fread(buf, 1, maxSize, o->fp);
   if(*size != maxSize)
   {
      if(feof(o->fp))
      {
         return *size == 0 ? IOINTF_EOF : 0;
      }
      setErrCode(&status, 0);
   }
   else
      status = *size == 0 ? IOINTF_IOERROR : 0;
   return status;
}


static int
DiskRes_write(ResIntfPtr super, const void* buf, size_t size)
{
   DiskRes* o = (DiskRes*)super; /* upcast */
   int status;
   if(fwrite(buf, size, 1, o->fp) == 1)
      status=0;
   else
      setErrCode(&status, 0);
   return status;
}


static int
DiskRes_seek(ResIntfPtr super, BaFileSize offset)
{
   DiskRes* o = (DiskRes*)super; /* upcast */
   int status;
   if(fseek(o->fp, offset, SEEK_SET))
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
   if(fflush(o->fp))
      setErrCode(&status, 0);
   else
      status=0;
   return status;
}

static int
DiskRes_close(ResIntfPtr super)
{
   FILE* fp;
   baAssert(super);
   if(!super) return -1;
   fp = ((DiskRes*)super)->fp;
   baFree(super);
   if(fclose(fp))
   {
      int status;
      setErrCode(&status, 0);
      return status;
   }
   return 0;
}



static void
DiskRes_constructor(DiskRes* o, FILE* fp)
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
#ifdef BA_VXWORKS
      rootPath = "";
      o->rootPathLen=0;
#else
      rootPath = "/";
      o->rootPathLen=1;
#endif
   }
#ifdef BA_VXWORKS
   if(*rootPath) /* if not "" */
#endif
   {
      if(*name == '/')
         name++;
   }
   len = o->rootPathLen+(int)strlen(name)+1;
#ifdef __INTIME__
   len+=4; /* for additional: '\*.*' */
#endif
   retVal = baMalloc(len);
   if(retVal)
   {
      memcpy(retVal, rootPath, o->rootPathLen);
      strcpy(retVal+o->rootPathLen, name);
      *status=0;
#ifdef __INTIME__
      {
         char* ptr = retVal;
         while(*ptr)
         {
            if(*ptr == '/') *ptr='\\';
            ptr++;
         }
      }
#endif
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
#ifdef __INTIME__
         DirIter_constructor(di);
         remEndingSlash(aname);
         strcat(aname,"\\*.*");
         if(_findfirst64x64(aname, &di->xfind))
         {
            setErrCode(status, ecode);
            baFree(di);
            di=0;
         }
         else
         {
            *status=0;
         }
#else
         DIR* dp;
         remEndingSlash(aname);
         dp = opendir(aname);
         if(dp)
         {
            DirIter_constructor(di, dp, aname);
            aname=0;
            *status=0;
         }
         else
         {
            setErrCode(status, ecode);
            baFree(di);
            di=0;
         }
#endif
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
#if defined(BA_VXWORKS) && (!defined(_WRS_VXWORKS_MAJOR) || _WRS_VXWORKS_MAJOR < 7)
      if(mkdir(aname))
#else
      if(mkdir(aname, S_IRWXU|S_IRGRP|S_IXGRP))
#endif
      {
         setErrCode(&status, ecode);
      }
      else
         status=0;
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
         if( ! rename(afrom, ato) )
            status=0;
         else
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
      if(remove(aname))
      {
         setErrCode(&status, ecode);
      }
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
      if(rmdir(aname))
      {
         setErrCode(&status, ecode);
      }
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
      struct stat statBuf;
      remEndingSlash(aname);
      if(stat(aname, &statBuf))
      {
         setErrCode(&status, 0);
      }
      else
      {
         status=0;
         st->isDir = (S_IFDIR & statBuf.st_mode) ? TRUE : FALSE;
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

   if( (aname = DiskIo_mkAbsPath(o, rname, status)) != 0)
   {
      DiskRes* dr = (DiskRes*)baMalloc(sizeof(DiskRes));
      if(dr)
      {
         const char* fmode;
         FILE* fp;
         if(mode & OpenRes_WRITE)
         {
            if(mode & OpenRes_READ)
            {
               fmode = mode & OpenRes_APPEND ? "ab+" : "wb+";
            }
            else
            {
               fmode = mode & OpenRes_APPEND ? "ab" : "wb";
            }
         }
         else
         {
            fmode="rb";
         }
         fp = fopen(aname, fmode);
         if(fp)
         {
#if !defined(BA_VXWORKS) && !defined(ESP_PLATFORM) && !defined(__INTIME__)
            fcntl(fileno(fp), F_SETFD, FD_CLOEXEC);
#endif			
            DiskRes_constructor(dr, fp);
            retVal = (ResIntfPtr)dr; /* Downcast */
            *status=0;
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
            *platform="POSIX";
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


#if defined(ESP_PLATFORM) || defined(BA_VXWORKS)
int DiskIo_setRootDir(DiskIo* o, const char* root)
{
   if( !root )
      return IOINTF_NOTFOUND;
   o->rootPathLen=0;
   if(o->rootPath) baFree(o->rootPath);
   o->rootPath = baMalloc(strlen(root)+2);
   if(o->rootPath)
   {
      strcpy(o->rootPath, root);
      if(strlen(root) > 0)
      {
         char* ptr = strrchr(o->rootPath, '/');
         if(!ptr || ptr[1])
            strcat(o->rootPath, "/");
      }
      o->rootPathLen = (int)strlen(o->rootPath);
      return 0;
   }
   return E_MALLOC;
}
#else
int DiskIo_setRootDir(DiskIo* o, const char* root)
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
      struct stat statBuf;
      char* cwd = 0;
      if(!*root)
         root=".";
      if(stat((char *)root, &statBuf))
      {
         setErrCode(&status, 0);
      }
      else if(S_IFDIR & statBuf.st_mode)
      {
         if(o->rootPath)
            baFree(o->rootPath);

         if(*root == '.')
         {
            cwd = (char*)baMalloc(1024);
            if(cwd)
            {
               if( ! getcwd(cwd, 1024) )
               {
                  baFree(cwd);
                  cwd=0;
               }
            }
         }
         o->rootPath = baMalloc(strlen(root)+2+(cwd ? strlen(cwd)+1:0));
         if(o->rootPath)
         {
            char* ptr;
            if(cwd)
            {
               strcpy(o->rootPath, cwd);
               strcat(o->rootPath, "/");
               if(root[1] == '/') /* ./??? */
                  root+=2;
               else /* . */
                  root++;
               ptr=((char*)o->rootPath)+strlen(o->rootPath);
            }
            else
               ptr=(char*)o->rootPath;
            strcpy(ptr, root);
            baElideDotDot(ptr);
            if(stat(o->rootPath, &statBuf))
               strcpy(ptr, root);
            if(((char*)o->rootPath)[strlen(o->rootPath)-1] != '/')
               strcat(ptr, "/");
            o->rootPathLen = strlen(o->rootPath);
            status=0;
         }
         else
            status=IOINTF_MEM;
      }
      else
         status = IOINTF_NOACCESS;
      if(cwd)
         baFree(cwd);
   }
   return status;
}
#endif


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
   return (int)strlen(buf);
}



/****************************************************************************
                                   BaFile functions
****************************************************************************/

static DiskIo io = {
   {0,0,0,0,0,0,0,0,0,0,0,0},
   (void *)"./",
   0,
   NULL};

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
