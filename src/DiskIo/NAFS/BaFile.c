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
 *   COPYRIGHT:  Real Time Logic, 2009
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
This file contains DiskIo and BaFile implementations for Digi's NAFS
file system.

This implementation sets the root directory to "FLASH0/" by default.  The
root directory can be changed with DiskIo_setRootDir, which can be
called at any time to change the root directory.

Please see the BaDiskIo.h and IoIntf.h documentation in the
reference manual for information on the API's that are implemented in
this file.

The NAFS support is not compiled and integrated into the Barracuda
library. You must explicitly compile and link this code with your
build if you want NAFS support.

The DiskIo "NAFS permissions" is set to all groups, and the group ID is
set to NAFS_FS_GROUP1 in the DiskIo_constructor. You can either change the
code if you need to change permissions/the group ID or you can call
the property function.

Example, changing permissions and group ID by calling the DiskIo
property function:

void setPermAndGid(DiskIo* dio, unsigned int perm, unsigned int gid)
{ 
   IoIntfPtr io = (IoIntfPtr)dio;
   o->propertyFp(io, "permissions", (void*)&perm, (void*)&gid);
}

This code is delivered "as is" due to the inconsistencies in the NAFS API,
non documented features, and documentation ambiguities.
*/


#include <ThreadLib.h>

#define ROOTPT char*

/* Define the private data type in DiskIo before including the header file.
   DiskIoData is otherwise a void pointer.
   We use DiskIoData to store NAFS private data.
 */
typedef struct
{
      ThreadSemaphore sem; /* Waiting for DiskIo_fsEventCb */
      U32 perm; /* Permission */
      U32 gid; /* Group ID */
      U32 volumeLen; /* strlen(volume name) */
} DiskIoData;
#define DiskIo_DATA DiskIoData* d

#include <BaDiskIo.h>

#include <BaFile.h>
#include <BaServerLib.h>
#include <fs_api.h>
#include <sysaccess.h>


/* Change by calling DiskIo_setRootDir
 */
static const char defaultRoot[] = {"FLASH0/"};


/* Convert NAFS error to a IOINTF_XXX error code.
*/
static int
doError(int err)
{
   switch(err)
   {
      case NAFS_DIR_ENTRY_NOT_FOUND:
         return IOINTF_NOTFOUND;
      case NAFS_DIR_NOT_EMPTY:
         return IOINTF_NOTEMPTY;
      case NAFS_DUPLICATE_DIR_ENTRY:
         return IOINTF_EXIST;
      case NAFS_END_OF_FILE:
         return IOINTF_EOF;

      case NAFS_INVALID_DIR_PATH_LENGTH:
      case NAFS_INVALID_DIR_NAME_LENGTH:
      case NAFS_INVALID_FILE_NAME:
      case NAFS_INVALID_FILE_NAME_LENGTH:
      case NAFS_INVALID_PATH_NAME:
      case NAFS_NULL_DIR_ENTRY_NAME:
         return IOINTF_INVALIDNAME;

      case NAFS_FILE_IS_OPEN:
      case NAFS_RENAME_DIR_ENTRY_FAILED:
      case NAFS_NO_READ_PERMISSION:
      case NAFS_NO_WRITE_PERMISSION:
      case NAFS_INVALID_FILE_ACCESS_TYPE:
      case NAFS_INVALID_FILE_TYPE:
      case NAFS_INVALID_GROUP_ID:
      case NAFS_INVALID_USER_PERMISSIONS:
         return IOINTF_NOACCESS;

      case NAFS_MALLOC_FAILED:
         return IOINTF_MEM;

      case NAFS_DIR_TABLE_FULL:
      case NAFS_FILE_TABLE_FULL:
      case NAFS_IO_INTF_TABLE_FULL:
      case NAFS_MEDIA_CB_VOLUME_TABLE_FULL:
      case NAFS_MSG_QUEUE_FULL:
      case NAFS_NO_FREE_INODES:
      case NAFS_SERVICES_TABLE_FULL:
      case NAFS_NO_FREE_BLOCKS:
         return IOINTF_NOSPACE;
   }
   return IOINTF_IOERROR;
}


