//  audio frontend
//  Copyright (C) 2005, 2006, 2007, 2008 Tim Blechmann
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

#ifndef _AUDIO_FRONTEND_HPP
#define _AUDIO_FRONTEND_HPP

#include <vector>

#include "audio_backend.hpp"
#include "portaudio.hpp"
#include "nova-tt/spin_lock.hpp"
#include <stdint.h>

namespace nova
{

/** \brief wrapper/dispatcher for different audio backends */
template <void(*dsp_cb)(void)>
class audio_frontend
{
    typedef nova::dummy_backend<dsp_cb> dummy_backend;

public:
    audio_frontend():
        is_active(false)
    {
        activate_dummy();
    }

    ~audio_frontend(void)
    {
        assert(not audio_is_opened());
        deactivate_dummy();
    }

    /* to be called from the audio callback */
    /* @{  */
    void deliver_dac_output(const_restricted_sample_ptr source, uint channel, uint frames)
    {
        if (likely(audio_is_active()))
        {
            spin_lock::scoped_lock lock(output_lock);
            active_backend->deliver_dac_output(source, channel, frames);
        }
    }

    void zero_dac_output(uint channel, uint frames)
    {
        if (likely(audio_is_active()))
        {
            spin_lock::scoped_lock lock(output_lock);
            active_backend->zero_dac_output(channel, frames);
        }
    }

    void deliver_dac_output_64(const_restricted_sample_ptr source, uint channel)
    {
        if (likely(audio_is_active()))
        {
            spin_lock::scoped_lock lock(output_lock);
            active_backend->deliver_dac_output_64(source, channel);
        }
    }

    void fetch_adc_input(restricted_sample_ptr destination, uint channel, uint frames)
    {
        if (likely(audio_is_active()))
            active_backend->fetch_adc_input(destination, channel, frames);
        else
            zerovec_simd(destination, frames);
    }

    void fetch_adc_input_64(restricted_sample_ptr destination, uint channel)
    {
        if (likely(audio_is_active()))
            active_backend->fetch_adc_input_64(destination, channel);
        else
            zerovec_simd_mp<64>(destination);
    }
    /* @} */

    uint get_audio_blocksize(void)
    {
        assert(audio_is_active());
        return active_backend->get_blocksize();
    }

public:
    std::vector<Device> list_devices(void)
    {
        return active_backend->list_devices();
    }

    void open_audio_stream(Device const & indevice, uint inchannels, Device const & outdevice, uint outchannels,
                         uint samplerate)
    {
        input_channels = inchannels,
        output_channels = outchannels;
        active_backend->open_audio_stream(indevice, inchannels, outdevice, outchannels, samplerate);
        samplerate_ = samplerate;
    }

    void close_audio_stream(void)
    {
        active_backend->close_audio_stream();
        input_channels = output_channels = 0;
    }

    bool audio_is_opened(void)
    {
        return active_backend != &dummy;
    }

    bool audio_is_ready(void)
    {
        return active_backend->audiostream_ready();
    }

    bool audio_is_active(void)
    {
        assert(active_backend->is_active() == is_active);
        return is_active;
    }

    bool open_portaudio_backend(void)
    {
        assert(!audio_is_opened());

#ifdef HAVE_PORTAUDIO
        deactivate_dummy();
        active_backend = new portaudio_backend<dsp_cb>();
        return true;
#else
        return false;
#endif
    }

    void close_audio_backend(void)
    {
        assert(audio_is_opened());

        if (active_backend->is_active())
            active_backend->deactivate();

        delete active_backend;
        activate_dummy();
    }

    void activate_audio(void)
    {
        is_active = true;
        active_backend->activate();
    }

    void deactivate_audio(void)
    {
        active_backend->deactivate();
        is_active = false;
    }

    float get_samplerate(void) const
    {
        return samplerate_;
    }

    uint16_t get_input_count(void) const
    {
        return input_channels;
    }

    uint16_t get_output_count(void) const
    {
        return output_channels;
    }

private:
    void activate_dummy(void)
    {
        active_backend = &dummy;

        device_list devs = dummy.list_devices();
        dummy.open_audio_stream(devs[0], devs[0].inchannels,
                                          devs[0], devs[0].outchannels,
                                          devs[0].defaultSampleRate);
        dummy.activate();
        is_active = true;
    }

    void deactivate_dummy(void)
    {
        assert(!audio_is_opened());
        dummy.deactivate();
        dummy.close_audio_stream();
        is_active = false;
    }

    bool is_active;
    float samplerate_;
    uint16_t input_channels, output_channels;

    audio_backend<dsp_cb> * active_backend;

    dummy_backend dummy;  /* the dummy backend */
    spin_lock output_lock;
};

} /* namespace nova */


#endif /* _AUDIO_FRONTEND_HPP */
