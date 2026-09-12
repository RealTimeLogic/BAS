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
 *   $Id: IoIntfZipReader.h 5978 2026-09-11 16:13:48Z wini $
 *
 *   COPYRIGHT:  Real Time Logic LLC, 2006-2008
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
/** @file IoIntfZipReader.h */

#ifndef __IoIntfZipReader_h
#define __IoIntfZipReader_h

#include <ZipFileIterator.h>
#include <IoIntf.h>

/** The IoIntfZipReader, which implements the abstract ZipReader
    interface, makes it possible for a ZipIo to open a ZIP file via
    another IoInterface.
@ingroup IO
 */
typedef struct IoIntfZipReader
#ifdef __cplusplus
: public ZipReader
{
      /** Uninitialized storage; call IoIntfZipReader_constructor before use
       * or destruction. */
      IoIntfZipReader() {}

   /** The IoIntfZipReader constructor opens the ZIP file for reading.
       \param io Borrowed I/O interface. Keep it alive until the reader closes.
       The interface must support seeking, or provide the seekAndRead property.
       \param pathName Required NUL-terminated ZIP file path relative to io;
       used during construction only. File size must fit U32 (no ZIP64).
       Check getECode and inherited CspReader::isValid before use. 
   */
   IoIntfZipReader(IoIntf* io, const char* pathName);
      
   /** Close any open resource, ignoring the close status.
       Destroy dependent ZipIo objects and stop reads before destruction.
    */
   ~IoIntfZipReader();

   /** Close and consume the underlying resource, even if closing fails.
       Do not use a dependent ZipIo after this call.
       @return Underlying close status (zero success, negative failure), or -1
       if no resource is open. Does not update getECode.
       The inherited isValid flag is not a current open/closed test.
    */
    int close();

   /** @return Last construction or read status (zero success, otherwise an
       I/O error). Short reads become IOINTF_IOERROR. Close errors and reads
       attempted after close are not recorded here.
    */
   int getECode();

   private:
#else
{
   ZipReader reader;
#endif
      U32 currentOffset;
      IoIntf_SeekAndRead seekAndReadFp;
      ResIntfPtr fp;
      int lastECode;
} IoIntfZipReader;

#ifdef __cplusplus
extern "C" {
#endif
/** Initialize and open a ZIP reader; see IoIntfZipReader::IoIntfZipReader.
 * @param[out] o Caller-owned object to initialize.
 * @param[in] io Borrowed seekable I/O interface, valid until close.
 * @param[in] pathName Required NUL-terminated path within io.
 * Inspect IoIntfZipReader_getECode before use. */
BA_API void IoIntfZipReader_constructor(IoIntfZipReader* o,
                                        IoIntf* io,
                                        const char* pathName);
/** Close the reader; see IoIntfZipReader::close for lifetime requirements.
 * @param[in,out] o Initialized reader, including one whose open failed.
 * @return Zero success, underlying negative close error, or -1 if not open. */
BA_API int IoIntfZipReader_close(IoIntfZipReader* o);
/** Close without freeing the object itself.
 * @param[in,out] o Initialized reader whose users have stopped.
 * The C++ destructor discards the close result; use close to inspect it. */
#define IoIntfZipReader_destructor(o) IoIntfZipReader_close(o);
/** Read the stored construction/read status.
 * @param[in] o Initialized reader.
 * @return Zero or the last I/O error; see IoIntfZipReader::getECode. */
#define IoIntfZipReader_getECode(o) (o)->lastECode
#ifdef __cplusplus
}
inline IoIntfZipReader::IoIntfZipReader(IoIntf* io, const char* pathName) {
   IoIntfZipReader_constructor(this, io, pathName); }
inline IoIntfZipReader::~IoIntfZipReader() {
   IoIntfZipReader_destructor(this); }
inline int IoIntfZipReader::close() {
   return IoIntfZipReader_close(this); }

inline int IoIntfZipReader::getECode() {
   return IoIntfZipReader_getECode(this); }
#endif


#endif
