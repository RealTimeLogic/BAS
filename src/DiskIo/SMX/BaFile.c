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
 *   COPYRIGHT:  Real Time Logic, 2007
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
This file contains DiskIo and BaFile implementations for SMXFS.

This implementation sets the root directory to '/' by default.

The root directory is implemented as a virtual directory with the
standard SMX disks such as a,b,c etc. as sub-directories. As an
example, the root of C: is /c/.

The root directory can be change with DiskIo_setRootDir. This function
can be called at any time to change the root directory.

Example of valid root directories:
  '/'		Creates a special virtual directory, where A:, B:, C:,
		etc are sub-directories under '/'.
  a:		Set the root to A:\
  b:		Set the root to B:\
  c:/webserver	Set the root to C:\webserver
*/

#include <smxfs.h>
#include <BaServerLib.h>

#define ROOTPT char*
#include <BaDiskIo.h>


/* Convert the SMX DATETIME to the Unix style time in Barracuda.
 */
static BaTime
dateTime2UnixTime(DATETIME* dt)
{
   struct BaTm tm;
   memset(&tm, sizeof(struct BaTm), 0);
   /*
     tm_year: Years since 1900
     wYear: Years since 1981
   */
    tm.tm_year = dt->wYear+81;
    tm.tm_mon = dt->wMonth;
    tm.tm_mday = dt->wDay;
    tm.tm_hour = dt->wHour;
    tm.tm_min = dt->wMinute;
    tm.tm_sec = dt->wSecond;
    return baTm2Time(&tm);
}

static char*
unix2DosPath(char* s)
{
   char* ptr;
   for(ptr = s; *ptr; ptr++)
   {
      if(*ptr == '/')
         *ptr='\\';
   }
   return s;
}


static char*
dos2UnixPath(char* s)
{
   char* ptr;
   for(ptr = s; *ptr; ptr++)
   {
      if(*ptr == '\\')
         *ptr='/';
   }
   return s;
}


/* The virtual root directory iterator.
 */
typedef struct DriveIter
{
      DirIntf super;
      int curDrive;
      char driveName[2];
}DriveIter;

static int
DriveIter_read(DirIntfPtr super)
{
   DriveIter* o = (DriveIter*)super;
   o->curDrive++;
   while(o->curDrive < SFS_MAX_DEV_NUM)
   {
      if(sfs_devstatus(o->curDrive) == SFS_DEVICE_MOUNTED)
         return 0;
      o->curDrive++;
   }
   return IOINTF_NOTFOUND;
}


static const
char* DriveIter_getName(DirIntfPtr super)
{
   DriveIter* o = (DriveIter*)super;
   o->driveName[0] = SFS_FIRST_DRIVE+o->curDrive;
   return o->driveName;
}


static int
DriveIter_stat(DirIntfPtr super, IoStat* st)
{
   memset(st, 0, sizeof(IoStat));
   st->isDir=TRUE;
   return 0;
}


static void
DriveIter_constructor(DriveIter* o)
{
   DirIntf_constructor((DirIntf*)o,
                       DriveIter_read,
                       DriveIter_getName,
                       DriveIter_stat);
   o->curDrive= -1; /* We increment in DriveIter_read */
   o->driveName[1]=0;
}

#define DriveIter_destructor(o)





typedef struct DirIter
{
      DirIntf super;
      FILEINFO fileinfo;
      int id;
      BaBool isFirstTime;
}DirIter;


static int
DirIter_read(DirIntfPtr super)
{
   DirIter* o = (DirIter*)super;
   const U8* an = o->fileinfo.name;
   if(o->id < 0)
      return IOINTF_NOTFOUND;
   if(o->isFirstTime)
   {
      o->isFirstTime=FALSE;
      if( ! (an[0]== '.' && (!an[1] || (an[1]=='.' && !an[2]))) )
         return 0;
   }
   for(;;)
   {
      o->id = sfs_findnext(o->id, &o->fileinfo);
      if(o->id < 0)
         return IOINTF_NOTFOUND;
      an = o->fileinfo.name;
      if(an[0]== '.' && (!an[1] || (an[1]=='.' && !an[2])))
         continue;
      return 0;
   }
}


static const char*
DirIter_getName(DirIntfPtr super)
{
   DirIter* o = (DirIter*)super;
   if(o->id < 0)
      return 0;
   return (const char*)o->fileinfo.name;
}




static int
DirIter_stat(DirIntfPtr super, IoStat* st)
{
   DirIter* o = (DirIter*)super;
   if(o->id < 0)
      return IOINTF_NOTFOUND;
   st->lastModified = dateTime2UnixTime(&o->fileinfo.st_mtime);
   st->isDir = o->fileinfo.bAttr & SFS_ATTR_DIRECTORY ?
      TRUE : FALSE;
   st->size = o->fileinfo.st_size;
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
   o->id = sfs_findfirst(aname, &o->fileinfo);
}


