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
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/


#include "PyrKernel.h"
#include "PyrSched.h"
#include "GC.h"
#include "PyrPrimitive.h"
#ifdef SC_DARWIN
# include <CoreAudio/HostTime.h>
#endif
#include <stdarg.h>
#include <stdlib.h>
#include <math.h>

#define SANITYCHECK 0

void addheap(VMGlobals *g, PyrObject *heap, double schedtime, PyrSlot *task) 
{
	short mom;	/* parent and sibling in the heap, not in the task hierarchy */
	PyrSlot *pme, *pmom;
	
#if SANITYCHECK
	gcSanity(g->gc);
#endif
	//dumpheap(heap);
	//post("->addheap\n");
	mom = heap->size;
	pme = heap->slots + mom;
	for (; mom>0;) {	/* percolate up heap */
		mom = (mom - 2 >> 1) & ~1;
		pmom = heap->slots + mom;
		if (schedtime < pmom->uf) {
			pme[0].ucopy = pmom[0].ucopy;
			pme[1].ucopy = pmom[1].ucopy;
			pme = pmom;
		} else break;
	}
	pme[0].uf = schedtime;
	pme[1].ucopy = task->ucopy;
	g->gc->GCWrite(heap, task);
	heap->size += 2;

#if SANITYCHECK
	gcSanity(g->gc);
#endif
	//dumpheap(heap);
	//post("<-addheap %g\n", schedtime);
}


bool lookheap(PyrObject *heap, double *schedtime, PyrSlot *task) 
{
	if (heap->size) {
		*schedtime = heap->slots[0].uf;
		task->ucopy = heap->slots[1].ucopy;
		return true;
	} else return false;
}


bool getheap(PyrObject *heap, double *schedtime, PyrSlot *task) 
{
	PyrSlot *pmom, *pme, *pend;
	short mom,me,size;	/* parent and sibling in the heap, not in the task hierarchy */
	double tasktemp;
	double timetemp;

	//dumpheap(heap);
	//post("->getheap\n");
	if (heap->size>0) {
		*schedtime = heap->slots[0].uf;
		task->ucopy = heap->slots[1].ucopy;
		size = heap->size -= 2;
		heap->slots[0].ucopy = heap->slots[size].ucopy;
		heap->slots[1].ucopy = heap->slots[size+1].ucopy;
		mom = 0;
		me = 2;
		pmom = heap->slots + mom;
		pme = heap->slots + me;
		pend = heap->slots + size;
		timetemp = pmom[0].uf;
		tasktemp = pmom[1].uf;
		for (;pme < pend;) { /* demote heap */
			if (pme+2 < pend && pme[0].uf > pme[2].uf) {
				me += 2; pme += 2; 
			}
			if (timetemp > pme[0].uf) {
				pmom[0].ucopy = pme[0].ucopy;
				pmom[1].ucopy = pme[1].ucopy;
				pmom = pme;
				me = ((mom = me) << 1) + 2;
				pme = heap->slots + me;
			} else break;
		}
		pmom[0].uf = timetemp;
		pmom[1].uf = tasktemp;
		
		//dumpheap(heap);
	//dumpheap(heap);
	//post("<-getheap true\n");
		return true;
	} else {
	//post("<-getheap false\n");
		return false;
	}
}

void offsetheap(VMGlobals *g, PyrObject *heap, double offset) 
{
	long i;
	for (i=0; i<heap->size; i+=2) {
		heap->slots[i].uf += offset;
		//post("%3d %9.2f %9.2f\n", i>>1, heap->slots[i].uf, offset);
	}
}

