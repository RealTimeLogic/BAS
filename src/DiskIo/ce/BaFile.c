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
This file contains DiskIo and BaFile implementations for Windows CE.

This implementation sets the root directory to '/' by default.

The root directory can be changed with DiskIo_setRootDir. This function
can be called at any time to change the root directory.
*/

#include <wchar.h>


#ifndef BA_LIB
#define BA_LIB 1
#endif

#include <BaDiskIo.h>



static void
setErrCode(int* status, const char** emsg)
{
   static char* savedEmsg=0;
   DWORD ecode = GetLastError();
   if(status)
   {
      switch(ecode)
      {
         case ERROR_FILE_NOT_FOUND:
            *status = IOINTF_NOTFOUND;
            break;

         case ERROR_PATH_NOT_FOUND:
            *status = IOINTF_ENOENT;
            break;

         case ERROR_NOT_DOS_DISK:
         case ERROR_INVALID_DRIVE:
         case ERROR_INVALID_NAME:
            *status = IOINTF_INVALIDNAME;
            break;

         case ERROR_NOT_ENOUGH_MEMORY:
            *status = IOINTF_MEM;
            break;

         case ERROR_CRC:
         case ERROR_WRITE_FAULT:
         case ERROR_READ_FAULT:
         case ERROR_SEEK:
            *status = IOINTF_IOERROR;
            break;

         case ERROR_HANDLE_DISK_FULL:
         case ERROR_DISK_FULL:
         case ERROR_OPEN_FAILED:
            *status = IOINTF_NOSPACE;
            break;

         case ERROR_FILE_EXISTS:
         case ERROR_ALREADY_EXISTS:
            *status = IOINTF_EXIST;
            break;

         default:
            *status = IOINTF_NOACCESS;
      }
   }
   else
      *status = IOINTF_NOACCESS;
   if(savedEmsg)
   {
      LocalFree(savedEmsg);
      savedEmsg=0;
   }
   if(emsg)
   {
      if( ! FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                       FORMAT_MESSAGE_FROM_SYSTEM,
                       NULL,
                       ecode,
                       0,
                       (LPTSTR)&savedEmsg,
                       0,
                       NULL) == 0)
      {
         *emsg=savedEmsg;
      }
      else
      {
         *emsg=0;
      }
   }
}


static int
removeReadOnlyFlag(WCHAR* wname)
{
   DWORD attr = GetFileAttributesW(wname);
   if(attr != 0xFFFFFFFF && (attr & FILE_ATTRIBUTE_READONLY))
   {
      attr &= ~(FILE_ATTRIBUTE_READONLY);
      if(SetFileAttributesW(wname, attr))
         return 0;
   }
   return -1;
}


static WCHAR* makebacksl(WCHAR* us)
{
  WCHAR* p = us;
  while (*p) {
    if (*p == L'/') *p = L'\\';
    p=CharNext(p);
  }
  return us;
}

static WCHAR* makefwdsl(WCHAR* us)
{
  WCHAR* p = us;
  while (*p) {
    if (*p == L'\\') *p = L'/';
    p=CharNext(p);
  }
  return us;
}


/* convert from the FILETIME 64-bit value representing the number of
 * 100-nanosecond intervals since January 1, 1601 to UNIX time in
 * seconds since 1-Jan-1970.
 */
static BaTime
fileTime2UnixTime(FILETIME* ft)
{
   U64 ll = (U64)ft->dwHighDateTime << 32;
   ll |= ft->dwLowDateTime;
   /*
     A time_t is counted from 1-Jan-1970 in seconds.
     A FILETIME is counted from 1-Jan-1601 in 100-nanoseconds.
     370 (years) * 365.25 (days/year) * 24 (hr/day) * 60 (min/hr) *
     60 (sec/min) * 10,000,000 = 116,444,736,000,000,000
   */
   ll -= 116444736000000000;
   return (BaTime)(ll/10000000); /* Convert from 100-nanoseconds to seconds */
}



