//  shared function tables
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

#ifndef DSP_SHARED_FUNCTION_TABLES_HPP
#define DSP_SHARED_FUNCTION_TABLES_HPP

#include "cos_table.hpp"

namespace nova
{

template <typename sample_type,
          unsigned int size,
          unsigned int overlap>
struct shared_cos_table
{
    static sample_type get(unsigned int index)
    {
        assert(index < size);
        return cos_table_[index];
    }

private:
    typedef cos_table<sample_type, size, overlap> table_type;

    static const table_type cos_table_;
};

template <typename sample_type, unsigned int table_size, unsigned int overlap>
const typename shared_cos_table<sample_type, table_size, overlap>::table_type
shared_cos_table<sample_type, table_size, overlap>::cos_table_;


template <typename sample_type,
          unsigned int size,
          unsigned int overlap>
struct shared_half_sin_table
{
    static sample_type get(unsigned int index)
    {
        assert(index < size);
        return half_sin_table_[index];
    }

private:
    typedef half_sin_table<sample_type, size, overlap> table_type;

    static const table_type half_sin_table_;
};

template <typename sample_type, unsigned int table_size, unsigned int overlap>
const typename shared_half_sin_table<sample_type, table_size, overlap>::table_type
shared_half_sin_table<sample_type, table_size, overlap>::half_sin_table_;



} /* namespace nova */

#endif /* DSP_SHARED_FUNCTION_TABLES_HPP */