/**************** Forw. Decl ****************/ 

static int DiskIo_stat(IoIntfPtr super, const char* rname, IoStat* st);
static int DiskIo_closeDir(IoIntfPtr super, DirIntfPtr* dirIntf);


/**************** Private DiskIo functions ****************/ 


/* Wait for the NAFS_IO_REQUEST_CB file system callback.
   This function signals the waiting Barracuda thread, which waits in
   function DiskIo_fsWait.
*/
static void
DiskIo_fsEventCb(void * d)
{
   ThreadSemaphore_signal(
      (ThreadSemaphore*)((NAFS_IO_REQUEST_CB*)d)->user_data);
}


/* Init NAFS_IO_REQUEST_CB
 */
static void
DiskIo_initCb(DiskIo* o, NAFS_IO_REQUEST_CB* req)
{
   int s=NAFSinit_io_request_cb(req,DiskIo_fsEventCb,(unsigned int)&o->d->sem);
   if(s)
      baFatalE(FE_INCORRECT_USE, s);
}


/*
  Wait for file system callback -- i.e. wait for DiskIo_fsEventCb.
 */
static int
DiskIo_fsWait(DiskIo* o, NAFS_IO_REQUEST_CB* req, int* status)
{
   ThreadSemaphore_wait((ThreadSemaphore*)&o->d->sem);
   if(req->status == NAFS_IO_REQUEST_COMPLETED)
   {
      *status=0;
      return 0;
   }
   *status = doError(req->status == NAFS_IO_REQUEST_TERMINATED ?
                     req->error_code : req->status);
   return -1;
}


/* o->rootPath was dynamically allocated if not pointing to defaultRoot.
 */
static void
DiskIo_freeRootDir(DiskIo* o)
{
   if(o->rootPath && o->rootPath != defaultRoot)
      baFree(o->rootPath);
   o->rootPath = (char*)defaultRoot;
   o->rootPathLen = strlen(defaultRoot);
   o->d->volumeLen = o->rootPathLen;
}


/* Make absolute path by adding rootPath + name.
 * Space to hold the returned string is obtained from baMalloc.
 */
static char*
DiskIo_mkAbsPath(DiskIo* o, const char* name, int* status)
{
   char* retVal;
   if(*name == '/') name++;
   retVal = baMalloc(o->rootPathLen+strlen(name)+1);
   if(retVal)
   {
      memcpy(retVal, o->rootPath, o->rootPathLen);
      strcpy(retVal+o->rootPathLen, name);
   }
   else
      *status=IOINTF_MEM;
   return retVal;
}


/* Convert rname (relative path and/or name) to an absolute path+name.
   Split the path+name into path and name.
   The complexity in this function comes from NAFS requirements, where
   a volume name must end with a slash, but not a directory.
*/
static char*
DiskIo_mkPathName(DiskIo* o, const char* rname,
                  char** opath, char** oname,
                  int* status)
{
   char* aname;
   int endsWithSlash;
   char* ptr;
   const char* endSlashPtr;
   *status = 0;
   if(*rname == '/') rname++;
   endSlashPtr = strrchr(rname, '/');
   endsWithSlash = endSlashPtr && !endSlashPtr[1];
   if(o->rootPathLen == o->d->volumeLen)
   {
      if(!endSlashPtr) /* No slash in name */
      {
         *opath = o->rootPath;
         *oname = (char*)rname;
         return 0;
      }
      if(strchr(rname, '/') == endSlashPtr && endsWithSlash)
      { /* No slash in name, except for ending with a slash */
         aname = baMalloc(o->rootPathLen+strlen(rname)+2);
         if(aname)
         { /* Directory is volume name and must end with a slash */
            strcpy(aname, o->rootPath);
            *opath=aname;
            ptr = o->rootPath + o->rootPathLen + 1;
            strcpy(ptr, rname);
            ptr[strlen(ptr)-1] = 0; /* Remove trailing slash */
            *oname = ptr;
         }
         else
            *status=IOINTF_MEM;
         return aname;
      }
   }
   aname = DiskIo_mkAbsPath(o, rname, status);
   if(aname)
   {
      if(endsWithSlash)
         aname[strlen(aname)-1] = 0; /* Remove */
      ptr = strrchr(aname, '/');
      baAssert(ptr);
      *ptr = 0; /* Remove trailing slash in directory name */
      *opath = aname;
      *oname = ptr+1;
   }
   return aname;
}