/* Convert unicode to UTF-8.
   Space to hold the returned string is obtained from baMalloc().
*/
static char* unicodeToUtf8(const WCHAR* wname, char* name, int* namesz)
{
  int sz = namesz ? *namesz : 0;
  int nByte = WideCharToMultiByte(CP_UTF8, 0, wname, -1, 0, 0, 0, 0);
  if (!sz || nByte > sz) {
    if (name)
      baFree(name);
    name = baMalloc(nByte);
    sz = nByte;
  }
  if (name) {
    int ret = WideCharToMultiByte(CP_UTF8, 0, wname, -1, name, nByte, 0, 0);
    if ( ret == 0 ) {
      baFree(name);
      name = NULL;
      sz = 0;
    }
  }
  if (namesz) *namesz = sz;
  return name;
}

/* the diriter unfortunately does two or more allocates per dir read
   this is not required given that the name member is bound to an upper
   limit of max path characters - see the cFile member
   This routine can be optimized greatly.
*/
typedef struct DirIter
{
    DirIntf super;
    WIN32_FIND_DATAW findData;
    HANDLE  findHandle;
    char* name; int namesz; /* a pair */
    WCHAR* wname;
    int count;
} DirIter;


static int aux_findnext(DirIter* di)
{
  if (di->count++ == 0) {
    di->findData.cFileName[0] = 0;
    di->findHandle = FindFirstFileW(di->wname, &di->findData);
    if (di->findHandle != (HANDLE)-1)
      return 1;
  }
  else {
    di->findData.cFileName[0] = 0;
    if (FindNextFileW(di->findHandle,&di->findData))
      return 1;
  }
  return 0;
}

static int DirIter_read(DirIntfPtr super)
{
  DirIter* di = (DirIter*)super;
  const WCHAR* wn = NULL;

  if (di->count && di->findHandle == (HANDLE)-1)
    return IOINTF_NOTFOUND;
  while (aux_findnext(di)) {
    wn = di->findData.cFileName;
    /* the following cant happen on CE */
    if(wn[0]== L'.' && (!wn[1] || (wn[1]==L'.' && !wn[2])))
      break;
    if (wn && wn[0])
       return 0;
  }
  FindClose(di->findHandle);
  di->findHandle = (HANDLE)-1;
  return IOINTF_NOTFOUND;
}


static const char* DirIter_getName(DirIntfPtr super)
{
   DirIter* o = (DirIter*)super;
   if(o->findHandle == (HANDLE)-1)
      return 0;
   makefwdsl(o->findData.cFileName);
   o->name=unicodeToUtf8(o->findData.cFileName, o->name, &o->namesz);
   return o->name;
}


static int
DirIter_stat(DirIntfPtr super, IoStat* st)
{
   DirIter* o = (DirIter*)super;
   if(o->findHandle == (HANDLE)-1)
      return IOINTF_NOTFOUND;
   st->lastModified = fileTime2UnixTime(&o->findData.ftLastWriteTime);
   st->isDir = o->findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ?
      TRUE : FALSE;
   st->size = o->findData.nFileSizeLow;
   return 0;
}

static void
DirIter_constructor(DirIter* o)
{
   DirIntf_constructor((DirIntf*)o,
                       DirIter_read,
                       DirIter_getName,
                       DirIter_stat);
   o->findHandle = NULL;
   o->name=NULL;
   o->namesz=0;
   o->findData.cFileName[0]=0;
   o->count=0;
   o->wname = NULL;
}


static void
DirIter_destructor(DirIter* o)
{
   if(o->findHandle != (HANDLE)-1)
      FindClose(o->findHandle);
   if(o->name)
      baFree(o->name);
   if(o->wname)
      baFree(o->wname);
   o->namesz=0;
}

typedef struct
{
   ResIntf super;
   HANDLE hndl;
} DiskRes;

static int
DiskRes_read(ResIntfPtr super, void* buf, size_t maxSize, size_t* size)
{
   DiskRes* o = (DiskRes*)super; /* upcast */
   if( ! ReadFile(o->hndl, buf, maxSize, (DWORD*)size, 0) )
   {
      int status;
      setErrCode(&status, 0);
      *size=0;
      return status;
   }
   else if(*size == 0)
      return IOINTF_EOF;
   return 0;
}


static int
DiskRes_write(ResIntfPtr super, const void* buf, size_t size)
{
   DiskRes* o = (DiskRes*)super; /* upcast */
   size_t writeSize;
   if( ! WriteFile(o->hndl, buf, size, (DWORD*)&writeSize, 0) )
   {
      int status;
      setErrCode(&status, 0);
      return status;
   }
   return 0;
}


