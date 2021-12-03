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
 *   $Id: IoIntfZipReader.h 4915 2021-12-01 18:26:55Z wini $
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
      IoIntfZipReader() {}

   /** The IoIntfZipReader constructor opens the Zip-File for reading.
       \param io the I/O-interface to read from.
       \param pathName the path+name to a ZIP file in the I/O-interface. 
   */
   IoIntfZipReader(IoIntf* io, const char* pathName);
      
   /** The destructor closes the file connection.
    */
   ~IoIntfZipReader();

   /** Close the file connection.
    */
    int close();

   /** Returns the last error code, if any.
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
BA_API void IoIntfZipReader_constructor(IoIntfZipReader* o,
                                        IoIntf* io,
                                        const char* pathName);
BA_API int IoIntfZipReader_close(IoIntfZipReader* o);
#define IoIntfZipReader_destructor(o) IoIntfZipReader_close(o);
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
