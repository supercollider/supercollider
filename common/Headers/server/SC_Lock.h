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


#ifndef _SC_Lock_
#define _SC_Lock_

#include <pthread.h>

class SC_Lock
{
public:
	SC_Lock() { pthread_mutex_init (&mutex, NULL); }
	~SC_Lock() { pthread_mutex_destroy (&mutex); }
	void Lock() { pthread_mutex_lock (&mutex); }
	void Unlock() { pthread_mutex_unlock (&mutex); }
private:
	pthread_mutex_t mutex;
};

#endif
