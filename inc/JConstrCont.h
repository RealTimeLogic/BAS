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
 *   $Id: JConstrCont.h 4915 2021-12-01 18:26:55Z wini $
 *
 *   COPYRIGHT:  Real Time Logic LLC, 2007
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

#ifndef __JConstrCont_h
#define __JConstrCont_h

#include "JUserCont.h"
#include "BaServerLib.h"
#include "HashTable.h"

/** Constraints Container that accepts JSON encoded data.
 */
typedef struct JConstrCont
#ifdef __cplusplus
   : public AuthorizerIntf 
{
      JConstrCont(JUserCont* userCont, AllocatorIntf* alloc=0);
      ~JConstrCont();
      void setConstraints(JVal* constraintsVal, JErr* err);

      /** A system with a case insensitive file system must call this
          method after running the constructor. Users may bypass
          security constraints if this method is not called.
      */
      void setCaseInsensitive();
 
#else
{
   AuthorizerIntf super; /* We inherit from AuthorizerIntf */
#endif

   DoubleList resourceList;
   JUserCont* userCont;
   HashTable* resourceTable;
   int noOfResources;
   AllocatorIntf* alloc;
   BaBool caseSensensitive;

} JConstrCont;

#ifdef __cplusplus
extern "C" {
#endif
BA_API void JConstrCont_constructor(JConstrCont* o, JUserCont* userCont,
                             AllocatorIntf* alloc);
BA_API void JConstrCont_destructor(JConstrCont* o);
BA_API void JConstrCont_setConstraints(JConstrCont* o,
                                JVal* constraintsVal, JErr* err);
#define JConstrCont_setCaseInsensitive(o) (o)->caseSensensitive=FALSE
#ifdef __cplusplus
}
inline JConstrCont::JConstrCont(JUserCont* userCont, AllocatorIntf* alloc) {
   JConstrCont_constructor(this, userCont, alloc); }
inline JConstrCont::~JConstrCont() {
   JConstrCont_destructor(this); }
inline void JConstrCont::setConstraints(JVal* constraintsVal, JErr* err) {
   JConstrCont_setConstraints(this,constraintsVal, err); }
inline void JConstrCont::setCaseInsensitive() {
   JConstrCont_setCaseInsensitive(this);
}
#endif

#endif