/* DirIter maintains state information for DiskIo_openDir,
 * DirIter_read, DirIter_getName, DirIter_stat, and DiskIo_closeDir.
 * A DirIter instance is created in DiskIo_openDir and released in
 * DiskIo_closeDir
 */
typedef struct
{
      DirIntf super; /* Implements the abstract DirIntf class */
      S32 cursor;
      S32 len;
      NAFS_DIR_ENTRY_INFO info[1]; /* Must be last, dynamic array */
} DirIter;


static int
DirIter_read(DirIntfPtr super)
{
   DirIter* o = (DirIter*)super;
   return o->len > ++o->cursor ? 0 : IOINTF_NOTFOUND;
}


static const char*
DirIter_getName(DirIntfPtr super)
{
   DirIter* o = (DirIter*)super;
   return o->cursor >= 0 && o->len > o->cursor ?
      o->info[o->cursor].entry_name : 0;
}


static int
DirIter_stat(DirIntfPtr super, IoStat* st)
{
   DirIter* o = (DirIter*)super;
   if(o->cursor >= 0 && o->len > o->cursor)
   {
      NAFS_DIR_ENTRY_INFO* iptr = o->info + o->cursor;
      st->lastModified = iptr->last_modified_time;
      if(iptr->entry_type == NAFS_FILE_TYPE_DIR)
      {
         st->isDir = TRUE;
         st->size = 0;
      }
      else
      {
         st->isDir = FALSE;
         st->size = iptr->file_size;
      }
      return 0;
   }
   return IOINTF_NOTFOUND;
}


static void
DirIter_constructor(DirIter* o, S32 dirEntryCount)
{
   DirIntf_constructor((DirIntf*)o,
                       DirIter_read,
                       DirIter_getName,
                       DirIter_stat);
   o->cursor=-1; /* Caller starts by calling DirIter_read */
   o->len=dirEntryCount;
}


/* Not needed */
#define DirIter_destructor(o)


/* DiskRes maintains state information for DiskIo_openRes,
 * DiskRes_read, DiskRes_write, DiskRes_seek, DiskRes_flush, and
 * DiskIo_closeRes.
 * A DiskRes instance is created in DiskIo_openRes and released in
 * DiskIo_closeRes.
 */
typedef struct
{
      ResIntf super; /* Implements the abstract ResIntf class */
      DiskIo* io;
      unsigned int fh; /* file handle */
      unsigned int cursor;
      unsigned int size;
} DiskRes;



static int
DiskRes_read(ResIntfPtr super, void* buf, size_t maxSize, size_t* size)
{
   DiskRes* o = (DiskRes*)super; /* upcast */
   int s;
   NAFS_IO_REQUEST_CB req;
   *size=0;
   DiskIo_initCb(o->io, &req);
   if(!(s=NAFSread_file(
           o->fh,o->io->d->perm,maxSize,o->cursor,(char*)buf,&req)))
   {
      if(!DiskIo_fsWait(o->io,&req,&s) || req.error_code == NAFS_END_OF_FILE)
      {
         *size = req.bytes_transferred;
         o->cursor += req.bytes_transferred;
      }
   }
   else
      s=doError(s);
   return s;
}


static int
DiskRes_write(ResIntfPtr super, const void* buf, size_t size)
{
   DiskRes* o = (DiskRes*)super; /* upcast */
   int s;
   NAFS_IO_REQUEST_CB req;
   DiskIo_initCb(o->io, &req);
   if(!(s=NAFSwrite_file(o->fh,o->io->d->perm,size,o->cursor,(char*)buf,&req,
                         NAFS_ZERO_OFFSET_ERASE)))
   {
      if(!DiskIo_fsWait(o->io,&req,&s))
      {
         o->cursor += req.bytes_transferred;
         if(o->size < o->cursor)
            o->size = o->cursor;
         if(req.bytes_transferred != size)
            s=IOINTF_IOERROR;
      }
   }
   else
      s=doError(s);
   return s;
}


