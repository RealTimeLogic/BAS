#include <bas/rest/Rest.h>

/* Copyright (c) Real Time Logic LLC. See the repository LICENSE file. */

#include <HttpTrace.h>

#include <limits.h>
#include <string.h>

namespace
{
size_t boundedLength(const char* value, size_t maximum, bool& terminated)
{
   size_t length = 0;
   if(!value)
   {
      terminated = false;
      return 0;
   }
   while(length <= maximum && value[length])
      ++length;
   terminated = length <= maximum;
   return length;
}

bool equalText(const char* left, size_t leftLength,
               const char* right, size_t rightLength)
{
   return leftLength == rightLength &&
      (leftLength == 0 || memcmp(left, right, leftLength) == 0);
}

bool validParameterName(const char* name, size_t length)
{
   size_t i;
   if(length == 0)
      return false;
   if(!((name[0] >= 'A' && name[0] <= 'Z') ||
        (name[0] >= 'a' && name[0] <= 'z') || name[0] == '_'))
      return false;
   for(i = 1; i < length; ++i)
   {
      const char ch = name[i];
      if(!((ch >= 'A' && ch <= 'Z') ||
           (ch >= 'a' && ch <= 'z') ||
           (ch >= '0' && ch <= '9') || ch == '_'))
         return false;
   }
   return true;
}

bool parseUnsigned(RestStringView value, U32& result)
{
   size_t i;
   U32 parsed = 0;
   if(!value.isPresent() || value.empty())
      return false;
   for(i = 0; i < value.size(); ++i)
   {
      U32 digit;
      const char ch = value.data()[i];
      if(ch < '0' || ch > '9')
         return false;
      digit = (U32)(ch - '0');
      if(parsed > (((U32)~(U32)0) - digit) / 10U)
         return false;
      parsed = parsed * 10U + digit;
   }
   result = parsed;
   return true;
}

bool parseSigned(RestStringView value, S32& result)
{
   size_t i = 0;
   U32 parsed = 0;
   U32 limit = 0x7FFFFFFFU;
   bool negative = false;

   if(!value.isPresent() || value.empty())
      return false;
   if(value.data()[0] == '-')
   {
      negative = true;
      limit = 0x80000000U;
      i = 1;
      if(i == value.size())
         return false;
   }
   for(; i < value.size(); ++i)
   {
      U32 digit;
      const char ch = value.data()[i];
      if(ch < '0' || ch > '9')
         return false;
      digit = (U32)(ch - '0');
      if(parsed > (limit - digit) / 10U)
         return false;
      parsed = parsed * 10U + digit;
   }
   if(negative)
   {
      if(parsed == 0x80000000U)
         result = (S32)(-2147483647L - 1L);
      else
         result = -(S32)parsed;
   }
   else
      result = (S32)parsed;
   return true;
}

int segmentRank(U8 type)
{
   switch((RestParameterType)type)
   {
      case RestParameterType_Int:
      case RestParameterType_UInt:
         return 3;
      case RestParameterType_String:
         return 2;
      case RestParameterType_Path:
         return 1;
   }
   return 4; /* Static segment uses a sentinel outside the enum. */
}

bool methodIsSingle(HttpMethod method)
{
   const U32 value = (U32)method;
   const U32 supported = (U32)HttpMethod_Get | (U32)HttpMethod_Head |
      (U32)HttpMethod_Post | (U32)HttpMethod_Put |
      (U32)HttpMethod_Patch | (U32)HttpMethod_Delete;
   return value != 0 && (value & supported) != 0 &&
      (value & ~supported) == 0 && (value & (value - 1U)) == 0;
}

const U8 StaticSegment = 0xFF;
}

RestStringView RestStringView::fromCString(const char* value)
{
   return value ? RestStringView(value, strlen(value)) : RestStringView();
}

bool RestStringView::equals(const char* value) const
{
   return value && isPresent() && equalText(data_, size_, value, strlen(value));
}

