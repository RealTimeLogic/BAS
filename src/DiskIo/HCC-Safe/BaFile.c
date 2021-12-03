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
 *   $I$
 *
 *   COPYRIGHT:  Real Time Logic, 2012
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
This file contains DiskIo and BaFile implementations for the HCC SAFE
File system.

This implementation sets the root directory to '/' by default.  The
root directory can be change with DiskIo_setRootDir, which can be
called at any time to change the root directory.

Please see the BaDiskIo.h, ResIntf.h, and BaFile.h documentation in the
reference manual for information on the API's that are implemented in
this file.
*/

#define ROOTPT W_CHAR*
#include <udefs_s.h>
#include <fsm.h>
#include <fsmf.h>
#include <fsf.h>
#include <fwerr.h>
#include <BaDiskIo.h>
#include <BaFile.h>
#include <BaServerLib.h>


#if FS_SEPARATORCHAR == '/'
#define unix2DosPath(x) x
#else
static W_CHAR*
unix2DosPath(W_CHAR* s)
{
   if(s)
   {
      W_CHAR* ptr;
      for(ptr = s; *ptr; ptr++)
      {
         if(*ptr == '/')
            *ptr='\\';
      }
   }
   return s;
}
#endif

static BaTime
fileTime2UnixTime(unsigned short cdate, unsigned short ctime)
{
   struct BaTm t;

   memset(&t,0,sizeof(t));
   t.tm_hour = (ctime & FS_CTIME_HOUR_MASK) >> FS_CTIME_HOUR_SHIFT;
   t.tm_min = (ctime & FS_CTIME_MIN_MASK) >> FS_CTIME_MIN_SHIFT;
   t.tm_sec = (ctime & FS_CTIME_SEC_MASK) >> FS_CTIME_SEC_SHIFT;
   t.tm_year = 1980+((cdate & FS_CDATE_YEAR_MASK) >>  FS_CDATE_YEAR_SHIFT);
   t.tm_mon = (cdate & FS_CDATE_MONTH_MASK) >> FS_CDATE_MONTH_SHIFT;
   t.tm_mday = (cdate & FS_CDATE_DAY_MASK) >> FS_CDATE_DAY_SHIFT;
	 t.tm_year -= 1900; /* baTm2Time requires years since 1900 */
	 t.tm_mon -= 1; /* baTm2Time requires month 0-11 */
	 return ( baTm2Time(&t) );
}


#ifdef HCC_UNICODE
#define BA_FIND FS_WFIND
#define ba_findfirst fsm_wfindfirst
#define ba_findnext fsm_wfindnext
#define ba_mkdir fsm_wmkdir
#define ba_move fsm_wmove
#define ba_delete fsm_wdelete
#define ba_rmdir fsm_wrmdir
#define ba_open fsm_wopen
#define ba_stat fsm_wstat

static void
ba_strcpy(W_CHAR* dst, W_CHAR* src)
{
   while(*src)
      *dst++=*src++;
   *dst=0;
}

static int
ba_strlen(W_CHAR* str)
{
   W_CHAR* ptr=str;
   while(*ptr) ptr++;
   return ptr-str;
}

