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
 *   $Id: JVal.h 4915 2021-12-01 18:26:55Z wini $
 *
 *   COPYRIGHT:  Real Time Logic LLC, 2006-2014
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

You can check for errors for each method called, but the methods are
designed such that they abort the operation if the error flag in JErr
is set. Some methods return NULL if an error is detected. You can
check for NULL values, but all methods are designed such that they
accept a NULL "this" pointer.

<h2>Examples:</h2>

See the JSON tutorial, section
[Using JParserValFact](@ref UsingJParserValFact)
, for examples on how to use JVal.

*/
struct JVal
{
#ifdef __cplusplus
      /** Returns the JSON type.
       */
      JVType getType();

      /** Equivalent to get with variable argument list replaced by argList.
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
      <tr><td>JVal[size]</td><td> A </td><td>Array of JVal</td>
      <td>All getXXX methods</td></tr>
      </table>

      See the JSON tutorial, section
      [Using JParserValFact](@ref UsingJParserValFact)
      , for examples on how to use JVal.

      \sa JDecoder::get
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
       */
      JVal* get(JErr* err, const char* fmt, ...);

      /** Returns the integer value for an integer JVal type or
          attempts to convert the value to an integer. JErr is set if the
          method cannot convert the value.
       */
      S32 getInt(JErr* e);

      /** Sets a new integer value or changes the JSON type and sets the
          integer value.
      */
      void setInt(JErr* e,S32 v);

      /** Returns the integer as a long value for an integer JVal type or
          attempts to convert the value to a long integer. JErr is set if the
          method cannot convert the value.
       */
      S64 getLong(JErr* e);

      /** Sets a new integer value or changes the JSON type and sets the
          integer value.
      */
      void setLong(JErr* e,S64 v);

      /** Returns the double value for a JVal float or
          attempts to convert the value to a float. JErr is set if the
          method cannot convert the value.
       */
      double getDouble(JErr* e);

      /** Sets a new float value or changes the JSON type to float and sets the
          float value.
      */
      void setDouble(JErr* e,double v);

      /** Returns the boolean value for a boolean JVal type or false if the
          JVal is JVType_Null. JErr is set for all other types.
      */
      BaBool getBoolean(JErr* e);

      /** Sets a new boolean value or changes the JSON type and sets the
          boolean value.
      */
      void setBoolean(JErr* e,BaBool v);

      /** Changes the JSON type if not NULL.
      */
      void setNull(JErr* e);

      /** Returns the string as a const or returns NULL if the JVAL is
          a NULL type. JErr is set for all other types.
       */
      const char* getString(JErr* e);

      /** Sets a string value and changes the JSON type if needed. The
          pointer is directly stored and not copied. The string must,
          therefore, have been allocated with the dynamic allocator
          used by the JVal tree. You cannot set a new string without
          managing the old string.
       */
      void setString(JErr* e, char* v);

      /** Similar to getString, but you must manage the value as the
          value is detached from the tree.
       */
      char* manageString(JErr* e);

      /** Returns the member name if this value is part of a JSON object.
       */
      const char* getName();

      /** Similar to getName, but you must manage the value as the
          value is detached from the tree.
       */
      char* manageName();

      /** Returns the next element if the parent is an object or an array.
       */
      JVal* getNextElem();

      /** Returns the first child if an object. JErr is set for all
          other types.
       */
      JVal* getObject(JErr* e);

      /** Returns the first child if an array. JErr is set for all other types.
       */
      JVal* getArray(JErr* e);

      /** Returns the first child if an array or object. JErr is set for all
          other types.
       */
      JVal* getJ(JErr* e);

      /** Similar to getJ, but you must manage the JVal as the
          value is detached from the tree.
       */
      JVal* manageJ(JErr* e);

      /** Returns the elements left in the list. Returns the number of
          childs if called on the value returned by getObject,
          getArray, or getJ.
       */
      S32 getLength(JErr* e);

      /** Returns true if this is a child element in an object.
          Returns false if this is a child element in an array.
      */
      bool isObjectMember();

