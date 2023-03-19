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

#include <cstring>

SoundCacheStream::SoundCacheStream():
    SoundStream(0, 0.0, 0.0),
    _caches(0),
    _fpu(0.0),
    _dataOffset(0),
    _dataSize(0),
    _ready(false),
    _loading(false),
    _loadProgress(0) {
    _loader = new SoundCacheLoader(this);
    connect(_loader, SIGNAL(loadProgress(int)), this, SLOT(onLoadProgress(int)), Qt::QueuedConnection);
    connect(_loader, SIGNAL(loadingDone()), this, SLOT(onLoadingDone()), Qt::QueuedConnection);
}

SoundCacheStream::~SoundCacheStream() {
    if (_loader->isRunning()) {
        _loader->terminate();
        _loader->wait();
    }
    delete[] _caches;
}

void SoundCacheStream::load(const QVector<double>& data, int nf, int offset, int ch) {
    if (_loader->isRunning()) {
        _loader->terminate();
        _loader->wait();
    }

    _ready = false;
    _loading = true;
    _loadProgress = 0;

    delete[] _caches;

    _ch = ch;
    _beg = _dataOffset = 0;
    _dur = _dataSize = nf;
    _fpu = 1.0;

    _caches = new SoundCache[ch];

    for (int c = 0; c < ch; ++c) {
        float* min = _caches[c].min = new float[nf];
        float* max = _caches[c].max = new float[nf];
        float* sum = _caches[c].sum = new float[nf];
        float* sum2 = _caches[c].sum2 = new float[nf];

        int s = c + (offset * ch);
        for (int f = 0; f < nf; ++f, s += ch) {
            double val = data[s];
            min[f] = val;
            sum[f] = val;
            sum2[f] = val * val;
        }
        memcpy(max, min, nf * sizeof(float));
    }

    _loadProgress = 100;
    _loading = false;
    _ready = true;
    Q_EMIT(loadingDone());
}

void SoundCacheStream::load(SNDFILE* sf, const SF_INFO& info, sf_count_t beg, sf_count_t dur, int maxFramesPerUnit,
                            int maxRawFrames) {
    Q_ASSERT(maxRawFrames > 0 && maxFramesPerUnit > 0);

    _ready = false;
    _loadProgress = 0;

    if (_loader->isRunning()) {
        _loader->terminate();
        _loader->wait();
    }
    delete[] _caches;

    _ch = info.channels;
    _beg = _dataOffset = beg;
    _dur = dur;

    // adjust data size for data amount limit
    if (_dur <= maxRawFrames) {
        // ok, not crossing the limit
        _dataSize = _dur;
        _fpu = 1.0;
    } else {
        _dataSize = maxRawFrames;
        _fpu = (double)_dur / _dataSize;
        // re-adjust for data resolution limit
        if (_fpu > maxFramesPerUnit) {
            _dataSize = (double)_dur / maxFramesPerUnit;
            _fpu = (double)_dur / _dataSize;
        }
    }

    _caches = new SoundCache[info.channels];
    int ch;
    for (ch = 0; ch < info.channels; ++ch) {
        _caches[ch].min = new float[_dataSize];
        _caches[ch].max = new float[_dataSize];
        _caches[ch].sum = new float[_dataSize];
        _caches[ch].sum2 = new float[_dataSize];
    }

    _loading = true;
    _loader->load(sf, info);
}

void SoundCacheStream::allocate(int nf, int ch) {
    Q_ASSERT(nf > 0 && ch > 0);

    if (_loader->isRunning()) {
        _loader->terminate();
        _loader->wait();
    }

    delete[] _caches;

    _ch = ch;
    _beg = _dataOffset = 0;
    _dur = _dataSize = nf;
    _fpu = 1.0;

    _caches = new SoundCache[ch];

    for (int c = 0; c < ch; ++c) {
        float* min = _caches[c].min = new float[nf];
        float* max = _caches[c].max = new float[nf];
        float* sum = _caches[c].sum = new float[nf];
        float* sum2 = _caches[c].sum2 = new float[nf];

        size_t bytes = nf * sizeof(float);
        memset(min, 0, bytes);
        memset(max, 0, bytes);
        memset(sum, 0, bytes);
        memset(sum2, 0, bytes);
    }

    _loadProgress = 100;
    _loading = false;
    _ready = true;
}

void SoundCacheStream::write(const QVector<double>& data, int offset, int count) {
    Q_ASSERT(_ready && _fpu == 1.0);
    Q_ASSERT(count == (float)data.size() / _ch);

    int end = offset + count;

    // make sure range is ok
    Q_ASSERT(offset >= 0 && end <= _dataSize);

    for (int c = 0; c < _ch; ++c) {
        float* min = _caches[c].min;
        float* max = _caches[c].max;
        float* sum = _caches[c].sum;
        float* sum2 = _caches[c].sum2;

        int s = c;
        for (int f = offset; f < end; ++f, s += _ch) {
            double val = data[s];
            min[f] = val;
            sum[f] = val;
            sum2[f] = val * val;
        }
        memcpy(max + offset, min + offset, count * sizeof(float));
    }
}

