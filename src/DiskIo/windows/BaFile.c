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
 *   COPYRIGHT:  Real Time Logic, 2006 - 2020
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
This file contains DiskIo and BaFile implementations for Windows NT and better.

This implementation sets the root directory to '/' by default.

The root directory is implemented as a virtual directory with the
standard Windows disks such as a,c, etc. as sub-directories. As an
example, the root of C: is /c/.

The root directory can be change with DiskIo_setRootDir. This function
can be called at any time to change the root directory.

Example of valid root directories:
  '/'		Creates a special virtual directory, where A:, C:, E:,
		etc are sub-directories under '/'.
  c:		Set the root to C:\
  a:		Set the root to A:\
  c:/webserver	Set the root to C:\webserver

The Windows DiskIo implementation is complex and not suited as a
reference when creating a new DiskIo implementation. The Posix
implementation is much easier to understand.

The Windows DiskIo implementation is complex for a number of reasons:

*  The virtual root directory creates added complexity.

*  The inconsistency in the Windows I/O functions makes the DiskIo
   implementation complex.

*  Many operating systems store file names as UTF8 and leave the
   rendering to the GUI. Windows require that non ANSI characters are
   stored as wide characters.
*/
#ifndef BA_LIB
#define BA_LIB
#endif
#include <TargConfig.h>
#include <wchar.h>
#undef ROOTPT
#define ROOTPT WCHAR*

#include <BaDiskIo.h>

#include <errno.h>
#include <sys/stat.h>


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
static char*
unicodeToUtf8(const WCHAR* wname)
{
  int nByte;
  char *name;

  nByte = WideCharToMultiByte(CP_UTF8, 0, wname, -1, 0, 0, 0, 0);
  name = baMalloc(nByte);
  if(name)
  {
     nByte = WideCharToMultiByte(CP_UTF8, 0, wname, -1, name, nByte, 0, 0);
     if( nByte == 0 )
     {
        baFree(name);
        name = 0;
     }
  }
  return name;
}

static WCHAR*
unix2DosPath(WCHAR* s)
{
   WCHAR* ptr;
   for(ptr = s; *ptr; ptr++)
   {
      if(*ptr == L'/')
         *ptr=L'\\';
   }
   return s;
}



typedef struct DriveIter
{
      DirIntf super;

      DWORD driveMask;
      char curDrive;
      char driveName[2];
}DriveIter;

static int
DriveIter_read(DirIntfPtr super)
{
   DriveIter* o = (DriveIter*)super;

   while(o->driveMask)
   {
      BaBool isDrive;
      o->curDrive++;
      isDrive = o->driveMask & 1 ? TRUE : FALSE;
      o->driveMask  >>= 1;
      if(isDrive) {
         return 0;
      }
   }
   return IOINTF_NOTFOUND;
}


static const
char* DriveIter_getName(DirIntfPtr super)
{
   DriveIter* o = (DriveIter*)super;
   o->driveName[0] = 'A'+o->curDrive;
   return o->driveName;
}


static int
DriveIter_stat(DirIntfPtr super, IoStat* st)
{
   (void)super;
   memset(st, 0, sizeof(IoStat));
   st->isDir=TRUE;
   return 0;
}

void
DriveIter_constructor(DriveIter* o)
{
   DirIntf_constructor((DirIntf*)o,
                       DriveIter_read,
                       DriveIter_getName,
                       DriveIter_stat);
   o->curDrive=-1;
   o->driveName[1]=0;
   o->driveMask = GetLogicalDrives();
}

#define DriveIter_destructor(o)





typedef struct DirIter
{
      DirIntf super;

      WIN32_FIND_DATAW findData;
      HANDLE  findHandle;
      BaBool isFirstTime;
      char* name;
}DirIter;


static int
DirIter_read(DirIntfPtr super)
{
   DirIter* o = (DirIter*)super;
   const WCHAR* wn = o->findData.cFileName;
   if(o->findHandle == (HANDLE)-1)
      return IOINTF_NOTFOUND;
   if(o->isFirstTime)
   {
      o->isFirstTime=FALSE;
      if( ! (wn[0]== L'.' && (!wn[1] || (wn[1]==L'.' && !wn[2]))) )
         return 0;
   }
   for(;;)
   {
      if(FindNextFileW(o->findHandle,&o->findData))
      {
         wn = o->findData.cFileName;
         if(wn[0]== L'.' && (!wn[1] || (wn[1]==L'.' && !wn[2])))
            continue;
         return 0;
      }
      else
         break;
   }
   FindClose(o->findHandle);
   o->findHandle = (HANDLE)-1;
   return IOINTF_NOTFOUND;
}


static const char*
DirIter_getName(DirIntfPtr super)
{
   DirIter* o = (DirIter*)super;
   if(o->findHandle == (HANDLE)-1)
      return 0;
   if(o->name)
      baFree(o->name);
   o->name=unicodeToUtf8(o->findData.cFileName);
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
#ifdef BA_FILESIZE64
   st->size |= ((BaFileSize)o->findData.nFileSizeHigh) << 32;
#endif
   return 0;
}

