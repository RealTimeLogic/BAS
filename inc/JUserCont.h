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
 *   $Id: JUserCont.h 4915 2021-12-01 18:26:55Z wini $
 *
 *   COPYRIGHT:  Real Time Logic LLC, 2007-2013
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

#ifndef __JUserCont_h
#define __JUserCont_h

#include <AuthenticatedUser.h>
#include <SplayTree.h>
#include <JVal.h>
 
typedef struct
{
      SplayTreeNode super;
      char* name; /* Pointer to memory below roles. */
      char* pwd;  /* Pointer to memory below name. */
      BaTime maxInactiveInterval;
      U16 noOfRoles;
      U16 maxUsers;
      BaBool recycle;
      BaBool pwdIsHA1; 
      /* Roles array must be last. */
      U16 roles[1]; /* List of roles. Roles are translated into unique numbers. */
} JUser;


typedef struct
{
      SplayTreeNode super;
      U16 roleId;
      char name[1];
} JRole;


typedef struct JUserCont
#ifdef __cplusplus
   : public UserIntf
{
      JUserCont(AllocatorIntf* alloc);
      ~JUserCont();
      void setUserDb(JVal* usersVal, JErr* err);
#else
{
   UserIntf super; /* We inherit from UserIntf. */
#endif

   SplayTree userdb;  /* JUser */
   SplayTree roledb;  /* JRole */
   AllocatorIntf* alloc;
   U32 noOfUsers;
   U16 nextUniqueRoleId;
} JUserCont;

#ifdef __cplusplus
extern "C" {
#endif
#define JUserCont_findUser(o, uname) \
  (JUser*)SplayTree_find(&(o)->userdb, uname)
U16 JUserCont_role2Id(JUserCont* o, const char* roleName);
U16* JUserCont_copyRoles(JUserCont* o,U16* roles,JVal* rolesVal,JErr* err);
BA_API void JUserCont_constructor(JUserCont* o, AllocatorIntf* alloc);
BA_API void JUserCont_destructor(JUserCont* o);
BA_API void JUserCont_setUserDb(JUserCont* o, JVal* usersVal, JErr* err);
#ifdef __cplusplus
}
inline JUserCont::JUserCont(AllocatorIntf* alloc) {
   JUserCont_constructor(this, alloc); }
inline JUserCont::~JUserCont() {
   JUserCont_destructor(this); }
inline void JUserCont::setUserDb(JVal* usersVal, JErr* err) {
   JUserCont_setUserDb(this, usersVal, err); }
#endif 

#endif
