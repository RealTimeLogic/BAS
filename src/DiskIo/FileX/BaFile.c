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


Example, set FX_MEDIA by calling the DiskIo
property function:

void setFxMedia(DiskIo* dio, FX_MEDIA* mptr)
{ 
   IoIntfPtr io = (IoIntfPtr)dio;
   io->propertyFp(io, "setmedia", (void*)&mptr, 0);
}

*/






/* Define the private data type in DiskIo before including the header file.
   DiskIoData is otherwise a void pointer.
   We use DiskIoData to store a pointer to the FX_MEDIA.
 */
#include <fx_api.h> 
#define DiskIo_DATA FX_MEDIA* mptr
#define ROOTPT char*
#include <BaDiskIo.h>

#include <BaFile.h>
#include <BaServerLib.h>


/* Change by calling DiskIo_setRootDir
 */
static const char defaultRoot[] = {"/"};


static int DiskIo_stat(IoIntfPtr super, const char* rname, IoStat* st);
static int DiskIo_closeDir(IoIntfPtr super, DirIntfPtr* dirIntf);


/* o->rootPath was dynamically allocated if not pointing to defaultRoot.
 */
static void
DiskIo_freeRootDir(DiskIo* o)
{
   if(o->rootPath && o->rootPath != defaultRoot)
      baFree(o->rootPath);
   o->rootPath = (char*)defaultRoot;
   o->rootPathLen = strlen(defaultRoot);
}


/* Convert to a IOINTF_XXX error code.
*/
static int
doError(int err, const char** ecode)
{
   char* dummy;
   if(!ecode) ecode=&dummy;
   switch(err)
   {
      case FX_NOT_FOUND:
         *ecode="FX_NOT_FOUND";
         return IOINTF_NOTFOUND;
      case FX_END_OF_FILE:
         *ecode="FX_END_OF_FILE";
         return IOINTF_EOF;
      case FX_NO_MORE_SPACE:
         *ecode="FX_NO_MORE_SPACE";
         return IOINTF_NOSPACE;
      case FX_ALREADY_CREATED:
         *ecode="FX_ALREADY_CREATED";
         return IOINTF_EXIST;
      case FX_INVALID_NAME:
         *ecode="FX_INVALID_NAME";
         return IOINTF_INVALIDNAME;
      case FX_INVALID_PATH:
         *ecode="FX_INVALID_PATH";
         return  IOINTF_ENOENT;
      case FX_NO_MORE_ENTRIES:
         *ecode="FX_NO_MORE_ENTRIES";
         return IOINTF_NOTFOUND;
      case FX_DIR_NOT_EMPTY:
         *ecode="FX_DIR_NOT_EMPTY";
         return  IOINTF_NOTEMPTY;
      case FX_WRITE_PROTECT:
         *ecode="FX_WRITE_PROTECT";
         return IOINTF_NOACCESS;
      case FX_NOT_ENOUGH_MEMORY:
         *ecode="FX_NOT_ENOUGH_MEMORY";
         return IOINTF_MEM;

      case FX_BOOT_ERROR: *ecode="FX_BOOT_ERROR"; break;
      case FX_MEDIA_INVALID: *ecode="FX_MEDIA_INVALID"; break;
      case FX_FAT_READ_ERROR: *ecode="FX_FAT_READ_ERROR"; break;
      case FX_NOT_A_FILE: *ecode="FX_NOT_A_FILE"; break;
      case FX_ACCESS_ERROR: *ecode="FX_ACCESS_ERROR"; break;
      case FX_NOT_OPEN: *ecode="FX_NOT_OPEN"; break;
      case FX_FILE_CORRUPT: *ecode="FX_FILE_CORRUPT"; break;
      case FX_NOT_DIRECTORY: *ecode="FX_NOT_DIRECTORY"; break;
      case FX_MEDIA_NOT_OPEN: *ecode="FX_MEDIA_NOT_OPEN"; break;
      case FX_INVALID_YEAR: *ecode="FX_INVALID_YEAR"; break;
      case FX_INVALID_MONTH: *ecode="FX_INVALID_MONTH"; break;
      case FX_INVALID_DAY: *ecode="FX_INVALID_DAY"; break;
      case FX_INVALID_HOUR: *ecode="FX_INVALID_HOUR"; break;
      case FX_INVALID_MINUTE: *ecode="FX_INVALID_MINUTE"; break;
      case FX_INVALID_SECOND: *ecode="FX_INVALID_SECOND"; break;
      case FX_PTR_ERROR: *ecode="FX_PTR_ERROR"; break;
      case FX_INVALID_ATTR: *ecode="FX_INVALID_ATTR"; break;
      case FX_CALLER_ERROR: *ecode="FX_CALLER_ERROR"; break;
      case FX_BUFFER_ERROR: *ecode="FX_BUFFER_ERROR"; break;
      case FX_NOT_IMPLEMENTED: *ecode="FX_NOT_IMPLEMENTED"; break;
      case FX_INVALID_OPTION: *ecode="FX_INVALID_OPTION"; break;
      case FX_SECTOR_INVALID: *ecode="FX_SECTOR_INVALID"; break;
      case FX_IO_ERROR: *ecode="FX_IO_ERROR"; break;
      case FX_ERROR_FIXED: *ecode="FX_ERROR_FIXED"; break;
      case FX_ERROR_NOT_FIXED: *ecode="FX_ERROR_NOT_FIXED"; break;
   }
   return IOINTF_IOERROR;
}


