#ifndef BAS_REST_REST_H
#define BAS_REST_REST_H

/* Copyright (c) Real Time Logic LLC. See the repository LICENSE file. */

#ifndef __cplusplus
#error bas/rest/Rest.h requires C++
#endif

#include <AuthenticatedUser.h>
#include <BaErrorCodes.h>
#include <HttpRecData.h>
#include <HttpServer.h>
#include <ThreadLib.h>

#include <stddef.h>
#include <new>
#include <type_traits>
#include <utility>

/*
 * These limits are deliberately compile-time constants. Applications can
 * override them before including this header to trade RAM for capacity.
 */
#ifndef BAS_REST_MAX_ROUTES
#define BAS_REST_MAX_ROUTES 8
#endif

#ifndef BAS_REST_MAX_SEGMENTS
#define BAS_REST_MAX_SEGMENTS 8
#endif

#ifndef BAS_REST_MAX_PARAMS
#define BAS_REST_MAX_PARAMS 4
#endif

#ifndef BAS_REST_MAX_PATTERN_LENGTH
#define BAS_REST_MAX_PATTERN_LENGTH 96
#endif

#ifndef BAS_REST_MAX_REQUEST_PATH
#define BAS_REST_MAX_REQUEST_PATH 256
#endif

#ifndef BAS_REST_MAX_PARAMETER_LENGTH
#define BAS_REST_MAX_PARAMETER_LENGTH 64
#endif

#ifndef BAS_REST_HANDLER_SIZE
#define BAS_REST_HANDLER_SIZE 24
#endif

#if __cplusplus >= 201703L
#define BAS_REST_NODISCARD [[nodiscard]]
#else
#define BAS_REST_NODISCARD
#endif

class RestContext;
class RestRequest;
class RestResponse;
class RestServerUnlock;

/** A non-owning, length-delimited string. A missing value has data()==0;
    a present but empty value has non-null data and size()==0. */
class RestStringView
{
public:
   RestStringView() : data_(0), size_(0) {}
   RestStringView(const char* data, size_t size) : data_(data), size_(size) {}

   static RestStringView fromCString(const char* value);

   const char* data() const { return data_; }
   size_t size() const { return size_; }
   bool isPresent() const { return data_ != 0; }
   bool empty() const { return size_ == 0; }
   bool equals(const char* value) const;

   /** Copies the view and a terminating NUL into caller-owned storage. */
   int copyTo(char* destination, size_t capacity) const;

private:
   const char* data_;
   size_t size_;
};

/** Propagates the native BAS/BWS return code. Zero means success. */
class RestResult
{
public:
   RestResult() : code_(0) {}
   explicit RestResult(int nativeCode) : code_(nativeCode) {}

   static RestResult success() { return RestResult(0); }
   static RestResult fromNative(int nativeCode) { return RestResult(nativeCode); }

   bool succeeded() const { return code_ == 0; }
   bool failed() const { return code_ != 0; }
   int nativeCode() const { return code_; }

private:
   int code_;
};

/** The kind encoded by a route parameter such as {id:int}. */
typedef enum
{
   RestParameterType_String,
   RestParameterType_Int,
   RestParameterType_UInt,
   RestParameterType_Path
} RestParameterType;

/** Registration result. Registration never allocates from the heap. */
typedef enum
{
   RestRouteStatus_Ok,
   RestRouteStatus_Frozen,
   RestRouteStatus_RouteCapacity,
   RestRouteStatus_PatternTooLong,
   RestRouteStatus_InvalidPattern,
   RestRouteStatus_SegmentCapacity,
   RestRouteStatus_ParameterCapacity,
   RestRouteStatus_Duplicate,
   RestRouteStatus_Ambiguous
} RestRouteStatus;

/** Streaming wrapper over HttpRecData. Construct only while the request is
    active and consume or destroy it before returning from the handler. */
class RestBodyReader
{
public:
   explicit RestBodyReader(RestRequest& request);
   ~RestBodyReader();

   RestBodyReader(const RestBodyReader&) = delete;
   RestBodyReader& operator=(const RestBodyReader&) = delete;

   bool isValid() const { return validationCode_ >= 0; }
   SBaFileSize validationCode() const { return validationCode_; }
   SBaFileSize expectedLength() const { return validationCode_; }
   S32 read(void* destination, size_t capacity);

private:
   typedef typename std::aligned_storage<sizeof(HttpRecData),
                                         alignof(HttpRecData)>::type Storage;
   Storage storage_;
   HttpRecData* reader_;
   SBaFileSize validationCode_;
};

/** Per-request execution context. It never owns the HttpCommand. */
class RestContext
{
public:
   explicit RestContext(HttpCommand* command);

   HttpCommand* nativeCommand() const { return command_; }
   HttpRequest* nativeRequest() const;
   HttpResponse* nativeResponse() const;
   bool isServerMutexOwned() const;

   /** Run application work without the dispatcher mutex. The mutex is
       reacquired before this function returns or unwinds. Nested calls are
       allowed and do not release the mutex twice. */
   template<typename Callable>
   auto withServerUnlocked(Callable&& callable)
      -> decltype(std::forward<Callable>(callable)());

private:
   friend class RestServerUnlock;
   HttpCommand* command_;
   unsigned int unlockDepth_;
};

