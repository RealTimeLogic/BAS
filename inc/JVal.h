/*
 *     ____             _________                __                _     
 *    / __ \___  ____ _/ /_  __(_)___ ___  ___  / /   ____  ____ _(_)____
 *   / /_/ / _ \/ __ `/ / / / / / __ `__ \/ _ \/ /   / __ \/ __ `/ / ___/
 *  / _, _/  __/ /_/ / / / / / / / / / / /  __/ /___/ /_/ / /_/ / / /__  
 * /_/ |_|\___/\__,_/_/ /_/ /_/_/ /_/ /_/\___/_____/\____/\__, /_/\___/  
 *                                                       /____/          
 *
 *                  Barracuda Application Server
 *
 ****************************************************************************
 *			      HEADER
 *
 *   $Id: JVal.h 5978 2026-09-11 16:13:48Z wini $
 *
 *   COPYRIGHT:  Real Time Logic LLC, 2006-2026
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

/** @file JVal.h */

#ifndef __JVal_h
#define __JVal_h

#include <JParser.h>
#include <stdarg.h>

/** @addtogroup JSONRef
@{
*/


/** JVal represents a value in a JSON tree.

An instance of the JParserValFact creates a syntax tree out of the parsed
JSON data. An instance of JVal is one node in the tree. Method
JParserValFact::getFirstVal returns the tree root element.

Most methods in this class take a JErr as argument. The methods set
an error code if the JVal is not of the expected type or any other
errors occur.

Always check the supplied JErr as well as the returned value: zero/NULL can
also be valid JSON results. Format extraction and add operations stop for an
existing error, but individual accessors and setters do not uniformly do so.
Some C functions accept NULL and record an error; direct accessor macros require
non-NULL nodes. Never call a C++ member through a NULL pointer.

Nodes, strings, and member names belong to their tree until explicitly detached.
Use the matching node and data allocators when releasing detached storage.

<h2>Examples:</h2>

See the JSON tutorial, section
[Using JParserValFact](@ref UsingJParserValFact)
, for examples on how to use JVal.

*/
struct JVal
{
#ifdef __cplusplus
      /** @return Stored JVType of this required live node; no conversion or validation.
 */
      JVType getType();

      /** Extract values as for get(), using mutable format and argument cursors.
    @param err Required initialized error container.
    @param fmt Required pointer to a NUL-terminated format cursor, advanced by parsing.
    @param argList Required pointer to a matching initialized va_list, consumed.
    @return Remaining sibling pointer, possibly NULL on success; inspect err.
 */
      JVal* vget(JErr* err,const char** fmt, va_list* argList);

      /** Get any type of value(s) from a JVal node or JVal tree.

      The get method works in a similar fashion to the ANSI C function
      scanf; thus, it performs formatted input conversion.

      The method internally uses the 'getXXX' methods in this class
      for extracting the actual value(s), where XXX is Int, Long, etc.. The
      following table shows the relationship between the format flags and
      the JSON value extracted:

      <table>
      <tr><th>JSON type</th><th>Format flag</th><th>C type</th>
      <th>Method</th></tr>
      <tr><td>Number</td><td> d </td><td>S32*</td>
      <td>JVal::getInt</td></tr>
      <tr><td>Number</td><td> l </td><td>S64*</td>
      <td>JVal::getLong</td></tr>
      <tr><td>Number</td><td> f </td><td>double*</td>
      <td>JVal::getDouble</td></tr>
      <tr><td>boolean</td><td> b </td><td>BaBool*</td>
      <td>JVal::getBoolean</td></tr>
      <tr><td>string</td><td> s </td><td>const char**</td>
      <td>JVal::getString</td></tr>
      <tr><td>Any type</td><td> J </td><td>JVal**</td>
      <td>All methods</td></tr>
      <tr><td>Start object</td><td> { </td><td>n/a</td>
      <td>JVal::getObject</td></tr>
      <tr><td>End object</td><td> } </td><td>n/a</td>
      <td>n/a</td></tr>
      <tr><td>Start array</td><td> [ </td><td>n/a</td>
      <td>JVal::getArray</td></tr>
      <tr><td>End array</td><td> ] </td><td>n/a</td>
      <td>n/a</td></tr>
      <tr><td>Array</td><td>A followed by b/d/l/f/s/J</td><td>typed output array pointer, int count</td>
      <td>All getXXX methods</td></tr>
      </table>

      See the JSON tutorial, section
      [Using JParserValFact](@ref UsingJParserValFact)
      , for examples on how to use JVal.

      \sa JDecoderr::get
      \sa JEncoder::set

      <h2>Advanced C Example:</h2>
\code
   JVal* jForecast[7]; //Array for 7 day forecast
   JVal_get(rootVal,
            &err,
            "{ssdddAJ}",
            "city",&city,
            "desc",&desc,
            "temp",&temp,
            "humidity",&humidity,
            "dt", &dt,
            "list",jForecast,7);//Flag '{A}' requires: member name, list, length
   if(JErr_noError(&err))
   {
      for(i=0 ; i < 7 && JErr_noError(&err) ; i++)
      {
         JVal_get(jForecast[i],
                  &err,
                  "{dddds}",
                  "wday",&wday,
                  "tmax",&tmax,
                  "tmin",&tmin,
                  "humidity",&humidity,
                  "desc",&desc);
      }
   }
\endcode
             @param err Required initialized error container, preserved once set.
      @param fmt Required NUL-terminated format. Supply a member name before each
      object's output arguments. All output pointers must have the exact listed
      types. A is followed by b/d/l/f/s/J and takes an output array pointer then an
      int count. Extra source elements are ignored; output can be partially filled
      before failure. The current empty-array path can leave outputs unchanged
      without an error, so check the source length when requiring a fixed count.
      Strings and JVal pointers returned in outputs remain borrowed from the tree.
      @return Pointer to the next unconsumed sibling, or NULL at the end or on
      failure. NULL alone is not an error test; inspect err.
       */
      JVal* get(JErr* err, const char* fmt, ...);

