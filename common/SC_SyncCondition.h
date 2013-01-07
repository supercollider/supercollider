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


#ifndef _SC_SyncCondition_
#define _SC_SyncCondition_

#include <pthread.h>

class SC_SyncCondition
{
public:
	SC_SyncCondition()
		: read(0), write(0)
	{
		// the mutex is only for pthread_cond_wait, which requires it.
		// since there is only supposed to be one signaller and one waiter
		// there is nothing to mutually exclude.
		pthread_mutex_init (&mutex, NULL);
		pthread_cond_init (&available, NULL);
	}

	~SC_SyncCondition()
	{
		pthread_mutex_destroy (&mutex);
		pthread_cond_destroy (&available);
	}

	void WaitEach()
	{
		// waits if it has caught up.
		// not very friendly, may be trying in vain to keep up.
		pthread_mutex_lock (&mutex);
		while (read == write)
			pthread_cond_wait (&available, &mutex);
		++read;
		pthread_mutex_unlock (&mutex);
	}

	void WaitOnce()
	{
		// waits if not signaled since last time.
		// if only a little late then can still go.
		int writeSnapshot;
		pthread_mutex_lock (&mutex);
		writeSnapshot = write;
		while (read == writeSnapshot)
			pthread_cond_wait (&available, &mutex);
		read = writeSnapshot;
		pthread_mutex_unlock (&mutex);
	}

	void WaitNext()
	{
		// will wait for the next signal after the read = write statement
		// this is the friendliest to other tasks, because if it is
		// late upon entry, then it has to lose a turn.
		pthread_mutex_lock (&mutex);
		read = write;
		while (read == write)
			pthread_cond_wait (&available, &mutex);
		pthread_mutex_unlock (&mutex);
	}

	void Signal()
	{
		++write;
		pthread_cond_signal (&available);
	}

private:
	pthread_cond_t available;
	pthread_mutex_t mutex;
	int read, write;
};

#endif

