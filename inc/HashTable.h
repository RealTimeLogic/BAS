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
 *   $Id: HashTable.h 4915 2021-12-01 18:26:55Z wini $
 *
 *   COPYRIGHT:  Real Time Logic LLC, 2010
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
 */
#ifndef _HashTable_h
#define _HashTable_h

#include "SingleList.h"
#include "BaServerLib.h"

struct HashTableNode;

typedef void (*HashTableNode_terminate)(struct HashTableNode*, void* tmobj);

typedef struct HashTableNode
{
#ifdef __cplusplus
      HashTableNode(const char* name, HashTableNode_terminate terminate);
#endif
      SingleLink super;
      HashTableNode_terminate destructor;
      const char* name;
} HashTableNode;

#ifdef __cplusplus
extern "C" {
#endif
BA_API void HashTableNode_constructor(HashTableNode* o,
                               const char* name,
                               HashTableNode_terminate destructor);
#define HashTableNode_terminate(o, tmObj) (*o->destructor)(o, tmObj)
#ifdef __cplusplus
}
inline HashTableNode::HashTableNode(const char* name,
                                    HashTableNode_terminate destructor) {
   HashTableNode_constructor(this,name,destructor);
}
#endif

typedef int (*HashTable_CbFunc)(void* cbObj, struct HashTableNode*);

typedef struct HashTable
{
#ifdef __cplusplus
      static HashTable* create(U32 noOfHashElements,AllocatorIntf* alloctr=0);
      ~HashTable();
      void add(HashTableNode* node);
      HashTableNode* lookup(const char* ident);
      void setTmObj(void* obj);
#endif
      void* tmObj;
      U32 noOfHashElements;
      SingleList table[1]; /*SingleList contain HashTableNode(s)*/
} HashTable;

#ifdef __cplusplus
extern "C" {
#endif
BA_API HashTable* HashTable_create(U32 noOfHashElements, AllocatorIntf* alloc);
BA_API void HashTable_destructor(HashTable* o);
BA_API void HashTable_add(HashTable* o, HashTableNode* node);
#define HashTable_setTmObj(o, tmObjMA) (o)->tmObj=tmObjMA
BA_API HashTableNode* HashTable_lookup(HashTable* o, const char* ident);
BA_API int HashTable_iter(HashTable* o, void* cbObj, HashTable_CbFunc cbFunc);
#ifdef __cplusplus
}
inline HashTable* HashTable::create(U32 noOfHashElements, AllocatorIntf* alloc)
{
   return HashTable_create(noOfHashElements, alloc);
}
inline HashTable::~HashTable() {
   HashTable_destructor(this);
}
inline void HashTable::add(HashTableNode* node) {
   HashTable_add(this, node);
}
inline HashTableNode* HashTable::lookup(const char* ident) {
   return HashTable_lookup(this, ident);
}

inline void HashTable::setTmObj(void* obj) {
   HashTable_setTmObj(this, obj);
}

#endif


#endif
