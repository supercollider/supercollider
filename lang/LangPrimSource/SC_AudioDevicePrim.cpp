/*
    SuperCollider real time audio synthesis system
    Copyright (c) 2002 James McCartney. All rights reserved.
    http://www.audiosynth.com

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
*/

#if defined(SC_AUDIO_API_COREAUDIO)
#    include <CoreAudio/AudioHardware.h>
#elif defined(SC_AUDIO_API_PORTAUDIO)
#    include "portaudio.h"
#    include "SC_PaUtils.hpp"
#endif

#include "SCBase.h"
#include "VMGlobals.h"
#include "PyrKernel.h"
#include "PyrPrimitive.h"
#include "GC.h"

enum class IoType { Out, In, Both };

// Creates an array of PyrString objects in the `slot`, filled with the contents of `deviceNames`
static void createDeviceNamesScArray(PyrSlot* slot, PyrGC* gc, const std::vector<std::string>& deviceNames)
{
    PyrObject* devArray = newPyrArray(gc, deviceNames.size() * sizeof(PyrObject), 0, true);
    SetObject(slot, devArray); // this is okay here as we don't use the receiver
    devArray->size = deviceNames.size();

    for (size_t i = 0; i < deviceNames.size(); i++) {
        PyrString* string = newPyrString(gc, deviceNames[i].c_str(), 0, true);
        SetObject(devArray->slots + i, string);
        // we know array is white so we can use GCWriteNew
        gc->GCWriteNew(devArray, reinterpret_cast<PyrObject*>(string));
    }
}

#if defined(SC_AUDIO_API_COREAUDIO)
int listDevices(VMGlobals* g, IoType type) {
    AudioObjectPropertyAddress propertyAddress;
    propertyAddress.mSelector = kAudioHardwarePropertyDevices;
    propertyAddress.mScope = kAudioObjectPropertyScopeGlobal;
    propertyAddress.mElement = kAudioObjectPropertyElementMaster;

    UInt32 count;
    OSStatus err = AudioObjectGetPropertyDataSize(kAudioObjectSystemObject, &propertyAddress, 0, nullptr, &count);
    std::vector<AudioDeviceID> deviceIds(count / sizeof(AudioDeviceID));
    err = AudioObjectGetPropertyData(kAudioObjectSystemObject, &propertyAddress, 0, nullptr, &count, &deviceIds[0]);
    if (err != kAudioHardwareNoError) {
        return errFailed;
    }

    if (type != IoType::Both) {
        if (type == IoType::Out) {
            propertyAddress.mScope = kAudioDevicePropertyScopeOutput;
        } else {
            propertyAddress.mScope = kAudioDevicePropertyScopeInput;
        }

        for (int i = 0; i < deviceIds.size(); i++) {
            propertyAddress.mSelector = kAudioDevicePropertyStreams;

            err = AudioObjectGetPropertyDataSize(deviceIds[i], &propertyAddress, 0, nullptr, &count);
            if (err != kAudioHardwareNoError) {
                return errFailed;
            }

            if (!count)
                devicesIds[i] = 0;
        }
    }

    deviceIds.erase(std::remove(deviceIds.begin(), deviceIds.end(), 0), deviceIds.end());

    std::vector<std::string> deviceNames;
    for (auto deviceId : deviceIds) {
        propertyAddress.mScope = kAudioObjectPropertyScopeGlobal;
        propertyAddress.mSelector = kAudioDevicePropertyDeviceName;

        err = AudioObjectGetPropertyDataSize(deviceId, &propertyAddress, 0, nullptr, &count);
        if (err != kAudioHardwareNoError) {
            break; // use what we have so far
        }

        std::string name;
        name.resize(count);
        err = AudioObjectGetPropertyData(deviceId, &propertyAddress, 0, nullptr, &count, &name[0]);
        if (err != kAudioHardwareNoError) {
            break; // use what we have so far
        }

        deviceNames.push_back(std::move(name));
    }

    createDeviceNamesScArray(g->sp - 2, g->gc, deviceNames);

    return errNone;
}

#elif defined(SC_AUDIO_API_PORTAUDIO)
int listDevices(VMGlobals* g, IoType type) {
    if (Pa_Initialize() != paNoError)
        return errFailed;

    auto numDevices = Pa_GetDeviceCount();
    if (numDevices < 0) {
        Pa_Terminate();
        return errFailed;
    }

    std::vector<const PaDeviceInfo*> deviceInfos;
    for (PaDeviceIndex i = 0; i < numDevices; i++) {
        auto* pdi = Pa_GetDeviceInfo(i);
        if (type == IoType::In) {
            if (pdi->maxInputChannels > 0)
                deviceInfos.push_back(pdi);
        } else if (type == IoType::Out) {
            if (pdi->maxOutputChannels > 0)
                deviceInfos.push_back(pdi);
        } else
            deviceInfos.push_back(pdi);
    }

    std::vector<std::string> deviceNames;
    for (auto* info : deviceInfos)
        deviceNames.push_back(GetPaDeviceName(info));

    createDeviceNamesScArray(g->sp - 2, g->gc, deviceNames);

    Pa_Terminate();
    return errNone;
}

#else
int listDevices(VMGlobals* g, IoType type) {
    return errFailed; // listing devices fails when using neither CoreAudio nor PortAudio
}
#endif

int prListAudioDevices(VMGlobals* g, int numArgsPushed) {
    int in, out;
    if (auto err = slotIntVal(g->sp, &out))
        return err;
    if (auto err = slotIntVal(g->sp - 1, &in))
        return err;

    if (in && out)
        return listDevices(g, IoType::Both);
    else if (in)
        return listDevices(g, IoType::In);
    else if (out)
        return listDevices(g, IoType::Out);
    else
        return errFailed;
}

void initAudioDevicePrimitives() {
    definePrimitive(nextPrimitiveIndex(), 0, "_ListAudioDevices", prListAudioDevices, 3, 0);
}
