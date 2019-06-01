//  audio backend helpers
//  Copyright (C) 2010-2015 Tim Blechmann
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

#include <vector>

#include <boost/mpl/if.hpp>
#include <boost/align/aligned_allocator.hpp>

#include "nova-simd/simd_memory.hpp"
#include "nova-tt/dummy_mutex.hpp"
#include "nova-tt/spin_lock.hpp"

#include "malloc_aligned.hpp"

namespace nova { namespace detail {

template <typename sample_type, typename io_sample_type, bool blocking, bool managed_memory = true>
class audio_backend_base : boost::mpl::if_c<blocking, spin_lock, dummy_mutex>::type {
    typedef typename boost::mpl::if_c<blocking, spin_lock, dummy_mutex>::type lock_t;
    typedef std::size_t size_t;

public:
    /* @{ */
    /** buffers can be directly mapped to the io regions of the host application */
    template <typename Iterator> void input_mapping(Iterator const& buffer_begin, Iterator const& buffer_end) {
        static_assert(!managed_memory, "audio_backend_common: managed_memory == true");

        size_t input_count = buffer_end - buffer_begin;

        input_samples.resize(input_count);
        std::copy(buffer_begin, buffer_end, input_samples.begin());
    }

    template <typename Iterator> void output_mapping(Iterator const& buffer_begin, Iterator const& buffer_end) {
        static_assert(!managed_memory, "audio_backend_common: managed_memory == true");

        size_t output_count = buffer_end - buffer_begin;

        output_samples.resize(output_count);
        std::copy(buffer_begin, buffer_end, output_samples.begin());
    }
    /* @} */

protected:
    void clear_inputs(size_t frames_per_tick) {
        for (uint16_t channel = 0; channel != input_samples.size(); ++channel)
            zerovec_simd(input_samples[channel].get(), frames_per_tick);
    }

    void clear_outputs(size_t frames_per_tick) {
        for (uint16_t channel = 0; channel != output_samples.size(); ++channel)
            zerovec_simd(output_samples[channel].get(), frames_per_tick);
    }

    void prepare_helper_buffers(size_t input_channels, size_t output_channels, size_t frames) {
        static_assert(managed_memory, "audio_backend_common: managed_memory == false");

        input_samples.resize(input_channels);
        output_samples.resize(output_channels);
        std::generate(input_samples.begin(), input_samples.end(), std::bind(calloc_aligned<sample_type>, frames));
        std::generate(output_samples.begin(), output_samples.end(), std::bind(calloc_aligned<sample_type>, frames));
    }

    void fetch_inputs(const float** inputs, size_t frames, int input_channels) {
        if (is_multiple_of_vectorsize(frames)) {
            for (uint16_t i = 0; i != input_channels; ++i) {
                if (is_aligned(inputs[i]))
                    nova::copyvec_simd(input_samples[i].get(), inputs[i], frames);
                else
                    nova::copyvec(input_samples[i].get(), inputs[i], frames);
                inputs[i] += frames;
            }
        } else {
            for (uint16_t i = 0; i != input_channels; ++i) {
                nova::copyvec(input_samples[i].get(), inputs[i], frames);
                inputs[i] += frames;
            }
        }
    }

    void deliver_outputs(float** outputs, size_t frames, int output_channels) {
        if (is_multiple_of_vectorsize(frames)) {
            for (uint16_t i = 0; i != output_channels; ++i) {
                if (is_aligned(outputs[i]))
                    nova::copyvec_simd(outputs[i], output_samples[i].get(), frames);
                else
                    nova::copyvec(outputs[i], output_samples[i].get(), frames);
                outputs[i] += frames;
            }
        } else {
            for (uint16_t i = 0; i != output_channels; ++i) {
                nova::copyvec(outputs[i], output_samples[i].get(), frames);
                outputs[i] += frames;
            }
        }
    }

    static bool is_aligned(const void* arg) {
        size_t mask = sizeof(vec<float>::size) * sizeof(float) * 8 - 1;
        return !((size_t)arg & mask);
    }

    static bool is_multiple_of_vectorsize(size_t count) { return !(count & (vec<float>::objects_per_cacheline - 1)); }

    std::vector<aligned_storage_ptr<sample_type, managed_memory>,
                boost::alignment::aligned_allocator<aligned_storage_ptr<sample_type, managed_memory>, 64>>
        input_samples, output_samples;
};

class audio_settings_basic {
protected:
    float samplerate_ = 0.f;
    uint16_t input_channels = 0;
    uint16_t output_channels = 0;

public:
    audio_settings_basic(void) {}

    float get_samplerate(void) const { return samplerate_; }

    uint16_t get_input_count(void) const { return input_channels; }

    uint16_t get_output_count(void) const { return output_channels; }
};

} /* namespace detail */ } /* namespace nova */