static int
DiskRes_seek(ResIntfPtr super, BaFileSize offset)
{
   DiskRes* o = (DiskRes*)super; /* upcast */
   if(SetFilePointer(o->hndl,(LONG)offset,NULL,FILE_BEGIN) == 0xFFFFFFFF)
   {
      int status;
      setErrCode(&status, 0);
      return status;
   }
   return 0;
}


static int
DiskRes_flush(ResIntfPtr super)
{
   DiskRes* o = (DiskRes*)super; /* upcast */
   if( ! FlushFileBuffers(o->hndl) )
   {
      int status;
      setErrCode(&status, 0);
      return status;
   }
   return 0;
}


static int
DiskRes_close(ResIntfPtr super)
{
   HANDLE hndl;
   baAssert(super);
   if(!super) return -1;
   hndl = ((DiskRes*)super)->hndl;
   baFree(super);
   if( ! CloseHandle(hndl) )
   {
      int status;
      setErrCode(&status, 0);
      return status;
   }
   return 0;
}


static void
DiskRes_constructor(DiskRes* o, HANDLE hndl)
{
	ResIntf_constructor((ResIntf*)o,
                       DiskRes_read,
                       DiskRes_write,
                       DiskRes_seek,
                       DiskRes_flush,
                       DiskRes_close);
   o->hndl=hndl;
}



/* Make absolute path from 'rootPath' and 'name'.
   Converts 'name', which must be a UTF-8 string, to wide-character
   (Unicode) string.
   Unicode strings will have all "//" replaced with "\\"
   Space to hold the returned string is obtained from baMalloc.
*/
static WCHAR*
DiskIo_mkAbsPath(DiskIo* o, const char* name, int extraChars, int* status)
{
   int len;
   WCHAR* wptr;

   /* The char \ is not seen as a dir sep by BA, thus a security issue */ 
   if(strchr(name, '\\'))
   {
      *status=IOINTF_INVALIDNAME;
      return 0;
   }

  if ( ! o->rootPath ) {
	  o->rootPath = baMalloc(2*sizeof(WCHAR));
	  o->rootPathLen = 1;
	  wcscpy(o->rootPath, L"\\");
  }

  if (*name == '/' || *name == '\\') name++;

  len = o->rootPathLen+strlen(name)+1;
  wptr = baMalloc((len+1+extraChars)*sizeof(WCHAR));
  if (wptr) {
    wcscpy(wptr, o->rootPath);
    MultiByteToWideChar(CP_UTF8, 0, name, -1, wptr+o->rootPathLen, 
		len - o->rootPathLen);
    makebacksl(wptr);
  }
  else
     *status=E_MALLOC;
  return wptr;
}


static DirIntfPtr
DiskIo_openDir(IoIntfPtr super, const char* dirname,
               int* status, const char** ecode)
{
   DiskIo* o = (DiskIo*)super;
   WCHAR* wname;

   if(ecode) *ecode=0;

   wname = DiskIo_mkAbsPath(o, dirname, 5, status);
   if(wname)
   {
      DirIter* di = (DirIter*)baMalloc(sizeof(DirIter));
      if(di)
      {
         int len = wcslen(wname)-1;
         DWORD attr = GetFileAttributesW(wname);
         if (attr != 0xFFFFFFFF && (attr & FILE_ATTRIBUTE_DIRECTORY)) {
           if(len >= 0 && wname[len] == L'\\')
              wcscat(wname, L"*.*");
           else
              wcscat(wname, L"\\*.*");
           DirIter_constructor(di);
           di->wname = wname;
           return (DirIntfPtr)di;
         }
      }
      else
         *status=IOINTF_MEM;
      baFree(wname);
      return (DirIntfPtr)di;
   }
   return 0;
}


static int
DiskIo_mkDir(IoIntfPtr super, const char* name, const char** ecode)
{
   DiskIo* o = (DiskIo*)super;
   WCHAR* wname;
   int status;

   if(ecode) *ecode=0;

   if( (wname = DiskIo_mkAbsPath(o, name, 0, &status)) != 0)
   {
      if(!CreateDirectoryW(wname, NULL))
      {
         setErrCode(&status, ecode);
      }
      else
         status=0;
      baFree(wname);
   }
   return status;
}


