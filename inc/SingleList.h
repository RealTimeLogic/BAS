/*
 *     ____             _________                __                _     
 *    / __ \___  ____ _/ /_  __(_)___ ___  ___  / /   ____  ____ _(_)____
 *   / /_/ / _ \/ __ `/ / / / / / __ `__ \/ _ \/ /   / __ \/ __ `/ / ___/
 *  / _, _/  __/ /_/ / / / / / / / / / / /  __/ /___/ /_/ / /_/ / / /__  
 * /_/ |_|\___/\__,_/_/ /_/ /_/_/ /_/ /_/\___/_____/\____/\__, /_/\___/  
 *                                                       /____/          
 ****************************************************************************
 *                            HEADER
 *
 *   $Id: SingleList.h 4915 2021-12-01 18:26:55Z wini $
 *
 *   COPYRIGHT:  Real Time Logic, 2002
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

        CONTENTS
        --------

        1  Description
        2  History of developmentDoubleLink
        3  Macros
        4  Include files
        5  types Constants Variables
        6  Function prototypes

 ****************************************************************************
 */
#ifndef _SingleList_h
#define _SingleList_h

/*
 ****************************************************************************
 * 1  DESCRIPTION.
 ****************************************************************************
 *
 */

/*
 ****************************************************************************
 *
 */

/*
 ****************************************************************************
 * 3  MACROS.
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 4  INCLUDE FILES.
 ****************************************************************************
 */
#include <TargConfig.h>


/*
 ****************************************************************************
 * 5  TYPES CONSTANTS VARIABLES
 ****************************************************************************
 */

/*Forward declarations*/
struct SingleList; 
struct SingleListEnumerator;

/*===========================================================================
 *
 *  Class:  SingleLink
 *---------------------------------------------------------------------------
 *  Description:
 *  Contains the link chain for the next element in the SingleList. Subclass
 *  o link for data stored in the SingleList.
 *  Note, o class contains no virtual destructor and the subclassed node
 *  will not be informed when the node is deleted.
 */
typedef struct SingleLink
{
#ifdef __cplusplus
      SingleLink();
      SingleLink* getNext();
   private:
      friend struct SingleList;
      friend struct SingleListEnumerator;
#endif
      struct SingleLink* next;
} SingleLink;
#define SingleLink_constructor(o) ((SingleLink*)(o))->next = 0
#define SingleLink_getNext(o) ((SingleLink*)(o))->next
#define SingleLink_isLinked(o) \
   (((SingleLink*)o)->next ? TRUE : FALSE) 

#ifdef __cplusplus
inline SingleLink::SingleLink() {SingleLink_constructor(this);}
inline SingleLink* SingleLink::getNext() {return SingleLink_getNext(this);}
#endif

/*===========================================================================
 *
 *  Class:  SingleList
 *---------------------------------------------------------------------------
 *  Description:
 *   Contains nodes of type SingleLink.
 */
typedef struct SingleList
{
#ifdef __cplusplus
      SingleList();
      void insertLast(SingleLink* link);
      SingleLink* removeFirst();
      SingleLink* peekFirst();
      bool isEmpty();
      bool isLast(SingleLink* link);
   private:
      friend struct SingleListEnumerator;
#endif
      SingleLink link;   
      SingleLink* last;
} SingleList;

#define SingleList_insertLast(o, linkMA) do \
{ \
   baAssert((SingleLink*)(linkMA) != (SingleLink*)(o)); \
   baAssert(((SingleLink*)(linkMA))->next == 0); \
   (o)->last->next = (SingleLink*)(linkMA); \
   (o)->last = (SingleLink*)(linkMA); \
   ((SingleLink*)(linkMA))->next = (SingleLink*)(o); \
} while(0)

#define SingleList_peekFirst(o) ((o)->link.next == (SingleLink*)(o) ? 0 : (o)->link.next)

#define SingleList_isEmpty(o) ((o)->link.next == (SingleLink*)(o))
#define SingleList_isLast(o, n) ((n)->next == (SingleLink*)(o))

#ifdef __cplusplus
extern "C" {
#endif
BA_API void SingleList_constructor(SingleList* o);
BA_API SingleLink* SingleList_removeFirst(SingleList* o);
#ifdef __cplusplus
}
inline
SingleList::SingleList() { SingleList_constructor(this); }
inline void
SingleList::insertLast(SingleLink* link) { SingleList_insertLast(this, link); }
inline SingleLink*
SingleList::peekFirst() { return SingleList_peekFirst(this); }
inline bool
SingleList::isEmpty() { return SingleList_isEmpty(this); }
inline bool
SingleList::isLast(SingleLink* link) { return SingleList_isLast(this,link); }
inline SingleLink*
SingleList::removeFirst() { return SingleList_removeFirst(this); }
#endif

