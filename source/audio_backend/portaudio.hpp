//  portaudio backend
//  Copyright (C) 2006, 2007, 2008 Tim Blechmann
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

#ifndef _PORTAUDIO_HPP
#define _PORTAUDIO_HPP

#include "audio_backend.hpp"

#include "portaudiocpp/PortAudioCpp.hxx"

#ifdef HAVE_PORTAUDIO_CONFIG_H
#include "portaudio/portaudio_config.h"
#endif /* HAVE_PORTAUDIO_CONFIG_H */

namespace nova
{
/** \brief class containing the static portaudio data
 *
 *  \ingroup kernel
 *  */
template <typename engine_functor>
class portaudio_backend:
    public audio_backend<dsp_cb>,
    private portaudio::AutoSystem,
    private engine_functor
{
    typedef nova::audio_backend<dsp_cb> audio_backend;

public:
    portaudio_backend(void)
    {
#ifdef PA_HAVE_JACK
        PaJack_SetClientName("Nova");
#endif
    }

    ~portaudio_backend(void)
    {
        free_buffers();
    }

    device_list list_devices(void)
    {
        device_list devs;

        portaudio::System & sys = portaudio::System::instance();

        for (portaudio::System::DeviceIterator it = sys.devicesBegin(); it != sys.devicesEnd(); ++it)
        {
            portaudio::Device & dev = *it;

            devs.push_back(Device(dev.index(), dev.name(), dev.maxInputChannels(), dev.maxOutputChannels(),
                                  uint(dev.defaultSampleRate()), dev.defaultLowInputLatency(), dev.defaultHighInputLatency(),
                                  dev.defaultLowOutputLatency(), dev.defaultHighOutputLatency()));
        }
        return devs;
    }


    void open_audio_stream(Device const & indevice, uint inchans, Device const & outdevice, uint outchans,
                                         uint samplerate)
    {
        assert(!stream.get());

        inchannels = inchans;
        outchannels = outchans;

        portaudio::System & sys = portaudio::System::instance();

        assert(inchans <= indevice.inchannels);
        portaudio::DirectionSpecificStreamParameters inParams(sys.deviceByIndex(indevice.apiIndex), inchannels, portaudio::FLOAT32,
                                                              false, sys.deviceByIndex(indevice.apiIndex).defaultLowInputLatency(), NULL);

        assert(outchans <= outdevice.outchannels);
        portaudio::DirectionSpecificStreamParameters outParams(sys.deviceByIndex(outdevice.apiIndex), outchannels, portaudio::FLOAT32,
                                                               false, sys.deviceByIndex(outdevice.apiIndex).defaultLowOutputLatency(), NULL);

        portaudio::StreamParameters params(inParams, outParams, samplerate, 64, paClipOff);

        params.isSupported();

        stream.reset(new portaudio::MemFunCallbackStream<portaudio_backend>(params, *this, &portaudio_backend::perform));
        allocate_buffers(inchannels, outchannels, 64);

#if 0
//#ifdef HAVE_PORTAUDIO_ALSA
        assert(not stream->isOpen());

        if (sys.deviceByIndex(indevice.apiIndex).hostApi().index() == paALSA or
            sys.deviceByIndex(outdevice.apiIndex).hostApi().index() == paALSA)
            PaAlsa_EnableRealtimeScheduling(stream->paStream(), 1);
#endif
    }

    void close_audio_stream(void)
    {
        if(stream->isActive())
            deactivate();

        assert(!stream->isActive());

        stream->close();
        stream.reset();
    }

    void activate()
    {
        assert(stream);
        audio_backend::activate();
        stream->start();
    }

    void deactivate()
    {
        assert(stream);
        stream->stop();
        audio_backend::deactivate();
    }

    bool audiostream_ready(void)
    {
        return stream;
    }

    /** \brief deliver data to dac */
    /* @{ */
    virtual void deliver_dac_output(const_restricted_sample_ptr source, uint channel, uint frames)
    {
        if (likely(channel < out_samples.size()))
            addvec_simd(out_samples[channel], source, audio_backend::dacblocksize);
    }

    virtual void deliver_dac_output_64(const_restricted_sample_ptr source, uint channel)
    {
        if (likely(channel < out_samples.size()))
            addvec_simd<64>(out_samples[channel], source);
    }

    /* @} */

    virtual void zero_dac_output(uint channel, uint frames)
    {
        if (likely(channel < out_samples.size()))
            zerovec_simd(out_samples[channel], audio_backend::dacblocksize);
    }

    /** \brief fetch data from adc */
    /* @{ */
    virtual void fetch_adc_input(restricted_sample_ptr destination, uint channel, uint frames)
    {
        if (likely(channel < in_samples.size()))
            copyvec_simd(destination, in_samples[channel], audio_backend::dacblocksize);
        else
            zerovec_simd(destination, audio_backend::dacblocksize);
    }

    virtual void fetch_adc_input_64(restricted_sample_ptr destination, uint channel)
    {
        if (likely(channel < in_samples.size()))
            copyvec_simd_mp<64>(destination, in_samples[channel]);
        else
            zerovec_simd_mp<64>(destination);
    }
    /* @} */

private:
    int perform(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer,
                const PaStreamCallbackTimeInfo *timeInfo, PaStreamCallbackFlags statusFlags)
    {
        engine_functor::init_tick();
        float * const *in = static_cast<float * const *>(inputBuffer);
        for (uint i = 0; i != inchannels; ++i)
        {
            sample * hostbuffer = in[i];
            sample * inbuffer = in_samples[i];
            copyvec_simd_mp<64>(inbuffer, hostbuffer);
        }

        engine_functor::run_tick();

        float **out = static_cast<float **>(outputBuffer);
        for (uint i = 0; i != outchannels; ++i)
        {
            sample * hostbuffer = out[i];
            sample * outbuffer = out_samples[i];
            copyvec_simd_mp<64>(hostbuffer, outbuffer);
            zerovec_simd_mp<64>(outbuffer);
        }
        return paContinue;
    }

    /** \brief allocate io buffers */
    void allocate_buffers(uint inchannels, uint outchannels, uint framesPerBuffer)
    {
        free_buffers();

        audio_backend::dacblocksize = framesPerBuffer;
        in_samples.resize(inchannels);
        out_samples.resize(outchannels);

        for (uint i = 0; i != inchannels; ++i)
            in_samples[i] = calloc_aligned<sample>(audio_backend::dacblocksize);

        for (uint i = 0; i != outchannels; ++i)
            out_samples[i] = calloc_aligned<sample>(audio_backend::dacblocksize);
    }

    /** \brief free io buffers */
    void free_buffers(void)
    {
        for (uint i = 0; i != in_samples.size(); ++i)
            free_aligned(in_samples[i]);

        for (uint i = 0; i != out_samples.size(); ++i)
            free_aligned(out_samples[i]);
    }

    std::valarray<sample*> in_samples;
    std::valarray<sample*> out_samples;

    typedef portaudio::MemFunCallbackStream<portaudio_backend> PaCbStream;
    scoped_ptr<PaCbStream> stream;
    unsigned short inchannels, outchannels;
};

} /* namespace nova */

#endif /* _PORTAUDIO_HPP */
