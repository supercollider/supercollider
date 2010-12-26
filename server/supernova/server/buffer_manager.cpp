//  nova server, buffer manager class, implementation
//  Copyright (C) 2009 Tim Blechmann
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

#include "sndfile.hh"

#include "buffer_manager.hpp"
#include "utilities/malloc_aligned.hpp"
#include "utilities/sized_array.hpp"


namespace nova
{

void buffer_wrapper::allocate(size_t frames, uint channels)
{
    if (data)
        free_aligned(data);
    data = calloc_aligned<sample_t>(frames * channels);

    frames_ = frames;
    channels_ = channels;
}

namespace
{

SndfileHandle open_file(const char * file, std::size_t start_frame)
{
    SndfileHandle sndfile(file);
    if (!sndfile)
        throw std::runtime_error(std::string("could not open file: ") + std::string(file));

    long seek = sndfile.seek(start_frame, SEEK_SET);
    if (seek == -1)
        throw std::runtime_error(std::string("could not seek file: ") + std::string(file));

    return sndfile;
}

} /* namespace */

void buffer_wrapper::read_file(const char * file, size_t start_frame, size_t frames)
{
    free();
    SndfileHandle sndfile = open_file(file, start_frame);

    sample_rate_ = sndfile.samplerate();
    channels_ = sndfile.channels();

    size_t end_frame = std::min(start_frame + frames, size_t(sndfile.frames()));
    size_t frame_count = end_frame - start_frame;

    data = malloc_aligned<sample_t>(frame_count * channels_);

    size_t read = sndfile.readf(data, frame_count);
    if (read != frame_count)
        throw std::runtime_error(std::string("could not read from file: ") + std::string(file));
}


void buffer_wrapper::read_file_channels(const char * file, size_t start_frame, size_t frames,
                                        uint channel_count, uint * channels)
{
    free();
    SndfileHandle sndfile = open_file(file, start_frame);

    channels_ = channel_count;
    sample_rate_ = sndfile.samplerate();

    for (uint c = 0; c != channel_count; ++c) {
        if (channels[c] >= uint(sndfile.channels()))
            throw std::runtime_error(std::string("requesting channel, that is not in file: ") + std::string(file));
    }

    size_t end_frame = std::min(start_frame + frames, size_t(sndfile.frames()));
    size_t frame_count = end_frame - start_frame;

    data = malloc_aligned<sample_t>(frame_count * channels_);

    sized_array<sample_t> tmp_array(channels_);

    for (size_t i = 0; i != frame_count; ++i) {
        size_t read = sndfile.readf(tmp_array.c_array(), 1);
        if (read != 1)
            throw std::runtime_error(std::string("could not from read file: ") + std::string(file));
        sample_t * data_frame = data + channel_count * i;

        for (uint c = 0; c != channel_count; ++c)
            data_frame[c] = tmp_array[channels[c]];
    }
}


void buffer_wrapper::write_file(const char * file, const char * header_format, const char * sample_format,
                                size_t start_frame, size_t frames)
{
    std::string header (header_format);
    std::string sample_format_ (sample_format);

    int format = 0;

    if (header == "wav")
        format = SF_FORMAT_WAV;
    else if (header == "aiff")
        format = SF_FORMAT_AIFF;
    else if (header == "next")
        format = SF_FORMAT_AU;
    else if (header == "ircam")
        format = SF_FORMAT_IRCAM;
    else if (header == "ircam")
        format = SF_FORMAT_IRCAM;
    else if (header == "raw")
        format = SF_FORMAT_RAW;
    else if (header == "flac")
        format = SF_FORMAT_FLAC;
    else
        throw std::runtime_error("unknown header format requested");

    if (sample_format_ == "int8")
        format |= SF_FORMAT_PCM_S8;
    else if (sample_format_ == "int16")
        format |= SF_FORMAT_PCM_16;
    else if (sample_format_ == "int24")
        format |= SF_FORMAT_PCM_24;
    else if (sample_format_ == "int32")
        format |= SF_FORMAT_PCM_32;
    else if (sample_format_ == "float")
        format |= SF_FORMAT_FLOAT;
    else if (sample_format_ == "double")
        format |= SF_FORMAT_DOUBLE;
    else if (sample_format_ == "mulaw")
        format |= SF_FORMAT_ULAW;
    else if (sample_format_ == "alaw")
        format |= SF_FORMAT_ALAW;
    else
        throw std::runtime_error("unknown sample format requested");

    SndfileHandle sndfile(file, SFM_WRITE, format, channels_, sample_rate_);
    if (!sndfile)
        throw std::runtime_error(std::string("could not open file: ") + std::string(file));

    size_t written = sndfile.writef(data, frames_);
    if (written != frames_)
        throw std::runtime_error(std::string("could not write file: ") + std::string(file));
}

} /* namespace nova */
