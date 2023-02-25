/*
 *     ____             _________                __                _
 *    / __ \___  ____ _/ /_  __(_)___ ___  ___  / /   ____  ____ _(_)____
 *   / /_/ / _ \/ __ `/ / / / / / __ `__ \/ _ \/ /   / __ \/ __ `/ / ___/
 *  / _, _/  __/ /_/ / / / / / / / / / / /  __/ /___/ /_/ / /_/ / / /__
 * /_/ |_|\___/\__,_/_/ /_/ /_/_/ /_/ /_/\___/_____/\____/\__, /_/\___/
 *                                                       /____/
 *
 *                 SharkSSL Embedded SSL/TLS Stack
 ****************************************************************************
 *   PROGRAM MODULE
 *
 *   $Id: SharkSslASN1.h 3670 2015-03-28 21:25:15Z gianluca $
 *
 *   COPYRIGHT:  Real Time Logic LLC, 2018 - 2022
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
 *               http://www.sharkssl.com
 ****************************************************************************
 *
 */
#ifndef _SharkSslASN1_h
#define _SharkSslASN1_h

/*! \file SharkSslASN1.h 
     \brief ASN.1 macros
*/


#include "SharkSslEx.h"

/** ASN.1-DER API
 *  http://luca.ntop.org/Teaching/Appunti/asn1.html
 *  for X.509 
 *  https://www.cs.auckland.ac.nz/~pgut001/pubs/x509guide.txt
 *  http://www.itu.int/ITU-T/studygroups/com10/languages/X.690_1297.pdf
 */
#define SHARKSSL_ASN1_BOOLEAN                         0x01
#define SHARKSSL_ASN1_INTEGER                         0x02
#define SHARKSSL_ASN1_BIT_STRING                      0x03
#define SHARKSSL_ASN1_OCTET_STRING                    0x04
#define SHARKSSL_ASN1_NULL                            0x05
#define SHARKSSL_ASN1_OID                             0x06
#define SHARKSSL_ASN1_UTF8_STRING                     0x0C
#define SHARKSSL_ASN1_SEQUENCE                        0x10
#define SHARKSSL_ASN1_SET                             0x11
#define SHARKSSL_ASN1_PRINTABLE_STRING                0x13
#define SHARKSSL_ASN1_T61_STRING                      0x14
#define SHARKSSL_ASN1_IA5_STRING                      0x16
#define SHARKSSL_ASN1_UTC_TIME                        0x17
#define SHARKSSL_ASN1_GENERALIZED_TIME                0x18
#define SHARKSSL_ASN1_BMP_STRING                      0x1E
#define SHARKSSL_ASN1_CONSTRUCTED                     0x20
#define SHARKSSL_ASN1_CONTEXT_SPECIFIC                0x80

/**
 * {joint-iso-itu-t(2) ds(5) attributeType(4)
 */
#define SHARKSSL_OID_JIIT_DS                          0x55
#define SHARKSSL_OID_JIIT_DS_ATTRTYPE                 0x04
#define SHARKSSL_OID_JIIT_DS_ATTRTYPE_CN              0x03
#define SHARKSSL_OID_JIIT_DS_ATTRTYPE_SERIAL          0x05
#define SHARKSSL_OID_JIIT_DS_ATTRTYPE_COUNTRY         0x06
#define SHARKSSL_OID_JIIT_DS_ATTRTYPE_LOCALITY        0x07
#define SHARKSSL_OID_JIIT_DS_ATTRTYPE_PROVINCE        0x08
#define SHARKSSL_OID_JIIT_DS_ATTRTYPE_ORGANIZATION    0x0A
#define SHARKSSL_OID_JIIT_DS_ATTRTYPE_UNIT            0x0B
/**
 * {joint-iso-itu-t(2) ds(5) certificateExtension(29)
 */
#define SHARKSSL_OID_JIIT_DS_CERTEXT                  0x1D
#define SHARKSSL_OID_JIIT_DS_CERTEXT_KEYUSAGE         0x0F
#define SHARKSSL_OID_JIIT_DS_CERTEXT_SUBJALTNAMES     0x11
#define SHARKSSL_OID_JIIT_DS_CERTEXT_BASICCONSTRAINTS 0x13

