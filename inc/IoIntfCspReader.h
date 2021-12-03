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
 *   $Id: IoIntfCspReader.h 4915 2021-12-01 18:26:55Z wini $
 *
 *   COPYRIGHT:  Real Time Logic LLC, 2008
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
#ifndef __IoIntfCspReader_h
#define __IoIntfCspReader_h

#include <CspRunTm.h>
#include <IoIntf.h>

/** The IoIntfCspReader, which implements the abstract CspReader
    interface, makes it possible to open a "CSP dat" file via a IoIntf.
@ingroup IO
 */
typedef struct IoIntfCspReader
#ifdef __cplusplus
: public CspReader
{
      IoIntfCspReader() {}

   /** The IoIntfCspReader constructor opens the Zip-File for reading.
       \param io the I/O-interface to read from.
       \param pathName the path+name to the "CSP dat" file. 
   */
   IoIntfCspReader(IoIntf* io, const char* pathName);
      
   /** The destructor closes the file connection.
    */
   ~IoIntfCspReader();

   /** Close the file connection.
    */
    int close();

   private:
#else
{
   CspReader reader;
#endif
      U32 currentOffset;
      IoIntf_SeekAndRead seekAndReadFp;
      ResIntfPtr fp;
} IoIntfCspReader;

#ifdef __cplusplus
extern "C" {
#endif
BA_API void IoIntfCspReader_constructor(IoIntfCspReader* o,
                                        IoIntf* io,
                                        const char* pathName);
BA_API int IoIntfCspReader_close(IoIntfCspReader* o);
#define IoIntfCspReader_destructor(o) IoIntfCspReader_close(o);
#ifdef __cplusplus
}
inline IoIntfCspReader::IoIntfCspReader(IoIntf* io, const char* pathName) {
   IoIntfCspReader_constructor(this, io, pathName); }
inline IoIntfCspReader::~IoIntfCspReader() {
   IoIntfCspReader_destructor(this); }
inline int IoIntfCspReader::close() {
   return IoIntfCspReader_close(this); }
#endif


#endif
