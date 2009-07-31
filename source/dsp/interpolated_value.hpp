//  template classes for interpolated values
//  Copyright (C) 2008 Tim Blechmann
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

#ifndef DSP_INTERPOLATED_VALUE_HPP
#define DSP_INTERPOLATED_VALUE_HPP

#include <boost/cstdint.hpp>

namespace nova
{

/** templated helper class for wrapping interpolation steps */
template <typename float_t = float>
class interpolated_value
{
public:
    typedef boost::uint_fast16_t interpolation_size_type;

    /** rtti-style interpolator class
     *
     * */
    struct interpolator
    {
        interpolator(float_t const & value, float_t const & slope):
            value (value), slope(slope)
        {}

        float_t step(void)
        {
            float_t ret = value;
            value += slope;
            return ret;
        }

        float_t value;
        const float_t slope;
    };

    interpolated_value(float_t const & begin, interpolation_size_type size):
        value(begin), inv_size(1/float_t(size))
    {}

    interpolator get_interpolator(float_t const & target)
    {
        float_t slope = (target - value) * inv_size;
        float_t val = value;
        value = target;
        return interpolator(val, slope);
    }

    float_t get_value(void) const
    {
        return value;
    }

private:
    float_t value;
    const float_t inv_size;
};

/** templated helper class for wrapping interpolation steps */
template <typename float_t = float>
class interpolated_value_long_interpolation
{
public:
    typedef boost::uint_fast16_t interpolation_size_type;

    /** rtti-style interpolator class
     *
     *  \todo check if it makes more sense to set the value from estimated target or value
     *
     * */
    struct interpolator
    {
        interpolator(interpolated_value_long_interpolation & iv):
            value (iv.value),
            slope (iv.slope),
            iv_(iv)
        {}

        ~interpolator(void)
        {
            iv_.value = value;
        }

        float_t step(void)
        {
            float_t ret = value;
            value += slope;
            return ret;
        }

        float_t value;
        const float_t slope;
        interpolated_value_long_interpolation & iv_;
    };

    interpolated_value_long_interpolation(float_t const & begin, interpolation_size_type size):
        value(begin), interpolation_size(size), remaining(0)
    {}

    void set_target(float_t const & target)
    {
        slope = (target - value) / float_t(interpolation_size);
        remaining = interpolation_size;
    }

    interpolator get_interpolator(interpolation_size_type steps)
    {
        assert(remaining >= steps);
        remaining -= steps;

        return interpolator(*this);
    }

    float_t get_value(void) const
    {
        return value;
    }

    interpolation_size_type get_remaining(void) const
    {
        return remaining;
    }

    interpolation_size_type get_remaining(interpolation_size_type const & maximum) const
    {
        return std::min(remaining, maximum);
    }

private:
    float_t value;
    float_t slope;
    const interpolation_size_type interpolation_size;
    interpolation_size_type remaining;
};


} /* namespace nova */

#endif /* DSP_INTERPOLATED_VALUE_HPP */