/* DirIter maintains state information for DiskIo_openDir,
 * DirIter_read, DirIter_getName, DirIter_stat, and DiskIo_closeDir.
 * A DirIter instance is created in DiskIo_openDir and released in
 * DiskIo_closeDir
 */
typedef struct
{
      DirIntf super; /* Implements the abstract DirIntf class */
      FX_LOCAL_PATH lp;
      FX_MEDIA* mptr;
      ULONG size; 
      UINT year;
      UINT month;
      UINT day;
      UINT hour;
      UINT minute;
      UINT second;
      BaBool isDir;
      BaBool firstEntry;
      char name[FX_MAX_LONG_NAME_LEN];
      char path[1]; /* Dynamic */
} DirIter;


static int
DirIter_read(DirIntfPtr super)
{
   DirIter* o = (DirIter*)super;
   UINT s;
   UINT attributes=0; /* Not used */
   if(o->firstEntry)
   {
      s = fx_directory_first_full_entry_find(o->mptr, 
                                             o->name, 
                                             &attributes,
                                             &o->size,
                                             &o->year, 
                                             &o->month, 
                                             &o->day, 
                                             &o->hour, 
                                             &o->minute, 
                                             &o->second);
      o->firstEntry=FALSE;
   }
   else
   {
      if( (s=fx_directory_local_path_restore(o->mptr, &o->lp)) != FX_SUCCESS)
         return doError(s, 0);
      s = fx_directory_next_full_entry_find(o->mptr, 
                                            o->name, 
                                            &attributes,
                                            &o->size,
                                            &o->year, 
                                            &o->month, 
                                            &o->day, 
                                            &o->hour, 
                                            &o->minute, 
                                            &o->second);

   }
   if(s == FX_SUCCESS)
      o->isDir = attributes & FX_DIRECTORY ? TRUE : FALSE;
   else
      return doError(s, 0);
   return 0;
}


static const char*
DirIter_getName(DirIntfPtr super)
{
   DirIter* o = (DirIter*)super;
   return o->name;
}


static int
DirIter_stat(DirIntfPtr super, IoStat* st)
{
   DirIter* o = (DirIter*)super;
   struct BaTm t;
   t.tm_sec = (int)o->second;
   t.tm_min = (int)o->minute;
   t.tm_hour = (int)o->hour;
   t.tm_mday = (int)o->day;
   t.tm_mon = (int)o->month;
   t.tm_year = (int)o->year + 80; /* Convert from 1980 to 1900 */
   st->lastModified = baTm2Time(&t);
   st->size = o->size;
   st->isDir = o->isDir;
   return 0;
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
      FX_FILE file;

} DiskRes;


static int
DiskRes_read(ResIntfPtr super, void* buf, size_t maxSize, size_t* size)
{
   DiskRes* o = (DiskRes*)super; /* upcast */
   UINT s;
   ULONG actualSize;
   if( (s=fx_file_read(&o->file, buf, maxSize,&actualSize)) !=0 )
   {
      *size = 0;
      return doError(s,0);
   }
   *size = actualSize;
   return 0;
}


