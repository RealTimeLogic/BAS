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
 *                            HEADER
 *
 *   $Id: RecIoIter.h 4915 2021-12-01 18:26:55Z wini $
 *
 *   COPYRIGHT:  Real Time Logic, 2008
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
 *
 */
#ifndef _RecIoIter_h
#define _RecIoIter_h

#include <IoIntf.h>
#include <BaServerLib.h>

struct RecIoIter;

typedef int (*RecIoIter_OnResponse)(
   struct RecIoIter* o, const char* from, const char* to, IoStat* st);
typedef int (*RecIoIter_OnErr)(
   struct RecIoIter* o, const char* name, int err, const char* errMsg);

typedef struct RecIoIter
{
      IoStat st;
      AllocatorIntf* alloc;
      IoIntf* io;
      RecIoIter_OnResponse onResponseFp;
      RecIoIter_OnErr onErrFp;
      int recCounter;
} RecIoIter;

#define RecIoIter_constructor( \
o, allocMA, ioMA, onResponseFpMA, onErrFpMA) do { \
   (o)->alloc=allocMA; \
   (o)->io=ioMA; \
   (o)->onResponseFp=onResponseFpMA; \
   (o)->onErrFp=onErrFpMA; \
   (o)->recCounter=0; \
} while(0)   


BA_API int RecIoIter_doResource(RecIoIter* o,const char* from,const char* to);

#endif