static int
utf8ToWChar(W_CHAR* wptr, const U8* ptr)
{
   while(*ptr)
   {
      U8 c = ptr[0];
      unsigned long uc = 0; /* UTF 32 */
      if (c < 0xc0)
         uc = c;
      else if (c < 0xe0)
      {
         if ((ptr[1] & 0xc0) == 0x80)
         {
            uc = ((c & 0x1f) << 6) | (ptr[1] & 0x3f);
            ++ptr;
         }
         else
            uc = c;
      }
      else if (c < 0xf0)
      {
         if ((ptr[1] & 0xc0) == 0x80 &&
             (ptr[2] & 0xc0) == 0x80)
         {
            uc = ((c & 0x0f) << 12) |
               ((ptr[1] & 0x3f) << 6) | (ptr[2] & 0x3f);
            ptr += 2;
         }
         else
            uc = c;
      }
      else if (c < 0xf8)
      {
         if ((ptr[1] & 0xc0) == 0x80 &&
             (ptr[2] & 0xc0) == 0x80 &&
             (ptr[3] & 0xc0) == 0x80)
         {
            uc = ((c & 0x03) << 18) |
               ((ptr[1] & 0x3f) << 12) |
               ((ptr[2] & 0x3f) << 6) |
               (ptr[4] & 0x3f);
            ptr += 3;
         }
         else
            uc = c;
      }
      else if (c < 0xfc)
      {
         if ((ptr[1] & 0xc0) == 0x80 &&
             (ptr[2] & 0xc0) == 0x80 &&
             (ptr[3] & 0xc0) == 0x80 &&
             (ptr[4] & 0xc0) == 0x80)
         {
            uc = ((c & 0x01) << 24) |
               ((ptr[1] & 0x3f) << 18) |
               ((ptr[2] & 0x3f) << 12) |
               ((ptr[4] & 0x3f) << 6) |
               (ptr[5] & 0x3f);
            ptr += 4;
         }
         else
            uc = c;
      }
      else
         ++ptr; /* ignore */
      if (uc >= 0x10000)
         return -1;
      *wptr++=(W_CHAR)uc;
      ++ptr;
   }
   *wptr=0;
   return 0;
}

static int
wCharToUtf8(U8* ptr, int ptrLen,const W_CHAR* wptr)
{
   int len=0;
   while(*wptr)
   {
      unsigned int uc = *wptr;
      if (uc < 0x80)
      {
         if(len >= ptrLen)
            return -1;
         *ptr++ = (U8)uc;
         len++;
      }
      else if(uc < 0x800)
      {
         if(len+1 >= ptrLen)
            return -1;
         *ptr++=(U8)(0xc0 | (uc >> 6));
         *ptr++=(U8)(0x80 | (uc & 0x3f));
         len+=2;
      }
      else
      {
         if(len+2 >= ptrLen)
            return -1;
         *ptr++ = (U8)(0xe0 | (uc >> 12));
         *ptr++ = (U8)(0x80 | ((uc >> 6) & 0x3f));
         *ptr++ = (U8)(0x80 | (uc & 0x3f));
         len+=3;
      }
      ++wptr;
   }
   *ptr=0;
   return len;
}

#else

/* non unicode */

#define BA_FIND FS_FIND
#define ba_findfirst fsm_findfirst
#define ba_findnext fsm_findnext
#define ba_mkdir fsm_mkdir
#define ba_move fsm_move
#define ba_delete fsm_delete
#define ba_rmdir fsm_rmdir
#define ba_open fsm_open
#define ba_stat fsm_stat

#define ba_strcpy strcpy
#define ba_strlen strlen

#endif


static int
ba_xstat(W_CHAR* name, FS_STAT *st)
{
   int status=ba_stat(name,st);
   if(status && name[1] == ':' && name[2] == 0)
   {
      memset(st,0,sizeof(st));
      st->secure=FSSEC_ATTR_DIR;
      return 0;
   }
   return status;
}


static int
toBaErr(int err)
{
   switch(err)
   {
      case F_ERR_INVALIDDIR:
      case F_ERR_INVALIDNAME:
      case F_ERR_INVALIDDRIVE:
      case F_ERR_TOOLONGNAME:
         err = IOINTF_INVALIDNAME;
         break;

      case F_ERR_NOTFOUND:
         err = IOINTF_ENOENT;
         break;

      case F_ERR_DUPLICATED:
         err = IOINTF_EXIST;
         break;

      case F_ERR_NOMOREENTRY:
         err = IOINTF_NOSPACE;
         break;

      case F_ERR_LOCKED:
      case F_ERR_ACCESSDENIED:
      case F_ERR_WRITEPROTECT:
         err = IOINTF_NOACCESS;
         break;

      case F_ERR_NOTEMPTY:
         err = IOINTF_NOTEMPTY;
         break;

      case F_ERR_ALLOCATION:
         err = IOINTF_MEM;
         break;

      default:
         err = IOINTF_IOERROR;
   }
   return err;
}

