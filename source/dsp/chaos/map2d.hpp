//  chaotic generators, twodimensional maps
//
//  Copyright (C) 2008, 2009 Tim Blechmann
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

#ifndef CHAOS_MAP2D_HPP
#define CHAOS_MAP2D_HPP

#include "chaos.hpp"

namespace nova
{
namespace chaos
{

/** \brief coupled logistic map
 *  coupled_logistic map: x[n+1] = r * x[n] * (1 - x[n]) + e * (y[n] - x[n])
 *                        y[n+1] = r * y[n] * (1 - y[n]) + e * (x[n] - y[n])
 *                        0 <= r <= 4
 *  taken from Willi-Hans Steeb: Chaos and Fractals
 */
template <typename sample_type_arg,
          typename internal_type = sample_type_arg>
class coupled_logistic_map:
    chaos_generator_base<internal_type, 2, 2>
{
    NOVA_CHAOS_GENERATOR_HEADER(coupled_logistic_map, 2, 2);

    coupled_logistic_map(internal_type const & init_e = 0.06,
                         internal_type const & init_r = 3.7)
    {
        generator_base::set(0, generate_pos_white_noise<internal_type>(1));
        generator_base::set(1, generate_pos_white_noise<internal_type>(1));
        set_e(init_e);
        set_r(init_r);
    }

    NOVA_CHAOS_PARAMETER_SETTER_SIMPLE(0, e)
    NOVA_CHAOS_PARAMETER_SETTER_CLIPPING(1, r, 0, 4)

    static always_inline void compute_sample(state_copy & state)
    {
        const internal_type x = state.get(0);
        const internal_type y = state.get(1);

        const internal_type e = state.get_parameter(0);
        const internal_type r = state.get_parameter(1);

        internal_type new_x = r * x * (1 - x) + e * (y - x);
        internal_type new_y = r * y * (1 - y) + e * (x - y);

        if (unlikely(std::abs(new_x - new_y) < 1e-15))
        {
            new_x += generate_pos_white_noise<internal_type>(0.2);
            new_y += generate_pos_white_noise<internal_type>(0.2);
        }
        else
        {
            if (unlikely(new_x <= 0 or new_x >= 1))
                new_x = generate_pos_white_noise<internal_type>(0.08);

            if (unlikely(new_y <= 0 or new_y >= 1))
                new_y = generate_pos_white_noise<internal_type>(0.08);
        }

        state.set(0, new_x);
        state.set(1, new_y);
    }
};

/** \brief latoocarfian model
 *  latoocarfian model: x1[n+1] = sin(x2[n]*b) + c*sin(x1[n]*b)
 *                      x2[n+1] = sin(x1[n]*a) + d*sin(x2[n]*a)
 *                      -3 < a,b < 3
 *                      -0.5 < c,d < 1.5
 *  taken from Pickover: Chaos In Wonderland
 */
template <typename sample_type_arg,
          typename internal_type = sample_type_arg>
class latoocarfian:
    chaos_generator_base<internal_type, 2, 4>
{
    NOVA_CHAOS_GENERATOR_HEADER(latoocarfian, 2, 4);


    latoocarfian(internal_type const & a = 0.1,
                 internal_type const & b = 0.2,
                 internal_type const & c = 0.3,
                 internal_type const & d = 0.4)
    {
        generator_base::set(0, 0.2);
        generator_base::set(1, 0.2);
        set_a(a);
        set_b(b);
        set_c(c);
        set_d(d);
    }

    NOVA_CHAOS_PARAMETER_SETTER_CLIPPING(0, a, -3, 3)
    NOVA_CHAOS_PARAMETER_SETTER_CLIPPING(1, b, -3, 3)
    NOVA_CHAOS_PARAMETER_SETTER_CLIPPING(2, c, -0.5, 1.5)
    NOVA_CHAOS_PARAMETER_SETTER_CLIPPING(3, d, -0.5, 1.5)

    static always_inline void compute_sample(state_copy & state)
    {
        const internal_type x1 = state.get(0);
        const internal_type x2 = state.get(1);

        const internal_type a = state.get_parameter(0);
        const internal_type b = state.get_parameter(1);
        const internal_type c = state.get_parameter(2);
        const internal_type d = state.get_parameter(3);

        using std::sin;
        const internal_type new_x1 = sin(x2 * b) + c*sin(x1*b);
        const internal_type new_x2 = sin(x1 * a) + d*sin(x2*a);

        state.set(0, new_x1);
        state.set(1, new_x2);
    }
};

/** \brief latoocarfian model, mutation alpha
 *         x1[n+1] = sin(x2[n]*b) + pow(sin(x1[n]*b),2) + pow(sin(x1[n]*b),3)
 *         x2[n+1] = sin(x1[n]*a) + pow(sin(x2[n]*a),2) + pow(sin(x2[n]*c),3)
 *  taken from Pickover: Chaos In Wonderland
 */
template <typename sample_type_arg,
          typename internal_type = sample_type_arg>
class latoomutalpha:
    chaos_generator_base<internal_type, 2, 3>
{
    NOVA_CHAOS_GENERATOR_HEADER(latoomutalpha, 2, 3);

    latoomutalpha(internal_type const & a = 0.1,
                  internal_type const & b = 0.2,
                  internal_type const & c = 0.3)
    {
        generator_base::set(0, 0.2);
        generator_base::set(1, 0.2);
        set_a(a);
        set_b(b);
        set_c(c);
    }

    NOVA_CHAOS_PARAMETER_SETTER_CLIPPING(0, a, -3, 3)
    NOVA_CHAOS_PARAMETER_SETTER_CLIPPING(1, b, -3, 3)
    NOVA_CHAOS_PARAMETER_SETTER_CLIPPING(2, c, -3, 3)

    static always_inline void compute_sample(state_copy & state)
    {
        const internal_type x1 = state.get(0);
        const internal_type x2 = state.get(1);

        const internal_type a = state.get_parameter(0);
        const internal_type b = state.get_parameter(1);
        const internal_type c = state.get_parameter(2);

        using std::sin;
        const internal_type new_x1 = sin(x2 * b) + pow2( sin(x1*b) ) + pow3 (sin(x1 * b));
        const internal_type new_x2 = sin(x1 * a) + pow2( sin(x2*a) ) + pow3 (sin(x2 * c));

        state.set(0, new_x1);
        state.set(1, new_x2);
    }
};

/** \brief latoocarfian model, mutation beta
 *         x1[n+1] = sin(x2[n]*b) + pow(sin(x1[n]*b),2)
 *         x2[n+1] = sin(x1[n]*a) + pow(sin(x2[n]*a),2)
 *  taken from Pickover: Chaos In Wonderland
 */
template <typename sample_type_arg,
          typename internal_type = sample_type_arg>
class latoomutbeta:
    chaos_generator_base<internal_type, 2, 2>
{
    NOVA_CHAOS_GENERATOR_HEADER(latoomutbeta, 2, 2);

    latoomutbeta(internal_type const & a = 0.1,
                 internal_type const & b = 0.2)
    {
        generator_base::set(0, 0.2);
        generator_base::set(1, 0.2);
        set_a(a);
        set_b(b);
    }

    NOVA_CHAOS_PARAMETER_SETTER_CLIPPING(0, a, -3, 3)
    NOVA_CHAOS_PARAMETER_SETTER_CLIPPING(1, b, -3, 3)

    static always_inline void compute_sample(state_copy & state)
    {
        const internal_type x1 = state.get(0);
        const internal_type x2 = state.get(1);

        const internal_type a = state.get_parameter(0);
        const internal_type b = state.get_parameter(1);

        using std::sin;
        internal_type new_x1 = sin(x2 * b) + pow2( sin(x1*b) );
        internal_type new_x2 = sin(x1 * a) + pow2( sin(x2*a) );

        if (unlikely(new_x1 == 0 and new_x2 == 0))
        {
            new_x1 = generate_white_noise<internal_type>();
            new_x2 = generate_white_noise<internal_type>();
        }

        state.set(0, new_x1);
        state.set(1, new_x2);
    }
};

/** \brief latoocarfian model, mutation gamma
 *         x1[n+1] = abs(sin(x2[n]*b)) + pow(sin(x1[n]*b),2)
 *         x2[n+1] = abs(sin(x1[n]*a)) + pow(sin(x2[n]*a),2)
 *  taken from Pickover: Chaos In Wonderland
 */
template <typename sample_type_arg,
          typename internal_type = sample_type_arg>
class latoomutgamma:
    chaos_generator_base<internal_type, 2, 2>
{
    NOVA_CHAOS_GENERATOR_HEADER(latoomutgamma, 2, 2);

    latoomutgamma(internal_type const & a = 0.1,
                  internal_type const & b = 0.2)
    {
        generator_base::set(0, 0.2);
        generator_base::set(1, 0.2);
        set_a(a);
        set_b(b);
    }

    NOVA_CHAOS_PARAMETER_SETTER_CLIPPING(0, a, -3, 3)
    NOVA_CHAOS_PARAMETER_SETTER_CLIPPING(1, b, -3, 3)

    static always_inline void compute_sample(state_copy & state)
    {
        const internal_type x1 = state.get(0);
        const internal_type x2 = state.get(1);

        const internal_type a = state.get_parameter(0);
        const internal_type b = state.get_parameter(1);

        using namespace std;
        internal_type new_x1 = abs(sin(x2 * b)) + pow2( sin(x1*b) );
        internal_type new_x2 = abs(sin(x1 * a)) + pow2( sin(x2*a) );

        if (unlikely(new_x1 == 0 and new_x2 == 0))
        {
            new_x1 = generate_white_noise<internal_type>();
            new_x2 = generate_white_noise<internal_type>();
        }

        state.set(0, new_x1);
        state.set(1, new_x2);
    }
};

/** \brief circle map (2d):
 *  x1[n+1] = x1[n] + c + sin (2 * pi * x2[n]) / (2*pi)
 *  x2[n+1] = b * x2[n] - a * sin (2 * pi * x1[n])
 *
 *  taken from Viktor Avrutin: AnT-Demos-4.669
 *
 */
template <typename sample_type_arg,
          typename internal_type = sample_type_arg>
class circle_map_2d:
    chaos_generator_base<internal_type, 2, 3>
{
    NOVA_CHAOS_GENERATOR_HEADER(circle_map_2d, 2, 3);

    circle_map_2d(internal_type const & a = 0.1,
                  internal_type const & b = 0.2,
                  internal_type const & c = 0.3)
    {
        generator_base::set(0, 0.2);
        generator_base::set(1, 0.2);
        set_a(a);
        set_b(b);
        set_c(b);
    }

    NOVA_CHAOS_PARAMETER_SETTER_SIMPLE(0, a)
    NOVA_CHAOS_PARAMETER_SETTER_SIMPLE(1, b)
    NOVA_CHAOS_PARAMETER_SETTER_SIMPLE(2, c)

    static always_inline void compute_sample(state_copy & state)
    {
        const internal_type x1 = state.get(0);
        const internal_type x2 = state.get(1);

        const internal_type a = state.get_parameter(0);
        const internal_type b = state.get_parameter(1);
        const internal_type c = state.get_parameter(2);

        static const internal_type pi = boost::math::constants::pi<internal_type>();
        static const internal_type two_pi = 2 * pi;
        static const internal_type one_div_two_pi = 2 * pi;

        using std::sin;
        internal_type new_x1 = x1 + c + sin(two_pi * x1) * one_div_two_pi;
        internal_type new_x2 = b * x2 - a * sin(two_pi * x1);

        state.set(0, new_x1);
        state.set(1, new_x2);
    }
};

/** \brief duffing map
 *         x1[n+1] = -b*x2[n] + a*x1[n] - pow(x1,3)
 *         x2[n+1] = x1[n]
 *
 *  taken from Viktor Avrutin: AnT-Demos-4.669
 */
template <typename sample_type_arg,
          typename internal_type = sample_type_arg>
class duffing_map:
    chaos_generator_base<internal_type, 2, 2, 1>
{
    NOVA_CHAOS_GENERATOR_HEADER_OUTDIMS(duffing_map, 2, 2, 1);

    duffing_map(internal_type const & a = 0.1,
                internal_type const & b = 0.2)
    {
        generator_base::set(0, 0.2);
        generator_base::set(1, 0.2);
        set_a(a);
        set_b(b);
    }

    NOVA_CHAOS_PARAMETER_SETTER_SIMPLE(0, a)
    NOVA_CHAOS_PARAMETER_SETTER_SIMPLE(1, b)

    static always_inline void compute_sample(state_copy & state)
    {
        const internal_type x1 = state.get(0);
        const internal_type x2 = state.get(1);

        const internal_type a = state.get_parameter(0);
        const internal_type b = state.get_parameter(1);

        internal_type new_x1 = -b * x2 + a * x1 - pow3(x1);
        internal_type new_x2 = x1;

        if (unlikely(new_x1 == 0 and new_x2 == 0))
        {
            new_x1 = generate_white_noise<internal_type>();
            new_x2 = generate_white_noise<internal_type>();
        }

        state.set(0, new_x1);
        state.set(1, new_x2);
    }
};

/** \brief lozi map
 *         x1[n+1] = 1 - abs(alpha * x1[n]) + y2[n]
 *         x2[n+1] = beta * x1[n]
 *
 *  taken from http://mathworld.wolfram.com/LoziMap.html
 *
 */
template <typename sample_type_arg,
          typename internal_type = sample_type_arg>
class lozi_map:
    chaos_generator_base<internal_type, 2, 2, 1>
{
    NOVA_CHAOS_GENERATOR_HEADER_OUTDIMS(lozi_map, 2, 2, 1);

    lozi_map(internal_type const & alpha = 1.4,
             internal_type const & beta = 0.3)
    {
        generator_base::set(0, 0.2);
        generator_base::set(1, 0.2);
        set_alpha(alpha);
        set_beta(beta);
    }

    NOVA_CHAOS_PARAMETER_SETTER_SIMPLE(0, alpha)
    NOVA_CHAOS_PARAMETER_SETTER_SIMPLE(1, beta)

    static always_inline void compute_sample(state_copy & state)
    {
        const internal_type x1 = state.get(0);
        const internal_type x2 = state.get(1);

        const internal_type alpha = state.get_parameter(0);
        const internal_type beta = state.get_parameter(1);

        internal_type new_x1 = 1 - std::abs(alpha * x1) + x2;
        internal_type new_x2 = beta * x1;

        state.set(0, new_x1);
        state.set(1, new_x2);
    }
};

/** \brief extended lozi map
 *         x1[n+1] = 1 - alpha * pow(abs(x1[n]), gamma) + x2[n]
 *         x2[n+1] = beta * x1[n]
 *
 *  taken from Viktor Avrutin: AnT-Demos-4.669
 *
 */
template <typename sample_type_arg,
          typename internal_type = sample_type_arg>
class extended_lozi_map:
    chaos_generator_base<internal_type, 2, 3, 1>
{
    NOVA_CHAOS_GENERATOR_HEADER_OUTDIMS(extended_lozi_map, 2, 3, 1);

    extended_lozi_map(internal_type const & alpha = 1.4,
                      internal_type const & beta = 0.3,
                      internal_type const & gamma = 1)
    {
        generator_base::set(0, 0.2);
        generator_base::set(1, 0.2);
        set_alpha(alpha);
        set_beta(beta);
        set_gamma(gamma);
    }

    NOVA_CHAOS_PARAMETER_SETTER_SIMPLE(0, alpha)
    NOVA_CHAOS_PARAMETER_SETTER_SIMPLE(1, beta)
    NOVA_CHAOS_PARAMETER_SETTER_SIMPLE(2, gamma)

    static always_inline void compute_sample(state_copy & state)
    {
        const internal_type x1 = state.get(0);
        const internal_type x2 = state.get(1);

        const internal_type alpha = state.get_parameter(0);
        const internal_type beta = state.get_parameter(1);
        const internal_type gamma = state.get_parameter(2);

        using namespace std;
        internal_type new_x1 = 1 - alpha * pow(abs(x1), gamma) + x2;
        internal_type new_x2 = beta * x1;

        state.set(0, new_x1);
        state.set(1, new_x2);
    }
};

/** \brief lozi-elhadj map
 *         x1[n+1] = 1 + alpha * (abs(x1[n]) - pow2(x2[n])) + x2[n]
 *         x2[n+1] = beta * x1[n]
 *
 *  taken from Zeraoulia Elhadj: A new chaotic attractor from 2d
 *  discrete mapping via border-collision period-doubling scenario
 *
 */
template <typename sample_type_arg,
          typename internal_type = sample_type_arg>
class lozi_elhadj_map:
    chaos_generator_base<internal_type, 2, 2, 1>
{
    NOVA_CHAOS_GENERATOR_HEADER_OUTDIMS(lozi_elhadj_map, 2, 2, 1);

    lozi_elhadj_map(internal_type const & alpha = -1.8,
                    internal_type const & beta = 0.5)
    {
        generator_base::set(0, 0.2);
        generator_base::set(1, 0.2);
        set_alpha(alpha);
        set_beta(beta);
    }

    NOVA_CHAOS_PARAMETER_SETTER_SIMPLE(0, alpha)
    NOVA_CHAOS_PARAMETER_SETTER_SIMPLE(1, beta)

    static always_inline void compute_sample(state_copy & state)
    {
        const internal_type x1 = state.get(0);
        const internal_type x2 = state.get(1);

        const internal_type alpha = state.get_parameter(0);
        const internal_type beta = state.get_parameter(1);

        using namespace std;
        internal_type new_x1 = 1 - alpha * (abs(x1) - pow2(x2)) + x2;
        internal_type new_x2 = beta * x1;

        state.set(0, new_x1);
        state.set(1, new_x2);
    }
};


/** \brief gingerbreadman map
 *         x1[n+1] = 1 - x2[n] + abs(x1[n])
 *         x2[n+1] = x1[n]
 *
 *  taken from http://mathworld.wolfram.com/GingerbreadmanMap.html
 *
 */
template <typename sample_type_arg,
          typename internal_type = sample_type_arg>
class gingerbreadman_map:
    chaos_generator_base<internal_type, 2, 0, 1>
{
    NOVA_CHAOS_GENERATOR_HEADER_OUTDIMS(gingerbreadman_map, 2, 0, 1);

    gingerbreadman_map()
    {
        generator_base::set(0, 0.2);
        generator_base::set(1, 0.2);
    }

    static always_inline void compute_sample(state_copy & state)
    {
        const internal_type x1 = state.get(0);
        const internal_type x2 = state.get(1);

        internal_type new_x1 = 1 - x2 + std::abs(x1);
        internal_type new_x2 = x1;

        state.set(0, new_x1);
        state.set(1, new_x2);
    }
};

/** \brief henon map
 *         x1[n+1] = 1 - alpha * pow(x[n], 2) + y[n]
 *         x2[n+1] = beta * x[n]
 *
 *  taken from http://mathworld.wolfram.com/HenonMap.html
 *
 */
template <typename sample_type_arg,
          typename internal_type = sample_type_arg>
class henon_map:
    chaos_generator_base<internal_type, 2, 2, 1>
{
    NOVA_CHAOS_GENERATOR_HEADER_OUTDIMS(henon_map, 2, 2, 1);

    henon_map(internal_type const & alpha = 1.4,
              internal_type const & beta = 0.3)
    {
        generator_base::set(0, 0.2);
        generator_base::set(1, 0.2);
        set_alpha(alpha);
        set_beta(beta);
    }

    NOVA_CHAOS_PARAMETER_SETTER_SIMPLE(0, alpha)
    NOVA_CHAOS_PARAMETER_SETTER_SIMPLE(1, beta)

    static always_inline void compute_sample(state_copy & state)
    {
        const internal_type x1 = state.get(0);
        const internal_type x2 = state.get(1);

        const internal_type alpha = state.get_parameter(0);
        const internal_type beta = state.get_parameter(1);

        internal_type new_x1 = 1 - alpha * pow2(x1) + x2;
        internal_type new_x2 = beta * x1;

        state.set(0, new_x1);
        state.set(1, new_x2);
    }
};

/** \brief kaplan-yorke map
 *         x1[n+1] = 2 * x1[n] mod 1
 *         x2[n+1] = alpha * x2[n] + cos(4 * pi * x1[n])
 *
 *  taken from http://mathworld.wolfram.com/Kaplan-YorkeMap.html
 */
template <typename sample_type_arg,
          typename internal_type = sample_type_arg>
class kaplan_yorke_map:
    chaos_generator_base<internal_type, 2, 1>
{
    NOVA_CHAOS_GENERATOR_HEADER(kaplan_yorke_map, 2, 1);

    kaplan_yorke_map(internal_type const & alpha = 0.1)
    {
        generator_base::set(0, 0.2);
        generator_base::set(1, 0.2);
        set_alpha(alpha);
    }

    NOVA_CHAOS_PARAMETER_SETTER_SIMPLE(0, alpha)

    static always_inline void compute_sample(state_copy & state)
    {
        const internal_type x1 = state.get(0);
        const internal_type x2 = state.get(1);

        const internal_type alpha = state.get_parameter(0);

        static const internal_type four_pi = 4 * boost::math::constants::pi<internal_type>();

        internal_type new_x1 = std::fmod(2 * x1, 1);
        internal_type new_x2 = alpha * x2 + std::cos(four_pi * x1);

        state.set(0, new_x1);
        state.set(1, new_x2);
    }
};

/** \brief sinai map
 *         x1[n+1] = x1[n] + x2[n] + a * cos(2*pi * x2[n]) mod 1
 *         x2[n+1] = x1[n] + 2*x2[n] mod 1
 *
 *  taken from Viktor Avrutin: AnT-Demos-4.669
 *
 */
template <typename sample_type_arg,
          typename internal_type = sample_type_arg>
class sinai_map:
    chaos_generator_base<internal_type, 2, 1>
{
    NOVA_CHAOS_GENERATOR_HEADER(sinai_map, 2, 1);

    sinai_map(internal_type const & a = 0.1)
    {
        generator_base::set(0, 0.5);
        generator_base::set(1, 0.5);
        set_a(a);
    }

    NOVA_CHAOS_PARAMETER_SETTER_SIMPLE(0, a)

    static always_inline void compute_sample(state_copy & state)
    {
        const internal_type x1 = state.get(0);
        const internal_type x2 = state.get(1);

        const internal_type a = state.get_parameter(0);

        static const internal_type two_pi = 2 * boost::math::constants::pi<internal_type>();

        using namespace std;
        internal_type new_x1 = fmod(x1 + x2 + a * cos(two_pi * x2), 1);
        internal_type new_x2 = fmod(x1 + 2 * x2, 1);

        state.set(0, new_x1);
        state.set(1, new_x2);
    }
};

/** \brief tinkerbell map
 *         x1[n+1] = pow(x1[n], 2) - pow(x2[n], 2) + a*x1 + b*x2
 *         x2[n+1] = 2*x1[n]*x2[n] + c*x1[n] + d*x2[n]
 *
 *  taken from Viktor Avrutin: AnT-Demos-4.669
 */
template <typename sample_type_arg,
          typename internal_type = sample_type_arg>
class tinkerbell_map:
    chaos_generator_base<internal_type, 2, 4>
{
    NOVA_CHAOS_GENERATOR_HEADER(tinkerbell_map, 2, 4);


    tinkerbell_map(internal_type const & a = 0.3,
                   internal_type const & b = 0.6,
                   internal_type const & c = 2,
                   internal_type const & d = 0.27)
    {
        generator_base::set(0, 0.2);
        generator_base::set(1, 0.2);
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
        const internal_type x1 = state.get(0);
        const internal_type x2 = state.get(1);

        const internal_type a = state.get_parameter(0);
        const internal_type b = state.get_parameter(1);
        const internal_type c = state.get_parameter(2);
        const internal_type d = state.get_parameter(3);

        using std::sin;
        const internal_type new_x1 = pow2(x1) - pow2(x2) + a*x1 + b*x2;
        const internal_type new_x2 = 2 * x1 * x2 + c*x1 + d*x2;

        state.set(0, new_x1);
        state.set(1, new_x2);
    }
};

/** \brief baker's map
 *
 *  taken from http://en.wikipedia.org/wiki/Baker%27s_map
 *
 */
template <typename sample_type_arg,
          typename internal_type = sample_type_arg>
class backers_map:
    chaos_generator_base<internal_type, 2, 0>
{
    NOVA_CHAOS_GENERATOR_HEADER(backers_map, 2, 0);

    backers_map()
    {
        generator_base::set(0, 0.2);
        generator_base::set(1, 0.2);
    }

    NOVA_CHAOS_PARAMETER_SETTER_SIMPLE(0, a)
    NOVA_CHAOS_PARAMETER_SETTER_SIMPLE(1, b)
    NOVA_CHAOS_PARAMETER_SETTER_SIMPLE(2, c)
    NOVA_CHAOS_PARAMETER_SETTER_SIMPLE(3, d)

    static always_inline void compute_sample(state_copy & state)
    {
        const internal_type x1 = state.get(0);
        const internal_type x2 = state.get(1);

        internal_type new_x1, new_x2;
        if (x1 <= 0.5)
        {
            new_x1 = 2*x1;
            new_x2 = 0.5 * x2;
        }
        else
        {
            new_x1 = 2 - 2*x1;
            new_x2 = 1 - 0.5*x2;
        }

        state.set(0, new_x1);
        state.set(1, new_x2);
    }
};

/** \brief standard map
 *         I[n+1] = I[n] + k * sin(theta[n])
 *         theta[n+1] = theta[n] + I[n] + k * sin(theta[n])
 *
 *  taken from http://mathworld.wolfram.com/StandardMap.html
 */
template <typename sample_type_arg,
          typename internal_type = sample_type_arg>
class standard_map:
    chaos_generator_base<internal_type, 2, 1>
{
    NOVA_CHAOS_GENERATOR_HEADER(standard_map, 2, 1);

    standard_map(internal_type const & k = 0.3)
    {
        generator_base::set(0, 0.2);
        generator_base::set(1, 0.2);
        set_k(k);
    }

    void set_k(internal_type k)
    {
        if (k < 0)
            k = 0;
        generator_base::set_parameter(0, k);
    }

    static always_inline void compute_sample(state_copy & state)
    {
        const internal_type i = state.get(0);
        const internal_type theta = state.get(1);

        const internal_type k = state.get_parameter(0);

        static const internal_type two_pi = 2 * boost::math::constants::pi<internal_type>();

        using namespace std;
        internal_type new_i = fmod(i + k * std::sin(theta), two_pi);
        internal_type new_theta = fmod(theta + new_i, two_pi);

        state.set(0, new_i);
        state.set(1, new_theta);
    }
};

/** \brief ikeda map
 *         x1[n+1] = 1 + u * (x1[n] * cos(t[n]) - x2[n] * sin(t[n]))
 *         x2[n+1] = u * (x1[n] * sin(t[n]) + x2[n] * cos(t[n]))
 *
 *         with t[n] = 0.4 - 6/(1 + pow(x1[n], 2) + pow(x2[n], 2))
 *
 *  taken from http://en.wikipedia.org/wiki/Ikeda_map
 */
template <typename sample_type_arg,
          typename internal_type = sample_type_arg>
class ikeda_map:
    chaos_generator_base<internal_type, 2, 1>
{
    NOVA_CHAOS_GENERATOR_HEADER(ikeda_map, 2, 1);

    ikeda_map(internal_type const & u = 0.3)
    {
        generator_base::set(0, 0.2);
        generator_base::set(1, 0.2);
        set_u(u);
    }

    NOVA_CHAOS_PARAMETER_SETTER_SIMPLE(0, u)

    static always_inline void compute_sample(state_copy & state)
    {
        const internal_type x1 = state.get(0);
        const internal_type x2 = state.get(1);

        const internal_type u = state.get_parameter(0);

        using namespace std;
        const internal_type t = 0.4 - 6/(1 + pow2(x1) + pow2(x2));

        internal_type new_x1 = 1 + u * (x1 * cos(t) - x2 * sin(t));
        internal_type new_x2 = u * (x1 * sin(t) + x2 * cos(t));

        state.set(0, new_x1);
        state.set(1, new_x2);
    }
};

/** \brief zaslavskii map
 *         x1[n+1] = (x1[n] + nu * (1 + mu * x2[n]) + epsilon * mu * nu * cos(2*pi * x1[n]) mod 1
 *         x2[n+1] = exp(-gamma) * (x2[n] + epsilon * cos (2*pi * x1[n]));
 *
 *         with mu = (1 - exp(-gamma))/gamma
 *
 *  taken from http://en.wikipedia.org/wiki/Zaslavskii_map
 */
template <typename sample_type_arg,
          typename internal_type = sample_type_arg>
class zaslavskii_map:
    chaos_generator_base<internal_type, 2, 4>
{
    NOVA_CHAOS_GENERATOR_HEADER(zaslavskii_map, 2, 4);

    zaslavskii_map(internal_type const & nu = 400.0/3,
                   internal_type const & gamma = 3.0,
                   internal_type const & epsilon = 0.3)
    {
        generator_base::set(0, 0.2);
        generator_base::set(1, 0.2);

        set_nu(nu);
        set_gamma(gamma);
        set_epsilon(epsilon);
    }

    NOVA_CHAOS_PARAMETER_SETTER_SIMPLE(0, nu)
    NOVA_CHAOS_PARAMETER_SETTER_SIMPLE(1, gamma)
    NOVA_CHAOS_PARAMETER_SETTER_SIMPLE(2, epsilon)

    static always_inline void compute_sample(state_copy & state)
    {
        const internal_type x1 = state.get(0);
        const internal_type x2 = state.get(1);

        const internal_type nu = state.get_parameter(0);
        const internal_type gamma = state.get_parameter(1);
        const internal_type epsilon = state.get_parameter(2);

        using namespace std;

        static const internal_type two_pi = 2 * boost::math::constants::pi<internal_type>();
        const internal_type cos_twopi_x1 = cos (two_pi * x1);

        const internal_type exp_m_gamma = exp(-gamma);
        const internal_type mu = (1 - exp_m_gamma) / gamma;

        internal_type new_x1 = std::fmod (x1 + nu * (1 + mu*x2) + epsilon * mu * nu * cos_twopi_x1, 1);
        internal_type new_x2 = exp_m_gamma  * (x2 + epsilon * cos_twopi_x1);

        state.set(0, new_x1);
        state.set(1, new_x2);
    }
};


} /* namespace chaos */
} /* namespace nova */

#endif /* CHAOS_MAP2D_HPP */