      /** Convert a numeric, boolean, or null node to S32.
    @param e Required initialized error container.
    @return Converted value; null yields zero. Wrong types or a missing C node
    record an error and return zero. Numeric conversion uses C casts; it is not
    a range or integrality check. Floating input must be finite and representable
    in the destination; fractional parts are discarded.
 */
      S32 getInt(JErr* e);

      /** Replace a scalar value and set its type.
    @param e Required initialized error container receiving refusal details.
    @param v New S32 value. No return value; inspect e.
    Cannot replace an object/array or an attached string. Detach and release an
    old string first. Requires a live non-NULL node; existing errors do not by
    themselves prevent this setter from changing the node.
 */
      void setInt(JErr* e,S32 v);

      /** Convert a numeric, boolean, or null node to S64.
    @param e Required initialized error container.
    @return Converted value; null yields zero. Wrong types or a missing C node
    record an error and return zero. Numeric conversion uses C casts; it is not
    a range or integrality check. Floating input must be finite and representable
    in the destination; fractional parts are discarded.
 */
      S64 getLong(JErr* e);

      /** Replace a scalar value and set its type.
    @param e Required initialized error container receiving refusal details.
    @param v New S64 value. No return value; inspect e.
    Cannot replace an object/array or an attached string. Detach and release an
    old string first. Requires a live non-NULL node; existing errors do not by
    themselves prevent this setter from changing the node.
 */
      void setLong(JErr* e,S64 v);

      /** Convert a numeric, boolean, or null node to double.
    @param e Required initialized error container.
    @return Converted value; null yields zero. Wrong types or a missing C node
    record an error and return zero. Numeric conversion uses C casts; it is not
    a range or integrality check. Integer conversion can lose precision.
 */
      double getDouble(JErr* e);

      /** Replace a scalar value and set its type.
    @param e Required initialized error container receiving refusal details.
    @param v New double value. No return value; inspect e.
    Cannot replace an object/array or an attached string. Detach and release an
    old string first. Requires a live non-NULL node; existing errors do not by
    themselves prevent this setter from changing the node.
 */
      void setDouble(JErr* e,double v);

      /** Read a boolean or null node.
    @param e Required initialized error container.
    @return Stored boolean, FALSE for JSON null, or FALSE with an error for other
    types/missing C nodes. Numeric values are not treated as booleans.
 */
      BaBool getBoolean(JErr* e);

      /** Replace a scalar value and set its type.
    @param e Required initialized error container receiving refusal details.
    @param v New BaBool value. No return value; inspect e.
    Cannot replace an object/array or an attached string. Detach and release an
    old string first. Requires a live non-NULL node; existing errors do not by
    themselves prevent this setter from changing the node.
 */
      void setBoolean(JErr* e,BaBool v);