static void
setErrCode(int err, int* status, const char** emsg)
{
   if(err)
   {
      *status=toBaErr(err);
      if(emsg)
      {
         const char* e;
         switch(err)
         {
            case F_ERR_INVALIDDRIVE: e="INVALIDDRIVE"; break;
            case F_ERR_NOTFORMATTED: e="NOTFORMATTED"; break;
            case F_ERR_INVALIDDIR: e="INVALIDDIR"; break;
            case F_ERR_INVALIDNAME: e="INVALIDNAME"; break;
            case F_ERR_NOTFOUND: e="NOTFOUND"; break;
            case F_ERR_DUPLICATED: e="DUPLICATED"; break;
            case F_ERR_NOMOREENTRY: e="NOMOREENTRY"; break;
            case F_ERR_NOTOPEN: e="NOTOPEN"; break;
            case F_ERR_EOF: e="EOF"; break;
            case F_ERR_RESERVED: e="RESERVED"; break;
            case F_ERR_NOTUSEABLE: e="NOTUSEABLE"; break;
            case F_ERR_LOCKED: e="LOCKED"; break;
            case F_ERR_ACCESSDENIED: e="ACCESSDENIED"; break;
            case F_ERR_NOTEMPTY: e="NOTEMPTY"; break;
            case F_ERR_INITFUNC: e="INITFUNC"; break;
            case F_ERR_CARDREMOVED: e="CARDREMOVED"; break;
            case F_ERR_ONDRIVE: e="ONDRIVE"; break;
            case F_ERR_INVALIDSECTOR: e="INVALIDSECTOR"; break;
            case F_ERR_READ: e="READ"; break;
            case F_ERR_WRITE: e="WRITE"; break;
            case F_ERR_INVALIDMEDIA: e="INVALIDMEDIA"; break;
            case F_ERR_BUSY: e="BUSY"; break;
            case F_ERR_WRITEPROTECT: e="WRITEPROTECT"; break;
            case F_ERR_INVFATTYPE: e="INVFATTYPE"; break;
            case F_ERR_MEDIATOOSMALL: e="MEDIATOOSMALL"; break;
            case F_ERR_MEDIATOOLARGE: e="MEDIATOOLARGE"; break;
            case F_ERR_NOTSUPPSECTORSIZE: e="NOTSUPPSECTORSIZE"; break;
            case F_ERR_DRVALREADYMNT: e="DRVALREADYMNT"; break;
            case F_ERR_TOOLONGNAME: e="TOOLONGNAME"; break;
            case F_ERR_NOTFORREAD: e="NOTFORREAD"; break;
            case F_ERR_DELFUNC: e="DELFUNC"; break;
            case F_ERR_ALLOCATION: e="ALLOCATION"; break;
            case F_ERR_INVALIDPOS: e="INVALIDPOS"; break;
            case F_ERR_NOMORETASK: e="NOMORETASK"; break;
            case F_ERR_NOTAVAILABLE: e="NOTAVAILABLE"; break;
            case F_ERR_TASKNOTFOUND: e="TASKNOTFOUND"; break;
            case F_ERR_UNUSABLE: e="UNUSABLE"; break;
            case F_ERR_CRCERROR: e="CRCERROR"; break;
            default:
               e="UNKNOWN";
         }
         *emsg=e;
      }
   }
   else
   {
      *status=0;
      if(emsg)
         *emsg=0;
   }
}


/* DirIter maintains state information for DiskIo_openDir,
 * DirIter_read, DirIter_getName, DirIter_stat, and DiskIo_closeDir.
 * A DirIter instance is created in DiskIo_openDir and released in
 * DiskIo_closeDir
 */
typedef struct DirIter
{
   DirIntf super;
   BA_FIND find;
   int err;
#ifdef HCC_UNICODE
   char* name;
#endif
}DirIter;


static int
DirIter_read(DirIntfPtr super)
{
   DirIter* o = (DirIter*)super;
   if(o->err)
      return toBaErr(o->err);
   for(;;)
   {
      if((o->err=ba_findnext(&o->find)) == 0)
      {
         const W_CHAR* wn = o->find.filename;
         if(wn[0]== '.' && (!wn[1] || (wn[1]=='.' && !wn[2])))
            continue;
         return 0;
      }
      else
         break;
   }
   o->err=F_ERR_NOTFOUND;
   return IOINTF_NOTFOUND;
}