int RestStringView::copyTo(char* destination, size_t capacity) const
{
   if(!isPresent() || !destination || capacity == 0)
      return E_INVALID_PARAM;
   if(size_ >= capacity)
      return E_TOO_MUCH_DATA;
   if(size_)
      memcpy(destination, data_, size_);
   destination[size_] = 0;
   return 0;
}

RestBodyReader::RestBodyReader(RestRequest& request)
   : reader_(0), validationCode_(HttpRecData_valid(request.request_))
{
   if(validationCode_ >= 0)
      reader_ = new (&storage_) HttpRecData(request.request_);
}

RestBodyReader::~RestBodyReader()
{
   if(reader_)
      reader_->~HttpRecData();
}

S32 RestBodyReader::read(void* destination, size_t capacity)
{
   if(!reader_ || !destination || capacity == 0 || capacity > INT_MAX)
      return -1;
   return reader_->read(destination, (S32)capacity);
}

RestContext::RestContext(HttpCommand* command)
   : command_(command), unlockDepth_(0)
{
   baAssert(command_ != 0);
}

HttpRequest* RestContext::nativeRequest() const
{
   return HttpCommand_getRequest(command_);
}

HttpResponse* RestContext::nativeResponse() const
{
   return HttpCommand_getResponse(command_);
}

bool RestContext::isServerMutexOwned() const
{
   HttpServer* server = HttpCommand_getServer(command_);
   ThreadMutex* mutex = server ? HttpServer_getMutex(server) : 0;
   return mutex && ThreadMutex_isOwner(mutex) ? true : false;
}

RestServerUnlock::RestServerUnlock(RestContext& context)
   : context_(&context), release_(0)
{
   if(context_->unlockDepth_++ == 0)
   {
      baAssert(context_->isServerMutexOwned());
      release_ = new (&storage_) ThreadReleaseLock(context_->nativeRequest());
   }
}

RestServerUnlock::~RestServerUnlock() noexcept
{
   baAssert(context_ != 0 && context_->unlockDepth_ != 0);
   if(release_)
   {
      release_->~ThreadReleaseLock();
      release_ = 0;
   }
   --context_->unlockDepth_;
}

RestRequest::RestRequest(HttpRequest* request, RestStringView path)
   : request_(request), relativePath_(path), parameterCount_(0)
{
}

void RestRequest::setParameters(const Parameter* parameters,
                                unsigned int count)
{
   unsigned int i;
   baAssert(count <= BAS_REST_MAX_PARAMS);
   parameterCount_ = count;
   for(i = 0; i < count; ++i)
      parameters_[i] = parameters[i];
}

HttpMethod RestRequest::method() const
{
   return HttpRequest_getMethodType(request_);
}

RestStringView RestRequest::requestUri() const
{
   return RestStringView::fromCString(HttpRequest_getRequestURI(request_));
}

RestStringView RestRequest::getHeader(const char* name) const
{
   return RestStringView::fromCString(
      name ? HttpRequest_getHeaderValue(request_, name) : 0);
}

RestStringView RestRequest::getQueryParameter(const char* name) const
{
   return RestStringView::fromCString(
      name ? HttpRequest_getParameter(request_, name) : 0);
}

RestStringView RestRequest::getCookie(const char* name) const
{
   HttpCookie* cookie = name ? HttpRequest_getCookie(request_, name) : 0;
   return RestStringView::fromCString(cookie ? HttpCookie_getValue(cookie) : 0);
}

RestStringView RestRequest::contentType() const
{
   return RestStringView::fromCString(
      HttpStdHeaders_getContentType(HttpRequest_getStdHeaders(request_)));
}

SBaFileSize RestRequest::contentLength() const
{
   return (SBaFileSize)HttpStdHeaders_getContentLength(
      HttpRequest_getStdHeaders(request_));
}

bool RestRequest::hasContentLength() const
{
   return HttpRequest_getHeaderValue(request_, "Content-Length") != 0;
}

