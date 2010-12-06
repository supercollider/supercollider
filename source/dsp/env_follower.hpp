//  amplitude follower class
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

#ifndef DSP_ENV_FOLLOWER_HPP
#define DSP_ENV_FOLLOWER_HPP

#include <cmath>

#include "utilities.hpp"

namespace nova
{

/* envelope follower, based on code from music_dsp */
template <typename sample_type>
class env_follower
{
    static inline sample_type compute_sample(sample_type current_sample, sample_type const & envelope,
                                             sample_type const & attack_coef,
                                             sample_type const & release_coef)
    {
        const sample_type abs_current = abs(current_sample);

        if(abs_current > envelope)
            return attack_coef * (envelope - abs_current) + abs_current;
        else
            return release_coef * (envelope - abs_current) + abs_current;
    }

    sample_type envelope_;
    sample_type attack_coef_;
    sample_type release_coef_;

public:
    env_follower(void):
        envelope_(0)
    {
        set_coeffs(4410, 4410);
    }

    void set_coeffs(sample_type attack_samples, sample_type release_samples)
    {
        set_attack_coeff(attack_samples);
        set_release_coeff(release_samples);
    }

    void set_attack_coeff(sample_type attack_samples)
    {
        attack_coef_ = decay_factor(attack_samples);
    }

    void set_release_coeff(sample_type release_samples)
    {
        release_coef_ = decay_factor(release_samples);
    }

    template <typename input_buffer_type, typename sample_count_t>
    inline void perform(input_buffer_type const & in, sample_count_t n)
    {
        const sample_type * in_sample = get_samples(in);

        sample_type envelope = envelope_;
        const sample_type attack_coef = attack_coef_;
        const sample_type release_coef = release_coef_;

        do
        {
            envelope = compute_sample(*in_sample++, envelope, attack_coef, release_coef);
            --n;
        }
        while(n);
        envelope_ = envelope;
    }

    sample_type current_envelope(void) const
    {
        return envelope_;
    }
};

} /* namespace nova */

#endif /* DSP_ENV_FOLLOWER_HPP */