static const char*
DirIter_getName(DirIntfPtr super)
{
   DirIter* o = (DirIter*)super;
   if(o->err)
      return 0;
#ifdef HCC_UNICODE
   if(o->name) baFree(o->name);
   {
      int len=(ba_strlen(o->find.filename)+1)*3;
      o->name=baMalloc(len);
      if(!o->name) return 0;
      if(wCharToUtf8(o->name, len, o->find.filename) < 0) return 0;
      return o->name;
   }
#else
   return o->find.filename;
#endif
}


static int
DirIter_stat(DirIntfPtr super, IoStat* st)
{
   DirIter* o = (DirIter*)super;
   if(o->err)
      return toBaErr(o->err);
   st->lastModified = fileTime2UnixTime(o->find.cdate,o->find.ctime);
   st->isDir = o->find.attr & FS_ATTR_DIR ? TRUE : FALSE;
   st->size = o->find.filesize;
   return 0;
}


static int
DirIter_constructor(DirIter* o, W_CHAR* aname)
{
   DirIntf_constructor((DirIntf*)o,
                       DirIter_read,
                       DirIter_getName,
                       DirIter_stat);
#ifdef HCC_UNICODE
   o->name=0;
#endif
   o->err=ba_findfirst(aname,&o->find);
   return o->err;
}


static void
DirIter_destructor(DirIter* o)
{
#ifdef HCC_UNICODE
   if(o->name) baFree(o->name);
#endif
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
      FS_FILE* fp;
} DiskRes;


static int
DiskRes_read(ResIntfPtr super, void* buf, size_t maxSize, size_t* size)
{
   int status;
   DiskRes* o = (DiskRes*)super; /* upcast */
   *size=fsm_read(buf, 1, maxSize, o->fp);
   if(*size != maxSize)
   {
      if(fsm_eof(o->fp))
      {
         return *size == 0 ? IOINTF_EOF : 0;
      }
      setErrCode(fsm_getlasterror(),&status,0);
   }
   else
      status = *size == 0 ? IOINTF_IOERROR : 0;
   return status;
}


static int
DiskRes_write(ResIntfPtr super, const void* buf, size_t size)
{
   int status;
   DiskRes* o = (DiskRes*)super; /* upcast */
   if(fsm_write(buf, size, 1, o->fp) == 1)
      status=0;
   else
      setErrCode(fsm_getlasterror(),&status,0);
   return status;
}


static int
DiskRes_seek(ResIntfPtr super, BaFileSize offset)
{
   int status;
   DiskRes* o = (DiskRes*)super; /* upcast */
   setErrCode(fsm_seek(o->fp, offset, FS_SEEK_SET),&status,0);
   return status;
}


static int
DiskRes_flush(ResIntfPtr super)
{
   int status;
   DiskRes* o = (DiskRes*)super; /* upcast */
   setErrCode(fsm_flush(o->fp),&status,0);
   return status;
}

static int
DiskRes_close(ResIntfPtr super)
{
   FS_FILE* fp;
   int status;
   baAssert(super);
   if(!super) return -1;
   fp = ((DiskRes*)super)->fp;
   baFree(super);
   setErrCode(fsm_close(fp),&status,0);
   return status;
}



static void
DiskRes_constructor(DiskRes* o, FS_FILE* fp)
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
static W_CHAR*
DiskIo_mkAbsPath(DiskIo* o, const char* name, int* status)
{
#ifdef HCC_UNICODE
   size_t len;
   W_CHAR* aname;
   if(*name == '/') name++;
   if( ! o->rootPath )
   {  /* If not using an offset start path i.e. if reading from '/',
       * where '/' contains the drives on the PC.
       */
      const char* ptr;

       /* Char \ is not seen as a dir sep by BA: potential security issue */
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
      aname = (W_CHAR*)baMalloc(len*sizeof(W_CHAR));
      if(aname)
      {
         aname[0]=name[0]; /* Set drive */
         aname[1]=':';
         aname[2]=0;
         utf8ToWChar(aname+2, ptr);
         return unix2DosPath(aname);
      }
      *status=IOINTF_MEM;
      return 0;
   }
   /* If using an offset start path */
   len = strlen(name)+1;
   aname = (W_CHAR*)baMalloc((o->rootPathLen+len)*sizeof(W_CHAR));
   if(aname)
   {
      W_CHAR* wptr;
      ba_strcpy(aname, o->rootPath);
      utf8ToWChar(aname+o->rootPathLen, name);
      wptr=aname+o->rootPathLen;
      while(*wptr)
      {  /* The server removes all .. by using baElideDotDot. A
            .. sequence after converting UTF8 to wide char indicates a
            security penetration attempt.
          */
         if(wptr[0] == '.' && wptr[1] == '.')
            break;
         wptr++;
      }
      if( ! *wptr )
         return unix2DosPath(aname);
      baFree(aname);
      *status=IOINTF_INVALIDNAME; /* Security penetration attempt */
      return 0;
   }
   *status=E_MALLOC;
   return 0;
#else
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
   return unix2DosPath(retVal);
#endif
}