/** 
 * PKCS #10: Certificate Request tags
 * https://tools.ietf.org/html/rfc2986
 *
 */
#define SHARKSSL_CSR_TAG_ATTRIBUTES                   0x00

/**
 * X.509v3 tags
 * https://www.cs.auckland.ac.nz/~pgut001/pubs/x509guide.txt
 * https://www.ietf.org/rfc/rfc2459.txt
 */
#define SHARKSSL_X509_TAG_VERSION                     0x00
#define SHARKSSL_X509_TAG_ISSUERUNIQUEID              0x01
#define SHARKSSL_X509_TAG_SUBJECTUNIQUEID             0x02
#define SHARKSSL_X509_TAG_EXTENSIONS                  0x03

/**
 * ECC private key tags
 * https://www.ietf.org/rfc/rfc5915.txt
 */
#define SHARKSSL_ECC_PRIVKEY_TAG_PARAMETERS           0x00
#define SHARKSSL_ECC_PRIVKEY_TAG_PUBLIC_KEY           0x01

/**
 * X.509 v3 Key Usage Extension flags
 */
#define SHARKSSL_X509_KU_DIGITAL_SIGNATURE            0x80
#define SHARKSSL_X509_KU_NON_REPUDIATION              0x40
#define SHARKSSL_X509_KU_KEY_ENCIPHERMENT             0x20
#define SHARKSSL_X509_KU_DATA_ENCIPHERMENT            0x10
#define SHARKSSL_X509_KU_KEY_AGREEMENT                0x08
#define SHARKSSL_X509_KU_KEY_CERT_SIGN                0x04
#define SHARKSSL_X509_KU_CRL_SIGN                     0x02

/**
 * Netscape certificate types
 * https://developer.mozilla.org/en-US/docs/Mozilla/Projects/NSS/nss_tech_notes/nss_tech_note3
 */
#define SHARKSSL_X509_NS_CERT_TYPE_SSL_CLIENT         0x80
#define SHARKSSL_X509_NS_CERT_TYPE_SSL_SERVER         0x40
#define SHARKSSL_X509_NS_CERT_TYPE_OBJECT_SIGNING     0x10
#define SHARKSSL_X509_NS_CERT_TYPE_SSL_CA             0x04
#define SHARKSSL_X509_NS_CERT_TYPE_OBJECT_SIGNING_CA  0x01


typedef struct SubjectAltNameEnumerator SharkSslParseASN1;
typedef SharkSslParseASN1 SharkSslASN1;


