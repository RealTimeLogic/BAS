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
 *   $Id: JUserCont.h 5978 2026-09-11 16:13:48Z wini $
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

/** @file JUserCont.h */

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


/** User database implementing UserIntf, loaded from a parsed JSON object.
 * Object member names are usernames. Each user requires pwd and roles:
 * @code{.json}
 * {"operator":{"pwd":"replace-this-password","roles":["viewer"],"maxUsers":5}}
 * @endcode
 * pwd is a string, or an array whose first string is a 32-character HA1 value
 * for the MD5 authentication variant. The remaining array elements are not
 * used. roles is an array of role-name strings; an empty array or empty object
 * means no roles. In JConstrCont, users without roles have super-user access.
 *
 * Optional maxUsers (also accepted as maxusers) is an integer, default 5,
 * clamped to 0..65535; zero omits the user. inactive is a nonnegative integer
 * timeout in seconds, default zero; negative values become zero. recycle is a
 * boolean, default FALSE. Missing or wrong-type optional values use defaults.
 * Passwords longer than 98 bytes are stored but cannot be returned through the
 * current AuthInfo password buffer, so authentication will fail for that user.
 *
 * Updates replace the user list without rollback; role-name mappings persist
 * until destruction. Serialize database updates with readers and authorizers.
 */
typedef struct JUserCont
#ifdef __cplusplus
   : public UserIntf
{
      /** Initialize an empty database without copying the allocator.
       * @param[in] alloc Borrowed allocator, or NULL for the default allocator.
       * The allocator must outlive the database. */
      JUserCont(AllocatorIntf* alloc);
      /** Release users and role mappings. Detach users of this UserIntf,
       * including any JConstrCont, before destruction. */
      ~JUserCont();
      /** Replace users from the JSON schema described by JUserCont.
       * @param[in] usersVal Required parsed JVal object. Strings are copied,
       * so the tree can be released after the call. Usernames must be unique.
       * @param[in,out] err Required initialized JErr receiving reported format
       * or allocation errors. Old users are removed first; failure can leave
       * a partially populated database. No value is returned. */
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
/** Initialize a user database.
 * @param[out] o Caller-owned storage.
 * @param[in] alloc Borrowed allocator, or NULL for the default allocator. */
BA_API void JUserCont_constructor(JUserCont* o, AllocatorIntf* alloc);
/** Release all users and role mappings.
 * @param[in,out] o Initialized database, no longer referenced by authenticators
 * or authorizers. Does not free the allocator itself. */
BA_API void JUserCont_destructor(JUserCont* o);
/** Replace the user database without rollback.
 * @param[in,out] o Initialized database.
 * @param[in] usersVal Parsed JSON object; see JUserCont::setUserDb.
 * @param[in,out] err Required initialized JErr receiving reported failures. */
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
