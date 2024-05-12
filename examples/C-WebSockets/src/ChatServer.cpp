/*
 *     ____             _________                __                _     
 *    / __ \___  ____ _/ /_  __(_)___ ___  ___  / /   ____  ____ _(_)____
 *   / /_/ / _ \/ __ `/ / / / / / __ `__ \/ _ \/ /   / __ \/ __ `/ / ___/
 *  / _, _/  __/ /_/ / / / / / / / / / / /  __/ /___/ /_/ / /_/ / / /__  
 * /_/ |_|\___/\__,_/_/ /_/ /_/_/ /_/ /_/\___/_____/\____/\__, /_/\___/  
 *                                                       /____/          
 *
 *                  Barracuda Embedded Web-Server 
 ****************************************************************************
 *
 *   $Id$
 *
 *   COPYRIGHT:  Real Time Logic
 *               https://realtimelogic.com
 *
 *   The copyright to the program herein is the property of
 *   Real Time Logic. The program may be used or copied only
 *   with the written permission from Real Time Logic or
 *   in accordance with the terms and conditions stipulated in
 *   the agreement under which the program has been supplied.
 ****************************************************************************
 
The following code example shows how to implement a basic WebSocket
chat server by using the WebSocket server library:
https://realtimelogic.com/ba/doc/en/C/reference/html/group__WebSockets.html

The client side JavaScript implementation can be found in ../html/index.html

The WebSocket library enables asynchronous sockets, where receiving
data is event driven, and writing data is blocking. This concept is
thoroughly documented in the Lua documentation, which provides the
same concept:
https://realtimelogic.com/ba/doc/?url=SockLib.html#AsynchronousSockets

Since writing data is blocking and broadcasting data to all connected
clients could potentially block the caller, a separate thread is
constructed for the purpose of broadcasting data to all connected
clients. See ChatPage::send2allThread for more information.

The Barracuda Server library is implemented in C code, but provides
wrappers for C++. This example shows how to use the C++
interface. Details:

ANSI C and Object Oriented Programming:
https://realtimelogic.com/ba/doc/en/C/introduction.html#refman_cpp
*/

#include "../../HostInit/OpenSocketCon.h"
#include <WebSocketServer.h>
#include <DoubleList.h>
#include <SingleList.h>
#include <HttpResRdr.h>
#include <BaDiskIo.h>
#include <ZipIo.h>
#include <stdio.h>

#if defined(_DEBUG) && !defined(BAIO_DISK)
// Read resources from file system
#define BAIO_DISK
#endif

// Compile with -DBAIO_DISK if you want to load pages from file system
#if !defined(BAIO_DISK)
// Else: read resources from embedded ZIP file html.zip.c
extern "C" ZipReader * getZipReader(void);
#endif 

class ChatPage;

/* One object (WebSocket connection) per connected chat client.
   WSS: inherits from WSS (WebSocket Server Connection)
   WSSCB: object also manages the WebSocket callbacks frame, close, and ping
   DoubleLink: instance can be inserted/removed from ChatPage::chatList
 */
class ChatConnection : public DoubleLink, public WSSCB, public WSS
{
   static void frame(WSSCB* o,WSS* wss,void* data,int len,int text);
   static void close(WSSCB* o, WSS* wss, int status);
   static void ping(WSSCB* o,WSS* wss,void* data,int len);
   class ChatPage* page;
public:
   ChatConnection(SoDisp* disp, ChatPage* p);
};


/* The chat entry page for WebSocket upgrade requests.
   HttpPage: the instance is inserted into the server's virtual file system
   Thread: Method send2allThread runs as a separate thread
 */
class ChatPage : public HttpPage, public Thread
{
    /* list of connected users: type ChatConnection */
   DoubleList chatList;
    /* Queued messages managed by send2allThread, type: ChatMessage */
   SingleList msgQueue;
   /* Semaphore used for starting and stopping send2allThread */
   ThreadSemaphore sem;
   SoDisp* dispatcher;
   int userCounter; /* Number of connected chat clients */
   /* WebSocket entry page */
   static void service(HttpPage* page, HttpRequest* req, HttpResponse* resp);
   static void send2allThread(Thread* thread);
public:
   ChatPage(const char *pageName, SoDisp* dispatcher);
   void send2all(void* data,int len,bool isText);
   void decrementUserCounter() { userCounter--; }
};