static void
DirIter_destructor(DirIter* o)
{
   if(o->id >= 0)
   {
      sfs_findclose(&o->fileinfo);
      o->id=-1;
   }
}

typedef struct
{
   ResIntf super;
   FILEHANDLE fh;
} DiskRes;


static int
DiskRes_read(ResIntfPtr super, void* buf, size_t maxSize, size_t* size)
{
   DiskRes* o = (DiskRes*)super; /* Downcast */
   if( (*size = sfs_fread(buf, 1, maxSize, o->fh)) == 0)
   {
      return sfs_feof(o->fh) ? IOINTF_EOF : IOINTF_IOERROR;
   }
   return 0;
}


static int
DiskRes_write(ResIntfPtr super, const void* buf, size_t size)
{
   DiskRes* o = (DiskRes*)super; /* Downcast */
   return sfs_fwrite((void*)buf, 1, size, o->fh) == size ?
      0 : IOINTF_IOERROR;
}


static int
DiskRes_seek(ResIntfPtr super, size_t offset)
{
   DiskRes* o = (DiskRes*)super; /* Downcast */
   return sfs_fseek(o->fh, offset, SFS_SEEK_SET) ?
      IOINTF_IOERROR : 0;
}


static int
DiskRes_flush(ResIntfPtr super)
{
   DiskRes* o = (DiskRes*)super; /* Downcast */
   return sfs_fflush(o->fh) ? IOINTF_IOERROR : 0;
}


static int
DiskRes_close(ResIntfPtr super)
{
   FILEHANDLE hndl;
   baAssert(super);
   if(!super) return -1;
   hndl = ((DiskRes*)super)->fh;
   baFree(super);
   super=0;
   sfs_fclose(hndl);
   return 0;
}



