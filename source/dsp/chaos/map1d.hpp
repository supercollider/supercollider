//  chaotic generators, onedimensional maps
//
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

#ifndef CHAOS_MAP1D_HPP
#define CHAOS_MAP1D_HPP

#include "chaos.hpp"

namespace nova
{
namespace chaos
{

/** \brief logistic map
 *   logistic map: x[n+1] = alpha * x[n] * (1 - x[n])
 *                 0 < x[n] <  1
 *                 0 <= alpha <= 4
 */
template <typename sample_type_arg,
          typename internal_type = sample_type_arg>
class logistic_map:
    chaos_generator_base<internal_type, 1, 1>
{
    NOVA_CHAOS_GENERATOR_HEADER(logistic_map, 1, 1);

    logistic_map(internal_type const & init_alpha = 0):
        generator_base(0.4)
    {
        set_alpha(init_alpha);
    }

    NOVA_CHAOS_PARAMETER_SETTER_CLIPPING(0, alpha, 0, 4)

    static always_inline void compute_sample(state_copy & state)
    {
        internal_type x = state.get(0);
        const internal_type alpha = state.get_parameter(0);
        x = alpha * x * (internal_type(1) - x);
        state.clip_and_set(0, x, 1e-10, 0.99999999999);
    }
};


/** \brief delayed logistic map
 *         x[n+1] = alpha * x[n] * (1 - x[n-1])
 *         0 < x[n] <  1
 *         0 <= alpha <= 4
 *
 *  taken from E. Atlee Jackson: Perspective of nonlinear dynamics (Vol. 2)
 */

template <typename sample_type_arg,
          typename internal_type = sample_type_arg>
class delayed_logistic_map:
    chaos_generator_base<internal_type, 2, 1, 1>
{
    NOVA_CHAOS_GENERATOR_HEADER_OUTDIMS(delayed_logistic_map, 2, 1, 1);

    delayed_logistic_map(internal_type const & init_alpha = 0):
        generator_base(0.4)
    {
        set_alpha(init_alpha);
    }

    NOVA_CHAOS_PARAMETER_SETTER_CLIPPING(0, alpha, 0, 4)

    static always_inline void compute_sample(state_copy & state)
    {
        internal_type x = state.get(0);
        internal_type x1 = state.get(1);
        const internal_type alpha = state.get_parameter(0);
        x = alpha * x * (internal_type(1) - x1);
        state.clip_and_set(0, x, 1e-10, 0.99999999999);
        state.set(1, x1);
    }
};


/** \brief bernoulli map
 *         x[n+1] = 2 * x[n] mod 1
 *         0 <= x[n] <  1
 *  taken from Willi-Hans Steeb: Chaos and Fractals
 *
 *  also known as dyadic map or 2x-mod-1 map
 *
 */
template <typename sample_type_arg,
          typename internal_type = sample_type_arg>
class bernoulli_map:
    chaos_generator_base<internal_type, 1, 0>
{
    NOVA_CHAOS_GENERATOR_HEADER(bernoulli_map, 1, 0);

    bernoulli_map(void):
        generator_base(0.4)
    {}

    static always_inline void compute_sample(state_copy & state)
    {
        const internal_type x = state.get(0);
        internal_type new_x = std::fmod(2*x, 1);

        assert(new_x >= 0 and new_x < 1);
        state.set(0, new_x);
    }
};

/** \brief tent map
 *         x[n+1] = 1 - 2*abs(x[n])
 *         -1 < x[n] < 1
 *  taken from Julien C. Sprott, Strange Attractors: Creating Patterns in Chaos
 */
template <typename sample_type_arg,
          typename internal_type = sample_type_arg>
class tent_map:
    chaos_generator_base<internal_type, 1, 0>
{
    NOVA_CHAOS_GENERATOR_HEADER(tent_map, 1, 0);

    tent_map(void):
        generator_base(0.4)
    {}

    static always_inline void compute_sample(state_copy & state)
    {
        const internal_type x = state.get(0);
        internal_type new_x = internal_type(1) - 2 * std::abs(x);
        assert(new_x > -1 and new_x < 1);
        state.set(0, new_x);
    }
};

/** \brief gauss map
 *         x[n+1] = fmod(1 / x[n], 1) (else)
 *         0 <= x[n] <  1
 *  taken from Willi-Hans Steeb: Chaos and Fractals
 */
template <typename sample_type_arg,
          typename internal_type = sample_type_arg>
class gauss_map:
    chaos_generator_base<internal_type, 1, 0>
{
    NOVA_CHAOS_GENERATOR_HEADER(gauss_map, 1, 0);

    gauss_map(void):
        generator_base(0.4)
    {}

    static always_inline void compute_sample(state_copy & state)
    {
        internal_type x = state.get(0);

        if (unlikely (x == 0))
            x = generate_pos_white_noise<internal_type>(0.001);

        internal_type new_x = std::fmod (1/x, 1);

        assert(new_x >= 0 and new_x < 1);
        state.set(0, new_x);
    }
};

/** \brief bungalow tent map:
 *         x[n+1] = 1 + 2 * r + 2 * (r + 1) * x[n]     (for -1 <= x[n] < -0.5f)
 *                  1 + 2 * (1 - r) * (x[n])           (for -0.5 <= x[n] < 0)
 *                  1 + 2 * (r - 1) * (x[n])           (for 0 <= x[n] < 0.5)
 *                  1 + 2 * r - 2 * (r + 1) * (x[n])   (for 0.5 <= x[n] < 1)
 *                              -1 <= x[n] <  1
 *                              -0.5 <= r < 1
 *  taken from Willi-Hans Steeb: Chaos and Fractals
 */
template <typename sample_type_arg,
          typename internal_type = sample_type_arg>
class bungalow_tent_map:
    chaos_generator_base<internal_type, 1, 1>
{
    NOVA_CHAOS_GENERATOR_HEADER(bungalow_tent_map, 1, 1);

    bungalow_tent_map(internal_type r = 0.012345):
        generator_base(0.4)
    {
        set_r(0.012345);
    }

    NOVA_CHAOS_PARAMETER_SETTER_CLIPPING(0, r, -0.5, 1)

    static always_inline void compute_sample(state_copy & state)
    {
        internal_type x = state.get(0);
        internal_type r = state.get_parameter(0);

        internal_type new_x;
        if (x < -0.5)
            new_x = 1 + 2 * r + 2 * (r + 1) * x;
        else if (x <= 0)
            new_x = 1 + 2 * (1 - r) * x;
        else if (x <= 0.5)
            new_x = 1 + 2 * (r - 1) * x;
        else
            new_x = 1 + 2 * r - 2 * (r + 1) * x;

        assert(new_x >= -1 and new_x < 1);
        state.set(0, new_x);
    }
};


/** \brief circle map:
 *  x[n+1] = x[n] + omega - k / (2*pi) * sin (2 * pi * x [n])
 *  taken from http://mathworld.wolfram.com/CircleMap.html
 *
 *  "Omega can be interpreted as an externally applied frequency, and K as a strength of nonlinearity"
 *
 */
template <typename sample_type_arg,
          typename internal_type = sample_type_arg>
class circle_map:
    chaos_generator_base<internal_type, 1, 2>
{
    NOVA_CHAOS_GENERATOR_HEADER(circle_map, 1, 2);

    circle_map(internal_type k = 0.12345, internal_type omega = 0.12345):
        generator_base(0.4)
    {
        set_omega(k);
        set_k(omega);
    }

    NOVA_CHAOS_PARAMETER_SETTER_CLIPPING(0, k, -0.5, 1)
    NOVA_CHAOS_PARAMETER_SETTER_CLIPPING(1, omega, -0.5, 1)

    static always_inline void compute_sample(state_copy & state)
    {
        internal_type x = state.get(0);

        const internal_type k = state.get_parameter(0);
        const internal_type omega = state.get_parameter(1);

        static const internal_type pi = boost::math::constants::pi<internal_type>();
        static const internal_type oned2pi = 1/(2*pi);

        const internal_type new_x = std::fmod(x + omega - k * oned2pi * std::sin(2*pi * x), 1);

        state.set(0, new_x);
    }
};


/** \brief piecewise linear map:
 *         x[n+1] = c*x[n] + d     (for x[n] < 0.5f)
 *                  a*x[n] - b     (for x[n] >= 0.5f)
 *
 *  taken from Viktor Avrutin: AnT-Demos-4.669
 */
template <typename sample_type_arg,
          typename internal_type = sample_type_arg>
class piecewise_linear_map:
    chaos_generator_base<internal_type, 1, 4>
{
    NOVA_CHAOS_GENERATOR_HEADER(piecewise_linear_map, 1, 4);

    piecewise_linear_map(internal_type a = 1, internal_type b = 0.2, internal_type c = 2, internal_type d = 0.4):
        generator_base(0.4)
    {
        set_a(a);
        set_b(b);
        set_c(c);
        set_d(d);
    }

    NOVA_CHAOS_PARAMETER_SETTER_SIMPLE(0, a)
    NOVA_CHAOS_PARAMETER_SETTER_SIMPLE(1, b)
    NOVA_CHAOS_PARAMETER_SETTER_SIMPLE(2, c)
    NOVA_CHAOS_PARAMETER_SETTER_SIMPLE(3, d)

    static always_inline void compute_sample(state_copy & state)
    {
        const internal_type x = state.get(0);

        const internal_type a = state.get_parameter(0);
        const internal_type b = state.get_parameter(1);
        const internal_type c = state.get_parameter(2);
        const internal_type d = state.get_parameter(3);

        internal_type new_x;
        if (x < -0.5)
            new_x = c * x + d;
        else
            new_x = a * x - b;

        state.set(0, new_x);
    }
};

/** \brief power law map
 *         x[n+1] = a / (0.5 * b) * ( pow(0.5, b) - pow(abs(x - 0.5), b))
 *
 *  taken from Viktor Avrutin: AnT-Demos-4.669
 */
template <typename sample_type_arg,
          typename internal_type = sample_type_arg>
class power_law_map:
    chaos_generator_base<internal_type, 1, 2>
{
    NOVA_CHAOS_GENERATOR_HEADER(power_law_map, 1, 2);

    power_law_map(internal_type a = 0.1, internal_type b = 0.1):
        generator_base(0.4)
    {
        set_a(a);
        set_b(b);
    }

    NOVA_CHAOS_PARAMETER_SETTER_SIMPLE(0, a)
    NOVA_CHAOS_PARAMETER_SETTER_SIMPLE(1, b)

    static always_inline void compute_sample(state_copy & state)
    {
        const internal_type x = state.get(0);

        const internal_type a = state.get_parameter(0);
        const internal_type b = state.get_parameter(1);

        using namespace std;
        const internal_type new_x = a / (0.5 * b) * ( pow(internal_type(0.5), b) -
                                                      pow(internal_type(abs(x - 0.5)), b));
        state.set(0, new_x);
    }
};

/** \brief shift map
 *         x[n+1] = a * x[n] mod 1
 *
 *  taken from Viktor Avrutin: AnT-Demos-4.669
 */
template <typename sample_type_arg,
          typename internal_type = sample_type_arg>
class shift_map:
    chaos_generator_base<internal_type, 1, 1>
{
    NOVA_CHAOS_GENERATOR_HEADER(shift_map, 1, 1);

    shift_map(internal_type const & a = 0.12345):
        generator_base(0.4)
    {
        set_a(a);
    }

    void set_a(internal_type const & a)
    {
        if (a != 0)
            generator_base::set_parameter(0, a);
    }

    static always_inline void compute_sample(state_copy & state)
    {
        const internal_type x = state.get(0);
        const internal_type a = state.get_parameter(0);

        const internal_type new_x = std::fmod (a * x, 1) + 1e-10; /* small offset to avoid value 0 */
        state.set(0, new_x);
    }
};

/** \brief sine map
 *         x[n+1] = 0.25 * a * sin(pi * x[n])
 *
 *  taken from Viktor Avrutin: AnT-Demos-4.669
 */
template <typename sample_type_arg,
          typename internal_type = sample_type_arg>
class sine_map:
    chaos_generator_base<internal_type, 1, 1>
{
    NOVA_CHAOS_GENERATOR_HEADER(sine_map, 1, 1);

    sine_map(internal_type const & a = 0.12345):
        generator_base(0.4)
    {
        set_a(a);
    }

    NOVA_CHAOS_PARAMETER_SETTER_SIMPLE(0, a)

    static always_inline void compute_sample(state_copy & state)
    {
        const internal_type x = state.get(0);
        const internal_type a = state.get_parameter(0);
        static const internal_type pi = boost::math::constants::pi<internal_type>();

        const internal_type new_x = 0.25 * a * std::sin (pi * x) + 1e-10; /* small offset to avoid value 0 */
        state.set(0, new_x);
    }
};

/** \brief agnesi map
 *         x[n+1] = alpha / (alpha + x*x)
 *
 *  taken from Viktor Avrutin: AnT-Demos-4.669
 */
template <typename sample_type_arg,
          typename internal_type = sample_type_arg>
class agnesi_map:
    chaos_generator_base<internal_type, 1, 1>
{
    NOVA_CHAOS_GENERATOR_HEADER(agnesi_map, 1, 1);

    agnesi_map(internal_type const & alpha = 1):
        generator_base(0.4)
    {
        set_alpha(alpha);
    }

    void set_alpha(internal_type const & alpha)
    {
        if (alpha != 0)
            generator_base::set_parameter(0, alpha);
    }

    static always_inline void compute_sample(state_copy & state)
    {
        const internal_type x = state.get(0);
        const internal_type alpha = state.get_parameter(0);

        internal_type denominator = alpha + x*x;

        if (unlikely(denominator == 0))
            denominator = 1;

        const internal_type new_x = alpha / denominator;

        state.set(0, new_x);
    }
};

/** \brief cusp map
 *         x[n+1] = 1 - (2 * sqrt(abs(x)))
 *
 *  taken from http://mathworld.wolfram.com/CuspMap.html
 */
template <typename sample_type_arg,
          typename internal_type = sample_type_arg>
class cusp_map:
    chaos_generator_base<internal_type, 1, 0>
{
    NOVA_CHAOS_GENERATOR_HEADER(cusp_map, 1, 0);

    cusp_map(internal_type const & alpha = 1):
        generator_base(0.4)
    {}

    static always_inline void compute_sample(state_copy & state)
    {
        const internal_type x = state.get(0);

        using namespace std;
        const internal_type new_x = 1 - (x * sqrt(abs(x)));

        state.set(0, new_x);
    }
};


} /* namespace chaos */
} /* namespace nova */

#endif /* CHAOS_MAP1D_HPP */
