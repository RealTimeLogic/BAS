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
 *            HEADER
 *
 *   $Id: BaTimer.h 5813 2026-06-15 10:15:50Z wini $
 *
 *   COPYRIGHT:  Real Time Logic LLC, 2008 - 2017
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

#ifndef __BaTimer_h
#define __BaTimer_h

#include <BaServerLib.h>
#include <SplayTree.h>
#include <DoubleList.h>
#include <ThreadLib.h>

/* The number of slots in the timer. This must be a value of 2^x */
#define BA_TIMER_SLOTS 32

/** Timer callback function.
    \param data Application data supplied when the timer was created.
    \return TRUE to keep a periodic timer active, or FALSE to remove the timer.
 */
typedef BaBool (*BaTimer_CB)(void* data);

/** Timer manager used for one-shot and periodic callbacks.

    BaTimer runs its own worker thread. Timer callbacks are therefore
    not executed in the HTTP request thread that created the timer.
 */
typedef struct BaTimer
#ifdef __cplusplus
: public Thread
{
      /** Create a BaTimer object and its worker thread.
         \param mutex Mutex shared with the server dispatcher.
         \param stackSize Worker thread stack size in bytes.
         \param ticklen Timer tick interval in milliseconds.
         \param priority Worker thread priority.
         \param alloc Optional allocator used for timer nodes.
       */
      BaTimer(ThreadMutex* mutex,int stackSize, U32 ticklen=10,
              ThreadPriority priority=ThreadPrioNormal,
              AllocatorIntf* alloc=0);
      /** Terminate the timer.
       */
      ~BaTimer();

      /** Create a timer event.
          \param cb the timer event function.
          \param data transparent data passed to the event callback.
          \param milliSec the timer callback function is activated in
          "millisecs" time, unless method "reset" or "cancelled" is
          called before the timer triggers.
          The callback runs in the timer thread, not in the web-server
          request thread.
          \returns the timer key.
       */
      size_t set(BaTimer_CB cb, void* data, U32 milliSec);

      /** Resets the timer.
          \param tkey the timer key.
          \param milliSec the new timeout.
          The timer is reset from the time this method is called.
      */
      int reset(size_t tkey, U32 milliSec);

      /** Cancels the timer.
          \param tkey the timer key.
          \return 0 on success or a non-zero value if the timer key was not found.
      */
      int cancel(size_t tkey);
#else
#if 0
}
#endif
{
   Thread super;
#endif
   DoubleList slots[BA_TIMER_SLOTS];
   DoubleList readyQ;
   SplayTree tnTree;
   ThreadMutex* mutex;
   AllocatorIntf* alloc;
   U32 ticklen;
   S16 dataInReadyQ;
   U16 curIndex;
} BaTimer;

#ifdef __cplusplus
extern "C" {
#endif  
BA_API void BaTimer_constructor(
   BaTimer* o, ThreadMutex* mutex,int stackSize, U32 ticklen,
   ThreadPriority priority, AllocatorIntf* alloc);
BA_API void BaTimer_destructor(BaTimer* o);
BA_API size_t BaTimer_set(BaTimer* o, BaTimer_CB cb, void* data, U32 milliSec);
BA_API int BaTimer_reset(BaTimer* o, size_t tkey, U32 milliSec);
BA_API int BaTimer_cancel(BaTimer* o, size_t tkey);
#ifdef __cplusplus
}
inline BaTimer::BaTimer(ThreadMutex* mutex,int stackSize, U32 ticklen,
                 ThreadPriority priority, AllocatorIntf* alloc) {
   BaTimer_constructor(this, mutex, stackSize, ticklen, priority,alloc);
}
inline BaTimer::~BaTimer() {
   BaTimer_destructor(this);
}
inline size_t BaTimer::set(BaTimer_CB cb, void* data, U32 milliSec) {
   return BaTimer_set(this, cb, data, milliSec);
}
inline int BaTimer::reset(size_t tkey, U32 milliSec) {
   return BaTimer_reset(this, tkey, milliSec);
}
inline int BaTimer::cancel(size_t tkey) {
   return BaTimer_cancel(this, tkey);
}
#endif  

#endif  
