/*
 *     ____             _________                __                _
 *    / __ \___  ____ _/ /_  __(_)___ ___  ___  / /   ____  ____ _(_)____
 *   / /_/ / _ \/ __ `/ / / / / / __ `__ \/ _ \/ /   / __ \/ __ `/ / ___/
 *  / _, _/  __/ /_/ / / / / / / / / / / /  __/ /___/ /_/ / /_/ / / /__
 * /_/ |_|\___/\__,_/_/ /_/ /_/_/ /_/ /_/\___/_____/\____/\__, /_/\___/
 *                                                       /____/
 *
 *                  Barracuda Embedded Web-Server
 *
 ****************************************************************************
 *			      HEADER
 *
 *   $Id: ThreadLibArch.h 4915 2021-12-01 18:26:55Z wini $
 *
 *   COPYRIGHT:  Real Time Logic, 2004
 *
 *   This software is copyrighted by and is the sole property of Real
 *   Time Logic LLC.  All rights, title, ownership, or other interests in
 *   the software remain the property of Real Time Logic LLC.  This
 *   software may only be used in accordance with the terms and
 *   conditions stipulated in the corresponding license agreement under
 *   which the software has been supplied.  Any unauthorized use,
 *   duplication, transmission, distribution, or disclosure of this
 *   software is expressly forbidden.
 *                                                                        
 *   This Copyright notice may not be removed or modified without prior
 *   written consent of Real Time Logic LLC.
 *                                                                         
 *   Real Time Logic LLC. reserves the right to modify this software
 *   without notice.
 *
 *               http://www.realtimelogic.com
 ****************************************************************************
 */

#ifndef ThreadLib_hpp
#define ThreadLib_hpp

#include <TargConfig.h>
#include <windows.h>
#include <winbase.h>

struct ThreadSemaphoreBase;

typedef struct ThreadMutexBase
{
      DWORD tid;  /* Lock owner */
      CRITICAL_SECTION section;
} ThreadMutexBase;

#define ThreadMutex_constructor(o) do{\
   (o)->tid = 0;\
   InitializeCriticalSection(&(o)->section);\
}while(0)

#define ThreadMutex_destructor(o)  DeleteCriticalSection(&(o)->section)

#define ThreadMutex_set(o) do{  \
   EnterCriticalSection(&(o)->section);\
   (o)->tid = GetCurrentThreadId(); \
}while(0)

#define ThreadMutex_release(o) do{\
   (o)->tid = 0;\
   LeaveCriticalSection(&(o)->section);\
}while(0)

#define ThreadMutex_isOwner(o) ((o)->tid == GetCurrentThreadId())

typedef struct ThreadSemaphoreBase
{
      HANDLE semaphore;
} ThreadSemaphoreBase;


#define ThreadSemaphore_constructor(o) \
         (o)->semaphore = CreateSemaphore(0, 0, 0x7ffffff, 0)
#define ThreadSemaphore_destructor(o) \
         CloseHandle((o)->semaphore)
#define ThreadSemaphore_wait(o) \
         WaitForSingleObject((o)->semaphore, INFINITE)
#define ThreadSemaphore_signal(o) \
         ReleaseSemaphore((o)->semaphore, 1, 0)


typedef struct ThreadBase
{
      HANDLE id; /* actually the hThread */
      Thread_Run runnable;
} ThreadBase;

#define Thread_start(o) ResumeThread((o)->id)
#define Thread_sleep(milliseconds) Sleep(milliseconds)

#ifdef __cplusplus
extern "C" {
#endif
BA_API void Thread_constructor(
   ThreadBase* o, Thread_Run r, ThreadPriority priority, int stackSize);
BA_API void Thread_destructor(Thread* o);
#ifdef __cplusplus
}
#endif

#endif