      /** Remove a child. Typically used together with method terminate.
          \param child is the child to remove.
       */
      int unlink(JVal* child);

      /** Add a child to an object.
          \param e the error container.
          \param memberName is the object member name.
          \param child is the element to add.
          \param dAlloc is the allocator used for copying the memberName.
      */
      int addMember(JErr* e, const char* memberName,
                    JVal* child, AllocatorIntf* dAlloc);

      /** Add a child to an array.
          \param e the error container.
          \param child is the element to add.
      */
      int add(JErr* e, JVal* child);

      /** Terminate the node and all sub nodes. The element to
          terminate must be managed.
          \param vAlloc is the allocator that was used for allocating
          the nodes.
          \param dAlloc is the allocator that was used for allocating
          the strings.

          \sa unlink manageJ JParserValFact::manageFirstVal
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
      JVType type;
};

typedef struct JVal JVal;

#ifdef __cplusplus
extern "C" {
#endif
#define JVal_getType(o) (o)->type
BA_API JVal* JVal_vget(JVal* o,JErr* err,const char** fmt, va_list* argList);
BA_API JVal* JVal_get(JVal* o, JErr* err, const char* fmt, ...);
BA_API S32 JVal_getInt(JVal* o, JErr* e);
BA_API S64 JVal_getLong(JVal* o, JErr* e);
BA_API double JVal_getDouble(JVal* o, JErr* e);
BA_API BaBool JVal_getBoolean(JVal* o, JErr* e);
BA_API const char* JVal_getString(JVal* o, JErr* e);
BA_API char* JVal_manageString(JVal* o, JErr* e);
BA_API const char* JVal_getName(JVal* o);
BA_API char* JVal_manageName(JVal* o);
#define JVal_getNextElem(o) (o)->next
BA_API JVal* JVal_getObject(JVal* o, JErr* e);
BA_API JVal* JVal_getArray(JVal* o, JErr* e);
BA_API JVal* JVal_getJ(JVal* o, JErr* e);
BA_API JVal* JVal_manageJ(JVal* o, JErr* e);
BA_API S32 JVal_getLength(struct JVal* o, JErr* e);
#define JVal_isObjectMember(o) ((o)->memberName != 0) 
BA_API int JVal_unlink(JVal* o, JVal* child);
BA_API int JVal_addMember(JVal* o, JErr* e, const char* memberName,
                          JVal* child, AllocatorIntf* dAlloc);
BA_API int JVal_add(JVal* o, JErr* e, JVal* child);
BA_API void JVal_terminate(JVal* o, AllocatorIntf* vAlloc,
                           AllocatorIntf* dAlloc);
#define JVal_setInt(o, e, v) JVal_setX(o, e, JVType_Int, &v)
#define JVal_setLong(o, e, v) JVal_setX(o, e, JVType_Long, &v)
#define JVal_setDouble(o, e, v) JVal_setX(o, e, JVType_Double, &v)
#define JVal_setBoolean(o, e, v) JVal_setX(o, e, JVType_Boolean, &v)
#define JVal_setNull(o, e) JVal_setX(o, e, JVType_Null, 0)
#define JVal_setString(o, e, v) JVal_setX(o, e, JVType_String, v)
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
inline char* JVal::manageString(JErr* e){
   return JVal_manageString(this, e); }
inline const char* JVal::getName(){
   return JVal_getName(this); }
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

inline void JVal::setInt(JErr* e,S32 v) {
  JVal_setInt(this, e, v); }
inline void JVal::setLong(JErr* e,S64 v) {
  JVal_setLong(this, e, v); }
inline void JVal::setDouble(JErr* e,double v) {
  JVal_setDouble(this, e, v); }
inline void JVal::setBoolean(JErr* e,BaBool v) {
  JVal_setBoolean(this, e, v); }
inline void JVal::setNull(JErr* e) {
  JVal_setNull(this, e); }
inline void JVal::setString(JErr* e, char* v) {
  JVal_setString(this, e, v); }
inline bool JVal::isObjectMember(){
   return JVal_isObjectMember(this) ? true : false; }
inline int JVal::unlink(JVal* child){
   return JVal_unlink(this, child); }
inline int JVal::addMember(JErr* e, const char* memberName,
                           JVal* child, AllocatorIntf* dAlloc) {
   return JVal_addMember(this, e, memberName, child, dAlloc); }
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
    \sa JDecode
*/
#ifdef __cplusplus
typedef struct JParserValFact : public JParserIntf
{
      /** create a JParserValFact JVal factory instance.
      \param vAlloc is used when allocating nodes.
      \param dAlloc is used when allocating strings. The two
      allocators can be the same.
      */
      JParserValFact(AllocatorIntf* vAlloc, AllocatorIntf* dAlloc);

      /** The destructor terminates all nodes not managed.
       */
      ~JParserValFact();

      /** Returns the root of the JVal syntax tree. 
       */
      JVal* getFirstVal();

      /** Similar to getFirstVal, but you must manage the JVal tree as the
          value is detached from the JParserValFact.
       */
      JVal* manageFirstVal();

      /** Terminate the syntax tree such that the JParserValFact
          instance can be reused when the JParser instance parses the
          next JSON object in a stream.
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
      U32 maxNodes;
      JParserValFactStat status;
} JParserValFact;
#ifdef __cplusplus
extern "C" {
#endif
BA_API void JParserValFact_constructor(
   JParserValFact* o, AllocatorIntf* vAlloc, AllocatorIntf* dAlloc);
#define JParserValFact_getFirstVal(o) \
   ((o)->vStack && *(o)->vStack ? *(o)->vStack : 0)
BA_API JVal* JParserValFact_manageFirstVal(JParserValFact* o);
BA_API void JParserValFact_termFirstVal(JParserValFact* o);
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
      /** create a JValFact JVal factory instance.
      \param vAlloc is used when allocating nodes.
      \param dAlloc is used when allocating strings. The two
      allocators can be the same.
      */
      JValFact(AllocatorIntf* vAlloc, AllocatorIntf* dAlloc);
      
      /** Make a string. Returns null if the allocators failed.
       */
      JVal* mkString(const char* v);

      /** Make a float. Returns null if the allocator failed.
       */
      JVal* mkDouble(double v);

      /** Make an integer. Returns null if the allocator failed.
       */
      JVal* mkInt(S32 v);

      /** Make a long integer. Returns null if the allocator failed.
       */
      JVal* mkLong(S64 v);

      /** Make a boolean. Returns null if the allocator failed.
       */
      JVal* mkBoolean(bool v);

      /** Make a null value. Returns null if the allocator failed.
       */
      JVal* mkNull();

      /** Make a JSON object. Returns null if the allocator failed.
       */
      JVal* mkObject();

      /** Make a JSON array. Returns null if the allocator failed.
       */
      JVal* mkArray();
#endif
      AllocatorIntf* dAlloc;
      AllocatorIntf* vAlloc;
} JValFact;
#ifdef __cplusplus
extern "C" {
#endif
#define JValFact_mkString(o, v) JValFact_mkVal(o, JVType_String, v)
#define JValFact_mkDouble(o, v) JValFact_mkVal(o, JVType_Double, &v)
#define JValFact_mkInt(o, v) JValFact_mkVal(o, JVType_Int, &v)
#define JValFact_mkLong(o, v) JValFact_mkVal(o, JVType_Long, &v)
#define JValFact_mkBoolean(o, v) JValFact_mkVal(o, JVType_Boolean, &v)
#define JValFact_mkNull(o) JValFact_mkVal(o, JVType_Null, 0)
#define JValFact_mkObject(o) JValFact_mkVal(o, JVType_Object, 0)
#define JValFact_mkArray(o) JValFact_mkVal(o, JVType_Array, 0)
BA_API void JValFact_constructor(
   JValFact* o, AllocatorIntf* vAlloc, AllocatorIntf* dAlloc);
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
