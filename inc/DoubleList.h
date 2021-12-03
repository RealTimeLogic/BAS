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
 *   $Id: DoubleList.h 4915 2021-12-01 18:26:55Z wini $
 *
 *   COPYRIGHT:  Real Time Logic, 2004 - 2020
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
 *  DiskIo implements the abstract class IoIntf. See the reference
 *  manual for more information on the IoIntf (IO interface)
 *  requirements.
 *
 *  This is a generic header file for all file systems and
 *  platforms. See the sub-directories for platform specific
 *  implementations.
 */

#ifndef _DoubleList_h
#define _DoubleList_h

#include <TargConfig.h>

#ifndef DL_INLINE
#define DL_INLINE 1
#endif

struct DoubleList;

typedef struct DoubleLink
{
#ifdef __cplusplus
      void *operator new(size_t s) { return ::baMalloc(s); }
      void operator delete(void* d) { if(d) ::baFree(d); }
      void *operator new(size_t, void *place) { return place; }
      void operator delete(void*, void *) { }
       DoubleLink();
      ~DoubleLink();
      void insertAfter(DoubleLink* newLink);
      void insertBefore(DoubleLink* newLink);
      void unlink();
      bool isLinked();
      DoubleLink* getNext();
#endif
      struct DoubleLink* next;
      struct DoubleLink* prev;
} DoubleLink;



typedef struct DoubleList
{
#ifdef __cplusplus
      DoubleList();
      void insertFirst(DoubleLink* newLink);
      void insertLast(DoubleLink* newLink);
      bool isLast(DoubleLink* n);
      DoubleLink* firstNode();
      DoubleLink* lastNode();
      bool isEmpty();
      DoubleLink* removeFirst();
      bool isInList(DoubleLink* n);
#endif
   DoubleLink* next;
   DoubleLink* prev;
} DoubleList;

#if DL_INLINE
#define DoubleLink_constructor(o) do { \
   ((DoubleLink*)o)->next = 0; \
   ((DoubleLink*)o)->prev = 0; \
} while(0)


#define DoubleLink_destructor(o) do {           \
      if(DoubleLink_isLinked(o))                \
         DoubleLink_unlink((DoubleLink*)o);     \
} while(0)

#define DoubleLink_insertAfter(o, newLink) do { \
   baAssert(((DoubleLink*)newLink)->prev==0&&((DoubleLink*)newLink)->next==0);\
   ((DoubleLink*)newLink)->next = ((DoubleLink*)o)->next; \
   ((DoubleLink*)newLink)->prev = ((DoubleLink*)o); \
   ((DoubleLink*)o)->next->prev = ((DoubleLink*)newLink); \
   ((DoubleLink*)o)->next = ((DoubleLink*)newLink); \
} while(0)


#define DoubleLink_insertBefore(o, newLink) do { \
   baAssert(((DoubleLink*)newLink)->prev==0&&((DoubleLink*)newLink)->next==0);\
   ((DoubleLink*)newLink)->prev = ((DoubleLink*)o)->prev; \
   ((DoubleLink*)newLink)->next = ((DoubleLink*)o); \
   ((DoubleLink*)o)->prev->next = ((DoubleLink*) newLink); \
   ((DoubleLink*)o)->prev = ((DoubleLink*) newLink); \
} while(0)


#ifdef NDEBUG
#define DoubleLink_isLinked(o) \
   (((DoubleLink*)o)->prev ? TRUE : FALSE)
#else
#define DoubleLink_isLinked(o) \
   (((DoubleLink*)o)->prev ? (baAssert(((DoubleLink*)o)->next), TRUE) : FALSE)
#endif

#define DoubleLink_getNext(o) ((DoubleLink*)(o))->next

#define DoubleLink_unlink(o) do { \
   baAssert(((DoubleLink*)o)->prev && ((DoubleLink*)o)->next);\
   ((DoubleLink*) o)->next->prev = ((DoubleLink*)o)->prev; \
   ((DoubleLink*) o)->prev->next = ((DoubleLink*)o)->next; \
   ((DoubleLink*) o)->next = 0; \
   ((DoubleLink*) o)->prev = 0; \
} while(0)