static int
DiskRes_write(ResIntfPtr super, const void* buf, size_t size)
{
   DiskRes* o = (DiskRes*)super; /* upcast */
   UINT s;
   if( (s=fx_file_write(&o->file, (void*)buf, size)) !=0 )
   {
      return doError(s,0);
   }
   return 0;
}


static int
DiskRes_seek(ResIntfPtr super, size_t offset)
{
   DiskRes* o = (DiskRes*)super; /* upcast */
   UINT s;
   if( (s = fx_file_seek(&o->file, offset)) !=0 )
      return doError(s,0);
   return 0;
}


/*
  Not implemented
 */
static int
DiskRes_flush(ResIntfPtr super)
{
   (void)super;
   return 0;
}


static int
DiskRes_close(ResIntfPtr super)
{
   UINT s;
   baAssert(super);
   if(!super) return -1;
   s = fx_file_close(&((DiskRes*)super)->file);
   baFree(super);
   return s ? doError(s,0) : 0;
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

   /* The char \ is not seen as a dir sep by BA, thus a security issue */ 
   if(strchr(name, '\\'))
   {
      *status=IOINTF_INVALIDNAME;
      return 0;
   }

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
   }
   else
      *status=E_MALLOC;
   return retVal;
}


static DirIntfPtr
DiskIo_openAbsDir(DiskIo* o, CHAR* aname, int* status, const char** ecode)
{
   DirIter* di = (DirIter*)baMalloc(sizeof(DirIter)+strlen(aname));
   if(di)
   {
      UINT s;
      memset(di,0,sizeof(di));
      s = fx_directory_local_path_set(o->mptr, &di->lp, aname);
      if(!s)
      {
         DirIntf_constructor((DirIntf*)di,
                             DirIter_read,
                             DirIter_getName,
                             DirIter_stat);
         di->mptr=o->mptr;
         di->firstEntry = TRUE;
         strcpy(di->path, aname);
         return (DirIntfPtr)di;
      }
      *status = doError(s, ecode);
      baFree(di);
   }
   else
      *status = IOINTF_MEM;
   return 0;
}


static DirIntfPtr
DiskIo_openDir(IoIntfPtr super, const char* rname,
               int* status, const char** ecode)
{
   DiskIo* o = (DiskIo*)super;
   CHAR* aname;
   if(ecode) *ecode=0;
   aname = DiskIo_mkAbsPath(o, rname, status);
   if(aname)
   {
      DirIntfPtr di;
      CHAR* endSlashPtr = strrchr(aname, '/');
      if(endSlashPtr && endSlashPtr != aname && !endSlashPtr[1]) /* ends with / */
         *endSlashPtr=0; /* Remove */
      di = DiskIo_openAbsDir(o, aname, status, ecode);
      baFree(aname);
      return di;
   }
   return 0;
}


static int
DiskIo_mkDir(IoIntfPtr super, const char* rname, const char** ecode)
{
   CHAR* aname;
   UINT s;
   DiskIo* o = (DiskIo*)super;
   if(!o->mptr)
      return IOINTF_IOERROR;
   if(ecode) *ecode=0;
   if( (aname = DiskIo_mkAbsPath(o, rname, &s)) != 0)
   {
      s = fx_directory_create(o->mptr, aname);
      if(s)
         s=doError(s, ecode);
      baFree(aname);
   }
   return s;
}

static int
DiskIo_rename(
   IoIntfPtr super, const char* rfrom, const char* rto, const char** ecode)
{
   IoStat st;
   DiskIo* o = (DiskIo*)super;
   int s = DiskIo_stat(super, rfrom, &st);
   if(!o->mptr)
      return IOINTF_IOERROR;
   if(*ecode) *ecode=0;
   if(!s)
   {
      char* afrom = DiskIo_mkAbsPath(o, rfrom, &s);
      if(afrom)
      {
         char* ato = DiskIo_mkAbsPath(o, rto, &s);
         if(ato)
         {
            s = st.isDir ?
               fx_directory_rename(o->mptr, afrom, ato) :
               fx_file_rename(o->mptr, afrom, ato);
            if(s)
               s=doError(s, ecode);
            baFree(ato);
         }
         baFree(afrom);
      }
   }
   return s;
}


