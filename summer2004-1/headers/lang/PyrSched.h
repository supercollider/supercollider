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

#ifndef _PYRSCHED_H_
#define _PYRSCHED_H_

#include "VMGlobals.h"
#include <pthread.h>

extern pthread_mutex_t gLangMutex;

void schedInit();
void schedCleanup();

void schedRun();
void schedStop();
void schedClear();

double elapsedTime();
int64 OSCTime();

int64 ElapsedTimeToOSC(double elapsed);
double OSCToElapsedTime(int64 oscTime);

void syncOSCOffsetWithTimeOfDay();
void doubleToTimespec(double secs, struct timespec *spec);


bool addheap(VMGlobals *g, PyrObject *heap, double schedtime, PyrSlot *task);
bool lookheap(PyrObject *heap, double *schedtime, PyrSlot *task) ;
bool getheap(PyrObject *heap, double *schedtime, PyrSlot *task) ;
void offsetheap(VMGlobals *g, PyrObject *heap, double offset) ;
void dumpheap(PyrObject *heap);

const double kSecondsToOSC  = 4294967296.; // pow(2,32)/1
const double kMicrosToOSC = 4294.967296; // pow(2,32)/1e6
const double kNanosToOSC  = 4.294967296; // pow(2,32)/1e9
const double kOSCtoSecs = 2.328306436538696e-10;  // 1/pow(2,32)
const double kOSCtoNanos  = 0.2328306436538696; // 1e9/pow(2,32)

#endif