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
 *   $Id: DeadProp.h 4915 2021-12-01 18:26:55Z wini $
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
#ifndef _DeadProp_h
#define _DeadProp_h

#include <SimpleXml.h>

typedef struct DeadProp
{
      SXmlRoot op;
      SXmlRoot* nr;
      U8* xmlRootBuf;
      SXmlNode* oprop;
      SXmlNode** nprops;
      SXmlNode* oPropIter; /* Used by getFirstPropName & getNextPropName */
      IoIntf* io;
      AllocatorIntf* alloc;
      char* propFileName;
      U16 npropsCursor;
      U16 npropsLen;
      BaBool noPropFile;
      BaBool elemRemoved;
} DeadProp;


void DeadProp_constructor(DeadProp* o, IoIntf* io, AllocatorIntf* alloc);
char* DeadProp_fname2PropFName(AllocatorIntf* alloc, const char* filename);
int DeadProp_mkPropDir(IoIntf* io, char* propFileName);
U8* DeadProp_readFile(IoIntf* io, AllocatorIntf* alloc,
                      const char* name, size_t* size);
int DeadProp_setFile(DeadProp* o, char* propFileName);
int DeadProp_save(DeadProp* o);
SXmlNode* DeadProp_getProp(DeadProp* o, const char* ns, const char* name);
int DeadProp_setProp(DeadProp* o, SXmlNode* nn, SXmlRoot* nr);
int DeadProp_removeProp(DeadProp* o, SXmlNode* nn, SXmlRoot* nr);
SXmlNode* DeadProp_getFirstProp(DeadProp* o);
SXmlNode* DeadProp_getNextProp(DeadProp* o);
void DeadProp_destructor(DeadProp* o);
#define DeadProp_getRoot(o) (&(o)->op)

#endif
