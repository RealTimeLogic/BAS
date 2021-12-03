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
 *   $Id: VirDir.h 4915 2021-12-01 18:26:55Z wini $
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
 
#ifndef __VirDir_h
#define __VirDir_h 

#include <BaServerLib.h>

struct VirFileNode;

typedef struct VirDirNode
{
      struct VirDirNode* next;
      struct VirDirNode* subDir;
      struct VirFileNode* firstFile;
      char name[1];
} VirDirNode;


typedef struct VirFileNode
{
      struct VirFileNode* next;
      const char* name;
} VirFileNode;


typedef enum { VirDir_NotFound=0, VirDir_IsDir, VirDir_IsFile } VirDir_Type;
typedef void (*VirFileNode_Free)(VirFileNode* o, AllocatorIntf* alloc);
void VirFileNode_constructor(VirFileNode* o, const char* name);
void VirDirNode_constructor(VirDirNode* o, const char* name, size_t len);
VirDir_Type VirDirNode_find(VirDirNode* o, const char* relPath, void** retVal);
VirDirNode* VirDirNode_findSubDir(VirDirNode* o, const char* name, size_t len);
VirFileNode* VirDirNode_findFile(VirDirNode* o, const char* name);
VirDirNode* VirDirNode_makeDir(VirDirNode* o, const char* pathName,
                               AllocatorIntf* alloc);
int VirDirNode_mkDirInsertFile(VirDirNode* o, const char* pathName,
                               VirFileNode* vfn, AllocatorIntf* alloc);

void VirDirNode_free(
   VirDirNode* o,AllocatorIntf* alloc,VirFileNode_Free freeNode);

#endif
