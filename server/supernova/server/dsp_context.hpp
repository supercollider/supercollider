//  dsp context
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

#ifndef DSP_CONTEXT_HPP
#define DSP_CONTEXT_HPP

#include <boost/cstdint.hpp>
#include <array>
#include <boost/lockfree/detail/prefix.hpp>

#include "sample_types.hpp"

namespace nova {

/** \brief struct holding thread-local resources
 *
 *  the instances are located in a global array, threads can access their corresponding
 *  resources by using the thread id as array index
 * */
struct BOOST_LOCKFREE_CACHELINE_ALIGNMENT thread_local_resource
{
};

static const int max_threads = 2;
extern std::array<thread_local_resource, max_threads> thread_local_resources;

/** \brief dsp context
 *  */
class dsp_context
{
    typedef boost::uint_fast8_t uint8;
    typedef boost::uint_fast16_t uint16;

public:
    /* don't initialize anything ... only for supercollider wrapper */
    dsp_context(void)
    {}

    dsp_context(sample sr, uint16 blocksize, uint8 thread_index):
        blocksize_(blocksize), samplerate_(sr), inv_samplerate_(sample(1) / sr), thread_index_(thread_index)
    {}

    sample samplerate(void) const
    {
        return samplerate_;
    }

    sample inv_samplerate(void) const
    {
        return inv_samplerate_;
    }

    sample ms2smps(sample ms) const
    {
        return ms * sample(0.001) * samplerate_;
    }

    sample smps2ms(sample smps)
    {
        return smps * inv_samplerate_ * 1000;
    }

    uint8 thread_index(void) const
    {
        return thread_index_;
    }

    uint16 blocksize(void) const
    {
        return blocksize_;
    }

private:
    uint16 blocksize_;
    sample samplerate_, inv_samplerate_;
    uint8 thread_index_;
};

} /* namespace nova */

#endif /* DSP_CONTEXT_HPP */
