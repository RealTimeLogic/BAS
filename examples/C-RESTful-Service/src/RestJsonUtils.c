#include "RestJsonUtils.h"
#include <JParser.h>
#include <HttpRecData.h>

/**
 * Parses the HTTP request body if it contains JSON data.
 *
 * This function checks if the incoming HTTP request has a payload and a
 * `Content-Type` of `application/json`. If so, it reads the payload in chunks
 * and uses the JParser to construct a JParserValFact (JSON value factory).
 *
 * @param req    Pointer to the HttpRequest.
 * @param status Pointer to an integer that will be set to:
 *               - 0 on success
 *               - -1 on error (invalid or non-JSON data)
 *               - If no payload is present, status is set to 0 and NULL is returned.
 *
 * @return Pointer to a dynamically allocated JParserValFact on success,
 *         or NULL on error or if no JSON payload is present.
 *
 * Note: The function terminates the connection if a parsing error occurs.
 *       Caller must free the returned JParserValFact using JParserValFact_destructor
 *       and baFree().
 */
JParserValFact* parseJSON(HttpRequest* req, int* status)
{
   JParserValFact* valFact=0;
   if(HttpStdHeaders_getContentLength(HttpRequest_getStdHeaders(req)) ||
      HttpRequest_getHeaderValue(req, "Transfer-encoding"))
   {
      *status=-1;
      const char* ct = HttpRequest_getHeaderValue(req, "Content-Type");
      if(ct && !strcmp("application/json",ct) && HttpRecData_valid(req) >=0)
      {
         HttpRecData rd;
         HttpRecData_constructor(&rd,req);
         valFact = baMalloc(sizeof(JParserValFact)+1024+512); // + add two bufs 1024 & 512
         if(valFact)
         {
            S32 len;
            JParser parser;
            U8* buf = (U8*)(valFact+1)+1024; // Ptr 2 second buf: size 512
            JParserValFact_constructor(
               valFact, AllocatorIntf_getDefault(), AllocatorIntf_getDefault());
            JParser_constructor(&parser, (JParserIntf*)valFact,
                                (char*)(valFact+1), 1024, // Ptr 2 first buf & size
                                AllocatorIntf_getDefault(), 0);
            while( (len = HttpRecData_read(&rd,buf, 512)) > 0 )
            {
               if(JParser_parse(&parser, buf, len) < 0)
               {
                  break;
               }
            }
            if(JParsStat_DoneEOS == JParser_getStatus(&parser))
            {
               *status=0;
            }
            else
            {
               JParserValFact_destructor(valFact);
               baFree(valFact);
               valFact=0;
            }
            JParser_destructor(&parser);
         }
         HttpRecData_destructor(&rd);
      }
      if(*status)
      {
         // Close connection on error
         HttpConnection_setState(HttpRequest_getConnection(req),HttpConnection_Terminated);
      }
   }
   else
   {
      *status=0; // no payload
   }
   return valFact;
}


/*
 * Initializes a JEncoder for the given HTTP response.
 *
 * This function sets up the JSON encoder that can be used to write structured
 * JSON responses to the HTTP output buffer.
 *
 * @param resp Pointer to the HttpResponse object.
 * @param enc  Pointer to an uninitialized JEncoder object.
 * @param err  Pointer to an uninitialized JErr object to track JSON encoding errors.
*/
void initializeJEncoder(HttpResponse* resp, JEncoder* enc, JErr* err)
{
   JErr_constructor(err);
   BufPrint* out = HttpResponse_getWriter(resp);
   JEncoder_constructor(enc, err, out);
}