SBaFileSize RestRequest::validateBody() const
{
   return HttpRecData_valid(request_);
}

RestStringView RestRequest::getPathParameter(const char* name) const
{
   unsigned int i;
   if(!name)
      return RestStringView();
   for(i = 0; i < parameterCount_; ++i)
      if(parameters_[i].name.equals(name))
         return parameters_[i].value;
   return RestStringView();
}

bool RestRequest::getIntParameter(const char* name, S32& value) const
{
   unsigned int i;
   if(!name)
      return false;
   for(i = 0; i < parameterCount_; ++i)
   {
      if(parameters_[i].name.equals(name) &&
         parameters_[i].type == RestParameterType_Int)
      {
         value = parameters_[i].signedValue;
         return true;
      }
   }
   return false;
}

bool RestRequest::getUnsignedParameter(const char* name, U32& value) const
{
   unsigned int i;
   if(!name)
      return false;
   for(i = 0; i < parameterCount_; ++i)
   {
      if(parameters_[i].name.equals(name) &&
         parameters_[i].type == RestParameterType_UInt)
      {
         value = parameters_[i].unsignedValue;
         return true;
      }
   }
   return false;
}

AuthenticatedUser* RestRequest::authenticatedUser() const
{
#ifndef NO_HTTP_SESSION
   return HttpRequest_getAuthenticatedUser(request_);
#else
   return 0;
#endif
}

RestStringView RestRequest::authenticatedUserName() const
{
   AuthenticatedUser* user = authenticatedUser();
   return RestStringView::fromCString(
      user ? AuthenticatedUser_getName(user) : 0);
}