/** Advanced scoped form of RestContext::withServerUnlocked(). */
class BAS_REST_NODISCARD RestServerUnlock
{
public:
   explicit RestServerUnlock(RestContext& context);
   ~RestServerUnlock() noexcept;

   RestServerUnlock(const RestServerUnlock&) = delete;
   RestServerUnlock& operator=(const RestServerUnlock&) = delete;
   RestServerUnlock(RestServerUnlock&&) = delete;
   RestServerUnlock& operator=(RestServerUnlock&&) = delete;

private:
   typedef typename std::aligned_storage<sizeof(ThreadReleaseLock),
                                         alignof(ThreadReleaseLock)>::type Storage;
   RestContext* context_;
   ThreadReleaseLock* release_;
   Storage storage_;
};

template<typename Callable>
auto RestContext::withServerUnlocked(Callable&& callable)
   -> decltype(std::forward<Callable>(callable)())
{
   RestServerUnlock unlock(*this);
   return std::forward<Callable>(callable)();
}

/** Lightweight non-owning facade over the active HttpRequest. */
class RestRequest
{
public:
   HttpMethod method() const;
   RestStringView relativePath() const { return relativePath_; }
   RestStringView requestUri() const;
   RestStringView getHeader(const char* name) const;
   RestStringView getQueryParameter(const char* name) const;
   RestStringView getCookie(const char* name) const;
   RestStringView contentType() const;
   SBaFileSize contentLength() const;
   bool hasContentLength() const;
   SBaFileSize validateBody() const;

   RestStringView getPathParameter(const char* name) const;
   bool getIntParameter(const char* name, S32& value) const;
   bool getUnsignedParameter(const char* name, U32& value) const;

   AuthenticatedUser* authenticatedUser() const;
   RestStringView authenticatedUserName() const;

   HttpRequest* nativeRequest() const { return request_; }

private:
   friend class RestRouter;
   friend class RestBodyReader;
   friend struct RestRouterTestAccess;

   struct Parameter
   {
      RestStringView name;
      RestStringView value;
      RestParameterType type;
      S32 signedValue;
      U32 unsignedValue;
   };

   RestRequest(HttpRequest* request, RestStringView path);
   void setParameters(const Parameter* parameters, unsigned int count);

   HttpRequest* request_;
   RestStringView relativePath_;
   Parameter parameters_[BAS_REST_MAX_PARAMS];
   unsigned int parameterCount_;
};

/** Direct facade over the active HttpResponse. It does not accumulate an
    owning response body. */
class RestResponse
{
public:
   explicit RestResponse(HttpResponse* response) : response_(response) {}

   RestResult setStatus(int statusCode);
   RestResult setContentType(const char* contentType);
   RestResult setContentLength(BaFileSize contentLength);
   RestResult setHeader(const char* name, const char* value, bool replace=true);
   /** Initializes incremental output. BAS/BWS selects chunked transfer only
       when the active HTTP connection requires and supports it. */
   RestResult beginStreaming();

   RestResult write(const void* data, size_t length, bool buffered=true);
   RestResult send(const void* data, size_t length);
   RestResult flush();

   RestResult text(int statusCode, RestStringView body);
   RestResult text(int statusCode, const char* body);
   RestResult json(int statusCode, RestStringView body);
   RestResult json(int statusCode, const char* body);
   RestResult error(int statusCode, const char* message);
   RestResult noContent();
   RestResult redirect(const char* url);

   HttpCookie* createCookie(const char* name);
   bool committed() const;
   int status() const;
   U32 byteCount() const;
   BufPrint* writer();
   HttpResponse* nativeResponse() const { return response_; }

private:
   static bool validHeaderName(const char* value);
   static bool validHeaderValue(const char* value);
   RestResult completeSmallResponse(int statusCode,
                                    const char* contentType,
                                    RestStringView body);

   HttpResponse* response_;
};

typedef RestResult (*RestHandlerFunction)(void* handlerContext,
                                          RestContext& context,
                                          RestRequest& request,
                                          RestResponse& response);

/** Fixed-capacity route registry. Add routes during startup, then freeze it
    before dispatch. Matching uses only stack and object-owned storage. */
class RestRouter
{
public:
   RestRouter();
   ~RestRouter();

   RestRouter(const RestRouter&) = delete;
   RestRouter& operator=(const RestRouter&) = delete;

   RestRouteStatus addRoute(HttpMethod method,
                            const char* pattern,
                            RestHandlerFunction handler,
                            void* handlerContext=0);

   template<typename Handler>
   RestRouteStatus addRoute(HttpMethod method, const char* pattern,
                            Handler&& handler);

   template<typename Handler>
   RestRouteStatus get(const char* pattern, Handler&& handler)
   {
      return addRoute(HttpMethod_Get, pattern,
                      std::forward<Handler>(handler));
   }

