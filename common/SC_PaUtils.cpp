// helper functions for SuperCollider server's PortAudio backend

#include "SC_PaUtils.hpp"
#include <cstring>
#include <cstdio>
#include <cstdint>

#ifdef __APPLE__
#    include <pa_mac_core.h>
#endif

using PaSupportCheckFunc = PaError (*)(PaStreamParameters&, double);
PaError CheckDeviceSampleRateOrGetDefault(int* device, double sampleRate, int maxChannels, int defaultDevice,
                                          const char* deviceType, PaSupportCheckFunc isSupportedFunc) {
    if (*device != paNoDevice && sampleRate) {
        // check if device can support requested SR
        PaStreamParameters parameters = MakePaStreamParameters(*device, maxChannels, 0);
        PaError err = isSupportedFunc(parameters, sampleRate);
        if (err != paNoError) {
            fprintf(stdout, "PortAudio error: %s\nRequested sample rate %f for device %s is not supported\n",
                    Pa_GetErrorText(err), sampleRate, Pa_GetDeviceInfo(*device)->name);
            return err;
        }
    }
    // in case we still don't have a proper device, use the default device
    if (*device == paNoDevice) {
        *device = defaultDevice;
        if (*device != paNoDevice)
            fprintf(stdout, "Selecting default system %s device\n", deviceType);
    }
    return paNoError;
}

void TryMatchDeviceSameAPI(int* matchingDevice, const int* knownDevice, bool isInput) {
    if (*matchingDevice != paNoDevice || *knownDevice == paNoDevice)
        return;

    const auto* devInfo = Pa_GetDeviceInfo(*knownDevice);
    const auto* apiInfo = Pa_GetHostApiInfo(devInfo->hostApi);
    const auto maxChannels = isInput ? devInfo->maxInputChannels : devInfo->maxOutputChannels;
    bool isAsioFullDuplex = apiInfo->type == paASIO && maxChannels > 0;
    if (isAsioFullDuplex)
        *matchingDevice = *knownDevice;
    else {
        *matchingDevice = isInput ? apiInfo->defaultInputDevice : apiInfo->defaultOutputDevice;
        if (*matchingDevice != paNoDevice)
            fprintf(stdout, "Selecting default %s %s device\n", apiInfo->name, (isInput ? "input" : "output"));
    }
}

std::string GetPaDeviceName(PaDeviceIndex index) {
    auto* pdi = Pa_GetDeviceInfo(index);
    return GetPaDeviceName(pdi);
}

std::string GetPaDeviceName(const PaDeviceInfo* pdi) {
    std::string name;
#ifndef __APPLE__
    name += Pa_GetHostApiInfo(pdi->hostApi)->name;
    name += " : ";
#endif
    name += pdi->name;
    return name;
}

PaDeviceIndex GetPaDeviceFromName(const char* device, bool isInput) {
    if (device == nullptr || device[0] == '\0')
        return paNoDevice;

    PaDeviceIndex numDevices = Pa_GetDeviceCount();
    for (PaDeviceIndex i = 0; i < numDevices; i++) {
        auto* pdi = Pa_GetDeviceInfo(i);
        std::string devString = GetPaDeviceName(i);
        if (strstr(devString.c_str(), device)) {
            if (isInput && pdi->maxInputChannels > 0) {
                return i;
            } else if (!isInput && pdi->maxOutputChannels > 0) {
                return i;
            }
        }
    }

    return paNoDevice;
}