bool RestResponse::validHeaderName(const char* value)
{
   const unsigned char* cursor = (const unsigned char*)value;
   if(!cursor || !*cursor)
      return false;
   for(; *cursor; ++cursor)
   {
      const unsigned char ch = *cursor;
      if((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') ||
         (ch >= '0' && ch <= '9'))
         continue;
      switch(ch)
      {
         case '!': case '#': case '$': case '%': case '&': case '\'':
         case '*': case '+': case '-': case '.': case '^': case '_':
         case '`': case '|': case '~':
            continue;
      }
      return false;
   }
   return true;
}

bool RestResponse::validHeaderValue(const char* value)
{
   const unsigned char* cursor = (const unsigned char*)value;
   if(!cursor)
      return false;
   for(; *cursor; ++cursor)
      if(*cursor == '\r' || *cursor == '\n' ||
         (*cursor < 0x20 && *cursor != '\t') || *cursor == 0x7F)
         return false;
   return true;
}

RestResult RestResponse::setStatus(int statusCode)
{
   const char* statusText = HttpServer_getStatusCode(statusCode);
   if(statusCode < 100 || statusCode > 599 ||
      !statusText || statusText[0] == '?')
      return RestResult(E_INVALID_PARAM);
   return RestResult::fromNative(HttpResponse_setStatus(response_, statusCode));
}

RestResult RestResponse::setContentType(const char* contentTypeValue)
{
   if(!validHeaderValue(contentTypeValue))
      return RestResult(E_INVALID_PARAM);
   return RestResult::fromNative(
      HttpResponse_setContentType(response_, contentTypeValue));
}

RestResult RestResponse::setContentLength(BaFileSize contentLengthValue)
{
   return RestResult::fromNative(
      HttpResponse_setContentLength(response_, contentLengthValue));
}

RestResult RestResponse::setHeader(const char* name,
                                   const char* value,
                                   bool replace)
{
   if(!validHeaderName(name) || !validHeaderValue(value))
      return RestResult(E_INVALID_PARAM);
   return RestResult::fromNative(
      HttpResponse_setHeader(response_, name, value, replace ? TRUE : FALSE));
}

RestResult RestResponse::beginStreaming()
{
   if(HttpResponse_committed(response_))
      return RestResult(E_IS_COMMITTED);
   if(response_->printAndWriteInitialized)
      return RestResult::success();
   return RestResult::fromNative(HttpResponse_printAndWriteInit(response_));
}

RestResult RestResponse::write(const void* data, size_t length, bool buffered)
{
   if((!data && length) || length > INT_MAX)
      return RestResult(E_INVALID_PARAM);
   return RestResult::fromNative(HttpResponse_write(
      response_, data, (int)length, buffered ? TRUE : FALSE));
}

RestResult RestResponse::send(const void* data, size_t length)
{
   if((!data && length) || length > INT_MAX)
      return RestResult(E_INVALID_PARAM);
   return RestResult::fromNative(HttpResponse_send(response_, data, (int)length));
}

RestResult RestResponse::flush()
{
   return RestResult::fromNative(HttpResponse_flush(response_));
}

RestResult RestResponse::completeSmallResponse(int statusCode,
                                               const char* type,
                                               RestStringView body)
{
   RestResult result;
   if(!body.isPresent())
      return RestResult(E_INVALID_PARAM);
   if(body.size() > INT_MAX)
      return RestResult(E_TOO_MUCH_DATA);
   result = setStatus(statusCode);
   if(result.succeeded())
   {
      result = setContentType(type);
      if(result.succeeded())
      {
         result = setContentLength((BaFileSize)body.size());
         if(result.succeeded())
         {
            if(!body.empty())
            {
               result = write(body.data(), body.size());
            }
         }
      }
   }
   return result;
}

RestResult RestResponse::text(int statusCode, RestStringView body)
{
   return completeSmallResponse(statusCode,
                                "text/plain; charset=UTF-8", body);
}

RestResult RestResponse::text(int statusCode, const char* body)
{
   return text(statusCode, RestStringView::fromCString(body));
}

RestResult RestResponse::json(int statusCode, RestStringView body)
{
   return completeSmallResponse(statusCode,
                                "application/json; charset=UTF-8", body);
}

RestResult RestResponse::json(int statusCode, const char* body)
{
   return json(statusCode, RestStringView::fromCString(body));
}

RestResult RestResponse::error(int statusCode, const char* message)
{
   return text(statusCode, message ? message : "Error");
}

RestResult RestResponse::noContent()
{
   RestResult result = setStatus(204);
   if(result.succeeded())
   {
      result = setContentLength(0);
   }
   return result;
}

RestResult RestResponse::redirect(const char* url)
{
   if(!validHeaderValue(url))
      return RestResult(E_INVALID_PARAM);
   return RestResult::fromNative(HttpResponse_sendRedirect(response_, url));
}

HttpCookie* RestResponse::createCookie(const char* name)
{
   return validHeaderName(name) ? HttpResponse_createCookie(response_, name) : 0;
}

bool RestResponse::committed() const
{
   return HttpResponse_committed(response_) ? true : false;
}

int RestResponse::status() const
{
   return HttpResponse_getStatus(response_);
}

U32 RestResponse::byteCount() const
{
   return HttpResponse_byteCount(response_);
}

BufPrint* RestResponse::writer()
{
   return HttpResponse_getWriter(response_);
}

RestRouter::RestRouter() : routeCount_(0), frozen_(false)
{
   static_assert(BAS_REST_MAX_ROUTES > 0, "BAS_REST_MAX_ROUTES must be positive");
   static_assert(BAS_REST_MAX_SEGMENTS > 0, "BAS_REST_MAX_SEGMENTS must be positive");
   static_assert(BAS_REST_MAX_PARAMS > 0, "BAS_REST_MAX_PARAMS must be positive");
   static_assert(BAS_REST_MAX_PATTERN_LENGTH <= 255,
                 "segment offsets use one byte");
   static_assert(sizeof(S32) == 4 && sizeof(U32) == 4,
                 "typed REST parameters require 32-bit S32 and U32");
   memset(routes_, 0, sizeof(routes_));
}

RestRouter::~RestRouter()
{
   unsigned int i;
   for(i = 0; i < routeCount_; ++i)
      if(routes_[i].destroy)
         routes_[i].destroy(routes_[i]);
}

RestRouteStatus RestRouter::prepareRoute(HttpMethod method,
                                         const char* pattern,
                                         Route** output)
{
   Route* route;
   size_t length;
   size_t cursor;
   size_t end;
   bool terminated;
   unsigned int parameterCount = 0;
   unsigned int segmentCount = 0;
   unsigned int i;

   if(output) *output = 0;
   if(frozen_)
      return RestRouteStatus_Frozen;
   if(routeCount_ >= BAS_REST_MAX_ROUTES)
      return RestRouteStatus_RouteCapacity;
   if(!pattern || pattern[0] != '/' || !methodIsSingle(method))
      return RestRouteStatus_InvalidPattern;

   length = boundedLength(pattern, BAS_REST_MAX_PATTERN_LENGTH, terminated);
   if(!terminated)
      return RestRouteStatus_PatternTooLong;
   if(length == 0)
      return RestRouteStatus_InvalidPattern;

   route = &routes_[routeCount_];
   memset(route, 0, sizeof(*route));
   memcpy(route->pattern, pattern, length + 1);
   route->method = method;
   route->trailingSlash = (length > 1 && pattern[length - 1] == '/') ? 1 : 0;

   cursor = 1;
   end = length - (route->trailingSlash ? 1U : 0U);
   while(cursor < end)
   {
      size_t start = cursor;
      size_t segmentLength;
      Segment* segment;
      const char* colon;
      size_t nameLength;

      while(cursor < end && route->pattern[cursor] != '/')
         ++cursor;
      segmentLength = cursor - start;
      if(segmentLength == 0)
         return RestRouteStatus_InvalidPattern;
      if(segmentCount >= BAS_REST_MAX_SEGMENTS)
         return RestRouteStatus_SegmentCapacity;
      segment = &route->segments[segmentCount++];

      if(route->pattern[start] == '{' &&
         route->pattern[start + segmentLength - 1] == '}')
      {
         const char* content = route->pattern + start + 1;
         const size_t contentLength = segmentLength - 2;
         if(parameterCount >= BAS_REST_MAX_PARAMS || contentLength == 0)
            return parameterCount >= BAS_REST_MAX_PARAMS ?
               RestRouteStatus_ParameterCapacity : RestRouteStatus_InvalidPattern;
         colon = (const char*)memchr(content, ':', contentLength);
         nameLength = colon ? (size_t)(colon - content) : contentLength;
         if(!validParameterName(content, nameLength) || nameLength > 255)
            return RestRouteStatus_InvalidPattern;

         segment->textOffset = 0;
         segment->textLength = 0;
         segment->nameOffset = (U8)(start + 1);
         segment->nameLength = (U8)nameLength;
         if(!colon)
            segment->type = (U8)RestParameterType_String;
         else
         {
            const char* type = colon + 1;
            const size_t typeLength = contentLength - nameLength - 1;
            if(equalText(type, typeLength, "int", 3))
               segment->type = (U8)RestParameterType_Int;
            else if(equalText(type, typeLength, "uint", 4))
               segment->type = (U8)RestParameterType_UInt;
            else if(equalText(type, typeLength, "string", 6))
               segment->type = (U8)RestParameterType_String;
            else if(equalText(type, typeLength, "path", 4))
               segment->type = (U8)RestParameterType_Path;
            else
               return RestRouteStatus_InvalidPattern;
         }
         if(segment->type == (U8)RestParameterType_Path && cursor != end)
            return RestRouteStatus_InvalidPattern;
         ++parameterCount;
      }
      else
      {
         if(memchr(route->pattern + start, '{', segmentLength) ||
            memchr(route->pattern + start, '}', segmentLength) ||
            segmentLength > 255)
            return RestRouteStatus_InvalidPattern;
         segment->textOffset = (U8)start;
         segment->textLength = (U8)segmentLength;
         segment->nameOffset = 0;
         segment->nameLength = 0;
         segment->type = StaticSegment;
      }
      if(cursor < end)
         ++cursor;
   }

   route->segmentCount = (U8)segmentCount;
   route->parameterCount = (U8)parameterCount;

   for(i = 0; i < routeCount_; ++i)
   {
      const Route& existing = routes_[i];
      bool exact = true;
      bool numericAmbiguity = false;
      bool compatible = true;
      unsigned int segmentIndex;
      if(existing.method != method ||
         existing.trailingSlash != route->trailingSlash ||
         existing.segmentCount != route->segmentCount)
         continue;

      for(segmentIndex = 0; segmentIndex < route->segmentCount; ++segmentIndex)
      {
         const Segment& left = existing.segments[segmentIndex];
         const Segment& right = route->segments[segmentIndex];
         if(left.type == StaticSegment || right.type == StaticSegment)
         {
            if(left.type != right.type ||
               !equalText(existing.pattern + left.textOffset, left.textLength,
                          route->pattern + right.textOffset, right.textLength))
            {
               exact = false;
               compatible = false;
               break;
            }
         }
         else if(left.type != right.type)
         {
            const bool bothNumeric =
               (left.type == (U8)RestParameterType_Int ||
                left.type == (U8)RestParameterType_UInt) &&
               (right.type == (U8)RestParameterType_Int ||
                right.type == (U8)RestParameterType_UInt);
            if(bothNumeric)
            {
               numericAmbiguity = true;
               exact = false;
            }
            else
            {
               exact = false;
               compatible = false;
               break;
            }
         }
      }
      if(exact)
         return RestRouteStatus_Duplicate;
      if(compatible && numericAmbiguity)
         return RestRouteStatus_Ambiguous;
   }

   if(output) *output = route;
   return RestRouteStatus_Ok;
}

RestRouteStatus RestRouter::addRoute(HttpMethod method,
                                     const char* pattern,
                                     RestHandlerFunction handler,
                                     void* handlerContext)
{
   Route* route = 0;
   RestRouteStatus status;
   if(!handler)
      return RestRouteStatus_InvalidPattern;
   status = prepareRoute(method, pattern, &route);
   if(status != RestRouteStatus_Ok)
      return status;
   route->directHandler = handler;
   route->directContext = handlerContext;
   route->invoke = &invokeDirect;
   route->destroy = 0;
   ++routeCount_;
   return RestRouteStatus_Ok;
}

RestResult RestRouter::invokeDirect(const Route& route,
                                    RestContext& context,
                                    RestRequest& request,
                                    RestResponse& response)
{
   return route.directHandler(route.directContext, context, request, response);
}

RestRouter::PathMatch RestRouter::matchRoute(
   const Route& route,
   const char* suppliedPath,
   RestRequest::Parameter* parameters,
   unsigned int& parameterCount)
{
   bool terminated;
   size_t pathLength = boundedLength(suppliedPath,
                                     BAS_REST_MAX_REQUEST_PATH,
                                     terminated);
   size_t cursor = 0;
   size_t end;
   unsigned int segmentIndex;
   bool malformed = false;

   parameterCount = 0;
   if(!terminated)
      return PathMatch_TooLong;
   if(pathLength && suppliedPath[0] == '/')
   {
      ++cursor;
      --pathLength;
   }
   if(route.trailingSlash !=
      ((pathLength > 0 && suppliedPath[cursor + pathLength - 1] == '/') ? 1 : 0))
      return PathMatch_No;
   end = cursor + pathLength - (route.trailingSlash ? 1U : 0U);

   for(segmentIndex = 0; segmentIndex < route.segmentCount; ++segmentIndex)
   {
      const Segment& segment = route.segments[segmentIndex];
      size_t segmentEnd;
      RestStringView value;

      if(cursor >= end)
         return PathMatch_No;
      if(segment.type == (U8)RestParameterType_Path)
         segmentEnd = end;
      else
      {
         segmentEnd = cursor;
         while(segmentEnd < end && suppliedPath[segmentEnd] != '/')
            ++segmentEnd;
      }
      value = RestStringView(suppliedPath + cursor, segmentEnd - cursor);

      if(segment.type == StaticSegment)
      {
         if(!equalText(value.data(), value.size(),
                       route.pattern + segment.textOffset,
                       segment.textLength))
            return PathMatch_No;
      }
      else
      {
         RestRequest::Parameter& parameter = parameters[parameterCount++];
         parameter.name = RestStringView(route.pattern + segment.nameOffset,
                                         segment.nameLength);
         parameter.value = value;
         parameter.type = (RestParameterType)segment.type;
         parameter.signedValue = 0;
         parameter.unsignedValue = 0;

         if(value.size() == 0 || value.size() > BAS_REST_MAX_PARAMETER_LENGTH)
            malformed = true;
         else if(segment.type == (U8)RestParameterType_Int)
         {
            if(!parseSigned(value, parameter.signedValue))
               malformed = true;
         }
         else if(segment.type == (U8)RestParameterType_UInt)
         {
            if(!parseUnsigned(value, parameter.unsignedValue))
               malformed = true;
         }
      }

      cursor = segmentEnd;
      if(cursor < end)
      {
         if(suppliedPath[cursor] != '/')
            return PathMatch_No;
         ++cursor;
      }
   }
   if(cursor != end)
      return PathMatch_No;
   return malformed ? PathMatch_Malformed : PathMatch_Yes;
}

bool RestRouter::moreSpecific(const Route& candidate, const Route& current)
{
   unsigned int i;
   const unsigned int count = candidate.segmentCount < current.segmentCount ?
      candidate.segmentCount : current.segmentCount;
   for(i = 0; i < count; ++i)
   {
      const int candidateRank = segmentRank(candidate.segments[i].type);
      const int currentRank = segmentRank(current.segments[i].type);
      if(candidateRank != currentRank)
         return candidateRank > currentRank;
   }
   return candidate.segmentCount > current.segmentCount;
}

RestResult RestRouter::sendAutomaticResponse(HttpMethod method,
                                             U32 allowedMethods,
                                             RestResponse& response)
{
   struct MethodName { HttpMethod method; const char* name; };
   static const MethodName names[] = {
      {HttpMethod_Get, "GET"}, {HttpMethod_Head, "HEAD"},
      {HttpMethod_Options, "OPTIONS"}, {HttpMethod_Post, "POST"},
      {HttpMethod_Put, "PUT"}, {HttpMethod_Patch, "PATCH"},
      {HttpMethod_Delete, "DELETE"}
   };
   char allow[96];
   size_t used = 0;
   unsigned int i;
   RestResult result;

   if(allowedMethods & (U32)HttpMethod_Get)
      allowedMethods |= (U32)HttpMethod_Head;
   allowedMethods |= (U32)HttpMethod_Options;
   for(i = 0; i < sizeof(names)/sizeof(names[0]); ++i)
   {
      const size_t nameLength = strlen(names[i].name);
      if(!(allowedMethods & (U32)names[i].method))
         continue;
      if(used)
      {
         allow[used++] = ',';
         allow[used++] = ' ';
      }
      memcpy(allow + used, names[i].name, nameLength);
      used += nameLength;
   }
   allow[used] = 0;
   result = response.setHeader("Allow", allow);
   if(result.succeeded())
   {
      result = method == HttpMethod_Options ? response.noContent() :
         response.error(405, "Method Not Allowed");
   }
   return result;
}

RestResult RestRouter::dispatch(const char* relativePath,
                                HttpCommand* command) const
{
   const Route* best = 0;
   unsigned int bestMethodRank = 0;
   RestRequest::Parameter bestParameters[BAS_REST_MAX_PARAMS];
   unsigned int bestParameterCount = 0;
   U32 allowedMethods = 0;
   bool malformed = false;
   bool tooLong = false;
   unsigned int i;
   HttpRequest* nativeRequest;
   HttpMethod requestMethod;
   RestStringView path;

   if(!relativePath || !command)
      return RestResult(E_INVALID_PARAM);
   nativeRequest = HttpCommand_getRequest(command);
   requestMethod = HttpRequest_getMethodType(nativeRequest);
   path = RestStringView::fromCString(relativePath);

   for(i = 0; i < routeCount_; ++i)
   {
      const Route& route = routes_[i];
      RestRequest::Parameter parameters[BAS_REST_MAX_PARAMS];
      unsigned int parameterCount = 0;
      const PathMatch match = matchRoute(route, relativePath,
                                         parameters, parameterCount);
      unsigned int methodRank = 0;

      if(match == PathMatch_TooLong)
      {
         tooLong = true;
         continue;
      }
      if(match == PathMatch_Malformed)
      {
         malformed = true;
         continue;
      }
      if(match != PathMatch_Yes)
         continue;

      allowedMethods |= (U32)route.method;
      if(route.method == requestMethod)
         methodRank = 2;
      else if(requestMethod == HttpMethod_Head &&
              route.method == HttpMethod_Get)
         methodRank = 1;
      if(!methodRank || requestMethod == HttpMethod_Options)
         continue;

      if(!best || methodRank > bestMethodRank ||
         (methodRank == bestMethodRank && moreSpecific(route, *best)))
      {
         unsigned int parameterIndex;
         best = &route;
         bestMethodRank = methodRank;
         bestParameterCount = parameterCount;
         for(parameterIndex = 0; parameterIndex < parameterCount;
             ++parameterIndex)
            bestParameters[parameterIndex] = parameters[parameterIndex];
      }
   }

   {
      RestContext context(command);
      RestRequest request(nativeRequest, path);
      RestResponse response(HttpCommand_getResponse(command));

      if(requestMethod == HttpMethod_Options && allowedMethods)
         return sendAutomaticResponse(requestMethod, allowedMethods, response);
      if(best)
      {
         request.setParameters(bestParameters, bestParameterCount);
         return best->invoke(*best, context, request, response);
      }
      if(allowedMethods)
         return sendAutomaticResponse(requestMethod, allowedMethods, response);
      if(tooLong)
         return response.error(414, "URI Too Long");
      if(malformed)
         return response.error(400, "Malformed route parameter");
      return response.error(404, "Not Found");
   }
}

RestService::RestService(const char* name, RestRouter& router, S8 priority)
   : HttpDir(name, priority), router_(&router)
{
   router_->freeze();
   setService(&RestService::service);
}

int RestService::service(HttpDir* directory,
                         const char* relativePath,
                         HttpCommand* command)
{
   RestService* self = static_cast<RestService*>(directory);
   RestResult result;
   if(!command)
      return 0;

#if defined(__cpp_exceptions) || defined(__EXCEPTIONS) || defined(_CPPUNWIND)
   try
   {
      result = self->router_->dispatch(relativePath, command);
   }
   catch(...)
   {
      HttpResponse* nativeResponse = HttpCommand_getResponse(command);
      TRPR(("RestService: handler threw an exception\n"));
      if(!HttpResponse_committed(nativeResponse))
      {
         HttpResponse_resetBuffer(nativeResponse);
         HttpResponse_resetHeaders(nativeResponse);
         RestResponse response(nativeResponse);
         response.error(500, "Internal Server Error");
      }
      return 0;
   }
#else
   result = self->router_->dispatch(relativePath, command);
#endif

   if(result.succeeded())
   {
      return 0;
   }

   HttpResponse* nativeResponse = HttpCommand_getResponse(command);
   TRPR(("RestService: native response error %d\n", result.nativeCode()));
   if(!HttpResponse_committed(nativeResponse))
   {
      HttpResponse_resetBuffer(nativeResponse);
      HttpResponse_resetHeaders(nativeResponse);
      RestResponse response(nativeResponse);
      response.error(500, "Internal Server Error");
   }
   return 0;
}