static int
DiskRes_seek(ResIntfPtr super, size_t offset)
{
   DiskRes* o = (DiskRes*)super; /* upcast */
   if(offset < o->size)
   {
      o->cursor = offset;
      return 0;
   }
   return IOINTF_EOF;
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
   NAFS_IO_REQUEST_CB req;
   DiskRes* o;
   int s;
   baAssert(super);
   if(!super) return -1;
   o = (DiskRes*)super;
   DiskIo_initCb(o->io, &req);
   if( ! (s=NAFSclose_file(o->fh, o->io->d->perm, &req)) )
      DiskIo_fsWait(o->io, &req, &s);
   else
      s=doError(s);
   baFree(super);
   return s;
}


static void
DiskRes_constructor(DiskRes* o, DiskIo* io, unsigned int fh, unsigned int size)
{
   ResIntf_constructor((ResIntf*)o,
                       DiskRes_read,
                       DiskRes_write,
                       DiskRes_seek,
                       DiskRes_flush,
                       DiskRes_close);
   o->io=io;
   o->fh=fh;
   o->size=size;
   o->cursor=0;
}


static DirIter*
DiskIo_openAbsDir(DiskIo* o, char* aname, int* status)
{
   NAFS_IO_REQUEST_CB req;
   unsigned int dirEntryCount;
   int s;
   DirIter* di = 0;
   char* ptr = strrchr(aname, '/');
   baAssert(ptr);
   if( ! ptr[1] ) /* If ends with '/' */
   {
      /* Volume name must end with '/', but not a directory */
      if((ptr - aname) > o->d->volumeLen)
      {
         if( ! ptr[1] ) /* ends with '/' */
            ptr[0] = 0; /* Removing trailing '/' */
      }
   }
   DiskIo_initCb(o, &req);
   if(!(s=NAFSdir_entry_count(aname,o->d->perm,&dirEntryCount,&req)))
   {
      if(!DiskIo_fsWait(o,&req,status))
      {
         di = (DirIter*) baMalloc(
            sizeof(DirIter) +
            sizeof(NAFS_DIR_ENTRY_INFO) * dirEntryCount);
         if(di)
         {
            DirIter_constructor(di, dirEntryCount);
            if(dirEntryCount != 0)
            {
               if(!(s=NAFSlist_dir(
                       aname,o->d->perm,di->info,dirEntryCount,&req)))
               {
                  DiskIo_fsWait(o, &req, status);
               }
               else
                  *status=doError(s);
            }
         }
         else
            *status=IOINTF_MEM;
      }
   }
   else
      *status=doError(s);
   if(*status && di)
   {
      baFree(di);
      di = 0;
   }
   return di;
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
      DirIntfPtr di = (DirIntfPtr)DiskIo_openAbsDir(o, aname, status);
      baFree(aname);
      return di;
   }
   return 0;
}


static int
DiskIo_mkDir(IoIntfPtr super, const char* rname, const char** ecode)
{
   char* aname;
   int s;
   char* path;
   char* name;
   DiskIo* o = (DiskIo*)super;
   if(*ecode) *ecode=0;
   aname = DiskIo_mkPathName(o, rname, &path, &name, &s);
   if(!s)
   {
      NAFS_IO_REQUEST_CB req;
      DiskIo_initCb(o, &req);
      if( ! (s=NAFScreate_dir(path, name, o->d->perm, o->d->gid, &req)) )
         DiskIo_fsWait(o, &req, &s);
      else
         s=doError(s);
   }
   if(aname)
      baFree(aname);
   return s;
}