static int
DiskIo_rename(
   IoIntfPtr super, const char* rfrom, const char* rto, const char** ecode)
{
   DiskIo* o = (DiskIo*)super;
   WCHAR* afrom;
   WCHAR* ato;
   int status;

   if(ecode) *ecode=0;

   if( (ato = DiskIo_mkAbsPath(o, rto, 0, &status)) != 0)
   {
      if( (afrom = DiskIo_mkAbsPath(o, rfrom, 0, &status)) != 0)
      {
         if( MoveFileW(afrom, ato) )
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
DiskIo_remove(IoIntfPtr super,const char* name, const char** ecode)
{
   DiskIo* o = (DiskIo*)super;
   WCHAR* wname;
   int status;

   if(ecode) *ecode=0;

   if( (wname = DiskIo_mkAbsPath(o, name, 0, &status)) != 0)
   {
      if(!DeleteFileW(wname))  /* probably should be CreateFile */
         setErrCode(&status, ecode);
      else
         status=0;
      baFree(wname);
   }
   return status;
}


static int
DiskIo_rmDir(IoIntfPtr super, const char* name, const char** ecode)
{
   DiskIo* o = (DiskIo*)super;
   WCHAR* wname;
   int status;

   if(ecode) *ecode=0;

   if( (wname = DiskIo_mkAbsPath(o, name, 0, &status)) != 0)
   {
      if(!RemoveDirectoryW(wname))
      {
         setErrCode(&status, ecode);
      }
      else
         status=0;
      baFree(wname);
   }
   return status;
}


static int
DiskIo_stat(IoIntfPtr super, const char* name, IoStat* st)
{
   DiskIo* o = (DiskIo*)super;
   WCHAR* wname;
   int status;
   int len;
   WIN32_FILE_ATTRIBUTE_DATA wfad;

   if(*name == '/') name++;

   if( ! o->rootPath && !*name)
   {
      st->isDir = TRUE;
      st->lastModified = 0;
      st->size = 0;
      return 0;
   }

   len = strlen(name);
   wname = DiskIo_mkAbsPath(o, name, 2, &status);
   if( ! wname )
      return status;
   len = wcslen(wname);
   if (len > 1 && wname[len-1] == L'\\')
      wname[len-1]=0;

   if (GetFileAttributesExW(wname, GetFileExInfoStandard, &wfad))
   {
      st->isDir = (wfad.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY) ? 1 : 0;
      st->lastModified = fileTime2UnixTime(&wfad.ftLastWriteTime);
      st->size = wfad.nFileSizeLow;
      status=0;
   }
   else
      status=IOINTF_NOTFOUND;
   baFree(wname);
   return status;
}


static ResIntfPtr
DiskIo_openRes(IoIntfPtr super, const char* name, U32 mode,
               int* status, const char** ecode)
{
   WCHAR* wname;
   DiskIo* o = (DiskIo*)super; /* Upcast */
   ResIntfPtr retVal=0;
   DWORD access;
   DWORD shareMode;
   DWORD creationDispostion;

   if(ecode) *ecode=0;

   if(mode == OpenRes_READ)
   {
      access = GENERIC_READ;
      shareMode = FILE_SHARE_READ;
      creationDispostion = OPEN_EXISTING;
   }
   else if(mode == OpenRes_WRITE)
   {
      access = GENERIC_WRITE;
      shareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;
      creationDispostion = CREATE_ALWAYS;
   }
   else
   {
      *status = IOINTF_NOIMPLEMENTATION;
      if(ecode) *ecode = "Unknown mode";
      return 0;
   }

   if( (wname = DiskIo_mkAbsPath(o, name, 0, status)) != 0)
   {
      DiskRes* dr = (DiskRes*)baMalloc(sizeof(DiskRes));
      if(dr)
      {
         HANDLE hndl;
         BaBool failedOnce;
         failedOnce=FALSE;
        L_retry:
         hndl = CreateFileW(
            wname,
            access,
            shareMode,
            NULL,
            creationDispostion,
            FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS,
            NULL);
         if(hndl == (HANDLE)-1)
         {
            if(! failedOnce && ! removeReadOnlyFlag(wname) )
            {
               failedOnce=TRUE;
               goto L_retry;
            }
            setErrCode(status, ecode);
            baFree(dr);
         }
         else
         {
            *status=0;
            DiskRes_constructor(dr, hndl);
            retVal = (ResIntfPtr)dr; /* Downcast */
         }
      }
      else
         *status = IOINTF_MEM;
      baFree(wname);
   }
   return retVal;
}


static int
DiskIo_closeDir(IoIntfPtr super, DirIntfPtr* dirIntf)
{
   DiskIo* o = (DiskIo*)super;
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
   if ( ! strcmp(name, "movedir") )
   {
      *((U32*)a) = TRUE; /* Windows file system can move directories */
      return 0;
   }
   if ( ! strcmp(name, "hidden") )
   {
      WCHAR* wname;
      if( (wname = DiskIo_mkAbsPath(o, (const char*)a, 0, &status)) != 0)
      {
         DWORD attr = GetFileAttributesW(wname);
         status=-1;
         if(attr != 0xFFFFFFFF)
         {
            attr = *((U32*)b) ?
               (attr | FILE_ATTRIBUTE_HIDDEN) :
               (attr & ~((U32)FILE_ATTRIBUTE_HIDDEN));
            if(SetFileAttributesW(wname, attr))
               status = 0;
         }
         baFree(wname);
      }
      return status;
   }
   if ( ! strcmp(name, "abs") )
   {
      WCHAR* wname = DiskIo_mkAbsPath(o, (const char*)a, 0, &status);
      if(wname)
      {
         char* name;
         makefwdsl(wname);
         name = unicodeToUtf8(wname,NULL,0);
         baFree(wname);
         if(name)
         {
            *((char**)b) = name;
            return 0;
         }
         status = IOINTF_MEM;
      }
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
            *platform="wince";
         }
         return 0;
      }
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
      WCHAR* wname;
      DiskIo* di = (DiskIo*)a;
      const char* ptr = (const char*)b;
      if(!ptr) ptr="";
      wname = DiskIo_mkAbsPath(o, ptr, 0, &status);
      DiskIo_constructor(di);
      if(wname)
      {
         char* name = unicodeToUtf8(wname,NULL,0);
         baFree(wname);
         if(name)
         {
            status=DiskIo_setRootDir(di, name);
            baFree(name);
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
      }
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


BA_API void DiskIo_constructor(DiskIo* o)
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


BA_API void DiskIo_destructor(DiskIo* o)
{
   IoIntfPtr super = (IoIntfPtr)o;
   if(super->onTerminate)
      super->onTerminate(super->attachedIo, super);
   if(o->rootPath)
      baFree(o->rootPath);
}


static WCHAR* elideDotDotW(WCHAR* wdir)
{
   /* Normalize path i.e. elide all .. */
   size_t len;
   WCHAR* ptr1;
   WCHAR* ptr2;
   unsigned i;
   for(i = 0 ; i < wcslen(wdir) ; i++)
      if(wdir[i] == L'\\') wdir[i]=L'/';
   while( ( ptr1 = wcsstr( wdir, L"/../" ) ) != 0 )
   {
      for( ptr2 = ptr1 - 1; ptr2 >= wdir && *ptr2 != L'/'; --ptr2 )
         continue;
      if( ptr2 < wdir )
         break;
      wcscpy( ptr2, ptr1 + 3 );
   }
   while( (len = wcslen( wdir )) > 3 &&
          wcscmp((ptr1 = wdir + len - 3), L"/.." ) == 0 )
   {
      for(ptr2 = ptr1 - 1; ptr2 >= wdir && *ptr2 != L'/'; --ptr2)
         continue;
      if(ptr2 < wdir)
         break;
      *ptr2 = 0;
   }
   return wdir;
}


BA_API int DiskIo_setRootDir(DiskIo* o, const char* root)
{
  int status;
  int len;
  WCHAR* wdir;
  WCHAR* wcurDir=0;

  /* If root set to '/'. A null value is the same as '/' */
  if (!root || ((root[0] == '/' || root[0] == '\\') && !root[1])) {
    if (o->rootPath)
      baFree(o->rootPath);
    o->rootPath = baMalloc(2 * sizeof(WCHAR));
    wcscpy(o->rootPath, L"\\");
    o->rootPathLen=1;
    return 0;
  }


  if (root[0] != '/' || root[0] != '\\')  /* If not an absolute path */
      wcurDir = L"/";

  len = (strlen(root)+2)*sizeof(WCHAR) +
      (wcurDir ? wcslen(wcurDir)*sizeof(WCHAR) : 0) ;
   wdir = baMalloc(len);
   if(wdir)
   {
      DWORD ftype;
      WCHAR* wname;
      if(wcurDir && wcurDir[0] != '/')
      {
         wcscpy(wdir, wcurDir);
         wname = wdir+wcslen(wcurDir);
      }
      else
         wname = wdir;
      MultiByteToWideChar(CP_UTF8, 0, root, -1, wname, len-1);
      if(wcurDir)
         wname=elideDotDotW(wdir);

      makebacksl(wdir);
      if( (ftype = GetFileAttributesW(wname)) == 0xFFFFFFFF
           || ! (ftype & FILE_ATTRIBUTE_DIRECTORY) )
      {
         if(wname[wcslen(wname)-1] == L'\\')
         {
            wname[wcslen(wname)-1] = 0;
            if( (ftype = GetFileAttributesW(wname)) != 0xFFFFFFFF
                &&  (ftype & FILE_ATTRIBUTE_DIRECTORY) )
               goto L_ok;
         }
         baFree(wname);
         status=IOINTF_NOTFOUND;
      }
      else
      {
L_ok:
         if(wname[wcslen(wname)-1] != L'\\')
            wcscat(wname, L"\\");
         if(o->rootPath)
            baFree(o->rootPath);
         elideDotDotW(wname);
         o->rootPath = wname;
         o->rootPathLen=wcslen(wname);
         status=0;
      }
   }
   else
      status = IOINTF_MEM;

   return status;
}

BA_API int DiskIo_getRootDir(DiskIo* o, char* buf, int len)
{
   int plen;
   char* path;
   int retVal=-1;
   if(len < o->rootPathLen || len < 2)
   {
      return -1;
   }
   if(!o->rootPath)
   {
      strcpy(buf, "/");
      return 1;
   }
   path = unicodeToUtf8(o->rootPath, NULL, 0);
   if(path)
   {
      plen = strlen(path);
      if(plen < len)
      {
         strcpy(buf,path);
         retVal=plen;
      }
      baFree(path);
   }
   return retVal;
}



/****************************************************************************
                                   BaFile functions
****************************************************************************/

#include <BaFile.h>


static DiskIo io = {
   {0,0,0,0,0,0,0,0,0,0},
   L"",
   0};

BA_API DirIntfPtr baOpenDir(const char* dirname, int* status, const char** ecode)
{
   return DiskIo_openDir((IoIntfPtr)&io, dirname, status, ecode);
}


BA_API int baCloseDir(DirIntfPtr* dirIntf)
{
   return DiskIo_closeDir((IoIntfPtr)&io, dirIntf);
}


BA_API int baStat(const char* name, IoStat* st)
{
   return DiskIo_stat((IoIntfPtr)&io, name, st);
}


BA_API ResIntfPtr baOpenRes(const char* name,U32 mode,int* status,const char** ecode)
{
   return DiskIo_openRes((IoIntfPtr)&io, name, mode, status, ecode);
}


BA_API int baCloseRes(ResIntfPtr* fp)
{
   int status=DiskRes_close(*fp);
   *fp=0;
   return status;
}


BA_API int baMkDir(const char* name, const char** ecode)
{
   return DiskIo_mkDir((IoIntfPtr)&io, name, ecode);
}


BA_API int baRmdDir(const char* name, const char** ecode)
{
   return DiskIo_rmDir((IoIntfPtr)&io, name, ecode);
}


BA_API int baRemove(const char* name, const char** ecode)
{
   return DiskIo_remove((IoIntfPtr)&io, name, ecode);
}


BA_API int baRename(const char* from, const char* to, const char** ecode)
{
   return DiskIo_rename((IoIntfPtr)&io, from, to, ecode);
}
