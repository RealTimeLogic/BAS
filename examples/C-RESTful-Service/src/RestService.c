#include "RestJsonUtils.h"
#include "../../HostInit/OpenSocketCon.h"
#include <stdlib.h>

#define MAX_USERS 10

// User structure holding name and email
typedef struct
{
   char* name;
   char* email;
} User;

// Static array of pointers to User objects, max 10 users
static User* users[MAX_USERS];

// Create and initialize a new User object using a single malloc
static User* createUser(const char* name, const char* email)
{
   User* user=malloc(sizeof(User)+strlen(name)+strlen(email)+2);
   if(!user)
      abort(); // Basic error management
   user->name=(char*)(user+1);
   user->email=(char*)(user+1)+strlen(name)+1;
   strcpy(user->name,name);
   strcpy(user->email,email);
   return user;
}

/*
 * Main user service handler for the /users endpoint.
 * Supports GET, POST, PUT, DELETE based on HTTP method and optional user ID path.
 */
static void usersService(const char* path, const char* method, JVal* val, HttpResponse* resp, JEncoder* enc)
{
   int ix;
   JErr err;
   User* user;
   int id = -1;
   char* username=0;
   char* email=0;
   JErr_constructor(&err); // Initialize error handler for JSON parsing

   if( ! *path )
   {
      // Handle /users endpoint without ID

      if(strcmp(method, "GET") == 0)
      {
         // Return all users in an array
         HttpResponse_setStatus(resp, 200);
         JEncoder_beginArray(enc);
         for (ix = 0, user = users[0]; ix < MAX_USERS; user = users[++ix])
         {
            if(user)
            {
               JEncoder_set(enc, "{dss}", "id",ix+1 , "name",user->name , "email",user->email);
            }
         }
         JEncoder_endArray(enc);
      }
      else if(strcmp(method, "POST") == 0)
      {
         // Create a new user
         if(!val)
         {
            HttpResponse_setStatus(resp, 400);
            JEncoder_set(enc, "{s}", "error","JSON payload required");
         }
         else
         {
            JVal_get(val,&err,"{ss}", "name",&username , "email",&email);
            if(username && email)
            {
               // Find free slot
               for(ix=0, user=users[0] ; ix < MAX_USERS ; user = users[++ix])
               {
                  if(!user)
                     break;
               }
               if(ix < MAX_USERS)
               {
                  HttpResponse_setStatus(resp, 201);
                  users[ix]=createUser(username, email);
                  id=ix+1;
                  JEncoder_set(enc, "{sd}", "message","User created" , "id",id);
               }
               else
               {
                  // No more space
                  HttpResponse_setStatus(resp, 500);
                  JEncoder_set(enc, "{s}", "error","DB full");
               }
            }
            else
            {
               // Missing fields
               HttpResponse_setStatus(resp, 400);
               JEncoder_set(enc, "{s}", "error","name and email required");
            }
         }
      }
   }
   // Handle /users/{id}
   else if(*path == '/' && strlen(path + 1) > 0 && (id = atoi(path + 1)) > 0 && id < MAX_USERS)
   {
      if(strcmp(method, "GET") == 0)
      {
         // Retrieve a user by ID
         user=users[id-1];
         if(user)
         {
            HttpResponse_setStatus(resp, 200);
            JEncoder_set(enc, "{dss}", "id",id , "name",user->name , "email",user->email);
         }
         else
         {
            HttpResponse_setStatus(resp, 404);
            JEncoder_set(enc, "{sd}", "error","User not found" , "id",id);
         }
      }
      else if(strcmp(method, "PUT") == 0)
      {
         // Update user
         if(!val)
         {
            HttpResponse_setStatus(resp, 400);
            JEncoder_set(enc, "{s}", "error","JSON payload required");
         }
         else
         {
            ix = id - 1;
            user=users[ix];
            if(user)
            {
               JVal_get(val, &err, "{ss}", "name",&username , "email",&email);
               if(username && email)
               {
                  free(users[ix]);
                  users[ix]=createUser(username, email);
                  JEncoder_set(enc, "{sd}", "message","User updated" , "id",id);
               }
               else
               {
                  HttpResponse_setStatus(resp, 400);
                  JEncoder_set(enc, "{s}", "error","name and email required");
               }
            }
            else
            {
               HttpResponse_setStatus(resp, 404);
               JEncoder_set(enc, "{sd}", "error","User not found" , "id",id);
            }
         }
      }
      else if(strcmp(method, "DELETE") == 0)
      {
         // Delete user
         user=users[id-1];
         if(user)
         {
            free(user);
            users[id-1]=0;
            HttpResponse_setStatus(resp, 204);
         }
         else
         {
            HttpResponse_setStatus(resp, 400);
            JEncoder_set(enc, "{sd}", "error","User not found" , "id",id);
         }
      }
   }
   else
   {
      // Invalid or out-of-range ID
      HttpResponse_setStatus(resp, 404);
      JEncoder_set(enc, "{sd}", "error","Invalid/unknown ID" , "id",id);
   }

   // Catch-all if no response was written and method is not DELETE (204)
   if(HttpResponse_byteCount(resp) == 0 && resp->statusCode != 204)
   {
      HttpResponse_setStatus(resp, 400);
      JEncoder_set(enc, "{sss}", "error","HTTP method cannot be used for the service" , "method",method, "service",path);
   }
}

