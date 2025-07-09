// This C++ implementation is designed to closely follow the structure
// of the C version. Its purpose is to show how to use the
// Barracuda Server C++ API in a way.


#include "RestJsonUtils.h"
#include "../../HostInit/OpenSocketCon.h"
#include <stdlib.h>
#include <new>

#define MAX_USERS 10

// This replaces the call to initializeJEncoder() in the C version
class JsonResp : public JEncoder
{
   JErr err;
public:
   JsonResp(HttpResponse* resp) : JEncoder(&err, resp->getWriter()) {}
};


class User
{
public:
   char* name;
   char* email;
   User() {}
   static User* create(const char* name, const char* email)
   {
      void* buffer = ::operator new(sizeof(User) + strlen(name) + strlen(email) + 2);
      User* user = new (buffer) User;
      user->name = reinterpret_cast<char*>(user + 1);
      user->email = user->name + strlen(name) + 1;
      strcpy(user->name, name);
      strcpy(user->email, email);
      return user;
   }
};

static User* users[MAX_USERS];

static void usersService(const char* path, const char* method, JVal* val, HttpResponse* resp, JsonResp* enc)
{
   int ix;
   JErr err;
   User* user;
   int id = -1;
   char* username = 0;
   char* email = 0;
   JErr errObj;

   if(!*path)
   {
      if(strcmp(method, "GET") == 0)
      {
         resp->setStatus(200);
         enc->beginArray();
         for (ix = 0, user = users[0]; ix < MAX_USERS; user = users[++ix])
         {
            if(user)
            {
               enc->set("{dss}", "id", ix + 1, "name", user->name, "email", user->email);
            }
         }
         enc->endArray();
      }
      else if(strcmp(method, "POST") == 0)
      {
         if(!val)
         {
            resp->setStatus(400);
            enc->set("{s}", "error", "JSON payload required");
         } 
         else
         {
            val->get(&errObj, "{ss}", "name", &username, "email", &email);
            if(username && email)
            {
               for (ix = 0, user = users[0]; ix < MAX_USERS; user = users[++ix])
               {
                  if(!user)
                     break;
               }
               if(ix < MAX_USERS)
               {
                  resp->setStatus(201);
                  users[ix] = User::create(username, email);
                  id = ix + 1;
                  enc->set("{sd}", "message", "User created", "id", id);
               } 
               else
               {
                  resp->setStatus(500);
                  enc->set("{s}", "error", "DB full");
               }
            } 
            else
            {
               resp->setStatus(400);
               enc->set("{s}", "error", "name and email required");
            }
         }
      }
   }
   else if(*path == '/' && strlen(path + 1) > 0 && (id = atoi(path + 1)) > 0 && id < MAX_USERS)
   {
      if(strcmp(method, "GET") == 0)
      {
         user = users[id - 1];
         if(user)
         {
            resp->setStatus(200);
            enc->set("{dss}", "id", id, "name", user->name, "email", user->email);
         }
         else
         {
            resp->setStatus(404);
            enc->set("{sd}", "error", "User not found", "id", id);
         }
      }
      else if(strcmp(method, "PUT") == 0)
      {
         if(!val)
         {
            resp->setStatus(400);
            enc->set("{s}", "error", "JSON payload required");
         }
         else
         {
            ix = id - 1;
            user = users[ix];
            if(user)
            {
               val->get(&errObj, "{ss}", "name", &username, "email", &email);
               if(username && email)
               {
                  delete user;
                  users[ix] = User::create(username, email);
                  enc->set("{sd}", "message", "User updated", "id", id);
               }
               else
               {
                  resp->setStatus(400);
                  enc->set("{s}", "error", "name and email required");
               }
            }
            else
            {
               resp->setStatus(404);
               enc->set("{sd}", "error", "User not found", "id", id);
            }
         }
      }
      else if(strcmp(method, "DELETE") == 0)
      {
         user = users[id - 1];
         if(user)
         {
            delete user;
            users[id - 1] = 0;
            resp->setStatus(204);
         }
         else
         {
            resp->setStatus(400);
            enc->set("{sd}", "error", "User not found", "id", id);
         }
      }
   }
   else
   {
      resp->setStatus(404);
      enc->set("{sd}", "error", "Invalid/unknown ID", "id", id);
   }

   if(resp->byteCount() == 0 && resp->statusCode != 204)
   {
      resp->setStatus(400);
      enc->set("{sss}", "error", "HTTP method cannot be used for the service", "method", method, "service", path);
   }
}

static int restService(HttpDir* o, const char* path, HttpCommand* cmd)
{
   JParserValFact* val;
   int status;
   HttpRequest* req = cmd->getRequest();
   HttpResponse* resp = cmd->getResponse();
   JsonResp enc(resp);
   if(strstr(path, "users") != path)
      return -1;
   val = parseJSON(req, &status);
   if(status)
      return 0;
   resp->setContentType("application/json; charset=UTF-8");
   usersService(path + strlen("users"), req->getMethod(), val ? val->getFirstVal() : 0, resp, &enc);
   if(val)
   {
      delete val;
   }
   return 0;
}

void installVirtualDir(HttpServer* server)
{
   static HttpDir restDir("api");
   restDir.setService(restService);
   server->insertRootDir(&restDir);
}

static HttpServer* createServer()
{
   HttpServerConfig scfg;
   static ThreadMutex mutex;
   static SoDisp dispatcher(&mutex);
   static HttpServer server(&dispatcher, &scfg);
   static HttpCmdThreadPool pool(&server, ThreadPrioNormal, BA_STACKSZ);
   scfg.setNoOfHttpCommands(3);
   scfg.setNoOfHttpConnections(12);
   scfg.setRequest(2 * 1024, 8 * 1024);
   scfg.setResponseData(8 * 1024);
   openSocketCon(&server, &dispatcher);
   return &server;
}

extern "C" void barracuda()
{
   HttpServer* server = createServer();
   installVirtualDir(server);
   server->getDispatcher()->run();
   baAssert(0);
}