/* A chat message created in ChatPage::send2all and processed in
 * ChatPage::send2allThread.
 */
struct ChatMessage : public SingleLink
{
   bool isTxt;
   int size;
   U8 data[1];
   void *operator new(size_t, void *place) { return place; }
   void operator delete(void* d, void*) { if(d) ::baFree(d); }
   ChatMessage(void* data, int len, bool isText);
};


ChatMessage::ChatMessage(void* d, int len, bool txt)
{
   memcpy(data,d,len);
   size=len;
   isTxt=txt;
}


ChatConnection::ChatConnection(SoDisp* disp, ChatPage* p) :
   WSSCB(frame,close,ping),
   WSS(this,disp,8*1024,8*1024),
   page(p)
{
}


/* WSSCB::frame callback: required */
void ChatConnection::frame(WSSCB* o,WSS* wss,void* data,int len,int isText)
{
   ChatConnection* self = (ChatConnection*)o;
   if(isText)
   {
      DynBuffer db(len+100, len);
      db.printf("User %p: %s",self,data);
      self->page->send2all(db.getBuf(), db.getBufSize(), true);
      printf("%s\n",db.getBuf());
   }
   else
   {
      printf("Image from %p, len = %d\n",self,len);
      self->page->send2all(data, len, false);
   }
}


/* WSSCB::close callback: required */
void ChatConnection::close(WSSCB* o, WSS* wss, int status)
{
   ChatConnection* self = (ChatConnection*)o;
   char buf[50];
   basnprintf(buf,sizeof(buf),"User %p leaving: status %d", self, status);
   printf("%s\n",buf);
   self->page->send2all(buf, strlen(buf), true);
   self->page->decrementUserCounter();
   delete self;
}


/* WSSCB::ping callback: optional */
void ChatConnection::ping(WSSCB* o,WSS* wss,void* data,int len)
{
   printf("Client Ping %p\n",wss);
}


/* HttpPage::service callback.
 * Activated when the server's virtual file system delegates the
 * request to this page. The function upgrades (morphes) the request
 * into a WebSocket connection.
 */
void ChatPage::service(HttpPage* page, HttpRequest* req, HttpResponse* resp)
{
   ChatPage* self = (ChatPage*)page;
   if(req->wsUpgrade()) /* WebSocket handshake */
   {
      resp->sendError(400, "Not a WebSocket Request");
      return;
   }
   else
   {
      ChatConnection* ws = new ChatConnection(
         req->getServer()->getDispatcher(), self);
      /** WSS:connect. WebSocket handshake done above. Morph request
       * into a WebSocket connection.
       */
      if(ws->connect(req->getConnection()))
      {
         delete ws; /* If the client prematurely closed the connection */
      }
      else
      {
         char buf[30];
         self->userCounter++;
         basnprintf(buf,sizeof(buf),"You are user %p.", self);
         /* Send a short message to the new chat client */
         ws->write(buf, strlen(buf), true);
         basnprintf(buf,sizeof(buf),"User %p entering.", self);
         /* Send message to all connected clients */
         self->send2all(buf, strlen(buf), true);
         /* Add the new client to the chat list */
         self->chatList.insertFirst(ws);
      }
   }
}


/* Creates a ChatMessage from args, inserts the message into the
 * message queue, and activates the send2allThread.
 */
void ChatPage::send2all(void* data, int len, bool isText)
{
   void* buf = baMalloc(sizeof(ChatMessage)+len);
   if(buf)
   {
      ChatMessage* msg = new(buf) ChatMessage(data, len, isText);
      msgQueue.insertLast(msg);
      sem.signal();
   }
}


/* This method, which runs as a separate thread, waits for
 * ChatMessages from ChatPage::send2all. When a chat message is
 * received, the message's content is sent to all connected chat
 * clients.
 */
