/*
 *     ____             _________                __                _     
 *    / __ \___  ____ _/ /_  __(_)___ ___  ___  / /   ____  ____ _(_)____
 *   / /_/ / _ \/ __ `/ / / / / / __ `__ \/ _ \/ /   / __ \/ __ `/ / ___/
 *  / _, _/  __/ /_/ / / / / / / / / / / /  __/ /___/ /_/ / /_/ / / /__  
 * /_/ |_|\___/\__,_/_/ /_/ /_/_/ /_/ /_/\___/_____/\____/\__, /_/\___/  
 *                                                       /____/          
 *
 *                  Barracuda Embedded Web-Server
 *
 ****************************************************************************
 *			      HEADER
 *
 *   $Id: ZipFileIterator.h 4915 2021-12-01 18:26:55Z wini $
 *
 *   COPYRIGHT:  Real Time Logic LLC, 2003-2019
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
 *
 */
#ifndef __ZipFileIterator_h
#define __ZipFileIterator_h

#include <HttpServer.h>
#include <CspRunTm.h>

#ifndef __DOXYGEN__
struct ZipContainer;
struct ZipFileHeaderData;
#endif

#ifdef _SHARK_DOXY
/** @defgroup ZipFileIo ZIP File System IO
    @ingroup ZipFileSystem
    @{

    \brief Minnow Server ZIP file system IO plugin.

 */
#else
/** @addtogroup IO
    @{
 */
#endif

