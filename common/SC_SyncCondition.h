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

#pragma once

#include "SC_Lock.h"

class SC_SyncCondition {
public:
    SC_SyncCondition(): read(0), write(0) {}

    ~SC_SyncCondition() {}

    void WaitEach() {
        // waits if it has caught up.
        // not very friendly, may be trying in vain to keep up.
        unique_lock<SC_Lock> lock(mutex);
        while (read == write)
            available.wait(lock);
        ++read;
    }

    void WaitOnce() {
        // waits if not signaled since last time.
        // if only a little late then can still go.

        unique_lock<SC_Lock> lock(mutex);
        int writeSnapshot = write;
        while (read == writeSnapshot)
            available.wait(lock);
        read = writeSnapshot;
    }

    void WaitNext() {
        // will wait for the next signal after the read = write statement
        // this is the friendliest to other tasks, because if it is
        // late upon entry, then it has to lose a turn.
        unique_lock<SC_Lock> lock(mutex);
        read = write;
        while (read == write)
            available.wait(lock);
    }

    void Signal() {
        ++write;
        available.notify_one();
    }

private:
    // the mutex is only for pthread_cond_wait, which requires it.
    // since there is only supposed to be one signaller and one waiter
    // there is nothing to mutually exclude.
    condition_variable_any available;
    SC_Lock mutex;
    int read, write;
};
