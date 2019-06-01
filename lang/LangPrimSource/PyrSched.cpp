/*
    SuperCollider real time audio synthesis system
    Copyright (c) 2002 James McCartney. All rights reserved.
    http://www.audiosynth.com

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
*/


#include "PyrKernel.h"
#include "PyrSched.h"
#include "GC.h"
#include "PyrPrimitive.h"
#include "PyrSymbol.h"
#ifdef __APPLE__
#    include <CoreAudio/HostTime.h>
#endif
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits>
#include <functional>

#if defined(__APPLE__) || defined(__linux__)
#    include <pthread.h>
#endif

#include "SCBase.h"
#include "SC_Lock.h"
#include "SC_Clock.hpp"
#include "SC_LinkClock.hpp"

#include <boost/sync/semaphore.hpp>
#include <boost/sync/support/std_chrono.hpp>

// FIXME: These includes needs to be last on Windows, otherwise Ableton build breaks
// (Windows header include ordering dependencies)
#include "SC_Win32Utils.h"
#ifdef _MSC_VER
#    include "wintime.h"
#else
#    include <sys/time.h>
#endif

static const double dInfinity = std::numeric_limits<double>::infinity();

void runAwakeMessage(VMGlobals* g);


bool addheap(VMGlobals* g, PyrObject* heapArg, double schedtime, PyrSlot* task) {
    PyrHeap* heap = (PyrHeap*)heapArg;
#ifdef GC_SANITYCHECK
    g->gc->SanityCheck();
#endif
    if (heap->size >= ARRAYMAXINDEXSIZE(heap))
        return false;
    assert(heap->size);

    // 	post("->addheap\n");
    // 	dumpheap(heapArg);

    /* parent and sibling in the heap, not in the task hierarchy */
    int mom = heap->size - 1;
    PyrSlot* pme = heap->slots + mom;
    int stabilityCount = slotRawInt(&heap->count);
    SetRaw(&heap->count, stabilityCount + 1);

    for (; mom > 0;) { /* percolate up heap */
        int newMom = ((mom - 3) / 2);
        mom = newMom - newMom % 3; /// LATER: we could avoid the division by using 4 slots per element
        PyrSlot* pmom = heap->slots + mom;
        if (schedtime < slotRawFloat(pmom)) {
            assert(slotRawInt(pmom + 2) < stabilityCount);
            slotCopy(&pme[0], &pmom[0]);
            slotCopy(&pme[1], &pmom[1]);
            slotCopy(&pme[2], &pmom[2]);
            pme = pmom;
        } else
            break;
    }
    SetFloat(&pme[0], schedtime);
    slotCopy(&pme[1], task);
    SetInt(&pme[2], stabilityCount);
    g->gc->GCWrite(heap, task);
    heap->size += 3;

#ifdef GC_SANITYCHECK
    g->gc->SanityCheck();
#endif
    // 	dumpheap(heapArg);
    // 	post("<-addheap %g\n", schedtime);
    return true;
}


bool lookheap(PyrObject* heap, double* schedtime, PyrSlot* task) {
    if (heap->size > 1) {
        *schedtime = slotRawFloat(&heap->slots[0]);
        slotCopy(task, &heap->slots[1]);
        return true;
    } else
        return false;
}

bool getheap(VMGlobals* g, PyrObject* heapArg, double* schedtime, PyrSlot* task) {
    PyrHeap* heap = (PyrHeap*)heapArg;
    PyrGC* gc = g->gc;
    bool isPartialScanObj = gc->IsPartialScanObject(heapArg);
    assert(heap->size);

    // 	post("->getheap\n");
    // 	dumpheap(heapArg);
    if (heap->size > 1) {
        *schedtime = slotRawFloat(&heap->slots[0]);
        slotCopy(task, &heap->slots[1]);
        heap->size -= 3;
        int size = heap->size - 1;
        slotCopy(&heap->slots[0], &heap->slots[size]);
        slotCopy(&heap->slots[1], &heap->slots[size + 1]);
        slotCopy(&heap->slots[2], &heap->slots[size + 2]);

        /* parent and sibling in the heap, not in the task hierarchy */
        int mom = 0;
        int me = 3;
        PyrSlot* pmom = heap->slots + mom;
        PyrSlot* pme = heap->slots + me;
        PyrSlot* pend = heap->slots + size;
        double timetemp = slotRawFloat(&pmom[0]);
        int stabilityCountTemp = slotRawInt(&pmom[2]);
        PyrSlot tasktemp;
        slotCopy(&tasktemp, &pmom[1]);
        for (; pme < pend;) {
            /* demote heap */
            if (pme + 3 < pend
                && ((slotRawFloat(&pme[0]) > slotRawFloat(&pme[3]))
                    || ((slotRawFloat(&pme[0]) == slotRawFloat(&pme[3]))
                        && (slotRawInt(&pme[2]) > slotRawInt(&pme[5]))))) {
                me += 3;
                pme += 3;
            }
            if (timetemp > slotRawFloat(&pme[0])
                || (timetemp == slotRawFloat(&pme[0]) && stabilityCountTemp > slotRawInt(&pme[2]))) {
                slotCopy(&pmom[0], &pme[0]);
                slotCopy(&pmom[1], &pme[1]);
                slotCopy(&pmom[2], &pme[2]);
                if (isPartialScanObj) {
                    gc->GCWriteBlack(pmom + 1);
                }
                pmom = pme;
                me = ((mom = me) * 2) + 3;
                pme = heap->slots + me;
            } else
                break;
        }
        SetRaw(&pmom[0], timetemp);
        slotCopy(&pmom[1], &tasktemp);
        SetRaw(&pmom[2], stabilityCountTemp);
        if (isPartialScanObj)
            gc->GCWriteBlack(pmom + 1);

        if (size == 0)
            SetInt(&heap->count, 0);

        // 	dumpheap(heapArg);
        // 	post("<-getheap true\n");
        return true;
    } else {
        // 	post("<-getheap false\n");
        return false;
    }
}