   template<typename Handler>
   RestRouteStatus post(const char* pattern, Handler&& handler)
   {
      return addRoute(HttpMethod_Post, pattern,
                      std::forward<Handler>(handler));
   }

   template<typename Handler>
   RestRouteStatus put(const char* pattern, Handler&& handler)
   {
      return addRoute(HttpMethod_Put, pattern,
                      std::forward<Handler>(handler));
   }

   template<typename Handler>
   RestRouteStatus patch(const char* pattern, Handler&& handler)
   {
      return addRoute(HttpMethod_Patch, pattern,
                      std::forward<Handler>(handler));
   }

   template<typename Handler>
   RestRouteStatus del(const char* pattern, Handler&& handler)
   {
      return addRoute(HttpMethod_Delete, pattern,
                      std::forward<Handler>(handler));
   }

   void freeze() { frozen_ = true; }
   bool isFrozen() const { return frozen_; }
   unsigned int routeCount() const { return routeCount_; }

   RestResult dispatch(const char* relativePath, HttpCommand* command) const;

private:
   friend struct RestRouterTestAccess;

   struct Segment
   {
      U8 textOffset;
      U8 textLength;
      U8 nameOffset;
      U8 nameLength;
      U8 type;
   };

   typedef typename std::aligned_storage<BAS_REST_HANDLER_SIZE>::type
      HandlerStorage;

   struct Route
   {
      char pattern[BAS_REST_MAX_PATTERN_LENGTH + 1];
      Segment segments[BAS_REST_MAX_SEGMENTS];
      HttpMethod method;
      U8 segmentCount;
      U8 parameterCount;
      U8 trailingSlash;
      HandlerStorage handlerStorage;
      RestHandlerFunction directHandler;
      void* directContext;
      RestResult (*invoke)(const Route&, RestContext&, RestRequest&,
                           RestResponse&);
      void (*destroy)(Route&);
   };

   enum PathMatch
   {
      PathMatch_No,
      PathMatch_Yes,
      PathMatch_Malformed,
      PathMatch_TooLong
   };

   RestRouteStatus prepareRoute(HttpMethod method,
                                const char* pattern,
                                Route** route);
   static RestResult invokeDirect(const Route& route,
                                  RestContext& context,
                                  RestRequest& request,
                                  RestResponse& response);
   static PathMatch matchRoute(const Route& route,
                               const char* path,
                               RestRequest::Parameter* parameters,
                               unsigned int& parameterCount);
   static bool moreSpecific(const Route& candidate, const Route& current);
   static RestResult sendAutomaticResponse(HttpMethod method,
                                           U32 allowedMethods,
                                           RestResponse& response);

   template<typename Handler>
   static RestResult invokeStored(const Route& route,
                                  RestContext& context,
                                  RestRequest& request,
                                  RestResponse& response)
   {
      Handler* stored = const_cast<Handler*>(
         reinterpret_cast<const Handler*>(&route.handlerStorage));
      return (*stored)(context, request, response);
   }

   template<typename Handler>
   static void destroyStored(Route& route)
   {
      reinterpret_cast<Handler*>(&route.handlerStorage)->~Handler();
   }

   Route routes_[BAS_REST_MAX_ROUTES];
   unsigned int routeCount_;
   bool frozen_;
};

template<typename Handler>
RestRouteStatus RestRouter::addRoute(HttpMethod method,
                                     const char* pattern,
                                     Handler&& handler)
{
   typedef typename std::decay<Handler>::type StoredHandler;
   typedef decltype(std::declval<StoredHandler&>()(
      std::declval<RestContext&>(),
      std::declval<RestRequest&>(),
      std::declval<RestResponse&>())) ReturnType;

   static_assert(std::is_same<ReturnType, RestResult>::value,
                 "REST handlers must return RestResult");
   static_assert(sizeof(StoredHandler) <= sizeof(HandlerStorage),
                 "handler capture exceeds BAS_REST_HANDLER_SIZE; use the "
                 "function-pointer plus context overload");
   static_assert(alignof(StoredHandler) <= alignof(HandlerStorage),
                 "handler capture requires unsupported alignment");

   Route* route = 0;
   RestRouteStatus status = prepareRoute(method, pattern, &route);
   if(status != RestRouteStatus_Ok)
      return status;

   new (&route->handlerStorage) StoredHandler(std::forward<Handler>(handler));
   route->directHandler = 0;
   route->directContext = 0;
   route->invoke = &invokeStored<StoredHandler>;
   route->destroy = &destroyStored<StoredHandler>;
   ++routeCount_;
   return RestRouteStatus_Ok;
}

/** Adapter installed into the existing BAS/BWS virtual hierarchy. */
class RestService : public HttpDir
{
public:
   RestService(const char* name, RestRouter& router, S8 priority=0);

   RestService(const RestService&) = delete;
   RestService& operator=(const RestService&) = delete;

   HttpDir* getHttpDir() { return this; }
   const HttpDir* getHttpDir() const { return this; }

private:
   static int service(HttpDir* directory,
                      const char* relativePath,
                      HttpCommand* command);

   RestRouter* router_;
};

#undef BAS_REST_NODISCARD

#endif