void dumpheap(PyrObject *heap)
{
	long i;
	double mintime;
	mintime = heap->slots[0].uf;
	post("SCHED QUEUE (%d)\n", heap->size);
	for (i=0; i<heap->size; i+=2) {
		post("%3d %9.2f %08X\n", i>>1, heap->slots[i].uf, heap->slots[i+1].ui);
		if (heap->slots[i].uf < mintime) 
			post("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
	}
}

void addheap4(VMGlobals *g, PyrObject *heap, double schedtime, PyrSlot *task, 
	PyrSlot *numRepeats, PyrSlot *maxRepeats) 
{
	short mom;	/* parent and sibling in the heap, not in the task hierarchy */
	PyrSlot *pme, *pmom;
	
#if SANITYCHECK
	gcSanity(g->gc);
#endif
	//dumpheap4(heap);
	//post("->addheap\n");
	mom = heap->size;
	pme = heap->slots + mom;
	for (; mom>0;) {	/* percolate up heap */
		mom = (mom - 4 >> 1) & ~3;
		pmom = heap->slots + mom;
		if (schedtime < pmom->uf) {
			pme[0].ucopy = pmom[0].ucopy;
			pme[1].ucopy = pmom[1].ucopy;
			pme[2].ucopy = pmom[2].ucopy;
			pme[3].ucopy = pmom[3].ucopy;
			pme = pmom;
		} else break;
	}
	pme[0].uf = schedtime;
	pme[1].ucopy = task->ucopy;
	pme[2].ucopy = numRepeats->ucopy;
	pme[3].ucopy = maxRepeats->ucopy;
	g->gc->GCWrite(heap, task);
	heap->size += 4;

#if SANITYCHECK
	gcSanity(g->gc);
#endif
	//dumpheap4(heap);
	//post("<-addheap %g\n", schedtime);
}


bool lookheap4(PyrObject *heap, double *schedtime, PyrSlot *task,
	PyrSlot *numRepeats, PyrSlot *maxRepeats) 
{
	if (heap->size) {
		*schedtime = heap->slots[0].uf;
		task->ucopy = heap->slots[1].ucopy;
		numRepeats->ucopy = heap->slots[2].ucopy;
		maxRepeats->ucopy = heap->slots[3].ucopy;
		return true;
	} else return false;
}


bool getheap4(PyrObject *heap, double *schedtime, PyrSlot *task,
	PyrSlot *numRepeats, PyrSlot *maxRepeats) 
{
	PyrSlot *pmom, *pme, *pend;
	short mom,me,size;	/* parent and sibling in the heap, not in the task hierarchy */
	double tasktemp;
	double timetemp;
	double counttemp;
	double maxcounttemp;

	if (heap->size>0) {
		//dumpheap4(heap);
		//post("->getheap\n");

		*schedtime = heap->slots[0].uf;
		task->ucopy = heap->slots[1].ucopy;
		numRepeats->ucopy = heap->slots[2].ucopy;
		maxRepeats->ucopy = heap->slots[3].ucopy;
		size = heap->size -= 4;
		heap->slots[0].ucopy = heap->slots[size].ucopy;
		heap->slots[1].ucopy = heap->slots[size+1].ucopy;
		heap->slots[2].ucopy = heap->slots[size+2].ucopy;
		heap->slots[3].ucopy = heap->slots[size+3].ucopy;
		mom = 0;
		me = 4;
		pmom = heap->slots + mom;
		pme = heap->slots + me;
		pend = heap->slots + size;
		timetemp = pmom[0].uf;
		tasktemp = pmom[1].uf;
		counttemp = pmom[2].uf;
		maxcounttemp = pmom[3].uf;
		for (;pme < pend;) { /* demote heap */
			if (pme+4 < pend && pme[0].uf > pme[4].uf) {
				me += 4; pme += 4; 
			}
			if (timetemp > pme[0].uf) {
				pmom[0].ucopy = pme[0].ucopy;
				pmom[1].ucopy = pme[1].ucopy;
				pmom[2].ucopy = pme[2].ucopy;
				pmom[3].ucopy = pme[3].ucopy;
				pmom = pme;
				me = ((mom = me) << 1) + 4;
				pme = heap->slots + me;
			} else break;
		}
		pmom[0].uf = timetemp;
		pmom[1].uf = tasktemp;
		pmom[2].uf = counttemp;
		pmom[3].uf = maxcounttemp;
		
		//dumpheap4(heap);
		//post("<-getheap true\n");

		return true;
	} else {
		//post("<-getheap false\n");
		return false;
	}
}

void dumpheap4(PyrObject *heap)
{
	long i, j;
	double mintime;
	mintime = heap->slots[0].uf;
	post("SCHED QUEUE (%d)\n", heap->size);
	for (i=0, j=0; i<heap->size; i+=4,j++) {
		post("%3d %9.2f %08X %d %d\n", j, heap->slots[i].uf, heap->slots[i+1].ui,
			heap->slots[i+2].ui, heap->slots[i+3].ui);
		if (i > 0) {
			post("** %d %d %g %g\n", (i - 4 >> 1) & ~3, i, heap->slots[(i - 4 >> 1) & ~3].uf, heap->slots[i].uf);
			if (heap->slots[(i - 4 >> 1) & ~3].uf > heap->slots[i].uf) {
				post("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
			}
		}
	}
}



bool gRunSched = false;
pthread_t gSchedThread;
pthread_t gResyncThread;
pthread_cond_t gSchedCond; 
pthread_mutex_t gLangMutex;

#ifdef SC_DARWIN
int64 gHostOSCoffset = 0; 
int64 gHostStartNanos = 0;
#endif

int64 gElapsedOSCoffset = 0;

const int32 kSECONDS_FROM_1900_to_1970 = (int32)2208988800UL; /* 17 leap years */
const double fSECONDS_FROM_1900_to_1970 = 2208988800.; /* 17 leap years */

#ifdef SC_DARWIN
void syncOSCOffsetWithTimeOfDay();
void* resyncThread(void* arg);
#else // !SC_DARWIN
#include <sys/time.h>

inline double GetTimeOfDay();
double GetTimeOfDay()
{
	struct timeval tv;
	gettimeofday(&tv, 0);
	return (double)tv.tv_sec + 1.0e-6 * (double)tv.tv_usec;
}
#endif // SC_DARWIN

void schedInit();
void schedInit()
{
	pthread_cond_init (&gSchedCond, NULL);
	pthread_mutex_init (&gLangMutex, NULL);

#ifdef SC_DARWIN
	syncOSCOffsetWithTimeOfDay();
	pthread_create (&gResyncThread, NULL, resyncThread, (void*)0);

	gHostStartNanos = AudioConvertHostTimeToNanos(AudioGetCurrentHostTime());
	gElapsedOSCoffset = (int64)(gHostStartNanos * kNanosToOSC) + gHostOSCoffset;
#else
	gElapsedOSCoffset = (int64)kSECONDS_FROM_1900_to_1970 << 32;
#endif
}

void schedCleanup();
void schedCleanup()
{
	pthread_mutex_destroy (&gLangMutex);
	pthread_cond_destroy (&gSchedCond);
}

double bootSeconds();
double bootSeconds()
{
#ifdef SC_DARWIN
	return 1e-9 * (double)AudioConvertHostTimeToNanos(AudioGetCurrentHostTime());
#else
	return GetTimeOfDay();
#endif
}

double elapsedTime();
double elapsedTime()
{
#ifdef SC_DARWIN
	return 1e-9 * (double)(AudioConvertHostTimeToNanos(AudioGetCurrentHostTime()) - gHostStartNanos);
#else
	return GetTimeOfDay();
#endif
}

int64 ElapsedTimeToOSC(double elapsed)
{
	return (int64)(elapsed * kSecondsToOSC) + gElapsedOSCoffset;
}

double OSCToElapsedTime(int64 oscTime)
{
        return (double)(oscTime - gElapsedOSCoffset) * kOSCtoSecs;
}

void ElapsedTimeToTimespec(double elapsed, struct timespec *spec);
void ElapsedTimeToTimespec(double elapsed, struct timespec *spec)
{
	int64 oscTime = ElapsedTimeToOSC(elapsed);

	spec->tv_sec = (time_t)((oscTime >> 32) - kSECONDS_FROM_1900_to_1970);
	spec->tv_nsec = (int32_t)((oscTime & 0xFFFFFFFF) * kOSCtoNanos);
}

int64 OSCTime()
{
	return ElapsedTimeToOSC(elapsedTime());
}

#ifdef SC_DARWIN
void syncOSCOffsetWithTimeOfDay()
{
	// generate a value gHostOSCoffset such that  
	// (gHostOSCoffset + systemTimeInOSCunits) 
	// is equal to gettimeofday time in OSCunits.
	// Then if this machine is synced via NTP, we are synced with the world.
	// more accurate way to do this??

	struct timeval tv;	
	
	int64 systemTimeBefore, systemTimeAfter, diff;
	int64 minDiff = 0x7fffFFFFffffFFFFLL;
	
	// take best of several tries
	const int numberOfTries = 8;
	int64 newOffset = gHostOSCoffset;
	for (int i=0; i<numberOfTries; ++i) {
		systemTimeBefore = AudioGetCurrentHostTime();
		gettimeofday(&tv, 0);
		systemTimeAfter = AudioGetCurrentHostTime();
		
		diff = systemTimeAfter - systemTimeBefore;
		if (diff < minDiff) {
			minDiff = diff;
			// assume that gettimeofday happens halfway between AudioGetCurrentHostTime calls
			int64 systemTimeBetween = systemTimeBefore + diff/2;
			int64 systemTimeInOSCunits = (int64)((double)AudioConvertHostTimeToNanos(systemTimeBetween) * kNanosToOSC); 
			int64 timeOfDayInOSCunits  = ((int64)(tv.tv_sec + kSECONDS_FROM_1900_to_1970) << 32) 
                                            + (int64)(tv.tv_usec * kMicrosToOSC);
			newOffset = timeOfDayInOSCunits - systemTimeInOSCunits;
		}
	}
	
	gHostOSCoffset = newOffset;
	//postfl("gHostOSCoffset %016llX\n", gHostOSCoffset);
}
#endif

void schedAdd(VMGlobals *g, PyrObject* inQueue, double inTime, PyrSlot* inTask);
void schedAdd(VMGlobals *g, PyrObject* inQueue, double inTime, PyrSlot* inTask)
{
	// gLangMutex must be locked
	double prevTime = inQueue->size ? inQueue->slots->uf : -1e10;
	addheap(g, inQueue, inTime, inTask);
	if (inQueue->size && inQueue->slots->uf != prevTime) {
		//post("pthread_cond_signal\n");
		pthread_cond_signal (&gSchedCond);
	}
}


void doubleToTimespec(double secs, struct timespec *spec);
void doubleToTimespec(double secs, struct timespec *spec)
{
	double isecs = floor(secs);
	spec->tv_sec = (long)isecs;
	spec->tv_nsec = (long)floor(1000000000. * (secs - isecs));
}

void runInterpreter(VMGlobals *g, PyrSymbol *selector, int numArgsPushed);

void schedStop();
void schedStop()
{
	//printf("->schedStop\n");
	pthread_mutex_lock (&gLangMutex);
	if (gRunSched) {
		gRunSched = false;
		pthread_cond_signal (&gSchedCond);
		pthread_mutex_unlock (&gLangMutex);
		pthread_join(gSchedThread, 0);
	} else {
		pthread_mutex_unlock (&gLangMutex);
	}
	//printf("<-schedStop\n");
}

void schedClearUnsafe();

void schedClear()
{
	pthread_mutex_lock (&gLangMutex);
	schedClearUnsafe();
	pthread_mutex_unlock (&gLangMutex);
}

void schedClearUnsafe()
{
	//postfl("->schedClear %d\n", gRunSched);
	if (gRunSched) {
		VMGlobals *g = gMainVMGlobals;
		PyrObject* inQueue = g->process->sysSchedulerQueue.uo;
		inQueue->size = 0;
		pthread_cond_signal (&gSchedCond);
		//pthread_mutex_unlock (&gLangMutex);
	}
	//postfl("<-schedClear %d\n", gRunSched);
}

void post(const char *fmt, ...);

#ifdef SC_DARWIN
void* resyncThread(void* arg)
{
	while (true) {
		sleep(20);
		syncOSCOffsetWithTimeOfDay();
		gElapsedOSCoffset = (int64)(gHostStartNanos * kNanosToOSC) + gHostOSCoffset;
	}
}
#endif

extern bool gTraceInterpreter;

void* schedRunFunc(void* arg);
void* schedRunFunc(void* arg)
{
	pthread_mutex_lock (&gLangMutex);
	
	
	VMGlobals *g = gMainVMGlobals;
	PyrObject* inQueue = g->process->sysSchedulerQueue.uo;
	//dumpObject(inQueue);
		
	gRunSched = true;
	while (true) {
		//postfl("wait until there is something in scheduler\n");
		// wait until there is something in scheduler
		while (inQueue->size == 0) {
			//postfl("wait until there is something in scheduler\n");
			pthread_cond_wait (&gSchedCond, &gLangMutex);
			if (!gRunSched) goto leave;
		}
		//postfl("wait until an event is ready\n");
		
		// wait until an event is ready
		double elapsed;
		while (inQueue->size > 0) {
			elapsed = elapsedTime();
			if (elapsed >= inQueue->slots->uf) break;
			struct timespec abstime;
			//doubleToTimespec(inQueue->slots->uf, &abstime);
			ElapsedTimeToTimespec(inQueue->slots->uf, &abstime);
			//postfl("wait until an event is ready\n");
			pthread_cond_timedwait (&gSchedCond, &gLangMutex, &abstime);
			if (!gRunSched) goto leave;
			//postfl("time diff %g\n", elapsedTime() - inQueue->slots->uf);
		}
		
		//postfl("perform all events that are ready %d %.9f\n", inQueue->size, elapsed);
		
		// perform all events that are ready
		//postfl("perform all events that are ready\n");
		while (inQueue->size && elapsed >= inQueue->slots->uf) {
			double schedtime, delta;
			PyrSlot task;
			
			//postfl("while %.6f >= %.6f\n", elapsed, inQueue->slots->uf);
						
			getheap(inQueue, &schedtime, &task);

			(++g->sp)->ucopy = task.ucopy;
			(++g->sp)->uf = schedtime;
			
			runInterpreter(g, s_awake, 2);
			long err = slotDoubleVal(&g->result, &delta);
			if (!err) {
				// add delta time and reschedule
				double time = schedtime + delta;
				schedAdd(g, inQueue, time, &task);
			}
		}
		//postfl("loop\n");
	}
	//postfl("exitloop\n");
leave:
	pthread_mutex_unlock (&gLangMutex);
	return 0;
}

#ifdef SC_DARWIN
#include <mach/mach.h>
#include <mach/thread_policy.h>

//Polls a (non-realtime) thread to find out how it is scheduled
//Results are undefined of an error is returned. Otherwise, the
//priority is returned in the address pointed to by the priority
//parameter, and whether or not the thread uses timeshare scheduling
//is returned at the address pointed to by the isTimeShare parameter
kern_return_t  GetStdThreadSchedule( mach_port_t    machThread,
                                    int            *priority,
                                    boolean_t      *isTimeshare )
{
    kern_return_t                       result = 0;
    thread_extended_policy_data_t       timeShareData;
    thread_precedence_policy_data_t     precidenceData;
    mach_msg_type_number_t		structItemCount;
    boolean_t				fetchDefaults = false;

    memset( &timeShareData, 0, sizeof( thread_extended_policy_data_t ));
    memset( &precidenceData, 0, sizeof( thread_precedence_policy_data_t ));

    if( 0 == machThread )
        machThread = mach_thread_self();

    if( NULL != isTimeshare )
    {
        structItemCount = THREAD_EXTENDED_POLICY_COUNT;
        result = thread_policy_get( machThread, THREAD_EXTENDED_POLICY,
                                    (integer_t*)&timeShareData, &structItemCount, &fetchDefaults );
        *isTimeshare = timeShareData.timeshare;
        if( 0 != result )
            return result;
    }

    if( NULL != priority )
    {
        fetchDefaults = false;
        structItemCount = THREAD_PRECEDENCE_POLICY_COUNT;
        result = thread_policy_get( machThread, THREAD_PRECEDENCE_POLICY,
                                    (integer_t*)&precidenceData, &structItemCount, &fetchDefaults );
        *priority = precidenceData.importance;
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
kern_return_t  RescheduleStdThread( mach_port_t    machThread,
                                    int            newPriority,
                                    boolean_t      isTimeshare )
{
    kern_return_t                       result = 0;
    thread_extended_policy_data_t       timeShareData;
    thread_precedence_policy_data_t     precidenceData;

    //Set up some variables that we need for the task
    precidenceData.importance = newPriority;
    timeShareData.timeshare = isTimeshare;
    if( 0 == machThread )
        machThread = mach_thread_self();

    //Set the scheduling flavor. We want to do this first, since doing so
    //can alter the priority
    result = thread_policy_set( machThread,
                                THREAD_EXTENDED_POLICY,
                                (integer_t*)&timeShareData,
                                THREAD_EXTENDED_POLICY_COUNT );

    if( 0 != result )
        return result;

    //Now set the priority
    return   thread_policy_set( machThread,
                                THREAD_PRECEDENCE_POLICY,
                                (integer_t*)&precidenceData,
                                THREAD_PRECEDENCE_POLICY_COUNT );

}
#endif // SC_DARWIN

void schedRun();
void schedRun()
{
	pthread_create (&gSchedThread, NULL, schedRunFunc, (void*)0);

        int policy;
        struct sched_param param;
        
        //pthread_t thread = pthread_self ();
        pthread_getschedparam (gSchedThread, &policy, &param);
        
        policy = SCHED_RR;         // round-robin, AKA real-time scheduling
        post("param.sched_priority %d\n", param.sched_priority);

#ifdef SC_DARWIN
        int machprio;
        boolean_t timeshare;
        GetStdThreadSchedule(pthread_mach_thread_np(gSchedThread), &machprio, &timeshare);
        post("mach priority %d   timeshare %d\n", machprio, timeshare);
        
        // what priority should gSchedThread use?
        
        RescheduleStdThread(pthread_mach_thread_np(gSchedThread), 10, false);

        GetStdThreadSchedule(pthread_mach_thread_np(gSchedThread), &machprio, &timeshare);
        post("mach priority %d   timeshare %d\n", machprio, timeshare);

        //param.sched_priority = 70; // you'll have to play with this to see what it does
        //pthread_setschedparam (gSchedThread, policy, &param);
#endif // SC_DARWIN

        pthread_getschedparam (gSchedThread, &policy, &param);
        
        post("param.sched_priority %d\n", param.sched_priority);
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


#if 1

class Clock
{
public:
	Clock(VMGlobals *inVMGlobals, PyrObject* inClockObj, double inBaseTime, double inBaseBeats);
	~Clock();
	void* Run();
	
	void Add(double inBeats, PyrSlot* inTask);
	void SetTempo(double inTempo);
	void SetTempoAtBeat(double inBeats, double inTempo);
	void SetTempoAtTime(double inTime, double inTempo);
	double ElapsedBeats();
	void Clear();
	
protected:
	VMGlobals* g;
	PyrObject* mClockObj;
	PyrObject* mQueue;
	
	double mTempo; // beats per second
	double mBeatDur; // 1/tempo
	double mBeats; // beats
	double mBaseTime;
	double mBaseBeats;
	bool mRun;
	pthread_t mThread;
	pthread_cond_t mCondition; 
	
};

void* Clock_run_func(void* p)
{
	Clock* clock = (Clock*)p;
	return clock->Run();
}

Clock::Clock(VMGlobals *inVMGlobals, PyrObject* inClockObj, double inBaseTime, double inBaseBeats)
	: g(inVMGlobals), mClockObj(inClockObj), 
	mBaseTime(inBaseTime), mBaseBeats(inBaseBeats), mRun(true)
{
	mQueue = mClockObj->slots[0].uo;
	pthread_cond_init (&mCondition, NULL);
	pthread_create (&mThread, NULL, Clock_run_func, (void*)this);
}

Clock::~Clock()
{
	pthread_mutex_lock (&gLangMutex);
	if (mRun) {
		mRun = false;
		pthread_cond_signal (&mCondition);
		pthread_mutex_unlock (&gLangMutex);
		pthread_join(mThread, 0);
	} else {
		pthread_mutex_unlock (&gLangMutex);
	}
	pthread_cond_destroy (&mCondition);
}

void Clock::SetTempo(double inTempo)
{
	SetTempoAtBeat(mBeats, inTempo);
}

void Clock::SetTempoAtBeat(double inBeats, double inTempo)
{
	mBaseTime += mBeatDur * (inBeats - mBaseBeats);
	mBaseBeats = inBeats;
	mTempo = inTempo;
	mBeatDur = 1. / mTempo;
	pthread_cond_signal (&mCondition);
}

void Clock::SetTempoAtTime(double inTime, double inTempo)
{
	mBaseBeats += mTempo * (inTime - mBaseTime);
	mBaseTime = inTime;
	mTempo = inTempo;
	mBeatDur = 1. / mTempo;
	pthread_cond_signal (&mCondition);
}

double Clock::ElapsedBeats()
{
	double elapsed = elapsedTime();
	return mBaseBeats + mTempo * (elapsed - mBaseTime);
}

void* Clock::Run()
{
	while (true) {
		//postfl("wait until there is something in scheduler\n");
		// wait until there is something in scheduler
		while (mQueue->size == 0) {
			//postfl("wait until there is something in scheduler\n");
			pthread_cond_wait (&mCondition, &gLangMutex);
			if (!mRun) goto leave;
		}
		//postfl("wait until an event is ready\n");
		
		// wait until an event is ready
		double elapsedBeats;
		while (mQueue->size > 0) {
			elapsedBeats = ElapsedBeats();
			if (elapsedBeats >= mQueue->slots->uf) break;
			struct timespec abstime;
			//doubleToTimespec(mQueue->slots->uf, &abstime);
			double wakeTime = mBaseTime + mBeatDur * (mQueue->slots->uf - mBaseBeats);
			ElapsedTimeToTimespec(wakeTime, &abstime);
			//postfl("wait until an event is ready\n");
			pthread_cond_timedwait (&mCondition, &gLangMutex, &abstime);
			if (!mRun) goto leave;
			//postfl("time diff %g\n", elapsedTime() - mQueue->slots->uf);
		}
		//postfl("perform all events that are ready %d %.9f\n", mQueue->size, elapsed);
		
		// perform all events that are ready
		//postfl("perform all events that are ready\n");
		while (mQueue->size && elapsedBeats >= mQueue->slots->uf) {
			double delta;
			PyrSlot task;
			
			//postfl("while %.6f >= %.6f\n", elapsed, inQueue->slots->uf);
						
			getheap(mQueue, &mBeats, &task);

			(++g->sp)->ucopy = task.ucopy;
			(++g->sp)->uf = mBeats;
			
			runInterpreter(g, s_awake, 2);
			long err = slotDoubleVal(&g->result, &delta);
			if (!err) {
				// add delta time and reschedule
				double time = mBeats + delta;
				schedAdd(g, mQueue, time, &task);
			}
		}
	}
leave:
	pthread_mutex_unlock (&gLangMutex);
	return 0;
}


void Clock::Add(double inBeats, PyrSlot* inTask)
{
	double prevBeats = mQueue->size ? mQueue->slots->uf : -1e10;
	addheap(g, mQueue, inBeats, inTask);
	if (mQueue->size && mQueue->slots->uf != prevBeats) {
		pthread_cond_signal (&mCondition);
	}
}

void Clock::Clear()
{
	if (mRun) {
		mQueue->size = 0;
		pthread_cond_signal (&mCondition);
	}
}

#endif