static void
DirIter_constructor(DirIter* o)
{
   DirIntf_constructor((DirIntf*)o,
                       DirIter_read,
                       DirIter_getName,
                       DirIter_stat);
   o->isFirstTime=TRUE;
   o->name=0;
}


static void
DirIter_destructor(DirIter* o)
{
   if(o->findHandle != (HANDLE)-1)
      FindClose(o->findHandle);
   if(o->name)
      baFree(o->name);
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
   DWORD s;
   if( ! ReadFile(o->hndl, buf, (DWORD)maxSize, &s, 0) )
   {
      int status;
      setErrCode(&status, 0);
      *size=0;
      return status;
   }
   *size=(size_t)s;
   if(s == 0)
      return IOINTF_EOF;
   return 0;
}


static int
DiskRes_write(ResIntfPtr super, const void* buf, size_t size)
{
   DiskRes* o = (DiskRes*)super; /* upcast */
   DWORD writeSize;
   if( ! WriteFile(o->hndl, buf, (DWORD)size, &writeSize, 0) )
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
#ifdef BA_FILESIZE64
   LONG offsHigh = (LONG)(offset >> 32);
   if(SetFilePointer(o->hndl,(DWORD)offset,&offsHigh,FILE_BEGIN) == 0xFFFFFFFF)
#else
   if(SetFilePointer(o->hndl,(DWORD)offset,NULL,FILE_BEGIN) == 0xFFFFFFFF)
#endif
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
   Space to hold the rereturned string is obtained from baMalloc.
*/
static WCHAR*
DiskIo_mkAbsPath(DiskIo* o, const char* name, int extraChars, int* status)
{
   size_t len;
   WCHAR* wptr;
   if(*name == '/') name++;
   if( ! o->rootPath )
   {  /* If not using an offset start path i.e. if reading from '/',
       * where '/' contains the drives on the PC.
       */
      const char* ptr;

       /* The char \ is not seen as a dir sep by BA, thus a security issue */ 
      if(strchr(name, '\\') ||
         /* Must have at minimum the drive i.e. 'c' */
         !name[0] || (name[1] && name[1] != '/') )
      {
         *status=IOINTF_INVALIDNAME;
         return 0;
      }
      if(name[1])
         ptr=name+1;
      else
         ptr="";
      len = 3 + strlen(ptr);
      wptr = baMalloc((len+extraChars)*sizeof(WCHAR));
      if(wptr)
      {
         wptr[0]=name[0]; /* Set drive */
         wptr[1]=L':';
         wptr[2]=0;
         MultiByteToWideChar(CP_UTF8, 0, ptr, -1, wptr+2, (int)len-2);
         return unix2DosPath(wptr);
      }
      *status=IOINTF_MEM;
      return 0;
   }
   /* If using an offset start path */
   len = strlen(name)+1;
   wptr = baMalloc((o->rootPathLen+len+extraChars)*sizeof(WCHAR));
   if(wptr)
   {
      int wlen;
      wcscpy(wptr, o->rootPath);
      wlen=MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                               name,-1,wptr+o->rootPathLen,(int)len);
      if(wlen > 0 && ! wcsstr(wptr+o->rootPathLen, L"..")) /* Security  check */
      {
         return unix2DosPath(wptr);
      }
      baFree(wptr);
      /* The server removes all .. by using baElideDotDot. A
         .. sequence after converting UTF8 to wide char indicates a
         security penetration attempt.
      */ 
      *status=IOINTF_INVALIDNAME;
      return 0;
   }
   *status=E_MALLOC;
   return 0;
}