bool SoundCacheStream::displayData(int ch, double f_beg, double f_dur, float* minBuffer, float* maxBuffer,
                                   float* minRMS, float* maxRMS, int bufferSize) {
    bool ok = _ready && ch < channels() && (f_beg >= beginning()) && (f_beg + f_dur <= beginning() + duration())
        && bufferSize <= f_dur * _fpu;
    if (!ok)
        return false;

    double ratio = f_dur / _fpu / bufferSize;
    double cache_pos = (f_beg - _dataOffset) / _fpu;

    float min = std::numeric_limits<float>::max();
    float max = std::numeric_limits<float>::lowest();

    int i;
    for (i = 0; i < bufferSize; ++i) {
        int f = floor(cache_pos); // first frame
        bool no_overlap = f == ceil(cache_pos);
        float frac0 = f + 1.f - cache_pos;

        cache_pos += ratio;
        // Due to possibility of floating point operation failures.
        if (cache_pos > _dataSize)
            cache_pos = _dataSize;
        int frame_count = ceil(cache_pos) - f;
        float frac1 = cache_pos + 1.f - ceil(cache_pos);

        double sum = 0.0;
        double sum2 = 0.0;

        float* p_min = _caches[ch].min;
        float* p_max = _caches[ch].max;
        float* p_sum = _caches[ch].sum;
        float* p_sum2 = _caches[ch].sum2;
        int countdown = frame_count;
        while (countdown--) {
            // NOTE for min-max, behave as if first frame was ceil(cache_pos) instead of floor(),
            // to not smudge too much at large scale
            if (countdown < frame_count - 1 || no_overlap) {
                if (p_min[f] < min)
                    min = p_min[f];
                if (p_max[f] > max)
                    max = p_max[f];
            }

            float frac;
            if (countdown == frame_count - 1)
                frac = frac0;
            else if (countdown == 0)
                frac = frac1;
            else
                frac = 1.0;

            sum += p_sum[f] * frac;
            sum2 += p_sum2[f] * frac;

            ++f;
        }

        double n = f_dur / bufferSize;
        double avg = sum / n;
        double stdDev = std::sqrt(std::abs((sum2 - (sum * avg)) / n));

        minBuffer[i] = min;
        maxBuffer[i] = max;
        minRMS[i] = avg - stdDev;
        maxRMS[i] = avg + stdDev;

        // assure continuity from pixel to pixel
        min = maxBuffer[i];
        max = minBuffer[i];
    }

    return true;
}

float* SoundCacheStream::rawFrames(int ch, sf_count_t b, sf_count_t d, bool* interleaved) {
    if (!_ready || _fpu != 1.0 || ch > channels() || b < _dataOffset || b + d > _dataOffset + _dataSize)
        return 0;

    *interleaved = false;
    return _caches[ch].min + b - _dataOffset;
}

/*SoundCacheLoader::SoundCacheLoader( SNDFILE *sf, const SF_INFO &info,
                                    int maxFPU, int maxRawFrames )
: _sf( sf ),
  _sfInfo( info ),
  _maxFPU( maxFPU ),
  maxRawFrames( maxRawFrames )
{}*/

void SoundCacheStream::onLoadProgress(int progress) {
    _loadProgress = progress;
    Q_EMIT(loadProgress(progress));
}

void SoundCacheStream::onLoadingDone() {
    // FIXME what if the signal is received just after starting another load?
    _ready = true;
    _loading = false;
    Q_EMIT(loadingDone());
}

void SoundCacheLoader::load(SNDFILE* sf, const SF_INFO& info) {
    Q_ASSERT(!isRunning());
    _sf = sf;
    _info = info;
    start();
}

void SoundCacheLoader::run() {
    Q_ASSERT(_sf);

    int channels = _cache->channels();
    double fpu = _cache->_fpu;
    int size = _cache->_dataSize;
    double offset = _cache->_dataOffset;
    SoundCache* chanCaches = _cache->_caches;

    int i = 0;
    while (i < size) {
        int chunkSize = qMin(1000, size - i);

        double beg = i * fpu + offset;
        double dur = chunkSize * fpu;

        sf_count_t i_beg = floor(beg);
        sf_count_t i_dur = ceil(beg + dur) - i_beg;

        SoundFileStream sfStream(_sf, _info, i_beg, i_dur);

        int ch;
        for (ch = 0; ch < channels; ++ch) {
            sfStream.integrate(ch, beg, dur, chanCaches[ch].min + i, chanCaches[ch].max + i, chanCaches[ch].sum + i,
                               chanCaches[ch].sum2 + i, chunkSize);
        }

        i += chunkSize;

        Q_EMIT(loadProgress(i * 100 / size));
    }

    Q_EMIT(loadingDone());
}
