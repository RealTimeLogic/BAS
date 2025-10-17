
/*
 * Zephyr-converted POSIX BaFile.c by ChatGPT, but with manual fixes.
 *
 */

#include <BaDiskIo.h>
//#include <BaServerLib.h>

//#include <string.h>
//#include <errno.h>

/* Zephyr FS */
#include <sys/types.h>
#include <zephyr/sys/dlist.h>
#include <zephyr/fs/fs_interface.h>

/* Map errno -> IOINTF_*
 */
static void setErrCode(int* status, const char** emsg)
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
#ifdef EFBIG
      case EFBIG:
#endif
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
   if(emsg) *emsg = strerror(ecode);
}

/* ---------------------- Directory iterator (Zephyr) ---------------------- */

typedef struct
{
   DirIntf super; /* Implements the abstract DirIntf class */
   struct fs_dir_t dir;
   struct fs_dirent cur;   /* last entry read */
   char* fname;            /* pointer into cur.name (or owned buffer) */
   char* dname;            /* directory name we opened; owned */
   int   dnameLen;
   bool  eod;              /* end-of-dir flag */
} DirIter;

static int DirIter_read(DirIntfPtr super)
{
   DirIter* o = (DirIter*)super;
   if(o->eod) return IOINTF_NOTFOUND;

   memset(&o->cur, 0, sizeof(o->cur));
   int rc = fs_readdir(&o->dir, &o->cur);
   if(rc)
   {
      int status; setErrCode(&status, 0); return status;
   }
   /* Zephyr signals end-of-dir with name length==0 */
   if(o->cur.name[0] == '\0')
   {
      o->eod = true;
      return IOINTF_NOTFOUND;
   }
   o->fname = o->cur.name;
   return 0;
}

static const char* DirIter_getName(DirIntfPtr super)
{
   DirIter* o = (DirIter*)super;
   return (o->eod) ? 0 : o->fname;
}

static int DirIter_stat(DirIntfPtr super, IoStat* st)
{
   int status=0;
   DirIter* o = (DirIter*)super;
   if(o->eod || !o->fname) return IOINTF_NOTFOUND;

   /* Build full path to current entry and call fs_stat */
   int len = (int)(strlen(o->dname) + strlen(o->fname) + 2);
   char* path = (char*)baMalloc(len);
   if(!path) return IOINTF_MEM;

   if(o->dname[0] != '/' || o->dname[1])
   {
      if(o->dname[o->dnameLen-1] == '/')
         basnprintf(path, len, "%s%s", o->dname, o->fname);
      else
         basnprintf(path, len, "%s/%s", o->dname, o->fname);
   }
   else
      basnprintf(path, len, "/%s", o->fname);
   struct fs_dirent entry;
   if(fs_stat(path, &entry))
   {
      setErrCode(&status, 0);
   }
   else
   {
      st->isDir = (entry.type == FS_DIR_ENTRY_DIR) ? TRUE : FALSE;
      st->size = st->isDir ? 0 : entry.size;
      /* TODO(zephyr?): mtime not standard across FS backends */
      st->lastModified = 0;
   }
   baFree(path);
   return status;
}

static void DirIter_constructor(DirIter* o, const char* dname)
{
   DirIntf_constructor((DirIntf*)o, DirIter_read, DirIter_getName, DirIter_stat);
   o->fname = 0;
   o->dname = (char*)dname; /* takes ownership of allocated string */
   o->dnameLen = (int)strlen(dname);
   o->eod = false;
}

static void DirIter_destructor(DirIter* o)
{
   (void)fs_closedir(&o->dir);
   if(o->dname) baFree(o->dname);
}


/* ---------------------- DiskRes (file resource) ---------------------- */

typedef struct
{
   ResIntf super; /* Implements the abstract ResIntf class */
   struct fs_file_t file;
} DiskRes;