void ChatPage::send2allThread(Thread* thread)
{
   ChatPage* self = (ChatPage*)thread;
   for(;;)
   {
      /* Wait for activation from method ChatPage::send2all */
      self->sem.wait();
      /* The function is running in it's own thread and is not running
       * in the context of the socket dispatcher thread. We must for
       * this reason lock the dispatcher thread when working. Each
       * call to WSS::write below releases the mutex while sending
       * socket data.
       */
      ThreadLock dispLock(self->dispatcher->getMutex());
      while( ! self->msgQueue.isEmpty() )
      {
         ChatMessage* msg = (ChatMessage*)self->msgQueue.removeFirst();
         DoubleListEnumerator e(&self->chatList);
         int users = self->userCounter;
         for(DoubleLink* link = e.getElement() ; link ; link = e.nextElement())
         {
            ((ChatConnection*)link)->write(msg->data, msg->size, msg->isTxt);
            if(users != self->userCounter)
            {
               /* Method write releases the dispatcher mutex.
                  if users != self->userCounter:
                    We had a disconnect while the mutex was released and
                    we can no longer trust the data and current pointer,
                    which may be deleted, in the iterator.
                */
               break;
            }
         }
         baFree(msg); /* Release memory allocated in ChatPage::send2all */
      }
      /* dispLock goes out of scope and destructor releases the mutex */
   }
}


ChatPage::ChatPage(const char *pageName, SoDisp* disp) :
   HttpPage(service,pageName),
   Thread(send2allThread, ThreadPrioNormal, 6000)
{
   dispatcher=disp;
   userCounter=0;
   start(); // Thread::start. Activate ChatPage::send2allThread
}



/* Create a resource reader (HttpResRdr) for reading static content,
 * insert the resource reader into the virtual file system, create a
 * ChatPage instance and insert the page as an HttpPage in the resource
 * reader (HttpResRdr with base class HttpDir).
 * 
 * Note that the virtual file system is not the operating system's
 * file system. See the following link for an introduction to
 * assembling a virtual file system, and consider the Lua code
 * examples as a pseudo-code.
 * https://realtimelogic.com/ba/doc/en/GettingStarted.html#VFS
 */
void
installVirtualDir(HttpServer* server)
{
// If using the file system for accessing web resources.
#if defined(BAIO_DISK)
   int status;
   // https://realtimelogic.com/ba/doc/en/C/reference/html/structDiskIo.html
   static DiskIo io;
   // In an embedded device, the file system root may be something like:
   // /mount-point/www
   status=io.setRootDir("../../html"); // host: executing from ./obj/release
   if(status)
      status=io.setRootDir("../html"); // host: executing from ./obj
   if(status)
      status=io.setRootDir("html"); // host: executing from ./
   if(status)
   {
      HttpTrace::printf(0, "Cannot set DiskIo ../../html directory: %s\n",
                       baErr2Str(status));
      baFatalE(FE_USER_ERROR_1, 0);
   }
#else // Using embedded ZIP file; all web resources are packaged in a zip file
   // https://realtimelogic.com/ba/doc/en/C/reference/html/structZipReader.html
   ZipReader* zipReader = getZipReader();
   if (!zipReader->isValid())
      baFatalE(FE_USER_ERROR_2, 0);
   // https://realtimelogic.com/ba/doc/en/C/reference/html/structZipIo.html
   static ZipIo io(zipReader);
#endif
   // https://realtimelogic.com/ba/doc/en/C/reference/html/structHttpResRdr.html
   static HttpResRdr readDir(&io, 0);
   server->insertDir(0,&readDir);
   static ChatPage wsp("my-web-socket-service", server->getDispatcher());
   readDir.insertPage(&wsp);
}



/* This function creates one HttpServer, one SoDisp and one
   HttpServCon object from static memory. This function should
   therefore be called one time only.
   https://realtimelogic.com/ba/doc/en/C/reference/html/structSoDisp.html
   https://realtimelogic.com/ba/doc/en/C/reference/html/structHttpServer.html
 */
static HttpServer*
createServer(void)
{
   static ThreadMutex mutex;
   static SoDisp dispatcher(&mutex);
   static HttpServer server(&dispatcher);
   openSocketCon(&server, &dispatcher); /* OpenSocketCon.h */
   return &server;
}


/* 
  This is the "main" barracuda function. This function initializes the
  web-server, sets up the Virtual File System and enters a forever
  loop.

  This function is typically called from a thread/task dedicated to
  the Barracuda web-server.
*/
extern "C" void
barracuda(void)
{
   HttpServer* server = createServer();
   installVirtualDir(server);

   /*
     The dispatcher object waits for incoming HTTP requests. These
     requests are delegated to the HttpServer object.
   */
   server->getDispatcher()->run(); /* Never returns */
   baAssert(0);
}