      /** Replace a scalar value with JSON null.
    @param e Required initialized error container. Objects/arrays and attached
    strings cannot be replaced; detach/release a string first. Requires a live
    non-NULL node. No return value; inspect e.
 */
      void setNull(JErr* e);

      /** Access a string without transferring ownership.
    @param e Required initialized error container.
    @return Borrowed NUL-terminated bytes for a string, NULL for JSON null, or NULL
    with an error for other types/missing C nodes. Use getStringLen() to preserve
    embedded NUL bytes. The pointer expires on replacement or tree destruction.
 */
      const char* getString(JErr* e);

      /** @return Stored string byte length, excluding the terminator. Call on a
    live string node; this direct accessor does not check the type.
 */
      size_t getStringLen();

      /** Store a string pointer without copying it and set the node's type.
    @param e Required initialized error container receiving refusal details.
    @param v NUL-terminated string allocated compatibly with the tree's data
    allocator, or NULL for an empty stored pointer. strlen determines its length.
    On success the node takes ownership. On failure ownership stays with the caller.
    Detach/release an old string first; object/array nodes cannot be replaced.
    Requires a live node. No return value; inspect e.
 */
      void setString(JErr* e, char* v);

      /** Detach a string's storage, leaving a String node with NULL data and zero length.
    @param e Required initialized error container.
    @return Previously owned string pointer, now caller-owned; release through the
    original data allocator. NULL can mean no stored pointer or a type/missing-node
    error; inspect e. JSON null is a type error for this operation.
 */
      char* manageString(JErr* e);

      /** @return Borrowed member name, or NULL when absent. Use getNameLen() for
    embedded NUL bytes. The C function also accepts a NULL node. No ownership transfers.
 */
      const char* getName();

      /** @return Stored member-name byte length excluding the terminator. Requires
    a live node; an empty name can still be an object member.
 */
      size_t getNameLen();

      /** Detach the member name and clear its pointer/length in the node.
    @return Caller-owned string to release through the original data allocator,
    or NULL if absent. The C function accepts NULL. The node is no longer marked
    as an object member after its name is detached.
 */
      char* manageName();

      /** @return Borrowed next sibling, or NULL at the end. Requires a live node.
    This does not return a child or transfer ownership.
 */
      JVal* getNextElem();

      /** Access the first child of an object node.
    @param e Required initialized error container.
    @return Borrowed first child, NULL for an empty container, or NULL with an
    error for the wrong type/missing C node. Inspect e to distinguish these cases.
 */
      JVal* getObject(JErr* e);

      /** Access the first child of an array node.
    @param e Required initialized error container.
    @return Borrowed first child, NULL for an empty container, or NULL with an
    error for the wrong type/missing C node. Inspect e to distinguish these cases.
 */
      JVal* getArray(JErr* e);

      /** Access the first child of an object or array node.
    @param e Required initialized error container.
    @return Borrowed first child, NULL for an empty container, or NULL with an
    error for the wrong type/missing C node. Inspect e to distinguish these cases.
 */
      JVal* getJ(JErr* e);

      /** Detach all children from an object/array, leaving the container empty.
    @param e Required initialized error container.
    @return Caller-owned first child and its complete sibling chain, or NULL for
    an empty container/error. Release using terminate() with the original allocators.
    The parent node itself is not detached or destroyed.
 */
      JVal* manageJ(JErr* e);

      /** Count immediate children of this object/array.
    @param e Required initialized error container.
    @return S32 child count, zero for an empty container or an error. Call on the
    container itself, not the first child; this walks the child list.
 */
      S32 getLength(JErr* e);

      /** @return True if a member-name pointer is present (including an empty name),
    false otherwise. Requires a live node; this does not inspect a parent pointer.
 */
      bool isObjectMember();

      /** Remove one immediate child without destroying it.
    @param child Required child to detach from this live object/array.
    @return Zero when found and removed, -1 otherwise. Success clears child->next
    and transfers that node/subtree to the caller; its member name is retained.
 */
      int unlink(JVal* child);

