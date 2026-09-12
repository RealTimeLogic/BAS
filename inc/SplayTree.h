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
 *   $Id: SplayTree.h 5978 2026-09-11 16:13:48Z wini $
 *
 *   COPYRIGHT:  Real Time Logic, 2004 - 2026
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

/** @file SplayTree.h */

#ifndef __SplayTree_h
#define __SplayTree_h

#include <TargConfig.h>

/** Borrowed key interpreted by SplayTree_Compare. Its representation and
 * NULL handling are defined by the comparison callback. */
typedef const void* SplayTreeKey;

/** Intrusive tree node. The caller owns the node and key and must keep both
 * valid while the node is in a tree. Initialize before insertion and do not
 * change its key while it is linked. */
typedef struct SplayTreeNode
{
#ifdef __cplusplus
      void *operator new(size_t s) { return ::baMalloc(s); }
      void operator delete(void* d) { if(d) ::baFree(d); }
      void *operator new(size_t, void *place) { return place; }
      void operator delete(void*, void *) { }
      /** Leave storage uninitialized; call SplayTreeNode_constructor before use. */
      SplayTreeNode(){} /* Dummy constructor */
      /** Initialize an unlinked node.
       * @param[in] key Borrowed key retained by this node. */
      SplayTreeNode(SplayTreeKey key);
      /** @return The borrowed key supplied at initialization. */
      SplayTreeKey getKey();
#endif
      struct SplayTreeNode* left;
      struct SplayTreeNode* right;
      SplayTreeKey key;
} SplayTreeNode;

#ifdef __cplusplus
extern "C" {
#endif
/** Initialize a node without allocating storage.
 * @param[out] o Caller-owned node to initialize; must not be linked in a tree.
 * @param[in] key Borrowed key stored without copying. */
BA_API void SplayTreeNode_constructor(SplayTreeNode* o, SplayTreeKey key);
/** Read a node's key.
 * @param[in] o Initialized node.
 * @return The borrowed key. */
#define SplayTreeNode_getKey(o) (o)->key

#ifdef __cplusplus
}
inline SplayTreeNode::SplayTreeNode(SplayTreeKey key) {
   SplayTreeNode_constructor(this, key); }
inline SplayTreeKey SplayTreeNode::getKey() {
   return SplayTreeNode_getKey(this); }
#endif

/** Compare a search key with a node's key.
 * @param[in] n Existing node; do not modify its links or key.
 * @param[in] k Search or insertion key.
 * @return Negative if k is less than n's key, zero if equal, positive if
 * greater. Use one consistent ordering for every operation on the tree. */
typedef int (*SplayTree_Compare)(SplayTreeNode* n, SplayTreeKey k);

/** Visit a node during SplayTree_iterate.
 * @param[in] o The userObj pointer passed to SplayTree_iterate; may be NULL.
 * @param[in] n Current node. Do not insert, remove, find or otherwise reshape
 * the tree from this callback, or free a node still being visited.
 * @return Zero to continue; any nonzero value stops traversal. */
typedef int (*SplayTree_Iter)(void* o, SplayTreeNode* n);

/** Self-adjusting tree of caller-owned nodes, with unique keys.
 * Searches, including unsuccessful searches, may change the root and links.
 * The tree does not allocate or free nodes or keys. */
typedef struct SplayTree
{
#ifdef __cplusplus
      /** Leave storage uninitialized; call SplayTree_constructor before use. */
      SplayTree(){} /* Dummy constructor */
      /** Initialize an empty tree.
       * @param[in] compare Required comparison callback retained by the tree. */
      SplayTree(SplayTree_Compare compare);
      /** Insert a node without allocating memory.
       * @param[in,out] n Initialized, unlinked node.
       * @return Zero on insertion, -1 if the key already exists. */
      int insert(SplayTreeNode* n);
      /** Find a node and adjust the tree.
       * @param[in] key Search key interpreted by the comparison callback.
       * @return Borrowed matching node, or NULL if no match exists. */
      SplayTreeNode* find(SplayTreeKey key);
      /** Unlink a node without freeing it.
       * @param[in,out] n Node to remove, identified by both key and pointer.
       * @return Zero on removal, -1 if this node is not in the tree. */
      int remove(SplayTreeNode* n);
      /** @return Borrowed current root, or NULL for an empty tree. */
      SplayTreeNode* getRoot();
   private:
#endif
      SplayTreeNode* root;
      SplayTree_Compare compare;
} SplayTree;

#ifdef __cplusplus
extern "C" {
#endif
/** Initialize an empty tree without allocating storage.
 * @param[out] o Caller-owned tree object. Existing linked nodes are not freed.
 * @param[in] compareCB Required callback used to compare keys. */
#define SplayTree_constructor(o, compareCB) do { \
   (o)->compare = compareCB; \
   (o)->root = 0; \
} while(0)
/** Insert a node.
 * @param[in,out] o Initialized tree.
 * @param[in,out] n Initialized, unlinked node whose key must remain valid.
 * @return Zero on insertion, -1 if an equal key exists. No storage is freed. */
BA_API int SplayTree_insert(SplayTree* o, SplayTreeNode* n);
/** Search and adjust the tree, even when no matching key is found.
 * @param[in,out] o Initialized tree.
 * @param[in] key Search key interpreted by the comparison callback.
 * @return Borrowed matching node, or NULL if no match exists. */
BA_API SplayTreeNode* SplayTree_find(SplayTree* o, SplayTreeKey key);
/** Unlink a node and clear its left and right links.
 * @param[in,out] o Initialized tree.
 * @param[in,out] n Node to remove. A different node with an equal key is not
 * removed. The caller retains ownership of n and its key.
 * @return Zero on removal, -1 if the specified node is not in the tree. */
BA_API int SplayTree_remove(SplayTree* o, SplayTreeNode* n);
/** Read the current root without reshaping the tree.
 * @param[in] o Initialized tree.
 * @return Borrowed root node, or NULL for an empty tree. */
#define SplayTree_getRoot(o) (o)->root
/** Visit nodes in root-left-right order, not sorted key order.
 * @param[in] o Initialized tree. Do not reshape it during traversal.
 * @param[in] userObj Opaque callback context; may be NULL.
 * @param[in] i Required callback invoked once for each visited node.
 * @return Zero when all nodes were visited (including an empty tree), or -1
 * when the callback stopped traversal. Its particular nonzero value is not
 * propagated. Traversal uses recursion proportional to the tree depth. */
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