#define DoubleList_constructor(o) do { \
   (o)->next = (DoubleLink*)o; \
   (o)->prev = (DoubleLink*)o; \
} while(0)


#define DoubleList_insertFirst(o, newLink) do { \
   baAssert(((DoubleLink*)newLink)->prev==0&&((DoubleLink*)newLink)->next==0);\
   ((DoubleLink*)newLink)->next = (o)->next; \
   ((DoubleLink*)newLink)->prev = (DoubleLink*)o; \
   (o)->next->prev = ((DoubleLink*) newLink); \
   (o)->next = ((DoubleLink*) newLink); \
} while(0)


#define DoubleList_insertLast(o, newLink) do { \
   baAssert(((DoubleLink*)newLink)->prev==0&&((DoubleLink*)newLink)->next==0);\
   ((DoubleLink*)newLink)->next = (DoubleLink*)o; \
   ((DoubleLink*)newLink)->prev = (o)->prev; \
   (o)->prev->next = ((DoubleLink*)newLink); \
   (o)->prev = ((DoubleLink*)newLink); \
} while(0)


#define DoubleList_isLast(o, n) (((DoubleLink*)(n))->next == (DoubleLink*)(o))
#define DoubleList_isEnd(o, n) ((DoubleLink*)(n) == (DoubleLink*)(o))

#define DoubleList_firstNode(o) \
   ((o)->next != (DoubleLink*)o ? (o)->next : 0)
#define DoubleList_lastNode(o) \
   ((o)->prev != (DoubleLink*)o ? (o)->prev : 0)

#endif /* DL_INLINE */

#define DoubleList_isEmpty(o)  \
   ((o)->next == ((DoubleLink*)(o)))


#ifdef __cplusplus
extern "C" {
#endif

BA_API DoubleLink* DoubleList_removeFirst(DoubleList* o);

/* Returns true if the node is in any list. You cannot
 * use this function for testing if the node is in a particular list, that
 * is your problem. The function performs some additional tests if
 * NDEBUG is not defined and asserts that if the node is in a list, it should
 * be in this list.
 */
#ifdef NDEBUG
#define DoubleList_isInList(o, node) (((DoubleLink*)node)->prev ? TRUE : FALSE)
#else
#define DoubleList_isInList(o, node) DoubleList_isInListF(o, node, __FILE__, __LINE__)
BA_API BaBool DoubleList_isInListF(DoubleList* o,void* node,const char* file,int line);
#endif

#if DL_INLINE == 0
BA_API void DoubleLink_constructor(void* o);
BA_API void DoubleLink_destructor(void* o);
BA_API void DoubleLink_insertAfter(void* o, void* newLink);
BA_API void DoubleLink_insertBefore(void* o, void* newLink);
BA_API int DoubleLink_isLinked(void* o);
BA_API DoubleLink* DoubleLink_getNext(void* o);
BA_API void DoubleLink_unlink(DoubleLink* o);
BA_API void DoubleList_constructor(DoubleList* o);
BA_API void DoubleList_insertFirst(DoubleList* o, void* newLink);
BA_API void DoubleList_insertLast(DoubleList* o, void* newLink);
BA_API int DoubleList_isLast(DoubleList* o, void* n);
BA_API int DoubleList_isEnd(DoubleList* o, void* n);
BA_API DoubleLink* DoubleList_firstNode(DoubleList* o);
BA_API DoubleLink* DoubleList_lastNode(DoubleList* o);
#endif /* DL_INLINE */


#ifdef __cplusplus
}
inline DoubleLink::DoubleLink() {
   DoubleLink_constructor(this);
}
inline DoubleLink::~DoubleLink() {
   DoubleLink_destructor(this);
}
inline void DoubleLink::insertAfter(DoubleLink* newLink) {
   DoubleLink_insertAfter(this, newLink);
}
inline void DoubleLink::insertBefore(DoubleLink* newLink) {
   DoubleLink_insertBefore(this, newLink);
}
inline void DoubleLink::unlink() {
   DoubleLink_unlink(this);
}
inline bool DoubleLink::isLinked() {
   return DoubleLink_isLinked(this) ? true : false;
}
inline DoubleLink* DoubleLink::getNext() {
   return DoubleLink_getNext(this);
}
inline DoubleList::DoubleList() {
   DoubleList_constructor(this);
}
inline void DoubleList::insertFirst(DoubleLink* newLink) {
   DoubleList_insertFirst(this, newLink);
}
inline void DoubleList::insertLast(DoubleLink* newLink) {
   DoubleList_insertLast(this, newLink);
}
inline bool DoubleList::isLast(DoubleLink* n) {
   return DoubleList_isLast(this, n) ? true : false;
}
inline  DoubleLink* DoubleList::firstNode() {
   return DoubleList_firstNode(this);
}
inline DoubleLink* DoubleList::lastNode() {
   return DoubleList_lastNode(this);
}
inline bool DoubleList::isEmpty() {
   return DoubleList_isEmpty(this) ? true : false;
}
inline DoubleLink* DoubleList::removeFirst() {
   return DoubleList_removeFirst(this);
}
inline bool DoubleList::isInList(DoubleLink* n) {
   return DoubleList_isInList(this, n)  ? true : false;
}
#endif




