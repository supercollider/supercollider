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
    portaudio_backend(void) {
        int err = Pa_Initialize();
        report_error(err, true);

        list_devices();

#ifdef PA_HAVE_JACK
        PaJack_SetClientName("SuperNova");
#endif
    }

    ~portaudio_backend(void) {
        if (audio_is_active())
            deactivate_audio();

        close_stream();

        int err = Pa_Terminate();
        report_error(err);
    }

    uint32_t get_audio_blocksize(void) const { return blocksize_; }

    uint32_t get_latency(void) const { return latency_; }

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
        for (int i = 0; i != device_number; ++i) {
            const PaDeviceInfo* device_info = Pa_GetDeviceInfo(i);
            if (device_info) {
#ifndef __APPLE__
                // list APIs on Windows and Linux
                const PaHostApiInfo* api_info = Pa_GetHostApiInfo(device_info->hostApi);
                printf("%d: %s : %s (%d inputs, %d outputs)\n", i, api_info->name, device_info->name,
                       device_info->maxInputChannels, device_info->maxOutputChannels);
#else
                printf("%d: %s (%d inputs, %d outputs)\n", i, device_info->name, device_info->maxInputChannels,
                       device_info->maxOutputChannels);
#endif
            }
        }
        printf("\n");
    }

    PaDeviceIndex match_device(std::string const& device) {
        if (device.empty())
            return paNoDevice;

        int device_number = Pa_GetDeviceCount();
        if (device_number < 0) {
            report_error(device_number);
            return paNoDevice;
        }

        for (int i = 0; i != device_number; ++i) {
            if (device_name(i) == device) {
                return i;
            }
        }
        return paNoDevice;
    }

    void report_latency() {
        const PaStreamInfo* psi = Pa_GetStreamInfo(stream);
        if (psi) {
            fprintf(stdout, "  Sample rate: %.3f\n", psi->sampleRate);
            fprintf(stdout, "  Latency (in/out): %.3f / %.3f sec\n", psi->inputLatency, psi->outputLatency);
        }
    }

    // this function will select default PA devices if they are not defined
    // it will also try to check for some configuration problems
    // numIns, numOuts and sampleRate are only the requested values, can change later
    PaError CheckPaDevices(int* inDevice, int* outDevice, int numIns, int numOuts, double sampleRate) {
        // make independent checks whether we are using only input, only output, or both input and outputTouched
        if (numIns && !numOuts) {
            // inputs only
            *outDevice = paNoDevice;
#ifdef _WIN32
            // in case we don't have a proper device, let's try to open one on ASIO
            if (*inDevice == paNoDevice) {
                *inDevice = Pa_GetHostApiInfo(Pa_HostApiTypeIdToHostApiIndex(paASIO))->defaultInputDevice;
                if (*inDevice != paNoDevice)
                    fprintf(stdout, "Selected default ASIO input device\n");
            }
#endif
            // check for requested sample rate
            if (*inDevice != paNoDevice) {
                if (sampleRate) {
                    // check if device can support requested SR
                    PaSampleFormat fmt = paFloat32 | paNonInterleaved;
                    PaStreamParameters parameters;
                    parameters.device = *inDevice;
                    parameters.sampleFormat = fmt;
                    parameters.hostApiSpecificStreamInfo = NULL;
                    parameters.channelCount = Pa_GetDeviceInfo(*outDevice)->maxOutputChannels;
                    PaError err = Pa_IsFormatSupported(&parameters, nullptr, sampleRate);
                    if (err != paNoError) {
                        fprintf(stdout,
                                "PortAudio error: %s\nRequested sample rate %f for device %s is not supported\n",
                                Pa_GetErrorText(err), sampleRate, Pa_GetDeviceInfo(*outDevice)->name);
                    }
                }
            }
            // in case we still don't have a proper device, use the default device
            if (*inDevice == paNoDevice) {
                *inDevice = Pa_GetDefaultInputDevice();
                if (*inDevice != paNoDevice)
                    fprintf(stdout, "Selected default system input device\n");
            }
        } else if (!numIns && numOuts) {
            // outputs only
            *inDevice = paNoDevice;
#ifdef _WIN32
            // in case we don't have a proper device, let's try to open one on ASIO
            if (*outDevice == paNoDevice) {
                *outDevice = Pa_GetHostApiInfo(Pa_HostApiTypeIdToHostApiIndex(paASIO))->defaultOutputDevice;
                if (*outDevice != paNoDevice)
                    fprintf(stdout, "Selected default ASIO output device\n");
            }
#endif
            // check for requested sample rate
            if (*outDevice != paNoDevice) {
                if (sampleRate) {
                    // check if device can support requested SR
                    PaSampleFormat fmt = paFloat32 | paNonInterleaved;
                    PaStreamParameters parameters;
                    parameters.device = *outDevice;
                    parameters.sampleFormat = fmt;
                    parameters.hostApiSpecificStreamInfo = NULL;
                    parameters.channelCount = Pa_GetDeviceInfo(*outDevice)->maxOutputChannels;
                    PaError err =
                        Pa_IsFormatSupported(nullptr, &parameters, Pa_GetDeviceInfo(*outDevice)->defaultSampleRate);
                    if (err != paNoError) {
                        fprintf(stdout,
                                "PortAudio error: %s\nRequested sample rate %f for device %s is not supported\n",
                                Pa_GetErrorText(err), sampleRate, Pa_GetDeviceInfo(*inDevice)->name);
                    }
                }
            }

            // in case we still don't have a proper device, use the default device
            if (*outDevice == paNoDevice) {
                *outDevice = Pa_GetDefaultOutputDevice();
                if (*outDevice != paNoDevice)
                    fprintf(stdout, "Selected default system output device\n");
            }
        } else if (numIns && numOuts) {
            // inputs and outputs
            // if one device is specified, let's try to open another one on matching api
            if (*inDevice == paNoDevice && *outDevice != paNoDevice) {
                *inDevice = Pa_GetHostApiInfo(Pa_GetDeviceInfo(*outDevice)->hostApi)->defaultInputDevice;
                if (*inDevice != paNoDevice)
                    fprintf(stdout, "Selected default %s input device\n",
                            Pa_GetHostApiInfo(Pa_GetDeviceInfo(*inDevice)->hostApi)->name);
            }
            if (*inDevice != paNoDevice && *outDevice == paNoDevice) {
                *outDevice = Pa_GetHostApiInfo(Pa_GetDeviceInfo(*inDevice)->hostApi)->defaultOutputDevice;
                if (*outDevice != paNoDevice)
                    fprintf(stdout, "Selected default %s output device\n",
                            Pa_GetHostApiInfo(Pa_GetDeviceInfo(*outDevice)->hostApi)->name);
            }
#ifdef _WIN32
            // check if devices are having mismatched API, but only if they are defined
            if (*inDevice != paNoDevice && *outDevice != paNoDevice) {
                if (Pa_GetDeviceInfo(*inDevice)->hostApi != Pa_GetDeviceInfo(*outDevice)->hostApi) {
                    fprintf(
                        stdout, "Requested devices %s and %s use different API. Setting output device to %s : %s.\n",
                        Pa_GetDeviceInfo(*inDevice)->name, Pa_GetDeviceInfo(*outDevice)->name,
                        Pa_GetHostApiInfo(Pa_GetDeviceInfo(*inDevice)->hostApi)->name,
                        Pa_GetDeviceInfo(Pa_GetHostApiInfo(Pa_GetDeviceInfo(*inDevice)->hostApi)->defaultOutputDevice)
                            ->name);
                    *outDevice = Pa_GetHostApiInfo(Pa_GetDeviceInfo(*inDevice)->hostApi)->defaultOutputDevice;
                }
            }
            // in case we don't have a proper device, let's try to open one on ASIO
            if (*inDevice == paNoDevice || *outDevice == paNoDevice) {
                *inDevice = Pa_GetHostApiInfo(Pa_HostApiTypeIdToHostApiIndex(paASIO))->defaultInputDevice;
                *outDevice = Pa_GetHostApiInfo(Pa_HostApiTypeIdToHostApiIndex(paASIO))->defaultOutputDevice;
                if (*inDevice != paNoDevice && *outDevice != paNoDevice)
                    fprintf(stdout, "Selected default ASIO input/output devices\n");
            }
#endif
            // check for matching sampleRate or requested sample rate
            if (*inDevice != paNoDevice && *outDevice != paNoDevice) {
                // these parameters are taken from the DriverSetup, here used for checking only
                PaSampleFormat fmt = paFloat32 | paNonInterleaved;
                PaStreamParameters in_parameters, out_parameters;
                in_parameters.device = *inDevice;
                in_parameters.sampleFormat = fmt;
                in_parameters.hostApiSpecificStreamInfo = NULL;
                in_parameters.channelCount = Pa_GetDeviceInfo(*inDevice)->maxInputChannels;
                out_parameters.device = *outDevice;
                out_parameters.sampleFormat = fmt;
                out_parameters.hostApiSpecificStreamInfo = NULL;
                out_parameters.channelCount = Pa_GetDeviceInfo(*outDevice)->maxOutputChannels;
                if (sampleRate) {
                    // check if devices can support requested SR
                    PaError err = Pa_IsFormatSupported(&in_parameters, &out_parameters, sampleRate);
                    if (err != paNoError) {
                        fprintf(stdout, "\nRequested sample rate %f for devices %s and %s is not supported.\n",
                                sampleRate, Pa_GetDeviceInfo(*inDevice)->name, Pa_GetDeviceInfo(*outDevice)->name);
                        return err;
                    }
                } else {
                    // if we don't request SR, check if devices have maching SR
                    uint32 inSR = uint32(Pa_GetDeviceInfo(*inDevice)->defaultSampleRate);
                    uint32 outSR = uint32(Pa_GetDeviceInfo(*outDevice)->defaultSampleRate);
                    if (inSR != outSR) {
                        // if defaults are different, check if both devices can be opened using the OUTPUT's SR
                        PaError err = Pa_IsFormatSupported(&in_parameters, &out_parameters,
                                                           Pa_GetDeviceInfo(*outDevice)->defaultSampleRate);
                        if (err != paNoError) {
                            fprintf(stdout,
                                    "\nRequested devices %s and %s use different sample rates. "
                                    "Please set matching sample rates "
                                    "in the Windows Sound Control Panel and try again.\n",
                                    Pa_GetDeviceInfo(*inDevice)->name, Pa_GetDeviceInfo(*outDevice)->name);
                            return err;
                        }
                    }
                }
            }

            // in case we still don't have a proper device, use the default device
            if (*inDevice == paNoDevice || *outDevice == paNoDevice) {
                *inDevice = Pa_GetDefaultInputDevice();
                *outDevice = Pa_GetDefaultOutputDevice();
                if (*inDevice != paNoDevice && *outDevice != paNoDevice)
                    fprintf(stdout, "Selected default system input/output devices\n");
            }
        } else {
            // no inputs nor outputs
            *inDevice = paNoDevice;
            *outDevice = paNoDevice;
        }
        return paNoError;
    }

    bool open_stream(std::string& input_device, unsigned int inchans, std::string& output_device, unsigned int outchans,
                     unsigned int samplerate, unsigned int pa_blocksize, int h_blocksize) {
        int input_device_index, output_device_index;

        input_device_index = match_device(input_device);
        output_device_index = match_device(output_device);

        // check device selection, select defaults if needed
        fprintf(stdout, "\n");
        PaError checked = CheckPaDevices(&input_device_index, &output_device_index, inchans, outchans, samplerate);
        fprintf(stdout, "\n");

        // if we got an error from CheckPaDevices, stop here
        if (checked != paNoError) {
            report_error(checked);
            return false;
        }

        // set device names
        if (inchans)
            input_device.assign(device_name(input_device_index));
        if (outchans)
            output_device.assign(device_name(output_device_index));

        PaStreamParameters in_parameters, out_parameters;
        PaTime suggestedLatencyIn, suggestedLatencyOut;

        if (!samplerate) {
            if (inchans)
                samplerate = Pa_GetDeviceInfo(input_device_index)->defaultSampleRate;
            if (outchans)
                samplerate = Pa_GetDeviceInfo(output_device_index)->defaultSampleRate;
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

            in_parameters.device = input_device_index;
            in_parameters.channelCount = inchans;
            in_parameters.sampleFormat = paFloat32 | paNonInterleaved;
            in_parameters.suggestedLatency = suggestedLatencyIn;
            in_parameters.hostApiSpecificStreamInfo = nullptr;
        }

        if (outchans) {
            const PaDeviceInfo* device_info = Pa_GetDeviceInfo(output_device_index);

            outchans = std::min(outchans, (unsigned int)device_info->maxOutputChannels);

            out_parameters.device = output_device_index;
            out_parameters.channelCount = outchans;
            out_parameters.sampleFormat = paFloat32 | paNonInterleaved;
            out_parameters.suggestedLatency = suggestedLatencyOut;
            out_parameters.hostApiSpecificStreamInfo = nullptr;
        }

        PaStreamParameters* in_stream_parameters = inchans ? &in_parameters : nullptr;
        PaStreamParameters* out_stream_parameters = outchans ? &out_parameters : nullptr;

        PaError supported = Pa_IsFormatSupported(in_stream_parameters, out_stream_parameters, samplerate);
        if (supported != 0) {
            report_error(supported);
            return false;
        }


        engine_initalised = false;
        blocksize_ = pa_blocksize;

        PaError opened = Pa_OpenStream(&stream, in_stream_parameters, out_stream_parameters, samplerate, pa_blocksize,
                                       paNoFlag, &portaudio_backend::pa_process, this);

        report_error(opened);

        if (opened != paNoError)
            return false;
        else {
            const PaStreamInfo* psi = Pa_GetStreamInfo(stream);
            if (psi)
                latency_ = (uint32_t)(psi->outputLatency * psi->sampleRate);
        }

        input_channels = inchans;
        super::input_samples.resize(inchans);
        output_channels = outchans;
        super::output_samples.resize(outchans);
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
    std::string device_name(PaDeviceIndex device_index) {
#ifndef __APPLE__
        // list APIs on Windows and Linux
        const PaDeviceInfo* device_info = Pa_GetDeviceInfo(device_index);
        const PaHostApiInfo* api_info = Pa_GetHostApiInfo(device_info->hostApi);
        std::string dev_string = api_info->name;
        dev_string += " : ";
        dev_string += device_info->name;
        return dev_string;
#else
        const PaDeviceInfo* device_info = Pa_GetDeviceInfo(device_index);
        return std::string(device_info->name);
#endif
    }

    int perform(const void* inputBuffer, void* outputBuffer, unsigned long frames,
                const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags) {
        if (unlikely(!engine_initalised)) {
            engine_functor::init_thread();
            engine_functor::sync_clock();
            engine_initalised = true;
        }

        if (statusFlags & (paInputOverflow | paInputUnderflow | paOutputOverflow | paOutputUnderflow))
            engine_functor::sync_clock();

        const float* inputs[input_channels];
        float* const* in = static_cast<float* const*>(inputBuffer);
        for (uint16_t i = 0; i != input_channels; ++i)
            inputs[i] = in[i];

        float* outputs[output_channels];
        float** out = static_cast<float**>(outputBuffer);
        for (uint16_t i = 0; i != output_channels; ++i)
            outputs[i] = out[i];

        unsigned long processed = 0;
        while (processed != frames) {
            super::fetch_inputs(inputs, blocksize_, input_channels);
            engine_functor::run_tick();
            super::deliver_outputs(outputs, blocksize_, output_channels);
            processed += blocksize_;
        }

        cpu_time_accumulator.update(Pa_GetStreamCpuLoad(stream) * 100.0);
        return paContinue;
    }

    static int pa_process(const void* input, void* output, unsigned long frame_count,
                          const PaStreamCallbackTimeInfo* time_info, PaStreamCallbackFlags status_flags,
                          void* user_data) {
        portaudio_backend* self = static_cast<portaudio_backend*>(user_data);
        return self->perform(input, output, frame_count, time_info, status_flags);
    }

    PaStream* stream = nullptr;
    uint32_t blocksize_ = 0;
    bool engine_initalised = false;
    cpu_time_info cpu_time_accumulator;

    uint32_t latency_ = 0;
};

} /* namespace nova */
