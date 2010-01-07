//  buffer classes
//  Copyright (C) 2006, 2007 Tim Blechmann
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


#ifndef _BUFFER_HPP
#define _BUFFER_HPP

#include "utilities.hpp"
#include "utilities/template_functions.hpp"
#include "utilities/utils.hpp"
#include "nova-simd/simd_memory.hpp"
#include "interpolation.hpp"
#include "server/sample_types.hpp"

namespace nova
{
class buffer_base:
    private noncopyable
{
public:
    /** \name Obtaining Samples
     *  Member functions to obtain samples
     */
    ///\{
    aligned_sample_ptr get_samples(void)
    {
        return buffer;
    }

    const_aligned_sample_ptr get_samples(void) const
    {
        return buffer;
    }

    sample get_sample(uint index) const
    {
        if (index < m_size)
            return get_sample_unchecked(index);
        else
            throw std::exception(/* "out of range" */);
    }

    sample operator[](uint index) const
    {
        return get_sample_unchecked(index);
    }

    sample get_sample_unchecked(uint index) const
    {
        assert(index < m_size);

        return buffer[index];
    }

    template <int n>
    void read_sampleblock_aligned(sample * dest, uint index) const
    {
        assert ((index < m_size) and (n < m_size));
        assert (index + n < m_size);
        copyvec_na_simd<n>(dest, buffer+index);
    }

    template <int n>
    void read_aligned_sampleblock(sample * dest, uint index) const
    {
        assert ((index < m_size) and (n < m_size));
        assert (index + n < m_size);
        copyvec_an_simd<n>(dest, buffer+index);
    }

    template <int n>
    void read_aligned_sampleblock_aligned(sample * dest, uint index) const
    {
        assert ((index < m_size) and (n < m_size));
        assert (index + n < m_size);
        copyvec_simd<n>(dest, buffer+index);
    }

    template <int n>
    void read_sampleblock(sample * dest, uint index) const
    {
        assert ((index < m_size) and (n < m_size));
        assert (index + n < m_size);
        copyvec_nn_simd<n>(dest, buffer+index);
    }

    void read_sampleblock_simd(sample * dest, uint index, uint n) const
    {
        assert ((index < m_size) and (n < m_size));
        assert (index + n < m_size);
        assert (n % 8 == 0);
        copyvec_nn_simd(dest, buffer+index, n);
    }

    void read_sampleblock(sample * dest, uint index, uint n) const
    {
        assert ((index < m_size) and (n < m_size));
        assert (index + n <= m_size);
        copyvec(dest, buffer+index, n);
    }

    template <typename F>
    sample get_sample_lin_interp(F index) const
    {
        BOOST_STATIC_ASSERT(boost::is_floating_point<F>::value);

        if (uint(index) < m_size-1)
            return get_sample_lin_interp_unchecked(index);
        else
            throw std::exception(/* "out of range" */);
    }

    template <typename F>
    sample get_sample_lin_interp_unchecked(F index) const
    {
        BOOST_STATIC_ASSERT(boost::is_floating_point<F>::value);
        assert(int(index) < int(m_size)-1);

        return linearinterp(buffer, index);
    }

    template <typename F>
    sample get_sample_cub_interp(F index) const
    {
        BOOST_STATIC_ASSERT(boost::is_floating_point<F>::value);

        if ((index < m_size-2)
            and (index >= 1))
            return get_sample_cub_interp_unchecked(index);
        else
            throw std::exception(/* "out of range" */);
    }

    template <typename F>
    sample get_sample_cub_interp_unchecked(F index) const
    {
        BOOST_STATIC_ASSERT(boost::is_floating_point<F>::value);
        assert((index < m_size-2)
               and (index >= 1));

        return cubicinterp(buffer, index);
    }
    ///\}

    /** \name Writing Samples
     *  Member functions to write samples
     */
    ///\{
    void set_sample(sample s, uint index)
    {
        assert(index < m_size);
        buffer[index] = s;
        dirty = true;
    }

    template <int n>
    void write_sampleblock_aligned(const sample * src, uint index)
    {
        assert ((index < m_size) and (n < m_size));
        assert (index + n < m_size);
        copyvec_na_simd<n>(buffer+index, src);
        dirty = true;
    }

    template <int n>
    void write_aligned_sampleblock_aligned(const sample * src, uint index)
    {
        assert ((index < m_size) and (n < m_size));
        assert (index + n < m_size);
        copyvec_simd<n>(buffer+index, src);
        dirty = true;
    }

    template <int n>
    void write_sampleblock(const sample * src, uint index)
    {
        assert ((index < m_size) and (n < m_size));
        assert (index + n <= m_size);
        copyvec_nn_simd<n>(buffer+index, src);
        dirty = true;
    }

    void write_sampleblock_simd(const sample * src, uint index, uint n)
    {
        assert ((index < m_size) and (n < m_size));
        assert (index + n <= m_size);
        assert (n % 8 == 0);
        copyvec_simd(buffer+index, src, n);
        dirty = true;
    }

    void write_sampleblock(const sample * src, uint index, uint n)
    {
        assert ((index < m_size) and (n < m_size));
        assert (index + n <= m_size);
        copyvec(buffer+index, src, n);
        dirty = true;
    }

    ///\}

    uint size(void) const
    {
        return m_size;
    }

    buffer_base(uint size = 0):
        buffer(0), m_size(size), dirty(true)
    {
        if (size)
            allocate(size);
    }

    ~buffer_base(void)
    {
        free();
    }

protected:
    /** \name Memory Management
     *  memory management functions
     */

    ///\{
    void allocate(uint size)
    {
        assert(buffer == 0);
        assert(size != 0);

        buffer = calloc_aligned<sample>(size);
        m_size = size;
    }

    void free(void)
    {
        assert(buffer);
        free_aligned(buffer);
        buffer = 0;
        m_size = 0;
    }

    /** \brief set buffer to a preallocated array */
    void set_array(sample * new_buffer, uint size)
    {
        assert(buffer == 0);

        buffer = new_buffer;
        m_size = size;
    }

public:
    void resize(uint size)
    {
        if (buffer == 0)
            allocate(size);
        else
        {
            sample * next = calloc_aligned<sample>(size);

            int copysamples = std::min(size, m_size);

            copyvec(next, buffer, copysamples);
            if (size > m_size)
                zerovec(next + size, m_size - size);
            free_aligned(buffer);
            buffer = next;
            m_size = size;
            dirty = true;
        }
    }
    ///\}


    /** \brief swap sample buffers, return old buffers */
    tuple<sample*, uint> swap_buffer(sample* new_buffer, uint newsize)
    {
        sample * old_buffer = buffer;
        uint oldsize = m_size;

        buffer = new_buffer;
        m_size = newsize;
        dirty = true;

        return tie(old_buffer, oldsize);
    }

protected:
    sample * buffer;

    uint m_size;
    bool dirty; ///< dirty flag
};

inline sample * get_samples (buffer_base & buffer)
{
    return buffer.get_samples();
}

inline const sample * get_samples (buffer_base const & buffer)
{
    return buffer.get_samples();
}


class ring_buffer:
    public buffer_base
{
public:
    ring_buffer(uint size = 0):
        buffer_base (size), head (0)
    {
    }

    /** \brief get head of delayline  */
    std::size_t get_head(void) const
    {
        return head;
    }

private:
    /** \brief handling of wrapping*/
    /* @{ */
    void write_wrapped_block(const sample * src, uint index, uint n)
    {
        assert(index + n >= m_size);

        int wrap = m_size - index;

        copyvec(buffer + index, src, wrap);
        copyvec(buffer, src + wrap, n - wrap);
    }

    void read_wrapped_block(sample * dest, uint index, uint n) const
    {
        assert(index + n >= m_size);

        int wrap = m_size - index;

        copyvec(dest, buffer + index, wrap);
        copyvec(dest + wrap, buffer, n - wrap);
    }
    /* @} */

    std::size_t head;           /**< playhead */

public:
    /** \brief read from ring_buffer */
    /* @{ */
    template <int n>
    void read_sampleblock_aligned(sample * dest, uint delta) const
    {
        uint index = wrap_index(delta);

        assert ((index < m_size) and (n < m_size));

        if (index + n <= m_size)
            buffer_base::read_sampleblock_aligned<n>(dest, index);
        else
            read_wrapped_block(dest, index, n);
    }

    template <int n>
    void read_aligned_sampleblock(sample * dest, uint delta) const
    {
        uint index = wrap_index(delta);

        assert ((index < m_size) and (n < m_size));

        if (index + n <= m_size)
            buffer_base::read_aligned_sampleblock<n>(dest, index);
        else
            read_wrapped_block(dest, index, n);
    }

    template <int n>
    void read_sampleblock(sample * dest, uint delta) const
    {
        uint index = wrap_index(delta);

        assert ((index < m_size) and (n < m_size));

        if (index + n <= m_size)
            buffer_base::read_sampleblock<n>(dest, index);
        else
            read_wrapped_block(dest, index, n);
    }

    void read_sampleblock_simd(sample * dest, uint delta, uint n) const
    {
        uint index = wrap_index(delta);

        assert ((index < m_size) and (n < m_size));
        assert(n % 8 == 0);

        if (index + n <= m_size)
            buffer_base::read_sampleblock_simd(dest, index, n);
        else
            read_wrapped_block(dest, index, n);
    }

    void read_sampleblock(sample * dest, uint delta, uint n) const
    {
        uint index = wrap_index(delta);

        assert ((index < m_size) and (n < m_size));

        if (index + n <= m_size)
            buffer_base::read_sampleblock(dest, index, n);
        else
            read_wrapped_block(dest, index, n);
    }
    /* @} */

public:
    /** \brief read from ring_buffer
     *
     */
    /* @{ */
    template <int n>
    void write_sampleblock_aligned(const sample * src, uint index)
    {
        assert ((index < m_size) and (n < m_size));
        assert(n % 8 == 0);

        if (index + n < m_size)
            buffer_base::write_sampleblock_aligned<n>(src, index);
        else
            write_wrapped_block(src, index, n);
        update_head(n);
    }

    template <int n>
    void write_sampleblock(const sample * src, uint index)
    {
        assert ((index < m_size) and (n < m_size));
        assert(n % 8 == 0);

        if (index + n <= m_size)
            buffer_base::write_sampleblock<n>(src, index);
        else
            write_wrapped_block(src, index, n);
        update_head(n);
    }

    void write_sampleblock_8(const sample * src, uint index, uint n)
    {
        assert ((index < m_size) and (n < m_size));
        assert(n % 8 == 0);

        if (index + n <= m_size)
            buffer_base::write_sampleblock_simd(src, index, n);
        else
            write_wrapped_block(src, index, n);
        update_head(n);
    }

    void write_sampleblock(const sample * src, uint index, uint n)
    {
        assert ((index < m_size) and (n < m_size));

        if (index + n <= m_size)
            buffer_base::write_sampleblock(src, index, n);
        else
            write_wrapped_block(src, index, n);
        update_head(n);
    }

    void write_sampleblock(const sample * src, uint n)
    {
        uint index = head;

        assert ((index < m_size) and (n < m_size));

        if (index + n <= m_size)
            buffer_base::write_sampleblock(src, index, n);
        else
            write_wrapped_block(src, index, n);
        update_head(n);
    }
    /* @} */

    /** \brief get the wrapped index */
    /* @{ */
    int wrap_index(int delta) const
    {
        int read_pos = head - delta;
        uint index = wrap_optimistic(read_pos, int(m_size));

        return index;
    }

    int wrap_index(uint delta) const
    {
        int read_pos = head - int(delta);
        uint index = wrap_optimistic(read_pos, int(m_size));

        return index;
    }

    template <typename F>
    F wrap_index(F delta) const
    {
        F read_pos = F(head) - delta;
        F index = wrap_optimistic(read_pos, F(m_size));

        return index;
    }
    /* @} */

private:
    inline void update_head(uint n)
    {
        dirty = true;

        uint new_head = head + n;
        while (new_head >= m_size)
            new_head -= m_size;
        head = new_head;
    }
};

} /* namespace nova */



#endif /* _BUFFER_HPP */