      /** Prepend a child to an object.
    @param e Required initialized error container.
    @param memberName Required NUL-terminated name used only if child has no name.
    An existing child name is retained; this does not rename it or check duplicates.
    @param child Required detached node with next=NULL and no other owning parent.
    @param dAlloc Allocator used to copy a missing member name. If NULL, the supplied
    pointer is stored directly; its eventual release must still match tree ownership.
    @return Zero on success, -1 on failure. Some linkage failures do not set e;
    check the return. A copied name can remain on child after failure. Success
    transfers child ownership to the tree and places it first, not last.
 */
      int addMember(JErr* e, const char* memberName,
                    JVal* child, AllocatorIntf* dAlloc);

      /** Prepend a child to an array.
    @param e Required initialized error container.
    @param child Required detached node with next=NULL and no other owner. Use
    an unnamed node for an array element. No copy or allocation is performed.
    @return Zero with ownership transferred, -1 on failure. Check the return as
    some linkage failures do not set e. Adds at the beginning of the array.
 */
      int add(JErr* e, JVal* child);

      /** Free this node, all descendants, and all following siblings recursively.
    @param vAlloc Required original allocator for node storage.
    @param dAlloc Required original allocator for strings/member names.
    Detach the chain from its owner before calling; every freed pointer becomes
    invalid. Allocators must accept all storage being released. The C function
    accepts NULL for an empty chain, but a C++ member call still requires a node.
 */
      void terminate(AllocatorIntf* vAlloc, AllocatorIntf* dAlloc);
#endif
      union
      {
            struct JVal* firstChild; /* If object or array */
            U8* s; /* If string */
#ifndef NO_DOUBLE
            double f; /* If floating point */
#endif
            S32 d; /* If integer */
            S64 l; /* If long integer */
            BaBool b; /* If true or false */
      } v;

      char* memberName;
      struct JVal* next;
      size_t stringLen;
      size_t memberNameLen;
      JVType type;
};

typedef struct JVal JVal;

