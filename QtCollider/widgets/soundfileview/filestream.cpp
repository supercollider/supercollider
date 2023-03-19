/************************************************************************
 *
 * Copyright 2012 Jakob Leben (jakob.leben@gmail.com)
 *
 * This file is part of SuperCollider Qt GUI.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 ************************************************************************/

#include "view.hpp"
#include <limits>

SoundFileStream::SoundFileStream(): _data(0), _dataSize(0), _dataOffset(0) {}

SoundFileStream::SoundFileStream(SNDFILE* sf, const SF_INFO& info, sf_count_t b, sf_count_t d): _data(0) {
    load(sf, info, b, d);
}

SoundFileStream::~SoundFileStream() { delete[] _data; }

void SoundFileStream::load(SNDFILE* sf, const SF_INFO& info, sf_count_t beg, sf_count_t dur) {
    delete[] _data;

    _dataOffset = beg;
    _dataSize = dur;

    _data = new float[_dataSize * info.channels];
    sf_seek(sf, _dataOffset, SEEK_SET);

    _dataSize = sf_readf_float(sf, _data, _dataSize);

    _ch = info.channels;
    _beg = _dataOffset;
    _dur = _dataSize;
}

bool SoundFileStream::integrate(int ch, double f_beg, double f_dur, float* minBuffer, float* maxBuffer, float* sumBuf,
                                float* sum2Buf, int bufferSize) {
    bool ok = _data != 0 && ch < channels() && (f_beg >= beginning()) && (f_beg + f_dur <= beginning() + duration());
    if (!ok)
        return false;

    double fpu = f_dur / bufferSize;
    double f_pos = f_beg - _dataOffset;
    double f_pos_max = _dataSize;

    int i;
    for (i = 0; i < bufferSize; ++i) {
        int data_pos = floor(f_pos);

        // increment position

        // slower, but error-proof:
        // f_pos = (double)(i+1) / width() * f_dur + f_beg;

        // the following is a faster variant, but floating point operations are fallible,
        // so we need to make sure we stay within the constraints of f_dur;
        double f_pos1 = f_pos + fpu;
        if (f_pos1 > f_pos_max)
            f_pos1 = f_pos_max;

        int frame_count = ceil(f_pos1) - data_pos;

        float frac0 = data_pos + 1.f - f_pos;
        float frac1 = f_pos1 + 1.f - ceil(f_pos1);

        // get min, max and sum
        float* samples = _data + (data_pos * channels()) + ch;
        float min = std::numeric_limits<float>::max();
        float max = std::numeric_limits<float>::lowest();
        float sum = 0.f;
        float sum2 = 0.f;
        int f; // frame
        for (f = 0; f < frame_count; ++f, samples += channels()) {
            // TODO should we overlap min-max or not here?
            float sample = *samples;
            float frac;
            if (f == 0)
                frac = frac0;
            else if (f == frame_count - 1)
                frac = frac1;
            else
                frac = 1.0;

            if (sample < min)
                min = sample;
            if (sample > max)
                max = sample;

            sum += sample * frac;
            sum2 += sample * sample * frac;
        }

        minBuffer[i] = min;
        maxBuffer[i] = max;
        sumBuf[i] = sum;
        sum2Buf[i] = sum2;

        f_pos = f_pos1;
    }

    return true;
}

bool SoundFileStream::displayData(int ch, double f_beg, double f_dur, float* minBuffer, float* maxBuffer, float* minRMS,
                                  float* maxRMS, int bufferSize) {
    bool ok = _data != 0 && ch < channels() && (f_beg >= beginning()) && (f_beg + f_dur <= beginning() + duration());
    if (!ok)
        return false;

    double fpu = f_dur / bufferSize;
    double f_pos = f_beg - _dataOffset;
    double f_pos_max = _dataSize;

    float min = std::numeric_limits<float>::max();
    float max = std::numeric_limits<float>::lowest();

    int i;
    for (i = 0; i < bufferSize; ++i) {
        int data_pos = floor(f_pos);

        // increment position

        // slower, but error-proof:
        // f_pos = (double)(i+1) / width() * f_dur + f_beg;

        // the following is a faster variant, but floating point operations are fallible,
        // so we need to make sure we stay within the constraints of f_dur;
        double f_pos1 = f_pos + fpu;
        if (f_pos1 > f_pos_max)
            f_pos1 = f_pos_max;

        int frame_count = ceil(f_pos1) - data_pos;

        float frac0 = data_pos + 1.f - f_pos;
        float frac1 = f_pos1 + 1.f - ceil(f_pos1);

        // get min, max and sum
        float* samples = _data + (data_pos * channels()) + ch;

        float sum = 0.f;
        float sum2 = 0.f;
        int f; // frame
        for (f = 0; f < frame_count; ++f, samples += channels()) {
            // TODO should we overlap min-max or not here?
            float sample = *samples;
            float frac;
            if (f == 0)
                frac = frac0;
            else if (f == frame_count - 1)
                frac = frac1;
            else
                frac = 1.0;

            if (sample < min)
                min = sample;
            if (sample > max)
                max = sample;

            sum += sample * frac;
            sum2 += sample * sample * frac;
        }

        double n = fpu;
        double avg = sum / n;
        double stdDev = std::sqrt(std::abs((sum2 - (sum * avg)) / n));

        minBuffer[i] = min;
        maxBuffer[i] = max;
        minRMS[i] = avg - stdDev;
        maxRMS[i] = avg + stdDev;

        f_pos = f_pos1;
        min = maxBuffer[i];
        max = minBuffer[i];
    }

    return true;
}

float* SoundFileStream::rawFrames(int ch, sf_count_t b, sf_count_t d, bool* interleaved) {
    if (ch > channels() || b < _dataOffset || b + d > _dataOffset + _dataSize)
        return 0;
    *interleaved = true;
    sf_count_t offset = (b - _dataOffset) * channels() + ch;
    return (_data + offset);
}
