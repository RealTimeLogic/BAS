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
 *   $Id: BaTimer.h 5978 2026-09-11 16:13:48Z wini $
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

/** @file BaTimer.h */

#ifndef __BaTimer_h
#define __BaTimer_h

#include <BaServerLib.h>
#include <SplayTree.h>
#include <DoubleList.h>
#include <ThreadLib.h>

/* The number of slots in the timer. This must be a value of 2^x */
#define BA_TIMER_SLOTS 32

/** Timer callback function.
    \param data Borrowed application pointer supplied to set(); may be NULL.
    The callback runs with the manager's mutex held. Keep data valid until
    cancellation or until the callback returns FALSE; it is never freed by
    the timer manager. Do not destroy the timer manager from its callback.
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
         \param mutex Required initialized mutex shared with the dispatcher.
         It is borrowed and must outlive the timer manager.
         \param stackSize Worker thread stack size in bytes.
         \param ticklen Positive timer tick interval in milliseconds; default
         10 in C++. Delays are quantized to this tick and worker scheduling,
         so this is not an exact-deadline facility. Zero is not supported.
         \param priority Worker thread priority; default ThreadPrioNormal in C++.
         \param alloc Borrowed allocator for timer nodes, or NULL to use
         AllocatorIntf_getDefault(). It must outlive the timer manager.
         Platform thread creation has no recoverable return status. With
         BA_TIMER_EXT_TICK enabled, only one timer manager is supported.
       */
      BaTimer(ThreadMutex* mutex,int stackSize, U32 ticklen=10,
              ThreadPriority priority=ThreadPrioNormal,
              AllocatorIntf* alloc=0);
      /** Stop the worker and discard outstanding timer events without calling
          their callbacks. Call from a thread other than the timer worker and
          prevent new operations during destruction. Waits for the worker to
          finish. If the caller owns the manager mutex, it is released during
          the wait and reacquired before returning. User data is not freed.
       */
      ~BaTimer();

      /** Create a timer event.
          \param cb Required event callback. Returning TRUE repeats the event;
          FALSE makes it a one-shot event.
          \param data Borrowed callback context; may be NULL. See BaTimer_CB.
          \param milliSec the timer callback function is activated in
          "millisecs" time, unless method "reset" or "cancelled" is
          called before the timer triggers.
          The callback runs in the timer thread, not in the web-server
          request thread.
          A zero delay still queues work; the callback is not called inline.
          \returns A nonzero event key on success, or zero if node allocation
          fails or shutdown has begun. The key is local to this manager.
       */
      size_t set(BaTimer_CB cb, void* data, U32 milliSec);

      /** Resets the timer.
          \param tkey the timer key.
          \param milliSec New delay in milliseconds, quantized to timer ticks.
          The timer is reset from the time this method is called.
          \return 0 on success, -1 if tkey was not found, or -2 if allocation
          of the replacement node fails or shutdown has begun. A -2 result
          leaves the old event cancelled; it does not preserve the old timer.
      */
      int reset(size_t tkey, U32 milliSec);

      /** Cancels the timer.
          \param tkey the timer key.
          \return 0 on cancellation, -1 if the key was not found. Cancellation
          does not free the application data. Internal node memory may be
          reclaimed later by the worker.
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
/** Initialize and start a timer manager.
 * @param[out] o Caller-owned storage.
 * @param[in] mutex Required borrowed mutex, held during callbacks.
 * @param[in] stackSize Worker stack bytes accepted by the platform Thread port.
 * @param[in] ticklen Positive tick interval in milliseconds.
 * @param[in] priority Worker thread priority.
 * @param[in] alloc Borrowed allocator, or NULL for the default.
 * @see BaTimer::BaTimer */
BA_API void BaTimer_constructor(
   BaTimer* o, ThreadMutex* mutex,int stackSize, U32 ticklen,
   ThreadPriority priority, AllocatorIntf* alloc);
/** Stop and release a timer manager's worker and queued events.
 * @param[in,out] o Initialized manager. See BaTimer::~BaTimer for thread,
 * synchronization and application-data lifetime requirements. */
BA_API void BaTimer_destructor(BaTimer* o);
/** Schedule an event; the manager acquires its mutex if needed.
 * @param[in,out] o Initialized manager.
 * @param[in] cb Required callback; see BaTimer_CB.
 * @param[in] data Borrowed callback context, possibly NULL.
 * @param[in] milliSec Delay in milliseconds; zero still queues asynchronously.
 * @return Nonzero event key, or zero on allocation failure or shutdown.
 * @see BaTimer::set */
BA_API size_t BaTimer_set(BaTimer* o, BaTimer_CB cb, void* data, U32 milliSec);
/** Replace an existing event's delay while preserving its key and callback.
 * @param[in,out] o Initialized manager; its mutex is acquired if needed.
 * @param[in] tkey Key returned by BaTimer_set.
 * @param[in] milliSec New delay in milliseconds.
 * @return 0 on success, -1 if not found, -2 if replacement allocation fails or
 * shutdown has begun. On -2, the original event is already cancelled. */
BA_API int BaTimer_reset(BaTimer* o, size_t tkey, U32 milliSec);
/** Cancel an event; its application data is not freed.
 * @param[in,out] o Initialized manager; its mutex is acquired if needed.
 * @param[in] tkey Key returned by BaTimer_set.
 * @return 0 on cancellation, -1 if the key is not present. */
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
