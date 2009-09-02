//  minblep oscillators
//  Copyright (C) 2007, 2009 Tim Blechmann
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


#ifndef MINBLEP_HPP
#define MINBLEP_HPP

#include <deque>
#include <cmath>

#include <boost/math/constants/constants.hpp>

#include "utilities.hpp"
#include "sample_extractor.hpp"

namespace nova
{

/** templated version of minblep algorithm, adapted from Eli Brandt's matlab files
 *
 *  \todo: dc blocker (required for high frequencies)
 * */
template <typename sample_type,                 /* the external data type */
          typename internal_type = sample_type, /* the internally used type */
          unsigned int ktable = 64,             /* oversampling factor */
          unsigned int nzc = 16                 /* number of zero crossings */
          >
class minblep_saw
{
    BOOST_STATIC_ASSERT(boost::is_floating_point<sample_type>::value);
    BOOST_STATIC_ASSERT(boost::is_floating_point<internal_type>::value);

public:
    minblep_saw(void):
        phase_(0), phase_inc_(0)
    {}

    /* void set frequency for control-driven operation */
    void set_frequency(internal_type const & new_frequency)
    {
        phase_inc_ = new_frequency;
    }

    /* set inverse samplerate for signal-driven operation */
    void set_inv_samplerate(internal_type const & new_inv_samplerate)
    {
        inv_samplerate = new_inv_samplerate;
    }

    template <typename output_buffer_type>
    void perform(output_buffer_type const & out_buffer, uint n)
    {
        sample_type * out = get_samples(out_buffer);

        for (int i = 0; i != n; ++i)
            out[i] = compute_sample(phase_inc_);
    };

    template <typename input_buffer_type, typename output_buffer_type>
    inline void perform(input_buffer_type const & in_buffer, output_buffer_type & out_buffer, uint n)
    {
        const sample_type * in = get_samples(in_buffer);
        sample_type * out = get_samples(out_buffer);

        for (int i = 0; i != n; ++i)
        {
            internal_type phase_inc = inv_samplerate * internal_type(in[i]);
            out[i] = compute_sample(phase_inc_);
        }
    }


private:
    inline sample_type compute_sample(internal_type const & phase_inc)
    {
        // do normal phase increment.
        phase_ += phase_inc;

        // if ph has crossed threshold, wrap it and trigger a correction.
        if (phase_ >= 1)
        {
            phase_ -= 1;
            internal_type alpha = phase_ / phase_inc_;  // subsample fraction, [0, 1)
            mb_ofs.push_back(alpha);      // push alpha onto mb_ofs
        }

        // sum up active corrections.
        internal_type dph = 0;

        for (int j = 0; j != mb_ofs.size(); ++j)
        {
            internal_type mbi = ktable*mb_ofs[j]; // index into table
            dph += step_correction_lookup(mbi);
            mb_ofs[j] += 1; // increment offset
        }

        // purge inactive minbleps, if any.
        while (mb_ofs.size() and  (mb_ofs[0] >= 2*nzc))
        {
            mb_ofs.pop_front();
        }

        // generate output
        return 2*(phase_ + dph) - 1;
    }


    internal_type phase_;
    internal_type phase_inc_;
    internal_type inv_samplerate;
    std::deque<internal_type> mb_ofs; // active offsets (in samples, 0 to 2*nzc) within minblep table.

    static internal_type step_correction_lookup(internal_type index)
    {
        static internal_type * minblep = GenerateMinBLEP(nzc, ktable);
        static internal_type * step_correction = generate_step_correction(minblep);

        uint left_index = uint(std::floor(index));
        uint right_index = left_index + 1;
        internal_type fractional_index = index - std::floor(index);

        return nova::linearinterp(fractional_index, step_correction[left_index], step_correction[right_index]);
    }

    /* minblep table generation, adapted from Daniel Werner */

    static internal_type SINC(internal_type x)
    {
        if(x == 0.0f)
            return 1.0f;
        else
        {
            internal_type pix = boost::math::constants::pi< internal_type >() * x;
            return std::sin(pix) / pix;
        }
    }

    // Generate Blackman Window
    static void BlackmanWindow(int n, internal_type *w)
    {
        int m = n - 1;
        internal_type f1, f2, fm;

        fm = internal_type(m);
        for(int i = 0; i != m; i++)
        {
            f1 = (2.0f * boost::math::constants::pi< internal_type >() * internal_type(i)) / fm;
            f2 = 2.0f * f1;
            w[i] = 0.42f - (0.5f * std::cos(f1)) + (0.08f * std::cos(f2));
        }
    }

    // Discrete Fourier Transform
    static void DFT(int n, internal_type *realTime, internal_type *imagTime, internal_type *realFreq, internal_type *imagFreq)
    {
        int k, i;
        internal_type sr, si, p;

        for(k = 0; k < n; k++)
        {
            realFreq[k] = 0.0f;
            imagFreq[k] = 0.0f;
        }

        for(k = 0; k < n; k++)
            for(i = 0; i < n; i++)
            {
                p = (2.0f *  * (internal_type)(k * i)) / n;
                sr = std::cos(p);
                si = -std::sin(p);
                realFreq[k] += (realTime[i] * sr) - (imagTime[i] * si);
                imagFreq[k] += (realTime[i] * si) + (imagTime[i] * sr);
            }
    }

