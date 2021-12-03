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
 *   $Id: SimpleXml.h 4915 2021-12-01 18:26:55Z wini $
 *
 *   COPYRIGHT:  Real Time Logic LLC, 2006-2008
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

/*

SimpleXml is a small namespace aware XML parser that is mainly
designed for parsing XML used in protocols such as WebDAV, UPNP, SOAP,
etc. It can also be used as a general purpose XML parser; however, the
parser comes with a number of restrictions:

The parser generates an error if the XML document contains DTDs,
Document Type Definitions.

The parser is not designed to handle xml elements inside data, thus
the following generates a parser error:

<e1>
  <e2> data data</e2>
  data data
</e1>

<e1>
  data
  <e2> data data</e2>
  data
</e1>

The following is OK, but not recommended:

<e1>
  data data
  <e2> data data</e2>
</e1>

OPERATION

Sax based parsers, i.e. callback based parsers, are generally hard to use
and create a lot of extra work. DOM based parsers are
much easier to use, but a full DOM based parser requires a lot of RAM.

The SimpleXml parser is similar to a DOM based parser, but uses much
less RAM. The parser requires that the document is in memory. The
document is no longer a valid XML document after calling the parser as
the parser directly uses the XML document for breaking the data into
XML node attributes and data. The parser also requires a second data
buffer for storing SXmlNode elements.

The parser and all SXmlNodes use offset positions into the buffer and
not C pointers for referencing other nodes and attributes. The size of
the offset variables is a U16 type and the maximum size of an XML
document can, for this reason, not be larger than 64K.

The parser is designed to handle XML namespaces and also nested
namespaces. The namespaces are kept on an internal stack during
parsing.  The parser can also handle CDATA sections, but the CDATA
sections follow the same limitations as described above.

The following CDATA section is OK:
<e1>
  <![CDATA[
  data
  data
  ]]>
</e1>

The following CDATA section fails:
<e1>
  data
  <![CDATA[
  data
  ]]>
</e1>

XML processing instructions are handled like XML comments; they are
ignored.  The parser validates the initial XML
processing instruction in the XML document, but the character type is
ignored. The parser assumes data is in ASCII or UTF8.

The element data is returned "as is" to the application using the
parser. The application must do all necessary transformations, if
needed.
*/



#ifndef __SimpleXml_h
#define __SimpleXml_h

#include <BaServerLib.h>
#include <HttpServer.h>
#include <IoIntf.h>
#include <setjmp.h>
#include <SingleList.h>

struct SXmlRoot;

/* setjmp/longjmp Exception
 */
typedef struct
{
   jmp_buf buf;
} SlException;

#define SlException_INIT(ex) setjmp(ex.buf)
void SlException_set(SlException* o, int err);
#define SlException_assert(o, expr) \
  if( !(expr) ) SlException_set(o, 1)
#define SlException_assertE(o, expr, errCode) \
  if( !(expr) ) SlException_set(o, errCode)

typedef enum
{
   SXmlErrT_OK,
   SXmlErrT_EOF,
   SXmlErrT_Lex,
   SXmlErrT_ExpectedEndElem,
   SXmlErrT_NameNotFound,
   SXmlErrT_NsNotFound,
   SXmlErrT_ExpectedElement,
   SXmlErrT_Mem,
   SXmlErrT_Err
} SXmlErrT;


/* XML element
 */
typedef struct
{
      U16 ns;   /* Full namespace name. 0 if no namespace. */
      U16 name; /* Element name. */
      U16 data; /* Data if any. 0 if no data. */
      U16 next; /* Next element if any. 0 if no element. */
      U16 childNodes; /* Number of child nodes. */
      U16 firstChild; /* First child element if any. 0 if no element. */
      U16 attributes[1]; /* 0 if no attributes or number of attributes if non
                            0. Attributes follow as ns,name,value -- i.e. three
                            array positions per attribute. */
} SXmlNode;


#define SXmlNode_getNs(o, r) (o->ns?SXmlRoot_dOffs2Str(r,o->ns):"")
#define SXmlNode_getNsId(o) ((unsigned int)o->ns)
#define SXmlNode_childNodes(o) ((unsigned int)o->childNodes)
#define SXmlNode_getName(o, r) SXmlRoot_dOffs2Str(r,o->name)
#define SXmlNode_getData(o, r) (o->data?SXmlRoot_dOffs2Str(r,o->data):0)
#define SXmlNode_getNoOfAttr(o) ((unsigned int)o->attributes[0])
#define SXmlNode_getAttr(o) (o->attributes[0]?o->attributes+1:0)
#define SXmlNode_next(o, r) (o->next?SXmlRoot_eOffs2Elem(r,o->next):0)
#define SXmlNode_firstChild(o, r) \
   (o->firstChild?SXmlRoot_eOffs2Elem(r,o->firstChild):0)