#ifdef __cplusplus
extern "C" {
#endif
/** @copydoc JVal::getType
    @param o Required live node.
 */
#define JVal_getType(o) (o)->type
/** @copydoc JVal::vget
    @param o Node pointer; see the operation contract for NULL handling.
 */
BA_API JVal* JVal_vget(JVal* o,JErr* err,const char** fmt, va_list* argList);
/** @copydoc JVal::get
    @param o Node pointer; see the operation contract for NULL handling.
 */
BA_API JVal* JVal_get(JVal* o, JErr* err, const char* fmt, ...);
/** @copydoc JVal::getInt
    @param o Node pointer; see the operation contract for NULL handling.
 */
BA_API S32 JVal_getInt(JVal* o, JErr* e);
/** @copydoc JVal::getLong
    @param o Node pointer; see the operation contract for NULL handling.
 */
BA_API S64 JVal_getLong(JVal* o, JErr* e);
/** @copydoc JVal::getDouble
    @param o Node pointer; see the operation contract for NULL handling.
 */
BA_API double JVal_getDouble(JVal* o, JErr* e);
/** @copydoc JVal::getBoolean
    @param o Node pointer; see the operation contract for NULL handling.
 */
BA_API BaBool JVal_getBoolean(JVal* o, JErr* e);
/** @copydoc JVal::getString
    @param o Node pointer; see the operation contract for NULL handling.
 */
BA_API const char* JVal_getString(JVal* o, JErr* e);
/** @copydoc JVal::getStringLen
    @param o Required live node.
 */
#define JVal_getStringLen(o) (o)->stringLen
/** @copydoc JVal::manageString
    @param o Node pointer; see the operation contract for NULL handling.
 */
BA_API char* JVal_manageString(JVal* o, JErr* e);
/** @copydoc JVal::getName
    @param o Node pointer; see the operation contract for NULL handling.
 */
BA_API const char* JVal_getName(JVal* o);
/** @copydoc JVal::getNameLen
    @param o Required live node.
 */
#define JVal_getNameLen(o) (o)->memberNameLen
/** @copydoc JVal::manageName
    @param o Node pointer; see the operation contract for NULL handling.
 */
BA_API char* JVal_manageName(JVal* o);
/** @copydoc JVal::getNextElem
    @param o Required live node.
 */
#define JVal_getNextElem(o) (o)->next
/** @copydoc JVal::getObject
    @param o Node pointer; see the operation contract for NULL handling.
 */
BA_API JVal* JVal_getObject(JVal* o, JErr* e);
/** @copydoc JVal::getArray
    @param o Node pointer; see the operation contract for NULL handling.
 */
BA_API JVal* JVal_getArray(JVal* o, JErr* e);
/** @copydoc JVal::getJ
    @param o Node pointer; see the operation contract for NULL handling.
 */
BA_API JVal* JVal_getJ(JVal* o, JErr* e);
/** @copydoc JVal::manageJ
    @param o Node pointer; see the operation contract for NULL handling.
 */
BA_API JVal* JVal_manageJ(JVal* o, JErr* e);
/** @copydoc JVal::getLength
    @param o Node pointer; see the operation contract for NULL handling.
 */
BA_API S32 JVal_getLength(struct JVal* o, JErr* e);
/** @copydoc JVal::isObjectMember
    @param o Required live node.
 */
#define JVal_isObjectMember(o) ((o)->memberName != 0) 
/** @copydoc JVal::unlink
    @param o Required live node.
 */
BA_API int JVal_unlink(JVal* o, JVal* child);
/** @copydoc JVal::addMember
    @param o Required live node.
 */
BA_API int JVal_addMember(JVal* o, JErr* e, const char* memberName,
                          JVal* child, AllocatorIntf* dAlloc);
/** @copydoc JVal::add
    @param o Required live node.
 */
BA_API int JVal_add(JVal* o, JErr* e, JVal* child);
/** @copydoc JVal::terminate
    @param o Node pointer; see the operation contract for NULL handling.
 */
BA_API void JVal_terminate(JVal* o, AllocatorIntf* vAlloc,
                           AllocatorIntf* dAlloc);
/** @copydoc JVal::setInt
    @param o Required live node.
    The C macro requires v to be an addressable variable of the declared type.
 */
#define JVal_setInt(o, e, v) JVal_setX(o, e, JVType_Int, &v)
/** @copydoc JVal::setLong
    @param o Required live node.
    The C macro requires v to be an addressable variable of the declared type.
 */
#define JVal_setLong(o, e, v) JVal_setX(o, e, JVType_Long, &v)
/** @copydoc JVal::setDouble
    @param o Required live node.
    The C macro requires v to be an addressable variable of the declared type.
 */
#define JVal_setDouble(o, e, v) JVal_setX(o, e, JVType_Double, &v)
/** @copydoc JVal::setBoolean
    @param o Required live node.
    The C macro requires v to be an addressable variable of the declared type.
 */
#define JVal_setBoolean(o, e, v) JVal_setX(o, e, JVType_Boolean, &v)
/** @copydoc JVal::setNull
    @param o Required live node.
 */
#define JVal_setNull(o, e) JVal_setX(o, e, JVType_Null, 0)
/** @copydoc JVal::setString
    @param o Required live node.
 */
#define JVal_setString(o, e, v) JVal_setX(o, e, JVType_String, v)
/** Low-level scalar replacement used by the typed setter macros.
    @param o Required live scalar node; objects/arrays cannot be replaced.
    @param e Required initialized error container. An attached old string must
    first be detached; failure is reported here, with no return value.
    @param t New type: Int, Long, Double (when enabled), Boolean, String, or Null.
    @param v Pointer to S32, S64, double, or BaBool storage matching t; for String
    it is the string itself and ownership transfers on success. NULL is accepted
    for Null and as a stored String pointer. See the typed setters for lifetime.
 */
BA_API void JVal_setX(JVal* o, JErr* e, JVType t, void* v);
#ifdef __cplusplus
}
inline JVType JVal::getType(){
   return JVal_getType(this); }
inline JVal* JVal::vget(JErr* err,const char** fmt, va_list* argList){
   return JVal_vget(this,err,fmt, argList); }
inline JVal* JVal::get( JErr* err, const char* fmt, ...){
   JVal* retv; va_list argList;
   va_start(argList, fmt); 
   retv = JVal_vget(this,err,&fmt,&argList);
   va_end(argList);
   return retv; 
}
inline S32 JVal::getInt(JErr* e){
   return JVal_getInt(this, e); }
inline S64 JVal::getLong(JErr* e){
   return JVal_getLong(this, e); }
inline double JVal::getDouble(JErr* e){
   return JVal_getDouble(this, e); }
inline BaBool JVal::getBoolean(JErr* e){
   return JVal_getBoolean(this, e); }
