//  portaudio backend
//  Copyright (C) 2006 - 2013 Tim Blechmann
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

#include <cstdio>
#include <string>
#include <algorithm>

#include "portaudio.h"
#ifdef HAVE_PORTAUDIO_CONFIG_H
#    include "portaudio/portaudio_config.h"
#endif /* HAVE_PORTAUDIO_CONFIG_H */

#include "audio_backend_common.hpp"
#include "utilities/branch_hints.hpp"
#include "cpu_time_info.hpp"
#include "SC_PaUtils.hpp"

#include "simd_binary_arithmetic.hpp"

namespace nova {

/** \brief portaudio backend for supernova
 *
 *  */
template <typename engine_functor, typename sample_type = float, bool blocking = false>
class portaudio_backend : public detail::audio_backend_base<sample_type, float, blocking, false>,
                          public detail::audio_settings_basic,
                          protected engine_functor {
    typedef detail::audio_backend_base<sample_type, float, blocking, false> super;

public:
    portaudio_backend(void) = default;

    ~portaudio_backend(void) {
        if (audio_is_active())
            deactivate_audio();

        close_stream();

        int err = Pa_Terminate();
        report_error(err);
    }

    void initialize(void) {
        int err = Pa_Initialize();
        report_error(err, true);

        list_devices();

#ifdef PA_HAVE_JACK
        PaJack_SetClientName("SuperNova");
#endif
    }

    uint32_t get_audio_blocksize(void) const { return blocksize_; }

    uint32_t get_latency(void) const { return latency_; }

private:
    // this will be clipped to the number of available hardware outs
    uint16_t m_hwOutputChannels = 0;

private:
    static void report_error(int err, bool throw_exception = false) {
        if (err < 0) {
            engine_functor::log_printf_("PortAudio error: %s\n", Pa_GetErrorText(err));
            if (throw_exception)
                throw std::runtime_error("PortAudio error");
        }
    }

public:
    static void list_devices(void) {
        int device_number = Pa_GetDeviceCount();
        if (device_number < 0)
            report_error(device_number);

        printf("Available Audio Devices:\n");
        for (int i = 0; i < device_number; i++) {
            const PaDeviceInfo* pdi = Pa_GetDeviceInfo(i);
            printf("- %s\n  (%d ins, %d outs)\n", GetPaDeviceName(i).c_str(), pdi->maxInputChannels,
                   pdi->maxOutputChannels);
        }
        printf("\n");
    }

    void report_latency() {
        const PaStreamInfo* psi = Pa_GetStreamInfo(stream);
        if (psi) {
            fprintf(stdout, "  Sample rate: %.3f\n", psi->sampleRate);
            fprintf(stdout, "  Latency (in/out): %.3f / %.3f sec\n", psi->inputLatency, psi->outputLatency);
        }
    }

    // open PA stream
    // input_device and output_device are names of requested audio devices
    // if empty, default system device(s) will be used
    // inchans and outchans are numbers of requested hardware input/output channels
    // setting inchans/outchans to 0 disables input/output respctively
    bool open_stream(std::string const& input_device, unsigned int inchans, std::string const& output_device,
                     unsigned int outchans, unsigned int samplerate, unsigned int pa_blocksize, int h_blocksize,
                     float safety_clip_threshold) {
        int input_device_index, output_device_index;

        input_device_index = GetPaDeviceFromName(input_device.c_str(), true);
        output_device_index = GetPaDeviceFromName(output_device.c_str(), false);

        // check device selection, select defaults if needed
        std::cout << std::endl;
        PaError checked =
            TryGetDefaultPaDevices(&input_device_index, &output_device_index, inchans, outchans, samplerate);
        std::cout << std::endl;

        std::cout << "Opening audio devices:" << std::endl;
        if (inchans)
            std::cout << "  In: " << GetPaDeviceName(input_device_index) << std::endl;
        if (outchans)
            std::cout << "  Out: " << GetPaDeviceName(output_device_index) << std::endl;

        // if we got an error from CheckPaDevices, stop here
        if (checked != paNoError) {
            report_error(checked);
            return false;
        }

        PaStreamParameters in_parameters, out_parameters;
        PaTime suggestedLatencyIn, suggestedLatencyOut;

        if (!samplerate) {
            if (outchans)
                samplerate = Pa_GetDeviceInfo(output_device_index)->defaultSampleRate;
            else if (inchans)
                samplerate = Pa_GetDeviceInfo(input_device_index)->defaultSampleRate;

            // use 44100 as a last resort
            if (samplerate == 0)
                samplerate = 44100;
        }

        if (h_blocksize == 0) {
            if (inchans)
                suggestedLatencyIn = Pa_GetDeviceInfo(input_device_index)->defaultHighInputLatency;
            if (outchans)
                suggestedLatencyOut = Pa_GetDeviceInfo(output_device_index)->defaultHighOutputLatency;
        } else {
            if (h_blocksize < 0) {
                if (inchans)
                    suggestedLatencyIn = Pa_GetDeviceInfo(input_device_index)->defaultLowInputLatency;
                if (outchans)
                    suggestedLatencyOut = Pa_GetDeviceInfo(output_device_index)->defaultLowOutputLatency;
            } else
                suggestedLatencyIn = suggestedLatencyOut = (double)h_blocksize / (double)samplerate;
        }

        if (inchans) {
            const PaDeviceInfo* device_info = Pa_GetDeviceInfo(input_device_index);

            inchans = std::min(inchans, (unsigned int)device_info->maxInputChannels);

            in_parameters = MakePaStreamParameters(input_device_index, inchans, suggestedLatencyIn);
        }

        if (outchans) {
            const PaDeviceInfo* device_info = Pa_GetDeviceInfo(output_device_index);

            m_hwOutputChannels = std::min(outchans, (unsigned int)device_info->maxOutputChannels);

            out_parameters = MakePaStreamParameters(output_device_index, m_hwOutputChannels, suggestedLatencyOut);
        }

        PaStreamParameters* in_stream_parameters = inchans ? &in_parameters : nullptr;
        PaStreamParameters* out_stream_parameters = outchans ? &out_parameters : nullptr;

        PaError supported = Pa_IsFormatSupported(in_stream_parameters, out_stream_parameters, samplerate);
        if (supported != paNoError) {
            report_error(supported);
            return false;
        }


        engine_initialized = false;
        blocksize_ = pa_blocksize;

        safety_clip_threshold_ = safety_clip_threshold;
        auto process_func = (safety_clip_threshold > 0 && safety_clip_threshold < INFINITY)
            ? &portaudio_backend::pa_process<true>
            : &portaudio_backend::pa_process<false>;
        PaError opened = Pa_OpenStream(&stream, in_stream_parameters, out_stream_parameters, samplerate, pa_blocksize,
                                       paNoFlag, process_func, this);

        report_error(opened);

        if (opened != paNoError)
            return false;
        else {
            const PaStreamInfo* psi = Pa_GetStreamInfo(stream);
            if (psi)
                latency_ = (uint32_t)(psi->outputLatency * psi->sampleRate);
        }

        // input_channels is clipped to the number of available audio inputs
        // output_channels is equal to the requested number of outputs (not clipped)
        input_channels = inchans;
        super::input_samples.resize(inchans);
        output_channels = outchans;
        super::output_samples.resize(m_hwOutputChannels);
        samplerate_ = samplerate;

        cpu_time_accumulator.resize(samplerate_, blocksize_, 1);

        return true;
    }

    void close_stream(void) {
        if (stream == nullptr)
            return;

        deactivate_audio();

        int err = Pa_CloseStream(stream);
        report_error(err);
        stream = nullptr;
    }

    void activate_audio() {
        assert(stream);
        int err = Pa_StartStream(stream);
        report_error(err);
    }

    bool audio_is_active(void) {
        if (stream == nullptr)
            return false;

        int is_active = Pa_IsStreamActive(stream);
        if (is_active == 1)
            return true;
        if (is_active == 0)
            return false;

        report_error(is_active);
        return false;
    }

    void deactivate_audio() {
        if (audio_is_active()) {
            PaError err = Pa_StopStream(stream);
            report_error(err);
        }
    }

    bool audiostream_ready(void) { return stream; }

    void get_cpuload(float& peak, float& average) const { cpu_time_accumulator.get(peak, average); }

private:
    template <bool IsClipping>
    int perform(const void* inputBuffer, void* outputBuffer, unsigned long frames,
                const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags) {
        if (unlikely(!engine_initialized)) {
            engine_functor::init_thread();
            engine_functor::sync_clock();
            engine_initialized = true;
        }

        if (statusFlags & (paInputOverflow | paInputUnderflow | paOutputOverflow | paOutputUnderflow))
            engine_functor::sync_clock();

        auto** inputs = static_cast<const float**>(alloca(sizeof(float*) * input_channels));
        float* const* in = static_cast<float* const*>(inputBuffer);
        for (uint16_t i = 0; i != input_channels; ++i)
            inputs[i] = in[i];

        auto** outputs = static_cast<float**>(alloca(sizeof(float*) * m_hwOutputChannels));
        float** out = static_cast<float**>(outputBuffer);
        for (uint16_t i = 0; i != m_hwOutputChannels; ++i)
            outputs[i] = out[i];

        unsigned long processed = 0;
        while (processed != frames) {
            super::fetch_inputs(inputs, blocksize_, input_channels);
            engine_functor::run_tick();
            super::deliver_outputs(outputs, blocksize_, m_hwOutputChannels);
            if (IsClipping)
                for (uint16_t i = 0; i != m_hwOutputChannels; ++i)
                    if (super::is_aligned(out[i]))
                        clip2_vec_simd(out[i], out[i], safety_clip_threshold_, blocksize_);
                    else
                        clip2_vec(out[i], out[i], safety_clip_threshold_, blocksize_);
            processed += blocksize_;
        }

        cpu_time_accumulator.update(Pa_GetStreamCpuLoad(stream) * 100.0);
        return paContinue;
    }

    template <bool IsClipping>
    static int pa_process(const void* input, void* output, unsigned long frame_count,
                          const PaStreamCallbackTimeInfo* time_info, PaStreamCallbackFlags status_flags,
                          void* user_data) {
        portaudio_backend* self = static_cast<portaudio_backend*>(user_data);
        return self->perform<IsClipping>(input, output, frame_count, time_info, status_flags);
    }

    PaStream* stream = nullptr;
    uint32_t blocksize_ = 0;
    bool engine_initialized = false;
    cpu_time_info cpu_time_accumulator;

    uint32_t latency_ = 0;
    float safety_clip_threshold_ = 0;
};

} /* namespace nova */
