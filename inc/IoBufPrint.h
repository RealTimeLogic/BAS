
/*     ____             _________                __                _     
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
 *   $Id: IoBufPrint.h 5978 2026-09-11 16:13:48Z wini $
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
 *
 */
/** @file IoBufPrint.h */
#ifndef _IoBufPrint_h
#define _IoBufPrint_h 

#include <BufPrint.h>
#include <IoIntf.h>
#include <BaServerLib.h>
 
/** BufPrint adapter for a writable ResIntf.
    Write through the embedded BufPrint and explicitly flush/check its return
    before destruction when output errors must be observed. Flush consumes its
    buffered bytes even when the resource write fails; no retry buffer remains.
 */
typedef struct IoBufPrint
{
      BufPrint super;
      IoIntf* io;
      ResIntfPtr res;
} IoBufPrint;


/** Open a file for writing and allocate a buffered writer.
    @param alloc Required allocator, retained by the caller for final release.
    @param io Required borrowed writable I/O, kept alive through destruction.
    @param bufSize Requested buffer bytes, positive and at most 65535; allocation
    size arithmetic must not overflow. Allocator-adjusted capacity must also fit U16.
    @param name Required NUL-terminated I/O path, used during this call.
    @return Allocated writer, or NULL for open/allocation failure. OpenRes_WRITE
    can create/truncate the target before a later allocation failure. On success
    the adapter closes the opened resource during destruction. After destruction,
    release the adapter with the same allocator; destruction does not free it.
 */
BA_API IoBufPrint* IoBufPrint_create(AllocatorIntf* alloc,
                              IoIntf* io,
                              size_t bufSize,
                              const char* name);
/** Allocate a writer for an already open resource.
    @param alloc Required allocator; caller retains it for final release.
    @param bufSize Positive byte capacity, at most 65535, including any allocator
    size adjustment. sizeof(IoBufPrint)+bufSize must be representable.
    @param out Required borrowed writable resource with writeFp installed.
    @return Allocated writer or NULL on allocation failure. This variant never
    closes out; the caller closes it after flushing/destroying the adapter.
 */
BA_API IoBufPrint* IoBufPrint_create2(AllocatorIntf* alloc,
                               size_t bufSize,
                               ResIntfPtr out);
/** Flush and release an adapter's owned resource.
    @param o Required live adapter, consumed for further writing. An adapter
    from create closes its resource; one from create2 leaves it open. Flush and
    close errors are discarded. Release o separately through its allocator.
 */
BA_API void IoBufPrint_destructor(IoBufPrint* o);

#endif