static DirIntfPtr
DiskIo_openDir(IoIntfPtr super, const char* rname,
               int* status, const char** ecode)
{
   W_CHAR* aname;
   DiskIo* o = (DiskIo*)super;
   int len = strlen(rname);
   char* name=baMalloc(len + 6);
   if(name)
   {
      if(ecode) *ecode=0;
      basprintf(name,"%s%s%s",
                rname,
                (len == 0 || rname[len-1] == '/') ? "" : "/",
                "*.*");
      aname = DiskIo_mkAbsPath(o, name, status);
      baFree(name);
      if(aname)
      {
         DirIter* di = (DirIter*)baMalloc(sizeof(DirIter));
         if(!di || DirIter_constructor(di,aname))
         {
            setErrCode(di ? di->err : F_ERR_ALLOCATION, status, ecode);
            if(di) baFree(di);
            di=0;
         }
         baFree(aname);
         return (DirIntfPtr)di;
      }
   }
   else
      setErrCode(F_ERR_ALLOCATION,status,ecode);
   return 0;
}


static int
DiskIo_mkDir(IoIntfPtr super, const char* rname, const char** ecode)
{
   DiskIo* o = (DiskIo*)super;
   W_CHAR* aname;
   int status;

   if(ecode) *ecode=0;

   if( (aname = DiskIo_mkAbsPath(o, rname, &status)) != 0)
   {
      setErrCode(ba_mkdir(aname),&status, ecode);
      baFree(aname);
   }
   return status;
}

