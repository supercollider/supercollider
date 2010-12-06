//  $Id$
//
//  templated nonlinear filter classes
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

#ifndef DSP_NONLINEAR_FILTER_HPP
#define DSP_NONLINEAR_FILTER_HPP

#include <boost/mpl/if.hpp>
#include "sample_extractor.hpp"

namespace nova
{

template <typename sample_type, typename internal_type = sample_type, bool copy_temporary = true>
class linear_smooth
{
private:
    typedef typename boost::mpl::if_c<copy_temporary,
                                      internal_type,
                                      internal_type &
                                      >::type temporary_type;

public:
    linear_smooth(void):
        last_(0), incr_(0), target_(0), nleft_(0)
    {
        set_down(0);
        set_up(0);
    }

    template <typename input_buffer_type, typename output_buffer_type>
    inline void perform(input_buffer_type const & in, output_buffer_type & out, uint n)
    {
        const sample_type * in_sample = get_samples(in);
        sample_type * out_sample = get_samples(out);

        temporary_type last = last_;
        temporary_type incr = incr_;
        uint nleft = nleft_;

        temporary_type target = target_;

        for (int i = 0; i != n; ++i)
        {
            internal_type current = *in_sample++;
            if (current != target)
            {
                target = current;
                if (current > last)
                {
                    if (nup_ > 1)
                    {
                        incr = (current - last) * upcoef_;
                        nleft = nup_;
                        last += incr;
                        *out_sample++ = last;
                        continue;
                    }
                }
                else if (current < last)
                {
                    if (ndown_ > 1)
                    {
                        incr = (current - last) * downcoef_;
                        nleft = ndown_;
                        last += incr;
                        *out_sample++ = last;
                        continue;
                    }
                }
                incr = 0.;
                nleft = 0;
                *out_sample++ = last = current;
            }

            /* input didn't change */
            else if (nleft > 0)
            {
                last += incr;
                *out_sample++ = last;
                if (--nleft == 0)
                    last = target;
            }
            else
                *out_sample++ = target;
        }

        target_ = target;
        nleft_ = nleft;
        last_ = last;
        incr_ = incr;
    }

    void set_down(internal_type f)
    {
        if (f > 1)
        {
            ndown_ = uint(f);
            downcoef_ = 1 / f;
        }
        else
        {
            ndown_ = 0;
            downcoef_ = 0.;
        }
    }

    void set_up(internal_type f)
    {
        if (f > 1)
        {
            nup_ = uint(f);
            upcoef_ = 1 / f;
        }
        else
        {
            nup_ = 0;
            upcoef_ = 0.;
        }
    }


private:
    internal_type last_;
    internal_type target_;

    uint nup_;
    uint ndown_;
    uint nleft_;

    internal_type incr_;
    internal_type upcoef_;
    internal_type downcoef_;
};



} /* namespace nova */

#endif /* DSP_NONLINEAR_FILTER_HPP */
