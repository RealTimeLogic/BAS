/* Copyright (c) Real Time Logic LLC, 2026. All rights reserved.
 * Use is subject to the Barracuda license supplied with this software.
 */
#include "start.h"
#include <BaDiskIo.h>
#include <HttpServCon.h>
#include <SharkSslCrypto.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#ifdef SHARKTRUSTX_PRIVATE_CA
#include <BamDNS.h>
#endif
#include "CA_ISRG_Root_X1.h"

/* Platform-specific entropy setup stays here. The server below uses BAS
 * APIs and the selected DiskIo filesystem port. Initialize networking/DNS,
 * the UTC clock, storage, tracing and error handling before barracuda().
 * Standalone programs use the shared Main.c and HostInit.c; an RTOS uses
 * its normal board initialization and server task. */
#if defined(_WIN32)
#include <bcrypt.h>
#endif

/* Seed before generating any credentials, account keys or TLS keys. */
static int seedRandom(void)
{
#if defined(_WIN32) || defined(__linux__)
   U32 seed[16];
   int i, rc = 0;
#ifdef _WIN32
   rc = BCryptGenRandom(0, (PUCHAR)seed, sizeof(seed),
                        BCRYPT_USE_SYSTEM_PREFERRED_RNG) != 0;
#else
   FILE* random = fopen("/dev/urandom", "rb");
   if(!random)
      rc = -1;
   else
   {
      if(fread(seed, 1, sizeof(seed), random) != sizeof(seed))
         rc = -1;
      if(fclose(random))
         rc = -1;
   }
#endif
   if(!rc)
      for(i = 0; i < 16; ++i)
         if(sharkssl_entropy(seed[i]))
         {
            rc = -1;
            break;
         }
   sharkssl_clear(seed, sizeof(seed));
   return rc;
#else
   /* EXAMPLE ONLY: a function address is predictable, NOT random entropy.
    * Before using TLS on a device, seed SharkSSL using sharkssl_entropy()
    * with real entropy from the board's hardware RNG or another secure
    * source. Replace this placeholder with that platform's seeding code.
    * tokengen credential obfuscation does not seed the random generator. */
   return sharkssl_entropy((U32)(uintptr_t)&seedRandom);
#endif
}

/* The downloaded portal source supplies these when TOKENGEN_C_API is set. */
extern void tokengen_info(const char** domain, const uint8_t** key);
extern int tokengen_proof(const uint8_t* message, size_t length,
                         uint8_t proof[32]);

#ifdef SHARKTRUSTX_PRIVATE_CA
/* The responder and its timer share the server mutex and have server lifetime.
 * Use the target's BamDNS socket/discovery port, without native OS calls here. */
typedef struct
{
   SoDisp* dispatcher;
   BaTimer timer;
   BamDNS responder;
   BaBool initialized;
} LocalName;

/* SharkTrustX calls this with the dispatcher mutex held after confirmation.
 * Advertise only the assigned label; a changed or absent name retires the old
 * responder. The constructor copies the temporary label and network snapshot. */
static void localName(void* context, const char* name, size_t length)
{
   LocalName* local = (LocalName*)context;
   BamDNS_Config config;
   char label[64];
   int rc;
   certificateName(0, name, length);
   if(local->initialized)
   {
      BamDNS_destructor(&local->responder);
      local->initialized = FALSE;
   }
   if(!name)
      return; /* IP-only enrollment has no name to advertise. */
   if(length <= 6 || length - 6 >= sizeof(label) ||
      memcmp(name + length - 6, ".local", 6))
   {
      HttpTrace_printf(0, "mDNS requires a portal-assigned .local name.\n");
      return;
   }
   memcpy(label, name, length - 6);
   label[length - 6] = 0;
   config.name = label;
   config.netInfo = 0; /* Discover the LAN and follow address changes. */
   BamDNS_constructor(&local->responder, local->dispatcher,
                       &local->timer, &config);
   local->initialized = TRUE;
   rc = BamDNS_status(&local->responder);
   if(rc)
      HttpTrace_printf(0, "mDNS start failed (%d); check the network/port "
                       "and restart the example.\n", rc);
   else
      HttpTrace_printf(0, "mDNS advertising %s on the selected LAN.\n", name);
}
#endif

/* Standard example entry point, called once from Main.c or an RTOS task.
 * Retained objects have static lifetime; the dispatcher normally never
 * returns. The target must provide a writable, persistent DiskIo port. */