static int
DiskIo_rename(
   IoIntfPtr super, const char* rfrom, const char* rto, const char** ecode)
{
   DiskIo* o = (DiskIo*)super;
   W_CHAR* afrom;
   W_CHAR* ato;
   int status;

   if(ecode) *ecode=0;

   if( (ato = DiskIo_mkAbsPath(o, rto, &status)) != 0)
   {
      if( (afrom = DiskIo_mkAbsPath(o, rfrom, &status)) != 0)
      {
         setErrCode(ba_move(afrom, ato), &status, ecode);
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
   W_CHAR* aname;
   int status;

   if(ecode) *ecode=0;

   if( (aname = DiskIo_mkAbsPath(o, rname, &status)) != 0)
   {
      setErrCode(ba_delete(aname),&status, ecode);
      baFree(aname);
   }
   return status;
}


static int
DiskIo_rmDir(IoIntfPtr super, const char* rname, const char** ecode)
{
   DiskIo* o = (DiskIo*)super;
   W_CHAR* aname;
   int status;

   if(ecode) *ecode=0;

   if( (aname = DiskIo_mkAbsPath(o, rname, &status)) != 0)
   {
      setErrCode(ba_rmdir(aname),&status, ecode);
      baFree(aname);
   }
   return status;
}


static int
DiskIo_stat(IoIntfPtr super, const char* rname, IoStat* st)
{
   DiskIo* o = (DiskIo*)super;
   W_CHAR* aname;
   int status;
   if( (aname = DiskIo_mkAbsPath(o, rname, &status)) != 0)
   {
      FS_STAT fst;
      W_CHAR* ptr = aname+ba_strlen(aname)-1;
      if(FS_SEPARATORCHAR == *ptr)
         *ptr=0;
      if( (status=ba_xstat(aname, &fst)) != 0 )
      {
         setErrCode(status,&status,0);
      }
      else
      {
		 /* *TEKMOD* Corrected typo */
         st->isDir = (FSEC_ATTR_DIR & fst.secure) ? TRUE : FALSE;
         st->lastModified =
            fileTime2UnixTime(fst.createdate,fst.createtime);
         st->size = fst.filesize;
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
   W_CHAR* aname;
   ResIntfPtr retVal=0;
   if(ecode) *ecode=0;
   if( (aname = DiskIo_mkAbsPath(o, rname, status)) != 0)
   {
      DiskRes* dr = (DiskRes*)baMalloc(sizeof(DiskRes));
      if(dr)
      {
         static const W_CHAR wb[]={'w',0};
         static const W_CHAR rb[]={'r',0};
         FS_FILE* fp = ba_open(aname, mode == OpenRes_WRITE ? wb : rb);
         if(fp)
         {
            DiskRes_constructor(dr, fp);
            retVal = (ResIntfPtr)dr; /* Downcast */
            *status=0;
         }
         else
         {
            setErrCode(fsm_getlasterror(),status, ecode);
            baFree(dr);
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
      *((U32*)a) = TRUE; /* "HCC SAFE file system" can move directories */
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
            *platform="HCC-SAFE";
         }
         return 0;
      }
   }
   if( ! strcmp(name, "abs") )
   {
      W_CHAR* wpath = DiskIo_mkAbsPath(o, (const char*)a, &status);
      if(wpath)
      {
#ifdef HCC_UNICODE
         int len=(ba_strlen(wpath)+1)*3;
         char* path=baMalloc(len);
         if(path)
         {
            if(wCharToUtf8(path, len, wpath) < 0)
            {
               baFree(path);
               path=0;
            }
         }
         baFree(wpath);
         status = path ? 0 : IOINTF_MEM;
         *((char**)b) = path;
#else
         *((char**)b) = wpath;
#endif
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
      W_CHAR* wpath;
      DiskIo* di = (DiskIo*)a;
      const char* ptr = (const char*)b;
      if(!ptr) ptr="";
      wpath = DiskIo_mkAbsPath(o, ptr, &status);
      DiskIo_constructor(di);
      if(wpath)
      {
#ifdef HCC_UNICODE
         int len=(ba_strlen(wpath)+1)*3;
         char* path=baMalloc(len);
         if(path)
         {
            if(wCharToUtf8(path, len, wpath) < 0)
            {
               baFree(path);
               path=0;
            }
            else
               status=DiskIo_setRootDir(di, path);
         }
         baFree(wpath);
         status = path ? 0 : IOINTF_MEM;
#else
         status=DiskIo_setRootDir(di, wpath);
#endif
         baFree(wpath);
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
      W_CHAR* aname;
      FS_STAT st;
      aname = baMalloc((strlen(root)+1)*sizeof(W_CHAR));
      if(!aname) return IOINTF_MEM;
#ifndef HCC_UNICODE
      strcpy(aname, root);
#else
      if(utf8ToWChar(aname, root))
         status=IOINTF_INVALIDNAME;
      else
#endif
      {
         if( (status=ba_xstat(aname, &st)) != 0 )
            setErrCode(status,&status, 0);
         else if(FSSEC_ATTR_DIR & st.secure)
         {
            if(o->rootPath)
               baFree(o->rootPath);
            o->rootPath = baMalloc((ba_strlen(aname)+2)*sizeof(W_CHAR));
            if(o->rootPath)
            {
               W_CHAR* ptr;
               ba_strcpy(o->rootPath, aname);
               ptr=o->rootPath;
               while(ptr[1]) ptr++;
               if(*ptr != '/')
               {
                  ptr[1]='/';
                  ptr[2]=0;
               }
               o->rootPathLen = ba_strlen(o->rootPath);
               status=0;
            }
            else
               status=IOINTF_MEM;
         }
         else
            status = IOINTF_NOACCESS;
      }
      baFree(aname);
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
#ifdef HCC_UNICODE
   return wCharToUtf8((U8*)buf,len,o->rootPath) < 0 ? -1 : 0;
#else
   strcpy(buf,o->rootPath);
   return strlen(buf);
#endif
}
