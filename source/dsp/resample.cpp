
//  $Id$
//
//  resample class, implementation wrapper for libresample & libsamplerate
//  Copyright (C) 2007 Tim Blechmann
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

//  $Revision$
//  $LastChangedRevision$
//  $LastChangedDate$
//  $LastChangedBy$

#include "resample.hpp"

#include "libs/resample_config.h"

#include <iostream>

#ifdef HAVE_LIBSAMPLERATE
#include <samplerate.h>
namespace nova
{
namespace
{
inline void handle_src_error(int error)
{
    if (unlikely (error))
        std::cerr << src_strerror(error) << std::endl;
}
} /* namespace */

resample::resample(void)
{
    int error;
    handle = reinterpret_cast<void*>(src_new(SRC_SINC_FASTEST, 1, &error));

    handle_src_error(error);
}

resample::~resample(void)
{
    src_delete(reinterpret_cast<SRC_STATE*>(handle));
}

void resample::set_ratio(double ratio)
{
    ratio_ = ratio;
    int error = src_set_ratio(reinterpret_cast<SRC_STATE*>(handle), ratio);

    handle_src_error(error);
}

uint resample::perform(const sample * in, uint availabe_frames, sample * out, uint output_frames)
{
    SRC_DATA data;

    data.data_in = const_cast<sample*>(in);
    data.data_out = out;

    data.input_frames = availabe_frames;
    data.output_frames = output_frames;

    data.src_ratio = ratio_;

    int error = src_process(reinterpret_cast<SRC_STATE*>(handle), &data);

    handle_src_error(error);

    return data.output_frames;
}
} /* namespace nova */

#else
#include <libresample.h>

namespace nova
{
resample::resample()
{
    handle = resample_open(1, 0.001, 100); /* libresample has some rate limitations */
}

resample::~resample(void)
{
    resample_close(handle);
}

void resample::set_ratio(double ratio)
{
    ratio = clip(ratio, 0.001, 100);
    ratio_ = ratio;
}

uint resample::perform(const sample * in, uint availabe_frames, sample * out, uint output_frames)
{
    int used_frames;
    int lastFlag = 0;       /* ??? */
    int written_frames = resample_process(handle, ratio_, const_cast<sample*>(in), availabe_frames, lastFlag,
                                          &used_frames, out, output_frames);

    return written_frames;
}
} /* namespace nova */

#endif
