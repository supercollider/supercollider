// helper functions for SuperCollider server's PortAudio backend

#pragma once

#include "portaudio.h"
#include <string>

enum class IOType { Input, Output };

// get device name from PaDeviceIndex
std::string GetPaDeviceName(PaDeviceIndex index);

// get PaDeviceIndex from device name
PaDeviceIndex GetPaDeviceFromName(const char* device, IOType ioType);

// select default PA devices if they are not defined
// it will also try to check for some configuration problems
// numIns, numOuts and sampleRate are only the requested values, may change later
PaError TryGetDefaultDevices(int* inDevice, int* outDevice, int numIns, int numOuts, double sampleRate);

// create PaStreamParameters with requested values
PaStreamParameters MakePaStreamParameters(int device, int channelCount, double suggestedLatency);