void SXmlNode_unlinkChild(SXmlNode* o, SXmlNode* child, struct SXmlRoot* r);

typedef U16* SXmlAttr;
#define SXmlAttr_getNs(o, r, n) (o[3*n]?SXmlRoot_dOffs2Str(r,o[3*n]):"")
#define SXmlAttr_getNsId(o, n) o[3*n]
#define SXmlAttr_getName(o, r, n) SXmlRoot_dOffs2Str(r,o[1+(3*n)])
#define SXmlAttr_getValue(o, r, n) SXmlRoot_dOffs2Str(r,o[2+(3*n)])


typedef struct
{
      U16 ns;   /* Full namespace name. */
      U16 next; /*We store SXmlNsNode instances as a U16 linked list.*/
} SXmlNsNode;

#define SXmlNsNode_getNs(o, r) SXmlRoot_dOffs2Str(r,o->ns)
#define SXmlNsNode_getNsId(o) ((unsigned int)o->ns)
#define SXmlNsNode_next(o, r) (o->next?SXmlRoot_eOffs2SXmlNsNode(r, o->next):0)


typedef struct SXmlRoot
{
      AllocatorIntf* alloc;
      SlException* ex;
      U8* xmlElemBuf; /* Buffer/container for SXmlNode objects. */
      int line;
      U8* xmlData;
      U16 xmlElemBufSize;
      U16 xmlElemBufNextPos; /* Position for where to allocate next SXmlNode. */
      U16 dataSize;
      U16 parserPos;
      U16 endTagPos;
      U16 xmlnsPos; /* Position to first Xmlns instance. */
      U16 xmlnsFreeList; /* List of free Xmlns objects. */
      U16 xmlnsNodePos; /* Position to first SXmlNsNode instance. */
      U16 csl; /* Current Scope Level (when parsing). */
} SXmlRoot;

void SXmlRoot_constructor(SXmlRoot* o, AllocatorIntf* alloc);

void SXmlRoot_constructor2(SXmlRoot* o, AllocatorIntf* alloc,
                          U8* xmlData, U16 dataSize, U16 elemBufStartSize,
                          SlException* ex);


void SXmlRoot_destructor(SXmlRoot* o);
int SXmlRoot_parse(
   SXmlRoot* o, U8* xmlData, U16 dataSize, U16 elemBufStartSize);
U16 SXmlRoot_childNodes(SXmlRoot* o);
SXmlNode* SXmlRoot_firstChild(SXmlRoot* o);
SXmlNsNode* SXmlRoot_firstNsNode(SXmlRoot* o);


/* Inline functions for converting to and from offset values in the 
   xmlElemBuf.
*/
#define SXmlRoot_eOffs2Xmlns(o, offs) ((Xmlns*)((o)->xmlElemBuf+offs))
#define SXmlRoot_eOffs2SXmlNsNode(o, offs) ((SXmlNsNode*)((o)->xmlElemBuf+offs))
#define SXmlRoot_eOffs2Elem(o, offs) ((SXmlNode*)((o)->xmlElemBuf+offs))
#define SXmlRoot_ePtr2Offs(o, ptr) (U16)(((U8*)ptr) - (o)->xmlElemBuf)

/* Inline functions for converting to and from offset values in the 
   xmlData buffer.
*/

#define SXmlRoot_dOffs2Str(o, offs) ((char*)((o)->xmlData+offs))


struct SerializeSXml;

typedef struct
{
      SplayTree super;
      SingleList nsTreeNodeList;
      AllocatorIntf* alloc;
      SlException* ex;
      unsigned int nextNsId;
} NsTree;

void NsTree_addNs(NsTree* o, SXmlNode* n, SXmlRoot* r);
void NsTree_addAllNs(NsTree* o, SXmlNode* n, SXmlRoot* r);
unsigned int NsTree_getNsId(NsTree* o, const char* ns);
void NsTree_printNsAttrList(NsTree* o, const char nsprfx, BufPrint* out);
struct SerializeSXml* NsTree_createSerializer(
   NsTree* o, const char nsprfx, BufPrint* bp);
void NsTree_constructor(NsTree* o, AllocatorIntf* alloc, SlException* ex);
void NsTree_destructor(NsTree* o);

typedef struct SerializeSXml
{
      BufPrint* bp;
      NsTree* nst;
      char nsprfx;
      BaBool printNsAttrList;
} SerializeSXml;

void SerializeSXml_printNode(
   SerializeSXml* o,SXmlNode* n,SXmlRoot* r, BaBool all);
#define SerializeSXml_constructor(o,nsprfx,bp) \
  SerializeSXml_constructor2(o,nsprfx,bp,0)
void SerializeSXml_constructor2(
   SerializeSXml* o, const char nsprfx, BufPrint* bp, NsTree* nst);
#define SerializeSXml_printNsAttrList(o) (o)->printNsAttrList=TRUE

#endif
