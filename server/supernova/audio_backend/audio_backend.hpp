//  audio interface
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

#ifndef _AUDIO_BACKEND_HPP
#define _AUDIO_BACKEND_HPP

#include <string>
#include <vector>

#include "utilities/utils.hpp"
#include "server/sample_types.hpp"
#include "simd_memory.hpp"

namespace nova
{
/** \brief the Device struct represents information of an audio device
 *
 *  \ingroup kernel
 */
struct Device
{
    Device(uint apiIndex, std::string const & name, uint inchannels, uint outchannels, uint defaultSampleRate,
           float LowInputLatency, float HighInputLatency, float LowOutputLatency, float HighOutputLatency):
        apiIndex(apiIndex), name(name), inchannels(inchannels), outchannels(outchannels),
        defaultSampleRate(defaultSampleRate), LowInputLatency(LowInputLatency), HighInputLatency(HighInputLatency),
        LowOutputLatency(LowOutputLatency), HighOutputLatency(HighOutputLatency)
    {}

    Device(Device const & rhs):
        apiIndex(rhs.apiIndex), name(rhs.name), inchannels(rhs.inchannels), outchannels(rhs.outchannels),
        defaultSampleRate(rhs.defaultSampleRate), LowInputLatency(rhs.LowInputLatency),
        HighInputLatency(rhs.HighInputLatency), LowOutputLatency(rhs.LowOutputLatency),
        HighOutputLatency(rhs.HighOutputLatency)
    {}

    void operator=(Device const & rhs)
    {
        apiIndex = rhs.apiIndex, name = rhs.name, inchannels = rhs.inchannels, outchannels = rhs.outchannels,
            defaultSampleRate = rhs.defaultSampleRate, LowOutputLatency = rhs.LowOutputLatency,
            LowInputLatency = rhs.LowInputLatency, HighOutputLatency = rhs.HighOutputLatency,
            HighInputLatency = rhs.HighInputLatency;
    }

    bool operator==(Device const & rhs)
    {
        return (apiIndex == rhs.apiIndex) and
            (name == rhs.name);
    }

    uint apiIndex;
    std::string name;
    unsigned short inchannels, outchannels;
    uint defaultSampleRate;
    float LowInputLatency, HighInputLatency;
    float LowOutputLatency, HighOutputLatency;
};

typedef std::vector<Device> device_list;

template <void(*dsp_cb)(void)>
class audio_frontend;

/** \brief abstract base class for audio backends
 *
 *  \ingroup kernel
 */
template <void(*dsp_cb)(void)>
class audio_backend:
    public noncopyable
{
protected:
    audio_backend(void):
        active(false)
    {}

    virtual ~audio_backend(void)
    {}

public:
    /** \brief deliver data to dac */
    /* { */
    virtual void deliver_dac_output(const_restricted_sample_ptr source, uint channel, uint frames) = 0;

    virtual void deliver_dac_output_64(const_restricted_sample_ptr source, uint channel)
    {
        this->deliver_dac_output(source, channel, 64);
    }
    /* } */

    virtual void zero_dac_output(uint channel, uint frames) = 0;

    /** \brief fetch data from adc */
    /* { */
    virtual void fetch_adc_input(restricted_sample_ptr destination, uint channel, uint frames) = 0;

    virtual void fetch_adc_input_64(restricted_sample_ptr destination, uint channel)
    {
        this->fetch_adc_input(destination, channel, 64);
    }
    /* } */

    /** \brief activate audio_backend */
    virtual void activate(void)
    {
        active = true;
    }

    /** \brief deactivate audio_backend */
    virtual void deactivate(void)
    {
        active = false;
    }

    /** \brief query activation state */
    bool is_active(void)
    {
        return active;
    }

    /** \brief get dac blocksize */
    uint get_blocksize(void)
    {
        return dacblocksize;
    }

    /** \brief list devices */
    virtual device_list list_devices(void) = 0;

    /** \brief open audio stream */
    virtual void open_audio_stream(Device const & indevice, uint inchannels, Device const & outdevice, uint outchannels,
                                   uint samplerate) = 0;

    /** \brief close the active audio stream */
    virtual void close_audio_stream(void) = 0;

    virtual bool audiostream_ready(void) = 0;

    /** \brief wrapper for Scheduler::Tick() */
    void tick(void)
    {
        (*dsp_cb)();
    }

protected:
    friend class audio_frontend<dsp_cb>;

    bool active;    /**< \brief is this interface active? */
    unsigned short dacblocksize;
};


template <void(*dsp_cb)(void)>
class dummy_backend:
    public audio_backend<dsp_cb>
{
public:
    dummy_backend(void):
        ready(false)
    {}

    void open_audio_stream(Device const & indevice, uint inchannels, Device const & outdevice, uint outchannels,
                           uint samplerate)
    {
        assert(not ready);
        ready = true;
    }

    void close_audio_stream(void)
    {
        assert(ready);
        ready = false;
    }

    bool audiostream_ready(void)
    {
        return ready;
    }

    device_list list_devices(void)
    {
        Device dummydevice(0, "Dummy", 256, 256, 44100, 0, 0, 0, 0);

        device_list ret;
        ret.push_back(dummydevice);
        return ret;
    }

    /** \brief noop */
    virtual void deliver_dac_output(const_restricted_sample_ptr source, uint channel, uint frames)
    {}

    virtual void zero_dac_output(uint channel, uint frames)
    {}

    /** \brief wipe buffer */
    virtual void fetch_adc_input(restricted_sample_ptr destination, uint channel, uint frames)
    {
        zerovec_simd(destination, audio_backend<dsp_cb>::dacblocksize);
    }

private:
    bool ready;
};


} /* namespace nova */



#endif /* _AUDIO_BACKEND_HPP */