/*===========================================================================
 *
 *  Class:  DoubleListEnumerator
 *---------------------------------------------------------------------------
 *  Description:
 * Usage:
 *   DoubleListEnumerator e(list);
 *   for(DoubleLink* link = e.getElement() ; link ; link = e.nextElement())
 * or
 *   DoubleListEnumerator e(list);
 *   DoubleLink* link = e.getElement();
 *   while(link)
 *   {
 *      if(link bla bla)
 *         //Deletes current element and returns next element
 *         link = e.deleteElement();
 *      else
 *         link = e.nextElement();
 *   }
 */
typedef struct DoubleListEnumerator
{
#ifdef __cplusplus
      DoubleListEnumerator(){}
      DoubleListEnumerator(DoubleList* list);
      DoubleLink* getElement();
      DoubleLink* nextElement();
      DoubleLink* removeElement();
   private:
#endif
      DoubleList* list;
      DoubleLink* curElement;
} DoubleListEnumerator;

#define DoubleListEnumerator_getElement(o) (o)->curElement

#if DL_INLINE
#define DoubleListEnumerator_constructor(o, listMA) do \
{ \
   (o)->list = listMA; \
   (o)->curElement = DoubleList_firstNode((o)->list);\
} while(0)

#define DoubleListEnumerator_nextElement(o) \
   ((o)->curElement ? ( \
      (o)->curElement = (o)->curElement->next == (DoubleLink*)(o)->list ? 0 : (o)->curElement->next, \
      (o)->curElement \
    ) : 0)
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if DL_INLINE == 0
BA_API void DoubleListEnumerator_constructor(DoubleListEnumerator* o, DoubleList* list);
BA_API DoubleLink* DoubleListEnumerator_nextElement(DoubleListEnumerator* o);
#endif


BA_API DoubleLink* DoubleListEnumerator_removeElement(DoubleListEnumerator* o);
#ifdef __cplusplus
}
inline DoubleListEnumerator::DoubleListEnumerator(DoubleList* list) {
   DoubleListEnumerator_constructor(this, list); }
inline DoubleLink*
DoubleListEnumerator::removeElement() {
   return DoubleListEnumerator_removeElement(this); }
inline DoubleLink*
DoubleListEnumerator::getElement() {return DoubleListEnumerator_getElement(this);}
inline DoubleLink*
DoubleListEnumerator::nextElement() {return DoubleListEnumerator_nextElement(this); }
#endif


#endif
