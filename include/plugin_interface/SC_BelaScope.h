#pragma once
#include "libraries/Scope/Scope.h"
#include <string.h>

class BelaScope {
public:
    BelaScope(uint32_t maxChannels_, float sampleRate, uint32_t blockSize):
        scope(),
        maxChannels(maxChannels_),
        bufferSamples(maxChannels_ * blockSize) {
        scope.setup(maxChannels, sampleRate);
        buffer = new float[bufferSamples]();
    }

    ~BelaScope() { delete[] buffer; }

    void logBuffer() {
        if (touched) {
            float* data = buffer;
            for (unsigned int frame = 0; frame < bufferSamples; frame += maxChannels) {
                scope.log(data);
                data += maxChannels;
            }
            memset(buffer, 0, bufferSamples * sizeof(float));
            touched = false;
        }
    }

    float* buffer;
    uint32_t maxChannels;
    uint32_t bufferSamples;
    bool touched;

private:
    Scope scope;
};