static int
DiskIo_rename(
   IoIntfPtr super, const char* rfrom, const char* rto, const char** ecode)
{
   IoStat st;
   DiskIo* o = (DiskIo*)super;
   int s = DiskIo_stat(super, rfrom, &st);
   if(*ecode) *ecode=0;
   if(!s)
   {
      char* fpath;
      char* fname;
      char* aname1 = DiskIo_mkPathName(o, rfrom, &fpath, &fname, &s);
      if(!s)
      {
         char* tpath;
         char* tname;
         char* aname2 = DiskIo_mkPathName(o, rto, &tpath, &tname, &s);
         if(!s)
         {
            if( ! strcmp(fpath, tpath) )
            {
               NAFS_IO_REQUEST_CB req;
               DiskIo_initCb(o, &req);
               s = st.isDir ?
                  NAFSrename_dir(fpath,fname,tname,o->d->perm,&req) :
                  NAFSrename_file(fpath,fname,tname,o->d->perm,&req);
               if(s)
                  s=doError(s);
               else
                  DiskIo_fsWait(o, &req, &s);
            }
            else
               s = IOINTF_INVALIDNAME;
         }
         if(aname2)
            baFree(aname2);
      }
      if(aname1)
            baFree(aname1);
   }
   return s;
}

/* Used by DiskIo_remove & DiskIo_rmDir
 */
static int
DiskIo_delete(DiskIo* o, int isDir, const char* rname, const char** ecode)
{
   char* aname;
   int s;
   char* path;
   char* name;
   if(*ecode) *ecode=0;
   aname = DiskIo_mkPathName(o, rname, &path, &name, &s);
   if(!s)
   {
      NAFS_IO_REQUEST_CB req;
      DiskIo_initCb(o, &req);
      s = isDir ? NAFSdelete_dir(path, name, o->d->perm, &req) :
         NAFSdelete_file(path, name, o->d->perm, &req);
      if(s)
         s=doError(s);
      else
         DiskIo_fsWait(o, &req, &s);
   }
   if(aname)
      baFree(aname);
   return s;
}


static int
DiskIo_remove(IoIntfPtr super, const char* rname, const char** ecode)
{
   return DiskIo_delete((DiskIo*)super, FALSE, rname, ecode);
}


static int
DiskIo_rmDir(IoIntfPtr super, const char* rname, const char** ecode)
{
   return DiskIo_delete((DiskIo*)super, TRUE, rname, ecode);
}


/* No stat() compatible function in NAFS so we use the directory
 * iterator to get the information.
 */
static int
DiskIo_stat(IoIntfPtr super, const char* rname, IoStat* st)
{
   char* aname;
   int status;
   char* path;
   char* name;
   DiskIo* o = (DiskIo*)super;
   aname = DiskIo_mkPathName(o, rname, &path, &name, &status);
   if(!status)
   {
      DirIter* di = DiskIo_openAbsDir(o, path, &status);
      if(di)
      {
         if(*name)
         {
            NAFS_DIR_ENTRY_INFO* iptr = di->info;
            status = IOINTF_NOTFOUND;
            while(di->len > 0)
            {
               if( ! strcmp(name, iptr->entry_name) )
               {
                  status = 0;
                  st->isDir = iptr->entry_type == NAFS_FILE_TYPE_DIR;
                  st->lastModified = iptr->last_modified_time;
                  st->size = iptr->file_size;
                  break;
               }
               iptr++;
               di->len--;
            }
         }
         else
         { /* The Volume */
            st->isDir = TRUE;
            st->lastModified = st->size = 0;
         }
         DiskIo_closeDir(super, (DirIntfPtr*)&di);
      }
      else if(status == IOINTF_NOTFOUND)
         status = IOINTF_ENOENT; /* Parent */
   }
   if(aname)
      baFree(aname);
   return status;
}