inline const char* JVal::getString(JErr* e){
   return JVal_getString(this, e); }
inline size_t JVal::getStringLen(){
   return JVal_getStringLen(this); }
inline char* JVal::manageString(JErr* e){
   return JVal_manageString(this, e); }
inline const char* JVal::getName(){
   return JVal_getName(this); }
inline size_t JVal::getNameLen(){
   return JVal_getNameLen(this); }
inline char* JVal::manageName(){
   return JVal_manageName(this); }
inline JVal* JVal::getNextElem(){
   return JVal_getNextElem(this); }
inline JVal* JVal::getObject(JErr* e){
   return JVal_getObject(this, e); }
inline JVal* JVal::getArray(JErr* e){
   return JVal_getArray(this, e); }
inline JVal* JVal::getJ(JErr* e){
   return JVal_getJ(this, e); }
inline JVal* JVal::manageJ(JErr* e){
   return JVal_manageJ(this, e); }
inline S32 JVal::getLength(JErr* e){
   return JVal_getLength(this, e); }

inline void JVal::setInt(JErr* e,S32 val) {
  JVal_setInt(this, e, val); }
inline void JVal::setLong(JErr* e,S64 val) {
  JVal_setLong(this, e, val); }
inline void JVal::setDouble(JErr* e,double val) {
  JVal_setDouble(this, e, val); }
inline void JVal::setBoolean(JErr* e,BaBool val) {
  JVal_setBoolean(this, e, val); }
inline void JVal::setNull(JErr* e) {
  JVal_setNull(this, e); }
inline void JVal::setString(JErr* e, char* val) {
  JVal_setString(this, e, val); }
inline bool JVal::isObjectMember(){
   return JVal_isObjectMember(this) ? true : false; }
inline int JVal::unlink(JVal* child){
   return JVal_unlink(this, child); }
inline int JVal::addMember(JErr* e, const char* name,
                           JVal* child, AllocatorIntf* dAlloc) {
   return JVal_addMember(this, e, name, child, dAlloc); }
inline int JVal::add(JErr* e, JVal* child) {
   return JVal_add(this, e, child); }
inline void JVal::terminate(AllocatorIntf* vAlloc, AllocatorIntf* dAlloc){
   JVal_terminate(this, vAlloc, dAlloc); }
#endif

/** @} */ /* end of JSONRef */

/** @addtogroup JSONCB
@{
*/

typedef enum
{
   JParserValFactStat_OK=0,
   JParserValFactStat_DMemErr,
   JParserValFactStat_VMemErr,
   JParserValFactStat_MaxNodes
} JParserValFactStat;

