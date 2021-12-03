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
 *   $Id: ThreadLibArch.h 2648 2012-04-26 20:27:21Z wini $
 *
 *   COPYRIGHT:  Real Time Logic, 2012
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
 *
 */

#ifndef ThreadLib_hpp
#define ThreadLib_hpp

#include <kproject.h>
#include <kcounter.h>
#include <rtxcapi.h>

#ifdef NDEBUG
#define Thread_ce(x) x
#else
#ifdef __cplusplus
extern "C" {
#endif
void Thread_cef(KSRC status, const char* file, int line);
#ifdef __cplusplus
}
#endif
#define Thread_ce(x) Thread_cef(x, __FILE__, __LINE__)
#endif

struct ThreadSemaphoreBase;
struct ThreadMutexBase;

typedef struct ThreadMutexBase
{
   MUTX mutex;
} ThreadMutexBase;

#define ThreadMutex_constructor(o) \
	{\
		Thread_ce(KS_OpenMutx(0,&(o)->mutex)); \
		MUTXPROP sprop;\
		sprop.attributes = 0;\
		sprop.attributes |= ATTR_INVERSION;\
		sprop.attributes |= ATTR_FIFO_ORDER;\
		KS_DefMutxProp( (o)->mutex, &sprop );\
	}
										
#define ThreadMutex_destructor(o) Thread_ce(KS_CloseMutx((o)->mutex))
#define ThreadMutex_set(o) KS_TestMutxW((o)->mutex)
#define ThreadMutex_release(o) Thread_ce(KS_ReleaseMutx((o)->mutex))
#define ThreadMutex_isOwner(o) (KS_GetMutxOwner((o)->mutex,0) == KS_GetTaskID())

typedef struct ThreadSemaphoreBase
{
   SEMA sem;
} ThreadSemaphoreBase;

   
#define ThreadSemaphore_constructor(o) \
	Thread_ce(KS_OpenSema(0,&(o)->sem));\
	SEMAPROP semaprop;\
	semaprop.attributes = 0;\
	semaprop.attributes |= ATTR_FIFO_ORDER;\
	semaprop.attributes |= ATTR_BINARY_SEMAPHORE;\
	KS_DefSemaProp( (o)->sem, &semaprop );\
	KS_DefSemaCount( (o)->sem, (SEMACOUNT)0 );

#define ThreadSemaphore_destructor(o)  Thread_ce(KS_CloseSema((o)->sem))
#define ThreadSemaphore_wait(o) KS_TestSemaW((o)->sem)
#define ThreadSemaphore_signal(o) KS_SignalSema((o)->sem)


typedef struct ThreadBase
{
   TASKPROP tprop;
   TASK tid;
   Thread_Run runnable;
} ThreadBase;


#define Thread_sleep(milliseconds) \
  KS_SleepTask (COUNTER1, (TICKS)milliseconds/CLKTICK)
#define Thread_start(o) KS_ExecuteTask((o)->tid)

#ifdef __cplusplus
extern "C" {
#endif
void Thread_constructor(
   ThreadBase* o, Thread_Run r, ThreadPriority priority, int stackSize);
 void Thread_destructor(struct ThreadBase* o);
#ifdef __cplusplus
}
#endif


#endif
