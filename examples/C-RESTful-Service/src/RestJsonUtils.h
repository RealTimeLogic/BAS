#include <barracuda.h>
#include <JEncoder.h>
#include <JParser.h>

#ifdef __cplusplus
extern "C" {
#endif

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
JParserValFact* parseJSON(HttpRequest* req, int* status);


/**
 * Initializes a JEncoder for the given HTTP response.
 *
 * This function sets up the JSON encoder that can be used to write structured
 * JSON responses to the HTTP output buffer.
 *
 * @param resp Pointer to the HttpResponse object.
 * @param enc  Pointer to an uninitialized JEncoder object.
 * @param err  Pointer to an uninitialized JErr object to track JSON encoding errors.
 */
void initializeJEncoder(HttpResponse* resp, JEncoder* encoder, JErr* err);


#ifdef __cplusplus
}
#endif
