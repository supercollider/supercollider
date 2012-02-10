//  portaudio backend
//  Copyright (C) 2006, 2007, 2008, 2012 Tim Blechmann
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

#include <cstdio>

#include "portaudio.h"
#ifdef HAVE_PORTAUDIO_CONFIG_H
#include "portaudio/portaudio_config.h"
#endif /* HAVE_PORTAUDIO_CONFIG_H */

#include "audio_backend_common.hpp"
#include "utilities/branch_hints.hpp"


namespace nova {

/** \brief portaudio backend for supernova
 *
 *  */
template <typename engine_functor,
          typename sample_type = float,
          bool blocking = false>
class portaudio_backend:
    public detail::audio_delivery_helper<sample_type, float, blocking, false>,
    public detail::audio_settings_basic,
    protected engine_functor
{
    typedef detail::audio_delivery_helper<sample_type, float, blocking, false> super;

public:
    portaudio_backend(void):
        stream(NULL), blocksize_(0)
    {
        int err = Pa_Initialize();
        report_error(err, true);


        list_devices();

#ifdef PA_HAVE_JACK
        PaJack_SetClientName("SuperNova");
#endif
    }

    ~portaudio_backend(void)
    {
        if (audio_is_active())
            deactivate_audio();

        close_stream();

        int err = Pa_Terminate();
        report_error(err);
    }

    uint32_t get_audio_blocksize(void) const
    {
        return blocksize_;
    }

private:
    static void report_error(int err, bool throw_exception = false)
    {
        if (err < 0) {
            engine_functor::log_printf_("PortAudio error: %s\n", Pa_GetErrorText( err ));
            if (throw_exception)
                throw std::runtime_error("PortAudio error");
        }
    }

public:
    static void list_devices(void)
    {
        int device_number = Pa_GetDeviceCount();
        if (device_number < 0)
            report_error(device_number);

        printf("Available Audio Devices:\n");
        for (int i = 0; i != device_number; ++i) {
            const PaDeviceInfo * device_info = Pa_GetDeviceInfo(i);
            if (device_info) {
                printf("%d: %s (%d inputs, %d outputs)\n", i, device_info->name,
                       device_info->maxInputChannels, device_info->maxOutputChannels);
            }
        }
        printf("\n");
    }

    static bool match_device (std::string const & device_name, int & r_device_index)
    {
        int device_number = Pa_GetDeviceCount();
        if (device_number < 0)
            report_error(device_number);

        for (int i = 0; i != device_number; ++i) {
            const PaDeviceInfo * device_info = Pa_GetDeviceInfo(i);
            if (std::string(device_info->name) == device_name) {
                r_device_index = i;
                return true;
            }
        }
        return false;
    }

    bool open_stream(std::string const & input_device, unsigned int inchans,
                     std::string const & output_device, unsigned int outchans,
                     unsigned int samplerate, unsigned int pa_blocksize, unsigned int blocksize)
    {
        int input_device_index, output_device_index;
        if (!match_device(input_device, input_device_index) || !match_device(output_device, output_device_index))
            return false;

        PaStreamParameters in_parameters, out_parameters;

        in_parameters.channelCount = inchans;
        in_parameters.device = input_device_index;
        in_parameters.sampleFormat = paFloat32 | paNonInterleaved;
        in_parameters.suggestedLatency = Pa_GetDeviceInfo(in_parameters.device)->defaultLowInputLatency;
        in_parameters.hostApiSpecificStreamInfo = NULL;

        out_parameters.channelCount = outchans;
        out_parameters.device = output_device_index;
        out_parameters.sampleFormat = paFloat32 | paNonInterleaved;
        out_parameters.suggestedLatency = Pa_GetDeviceInfo(out_parameters.device)->defaultLowOutputLatency;
        out_parameters.hostApiSpecificStreamInfo = NULL;

        PaError supported = Pa_IsFormatSupported(&in_parameters, &out_parameters, samplerate);
        report_error(supported);
        if (supported != 0)
            return false;

        callback_initialized = false;
        blocksize_ = blocksize;
        PaError opened = Pa_OpenStream(&stream, &in_parameters, &out_parameters,
                                       samplerate, pa_blocksize, paNoFlag,
                                       &portaudio_backend::pa_process, this);

        report_error(opened);

        if (opened != paNoError)
            return false;

        input_channels = inchans;
        super::input_samples.resize(inchans);
        output_channels = outchans;
        super::output_samples.resize(outchans);
        samplerate_ = samplerate;
        return true;
    }

    void close_stream(void)
    {
        if (stream == NULL)
            return;

        deactivate_audio();

        int err = Pa_CloseStream(stream);
        report_error(err);
        stream = NULL;
    }

    void activate_audio()
    {
        assert(stream);
        int err = Pa_StartStream(stream);
        report_error(err);
    }

    bool audio_is_active(void)
    {
        int is_active = Pa_IsStreamActive(stream);
        if (is_active == 1)
            return true;
        if (is_active == 0)
            return false;

        report_error(is_active);
        return false;
    }

    void deactivate_audio()
    {
        if (audio_is_active()) {
            PaError err = Pa_StopStream(stream);
            report_error(err);
        }
    }

    bool audiostream_ready(void)
    {
        return stream;
    }

private:
    int perform(const void *inputBuffer, void *outputBuffer, unsigned long frames,
                const PaStreamCallbackTimeInfo *timeInfo, PaStreamCallbackFlags statusFlags)
    {
        if (unlikely(!callback_initialized)) {
            engine_functor::init_thread();
            engine_functor::sync_clock();
            callback_initialized = true;
        }

        if (statusFlags & (paInputOverflow | paInputUnderflow | paOutputOverflow | paOutputUnderflow))
            engine_functor::sync_clock();

        const float * inputs[input_channels];
        float * const *in = static_cast<float * const *>(inputBuffer);
        for (uint16_t i = 0; i != input_channels; ++i)
            inputs[i] = in[i];

        float * outputs[output_channels];
        float **out = static_cast<float **>(outputBuffer);
        for (uint16_t i = 0; i != output_channels; ++i)
            outputs[i] = out[i];

        unsigned long processed = 0;
        while (processed != frames) {
            super::fetch_inputs(inputs, blocksize_, input_channels);
            engine_functor::run_tick();
            super::deliver_outputs(outputs, blocksize_, output_channels);
            processed += blocksize_;
        }

        return paContinue;
    }

    static int pa_process(const void *input, void *output, unsigned long frame_count,
                          const PaStreamCallbackTimeInfo *time_info, PaStreamCallbackFlags status_flags, void * user_data)
    {
        portaudio_backend * self = static_cast<portaudio_backend*>(user_data);
        return self->perform(input, output, frame_count, time_info, status_flags);
    }

    PaStream *stream;
    uint32_t blocksize_;
    bool callback_initialized;
};

} /* namespace nova */

#endif /* _PORTAUDIO_HPP */