/* OID's - length has to match the content in SharkSslASN1.c */
extern const U8 sharkssl_oid_CN[3];
extern const U8 sharkssl_oid_serial[3];
extern const U8 sharkssl_oid_country[3];
extern const U8 sharkssl_oid_locality[3];
extern const U8 sharkssl_oid_province[3];
extern const U8 sharkssl_oid_organization[3];
extern const U8 sharkssl_oid_unit[3];
extern const U8 sharkssl_oid_emailAddress[9];
extern const U8 sharkssl_oid_csr_ext_req[9];
extern const U8 sharkssl_oid_signedData[9];
extern const U8 sharkssl_oid_ns_cert_type[9];
extern const U8 sharkssl_oid_key_usage[3];
extern const U8 sharkssl_oid_san[3];
extern const U8 sharkssl_oid_basic_constraints[3];
extern const U8 sharkssl_oid_ecPublicKey[7];
extern const U8 sharkssl_oid_prime192v1[8];
extern const U8 sharkssl_oid_prime256v1[8];
extern const U8 sharkssl_oid_ecdsaWithSHA1[7];
extern const U8 sharkssl_oid_ecdsaWithSHA256[8];
extern const U8 sharkssl_oid_ecdsaWithSHA384[8];
extern const U8 sharkssl_oid_ecdsaWithSHA512[8];
extern const U8 sharkssl_oid_secp224r1[5];
extern const U8 sharkssl_oid_secp384r1[5];
extern const U8 sharkssl_oid_secp521r1[5];
extern const U8 sharkssl_oid_brainpoolP256r1[9];
extern const U8 sharkssl_oid_brainpoolP384r1[9];
extern const U8 sharkssl_oid_brainpoolP512r1[9];
extern const U8 sharkssl_oid_curve25519[9];
extern const U8 sharkssl_oid_rsaEncryption[9];          
extern const U8 sharkssl_oid_md2withRSAEncryption[9];
extern const U8 sharkssl_oid_md5withRSAEncryption[9];
extern const U8 sharkssl_oid_sha1withRSAEncryption[9];  
extern const U8 sharkssl_oid_sha256withRSAEncryption[9];
extern const U8 sharkssl_oid_sha384withRSAEncryption[9];
extern const U8 sharkssl_oid_sha512withRSAEncryption[9];
extern const U8 sharkssl_oid_md5[8];
extern const U8 sharkssl_oid_sha1[5];
extern const U8 sharkssl_oid_sha256[9];
extern const U8 sharkssl_oid_sha384[9];
extern const U8 sharkssl_oid_sha512[9];
extern const U8 sharkssl_oid_pkcs5PBES2[9];
extern const U8 sharkssl_oid_pkcs5PBKDF2[9];
extern const U8 sharkssl_oid_HMACWithSHA256[8];
#if SHARKSSL_ENABLE_AES_CBC
extern const U8 sharkssl_oid_aes128cbc[9];
extern const U8 sharkssl_oid_aes256cbc[9];
#endif


#define sharkssl_oid_ex(type)  (const U8*)sharkssl_oid_ ## type, (sizeof(sharkssl_oid_ ## type)/sizeof(sharkssl_oid_ ## type[0]))



/* never call these functions directly but only through the macros below */
int SharkSslParseASN1_getLength(SharkSslParseASN1 *o);
SHARKSSL_API int SharkSslParseASN1_getType(SharkSslParseASN1 *o, U8 ASN1type);  /* dataptr, datalen are valid */
SHARKSSL_API int SharkSslParseASN1_getSetSeq(SharkSslParseASN1 *o, U8 ASN1id);

#define SharkSslParseASN1_getBool(o)            SharkSslParseASN1_getType(o, SHARKSSL_ASN1_BOOLEAN)
#define SharkSslParseASN1_getInt(o)             SharkSslParseASN1_getType(o, SHARKSSL_ASN1_INTEGER)
#define SharkSslParseASN1_getBitString(o)       SharkSslParseASN1_getType(o, SHARKSSL_ASN1_BIT_STRING)
#define SharkSslParseASN1_getOctetString(o)     SharkSslParseASN1_getType(o, SHARKSSL_ASN1_OCTET_STRING)
#define SharkSslParseASN1_getNULL(o)            SharkSslParseASN1_getType(o, SHARKSSL_ASN1_NULL)
#define SharkSslParseASN1_getOID(o)             SharkSslParseASN1_getType(o, SHARKSSL_ASN1_OID)
#define SharkSslParseASN1_getUTCTime(o)         SharkSslParseASN1_getType(o, SHARKSSL_ASN1_UTC_TIME) 
#define SharkSslParseASN1_getGenTime(o)         SharkSslParseASN1_getType(o, SHARKSSL_ASN1_GENERALIZED_TIME)
#define SharkSslParseASN1_getIssuerUniqueID(o)  SharkSslParseASN1_getType(o, SHARKSSL_ASN1_CONTEXT_SPECIFIC | SHARKSSL_ASN1_CONSTRUCTED | SHARKSSL_X509_TAG_ISSUERUNIQUEID)
#define SharkSslParseASN1_getSubjectUniqueID(o) SharkSslParseASN1_getType(o, SHARKSSL_ASN1_CONTEXT_SPECIFIC | SHARKSSL_ASN1_CONSTRUCTED | SHARKSSL_X509_TAG_SUBJECTUNIQUEID)
#define SharkSslParseASN1_getExtensions(o)      SharkSslParseASN1_getType(o, SHARKSSL_ASN1_CONTEXT_SPECIFIC | SHARKSSL_ASN1_CONSTRUCTED | SHARKSSL_X509_TAG_EXTENSIONS)
#define SharkSslParseASN1_getCSRAttributes(o)   SharkSslParseASN1_getType(o, SHARKSSL_ASN1_CONTEXT_SPECIFIC | SHARKSSL_ASN1_CONSTRUCTED | SHARKSSL_CSR_TAG_ATTRIBUTES)

