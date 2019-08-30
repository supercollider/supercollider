/*
    SuperCollider real time audio synthesis system
    Copyright (c) 2002 James McCartney. All rights reserved.
    Copyright (c) 2013 Tim Blechmann
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

#include "../../common/SC_Lock.h" // for chrono
#include "SC_Types.h"

const int32 kSECONDS_FROM_1900_to_1970 = (int32)2208988800UL; /* 17 leap years */
const double kOSCtoSecs = 2.328306436538696e-10;

const double kSecondsToOSCunits = 4294967296.; // pow(2,32)
const double kMicrosToOSCunits = 4294.967296; // pow(2,32)/1e6
const double kNanosToOSCunits = 4.294967296; // pow(2,32)/1e9

typedef std::chrono::system_clock::time_point HostTime;

static inline std::chrono::system_clock::time_point getTime() { return std::chrono::system_clock::now(); }

template <typename TimePoint> static inline double secondsSinceEpoch(TimePoint const& tp) {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(tp.time_since_epoch()).count() * 1e-9;
}

template <typename TimePoint> static inline int64 OSCTime(TimePoint const& tp) {
    using namespace std::chrono;
    typedef typename TimePoint::duration Duration;
    Duration sinceEpoch = tp.time_since_epoch();
    seconds secs = duration_cast<seconds>(sinceEpoch);

    nanoseconds nsecs = sinceEpoch - secs;

    return ((int64)(secs.count() + kSECONDS_FROM_1900_to_1970) << 32) + (int64)(nsecs.count() * kNanosToOSCunits);
}

static inline int32 timeSeed() {
    using namespace std::chrono;
    static int32 count = 0;

    typedef high_resolution_clock::time_point TimePoint;

    typedef TimePoint::duration Duration;

    TimePoint now = high_resolution_clock::now();
    Duration sinceEpoch = now.time_since_epoch();
    seconds secs = duration_cast<seconds>(sinceEpoch);

    nanoseconds nsecs = sinceEpoch - secs;

    return (int32)secs.count() ^ (int32)nsecs.count() ^ count--;
}