void barracuda(void)
{
   static DiskIo stateIo;
   static ThreadMutex mutex;
   static SoDisp dispatcher;
   static HttpServer server;
   static HttpServCon http;
#ifdef USE_IPV6
   static HttpServCon http6;
#endif
   static SharkSsl clientSsl;
   static BaTimer timer;
#ifdef SHARKTRUSTX_PRIVATE_CA
   static LocalName local;
#endif
   static SharkTrustX_Config config;
   int rc;

   HttpTrace_setPrio(5);
   DiskIo_constructor(&stateIo);
   rc = DiskIo_setRootDir(&stateIo, EXAMPLE_STATE_DIR);
   if(rc)
   {
      HttpTrace_printf(0, "Cannot set private state directory %s (%d).\n",
                       EXAMPLE_STATE_DIR, rc);
      baFatalE(FE_USER_ERROR_1, rc);
   }
   HttpTrace_printf(0, "Private state: %s\n", EXAMPLE_STATE_DIR);
   if(seedRandom())
   {
      HttpTrace_printf(0, "Cannot seed SharkSSL.\n");
      baFatalE(FE_USER_ERROR_1, 0);
   }
   /* Objects retained by BWS and SharkTrustX outlive every request/callback. */
   ThreadMutex_constructor(&mutex);
   SoDisp_constructor(&dispatcher, &mutex);
   HttpServer_constructor(&server, &dispatcher, 0);
   SharkSsl_constructor(&clientSsl, SharkSsl_Client, 0, 4096, 4096);
   if(!SharkSsl_setCAListEx(&clientSsl, sharkSSL_CA_ISRG_Root_X1,
                          sizeof(sharkSSL_CA_ISRG_Root_X1)))
   {
      HttpTrace_printf(0, "Cannot load the outbound HTTPS trust anchor.\n");
      baFatalE(FE_USER_ERROR_1, 0);
   }
   /* A dedicated worker handles enrollment and renewal using this mutex. */
   BaTimer_constructor(&timer, &mutex, BA_STACKSZ, 10, ThreadPrioNormal, 0);
#ifdef SHARKTRUSTX_PRIVATE_CA
   /* Enrollment can block its worker during HTTPS operations. Give mDNS its
    * own timer so announcements and delayed replies can still run. */
   local.dispatcher = &dispatcher;
   BaTimer_constructor(&local.timer, &mutex, BA_STACKSZ, 10, ThreadPrioNormal, 0);
#endif
   ThreadMutex_set(&mutex);
   installVirtualDir(&server);
   /* Follow Mako/Xedge: an IPv6 socket may also accept IPv4 connections.
    * Keep either or both successful listeners without changing port policy. */
#ifdef USE_IPV6
   HttpServCon_constructor(&http6, &server, &dispatcher, EXAMPLE_HTTP_PORT,
                           TRUE, 0, 0);
#endif
   HttpServCon_constructor(&http, &server, &dispatcher, EXAMPLE_HTTP_PORT,
                           FALSE, 0, 0);
   if(!HttpServCon_isValid(&http)
#ifdef USE_IPV6
      && !HttpServCon_isValid(&http6)
#endif
      )
   {
      HttpTrace_printf(0, "Cannot listen on HTTP port %u.\n", EXAMPLE_HTTP_PORT);
      ThreadMutex_release(&mutex);
      baFatalE(FE_USER_ERROR_1, 0);
   }
#ifdef USE_IPV6
   if(HttpServCon_isValid(&http6))
      HttpTrace_printf(0, "HTTP IPv6 socket listening on port %u.\n", EXAMPLE_HTTP_PORT);
#endif
   if(HttpServCon_isValid(&http))
      HttpTrace_printf(0, "HTTP IPv4 socket listening on port %u.\n", EXAMPLE_HTTP_PORT);

   config.server = &server;
   config.timer = &timer;
   config.io = (IoIntf*)&stateIo; /* Never mount private storage in the VFS. */
   config.clientSsl = &clientSsl;
   tokengen_info(&config.portalDomain, &config.zoneKey);
   config.proof = tokengen_proof;
   config.name = EXAMPLE_NAME;
   /* SharkCA derives https://portalDomain/acme/directory from tokengen. */
#ifndef SHARKTRUSTX_PRIVATE_CA
   config.email = EXAMPLE_EMAIL;
   config.directoryUrl = "https://acme-staging-v02.api.letsencrypt.org/directory";
   config.acceptTerms = TRUE;
#endif
   config.port = EXAMPLE_HTTPS_PORT;
   config.onEvent = certificateEvent;
   config.onName = certificateName;
#ifdef SHARKTRUSTX_PRIVATE_CA
   config.onName = localName;
   config.context = &local;
#endif
   rc = SharkTrustX_start(&config);
   ThreadMutex_release(&mutex);
   if(rc)
   {
      HttpTrace_printf(0, "SharkTrustX start rejected (%d).\n", rc);
      baFatalE(FE_USER_ERROR_1, rc);
   }
   HttpTrace_printf(0,
#ifdef SHARKTRUSTX_PRIVATE_CA
      "SharkTrustX C example: SharkCA private CA\n"
#else
      "SharkTrustX C example: Let's Encrypt STAGING\n"
#endif
      "Portal: %s\nStatus page: HTTP port %u at this device's address\n"
      "HTTPS port: %u (opens when a certificate is ready)\n"
#ifdef SHARKTRUSTX_PRIVATE_CA
      "Trust the SharkCA zone's root CA in browsers connecting to this server.\n",
#else
      "Staging certificates cause browser trust warnings.\n",
#endif
      config.portalDomain, EXAMPLE_HTTP_PORT, EXAMPLE_HTTPS_PORT);
   SoDisp_run(&dispatcher, -1);
}
