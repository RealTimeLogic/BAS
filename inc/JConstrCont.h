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
 *   $Id: JConstrCont.h 5992 2026-09-12 09:32:44Z wini $
 *
 *   COPYRIGHT:  Real Time Logic LLC, 2007 - 2026
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

/** @file JConstrCont.h */

#ifndef __JConstrCont_h
#define __JConstrCont_h

#include "JUserCont.h"
#include "BaServerLib.h"
#include "HashTable.h"

/** Authorization rules loaded from a parsed JSON object.
 * Each object member is a rule object with urls, methods and roles arrays.
 * The rule member name is only a label. For example:
 * @code{.json}
 * {"admin":{"urls":["/admin/ *"],"methods":["GET"],"roles":["admin"]}}
 * @endcode
 * Exact paths are checked before prefix rules. A final slash-star selects a
 * prefix; this is not a general wildcard matcher. An empty methods array
 * matches all HTTP methods, and an empty roles array matches all roles.
 * A user with no roles has special super-user access as defined by JUserCont.
 * HTTP HEAD is checked as GET. Unmatched requests are denied.
 *
 * The user database is borrowed and must outlive this object. Rule updates
 * are not transactional: existing rules are removed before replacement is
 * complete. Serialize configuration changes with requests using this object.
 */
typedef struct JConstrCont
#ifdef __cplusplus
   : public AuthorizerIntf 
{
      /** Initialize an empty rule container.
       * @param[in] userCont Required borrowed user database used to resolve roles.
       * @param[in] alloc Borrowed allocator, or NULL for the default allocator.
       * Both must remain valid until this container is destroyed. */
      JConstrCont(JUserCont* userCont, AllocatorIntf* alloc=0);
      /** Release the stored rules; does not destroy userCont or its allocator.
       * Detach the authorizer and stop its users before destruction. */
      ~JConstrCont();
      /** Replace rules from a parsed JSON object.
       * @param[in] constraintsVal Required JVal object with rule objects as
       * members. Each rule requires urls (nonempty array of nonempty path
       * strings), methods (array of HTTP method-name strings, or empty), and
       * roles (array of role-name strings, or empty). Strings and role IDs are
       * copied; the JVal tree can be released after this call.
       * @param[in,out] err Required initialized JErr. Reported format and
       * allocation failures are stored here; the old rules are not restored
       * and partial replacement rules can remain. No value is returned.
       */
      void setConstraints(JVal* constraintsVal, JErr* err);

      /** Make prefix-rule comparisons case-insensitive. Call during setup
       * when the backing filesystem ignores case. Exact-path hash lookups are
       * unaffected by this flag in the current implementation; do not assume
       * this method makes every constraint comparison case-insensitive.
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
/** Initialize an empty rule container.
 * @param[out] o Caller-owned storage.
 * @param[in] userCont Required borrowed user database.
 * @param[in] alloc Borrowed allocator, or NULL for the default.
 * @see JConstrCont::JConstrCont */
BA_API void JConstrCont_constructor(JConstrCont* o, JUserCont* userCont,
                             AllocatorIntf* alloc);
/** Release rules after detaching the authorizer from its users.
 * @param[in,out] o Initialized container; its user database is not destroyed. */
BA_API void JConstrCont_destructor(JConstrCont* o);
/** Replace rules without rollback on failure.
 * @param[in,out] o Initialized container.
 * @param[in] constraintsVal Parsed JSON object; see JConstrCont::setConstraints.
 * @param[in,out] err Required initialized JErr receiving reported failures. */
BA_API void JConstrCont_setConstraints(JConstrCont* o,
                                JVal* constraintsVal, JErr* err);
/** Enable case-insensitive prefix matching.
 * @param[in,out] o Initialized container. Exact-path comparisons are unchanged. */
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

