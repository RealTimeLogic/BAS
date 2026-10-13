/* Copyright (c) Real Time Logic LLC, 2026. All rights reserved.
 * Use is subject to the Barracuda license supplied with this software.
 */
#include "start.h"
#include <string.h>

/* These values are read and written only while the dispatcher mutex is held. */
static char assignedName[254];
static int lastEvent;

static const char* eventText(int code)
{
   switch(code)
   {
      case 0:
         return "Starting enrollment or restoring saved state";
      case SharkTrustX_Ready:
         return "Certificate installed; renewal scheduled";
      case SharkTrustX_Retry:
         return "Retry scheduled after the preceding error";
      case SharkTrustX_Busy:
         return "Client lifecycle is busy";
      case SharkTrustX_Identity:
         return "Saved identity or configuration mismatch";
      case SharkTrustX_Storage:
         return "Cannot read or save client state";
      case SharkTrustX_Portal:
         return "Portal enrollment or DNS operation failed";
      case SharkTrustX_Acme:
         return "Certificate authority operation failed";
      case SharkTrustX_Trust:
         return "Outbound HTTPS trust validation failed";
      case SharkTrustX_Crypto:
         return "Key, allocation, or HTTPS listener failure";
      case SharkTrustX_Terms:
         return "Certificate authority terms not accepted";
      default:
         return "Unknown client event";
   }
}

void certificateEvent(void* context, int code)
{
   SharkTrustX_Status status;
   char expires[40], renew[40];
   (void)context;
   /* Retain the error on the page when its accompanying Retry event arrives. */
   if(code != SharkTrustX_Retry)
      lastEvent = code;
   HttpTrace_printf(0, "SharkTrustX: %s (%d)\n", eventText(code), code);
   if(code == SharkTrustX_Ready && SharkTrustX_getStatus(&status) && status.ready)
   {
      httpFmtDate(expires, sizeof(expires), status.expires);
      httpFmtDate(renew, sizeof(renew), status.renew);
      HttpTrace_printf(0, "Certificate expires: %s\nRenewal due: %s\n", expires, renew);
   }
}

void certificateName(void* context, const char* name, size_t length)
{
   (void)context;
   assignedName[0] = 0; /* IP-only enrollment clears any previous name. */
   if(name && length < sizeof(assignedName))
   {
      memcpy(assignedName, name, length);
      assignedName[length] = 0;
      HttpTrace_printf(0, "Confirmed name: %s\nHTTPS: https://%s:%u/\n",
                       assignedName, assignedName, EXAMPLE_HTTPS_PORT);
   }
}

static int statusPage(HttpDir* dir, const char* path, HttpCommand* cmd)
{
   HttpResponse* response;
   HttpMethod method;
   SharkTrustX_Status status;
   BaBool active;
   int event;
   char name[sizeof(assignedName)], expires[40], renew[40], now[40];
   const char* certificate;
   const char* renewalNote;
   (void)dir;
   if(!cmd)
      return 0;
   if(*path && strcmp(path, "index.html"))
      return -1;
   response = HttpCommand_getResponse(cmd);
   method = HttpRequest_getMethodType(HttpCommand_getRequest(cmd));
   if(method != HttpMethod_Get && method != HttpMethod_Head)
   {
      HttpResponse_setStatus(response, 405);
      HttpResponse_setHeader(response, "Allow", "GET, HEAD", TRUE);
      HttpResponse_setContentLength(response, 0);
      return 0;
   }
   /* Socket writes may release the mutex. Snapshot everything before writing. */
   active = SharkTrustX_getStatus(&status);
   memcpy(name, assignedName, sizeof(name));
   event = lastEvent;
   certificate = status.ready ? "Installed; HTTPS is ready" :
                 active ? "Waiting for a usable certificate" : "Client stopped";
   strcpy(expires, "Not available yet");
   strcpy(renew, "Not scheduled yet");
   renewalNote = "Renewal is due halfway through the certificate lifetime.";
   if(status.ready)
   {
      httpFmtDate(expires, sizeof(expires), status.expires);
      httpFmtDate(renew, sizeof(renew), status.renew);
      if(status.renew <= baGetUnixTime())
         renewalNote = "Renewal is due. Issuance or retries may still be in progress.";
   }
   httpFmtDate(now, sizeof(now), baGetUnixTime());
   HttpResponse_setContentType(response, "text/html; charset=utf-8");
   HttpResponse_setHeader(response, "Cache-Control", "no-store", TRUE);
   /* BWS suppresses the body for HEAD while retaining the GET metadata. */
   /* The client validates the DNS name as ASCII letters/digits/dots/hyphens.
    * All other inserted strings are fixed text or BWS-formatted dates. */
   HttpResponse_printf(response,
      "<!doctype html><html lang=\"en\"><head><meta charset=\"utf-8\">"
      "<meta name=\"viewport\" content=\"width=device-width,initial-scale=1\">"
      "<meta http-equiv=\"refresh\" content=\"10\">"
      "<title>SharkTrustX certificate status</title><style>"
      "body{font:17px/1.6 system-ui,sans-serif;background:#f3f6f8;color:#182c3c;"
      "max-width:48rem;margin:4rem auto;padding:0 1.2rem}"
      "h1{line-height:1.2}main{background:white;padding:1.5rem;border-radius:.6rem}"
      "dl{margin:0}dt{font-weight:650;margin-top:1rem}dd{margin:0;overflow-wrap:anywhere}"
      "a{color:#075fa8}.note{color:#42566a}.trust{color:#7a4300}"
      "</style></head><body><h1>SharkTrustX certificate status</h1>"
#ifdef SHARKTRUSTX_PRIVATE_CA
      "<p class=\"trust\">Private CA certificate. Trust this zone's SharkCA "
      "root certificate in the browser.</p><main><dl>"
#else
      "<p class=\"trust\">Let's Encrypt staging. These test certificates cause "
      "browser trust warnings.</p><main><dl>"
#endif
      "<dt>Certificate</dt><dd>%s</dd>"
      "<dt>Confirmed DNS name</dt><dd>%s</dd>"
      "<dt>Expires</dt><dd>%s</dd>"
      "<dt>Renewal due</dt><dd>%s</dd>"
      "<dt>Last notification</dt><dd>%s (%d)</dd>"
      "</dl><p class=\"note\">%s Network or CA failures can delay renewal.</p>",
      certificate, *name ? name : "Waiting for portal confirmation",
      expires, renew, eventText(event), event, renewalNote);
   if(status.ready && *name)
      HttpResponse_printf(response,
         "<p><a href=\"https://%s:%u/\">Open this page over HTTPS</a></p>",
         name, EXAMPLE_HTTPS_PORT);
   HttpResponse_printf(response,
      "</main><p class=\"note\">Updated %s. Refreshes every 10 seconds. "
      "All times are UTC (GMT).</p></body></html>", now);
   return 0;
}

void installVirtualDir(HttpServer* server)
{
   static HttpDir root;
   HttpDir_constructor(&root, 0, 0);
   HttpDir_setService(&root, statusPage);
   HttpServer_insertRootDir(server, &root);
}
