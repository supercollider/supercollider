//  server scheduler
//  Copyright (C) 2008 Tim Blechmann
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; see the file COPYING.  If not, write to
//  the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
//  Boston, MA 02111-1307, USA.

#pragma once

#include <cstdint>
#include <cmath>

#include "boost/date_time/posix_time/posix_time_types.hpp"
#include "boost/date_time/gregorian/gregorian_types.hpp"

namespace nova {

/** time_tag class, compatible to osc time tags */
class time_tag {
    typedef std::uint32_t uint32;
    typedef std::uint64_t uint64;

    typedef union {
        uint64 packed;
        uint32 unpacked[2];
    } cast_union;

    static const uint32 ntp_offset = 2208988800UL; /* seconds between 01-01-1900 and 01-01-1970 */

public:
    static const uint64 fraction_steps = uint64(1) << 32;

    time_tag(void): data_(0) {}

    time_tag(time_tag const& rhs): data_(rhs.data_) {}

    time_tag(uint64 timetag): data_(timetag) {}

    time_tag(uint32 secs, uint32 fraction) {
        cast_union cu;
        cu.unpacked[1] = secs;
        cu.unpacked[0] = fraction;

        data_ = cu.packed;
    }

    uint32 get_secs(void) const { return reinterpret_cast<const cast_union*>(&data_)->unpacked[1]; }

    uint32 get_fraction(void) const { return reinterpret_cast<const cast_union*>(&data_)->unpacked[0]; }

    double get_nanoseconds(void) const { return get_fractional_seconds() * 1e9; }

    double get_fractional_seconds(void) const { return get_fraction() / double(fraction_steps); }

    bool operator<(time_tag const& rhs) const { return data_ < rhs.data_; }

    bool operator<=(time_tag const& rhs) const { return data_ <= rhs.data_; }

    bool operator==(time_tag const& rhs) const { return data_ == rhs.data_; }

    bool operator!=(time_tag const& rhs) const { return !operator==(rhs); }

    time_tag& operator+=(time_tag const& rhs) {
        data_ += rhs.data_;
        return *this;
    }

    time_tag operator+(time_tag const& rhs) const {
        time_tag ret(*this);
        ret += rhs;
        return ret;
    }

    time_tag& operator-=(time_tag const& rhs) {
        data_ -= rhs.data_;
        return *this;
    }

    time_tag operator-(time_tag const& rhs) const {
        time_tag ret(*this);
        ret -= rhs;
        return ret;
    }

    template <typename float_t> static time_tag from_ns(float_t ns) {
        const float_t units_per_ns = float_t(fraction_steps) / 1e9;

        if (ns < 1e9)
            return time_tag(0, ns * units_per_ns);
        else {
            float_t secs = std::floor(ns / 1e9);
            ns = std::fmod(ns, 1e9);
            return time_tag(secs, ns * units_per_ns);
        }
    }

    static time_tag from_samples_small(unsigned int sample_count, float samplerate) {
        assert(sample_count < samplerate);

        const float units_per_sample = float(fraction_steps) / samplerate;
        return time_tag(0, sample_count * units_per_sample);
    }

    static time_tag from_samples(unsigned int sample_count, float samplerate) {
        const float_t units_per_sample = float_t(fraction_steps) / samplerate;

        if (sample_count < samplerate)
            return from_samples_small(sample_count, samplerate);
        else {
            float_t secs = std::floor(sample_count / samplerate);
            float_t samples = std::fmod(sample_count, samplerate);
            return time_tag(secs, samples * units_per_sample);
        }
    }

    float to_samples(float samplerate) {
        float seconds = get_fractional_seconds();
        uint32_t secs = get_secs();

        if (secs == 0)
            seconds += (float)secs;
        return seconds * samplerate;
    }

    double to_seconds() {
        double seconds = get_fractional_seconds();
        uint32_t secs = get_secs();

        if (secs == 0)
            seconds += (double)secs;
        return seconds;
    }

    bool is_immediate() { return data_ == 1; }

    static time_tag from_ptime(boost::posix_time::ptime const& pt) {
        using namespace boost::gregorian;
        using namespace boost::posix_time;

        const date base(1970, Jan, 1);

        time_duration diff = pt - ptime(base);

        uint32 secs = diff.total_seconds() + ntp_offset;

        double fraction = double(diff.fractional_seconds());

        if (diff.resolution() == boost::date_time::nano)
            fraction /= 1000000000;
        else if (diff.resolution() == boost::date_time::micro)
            fraction /= 1000000;
        else if (diff.resolution() == boost::date_time::milli)
            fraction /= 1000;
        else
            assert(false);

        assert(fraction < 1.0 && fraction >= 0);

        uint32 fraction_units = std::floor(fraction * double(std::numeric_limits<uint32>::max()));
        return time_tag(secs, fraction_units);
    }

    boost::posix_time::ptime to_ptime(void) const {
        using namespace boost::gregorian;
        using namespace boost::posix_time;

        const date base(1970, Jan, 1);
#ifdef BOOST_DATE_TIME_POSIX_TIME_STD_CONFIG
        time_duration offset = seconds(get_secs() - ntp_offset) + nanoseconds(get_nanoseconds());
#else
        time_duration offset =
            seconds(get_secs() - ntp_offset) + microseconds(static_cast<long>(get_nanoseconds() / 1000));
#endif
        return ptime(base, offset);
    }

private:
    uint64 data_;
};

} /* namespace nova */
