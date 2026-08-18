#include <bas/rest/Rest.h>

#include <HttpCmdThreadPool.h>
#include <HttpServCon.h>
#include <HttpTrace.h>
#include <SoDisp.h>

#ifdef BAS_REST_EMBEDDED_UI
#include <HttpResRdr.h>
#include <ZipIo.h>

extern "C" ZipReader* getEmbeddedUiZipReader(void);
#endif

#include <stdio.h>
#include <string.h>

namespace
{
#ifndef HTTP_PORT
#define HTTP_PORT 9357
#endif

HttpServCon httpListener;

void openHttpListener(HttpServer* server, SoDisp* dispatcher)
{
   HttpServCon_constructor(&httpListener, server, dispatcher,
                           HTTP_PORT, FALSE, 0, 0);
   if(!HttpServCon_isValid(&httpListener))
      baFatalE(FE_USER_ERROR_1, HTTP_PORT);
   HttpTrace::printf(0, "HTTP: REST example listening on port %d.\n",
                     HTTP_PORT);
}

class DeviceState
{
public:
   DeviceState() : pin_(0), state_(0) {}

   bool set(S32 pin, U32 state)
   {
      ThreadLock lock(mutex_); // Simulates protecting HW
      if(pin < 0 || pin > 63 || state > 1)
         return false;
      pin_ = pin;
      state_ = state;
      return true;
   }

private:
   ThreadMutex mutex_;
   S32 pin_;
   U32 state_;
};

// Helper function for printing a non null terminated RestStringView
RestResult writeView(RestResponse& response, RestStringView value)
{
   static const char missing[] = "<missing>";
   return value.isPresent() ? response.write(value.data(), value.size()) :
      response.write(missing, sizeof(missing) - 1);
}

RestResult writeField(RestResponse& response,
                      const char* label,
                      RestStringView value)
{
   RestResult result = response.write(label, strlen(label));
   if(result.succeeded())
   {
      result = writeView(response, value);
      if(result.succeeded())
      {
         result = response.write("\n", 1);
      }
   }
   return result;
}

void writeTraceView(BufPrint* trace, RestStringView value)
{
   static const char missing[] = "<missing>";
   if(value.isPresent())
      trace->write(value.data(), (int)value.size());
   else
      trace->write(missing, sizeof(missing) - 1);
}

void requireRoute(RestRouteStatus status)
{
   if(status != RestRouteStatus_Ok)
      baFatalE(FE_USER_ERROR_3, (unsigned int)status);
}

void configureRoutes(RestRouter& router, DeviceState& device)
{
   requireRoute(router.get("/health",
      [](RestContext&, RestRequest&, RestResponse& response) -> RestResult {
         HttpTrace::printf(0, "REST GET /api/health -> 200\n");
         return response.json(200, "{\"status\":\"ok\"}");
      }));

   requireRoute(router.post("/gpio/{pin:int}/set/{state:uint}",
      [&device](RestContext& context,
                RestRequest& request,
                RestResponse& response) -> RestResult {
         S32 pin;
         U32 state;
         if(!request.getIntParameter("pin", pin) ||
            !request.getUnsignedParameter("state", state))
         {
            HttpTrace::printf(0,
               "REST POST /api/gpio/{pin}/set/{state} invalid parameters -> 400\n");
            return response.error(400, "Invalid GPIO parameters");
         }

         const bool updated = context.withServerUnlocked([&]() -> bool {
            /* Simulate a blocking device operation. No BAS/BWS object is
               accessed while the dispatcher mutex is released. */
            Thread::sleep(500);
            return device.set(pin, state);
         });
         HttpTrace::printf(0,
            "REST POST /api/gpio/%d/set/%u updated=%s -> %d\n",
            pin, state, updated ? "yes" : "no", updated ? 204 : 400);
         return updated ? response.noContent() :
            response.error(400, "GPIO pin or state is out of range");
      }));

   requireRoute(router.get("/inspect",
      [](RestContext&, RestRequest& request,
         RestResponse& response) -> RestResult {
         const RestStringView mode = request.getQueryParameter("mode");
         const RestStringView deviceHeader = request.getHeader("X-Device");
         const RestStringView session = request.getCookie("session");
         RestStringView user = request.authenticatedUserName();
         if(!user.isPresent())
            user = RestStringView::fromCString("anonymous");
         {
            HttpTraceWriteLock traceLock;
            BufPrint* trace = traceLock;
            if(trace) // Prevent other threads from printing in this section
            {
               trace->printf("REST GET /api/inspect mode=\"");
               writeTraceView(trace, mode);
               trace->printf("\" x-device=\"");
               writeTraceView(trace, deviceHeader);
               trace->printf("\" session=\"");
               writeTraceView(trace, session);
               trace->printf("\" user=\"");
               writeTraceView(trace, user);
               trace->printf("\" -> 200\n");
            }
         }
         RestResult result = response.setStatus(200);
         if(result.succeeded())
         {
            result = response.setContentType("text/plain; charset=UTF-8");
            if(result.succeeded())
            {
               result = response.beginStreaming();
               if(result.succeeded())
               {
                  result = writeField(response, "mode=", mode);
                  if(result.succeeded())
                  {
                     result = writeField(response, "x-device=", deviceHeader);
                     if(result.succeeded())
                     {
                        result = writeField(response, "session=", session);
                        if(result.succeeded())
                        {
                           result = writeField(response, "user=", user);
                        }
                     }
                  }
               }
            }
         }
         return result;
      }));

   requireRoute(router.get("/slow/{milliseconds:uint}",
      [](RestContext& context, RestRequest& request,
         RestResponse& response) -> RestResult {
         U32 milliseconds;
         if(!request.getUnsignedParameter("milliseconds", milliseconds) ||
            milliseconds > 2000)
         {
            HttpTrace::printf(0,
               "REST GET /api/slow/{milliseconds} invalid delay -> 400\n");
            return response.error(400, "Delay must be between 0 and 2000");
         }
         HttpTrace::printf(0,
            "REST GET /api/slow/%u delay-ms=%u started\n",
            milliseconds, milliseconds);
         const bool ownedBefore = context.isServerMutexOwned();
         bool ownedInside = true;
         context.withServerUnlocked([&]() -> void {
            ownedInside = context.isServerMutexOwned();
            context.withServerUnlocked([&]() -> void {
               Thread::sleep(milliseconds);
            });
         });
         if(!ownedBefore || ownedInside || !context.isServerMutexOwned())
         {
            HttpTrace::printf(0,
               "REST GET /api/slow/%u mutex-invariant=failed -> 500\n",
               milliseconds);
            return response.error(500, "Dispatcher mutex invariant failed");
         }
         HttpTrace::printf(0,
            "REST GET /api/slow/%u completed -> 200\n", milliseconds);
         return response.json(200, "{\"completed\":true}");
      }));

   requireRoute(router.get("/stream",
      [](RestContext& context, RestRequest&, RestResponse& response) -> RestResult {
         static const char* const chunkNames[] = {
            "first", "second", "third", "fourth", "fifth"
         };
         HttpTrace::printf(0,
            "REST GET /api/stream chunks=5 flushes=5 -> 200\n");
         RestResult result = response.setStatus(200);
         if(result.succeeded())
         {
            result = response.setContentType("text/plain; charset=UTF-8");
            if(result.succeeded())
            {
               result = response.beginStreaming();
               if(result.succeeded())
               {
                  for(unsigned int i = 0;
                      i < sizeof(chunkNames) / sizeof(chunkNames[0]) &&
                      result.succeeded(); ++i)
                  {
                     response.write(chunkNames[i], strlen(chunkNames[i]));
                     response.write("\n", 1);
                     result = response.flush();
                     if(result.succeeded())
                     {
                        context.withServerUnlocked(
                           []() -> void { Thread::sleep(1500); });
                     }
                  }
               }
            }
         }
         return result;
      }));

   requireRoute(router.post("/body",
      [](RestContext&, RestRequest& request,
         RestResponse& response) -> RestResult {
         char input[128];
         char output[64];
         S32 count;
         U32 total = 0;
         U32 checksum = 0;

         if(!request.hasContentLength())
         {
            HttpTrace::printf(0,
               "REST POST /api/body content-length=missing -> 411\n");
            return response.error(411, "Content-Length required by this example");
         }
         if(request.contentLength() < 0 || request.contentLength() > 4096)
         {
            HttpTrace::printf(0,
               "REST POST /api/body content-length=out-of-range -> 413\n");
            return response.error(413, "Body limit is 4096 bytes");
         }

         RestBodyReader body(request);
         if(!body.isValid())
         {
            HttpTrace::printf(0,
               "REST POST /api/body reader=invalid -> 400\n");
            return response.error(400, "Request body is not readable");
         }
         while((count = body.read(input, sizeof(input))) > 0)
         {
            S32 i;
            total += (U32)count;
            for(i = 0; i < count; ++i)
               checksum += (U8)input[i];
         }
         if(count < 0)
         {
            HttpTrace::printf(0,
               "REST POST /api/body read-error=%d\n", count);
            return RestResult::fromNative(count);
         }
         HttpTrace::printf(0,
            "REST POST /api/body bytes=%u checksum=%u -> 200\n",
            total, checksum);
         basnprintf(output, sizeof(output),
                    "{\"bytes\":%u,\"checksum\":%u}", total, checksum);
         return response.json(200, output);
      }));

   /* These two routes demonstrate deterministic static-over-typed
      precedence independent of registration order. */
   requireRoute(router.get("/device/{id:uint}",
      [](RestContext&, RestRequest& request,
         RestResponse& response) -> RestResult {
         U32 id;
         char output[48];
         if(!request.getUnsignedParameter("id", id))
         {
            HttpTrace::printf(0,
               "REST GET /api/device/{id} invalid id -> 400\n");
            return response.error(400, "Invalid device ID");
         }
         HttpTrace::printf(0,
            "REST GET /api/device/%u id=%u -> 200\n", id, id);
         basnprintf(output, sizeof(output), "device=%u", id);
         return response.text(200, output);
      }));

   requireRoute(router.get("/device/status",
      [](RestContext&, RestRequest&, RestResponse& response) -> RestResult {
         HttpTrace::printf(0, "REST GET /api/device/status -> 200\n");
         return response.text(200, "all devices ready");
      }));

#ifdef BAS_REST_ENABLE_TEST_ROUTES
#if defined(__cpp_exceptions) || defined(__EXCEPTIONS) || defined(_CPPUNWIND)
   requireRoute(router.get("/test/throw",
      [](RestContext& context, RestRequest&,
         RestResponse& response) -> RestResult {
         HttpTrace::printf(0,
            "REST GET /api/test/throw action=throw-test -> 500\n");
         context.withServerUnlocked([]() -> void { throw 7; });
         return response.error(500, "unreachable");
      }));
#endif
   requireRoute(router.get("/test/committed",
      [](RestContext&, RestRequest&, RestResponse& response) -> RestResult {
         HttpTrace::printf(0,
            "REST GET /api/test/committed action=commit-test\n");
         RestResult result = response.write("committed", 9, false);
         if(result.succeeded())
         {
            result = response.flush();
            if(result.succeeded())
            {
               result = response.setHeader("X-Too-Late", "rejected");
               result = result.nativeCode() == E_IS_COMMITTED ?
                  RestResult::success() : RestResult(E_INCORRECT_USE);
            }
         }
         return result;
      }));
#endif
}

} // Namespace

