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

#ifndef _PYRSCHED_H_
#define _PYRSCHED_H_

#include "VMGlobals.h"
#include "SC_Export.h"
#include <pthread.h>
#include <errno.h>

extern pthread_mutex_t gLangMutex;

SC_DLLEXPORT_C void schedInit();
SC_DLLEXPORT_C void schedCleanup();

SC_DLLEXPORT_C void schedRun();
SC_DLLEXPORT_C void schedStop();
SC_DLLEXPORT_C void schedClear();

double elapsedTime();
double elapsedRealTime();
int64 OSCTime();

int64 ElapsedTimeToOSC(double elapsed);
double OSCToElapsedTime(int64 oscTime);

void doubleToTimespec(double secs, struct timespec *spec);


bool addheap(VMGlobals *g, PyrObject *heap, double schedtime, PyrSlot *task);
bool lookheap(PyrObject *heap, double *schedtime, PyrSlot *task) ;
bool getheap(VMGlobals *g, PyrObject *heap, double *schedtime, PyrSlot *task) ;
void offsetheap(VMGlobals *g, PyrObject *heap, double offset) ;
void dumpheap(PyrObject *heap);

const double kSecondsToOSC  = 4294967296.; // pow(2,32)/1
const double kMicrosToOSC = 4294.967296; // pow(2,32)/1e6
const double kNanosToOSC  = 4.294967296; // pow(2,32)/1e9
const double kOSCtoSecs = 2.328306436538696e-10;  // 1/pow(2,32)
const double kOSCtoNanos  = 0.2328306436538696; // 1e9/pow(2,32)

// lock language,
// if shouldBeRunning == false, return EINTR
// if language has been locked, return 0
inline int lockLanguageOrQuit(bool shouldBeRunning)
{
#ifdef __linux__
	// we have pthread_mutex_timedlock

	int status = pthread_mutex_trylock(&gLangMutex);
	if (status == 0) {
		if (shouldBeRunning == false) {
			pthread_mutex_unlock(&gLangMutex);
			return EINTR;
		}
	} else if (status == EBUSY) {
		do {
			struct timespec now;
			clock_gettime(CLOCK_REALTIME, &now);
			now.tv_sec += 1;

			status = pthread_mutex_timedlock(&gLangMutex, &now);
			if (shouldBeRunning == false) {
				if (status == 0)
					pthread_mutex_unlock(&gLangMutex);
				return EINTR;
			}
		} while (status == ETIMEDOUT);
	}
	return status;
#else
	int status;
	do {
		status = pthread_mutex_trylock (&gLangMutex);
		if (shouldBeRunning == false) {
			if (status == 0)
				pthread_mutex_unlock(&gLangMutex);
			return EINTR;
		}
		if (status == EBUSY) {
			struct timespec sleepTime, remain;
			sleepTime.tv_sec = 0;
			sleepTime.tv_nsec = 100000;
			nanosleep(&sleepTime, &remain);
		}
	} while (status);
	return 0;
#endif
}

#endif
