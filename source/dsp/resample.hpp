//  $Id$
//
//  resample class
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


#ifndef DSP_RESAMPLE_HPP
#define DSP_RESAMPLE_HPP

#include "utilities/template_functions.hpp"
#include "kernel/ugen/ugen_types.hpp"

namespace nova
{

class resample
{
public:
    /*  */
    resample();

    ~resample(void);

    void set_ratio(double ratio);

    uint perform(const sample * in, uint availabe_frames, sample * out, uint output_frames);

private:
    double ratio_;
    void * handle;
};
}

#endif /* DSP_RESAMPLE_HPP */