static int
DiskIo_remove(IoIntfPtr super, const char* rname, const char** ecode)
{
   DiskIo* o = (DiskIo*)super;
   CHAR* aname;
   UINT s;
   if(!o->mptr)
      return IOINTF_IOERROR;
   if(ecode) *ecode=0;
   if( (aname = DiskIo_mkAbsPath(o, rname, &s)) != 0)
   {
      s = fx_file_delete(o->mptr, aname);
      baFree(aname);
      if(s)
         s=doError(s, ecode);
   }
   return s;
}


static int
DiskIo_rmDir(IoIntfPtr super, const char* rname, const char** ecode)
{
   DiskIo* o = (DiskIo*)super;
   CHAR* aname;
   UINT s;
   if(!o->mptr)
      return IOINTF_IOERROR;
   if(ecode) *ecode=0;
   if( (aname = DiskIo_mkAbsPath(o, rname, &s)) != 0)
   {
      s = fx_directory_delete(o->mptr, aname);
      baFree(aname);
      if(s)
         s=doError(s, ecode);
   }
   return s;
}


static int
DiskIo_stat(IoIntfPtr super, const char* rname, IoStat* st)
{
   int status;
   CHAR* aname;
   UINT s;
   DiskIo* o = (DiskIo*)super;
   if(!*rname || (rname[0] == '/' && ! rname[1]))
   {
      st->isDir=TRUE;
      st->lastModified=0;
      st->size=0;
      return 0;
   }
   aname = DiskIo_mkAbsPath(o, rname, &s);
   if(aname)
   {
      CHAR* path;
      CHAR* endSlashPtr;
      CHAR* name;
      DirIntfPtr di;
      endSlashPtr = strrchr(aname, '/');
      baAssert(endSlashPtr); /* aname starts with /.... */
      if(endSlashPtr != aname && !endSlashPtr[1])
      {
         *endSlashPtr=0; /* Searching for a dir. Remove trailing slash. */
         endSlashPtr = strrchr(aname, '/');
      }
      if(endSlashPtr == aname)
      {
         path="/";
         name=aname+1;
      }
      else
      {
         baAssert(endSlashPtr); /* aname starts with /.... */
         path=aname;
         *endSlashPtr=0;
         name=endSlashPtr+1;
      }
      di = DiskIo_openAbsDir(o, path, &status, 0);
      if(di)
      {
         status = IOINTF_NOTFOUND;
         while( !(DirIter_read(di)) )
         {
            if( ! strcmp(name, ((DirIter*)di)->name) )
            {
               status=DirIter_stat(di, st);
               break;
            }
         }
         DiskIo_closeDir(super, &di);
      }
      else if(status == IOINTF_NOTFOUND)
         status = IOINTF_ENOENT; /* Parent dir not found */
      baFree(aname);
   }
   else
      status = IOINTF_MEM;
   return status;
}