/*
 * Main REST service handler for /api/users*
 * Validates JSON payload and dispatches to user handler.
 */
static int restService(HttpDir *o, const char *path, HttpCommand *cmd)
{
   JEncoder enc;
   JErr err;
   JParserValFact* val;
   int status;

   HttpRequest* req = HttpCommand_getRequest(cmd);
   HttpResponse* resp = HttpCommand_getResponse(cmd);

   // Only handle paths beginning with "users"
   if(strstr(path, "users") != path)
   {
      return -1; // Not handled here.
   }

   val = parseJSON(req, &status); // Parse JSON request body; function in RestJsonUtils.c
   if(status)
   {
      return 0; // Failed. Connection closed by parseJSON
   }

   HttpResponse_setContentType(resp,"application/json; charset=UTF-8");
   initializeJEncoder(resp, &enc, &err); // 'enc' used when sending JSON resp.; function in RestJsonUtils.c

   // Route to user handler with the trimmed path
   usersService(path + strlen("users"), HttpRequest_getMethod(req), val ? JParserValFact_getFirstVal(val) : 0, resp, &enc);

   if(val)
   {
      JParserValFact_destructor(val);
      baFree(val);
   }

   JEncoder_destructor(&enc); // Finalize (flush) JSON output
   return 0;
}

/*
 * Installs the REST virtual directory at /api/
 */
void
installVirtualDir(HttpServer* server)
{
   static HttpDir restDir;
   HttpDir_constructor(&restDir, "api", 0);
   HttpDir_setService(&restDir, restService);
   HttpServer_insertRootDir(server, &restDir);
}

/*
 * Creates and configures the HTTP server instance.
 */
static HttpServer* createServer(void)
{
   HttpServerConfig scfg;
   static ThreadMutex mutex;
   static SoDisp dispatcher;
   static HttpServer server;
   static HttpCmdThreadPool pool;


   ThreadMutex_constructor(&mutex);
   SoDisp_constructor(&dispatcher, &mutex);

   HttpServerConfig_constructor(&scfg);
   HttpServerConfig_setNoOfHttpCommands(&scfg,3); /* Ref t: 3 threads */
   HttpServerConfig_setNoOfHttpConnections(&scfg,12);
   HttpServerConfig_setRequest(&scfg,2*1024, 8*1024); // request buffer sizes
   HttpServerConfig_setResponseData(&scfg,8*1024);   // response buffer size
   HttpServer_constructor(&server, &dispatcher, &scfg);
   HttpCmdThreadPool_constructor(&pool, &server, ThreadPrioNormal, BA_STACKSZ); /* Ref t */
   openSocketCon(&server, &dispatcher); /* Initialize TCP listening */
   return &server;
}

/*
 * Barracuda entry point; sets up the HTTP server and enters dispatcher loop.
 */
extern void barracuda(void)
{
   HttpServer* server = createServer();
   installVirtualDir(server);
   SoDisp_run(HttpServer_getDispatcher(server), -1); // Start processing HTTP requests

   baAssert(0); // This point should never be reached
}