void offsetheap(VMGlobals* g, PyrObject* heap, double offset) {
    long i;
    for (i = 0; i < heap->size; i += 2) {
        SetRaw(&heap->slots[i], slotRawFloat(&heap->slots[i]) + offset);
        // post("%3d %9.2f %9.2f\n", i>>1, heap->slots[i].uf, offset);
    }
}

void dumpheap(PyrObject* heapArg) {
    PyrHeap* heap = (PyrHeap*)heapArg;
    double mintime = slotRawFloat(&heap->slots[0]);
    int count = slotRawFloat(&heap->slots[2]);
    int heapSize = heap->size - 1;
    post("SCHED QUEUE (%d)\n", heapSize);
    for (int i = 0; i < heapSize; i += 3) {
        post("%3d(%3d) %9.2f %p %d\n", i / 3, i, slotRawFloat(&heap->slots[i]), slotRawObject(&heap->slots[i + 1]),
             slotRawInt(&heap->slots[i + 2]));
        if ((slotRawFloat(&heap->slots[i]) < mintime)
            || (slotRawFloat(&heap->slots[i]) == mintime && slotRawInt(&heap->slots[i + 2]) < count))
            post("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
    }
}


bool gRunSched = false;
static std::thread gSchedThread;
static std::thread gResyncThread;
static boost::sync::semaphore gResyncThreadSemaphore;

std::condition_variable_any gSchedCond;
std::timed_mutex gLangMutex;

int64 gHostOSCoffset = 0;
int64 gHostStartNanos = 0;

int64 gElapsedOSCoffset = 0;

const int32 kSECONDS_FROM_1900_to_1970 = (int32)2208988800UL; /* 17 leap years */

static void syncOSCOffsetWithTimeOfDay();
void resyncThread();

// Use the highest resolution clock available for monotonic clock time
using monotonic_clock = std::conditional<std::chrono::high_resolution_clock::is_steady,
                                         std::chrono::high_resolution_clock, std::chrono::steady_clock>::type;

static std::chrono::high_resolution_clock::time_point hrTimeOfInitialization;

SCLANG_DLLEXPORT_C void schedInit() {
    using namespace std::chrono;
    hrTimeOfInitialization = high_resolution_clock::now();
#ifdef SC_ABLETON_LINK
    LinkClock::Init();
#endif

    syncOSCOffsetWithTimeOfDay();
    gResyncThread = std::thread(resyncThread);

    gHostStartNanos = duration_cast<nanoseconds>(hrTimeOfInitialization.time_since_epoch()).count();
    gElapsedOSCoffset = (int64)(gHostStartNanos * kNanosToOSC) + gHostOSCoffset;
}

SCLANG_DLLEXPORT_C void schedCleanup() {
    gResyncThreadSemaphore.post();
    gResyncThread.join();
}

double elapsedTime() { return DurToFloat(std::chrono::high_resolution_clock::now() - hrTimeOfInitialization); }

double monotonicClockTime() { return DurToFloat(monotonic_clock::now().time_since_epoch()); }

int64 ElapsedTimeToOSC(double elapsed) { return (int64)(elapsed * kSecondsToOSC) + gElapsedOSCoffset; }

double OSCToElapsedTime(int64 oscTime) { return (double)(oscTime - gElapsedOSCoffset) * kOSCtoSecs; }

void ElapsedTimeToChrono(double elapsed, std::chrono::system_clock::time_point& out_time_point) {
    int64 oscTime = ElapsedTimeToOSC(elapsed);

    int64 secs = ((oscTime >> 32) - kSECONDS_FROM_1900_to_1970);
    int32 nano_secs = (int32)((oscTime & 0xFFFFFFFF) * kOSCtoNanos);


    using namespace std::chrono;
#if 0 // TODO: check system_clock precision
	system_clock::time_point time_point = system_clock::time_point(seconds(secs) + nanoseconds(nano_secs));
#else
    int32 usecs = nano_secs / 1000;
    system_clock::time_point time_point = system_clock::time_point(seconds(secs) + microseconds(usecs));
#endif
    out_time_point = time_point;
}

int64 OSCTime() { return ElapsedTimeToOSC(elapsedTime()); }

void syncOSCOffsetWithTimeOfDay() {
    // generate a value gHostOSCoffset such that
    // (gHostOSCoffset + systemTimeInOSCunits)
    // is equal to gettimeofday time in OSCunits.
    // Then if this machine is synced via NTP, we are synced with the world.
    // more accurate way to do this??

    using namespace std::chrono;
    struct timeval tv;

    nanoseconds systemTimeBefore, systemTimeAfter;
    int64 diff, minDiff = 0x7fffFFFFffffFFFFLL;

    // take best of several tries
    const int numberOfTries = 8;
    int64 newOffset = gHostOSCoffset;
    for (int i = 0; i < numberOfTries; ++i) {
        systemTimeBefore = high_resolution_clock::now().time_since_epoch();
        gettimeofday(&tv, 0);
        systemTimeAfter = high_resolution_clock::now().time_since_epoch();

        diff = (systemTimeAfter - systemTimeBefore).count();
        if (diff < minDiff) {
            minDiff = diff;
            // assume that gettimeofday happens halfway between high_resolution_clock::now() calls
            int64 systemTimeBetween = systemTimeBefore.count() + diff / 2;
            int64 systemTimeInOSCunits = (int64)((double)systemTimeBetween * kNanosToOSC);
            int64 timeOfDayInOSCunits =
                ((int64)(tv.tv_sec + kSECONDS_FROM_1900_to_1970) << 32) + (int64)(tv.tv_usec * kMicrosToOSC);
            newOffset = timeOfDayInOSCunits - systemTimeInOSCunits;
        }
    }

    gHostOSCoffset = newOffset;
    // postfl("gHostOSCoffset %016llX\n", gHostOSCoffset);
}


void schedAdd(VMGlobals* g, PyrObject* inQueue, double inSeconds, PyrSlot* inTask);
void schedAdd(VMGlobals* g, PyrObject* inQueue, double inSeconds, PyrSlot* inTask) {
    // gLangMutex must be locked
    double prevTime = inQueue->size > 1 ? slotRawFloat(inQueue->slots + 1) : -1e10;
    bool added = addheap(g, inQueue, inSeconds, inTask);
    if (!added)
        post("scheduler queue is full.\n");
    else {
        if (isKindOfSlot(inTask, class_thread)) {
            SetFloat(&slotRawThread(inTask)->nextBeat, inSeconds);
        }
        if (slotRawFloat(inQueue->slots + 1) != prevTime) {
            gSchedCond.notify_all();
        }
    }
}

SCLANG_DLLEXPORT_C void schedStop() {
    // printf("->schedStop\n");
    gLangMutex.lock();
    if (gRunSched) {
        gRunSched = false;
        gLangMutex.unlock();
        gSchedCond.notify_all();
        gSchedThread.join();
    } else {
        gLangMutex.unlock();
    }
    // printf("<-schedStop\n");
}

void schedClearUnsafe();

SCLANG_DLLEXPORT_C void schedClear() {
    gLangMutex.lock();
    schedClearUnsafe();
    gLangMutex.unlock();
}

void schedClearUnsafe() {
    // postfl("->schedClear %d\n", gRunSched);
    if (gRunSched) {
        VMGlobals* g = gMainVMGlobals;
        PyrObject* inQueue = slotRawObject(&g->process->sysSchedulerQueue);
        inQueue->size = 1;
        gSchedCond.notify_all();
    }
    // postfl("<-schedClear %d\n", gRunSched);
}

void post(const char* fmt, ...);

void resyncThread() {
    while (true) {
        if (gResyncThreadSemaphore.wait_for(std::chrono::seconds(20)))
            return;

        syncOSCOffsetWithTimeOfDay();
        gElapsedOSCoffset = (int64)(gHostStartNanos * kNanosToOSC) + gHostOSCoffset;
    }
}

extern bool gTraceInterpreter;

static void schedRunFunc() {
    using namespace std::chrono;
    unique_lock<timed_mutex> lock(gLangMutex);

    VMGlobals* g = gMainVMGlobals;
    PyrObject* inQueue = slotRawObject(&g->process->sysSchedulerQueue);
    // dumpObject(inQueue);

    gRunSched = true;
    while (true) {
        assert(inQueue->size);

        // postfl("wait until there is something in scheduler\n");
        // wait until there is something in scheduler
        while (inQueue->size == 1) {
            // postfl("wait until there is something in scheduler\n");
            gSchedCond.wait(lock);
            if (!gRunSched)
                goto leave;
        }
        // postfl("wait until an event is ready\n");

        // wait until an event is ready
        high_resolution_clock::duration schedSecs;
        high_resolution_clock::time_point now, schedPoint;
        do {
            now = high_resolution_clock::now();
            schedSecs =
                duration_cast<high_resolution_clock::duration>(duration<double>(slotRawFloat(inQueue->slots + 1)));
            schedPoint = hrTimeOfInitialization + schedSecs;
            if (now >= schedPoint)
                break;
            // postfl("wait until an event is ready\n");
            gSchedCond.wait_until(lock, schedPoint);
            if (!gRunSched)
                goto leave;
            // postfl("time diff %g\n", elapsedTime() - inQueue->slots->uf);
        } while (inQueue->size > 1);

        // postfl("perform all events that are ready %d %.9f\n", inQueue->size, elapsed);

        // perform all events that are ready
        // postfl("perform all events that are ready\n");
        while ((inQueue->size > 1)
               && now >= hrTimeOfInitialization
                       + duration_cast<high_resolution_clock::duration>(
                             duration<double>(slotRawFloat(inQueue->slots + 1)))) {
            double schedtime, delta;
            PyrSlot task;

            // postfl("while %.6f >= %.6f\n", elapsed, inQueue->slots->uf);

            getheap(g, inQueue, &schedtime, &task);

            if (isKindOfSlot(&task, class_thread)) {
                SetNil(&slotRawThread(&task)->nextBeat);
            }

            slotCopy((++g->sp), &task);
            SetFloat(++g->sp, schedtime);
            SetFloat(++g->sp, schedtime);
            ++g->sp;
            SetObject(g->sp, s_systemclock->u.classobj);

            runAwakeMessage(g);
            long err = slotDoubleVal(&g->result, &delta);
            if (!err) {
                // add delta time and reschedule
                double time = schedtime + delta;

                schedAdd(g, inQueue, time, &task);
            }
        }
        // postfl("loop\n");
    }
    // postfl("exitloop\n");
leave:
    return;
}

#ifdef __APPLE__
#    include <mach/mach.h>
#    include <mach/thread_policy.h>

// Polls a (non-realtime) thread to find out how it is scheduled
// Results are undefined of an error is returned. Otherwise, the
// priority is returned in the address pointed to by the priority
// parameter, and whether or not the thread uses timeshare scheduling
// is returned at the address pointed to by the isTimeShare parameter
kern_return_t GetStdThreadSchedule(mach_port_t machThread, int* priority, boolean_t* isTimeshare) {
    kern_return_t result = 0;
    thread_extended_policy_data_t timeShareData;
    thread_precedence_policy_data_t precedenceData;
    mach_msg_type_number_t structItemCount;
    boolean_t fetchDefaults = false;

    memset(&timeShareData, 0, sizeof(thread_extended_policy_data_t));
    memset(&precedenceData, 0, sizeof(thread_precedence_policy_data_t));

    if (0 == machThread)
        machThread = mach_thread_self();

    if (NULL != isTimeshare) {
        structItemCount = THREAD_EXTENDED_POLICY_COUNT;
        result = thread_policy_get(machThread, THREAD_EXTENDED_POLICY, (integer_t*)&timeShareData, &structItemCount,
                                   &fetchDefaults);
        *isTimeshare = timeShareData.timeshare;
        if (0 != result)
            return result;
    }

    if (NULL != priority) {
        fetchDefaults = false;
        structItemCount = THREAD_PRECEDENCE_POLICY_COUNT;
        result = thread_policy_get(machThread, THREAD_PRECEDENCE_POLICY, (integer_t*)&precedenceData, &structItemCount,
                                   &fetchDefaults);
        *priority = precedenceData.importance;
    }

    return result;
}

// Reschedules the indicated thread according to new parameters:
//
// machThread           The mach thread id. Pass 0 for the current thread.
// newPriority          The desired priority.
// isTimeShare          false for round robin (fixed) priority,
//                      true for timeshare (normal) priority
//
// A standard new thread usually has a priority of 0 and uses the
// timeshare scheduling scheme. Use pthread_mach_thread_np() to
// to convert a pthread id to a mach thread id
kern_return_t RescheduleStdThread(mach_port_t machThread, int newPriority, boolean_t isTimeshare) {
    kern_return_t result = 0;
    thread_extended_policy_data_t timeShareData;
    thread_precedence_policy_data_t precedenceData;

    // Set up some variables that we need for the task
    precedenceData.importance = newPriority;
    timeShareData.timeshare = isTimeshare;
    if (0 == machThread)
        machThread = mach_thread_self();

    // Set the scheduling flavor. We want to do this first, since doing so
    // can alter the priority
    result =
        thread_policy_set(machThread, THREAD_EXTENDED_POLICY, (integer_t*)&timeShareData, THREAD_EXTENDED_POLICY_COUNT);

    if (0 != result)
        return result;

    // Now set the priority
    return thread_policy_set(machThread, THREAD_PRECEDENCE_POLICY, (integer_t*)&precedenceData,
                             THREAD_PRECEDENCE_POLICY_COUNT);
}
#endif // __APPLE__

#ifdef __linux__
#    include <string.h>

static void SC_LinuxSetRealtimePriority(pthread_t thread, int priority) {
    int policy;
    struct sched_param param;

    pthread_getschedparam(thread, &policy, &param);

    policy = SCHED_FIFO;
    const int minprio = sched_get_priority_min(policy);
    const int maxprio = sched_get_priority_max(policy);
    param.sched_priority = sc_clip(priority, minprio, maxprio);

    int err = pthread_setschedparam(thread, policy, &param);
    if (err != 0) {
        post("Couldn't set realtime scheduling priority %d: %s\n", param.sched_priority, strerror(err));
    }
}
#endif // __linux__


SCLANG_DLLEXPORT_C void schedRun() {
    SC_Thread thread(schedRunFunc);
    gSchedThread = std::move(thread);

#ifdef __APPLE__
    int policy;
    struct sched_param param;

    // pthread_t thread = pthread_self ();
    pthread_getschedparam(gSchedThread.native_handle(), &policy, &param);
    // post("param.sched_priority %d\n", param.sched_priority);

    policy = SCHED_RR; // round-robin, AKA real-time scheduling

    int machprio;
    boolean_t timeshare;
    GetStdThreadSchedule(pthread_mach_thread_np(gSchedThread.native_handle()), &machprio, &timeshare);
    // post("mach priority %d   timeshare %d\n", machprio, timeshare);

    // what priority should gSchedThread use?

    RescheduleStdThread(pthread_mach_thread_np(gSchedThread.native_handle()), 62, false);

    GetStdThreadSchedule(pthread_mach_thread_np(gSchedThread.native_handle()), &machprio, &timeshare);
    // post("mach priority %d   timeshare %d\n", machprio, timeshare);

    // param.sched_priority = 70; // you'll have to play with this to see what it does
    // pthread_setschedparam (gSchedThread, policy, &param);

    pthread_getschedparam(gSchedThread.native_handle(), &policy, &param);

    // post("param.sched_priority %d\n", param.sched_priority);
#endif // __APPLE__

#ifdef __linux__
    SC_LinuxSetRealtimePriority(gSchedThread.native_handle(), 1);
#endif // __linux__
}


/*

unscheduled events:
    startup,
    receive OSC,
    mouse, keyboard, MIDI

all these happen in the main thread.

*/

/*
new clock:
    create
    destroy
    wake up at time x.
    unschedule
    awake
        reschedules.
*/


TempoClock* TempoClock::sAll = 0;

void TempoClock_stopAll(void) {
    // printf("->TempoClock_stopAll %p\n", TempoClock::sAll);
    auto* clock = TempoClock::GetAll();
    while (clock) {
        auto* next = clock->GetNext();
        clock->Stop();
        // printf("delete\n");
        delete clock;
        clock = next;
    }
    // printf("<-TempoClock_stopAll %p\n", TempoClock::sAll);
    TempoClock::InitAll();
}

TempoClock::TempoClock(VMGlobals* inVMGlobals, PyrObject* inTempoClockObj, double inTempo, double inBaseBeats,
                       double inBaseSeconds):
    g(inVMGlobals),
    mTempoClockObj(inTempoClockObj),
    mTempo(inTempo),
    mBeatDur(1. / inTempo),
    mBaseSeconds(inBaseSeconds),
    mBaseBeats(inBaseBeats),
    mRun(true),
    mPrev(0),
    mNext(sAll) {
    if (sAll)
        sAll->mPrev = this;
    sAll = this;

    mQueue = (PyrHeap*)slotRawObject(&mTempoClockObj->slots[0]);
    mQueue->size = 1;
    SetInt(&mQueue->count, 0);

    SC_Thread thread(std::bind(&TempoClock::Run, this));
    mThread = std::move(thread);

#ifdef __APPLE__
    int machprio;
    boolean_t timeshare;
    GetStdThreadSchedule(pthread_mach_thread_np(mThread.native_handle()), &machprio, &timeshare);
    // post("mach priority %d   timeshare %d\n", machprio, timeshare);

    // what priority should gSchedThread use?

    RescheduleStdThread(pthread_mach_thread_np(mThread.native_handle()), 10, false);

    GetStdThreadSchedule(pthread_mach_thread_np(mThread.native_handle()), &machprio, &timeshare);
    // post("mach priority %d   timeshare %d\n", machprio, timeshare);

    // param.sched_priority = 70; // you'll have to play with this to see what it does
    // pthread_setschedparam (mThread, policy, &param);
#endif // __APPLE__

#ifdef __linux__
    SC_LinuxSetRealtimePriority(mThread.native_handle(), 1);
#endif // __linux__
}

void TempoClock::StopReq() {
    // printf("->TempoClock::StopReq\n");
    SC_Thread stopThread(std::bind(&TempoClock::StopAndDelete, this));
    stopThread.detach();

    // printf("<-TempoClock::StopReq\n");
}

void TempoClock::Stop() {
    // printf("->TempoClock::Stop\n");
    {
        lock_guard<timed_mutex> lock(gLangMutex);

        // printf("Stop mRun %d\n", mRun);
        if (mRun) {
            mRun = false;

            // unlink
            if (mPrev)
                mPrev->mNext = mNext;
            else
                sAll = mNext;
            if (mNext)
                mNext->mPrev = mPrev;

            mCondition.notify_all();
        }
    }
    mThread.join();

    // printf("<-TempoClock::Stop\n");
}

void TempoClock::SetAll(double inTempo, double inBeats, double inSeconds) {
    mBaseSeconds = inSeconds;
    mBaseBeats = inBeats;
    mTempo = inTempo;
    mBeatDur = 1. / mTempo;
    mCondition.notify_one();
}

void TempoClock::SetTempoAtBeat(double inTempo, double inBeats) {
    mBaseSeconds = BeatsToSecs(inBeats);
    mBaseBeats = inBeats;
    mTempo = inTempo;
    mBeatDur = 1. / mTempo;
    mCondition.notify_one();
}

void TempoClock::SetTempoAtTime(double inTempo, double inSeconds) {
    mBaseBeats = SecsToBeats(inSeconds);
    mBaseSeconds = inSeconds;
    mTempo = inTempo;
    mBeatDur = 1. / mTempo;
    mCondition.notify_one();
}

double TempoClock::ElapsedBeats() { return SecsToBeats(elapsedTime()); }

void* TempoClock::Run() {
    using namespace std::chrono;
    // printf("->TempoClock::Run\n");
    unique_lock<timed_mutex> lock(gLangMutex);

    while (mRun) {
        assert(mQueue->size);
        // printf("tempo %g  dur %g  beats %g\n", mTempo, mBeatDur, mBeats);
        // printf("wait until there is something in scheduler\n");
        // wait until there is something in scheduler
        while (mQueue->size == 1) {
            // printf("wait until there is something in scheduler\n");
            mCondition.wait(gLangMutex);
            // printf("mRun a %d\n", mRun);
            if (!mRun)
                goto leave;
        }
        // printf("wait until an event is ready\n");

        // wait until an event is ready
        double elapsedBeats;
        high_resolution_clock::duration schedSecs;
        high_resolution_clock::time_point schedPoint;
        do {
            elapsedBeats = ElapsedBeats();
            if (elapsedBeats >= slotRawFloat(mQueue->slots))
                break;

            // printf("event ready at %g . elapsed beats %g\n", mQueue->slots->uf, elapsedBeats);
            double wakeTime = BeatsToSecs(slotRawFloat(mQueue->slots));

            schedSecs = duration_cast<high_resolution_clock::duration>(duration<double>(wakeTime));
            schedPoint = hrTimeOfInitialization + schedSecs;

            // printf("wait until an event is ready. wake %g  now %g\n", wakeTime, elapsedTime());
            mCondition.wait_until(lock, schedPoint);
            // printf("mRun b %d\n", mRun);
            if (!mRun)
                goto leave;
            // printf("time diff %g\n", elapsedTime() - mQueue->slots->uf);
        } while (mQueue->size > 1);
        // printf("perform all events that are ready %d %.9f\n", mQueue->size, elapsedBeats);

        // perform all events that are ready
        // printf("perform all events that are ready\n");
        while (mQueue->size > 1 && elapsedBeats >= slotRawFloat(mQueue->slots)) {
            double delta;
            PyrSlot task;

            // printf("while %.6f >= %.6f\n", elapsedBeats, mQueue->slots->uf);

            getheap(g, (PyrObject*)mQueue, &mBeats, &task);

            if (isKindOfSlot(&task, class_thread)) {
                SetNil(&slotRawThread(&task)->nextBeat);
            }

            slotCopy((++g->sp), &task);
            SetFloat(++g->sp, mBeats);
            SetFloat(++g->sp, BeatsToSecs(mBeats));
            ++g->sp;
            SetObject(g->sp, mTempoClockObj);

            runAwakeMessage(g);
            long err = slotDoubleVal(&g->result, &delta);
            if (!err) {
                // add delta time and reschedule
                double beats = mBeats + delta;
                Add(beats, &task);
            }
        }
    }
leave:
    // printf("<-TempoClock::Run\n");
    return 0;
}

/*
void TempoClock::Flush()
{
    while (mQueue->size && elapsedBeats >= mQueue->slots->uf) {
        double delta;
        PyrSlot task;

        //printf("while %.6f >= %.6f\n", elapsedBeats, mQueue->slots->uf);

        getheap(g, mQueue, &mBeats, &task);

        slotCopy((++g->sp), &task);
        (++g->sp)->uf = mBeats;
        (++g->sp)->uf = BeatsToSecs(mBeats);
        ++g->sp;	SetObject(g->sp, mTempoClockObj);

        runAwakeMessage(g);
        long err = slotDoubleVal(&g->result, &delta);
        if (!err) {
            // add delta time and reschedule
            double beats = mBeats + delta;
            Add(beats, &task);
        }
    }
}
*/


void TempoClock::Add(double inBeats, PyrSlot* inTask) {
    double prevBeats = mQueue->size > 1 ? slotRawFloat(mQueue->slots) : -1e10;
    bool added = addheap(g, (PyrObject*)mQueue, inBeats, inTask);
    if (!added)
        post("scheduler queue is full.\n");
    else {
        if (isKindOfSlot(inTask, class_thread)) {
            SetFloat(&slotRawThread(inTask)->nextBeat, inBeats);
        }
        if (slotRawFloat(mQueue->slots) != prevBeats)
            mCondition.notify_one();
    }
}

void TempoClock::Clear() {
    if (mRun) {
        mQueue->size = 1;
        mCondition.notify_one();
    }
}

void TempoClock::Dump() {
    post("mTempo %g\n", mTempo);
    post("mBeatDur %g\n", mBeatDur);
    post("mBeats %g\n", mBeats);
    post("seconds %g\n", BeatsToSecs(mBeats));
    post("mBaseSeconds %g\n", mBaseSeconds);
    post("mBaseBeats %g\n", mBaseBeats);
}


int prTempoClock_Free(struct VMGlobals* g, int numArgsPushed);
int prTempoClock_Free(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* a = g->sp;
    TempoClock* clock = (TempoClock*)slotRawPtr(&slotRawObject(a)->slots[1]);
    if (!clock)
        return errNone; // not running

    SetNil(slotRawObject(a)->slots + 1);
    clock->StopReq();

    return errNone;
}

int prTempoClock_Clear(struct VMGlobals* g, int numArgsPushed);
int prTempoClock_Clear(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* a = g->sp;
    TempoClock* clock = (TempoClock*)slotRawPtr(&slotRawObject(a)->slots[1]);
    if (clock)
        clock->Clear();

    return errNone;
}

int prTempoClock_Dump(struct VMGlobals* g, int numArgsPushed);
int prTempoClock_Dump(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* a = g->sp;
    TempoClock* clock = (TempoClock*)slotRawPtr(&slotRawObject(a)->slots[1]);
    if (clock)
        clock->Dump();

    return errNone;
}


int prTempoClock_Tempo(struct VMGlobals* g, int numArgsPushed);
int prTempoClock_Tempo(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* a = g->sp;
    TempoClock* clock = (TempoClock*)slotRawPtr(&slotRawObject(a)->slots[1]);
    if (!clock) {
        error("clock is not running.\n");
        return errFailed;
    }

    SetFloat(a, clock->GetTempo());

    return errNone;
}

int prTempoClock_BeatDur(struct VMGlobals* g, int numArgsPushed);
int prTempoClock_BeatDur(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* a = g->sp;
    TempoClock* clock = (TempoClock*)slotRawPtr(&slotRawObject(a)->slots[1]);
    if (!clock) {
        error("clock is not running.\n");
        return errFailed;
    }

    SetFloat(a, clock->GetBeatDur());

    return errNone;
}

int prTempoClock_ElapsedBeats(struct VMGlobals* g, int numArgsPushed);
int prTempoClock_ElapsedBeats(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* a = g->sp;
    TempoClock* clock = (TempoClock*)slotRawPtr(&slotRawObject(a)->slots[1]);
    if (!clock) {
        error("clock is not running.\n");
        return errFailed;
    }

    SetFloat(a, clock->ElapsedBeats());

    return errNone;
}

int prTempoClock_Beats(struct VMGlobals* g, int numArgsPushed);
int prTempoClock_Beats(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* a = g->sp;
    double beats, seconds;

    if (SlotEq(&g->thread->clock, a)) {
        int err = slotDoubleVal(&g->thread->beats, &beats);
        if (err)
            return errWrongType;
    } else {
        TempoClock* clock = (TempoClock*)slotRawPtr(&slotRawObject(a)->slots[1]);
        if (!clock) {
            error("clock is not running.\n");
            return errFailed;
        }

        int err = slotDoubleVal(&g->thread->seconds, &seconds);
        if (err)
            return errWrongType;

        beats = clock->SecsToBeats(seconds);
    }
    SetFloat(a, beats);
    return errNone;
}

int prTempoClock_Sched(struct VMGlobals* g, int numArgsPushed);
int prTempoClock_Sched(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* a = g->sp - 2;
    PyrSlot* b = g->sp - 1;
    PyrSlot* c = g->sp;
    double delta, beats;
    int err;

    TempoClock* clock = (TempoClock*)slotRawPtr(&slotRawObject(a)->slots[1]);
    if (!clock) {
        error("clock is not running.\n");
        return errFailed;
    }

    if (SlotEq(&g->thread->clock, a)) {
        err = slotDoubleVal(&g->thread->beats, &beats);
        if (err)
            return errNone; // return nil OK, just don't schedule
    } else {
        double seconds;
        err = slotDoubleVal(&g->thread->seconds, &seconds);
        if (err)
            return errNone;
        beats = clock->SecsToBeats(seconds);
    }

    err = slotDoubleVal(b, &delta);
    if (err)
        return errNone; // return nil OK, just don't schedule
    beats += delta;
    if (beats == dInfinity)
        return errNone; // return nil OK, just don't schedule

    clock->Add(beats, c);

    return errNone;
}

int prTempoClock_SchedAbs(struct VMGlobals* g, int numArgsPushed);
int prTempoClock_SchedAbs(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* a = g->sp - 2;
    PyrSlot* b = g->sp - 1;
    PyrSlot* c = g->sp;

    TempoClock* clock = (TempoClock*)slotRawPtr(&slotRawObject(a)->slots[1]);
    if (!clock) {
        error("clock is not running.\n");
        return errFailed;
    }

    double beats;
    int err = slotDoubleVal(b, &beats) || (beats == dInfinity);
    if (err)
        return errNone; // return nil OK, just don't schedule

    clock->Add(beats, c);

    return errNone;
}

int prTempoClock_BeatsToSecs(struct VMGlobals* g, int numArgsPushed);
int prTempoClock_BeatsToSecs(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* a = g->sp - 1;
    PyrSlot* b = g->sp;

    TempoClock* clock = (TempoClock*)slotRawPtr(&slotRawObject(a)->slots[1]);
    if (!clock) {
        error("clock is not running.\n");
        return errFailed;
    }

    double beats;
    int err = slotDoubleVal(b, &beats);
    if (err)
        return errFailed;

    SetFloat(a, clock->BeatsToSecs(beats));

    return errNone;
}

int prTempoClock_SecsToBeats(struct VMGlobals* g, int numArgsPushed);
int prTempoClock_SecsToBeats(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* a = g->sp - 1;
    PyrSlot* b = g->sp;

    TempoClock* clock = (TempoClock*)slotRawPtr(&slotRawObject(a)->slots[1]);
    if (!clock) {
        error("clock is not running.\n");
        return errFailed;
    }

    double secs;
    int err = slotDoubleVal(b, &secs);
    if (err)
        return errFailed;

    SetFloat(a, clock->SecsToBeats(secs));

    return errNone;
}

int prSystemClock_Clear(struct VMGlobals* g, int numArgsPushed);
int prSystemClock_Clear(struct VMGlobals* g, int numArgsPushed) {
    // PyrSlot *a = g->sp;

    schedClearUnsafe();

    return errNone;
}

int prSystemClock_Sched(struct VMGlobals* g, int numArgsPushed);
int prSystemClock_Sched(struct VMGlobals* g, int numArgsPushed) {
    // PyrSlot *a = g->sp - 2;
    PyrSlot* b = g->sp - 1;
    PyrSlot* c = g->sp;

    double delta, seconds;
    int err = slotDoubleVal(b, &delta);
    if (err)
        return errNone; // return nil OK, just don't schedule
    err = slotDoubleVal(&g->thread->seconds, &seconds);
    if (err)
        return errNone; // return nil OK, just don't schedule
    seconds += delta;
    if (seconds == dInfinity)
        return errNone; // return nil OK, just don't schedule
    PyrObject* inQueue = slotRawObject(&g->process->sysSchedulerQueue);

    schedAdd(g, inQueue, seconds, c);

    return errNone;
}

int prSystemClock_SchedAbs(struct VMGlobals* g, int numArgsPushed);
int prSystemClock_SchedAbs(struct VMGlobals* g, int numArgsPushed) {
    // PyrSlot *a = g->sp - 2;
    PyrSlot* b = g->sp - 1;
    PyrSlot* c = g->sp;

    double time;
    int err = slotDoubleVal(b, &time) || (time == dInfinity);
    if (err)
        return errNone; // return nil OK, just don't schedule
    PyrObject* inQueue = slotRawObject(&g->process->sysSchedulerQueue);

    schedAdd(g, inQueue, time, c);

    return errNone;
}

int prElapsedTime(struct VMGlobals* g, int numArgsPushed) {
    SetFloat(g->sp, elapsedTime());
    return errNone;
}

int prmonotonicClockTime(struct VMGlobals* g, int numArgsPushed) {
    SetFloat(g->sp, monotonicClockTime());
    return errNone;
}

void initSchedPrimitives() {
    int base, index = 0;

    base = nextPrimitiveIndex();

    definePrimitive(base, index++, "_TempoClock_New", prClock_New<TempoClock>, 4, 0);
    definePrimitive(base, index++, "_TempoClock_Free", prTempoClock_Free, 1, 0);
    definePrimitive(base, index++, "_TempoClock_Clear", prTempoClock_Clear, 1, 0);
    definePrimitive(base, index++, "_TempoClock_Dump", prTempoClock_Dump, 1, 0);
    definePrimitive(base, index++, "_TempoClock_Sched", prTempoClock_Sched, 3, 0);
    definePrimitive(base, index++, "_TempoClock_SchedAbs", prTempoClock_SchedAbs, 3, 0);
    definePrimitive(base, index++, "_TempoClock_Tempo", prTempoClock_Tempo, 1, 0);
    definePrimitive(base, index++, "_TempoClock_BeatDur", prTempoClock_BeatDur, 1, 0);
    definePrimitive(base, index++, "_TempoClock_ElapsedBeats", prTempoClock_ElapsedBeats, 1, 0);
    definePrimitive(base, index++, "_TempoClock_Beats", prTempoClock_Beats, 1, 0);
    definePrimitive(base, index++, "_TempoClock_SetBeats", prClock_SetBeats<TempoClock>, 2, 0);
    definePrimitive(base, index++, "_TempoClock_SetTempoAtBeat", prClock_SetTempoAtBeat<TempoClock>, 3, 0);
    definePrimitive(base, index++, "_TempoClock_SetTempoAtTime", prClock_SetTempoAtTime<TempoClock>, 3, 0);
    definePrimitive(base, index++, "_TempoClock_SetAll", prClock_SetAll<TempoClock>, 4, 0);
    definePrimitive(base, index++, "_TempoClock_BeatsToSecs", prTempoClock_BeatsToSecs, 2, 0);
    definePrimitive(base, index++, "_TempoClock_SecsToBeats", prTempoClock_SecsToBeats, 2, 0);

    definePrimitive(base, index++, "_SystemClock_Clear", prSystemClock_Clear, 1, 0);
    definePrimitive(base, index++, "_SystemClock_Sched", prSystemClock_Sched, 3, 0);
    definePrimitive(base, index++, "_SystemClock_SchedAbs", prSystemClock_SchedAbs, 3, 0);

    definePrimitive(base, index++, "_ElapsedTime", prElapsedTime, 1, 0);
    definePrimitive(base, index++, "_monotonicClockTime", prmonotonicClockTime, 1, 0);
}