static DirIntfPtr
DiskIo_openDir(IoIntfPtr super, const char* dirname,
               int* status, const char** ecode)
{
   DiskIo* o = (DiskIo*)super;
   WCHAR* wname;

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
      wname = DiskIo_mkAbsPath(o, dirname, 4, status);
      if(wname)
      {
         DirIter* di = (DirIter*)baMalloc(sizeof(DirIter));
         if(di)
         {
            size_t len = wcslen(wname)-1;
            if(len > 0 && wname[len] == L'\\')
               wcscat(wname, L"*.*");
            else
               wcscat(wname, L"\\*.*");
            di->findHandle = FindFirstFileW(wname, &di->findData);
            if(di->findHandle == (HANDLE)-1)
            {
               setErrCode(status, ecode);
               baFree(di);
               di=0;
            }
            else
               DirIter_constructor(di);
         }
         else
            *status=IOINTF_MEM;
         baFree(wname);
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
   size_t len;
   BaBool endsWithSlash;
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
   endsWithSlash = (!len || name[len-1] == '/') ? TRUE : FALSE;
   wname = DiskIo_mkAbsPath(o, name, 1, &status);
   if( ! wname )
      return status;
   len = wcslen(wname);
   if(len < 3)
      wcscat(wname, L"\\");
   else if(endsWithSlash && len != 3)
      wname[len-1]=0;

   if (GetFileAttributesExW(wname, GetFileExInfoStandard, &wfad))
   {
       st->isDir = (wfad.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY) ? 1 : 0;
      st->lastModified = fileTime2UnixTime(&wfad.ftLastWriteTime);
      st->size = wfad.nFileSizeLow;
#ifdef BA_FILESIZE64
      st->size |= ((BaFileSize)wfad.nFileSizeHigh) << 32;
#endif
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
   DWORD access=0;
   DWORD shareMode;
   DWORD creationDispostion=0;
   if(ecode) *ecode=0;
   if(mode & OpenRes_READ)
   {
      access |= GENERIC_READ;
      creationDispostion = OPEN_EXISTING;
   }
   if(mode & OpenRes_APPEND)
   {
      access |= FILE_APPEND_DATA;
      creationDispostion = OPEN_ALWAYS;
   }
   else if(mode & OpenRes_WRITE)
   {
      access |= GENERIC_WRITE;
      creationDispostion = CREATE_ALWAYS;
   }
   shareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;
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
   (void)super;
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
   int status;
   DiskIo* o = (DiskIo*)super;
   if( ! strcmp(name, "movedir") )
   {
      *((U32*)a) = TRUE; /* Windows file system can move directories */
      return 0;
   }
   if( ! strcmp(name, "hidden") )
   {
      WCHAR* wname;
      int status;
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
   if( ! strcmp(name, "type") )
   {
      if(a)
      {
         const char** type = (const char**)a;
         *type = "disk";
         if(b)
         {
            const char** platform = (const char**)b;
            *platform="windows";
         }
         return 0;
      }
   }
   if( ! strcmp(name, "abs") )
   {
      WCHAR* wname = DiskIo_mkAbsPath(o, (const char*)a, 0, &status);
      if(wname)
      {
         char* name = unicodeToUtf8(wname);
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
         char* name = unicodeToUtf8(wname);
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
   {
      baFree(o->rootPath);
      o->rootPath=0;
   }
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
   size_t len;
   WCHAR* wdir;
   WCHAR* wcurDir=0;

   /* If root set to '/'. A null value is the same as '/' */
   if(!root || (root[0] == '/' && !root[1]))
   {
      if(o->rootPath)
         baFree(o->rootPath);
      o->rootPath=0;
      o->rootPathLen=1;
      return 0;
   }


   if(root[0] != '/' && root[1] != ':') /* If not an absolute path */
   {
      DWORD len = GetCurrentDirectoryW(0, 0);
      if(len == 0)
         return IOINTF_IOERROR;
      wcurDir = baMalloc((len+1)*sizeof(WCHAR));
      if(!wcurDir)
         return IOINTF_MEM;
      if( ! GetCurrentDirectoryW(len,wcurDir) )
      {
         baFree(wcurDir);
         return IOINTF_IOERROR;
      }
      wcscat(wcurDir, L"/");
   }


   len = (strlen(root)+2)*sizeof(WCHAR) +
      (wcurDir ? wcslen(wcurDir)*sizeof(WCHAR) : 0) ;
   wdir = baMalloc(len);
   if(wdir)
   {
      DWORD ftype;
      WCHAR* wname;
      if(wcurDir)
      {
         wcscpy(wdir, wcurDir);
         wname = wdir+wcslen(wcurDir);
      }
      else
         wname = wdir;
      MultiByteToWideChar(CP_UTF8, 0, root, -1, wname, (int)len-1);
      if(wcurDir)
         wname=elideDotDotW(wdir);
      else if(strlen(root) == 2 && root[1] == ':')
         wcscat(wname, L"/");

      if(wname[0] == L'/' && wname[2] == L'/')
      {
         //Translate path of form /c/mydir to c:/mydir
         wname[0] = wname[1];
         wname[1]=L':';
      }

      if( (ftype = GetFileAttributesW(wname)) == 0xffffffff
                || ! (ftype & FILE_ATTRIBUTE_DIRECTORY) )
      {
         if(wname[wcslen(wname)-1] == L'/')
         {
            wname[wcslen(wname)-1] = 0;
            if( (ftype = GetFileAttributesW(wname)) != 0xffffffff
                &&  (ftype & FILE_ATTRIBUTE_DIRECTORY) )
               goto L_ok;
         }
         baFree(wname);
         status=IOINTF_NOTFOUND;
      }
      else
      {
L_ok:
         if(wname[wcslen(wname)-1] != L'/')
            wcscat(wname, L"/");
         if(o->rootPath)
            baFree(o->rootPath);
         elideDotDotW(wname);
         o->rootPath = wname;
         o->rootPathLen=(int)wcslen(wname);
         status=0;
      }
   }
   else
      status = IOINTF_MEM;

   if(wcurDir)
      baFree(wcurDir);
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
   path = unicodeToUtf8(o->rootPath);
   if(path)
   {
      plen = iStrlen(path);
      if(plen < len)
      {
         if(path[1] == ':')
         {
            path[1] = path[0];
            path[0]='/';
         }
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
