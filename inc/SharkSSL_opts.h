
/*

SharkSSL_opts.h is a user file that you can use to configure
SharkSSL. You do not need to define every option that is defined in
the main configuration file SharkSSL_cfg.h; if you do not define an
option, a default value will be used. In other words, SharkSSL_opts.h
provides a way to override the default configuration options for
SharkSSL.

*/

/*
The following options configure the smallest possible SharkSSL library:
*/

#ifdef SHARKSSL_TINY                                  /* DEFAULT value in SharkSsl_cfg.h */  

#define SHARKSSL_USE_AES_256 0                        /* 1 */
#define SHARKSSL_ENABLE_AES_GCM 0                     /* 1 */
#define SHARKSSL_USE_SHA_384 0                        /* 1 */
#define SHARKSSL_USE_SHA_512 0                        /* 0 */ 
#define SHARKSSL_SSL_SERVER_CODE 0                    /* 1 */
#define SHARKSSL_ENABLE_CLIENT_AUTH 0                 /* 1 */ 
#define SHARKSSL_ENABLE_SNI 1                         /* 1 */
#define SHARKSSL_ENABLE_RSA 0                         /* 1 */
#define SHARKSSL_ENABLE_SESSION_CACHE 0               /* 1 */  
#define SHARKSSL_ENABLE_SECURE_RENEGOTIATION 0        /* 1 */
#define SHARKSSL_ENABLE_DHE_RSA 0                     /* 1 */
#define SHARKSSL_ENABLE_SELECT_CIPHERSUITE 0          /* 1 */
#define SHARKSSL_ENABLE_ALPN_EXTENSION 1              /* 1 */
#define SHARKSSL_ENABLE_RSA_API 0                     /* 0 */
#define SHARKSSL_ENABLE_RSA_PKCS1 0                   /* 1 */
#define SHARKSSL_ENABLE_ECDSA_API 0                   /* 1 */
#define SHARKSSL_ENABLE_PEM_API 0                     /* 1 */
#define SHARKSSL_ENABLE_INFO_API 0                    /* 1 */
#define SHARKSSL_ENABLE_CERT_CHAIN 0                  /* 1 */
#define SHARKSSL_ENABLE_CA_LIST 1                     /* 1 */
#define SHARKSSL_ENABLE_CERTSTORE_API 0               /* 0 */
#define SHARKSSL_SHA256_SMALL_FOOTPRINT 1             /* 0 */
#define SHARKSSL_BIGINT_EXP_SLIDING_WINDOW_K 1        /* 4 */
#define SHARKSSL_ENABLE_AES_CTR_MODE 0                /* 1 */ 
#define SHARKSSL_AES_CIPHER_LOOP_UNROLL 0             /* 1 */
#define SHARKSSL_UNALIGNED_ACCESS 1                   /* nd */
#define SHARKSSL_BIGINT_WORDSIZE 32                   /* 32 */
#define SHARKSSL_USE_ECC 1                            /* 1 */ 
#define SHARKSSL_ENABLE_ECDSA 1                       /* 1 */
#define SHARKSSL_ECC_VERIFY_POINT 0                   /* 1 */
#define SHARKSSL_ECC_TIMING_RESISTANT 0               /* 0 */
#define SHARKSSL_ECC_USE_SECP521R1 0                  /* 1 */
#define SHARKSSL_ECC_USE_BRAINPOOLP512R1 0            /* 1 */
#define SHARKSSL_ENABLE_ECDHE_RSA 0                   /* 1 */
#define SHARKSSL_USE_RNG_TINYMT 1                     /* 0 */


#ifndef BASIC_TRUST_CHECK
#define BASIC_TRUST_CHECK
#endif

#ifndef NDEBUG
#define NDEBUG
#endif


#endif
