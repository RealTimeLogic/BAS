/*
 *     ____             _________                __                _     
 *    / __ \___  ____ _/ /_  __(_)___ ___  ___  / /   ____  ____ _(_)____
 *   / /_/ / _ \/ __ `/ / / / / / __ `__ \/ _ \/ /   / __ \/ __ `/ / ___/
 *  / _, _/  __/ /_/ / / / / / / / / / / /  __/ /___/ /_/ / /_/ / / /__  
 * /_/ |_|\___/\__,_/_/ /_/ /_/_/ /_/ /_/\___/_____/\____/\__, /_/\___/  
 *                                                       /____/          
 *
 *                  Barracuda Embedded Web-Server 
 ****************************************************************************
 *            HEADER
 *
 *   $Id: SplayTree.h 4915 2021-12-01 18:26:55Z wini $
 *
 *   COPYRIGHT:  Real Time Logic, 2004
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
 */

#ifndef __SplayTree_h
#define __SplayTree_h

#include <TargConfig.h>

typedef const void* SplayTreeKey;

typedef struct SplayTreeNode
{
#ifdef __cplusplus
      void *operator new(size_t s) { return ::baMalloc(s); }
      void operator delete(void* d) { if(d) ::baFree(d); }
      void *operator new(size_t, void *place) { return place; }
      void operator delete(void*, void *) { }
      SplayTreeNode(){} /* Dummy constructor */
      SplayTreeNode(SplayTreeKey key);
      SplayTreeKey getKey();
#endif
      struct SplayTreeNode* left;
      struct SplayTreeNode* right;
      SplayTreeKey key;
} SplayTreeNode;

#ifdef __cplusplus
extern "C" {
#endif
BA_API void SplayTreeNode_constructor(SplayTreeNode* o, SplayTreeKey key);
#define SplayTreeNode_getKey(o) (o)->key

#ifdef __cplusplus
}
inline SplayTreeNode::SplayTreeNode(SplayTreeKey key) {
   SplayTreeNode_constructor(this, key); }
inline SplayTreeKey SplayTreeNode::getKey() {
   return SplayTreeNode_getKey(this); }
#endif

/** Compares k and n, returning negative value if k<n, zero if k==n,
 * positive value if k>n.
 */
typedef int (*SplayTree_Compare)(SplayTreeNode* n, SplayTreeKey k);
 
typedef int (*SplayTree_Iter)(void* o, SplayTreeNode* n);


typedef struct SplayTree
{
#ifdef __cplusplus
      SplayTree(){} /* Dummy constructor */
      SplayTree(SplayTree_Compare compare);
      int insert(SplayTreeNode* n);
      SplayTreeNode* find(SplayTreeKey key);
      int remove(SplayTreeNode* n);
      SplayTreeNode* getRoot();
   private:
#endif
      SplayTreeNode* root;
      SplayTree_Compare compare;
} SplayTree;

#ifdef __cplusplus
extern "C" {
#endif
#define SplayTree_constructor(o, compareCB) do { \
   (o)->compare = compareCB; \
   (o)->root = 0; \
} while(0)
BA_API int SplayTree_insert(SplayTree* o, SplayTreeNode* n);
BA_API SplayTreeNode* SplayTree_find(SplayTree* o, SplayTreeKey key);
BA_API int SplayTree_remove(SplayTree* o, SplayTreeNode* n);
#define SplayTree_getRoot(o) (o)->root
BA_API int SplayTree_iterate(SplayTree* o, void* userObj, SplayTree_Iter i);
#ifdef __cplusplus
}
inline SplayTree::SplayTree(SplayTree_Compare compare) {
   SplayTree_constructor(this, compare); }
inline int SplayTree::insert(SplayTreeNode* n) {
   return SplayTree_insert(this, n); }
inline SplayTreeNode* SplayTree::find(SplayTreeKey key) {
   return SplayTree_find(this, key); }
inline int SplayTree::remove(SplayTreeNode* n) {
   return SplayTree_remove(this, n); }
inline SplayTreeNode* SplayTree::getRoot() {
   return SplayTree_getRoot(this); }
#endif


#endif