/** JParserValFact is the JVal JSON parser factory class. An instance
    of this class is connected to an instance of the JParser. The
    JParser communicates with the JVal factory when parsing JSON, and
    the JVal factory creates JVal objects.

    Method getFirstVal returns the first value in the syntax tree.

    The class automatically manages the nodes allocated. The allocated
    nodes are automatically de-allocated when the destructor runs. You
    can manage the values by calling JParserValFact::manageFirstVal or
    by calling JVal::manageJ on any of the children.

    \sa JValFact
    \sa JDecoder
*/
#ifdef __cplusplus
typedef struct JParserValFact : public JParserIntf
{
      /** Initialize a factory without allocating a tree.
    @param vAlloc Required borrowed allocator for nodes.
    @param dAlloc Required borrowed allocator for strings and factory stack.
    Allocators may be the same and must outlive all storage they allocate.
    NULL does not select a default. The factory owns its attached root.
 */
      JParserValFact(AllocatorIntf* vAlloc, AllocatorIntf* dAlloc);

      /** Free the attached root and all factory stack storage. Detached nodes
    remain caller-owned. Borrowed allocators are not destroyed.
 */
      ~JParserValFact();

      /** @return Borrowed current root, or NULL when absent. After a parse failure
    the tree may be partial; only treat it as complete after successful parsing.
 */
      JVal* getFirstVal();

      /** Detach the current root and reset the node counter when a root exists.
    @return Caller-owned root, or NULL if absent. Release with JVal::terminate
    using the original allocators. The factory retains stack storage and status.
 */
      JVal* manageFirstVal();

      /** Free the attached tree and factory stack, preparing for another document.
    Detached trees are unaffected. The stored status is not cleared; after a
    failure, reinitialize the cleaned factory before reuse.
 */
      void termFirstVal();
#else
typedef struct JParserValFact
{
      JParserIntf super; /*Inherits from JParserIntf*/
#endif
      AllocatorIntf* dAlloc;
      AllocatorIntf* vAlloc;
      JVal** vStack;
      int vStackSize;
      U32 nodeCounter;
      /** Node-count threshold, initially U32 maximum. Allocation is refused
          when the incremented count reaches this value; it is an exclusive limit. */
      U32 maxNodes;
      /** Factory status, initially OK; parse failures may set it. Terminating or
          detaching the root does not clear this field. */
      JParserValFactStat status;
} JParserValFact;
#ifdef __cplusplus
extern "C" {
#endif
/** @copydoc JParserValFact::JParserValFact
    @param o Required storage to initialize.
 */
BA_API void JParserValFact_constructor(
   JParserValFact* o, AllocatorIntf* vAlloc, AllocatorIntf* dAlloc);
/** @copydoc JParserValFact::getFirstVal
    @param o Required initialized factory.
 */
#define JParserValFact_getFirstVal(o) \
   ((o)->vStack && *(o)->vStack ? *(o)->vStack : 0)
/** @copydoc JParserValFact::manageFirstVal
    @param o Required initialized factory.
 */
BA_API JVal* JParserValFact_manageFirstVal(JParserValFact* o);
/** @copydoc JParserValFact::termFirstVal
    @param o Required initialized factory.
 */
BA_API void JParserValFact_termFirstVal(JParserValFact* o);
/** @copydoc JParserValFact::~JParserValFact
    @param o Required initialized factory.
 */
BA_API void JParserValFact_destructor(JParserValFact* o);
#ifdef __cplusplus
}
inline JParserValFact::JParserValFact(
   AllocatorIntf* vAlloc, AllocatorIntf* dAlloc){
   JParserValFact_constructor(this, vAlloc, dAlloc);}
inline JVal* JParserValFact::getFirstVal() {
   return JParserValFact_getFirstVal(this); }
inline JVal* JParserValFact::manageFirstVal() {
   return JParserValFact_manageFirstVal(this); }
inline void JParserValFact::termFirstVal() {
    JParserValFact_termFirstVal(this); }
inline JParserValFact::~JParserValFact() {
    JParserValFact_destructor(this); }
#endif

/** @} */ /* end of JSONCB */

/** @addtogroup JSONRef
@{
*/

/** The JValFact enables one to manually create a JVal syntax tree or
    add nodes to an existing syntax tree.

    \sa JParserValFact
 */
typedef struct JValFact
{
#ifdef __cplusplus
      /** Initialize a factory without allocating a tree.
    @param vAlloc Required borrowed allocator for nodes.
    @param dAlloc Required borrowed allocator for strings.
    Allocators may be the same and must outlive all storage they allocate.
    NULL does not select a default. The caller owns created nodes; the factory does not track or free them.
 */
      JValFact(AllocatorIntf* vAlloc, AllocatorIntf* dAlloc);
      
      /** Create a detached string node.
    @param v Required NUL-terminated string, copied with dAlloc.
    @return Caller-owned node, or NULL on allocation failure. Attach it to a tree
    or release it with JVal::terminate and the factory's original allocators.
 */
      JVal* mkString(const char* v);

      /** Create a detached double node.
    @param v Value copied into the new node.
    @return Caller-owned node, or NULL on allocation failure. Attach it to a tree
    or release it with JVal::terminate and the factory's original allocators.
 */
      JVal* mkDouble(double v);

      /** Create a detached int node.
    @param v Value copied into the new node.
    @return Caller-owned node, or NULL on allocation failure. Attach it to a tree
    or release it with JVal::terminate and the factory's original allocators.
 */
      JVal* mkInt(S32 v);

      /** Create a detached long node.
    @param v Value copied into the new node.
    @return Caller-owned node, or NULL on allocation failure. Attach it to a tree
    or release it with JVal::terminate and the factory's original allocators.
 */
      JVal* mkLong(S64 v);

      /** Create a detached boolean node.
    @param v Value copied into the new node.
    @return Caller-owned node, or NULL on allocation failure. Attach it to a tree
    or release it with JVal::terminate and the factory's original allocators.
 */
      JVal* mkBoolean(bool v);

      /** Create a detached null node.
    @return Caller-owned node, or NULL on allocation failure. Attach it to a tree
    or release it with JVal::terminate and the factory's original allocators.
 */
      JVal* mkNull();

      /** Create a detached object node.
    @return Caller-owned node, or NULL on allocation failure. Attach it to a tree
    or release it with JVal::terminate and the factory's original allocators. The container starts empty.
 */
      JVal* mkObject();

      /** Create a detached array node.
    @return Caller-owned node, or NULL on allocation failure. Attach it to a tree
    or release it with JVal::terminate and the factory's original allocators. The container starts empty.
 */
      JVal* mkArray();
#endif
      AllocatorIntf* dAlloc;
      AllocatorIntf* vAlloc;
} JValFact;
#ifdef __cplusplus
extern "C" {
#endif
/** @copydoc JValFact::mkString
    @param o Required initialized factory.
 */
#define JValFact_mkString(o, v) JValFact_mkVal(o, JVType_String, v)
/** @copydoc JValFact::mkDouble
    @param o Required initialized factory.
    The C macro requires v to be an addressable variable of its exact numeric type.
 */
#define JValFact_mkDouble(o, v) JValFact_mkVal(o, JVType_Double, &v)
/** @copydoc JValFact::mkInt
    @param o Required initialized factory.
    The C macro requires v to be an addressable variable of its exact numeric type.
 */
#define JValFact_mkInt(o, v) JValFact_mkVal(o, JVType_Int, &v)
/** @copydoc JValFact::mkLong
    @param o Required initialized factory.
    The C macro requires v to be an addressable variable of its exact numeric type.
 */
#define JValFact_mkLong(o, v) JValFact_mkVal(o, JVType_Long, &v)
/** @copydoc JValFact::mkBoolean
    @param o Required initialized factory.
    The C macro requires v to be an addressable variable of its exact numeric type.
 */
#define JValFact_mkBoolean(o, v) JValFact_mkVal(o, JVType_Boolean, &v)
/** @copydoc JValFact::mkNull
    @param o Required initialized factory.
 */
#define JValFact_mkNull(o) JValFact_mkVal(o, JVType_Null, 0)
/** @copydoc JValFact::mkObject
    @param o Required initialized factory.
 */
#define JValFact_mkObject(o) JValFact_mkVal(o, JVType_Object, 0)
/** @copydoc JValFact::mkArray
    @param o Required initialized factory.
 */
#define JValFact_mkArray(o) JValFact_mkVal(o, JVType_Array, 0)
/** @copydoc JValFact::JValFact
    @param o Required storage to initialize.
 */
BA_API void JValFact_constructor(
   JValFact* o, AllocatorIntf* vAlloc, AllocatorIntf* dAlloc);
/** Allocate a detached typed value.
    @param o Required initialized factory.
    @param t Supported JVType, excluding InvalidType; Double requires double support.
    @param uv Pointer to the matching S32/S64/double/BaBool scalar, or the required
    NUL-terminated string itself for String. Ignored for Null/Object/Array.
    Values and strings are copied, not retained.
    @return Caller-owned node, or NULL on allocation failure. Empty objects/arrays
    have no children. Free with the factory's node and data allocators.
 */
BA_API JVal* JValFact_mkVal(JValFact* o, JVType t, const void* uv);
#ifdef __cplusplus
}
inline JValFact::JValFact(AllocatorIntf* vAlloc, AllocatorIntf* dAlloc) {
   JValFact_constructor(this,vAlloc, dAlloc); }
inline JVal* JValFact::mkString(const char* v) {
   return JValFact_mkString(this, v); }
inline JVal* JValFact::mkDouble(double v) {
   return JValFact_mkDouble(this, v); }
inline JVal* JValFact::mkInt(S32 v) {
   return JValFact_mkInt(this, v); }
inline JVal* JValFact::mkLong(S64 v) {
   return JValFact_mkLong(this, v); }
inline JVal* JValFact::mkBoolean(bool v) {
   BaBool b = v ? TRUE : FALSE;
   return JValFact_mkBoolean(this, b); }
inline JVal* JValFact::mkNull() {
   return JValFact_mkNull(this); }
inline JVal* JValFact::mkObject() {
   return JValFact_mkObject(this); }
inline JVal* JValFact::mkArray() {
   return JValFact_mkArray(this); }
#endif

/** @} */ /* end of JSONRef */

#endif