static int DiskRes_read(ResIntfPtr super, void* buf, size_t maxSize, size_t* size)
{
   DiskRes* o = (DiskRes*)super;
   int rc = fs_read(&o->file, buf, maxSize);
   if(rc < 0)
   {
      int status;
      setErrCode(&status, 0);
      return status;
   }
   *size = (size_t)rc;
   if((size_t)rc == 0)
      return IOINTF_EOF; /* EOF */
   return 0;
}

static int DiskRes_write(ResIntfPtr super, const void* buf, size_t size)
{
   DiskRes* o = (DiskRes*)super;
   int rc = fs_write(&o->file, buf, size);
   if(rc < 0)
   {
      int status;
      setErrCode(&status, 0);
      return status;
   }
   if((size_t)rc != size)
      return IOINTF_IOERROR; /* short write */
   return 0;
}

static int DiskRes_seek(ResIntfPtr super, BaFileSize offset)
{
   DiskRes* o = (DiskRes*)super;
   int rc = fs_seek(&o->file, (off_t)offset, FS_SEEK_SET);
   if(rc)
   {
      int status;
      setErrCode(&status, 0);
      return status;
   }
   return 0;
}

static int DiskRes_flush(ResIntfPtr super)
{
   DiskRes* o = (DiskRes*)super;
   int rc = fs_sync(&o->file);
   if(rc)
   {
      int status;
      setErrCode(&status, 0);
      return status;
   }
   return 0;
}

static int DiskRes_close(ResIntfPtr super)
{
   DiskRes* o = (DiskRes*)super;
   if(!o)
      return -1;
   int rc = fs_close(&o->file);
   baFree(super);
   if(rc)
   {
      setErrCode(&rc, 0);
      return rc;
   }
   return 0;
}

static void DiskRes_constructor(DiskRes* o)
{
   ResIntf_constructor((ResIntf*)o,
                       DiskRes_read,
                       DiskRes_write,
                       DiskRes_seek,
                       DiskRes_flush,
                       DiskRes_close);
   memset(&o->file, 0, sizeof(o->file));
}


/* ---------------------- DiskIo helpers ---------------------- */

/* Make absolute path by adding rootPath + name.
 * Space to hold the returned string is obtained from baMalloc.
 */
static char* DiskIo_mkAbsPath(DiskIo* o, const char* name, int* status)
{
   int len;
   char* retVal;

   const char* rootPath;
   if(o->rootPath)
   {
      rootPath = (const char*)o->rootPath;
   } else
   {
      rootPath = "/"; /* default root */
      o->rootPathLen = 1;
   }

   if(*name == '/') name++; /* make name relative to rootPath */

   len = o->rootPathLen + (int)strlen(name) + 1;
   retVal = (char*)baMalloc(len);
   if(retVal)
   {
      memcpy(retVal, rootPath, o->rootPathLen);
      strcpy(retVal + o->rootPathLen, name);
      *status = 0;
   } else
   {
      *status = E_MALLOC;
   }
   return retVal;
}

/* ---------------------- DiskIo directory ops ---------------------- */

static DirIntfPtr DiskIo_openDir(
   IoIntfPtr super, const char* rname, int* status, const char** ecode)
{
   DiskIo* o = (DiskIo*)super;
   if(ecode) *ecode = 0;

   char* aname = DiskIo_mkAbsPath(o, rname, status);
   if(!aname) return 0;

   DirIter* di = (DirIter*)baMalloc(sizeof(DirIter));
   if(!di)
   {
      *status = E_MALLOC;
      baFree(aname);
      return 0;
   }
   memset(di,0,sizeof(DirIter));
   DirIter_constructor(di, aname);
   int rc = fs_opendir(&di->dir, aname);
   if(rc)
   {
      int st; setErrCode(&st, ecode);
      baFree(di);
      baFree(aname);
      return 0;
   }
   *status = 0;
   return (DirIntfPtr)di;
}

static int DiskIo_closeDir(IoIntfPtr super, DirIntfPtr* dirIntf)
{
   (void)super;
   if(dirIntf && *dirIntf)
   {
      DirIter_destructor((DirIter*)*dirIntf);
      baFree(*dirIntf);
      *dirIntf = 0;
      return 0;
   }
   return IOINTF_IOERROR;
}

