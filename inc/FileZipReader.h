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
 *   $Id: FileZipReader.h 4914 2021-12-01 18:24:30Z wini $
 *
 *   COPYRIGHT:  Real Time Logic, 2003
 *               http://www.realtimelogic.com
 *
 *   The copyright to the program herein is the property of
 *   Real Time Logic. The program may be used or copied only
 *   with the written permission from Real Time Logic or
 *   in accordance with the terms and conditions stipulated in
 *   the agreement under which the program has been supplied.
 ****************************************************************************
 *
 *
 */
#ifndef __FileZipReader_h
#define __FileZipReader_h

#include <ZipFileIterator.h>
#include <BaFile.h>

/** Example code that shows you how to write a ZipReader interface for
the ZipIo class.
@ingroup IO

This example code shows you how to write a ZipReader driver object.
The ZipIo class uses the ZipReader driver object when reading Zip-data
from a Zip-File. See class ZipIo for more information.

This example code uses the file system for reading the Zip-File.
The constructor opens the Zip-File and method "diskRead", see code,  uses
the Posix function fseek for setting the file pointer to the requested
offset position and function fread for reading the actual data.

 */
typedef struct FileZipReader
#ifdef __cplusplus
: public ZipReader
{
   /** The ZipReader constructor opens the Zip-File for reading.
       You must call CspReader::isValid, which informs you if the constructor
       successfully opened the Zip-File for reading.
       \param pathName is the path and name to the Zip-File.
   */
   FileZipReader(const char* pathName);
      
   /** The destructor closes the file connection.
    */
   ~FileZipReader();

   private:
#else
{
   ZipReader reader;
#endif
      U32 currentOffset;
      ResIntfPtr fp;
} FileZipReader;

#ifdef __cplusplus
extern "C" {
#endif
void FileZipReader_constructor(FileZipReader* o,
                                     const char* pathName);
void FileZipReader_destructor(FileZipReader* o);
#ifdef __cplusplus
}
inline FileZipReader::FileZipReader(const char* pathName) {
   FileZipReader_constructor(this, pathName); }
inline FileZipReader::~FileZipReader() {
   FileZipReader_destructor(this); }
#endif


#endif