    // Inverse Discrete Fourier Transform
    static void InverseDFT(int n, internal_type *realTime, internal_type *imagTime, internal_type *realFreq, internal_type *imagFreq)
    {
        int k, i;
        internal_type sr, si, p;

        for(k = 0; k < n; k++)
        {
            realTime[k] = 0.0f;
            imagTime[k] = 0.0f;
        }

        for(k = 0; k < n; k++)
        {
            for(i = 0; i < n; i++)
            {
                p = (2.0f * boost::math::constants::pi< internal_type >() * (internal_type)(k * i)) / n;
                sr = std::cos(p);
                si = -std::sin(p);
                realTime[k] += (realFreq[i] * sr) + (imagFreq[i] * si);
                imagTime[k] += (realFreq[i] * si) - (imagFreq[i] * sr);
            }
            realTime[k] /= n;
            imagTime[k] /= n;
        }
    }

    // Complex Absolute Value
    static inline internal_type cabs(internal_type x, internal_type y)
    {
        return std::sqrt((x * x) + (y * y));
    }

    // Complex Exponential
    static inline void cexp(internal_type x, internal_type y, internal_type *zx, internal_type *zy)
    {
        internal_type expx;

        expx = std::exp(x);
        *zx = expx * std::cos(y);
        *zy = expx * std::sin(y);
    }

    // Compute Real Cepstrum Of Signal
    static void RealCepstrum(int n, internal_type *signal, internal_type *realCepstrum)
    {
        internal_type realTime[n], imagTime[n], realFreq[n], imagFreq[n];
        int i;

        // Compose Complex FFT Input

        for(i = 0; i < n; i++)
        {
            realTime[i] = signal[i];
            imagTime[i] = 0.0f;
        }

        // Perform DFT
        DFT(n, realTime, imagTime, realFreq, imagFreq);

        // Calculate Log Of Absolute Value
        for(i = 0; i < n; i++)
        {
            realFreq[i] = std::log(cabs(realFreq[i], imagFreq[i]));
            imagFreq[i] = 0.0f;
        }

        // Perform Inverse FFT
        InverseDFT(n, realTime, imagTime, realFreq, imagFreq);

        // Output Real Part Of FFT
        for(i = 0; i < n; i++)
            realCepstrum[i] = realTime[i];
    }

    // Compute Minimum Phase Reconstruction Of Signal
    static void MinimumPhase(int n, internal_type *realCepstrum, internal_type *minimumPhase)
    {
        int i, nd2;
        internal_type realTime[n], imagTime[n], realFreq[n], imagFreq[n];

        nd2 = n / 2;

        if((n % 2) == 1)
        {
            realTime[0] = realCepstrum[0];
            for(i = 1; i < nd2; i++)
                realTime[i] = 2.0f * realCepstrum[i];
            for(i = nd2; i < n; i++)
                realTime[i] = 0.0f;
        }
        else
        {
            realTime[0] = realCepstrum[0];
            for(i = 1; i < nd2; i++)
                realTime[i] = 2.0f * realCepstrum[i];
            realTime[nd2] = realCepstrum[nd2];
            for(i = nd2 + 1; i < n; i++)
                realTime[i] = 0.0f;
        }

        for(i = 0; i < n; i++)
            imagTime[i] = 0.0f;

        DFT(n, realTime, imagTime, realFreq, imagFreq);

        for(i = 0; i < n; i++)
            cexp(realFreq[i], imagFreq[i], &realFreq[i], &imagFreq[i]);

        InverseDFT(n, realTime, imagTime, realFreq, imagFreq);

        for(i = 0; i < n; i++)
            minimumPhase[i] = realTime[i];
    }

    // Generate MinBLEP And Return It In An Array Of Floating Point Values
    static internal_type *GenerateMinBLEP(int zeroCrossings, int overSampling)
    {
        int i, n;
        internal_type r, a, b;
        internal_type *minBLEP;

        n = (zeroCrossings * 2 * overSampling) + 1;
        internal_type buffer1[n];
        internal_type buffer2[n];

        // Generate Sinc
        a = (internal_type)-zeroCrossings;
        b = (internal_type)zeroCrossings;
        for(i = 0; i < n; i++)
        {
            r = ((internal_type)i) / ((internal_type)(n - 1));
            buffer1[i] = SINC(a + (r * (b - a)));
        }

        // Window Sinc
        BlackmanWindow(n, buffer2);
        for(i = 0; i < n; i++)
            buffer1[i] *= buffer2[i];

        // Minimum Phase Reconstruction
        RealCepstrum(n, buffer1, buffer2);
        MinimumPhase(n, buffer2, buffer1);

        // Integrate Into MinBLEP
        minBLEP = new internal_type[n];
        a = 0.0f;
        for(i = 0; i < n; i++)
        {
            a += buffer1[i];
            minBLEP[i] = a;
        }

        // Normalize
        a = minBLEP[n - 1];
        a = 1.0f / a;
        for(i = 0; i < n; i++)
            minBLEP[i] *= a;

        return minBLEP;
    }

    static internal_type * generate_step_correction(internal_type * minblep)
    {
        int n = (nzc * 2 * ktable) + 1;
        internal_type * step_correction = new internal_type[n];

        for (int i = 0; i != n; ++i)
            step_correction[i] = 1 - minblep[i];
        return step_correction;
    }
};

} /* namespace nova */

#endif /* MINBLEP_HPP */