/*===========================================================================
 *
 *  Class:  SingleListEnumerator
 *---------------------------------------------------------------------------
 *  Description:
 * Usage:
 *   SingleListEnumerator e(list);
 *   for(Slink* link = e.getElement() ; link ; link = e.nextElement())
 * or
 *   SingleListEnumerator e(list);
 *   Slink* link = e.getElement();
 *   while(link)
 *   {
 *      if(link bla bla)
 *         //Deletes current element and returns next element
 *         link = e.deleteElement(); 
 *      else
 *         link = e.nextElement();
 *   }
 */
typedef struct SingleListEnumerator
{
#ifdef __cplusplus
      SingleListEnumerator(){}
      SingleListEnumerator(SingleList* list);
      SingleLink* getElement();
      SingleLink* nextElement();
      SingleLink* removeElement();
      int insertBefore(SingleLink* l);
   private:
#endif
      SingleList* list;
      SingleLink* prevElement;
      SingleLink* curElement;
} SingleListEnumerator;

#define SingleListEnumerator_constructor(o, listMA) do \
{ \
   (o)->list = listMA; \
   (o)->prevElement = (SingleLink*)listMA; \
   (o)->curElement = SingleList_isEmpty(listMA) ? 0 : (o)->list->link.next; \
} while(0)

#define SingleListEnumerator_getElement(o) (o)->curElement

#define SingleListEnumerator_nextElement(o) \
   ((o)->curElement ? ( \
      (o)->prevElement = (o)->prevElement->next, \
      (o)->curElement = (o)->curElement == (o)->list->last ? 0 : (o)->curElement->next, \
      (o)->curElement \
    ) : 0)

#ifdef __cplusplus
extern "C" {
#endif
BA_API int SingleListEnumerator_insertBefore(
   SingleListEnumerator*, SingleLink*);
BA_API SingleLink* SingleListEnumerator_removeElement(SingleListEnumerator* o);
#ifdef __cplusplus
}
inline SingleListEnumerator::SingleListEnumerator(SingleList* list) {
   SingleListEnumerator_constructor(this, list); }
inline SingleLink*
SingleListEnumerator::removeElement() {
   return SingleListEnumerator_removeElement(this); }
inline SingleLink*
SingleListEnumerator::getElement() {return SingleListEnumerator_getElement(this);}
inline SingleLink*
SingleListEnumerator::nextElement() {return SingleListEnumerator_nextElement(this); }
inline int SingleListEnumerator::insertBefore(SingleLink* l) {
   return SingleListEnumerator_insertBefore(this, l); }
#endif


#endif /*_SingleList_h*/


#if defined(SingleListCode) && ! defined(SingleListCodeIncluded)
#define SingleListCodeIncluded

BA_API void
SingleList_constructor(SingleList* o)
{
   SingleLink_constructor((SingleLink*)o);
   o->last = ((SingleLink*)o);
   o->last->next = ((SingleLink*)o);
   o->link.next = ((SingleLink*)o);
}


BA_API int
SingleListEnumerator_insertBefore(SingleListEnumerator* o, SingleLink* l)
{
   if(l->next)
      return -1;

   if(SingleList_isEmpty(o->list))
      SingleList_insertLast(o->list, l);
   else
   {
      l->next = o->prevElement->next;
      o->prevElement->next = l;
   }
   o->prevElement = l;
   return 0;
}


BA_API SingleLink*
SingleListEnumerator_removeElement(SingleListEnumerator* o)
{
   if(o->curElement)
   {
      /*Store current position and iterate iterator*/
      SingleLink* cur = o->curElement;
      /*If element to remove is last element in list*/
      if(cur == o->list->last)
      {
         o->curElement = 0;
         o->list->last = o->prevElement;
         o->prevElement->next = (SingleLink*)o->list;
      }
      else
      {
         o->curElement =  o->curElement->next;
         o->prevElement->next = o->curElement;
      }
      cur->next = 0;
   }
   return o->curElement;
}


BA_API SingleLink*
SingleList_removeFirst(SingleList* o)
{
   SingleLink* link2Remove;
   link2Remove = o->link.next;
   if(o->link.next == o->last)
   {
      if(o->link.next == (SingleLink*)o)
         return 0;
      o->link.next = o->last = (SingleLink*)o;
   }
   else
      o->link.next = o->link.next->next;
   link2Remove->next = 0;
   return link2Remove;
}

#endif /* SingleListCode */