static ResIntfPtr
DiskIo_openRes(IoIntfPtr super, const char* rname, U32 mode,
               int* status, const char** ecode)
{
   CHAR* aname;
   DiskIo* o = (DiskIo*)super; /* Upcast */
   ResIntfPtr retVal=0;
   if(ecode) *ecode=0;
   if(!o->mptr)
   {
      *status=IOINTF_IOERROR;
      return 0;
   }

   if( (aname = DiskIo_mkAbsPath(o, rname, status)) != 0)
   {
      DiskRes* dr = (DiskRes*)baMalloc(sizeof(DiskRes));
      if(dr)
      {
         UINT s;
         if(mode==OpenRes_WRITE)
         {
            fx_file_create(o->mptr, aname);
            s = fx_file_open(o->mptr, &dr->file, aname, FX_OPEN_FOR_WRITE);
         }
         else
         {
            s = fx_file_open(o->mptr, &dr->file, aname, FX_OPEN_FOR_READ_FAST);
         }
         if(!s)
         {
            retVal = (ResIntfPtr)dr; /* Cast to base class */
            ResIntf_constructor(retVal,
                                DiskRes_read,
                                DiskRes_write,
                                DiskRes_seek,
                                DiskRes_flush,
                                DiskRes_close);
            *status=0;
         }
         else
         {
            baFree(dr);
            *status = doError(s, ecode);
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
      baFree(*dirIntf);
      *dirIntf=0;
      return 0;
   }
   return IOINTF_IOERROR;
}


static int
DiskIo_property(IoIntfPtr super,const char* name,void* a,void* b)
{
   UINT s;
   DiskIo* o = (DiskIo*)super;
   if(!o->mptr && strcmp(name, "setmedia"))
      return IOINTF_IOERROR;
   if( ! strcmp(name, "hidden") )
   {
      CHAR* aname;
      if( (aname = DiskIo_mkAbsPath(o, (const char*)a, &s)) != 0)
      {
         UINT attr;
         if( (s=fx_directory_attributes_read(o->mptr, aname, &attr)) == 0)
         {
            attr = *((U32*)b) ? (attr|FX_HIDDEN) : (attr & ~((UINT)FX_HIDDEN));
            s=fx_directory_attributes_set(o->mptr, aname, attr);
            if(s)
               s = doError(s,0);
         }
         else
            s = doError(s,0);
         baFree(aname);
      }
      return s;
   }
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
            *platform="FileX";
         }
         return 0;
      }
   }
   if( ! strcmp(name, "abs") )
   {
      char* path = DiskIo_mkAbsPath(o, (const char*)a, &s);
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
      path = DiskIo_mkAbsPath(o, ptr, &s);
      DiskIo_constructor(di);
      if(path)
      {
         di->mptr = o->mptr;
         s=DiskIo_setRootDir(di, path);
         baFree(path);
         if(s == IOINTF_NOTFOUND)
         {
            /* The path could be an absolute path */
            s=DiskIo_setRootDir(di, (const char*)b);
            if(s)
               s = IOINTF_NOTFOUND; /* Set to original error */
         }
      }
      else
         s=E_MALLOC;
      if(s)
         DiskIo_destructor(di);
      return s;
   }
   if( ! strcmp(name, "destructor") )
   {
      DiskIo_destructor(o);
      return 0;
   }

   /* ThreadX extension. Set FX_MEDIA.
    */
   if( ! strcmp(name, "setmedia") )
   {
      o->mptr = ((FX_MEDIA*)a);
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
   o->rootPath = (char*)defaultRoot;
   o->rootPathLen = strlen(defaultRoot);
}


void
DiskIo_destructor(DiskIo* o)
{
   IoIntfPtr super = (IoIntfPtr)o;
   if(super->onTerminate)
      super->onTerminate(super->attachedIo, super);
   DiskIo_freeRootDir(o);
}


int
DiskIo_setRootDir(DiskIo* o, const char* root)
{
   UINT s;
   if(!o->mptr)
      return IOINTF_IOERROR;

   DiskIo_freeRootDir(o);
   if( !root || (root[0] == '/' && !root[1]) )
   { /* Absolute root path */
      o->rootPath=(char*)defaultRoot;
      o->rootPathLen=strlen(defaultRoot);
      s = 0;
   }
   else if(*root != '/')
      return IOINTF_ENOENT;
   else
   {
      IoStat st;
      o->rootPath = baMalloc(strlen(root)+2);
      if(o->rootPath)
      {
         char* ptr;
         strcpy(o->rootPath, root);
         ptr = strrchr(o->rootPath, '/');
         if(!ptr || (ptr > o->rootPath && ptr[1]))
            strcat(o->rootPath, "/");
         o->rootPathLen = strlen(o->rootPath);
         s = (DiskIo_stat((IoIntfPtr)o, "/", &st) || !st.isDir) ?
            IOINTF_NOTFOUND : 0;
      }
      else
         s = IOINTF_MEM;
      if(s)
         DiskIo_freeRootDir(o);
   }
   return s;
}


int
DiskIo_getRootDir(DiskIo* o, char* buf, int len)
{
   if(len < o->rootPathLen+1)
   {
      return -1;
   }
   strcpy(buf,o->rootPath);
   return strlen(buf);
}
