// helper functions for SuperCollider server's PortAudio backend

#pragma once

#include "portaudio.h"
#include <string>

// get device name from PaDeviceIndex or PaDeviceInfo
std::string GetPaDeviceName(PaDeviceIndex index);
std::string GetPaDeviceName(const PaDeviceInfo* pdi);

// get PaDeviceIndex from device name
PaDeviceIndex GetPaDeviceFromName(const char* device, bool isInput);

// select default PA devices if they are not defined
// it will also try to check for some configuration problems
// numIns, numOuts and sampleRate are only the requested values, may change later
// PaError will be reported if the devices don't support a requested or a common sample rate
PaError TryGetDefaultPaDevices(PaDeviceIndex* inDevice, PaDeviceIndex* outDevice, int numIns, int numOuts,
                               double sampleRate);

// create PaStreamParameters with requested values
PaStreamParameters MakePaStreamParameters(int device, int channelCount, double suggestedLatency);