#define SharkSslParseASN1_getECParameters(o)    SharkSslParseASN1_getSetSeq(o, SHARKSSL_ASN1_CONTEXT_SPECIFIC | SHARKSSL_ASN1_CONSTRUCTED | SHARKSSL_ECC_PRIVKEY_TAG_PARAMETERS)
#define SharkSslParseASN1_getECPublicKey(o)     SharkSslParseASN1_getSetSeq(o, SHARKSSL_ASN1_CONTEXT_SPECIFIC | SHARKSSL_ASN1_CONSTRUCTED | SHARKSSL_ECC_PRIVKEY_TAG_PUBLIC_KEY)
#define SharkSslParseASN1_getVersion(o)         SharkSslParseASN1_getSetSeq(o, SHARKSSL_ASN1_CONTEXT_SPECIFIC | SHARKSSL_ASN1_CONSTRUCTED | SHARKSSL_X509_TAG_VERSION)
#define SharkSslParseASN1_getSet(o)             SharkSslParseASN1_getSetSeq(o, SHARKSSL_ASN1_CONSTRUCTED | SHARKSSL_ASN1_SET)
#define SharkSslParseASN1_getSequence(o)        SharkSslParseASN1_getSetSeq(o, SHARKSSL_ASN1_CONSTRUCTED | SHARKSSL_ASN1_SEQUENCE)

int SharkSslParseASN1_getContextSpecific(SharkSslParseASN1 *o, U8 *tag);


/** 
Opaque object used when creating
[ASN.1](https://en.wikipedia.org/wiki/Abstract_Syntax_Notation_One)
encoded data.
*/
typedef struct SharkSslASN1Create
{
   U8 *start;
   U8 *end;
   U8 *ptr;
} SharkSslASN1Create;

/**
 * https://developer.mozilla.org/en-US/docs/Mozilla/Projects/NSS/nss_tech_notes/nss_tech_note3
 * http://tools.ietf.org/html/rfc5280#section-4.2.1.12
 */
typedef struct SharkSslBitExtReq
{
   U8 bits;
} SharkSslBitExtReq;


#define SharkSslASN1Create_getLen(o) ((int)((o)->end - (o)->ptr))
#define SharkSslASN1Create_getData(o) ((o)->ptr)

/** 
  Returns the length of the ASN.1 encoded data.
  \param o ASN.1 instance
  \param startOfDataPtr pointer to the beginning of the ASN.1 encoded data
*/
#define SharkSslASN1Create_getDataLen(o, startOfDataPtr) (*(startOfDataPtr) = (o)->ptr, (int)((o)->end - (o)->ptr))

SHARKSSL_API void SharkSslASN1Create_constructor(SharkSslASN1Create *o, U8 *buf, int bufSize);
SHARKSSL_API int  SharkSslASN1Create_int(SharkSslASN1Create *o, const U8 *intptr, int len);

SHARKSSL_API int  SharkSslASN1Create_length(SharkSslASN1Create *o, int len);
SHARKSSL_API int  SharkSslASN1Create_tag(SharkSslASN1Create *o, U8 ASN1type);