static HttpServer* createServer()
{
   static ThreadMutex mutex;
   static SoDisp dispatcher(&mutex);
   static HttpServerConfig configuration;

   configuration.setNoOfHttpCommands(3);
   configuration.setNoOfHttpConnections(12);
   configuration.setRequest(2 * 1024, 8 * 1024);
   configuration.setResponseData(1400);

   static HttpServer server(&dispatcher, &configuration);
   static HttpCmdThreadPool pool(&server, ThreadPrioNormal, BA_STACKSZ);
   openHttpListener(&server, &dispatcher);
   return &server;
}

#ifdef BAS_REST_EMBEDDED_UI
static void installEmbeddedUi(HttpServer* server)
{
   ZipReader* zipReader = getEmbeddedUiZipReader();
   if(!zipReader || !zipReader->isValid())
      baFatalE(FE_USER_ERROR_5, 0);

   static ZipIo uiIo(zipReader);
   if(uiIo.getECode() != ZipErr_NoError)
      baFatalE(FE_USER_ERROR_6, (unsigned int)uiIo.getECode());

   static HttpResRdr uiResources(&uiIo, 0);
   if(server->insertRootDir(&uiResources) != 0)
      baFatalE(FE_USER_ERROR_7, 0);
   HttpTrace::printf(0, "HTTP: embedded device console mounted at /.\n");
}
#endif

extern "C" void barracuda(void)
{
   static DeviceState device;
   static RestRouter router;
   static bool configured = false;
   HttpServer* server = createServer();

   if(!configured)
   {
      configureRoutes(router, device);
      configured = true;
   }
   static RestService service("api", router);
   if(server->insertRootDir(service.getHttpDir()) != 0)
      baFatalE(FE_USER_ERROR_4, 0);
#ifdef BAS_REST_EMBEDDED_UI
   installEmbeddedUi(server);
#endif
   server->getDispatcher()->run();
   baAssert(0);
}
