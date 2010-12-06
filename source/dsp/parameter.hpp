//  $Id$
//
//  templated parameter classes
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


#ifndef DSP_PARAMETER_HPP
#define DSP_PARAMETER_HPP

#include "utilities.hpp"

namespace nova
{

namespace detail
{

template <typename data_type>
class noninterpolating_parameter
{
public:
    noninterpolating_parameter(void):
        data_(0)
    {
    }

    /* used for copying and assignment */
    template <typename arg_type>
    noninterpolating_parameter(arg_type const & data):
        data_(data)
    {
    }

    operator data_type(void) const
    {
        return data_;
    }

    void update(noninterpolating_parameter const & arg)
    {
    }

    void reset(data_type const & arg)
    {
        data_ = arg;
    }

protected:
    data_type data_;
};


template <typename data_type,
          std::size_t interpolation_steps = 4410>
class linear_interpolating_parameter
{
public:
    linear_interpolating_parameter(void):
        current_(0), steps_(0), incr_(0)
    {}

    linear_interpolating_parameter(linear_interpolating_parameter const & rhs):
        current_(rhs.current_), steps_(rhs.steps_), incr_(rhs.incr_)
    {}

    linear_interpolating_parameter(data_type const & data):
        current_(data), steps_(0), incr_(0)
    {}

    void operator=(data_type const & data)
    {
        steps_ = interpolation_steps;
        incr_ = (data - current_) / data_type(interpolation_steps);
    }

    operator data_type(void) const
    {
        if (steps_)
        {
            current_ += incr_;
            steps_ -= 1;
        }

        return current_;
    }

    void update(linear_interpolating_parameter const & rhs)
    {
        current_ = rhs.current_;
        steps_ = rhs.steps_;
    }

    void reset(data_type const & arg)
    {
        current_ = arg;
        steps_ = 0;
    }

protected:
    mutable data_type current_;
    mutable std::size_t steps_;
    data_type incr_;
};


template <typename data_type,
          std::size_t delay_time = 1,
          std::size_t decat_time = 1000>
class exponential_interpolating_parameter
{
public:
    exponential_interpolating_parameter(void):
        current_(0), target_(0)
    {
    }

    exponential_interpolating_parameter(exponential_interpolating_parameter const & rhs):
        current_(rhs.current_), target_(rhs.target_)
    {
    }

    exponential_interpolating_parameter(data_type const & data):
        current_(data), target_(data)
    {
    }

    void operator=(data_type const & data)
    {
        target_ = data;
    }

    operator data_type(void) const
    {
        const data_type interpolation (GetDecayFactor(data_type(delay_time), data_type(decat_time)));
        current_ = current_ * interpolation + target_ * (data_type(1) - interpolation);
        return current_;
    }

    void update(exponential_interpolating_parameter const & rhs)
    {
        current_ = rhs.current_;
    }

    void reset(data_type const & arg)
    {
        current_ = arg;
        target_ = arg;
    }

protected:
    mutable data_type current_;
    data_type target_;
};


} /* namespace detail */



}

#endif /* DSP_PARAMETER_HPP */