/** Abstract interface class for reading a ZipFile.
    See the example code FileZipReader for more information.
    You can also use the <a href="../CspTools.html#bin2c">
    bin2c</a> tool if you want to embed the ZIP file in the
    application executable or firmware.
*/
typedef struct ZipReader
#ifdef __cplusplus
: public CspReader
{
      ZipReader() {}
      ZipReader(CspReader_Read r, U32 zipFileSize);
#else
{ 
      CspReader super; /* As if inherited. */
#endif
      U32 size;
} ZipReader;

#ifdef __cplusplus
extern "C" {
#endif
BA_API void ZipReader_constructor(
   ZipReader* o, CspReader_Read r, U32 zipFileSize);
#ifdef __cplusplus
}
inline ZipReader::ZipReader(CspReader_Read r, U32 zipFileSize) {
   ZipReader_constructor(this, r, zipFileSize); }
#endif


typedef enum {
   ZipErr_Buf = -2000, /* The buffer is too small. */
   ZipErr_Reading, /* Reading failed. */
   ZipErr_Spanned,  /* Spanned/Split archives not supported. */
   ZipErr_Compression, /* Unsupported compr. Can be one of Stored or Deflated */
   ZipErr_Incompatible, /* Unknown ZIP Central Directory Structure. */
   ZipErr_NoError = 0
} ZipErr;


typedef enum {
   ZipComprMethod_Stored=0,
   ZipComprMethod_Deflated=8,
   ZipComprMethod_AES=99       /* Gianluca 9/22 */
} ZipComprMethod;


#ifndef __DOXYGEN__

typedef struct ZipFileHeader
{
#ifdef __cplusplus
      const char* getFn();
      const char* getEf();
	  const char* getAESef();
      bool isDirectory();
      U16 getFnLen();
      U16 getEfLen();
      U16 getFcLen();
      ZipComprMethod getComprMethod();
      U32 getUncompressedSizeLittleEndian();
      U32 getCrc32LittleEndian();
      U32 getCompressedSize();
      U32 getUncompressedSize();
      U32 getCrc32();
      static const char* e2str(ZipErr e);
      U16 getVersionMade();
      U16 getFlag();
      U32 getDataOffset();
      U32 getTime();
      CspReader* getReader();
   private:
#endif
      struct ZipFileHeaderData* data;
      char* fn;  /* File name. Not null terminated. */
      U8* ef;    /* Extra field - GV 9/28 */
      U8* AESef; /* AES extra field - GV 11/30/2010 */
      ZipReader* reader;
      U8* buf;
      U32 bufSize;
      U32 fileHeaderOffs;
      ZipComprMethod comprMethod;
      U16 fnLen;
      U16 efLen;
      U16 fcLen;
} ZipFileHeader;

#ifdef __cplusplus
extern "C" {
#endif
#define ZipFileHeader_getFn(o) (o)->fn
#define ZipFileHeader_getEf(o) (o)->ef
#define ZipFileHeader_getAESef(o) (o)->AESef
#define ZipFileHeader_isDirectory(o) ((o)->fn[(o)->fnLen-1] == '/')
#define ZipFileHeader_getFnLen(o) (o)->fnLen
#define ZipFileHeader_getEfLen(o) (o)->efLen
#define ZipFileHeader_getFcLen(o) (o)->fcLen
#define ZipFileHeader_getComprMethod(o) (o)->comprMethod
#define ZipFileHeader_getReader(o) (CspReader*)(o)->reader
BA_API U32 ZipFileHeader_getUncompressedSizeLittleEndian(ZipFileHeader* o);
BA_API U32 ZipFileHeader_getCrc32LittleEndian(ZipFileHeader* o);
BA_API U32 ZipFileHeader_getCompressedSize(ZipFileHeader* o);
BA_API U32 ZipFileHeader_getUncompressedSize(ZipFileHeader* o);
BA_API U32 ZipFileHeader_getCrc32(ZipFileHeader* o);
BA_API const char* ZipFileHeader_e2str(ZipErr e);

/* Return: The operating system that compressed the file. */
BA_API U16 ZipFileHeader_getVersionMade(ZipFileHeader* o);

/*  ZipFileHeader_getFlag
     Bit 0: If set, indicates that the file is encrypted.
     Bit 2  Bit 1
     0      0    Normal (-en) compression option was used.
     0      1    Maximum (-exx/-ex) compression option was used.
     1      0    Fast (-ef) compression option was used.
     1      1    Super Fast (-es) compression option was used.
*/
BA_API U16 ZipFileHeader_getFlag(ZipFileHeader* o);

BA_API U32 ZipFileHeader_getDataOffset(ZipFileHeader* o);
BA_API U32 ZipFileHeader_getTime(ZipFileHeader* o);

#ifdef __cplusplus
}
inline const char* ZipFileHeader::getFn() {
   return ZipFileHeader_getFn(this);
}
inline const char* ZipFileHeader::getEf() {
   return (char*)ZipFileHeader_getEf(this);
}
inline const char* ZipFileHeader::getAESef() {
   return (char*)ZipFileHeader_getAESef(this);
}
inline bool ZipFileHeader::isDirectory() {
   return ZipFileHeader_isDirectory(this) ? true : false;
}
inline U16 ZipFileHeader::getFnLen() {
   return ZipFileHeader_getFnLen(this);
}
inline U16 ZipFileHeader::getEfLen() {
   return ZipFileHeader_getEfLen(this);
}
inline U16 ZipFileHeader::getFcLen() {
   return ZipFileHeader_getFcLen(this);
}
inline ZipComprMethod ZipFileHeader::getComprMethod() {
   return ZipFileHeader_getComprMethod(this);
}
inline CspReader* ZipFileHeader::getReader() {
   return ZipFileHeader_getReader(this);
}
inline U32 ZipFileHeader::getUncompressedSizeLittleEndian() {
   return ZipFileHeader_getUncompressedSizeLittleEndian(this);
}
inline U32 ZipFileHeader::getCrc32LittleEndian() {
   return ZipFileHeader_getCrc32LittleEndian(this);
}
inline U32 ZipFileHeader::getCompressedSize() {
   return ZipFileHeader_getCompressedSize(this);
}
inline U32 ZipFileHeader::getUncompressedSize() {
   return ZipFileHeader_getUncompressedSize(this);
}
inline U32 ZipFileHeader::getCrc32() {
   return ZipFileHeader_getCrc32(this);
}
inline const char* ZipFileHeader::e2str(ZipErr e) {
   return ZipFileHeader_e2str(e);
}
inline U16 ZipFileHeader::getVersionMade() {
   return ZipFileHeader_getVersionMade(this);
}
inline U16 ZipFileHeader::getFlag() {
   return ZipFileHeader_getFlag(this);
}
inline U32 ZipFileHeader::getDataOffset() {
   return ZipFileHeader_getDataOffset(this);
}
inline U32 ZipFileHeader::getTime() {
   return ZipFileHeader_getTime(this);
}
#endif

#endif /* __DOXYGEN__ */


/** Low level ZIP file information used internally by the Zip File System
 */
typedef struct ZipFileInfo
{
      U32 crc32;
      U32 dataOffset;
      U32 uncompressedSize;
      U32 compressedSize;
      U32 time;
      ZipComprMethod comprMethod;
      U16 flag;
      U8  *ef;  /* AES encrypted ZIP file info */
      U8  *AESef; /* AES encrypted ZIP file info */
} ZipFileInfo;

#if defined(B_LITTLE_ENDIAN)
#define ZipFileInfo_makeLittleEndianU32(x) x
#elif defined(B_BIG_ENDIAN)
#define ZipFileInfo_makeLittleEndianU32(x) \
 ((U32)((((U32)(x) & 0x000000ffU) << 24) | \
 (((U32)(x) & 0x0000ff00U) <<  8) | \
 (((U32)(x) & 0x00ff0000U) >>  8) | \
 (((U32)(x) & 0xff000000U) >> 24))) 
#else
#error in config/make file, you must define one of B_LITTLE_ENDIAN or B_BIG_ENDIAN
#endif

#define ZipFileInfo_getCrc32LittleEndian(o) \
  ZipFileInfo_makeLittleEndianU32((o)->crc32);
#define ZipFileInfo_getUncompressedSizeLittleEndian(o) \
  ZipFileInfo_makeLittleEndianU32((o)->uncompressedSize);


#define ZipFileInfo_constructor(o, fileHeader, EfPtr) \
do { \
   (o)->crc32 = ZipFileHeader_getCrc32(fileHeader); \
   (o)->dataOffset = ZipFileHeader_getDataOffset(fileHeader); \
   (o)->uncompressedSize = ZipFileHeader_getUncompressedSize(fileHeader); \
   (o)->compressedSize = ZipFileHeader_getCompressedSize(fileHeader); \
   (o)->time = ZipFileHeader_getTime(fileHeader); \
   (o)->comprMethod = ZipFileHeader_getComprMethod(fileHeader); \
   (o)->flag = ZipFileHeader_getFlag(fileHeader); \
   (o)->ef = EfPtr; \
   (o)->AESef = (EfPtr + (ZipFileHeader_getAESef(fileHeader) - ZipFileHeader_getEf(fileHeader))); \
}while(0)


#ifndef __DOXYGEN__
typedef struct
{
      U32 dataOffset;
      U32 uncompressedSize;
      U32 compressedSize;
      ZipComprMethod comprMethod;
} MiniZipFileInfo;
#endif

#define MiniZipFileInfo_constructor(o, fileHeader) \
do { \
   (o)->dataOffset = ZipFileHeader_getDataOffset(fileHeader); \
   (o)->uncompressedSize = ZipFileHeader_getUncompressedSize(fileHeader); \
   (o)->compressedSize = ZipFileHeader_getCompressedSize(fileHeader); \
   (o)->comprMethod = ZipFileHeader_getComprMethod(fileHeader); \
}while(0)


/** Low level <a target="_blank"
    href="http://en.wikipedia.org/wiki/Zip_(file_format)#Structure">
    ZIP file central directory</a> iterator.
 */
typedef struct CentralDirIterator
{
#ifdef __cplusplus
      CentralDirIterator(ZipContainer* container);
      CentralDirIterator(ZipContainer* container, U8* buf, U32 bufSize);
      ZipErr getECode();
      ZipFileHeader* getElement();
      bool nextElement();
   private:
#endif
      ZipFileHeader fileHeader;
      U32 curFileHeaderOffs;
      ZipErr err;
      U16 entriesInCd;
      U16 curEntry;
} CentralDirIterator;

#ifdef __cplusplus
extern "C" {
#endif
#define CentralDirIterator_getECode(o) (o)->err
BA_API void CentralDirIterator_constructor(CentralDirIterator* o,
                                    struct ZipContainer* container);
/* Re-entrant version,
 * minimum size for buffer is 256
 */
BA_API void CentralDirIterator_constructorR(CentralDirIterator* o,
                                     struct ZipContainer* container,
                                     U8* buf, U32 bufSize);
BA_API ZipFileHeader* CentralDirIterator_getElement(CentralDirIterator* o);
BA_API BaBool CentralDirIterator_nextElement(CentralDirIterator* o);
#ifdef __cplusplus
}
inline CentralDirIterator::CentralDirIterator(ZipContainer* container) {
   CentralDirIterator_constructor(this, container);
}
inline CentralDirIterator::CentralDirIterator(
   ZipContainer* container, U8* buf, U32 bufSize) {
   CentralDirIterator_constructorR(this, container, buf, bufSize);
}
inline ZipErr CentralDirIterator::getECode() {
   return CentralDirIterator_getECode(this);
}
inline ZipFileHeader* CentralDirIterator::getElement() {
   return CentralDirIterator_getElement(this);
}
inline bool CentralDirIterator::nextElement() {
   return CentralDirIterator_nextElement(this) ? true : false;
}
#endif



/** A ZipContainer is a buffer used by a ZipIo when reading data from
    a ZipReader. You do not directly use a ZipContainer unless you use
    the internal ZIP CentralDirIterator class. See the ZipFileIterator.h
    header file for more information.
*/
typedef struct ZipContainer
{
#ifdef __cplusplus
      ZipContainer(){}
      void *operator new(size_t s) { return ::baMalloc(s); }
      void operator delete(void* d) { if(d) ::baFree(d); }
      void *operator new(size_t, void *place) { return place; }
      void operator delete(void*, void *) { }
 
      /**
         \param reader a ZipReader instance.
         \param buf is a buffer with minimum size 256 bytes.
         You must make sure that this buffer is valid during the lifetime
         of the class instance.
         \param bufSize size of buffer.
       */
      ZipContainer(ZipReader* reader, U8* buf, U32 bufSize);
      ZipErr getECode();
   private:
#endif
      ZipReader* reader;
      U8* buf;
      U32 bufSize;
      U32 cdOffset;
      ZipErr errCode;
      U16 entriesInCd;
} ZipContainer;

#ifdef __cplusplus
extern "C" {
#endif
BA_API void ZipContainer_constructor(ZipContainer* o,
                              ZipReader* reader,
                              U8* buf,
                              U32 bufSize);
#define ZipContainer_getECode(o) (o)->errCode
#ifdef __cplusplus
}
inline ZipContainer::ZipContainer(ZipReader* reader, U8* buf, U32 bufSize) {
   ZipContainer_constructor(this,reader,buf,bufSize); }
inline ZipErr ZipContainer::getECode() {
   return ZipContainer_getECode(this); }
#endif

int initGZipHeader(struct ZipFileInfo* zfi, GzipHeader* gzipH);


/** @} */ /* end of IO */ 


#endif