PaError TryGetDefaultPaDevices(PaDeviceIndex* inDevice, PaDeviceIndex* outDevice, int numIns, int numOuts,
                               double sampleRate) {
    if (numIns && !numOuts) {
        *outDevice = paNoDevice;
        auto maxChannels = (*inDevice != paNoDevice) ? Pa_GetDeviceInfo(*inDevice)->maxInputChannels : 0;
        return CheckDeviceSampleRateOrGetDefault(
            inDevice, sampleRate, maxChannels, Pa_GetDefaultInputDevice(), "input",
            [](PaStreamParameters& params, double sr) { return Pa_IsFormatSupported(&params, nullptr, sr); });
    } else if (!numIns && numOuts) {
        *inDevice = paNoDevice;
        auto maxChannels = (*outDevice != paNoDevice) ? Pa_GetDeviceInfo(*outDevice)->maxOutputChannels : 0;
        return CheckDeviceSampleRateOrGetDefault(
            outDevice, sampleRate, maxChannels, Pa_GetDefaultOutputDevice(), "output",
            [](PaStreamParameters& params, double sr) { return Pa_IsFormatSupported(nullptr, &params, sr); });
    } else if (numIns && numOuts) {
        // if one device is specified, let's try to open another one on matching api
        TryMatchDeviceSameAPI(inDevice, outDevice, true);
        TryMatchDeviceSameAPI(outDevice, inDevice, false);

        bool apisAreDifferent = *inDevice != paNoDevice && *outDevice != paNoDevice
            && Pa_GetDeviceInfo(*inDevice)->hostApi != Pa_GetDeviceInfo(*outDevice)->hostApi;
        if (apisAreDifferent) {
            fprintf(stdout, "Requested devices %s and %s use different API. ", GetPaDeviceName(*inDevice).c_str(),
                    GetPaDeviceName(*outDevice).c_str());
            *outDevice = Pa_GetHostApiInfo(Pa_GetDeviceInfo(*inDevice)->hostApi)->defaultOutputDevice;
            fprintf(stdout, "Setting output device to %s.\n", GetPaDeviceName(*outDevice).c_str());
        }
        // check for matching sampleRate or requested sample rate
        if (*inDevice != paNoDevice && *outDevice != paNoDevice) {
            const auto in_parameters =
                MakePaStreamParameters(*inDevice, Pa_GetDeviceInfo(*inDevice)->maxInputChannels, 0);
            const auto out_parameters =
                MakePaStreamParameters(*outDevice, Pa_GetDeviceInfo(*outDevice)->maxOutputChannels, 0);
            if (sampleRate) {
                // check if devices can support requested SR
                PaError err = Pa_IsFormatSupported(&in_parameters, &out_parameters, sampleRate);
                if (err != paNoError) {
                    fprintf(stdout, "\nRequested sample rate %f for devices %s and %s is not supported.\n", sampleRate,
                            GetPaDeviceName(*inDevice).c_str(), GetPaDeviceName(*outDevice).c_str());
                    return err;
                }
            } else {
                // if we don't request SR, check if devices have matching SR
                auto inSR = Pa_GetDeviceInfo(*inDevice)->defaultSampleRate;
                auto outSR = Pa_GetDeviceInfo(*outDevice)->defaultSampleRate;
                if (uint32_t(inSR) != uint32_t(outSR)) {
                    // if defaults are different, check if both devices can be opened using the OUTPUT's SR
                    PaError err = Pa_IsFormatSupported(&in_parameters, &out_parameters, outSR);
                    if (err != paNoError) {
                        fprintf(stdout,
                                "\nRequested devices %s and %s use different sample rates. "
                                "Please set matching sample rates "
                                "in the Windows Sound Control Panel and try again.\n",
                                GetPaDeviceName(*inDevice).c_str(), GetPaDeviceName(*outDevice).c_str());
                        return err;
                    }
                }
            }
        }

        // in case we still don't have a proper device, use default devices
        if (*inDevice == paNoDevice || *outDevice == paNoDevice) {
            *inDevice = Pa_GetDefaultInputDevice();
            *outDevice = Pa_GetDefaultOutputDevice();
            if (*inDevice != paNoDevice && *outDevice != paNoDevice)
                fprintf(stdout, "Selecting default system input/output devices\n");
        }
    } else {
        // no inputs nor outputs
        *inDevice = paNoDevice;
        *outDevice = paNoDevice;
    }
    return paNoError;
}

PaStreamParameters MakePaStreamParameters(int device, int channelCount, double suggestedLatency) {
    PaStreamParameters streamParams;
    PaSampleFormat fmt = paFloat32 | paNonInterleaved;
    streamParams.device = device;
    streamParams.channelCount = channelCount;
    streamParams.sampleFormat = fmt;
    streamParams.suggestedLatency = suggestedLatency;
#ifdef __APPLE__
    static PaMacCoreStreamInfo macInfo;
    PaMacCore_SetupStreamInfo(&macInfo, paMacCorePro);
    streamParams.hostApiSpecificStreamInfo = &macInfo;
#else
    streamParams.hostApiSpecificStreamInfo = nullptr;
#endif
    return streamParams;
}