static int DiskIo_mkDir(IoIntfPtr super, const char* rname, const char** ecode)
{
   DiskIo* o = (DiskIo*)super;
   if(ecode) *ecode = 0;
   int status;
   char* aname = DiskIo_mkAbsPath(o, rname, &status);
   if(!aname) return status;
   status = fs_mkdir(aname);
   baFree(aname);
   if(status)
   {
      setErrCode(&status, ecode);
      return status;
   }
   return 0;
}

static int DiskIo_unlink(IoIntfPtr super, const char* rname, const char** ecode)
{
   DiskIo* o = (DiskIo*)super;
   if(ecode) *ecode = 0;
   int status;
   char* aname = DiskIo_mkAbsPath(o, rname, &status);
   if(!aname) return status;
   status = fs_unlink(aname);
   baFree(aname);
   if(status)
   {
      setErrCode(&status, ecode);
      return status;
   }
   return 0;
}

static int DiskIo_rename(IoIntfPtr super, const char* rfrom, const char* rto, const char** ecode)
{
   DiskIo* o = (DiskIo*)super;
   if(ecode) *ecode = 0;
   int status;
   char* afrom = DiskIo_mkAbsPath(o, rfrom, &status);
   if(!afrom) return status;
   char* ato   = DiskIo_mkAbsPath(o, rto, &status);
   if(!ato)
   { baFree(afrom); return status; }

   status = fs_rename(afrom, ato);
   baFree(afrom); baFree(ato);
   if(status)
   {
      setErrCode(&status, ecode);
      return status;
   }
   return 0;
}

static int DiskIo_stat(IoIntfPtr super, const char* rname, IoStat* st)
{
   DiskIo* o = (DiskIo*)super;
   int status;
   char* aname = DiskIo_mkAbsPath(o, rname, &status);
   if(!aname) return status;
   struct fs_dirent entry;
   status = fs_stat(aname, &entry);
   baFree(aname);
   if(status)
   {
      setErrCode(&status, 0);
      return status;
   }
   st->isDir = (entry.type == FS_DIR_ENTRY_DIR) ? TRUE : FALSE;
   st->size = st->isDir ? 0 : entry.size;
   st->lastModified = 0; /* TODO(zephyr?): no standard mtime */
   return 0;
}



/* ---------------------- DiskIo open resource (file) ---------------------- */

static ResIntfPtr DiskIo_openRes(IoIntfPtr super, const char* rname, U32 mode,
                                 int* status, const char** ecode)
{
   DiskIo* o = (DiskIo*)super; /* Upcast */
   if(ecode) *ecode = 0;

   char* aname = DiskIo_mkAbsPath(o, rname, status);
   if(!aname) return 0;

   DiskRes* dr = (DiskRes*)baMalloc(sizeof(DiskRes));
   if(!dr)
   {
      *status = IOINTF_MEM;
      baFree(aname);
      return 0;
   }

   DiskRes_constructor(dr);

   /* Map Ba OpenRes_* flags to Zephyr fs flags */
   int flags = 0;
   if(mode & OpenRes_WRITE)
   {
      flags |= FS_O_WRITE;
      if(mode & OpenRes_READ) flags |= FS_O_READ;
      if(mode & OpenRes_APPEND)
      {
         flags |= FS_O_APPEND;
      }
      else
      {
         flags |= FS_O_CREATE | FS_O_TRUNC; /* create/truncate when opening for write */
      }
   }
   else
   {
      flags |= FS_O_READ;
   }

   int rc = fs_open(&dr->file, aname, flags);
   if(rc)
   {
      baFree(dr);
      setErrCode(status, ecode);
      baFree(aname);
      return 0;
   }

   *status = 0;
   baFree(aname);
   return (ResIntfPtr)dr; /* Downcast */
}

/* ---------------------- DiskIo property/ctor/dtor ---------------------- */

