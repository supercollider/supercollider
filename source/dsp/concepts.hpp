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


/* metaconcept for nova's dsp framework
 *
 * keywords are used as defined in RFC 2119
 *
 * */

/* output-only class */
struct nova_dsp_class_output<typename sample_type>
{
    /* output-only classes MUST provide at least one perform method */
    template <typename output_buffer_type, typename sample_count_t>
    inline void perform(output_buffer_type & out, [output_buffer_type & out2, ... ] sample_count_t n
                        [, optional arguments]);

    /* output-only classes SHOULD provide at least one perform method, supporting a muladd helper class */
    template <typename output_buffer_type, typename sample_count_t,
              typename muladd_helper>
    inline void perform(output_buffer_type & out, [output_buffer_type & out2, ... ] sample_count_t n,
                        [, optional arguments], muladd_helper & ma);

    /* output-only classes MAY provide an unrolled versions of the perform method */
    template <typename output_buffer_type, typename sample_count_t>
    inline void perform_8(output_buffer_type & out, [output_buffer_type & out2, ... ] sample_count_t n
                        [, optional arguments]);

    template <typename output_buffer_type, typename sample_count_t,
              typename muladd_helper>
    inline void perform_8(output_buffer_type & out, [output_buffer_type & out2, ... ] sample_count_t n,
                          [, optional arguments], muladd_helper & ma);
};



/* input-only class */
struct nova_dsp_class_input<typename sample_type>
{
    /* input-only classes MUST provide at least one perform method */
    template <typename input_buffer_type, typename sample_count_t>
    inline void perform(input_buffer_type const & in, [input_buffer_type const & in2, ... ]
                        [, optional arguments], sample_count_t n);

    /* input-only classes MAY provide an unrolled version of the perform method */
    template <typename input_buffer_type, typename sample_count_t>
    inline void perform_8(input_buffer_type const & in, [input_buffer_type const & in2, ... ]
                          [, optional arguments], sample_count_t n);
};


/* input/output class */
struct nova_dsp_class<typename sample_type>
{
    /* input/output classes MUST provide at least one perform method */
    template <typename input_buffer_type, typename output_buffer_type, typename sample_count_t>
    inline void perform(input_buffer_type const & in, [input_buffer_type const & in2, ...]
                        output_buffer_type & out, [output_buffer_type & out2, ... ] sample_count_t n,
                        [, optional arguments]);

    /* input/output classes SHOULD provide at least one perform method, supporting a muladd helper class */
    template <typename input_buffer_type, typename output_buffer_type, typename sample_count_t,
              typename muladd_helper>
    inline void perform(input_buffer_type const & in, [input_buffer_type const & in2, ...]
                        output_buffer_type & out, [output_buffer_type & out2, ... ] sample_count_t n,
                        [, optional arguments], muladd_helper & ma);

    /* input/output classes MAY provide an unrolled version of the perform method */
    template <typename input_buffer_type, typename output_buffer_type, typename sample_count_t>
    inline void perform_8(input_buffer_type const & in, [input_buffer_type const & in2, ...]
                          output_buffer_type & out, [output_buffer_type & out2, ... ] sample_count_t n,
                          [, optional arguments]);

    template <typename input_buffer_type, typename output_buffer_type, typename sample_count_t,
              typename muladd_helper>
    inline void perform_8(input_buffer_type const & in, [input_buffer_type const & in2, ...]
                          output_buffer_type & out, [output_buffer_type & out2, ... ] sample_count_t n,
                          [, optional arguments], muladd_helper & ma);
};