static void
DiskRes_constructor(DiskRes* o, FILEHANDLE hndl)
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

   /* The char \ is not seen as a dir sep by BA, thus a security issue */ 
   if(strchr(name, '\\'))
   {
      *status=IOINTF_INVALIDNAME;
      return 0;
   }

   if(*name == '/') name++;
   if( ! o->rootPath )
   {  /* If not using an offset start path i.e. if reading from '/',
       * where '/' contains the drives on the PC.
       */
      const char* ptr;
       /* Must have at minimum the drive i.e. 'c' */
      if( !name[0] || (name[1] && name[1] != '/') )
      {
         *status=IOINTF_INVALIDNAME;
         return 0;
      }
      if(name[1])
         ptr=name+1;
      else
         ptr="";
      len = 3 + strlen(ptr);
      aptr = baMalloc(len+extraChars);
      if(aptr)
      {
         aptr[0]=name[0]; /* Set drive */
         aptr[1]=':';
         aptr[2]=0;
         strcpy(aptr+2, ptr);
         return unix2DosPath(aptr);
      }
   }
   else
   {
      /* If using an offset start path */
      len = strlen(name)+1;
      aptr = baMalloc((o->rootPathLen+len+extraChars));
      if(aptr)
      {
         strcpy(aptr, o->rootPath);
         if(*name) strcpy(aptr+o->rootPathLen, name);
         return unix2DosPath(aptr);
      }
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

   if( !o->rootPath && (!*dirname || (*dirname=='/' && !dirname[1])) )
   {
      DriveIter* di = (DriveIter*)baMalloc(sizeof(DriveIter));
      if(di)
      {
         DriveIter_constructor(di);
         return (DirIntfPtr)di;
      }
      *status=IOINTF_MEM;
   }
   else
   {
      IoStat st;
      if(DiskIo_stat(super, dirname, &st))
         *status=IOINTF_NOTFOUND;
      else if( ! st.isDir )
         *status=IOINTF_EXIST;
      else
      {
         char* aname = DiskIo_mkAbsPath(o, dirname, 2, status);
         if(aname)
         {
            DirIter* di = (DirIter*)baMalloc(sizeof(DirIter));
            if(di)
            {
               int len = strlen(aname)-1;
               if(len > 0 && aname[len] == '\\')
                  strcat(aname, "*");
               else
                  strcat(aname, "\\*");
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
      if(sfs_mkdir(aname) == SB_PASS)
         status=0;
      else
         status=IOINTF_NOACCESS;
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
         if(sfs_rename(afrom, ato))
            status=0;
         else
            status=IOINTF_NOACCESS;
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
      if(sfs_fdelete(aname))
         status=0;
      else
         status=IOINTF_NOACCESS;
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
      if(sfs_rmdir(aname))
         status=0;
      else
         status=IOINTF_NOACCESS;
      baFree(aname);
   }
   return status;
}


static int
DiskIo_stat(IoIntfPtr super, const char* name, IoStat* st)
{
   DiskIo* o = (DiskIo*)super;
   char* aname;
   int status;
   int len;
   BaBool endsWithSlash;

   if(*name == '/') name++;

   if( ! o->rootPath && !*name)
   {
     L_isDisk:
      st->isDir = TRUE;
      st->lastModified = 0;
      st->size = 0;
      return 0;
   }

   len = strlen(name);
   endsWithSlash = (!len || name[len-1] == '/') ? TRUE : FALSE;
   aname = DiskIo_mkAbsPath(o, name, 1, &status);
   if( ! aname )
      return status;
   len = strlen(aname);
   if(len == 2 || (len == 3 && endsWithSlash))
   {
      if(SFS_DEVICE_MOUNTED == sfs_devstatus(toupper(aname[0]) - SFS_FIRST_DRIVE))
         goto L_isDisk;
      status=IOINTF_NOTFOUND;
   }
   else
   {
      FILEINFO fileinfo;
      if(endsWithSlash)
         aname[len-1]=0;
      if(sfs_stat(aname, &fileinfo))
         status=IOINTF_NOTFOUND;
      else
      {
         st->isDir = (fileinfo.bAttr & SFS_ATTR_DIRECTORY) ? TRUE : FALSE;
         st->lastModified = dateTime2UnixTime(&fileinfo.st_mtime);
         st->size = fileinfo.st_size;
         status=0;
      }
   }
   baFree(aname);
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
      access = "r";
   }
   else if(mode == OpenRes_WRITE)
   {
      access = "w";
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
         FILEHANDLE hndl = sfs_fopen(aname, access);
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
      if((*dirIntf)->readFp == DirIter_read)
      {
         DirIter_destructor((DirIter*)*dirIntf);
      }
      else
      {
         baAssert((*dirIntf)->readFp == DriveIter_read);
         DriveIter_destructor((DriveIter*)*dirIntf);
      }
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
      *((U32*)a) = TRUE; /* SMXFS can move directories */
      return 0;
   }
   if( ! strcmp(name, "hidden") )
   {
      char* aname;
      int status;
      if( (aname = DiskIo_mkAbsPath(o, (const char*)a, 0, &status)) != 0)
      {
         FILEINFO fi;
         if( ! sfs_getprop(aname, &fi) )
         {
            fi.bAttr |= SFS_ATTR_HIDDEN;
            sfs_setprop(aname, &fi, SFS_SET_ATTRIBUTE);
            status=0;
         }
         else
            status = IOINTF_NOTFOUND;
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
            *platform="SMX";
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
      int status;
      char* path;
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
   int len;
   char* adir;
   char* cwd=0;

   if(o->rootPath)
      baFree(o->rootPath);
   o->rootPath=0;
   o->rootPathLen=1;

   /* If root set to '/'. A null value is the same as '/' */
   if(!root || (root[0] == '/' && !root[1]))
   {
      return 0;
   }

   if(root[0] != '/' && root[1] != ':') /* If not an absolute path */
   {
      len = 200;
      cwd = baMalloc(len+2);
      if(!cwd)
         return IOINTF_MEM;
      if( ! sfs_getcwd(cwd, len) )
      {
         baFree(cwd);
         return IOINTF_NOTFOUND;
      }
      else
      {
         if(cwd[strlen(cwd)-1] != '\\')
            strcat(cwd, "\\");
      }
   }

   len = strlen(root)+2;
   if(cwd)
      len += strlen(cwd);
   adir = baMalloc(len);
   if(adir)
   {
      IoStat st;
      strcpy(adir, root);
      if(cwd)
      {
         strcpy(adir, cwd);
         strcat(adir, root);
         dos2UnixPath(adir);
         baElideDotDot(adir);
      }
      else
      {
         strcpy(adir, root);
         dos2UnixPath(adir);
      }
      if(adir[0] == '/' && adir[2] == '/')
      {
         /* Translate path of form /c/mydir to c:/mydir */
         adir[0] = adir[1];
         adir[1]=':';
      }
      len = strlen(adir);
      if(adir[len-1] == '/')
         adir[len-1] = 0;
      /* len = 3 if path is say c:\ and we cannot run stat on the disk */
      if(len > 3 && DiskIo_stat((IoIntfPtr)o, adir, &st))
         status=IOINTF_NOTFOUND;
      else if(len > 3 && ! st.isDir )
         status=IOINTF_INVALIDNAME;
      else
      {
         strcat(adir, "/");
         o->rootPath = adir;
         o->rootPathLen=len;
         adir=0;
         status=0;
      }
      if(adir)
         baFree(adir);
   }
   else
      status = IOINTF_MEM;
   if(cwd)
      baFree(cwd);

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
   if(buf[1] == ':')
   {
      /* Translate path of form c:/mydir to /c/mydir */
      buf[1] = buf[0];
      buf[0]='/';
   }   
   return strlen(o->rootPath);
}



/****************************************************************************
                                   BaFile functions
****************************************************************************/

#include <BaFile.h>


static DiskIo io = {
   {0,0,0,0,0,0,0,0,0,0},
   "",
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