static int DiskIo_property(IoIntfPtr super,const char* name,void* a,void* b)
{
   int status;
   DiskIo* o = (DiskIo*)super;
   if(!strcmp(name, "movedir"))
   {
      *((U32*)a) = TRUE; /* Zephyr fs_rename works for directories when supported by backend */
      return 0;
   }
   if(!strcmp(name, "type"))
   {
      if(a)
      {
         const char** type = (const char**)a;
         *type = "disk"; /* still a disk-like FS */
         if(b)
         {
            const char** platform = (const char**)b;
            *platform = "ZephyrFS";
         }
         return 0;
      }
   }
   if(!strcmp(name, "abs"))
   {
      char* path = DiskIo_mkAbsPath(o, (const char*)a, &status);
      if(path)
      {
         *((char**)b) = path;
         return 0;
      }
      return status;
   }
   if(!strcmp(name, "attach"))
   {
      if(a)
      {
         if(super->onTerminate) super->onTerminate(super->attachedIo, super);
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
   if(!strcmp(name, "dupsize"))
   {
      *((size_t*)a) = sizeof(DiskIo);
      return 0;
   }
   if(!strcmp(name, "dup"))
   {
      char* path;
      DiskIo* di = (DiskIo*)a;
      const char* ptr = (const char*)b; if(!ptr) ptr = "";
      path = DiskIo_mkAbsPath(o, ptr, &status);
      DiskIo_constructor(di);
      if(path)
      {
         status = DiskIo_setRootDir(di, path);
         baFree(path);
         if(status == IOINTF_NOTFOUND)
         {
            /* The path could be an absolute path */
            status = DiskIo_setRootDir(di, (const char*)b);
            if(status) status = IOINTF_NOTFOUND; /* original error */
         }
      }
      else
         status = E_MALLOC;
      if(status) DiskIo_destructor(di);
      return status;
   }

   if(!strcmp(name, "destructor"))
   {
      DiskIo_destructor(o);
      return 0;
   }

   return IOINTF_NOIMPLEMENTATION;
}

void DiskIo_constructor(DiskIo* o)
{
   IoIntf_constructorRW((IoIntf*)o,
                        DiskIo_property,
                        DiskIo_closeDir,
                        DiskIo_mkDir,
                        DiskIo_rename,
                        DiskIo_openDir,
                        DiskIo_openRes,
                        0, /* openResGzip */
                        DiskIo_unlink,
                        DiskIo_unlink,
                        DiskIo_stat);
   o->rootPath = 0;
   o->rootPathLen = 0;
}

void DiskIo_destructor(DiskIo* o)
{
   IoIntfPtr super = (IoIntfPtr)o;
   if(super->onTerminate) super->onTerminate(super->attachedIo, super);
   if(o->rootPath)
   { baFree(o->rootPath); o->rootPath = 0; o->rootPathLen = 0; }
}

/* Zephyr-friendly root setter: keep it simple and trust the caller/mounts.
 * Optionally we could fs_stat() to verify directory existence. */
int DiskIo_setRootDir(DiskIo* o, const char* root)
{
   if(!root) return IOINTF_NOTFOUND;
   if(o->rootPath)
   { baFree(o->rootPath); o->rootPath = 0; }

   size_t n = strlen(root);
   o->rootPath = (char*)baMalloc(n + 2);
   if(!o->rootPath) return E_MALLOC;

   strcpy(o->rootPath, root);
   if(n > 0 && root[n-1] != '/') strcat(o->rootPath, "/");
   o->rootPathLen = (int)strlen(o->rootPath);

   struct fs_dirent entry;
   if(fs_stat(o->rootPath, &entry) || entry.type != FS_DIR_ENTRY_DIR)
   {
      return IOINTF_NOTFOUND;
   }
   return 0;
}

int DiskIo_getRootDir(DiskIo* o, char* buf, int len)
{
   if(len < o->rootPathLen || len < 2) return -1;
   if(!o->rootPath)
   {
      strcpy(buf, "/");
      return 1;
   }
   strcpy(buf, o->rootPath);
   return (int)strlen(buf);
}

/****************************************************************************
                                   BaFile functions
****************************************************************************/
/* not ported; not required */
