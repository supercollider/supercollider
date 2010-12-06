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


#include "SC_Sem.h"

SC_Semaphore::SC_Semaphore(int initialCount)
{
	pthread_mutex_init (&mutex, NULL);
	pthread_cond_init (&available, NULL);
	count = initialCount;
}

SC_Semaphore::~SC_Semaphore()
{
	pthread_mutex_destroy (&mutex);
	pthread_cond_destroy (&available);
}

void SC_Semaphore::Acquire()
{
	pthread_mutex_lock (&mutex);
	while (count < 1) {
		pthread_cond_wait (&available, &mutex);
	}
	count--;
	pthread_mutex_unlock (&mutex);

}

void SC_Semaphore::Release()
{
	pthread_mutex_lock (&mutex);
	if (++count > 0) {
		pthread_cond_signal (&available);
	}
	pthread_mutex_unlock (&mutex);
}