#define SharkSslASN1Create_bitString(o)        SharkSslASN1Create_tag(o, SHARKSSL_ASN1_BIT_STRING)
#define SharkSslASN1Create_octetString(o)      SharkSslASN1Create_tag(o, SHARKSSL_ASN1_OCTET_STRING)
#define SharkSslASN1Create_printableString(o)  SharkSslASN1Create_tag(o, SHARKSSL_ASN1_PRINTABLE_STRING)
#define SharkSslASN1Create_IA5String(o)        SharkSslASN1Create_tag(o, SHARKSSL_ASN1_IA5_STRING)
#define SharkSslASN1Create_extensions(o)       SharkSslASN1Create_tag(o, SHARKSSL_ASN1_CONTEXT_SPECIFIC | SHARKSSL_ASN1_CONSTRUCTED | SHARKSSL_X509_TAG_EXTENSIONS)
#define SharkSslASN1Create_version(o)          SharkSslASN1Create_tag(o, SHARKSSL_ASN1_CONTEXT_SPECIFIC | SHARKSSL_ASN1_CONSTRUCTED | SHARKSSL_X509_TAG_VERSION)
#define SharkSslASN1Create_CSRAttributes(o)    SharkSslASN1Create_tag(o, SHARKSSL_ASN1_CONTEXT_SPECIFIC | SHARKSSL_ASN1_CONSTRUCTED | SHARKSSL_CSR_TAG_ATTRIBUTES)
#define SharkSslASN1Create_ECParameters(o)     SharkSslASN1Create_tag(o, SHARKSSL_ASN1_CONTEXT_SPECIFIC | SHARKSSL_ASN1_CONSTRUCTED | SHARKSSL_ECC_PRIVKEY_TAG_PARAMETERS)
#define SharkSslASN1Create_ECPublicKey(o)      SharkSslASN1Create_tag(o, SHARKSSL_ASN1_CONTEXT_SPECIFIC | SHARKSSL_ASN1_CONSTRUCTED | SHARKSSL_ECC_PRIVKEY_TAG_PUBLIC_KEY)

#define SharkSslASN1Create_set(o)              SharkSslASN1Create_tag(o, SHARKSSL_ASN1_CONSTRUCTED | SHARKSSL_ASN1_SET)
#define SharkSslASN1Create_sequence(o)         SharkSslASN1Create_tag(o, SHARKSSL_ASN1_CONSTRUCTED | SHARKSSL_ASN1_SEQUENCE)

#if (SHARKSSL_ENABLE_CSR_CREATION || SHARKSSL_ENABLE_CSR_SIGNING || SHARKSSL_ENABLE_ASN1_KEY_CREATION)
SHARKSSL_API int SharkSslASN1Create_oid(SharkSslASN1Create *o, const U8 *oid, int oidLen);
SHARKSSL_API int SharkSslASN1Create_raw(SharkSslASN1Create *o, const void *data, int size);
#endif
#if (SHARKSSL_ENABLE_CSR_CREATION || SHARKSSL_ENABLE_CSR_SIGNING)
SHARKSSL_API int SharkSslASN1Create_email(SharkSslASN1Create *o, const U8 *oid, int oidLen, const U8 *email, int emailLen);
SHARKSSL_API int SharkSslASN1Create_name(SharkSslASN1Create *o, const U8 *oid, int oidLen, const U8 *name, int nameLen);
#endif
#if (SHARKSSL_ENABLE_CSR_SIGNING)
SHARKSSL_API int  SharkSslASN1Create_boolean(SharkSslASN1Create *o, U8 boolean);
#endif


/* Semi public (non documented) types and functions */
SHARKSSL_API int sharkssl_PEM_getSeqVersion(SharkSslParseASN1 *parseKey, U32 len);
SHARKSSL_API U8 SharkSslParseASN1_getAlgoID(const SharkSslParseASN1 *o);
SHARKSSL_API U8 SharkSslParseASN1_getCurveID(const SharkSslParseASN1 *o);

/* heterogeneous usage of _getAlgoID to detect ecPublicKey OID in EC
 * public certs
*/
#define ALGO_OID_EC_PUBLIC_KEY             SHARKSSL_OID_EC_PUBLIC_KEY

#define SHARKSSL_EC_CURVE_ID_UNKNOWN    0
#define SHARKSSL_EC_POINT_UNCOMPRESSED  0x04


#endif