static ResIntfPtr
DiskIo_openRes(IoIntfPtr super, const char* rname, U32 mode,
               int* status, const char** ecode)
{
   char* aname;
   int s;
   char* path;
   char* name;
   ResIntfPtr retVal=0;
   DiskIo* o = (DiskIo*)super;
   if(*ecode) *ecode=0;
   aname = DiskIo_mkPathName(o, rname, &path, &name, status);
   if(!*status)
   {
      NAFS_IO_REQUEST_CB req;
      unsigned int fh;
      DiskIo_initCb(o, &req);
      /* Ignore errors from create. The file may already exist. */
      if( ! NAFScreate_file(path,name,o->d->perm,o->d->gid,&req) )
         DiskIo_fsWait(o,&req,&s);
      if( ! (s=NAFSopen_file(path,name,&fh,o->d->perm,&req)) )
      {
         if( ! DiskIo_fsWait(o, &req, status) )
         {
            unsigned int size;
            if( ! (s=NAFSopen_file_size(fh, o->d->perm, &size, &req)) )
            {
               if( ! DiskIo_fsWait(o, &req, status) )
               {
                  DiskRes* dr = (DiskRes*)baMalloc(sizeof(DiskRes));
                  if(dr)
                  {
                     DiskRes_constructor(dr,o,fh,size);
                     retVal = (ResIntfPtr)dr; /* Cast to base class */
                  }
                  else
                     *status=IOINTF_MEM;
               }
            }
         }
         if( ! retVal )
         {
            if(!NAFSclose_file(fh, o->d->perm, &req))
               DiskIo_fsWait(o, &req, &s);
         }
      }
      else
         *status=doError(s);
   }
   if(aname)
      baFree(aname);
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
      *((U32*)a) = FALSE;
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
            *platform="NAFS";
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
         status=IOINTF_MEM;
      if(status)
         DiskIo_destructor(di);
      return status;
   }
   if( ! strcmp(name, "destructor") )
   {
      DiskIo_destructor(o);
      return 0;
   }
   if( ! strcmp(name, "permissions") )
   {
      unsigned int* perm = (unsigned int*)a;
      unsigned int* gid = (unsigned int*)b;
      o->d->perm = *perm;
      o->d->gid = *gid;
      return 0;
   }
   return IOINTF_NOIMPLEMENTATION;
}


void
DiskIo_constructor(DiskIo* o)
{
   memset(o, 0, sizeof(DiskIo));
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
   o->d = (DiskIoData*)baMalloc(sizeof(DiskIoData));
   if(!o->d)
      baFatalE(FE_MALLOC, 0);
   DiskIo_setRootDir(o, 0);
   ThreadSemaphore_constructor(&o->d->sem);
   o->d->perm = 0xFFFFFFFF;
   o->d->gid = NAFS_FS_GROUP1;
}


void
DiskIo_destructor(DiskIo* o)
{
   IoIntfPtr super = (IoIntfPtr)o;
   if(super->onTerminate)
      super->onTerminate(super->attachedIo, super);
   if(o->d)
   {
      ThreadSemaphore_destructor(&o->d->sem);
      baFree(o->d);
      o->d=0;
   }
   DiskIo_freeRootDir(o);
}


int
DiskIo_setRootDir(DiskIo* o, const char* root)
{
   int status = 0;
   DiskIo_freeRootDir(o);
   if( !root || (root[0] == '/' && !root[1]) )
   {
      /* All done in DiskIo_freeRootDir */
   }
   else
   {
      IoStat st;
      if(*root == '/')
         root++;
      o->rootPath = baMalloc(strlen(root)+2);
      if(o->rootPath)
      {
         strcpy(o->rootPath, root);
         char* ptr = strrchr(o->rootPath, '/');
         if(!ptr || ptr[1])
            strcat(o->rootPath, "/");
         o->rootPathLen = strlen(o->rootPath);
         o->d->volumeLen = (strchr(o->rootPath, '/') - o->rootPath) + 1;
         if(DiskIo_stat((IoIntfPtr)o, "/", &st) || !st.isDir)
            status = IOINTF_NOTFOUND;
      }
      else
         status = IOINTF_MEM;
      if(status)
         DiskIo_freeRootDir(o);
   }
   return status;
}


int
DiskIo_getRootDir(DiskIo* o, char* buf, int len)
{
   if(len < o->rootPathLen)
   {
      return -1;
   }
   strcpy(buf,o->rootPath);
   return strlen(buf);
}
