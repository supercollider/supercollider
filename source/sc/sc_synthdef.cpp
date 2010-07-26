//  supercollider-style synthdef
//  Copyright (C) 2008, 2009, 2010 Tim Blechmann
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

#include <iostream>
#include <fstream>
#include <sstream>

#undef is // libsimdmath workaround
#include <boost/format.hpp>
#include <boost/integer/endian.hpp>

#include "sc_synthdef.hpp"
#include "sc_ugen_factory.hpp"

#include <utilities/sized_array.hpp>

namespace nova
{
typedef boost::int16_t int16;
typedef boost::int32_t int32;
typedef boost::int8_t int8;

using boost::integer::big32_t;
using boost::integer::big16_t;
using boost::integer::big8_t;

using std::size_t;

namespace {

std::string read_pstring(const char *& buffer)
{
    char str[256+1];
    char name_size = *buffer++;
    memcpy(str, buffer, name_size);
    str[int(name_size)] = 0;

    buffer += name_size;
    return std::string(str);
}

float read_float(const char *& ptr)
{
    big32_t data = *(big32_t*)ptr;
    ptr += 4;

    union {
        int32_t i;
        float f;
    } cast;

    cast.i = data;
    return cast.f;
}

int8_t read_int8(const char *& ptr)
{
    big8_t data = *(big8_t*)ptr;
    ptr += 1;
    return data;
}

int16_t read_int16(const char *& ptr)
{
    big16_t data = *(big16_t*)ptr;
    ptr += 2;
    return data;
}

int32_t read_int32(const char *& ptr)
{
    big32_t data = *(big32_t*)ptr;
    ptr += 4;
    return data;
}

} /* namespace */

std::vector<sc_synthdef> read_synthdefs(const char * buf_ptr)
{
    /* int32 header = */ read_int32(buf_ptr);
    /* int32 version = */ read_int32(buf_ptr);

    int16 definition_count = read_int16(buf_ptr);

    std::vector<sc_synthdef> ret;

    for (int i = 0; i != definition_count; ++i) {
        try {
            sc_synthdef def(buf_ptr);
            ret.push_back(def);
        }
        catch (std::runtime_error const & e) {
            std::cerr << "Exception when reading synthdef: " << e.what() << std::endl;
        }
    }
    return ret;
}

std::vector<sc_synthdef> read_synthdef_file(boost::filesystem::path const & filename)
{
    using namespace std;

    ifstream stream;
    stream.open(filename.string().c_str());

    if (!stream.is_open()) {
        cout << "cannot open file " << filename << endl;
        return std::vector<sc_synthdef>();
    }

    /* get length of file */
    stream.seekg (0, ios::end);
    size_t length = stream.tellg();
    stream.seekg (0, ios::beg);

    sized_array<char> buffer(length);
    stream.read(buffer.c_array(), length);
    stream.close();

    return read_synthdefs(buffer.c_array());
}

sc_synthdef::unit_spec_t::unit_spec_t(const char *& buffer)
{
    name = read_pstring(buffer);
    rate = read_int8(buffer);
    int16 inputs = read_int16(buffer);
    int16 outputs = read_int16(buffer);
    special_index = read_int16(buffer);

    for (int i = 0; i != inputs; ++i)
    {
        int16_t source = read_int16(buffer);
        int16_t index = read_int16(buffer);
        input_spec spec(source, index);
        input_specs.push_back(spec);
    }

    for (int i = 0; i != outputs; ++i)
    {
        char rate = read_int8(buffer);
        output_specs.push_back(rate);
    }
}

sc_synthdef::sc_synthdef(const char*& data)
{
    read_synthdef(data);
}

void sc_synthdef::read_synthdef(const char *& ptr)
{
    using namespace std;

    /* read name */
    name_ = read_pstring(ptr);

    /* read constants */
    int16_t consts = read_int16(ptr);

    for (int i = 0; i != consts; ++i) {
        float data = read_float(ptr);
        constants.push_back(data);
    }

    /* read parameters */
    int16_t pars = read_int16(ptr);

    for (int i = 0; i != pars; ++i) {
        float data = read_float(ptr);
        parameters.push_back(data);
    }

    /* read parameter names */
    int16_t par_names = read_int16(ptr);

    for (int i = 0; i != par_names; ++i) {
        string data = read_pstring(ptr);
        int16_t index = read_int16(ptr);

        parameter_map[data] = index;
    }

    int16_t ugens = read_int16(ptr);
    graph.reserve(ugens);

    for (int i = 0; i != ugens; ++i) {
        unit_spec_t data(ptr);
        graph.push_back(data);
    }

    prepare();
}

namespace
{

template <typename Alloc = std::allocator<int16_t> >
class buffer_allocator
{
    std::vector<size_t, Alloc> buffers; /* index: buffer id, value: last reference */

public:
    /** allocate buffer for current ugen
     *
     *  reuse buffers, which are not used after the current ugen
     */
    int16_t allocate_buffer(size_t current_ugen)
    {
        for (size_t i = 0; i != buffers.size(); ++i) {
            if (buffers[i] <= current_ugen)
                return i;
        }
        buffers.push_back(current_ugen);
        return buffers.size() - 1;
    }

    /** allocate buffer for current ugen
     *
     * reuse the buffers, which have been used before the current ugen
     */
    int16_t allocate_buffer_noalias(size_t current_ugen)
    {
        for (size_t i = 0; i != buffers.size(); ++i) {
            if (buffers[i] < current_ugen)
                return i;
        }
        buffers.push_back(current_ugen);
        return buffers.size() - 1;
    }

    size_t buffer_count (void) const
    {
        return buffers.size();
    }

    void set_last_reference (int16_t buffer_id, size_t ugen_index)
    {
        buffers[buffer_id] = ugen_index;
    }
};

} /* namespace */

void sc_synthdef::prepare(void)
{
    memory_requirement_ = 0;

    const size_t ugens = graph.size();

    buffer_allocator<> allocator;

    for (size_t ugen_index = 0; ugen_index != ugens; ++ugen_index) {
        unit_spec_t & spec = graph[ugen_index];

        /* calc units are stored in an additional vector */
        switch (spec.rate) {
        case calc_BufRate:
        case calc_FullRate:
            calc_unit_indices.push_back(ugen_index);
        }

        memory_requirement_ += spec.memory_requirement();

        spec.buffer_mapping.resize(spec.output_specs.size());

        sc_ugen_def * ugen = sc_factory->find_ugen(spec.name);

        if (unlikely(ugen == NULL))
        {
            /* we cannot prepare the synthdef, if the ugens are not installed */
            boost::format frmt("Cannot load synth %1%: Unit generator %2% not installed");
            frmt % name_ % spec.name;

            throw std::runtime_error(frmt.str());
        }

        spec.prototype = ugen;

        const bool can_alias = !ugen->cant_alias();
        memory_requirement_ += ugen->memory_requirement();

        for (size_t output_index = 0; output_index != spec.output_specs.size(); ++output_index) {
            int16_t buffer_id;
            if (spec.output_specs[output_index] == 2) {
                if (can_alias)
                    buffer_id = allocator.allocate_buffer(ugen_index);
                else
                    buffer_id = allocator.allocate_buffer_noalias(ugen_index);

                /* find last reference to this buffer */
                size_t last_ref = ugen_index;
                for (size_t i = ugens - 1; i > ugen_index; --i) {
                    unit_spec_t const & test_spec = graph[i];
                    for (size_t j = 0; j != test_spec.input_specs.size(); ++j) {
                        input_spec const & in_spec = test_spec.input_specs[j];
                        if (in_spec.source == (int16_t)ugen_index &&
                            in_spec.index == (int16_t)output_index) {
                            last_ref = i;
                            goto found_last_reference;
                        }
                    }
                }
            found_last_reference:
                allocator.set_last_reference(buffer_id, last_ref);
            }
            else
                buffer_id = -1;
            spec.buffer_mapping[output_index] = buffer_id;
        }
    }

    memory_requirement_ += (graph.size() + calc_unit_indices.size()) * sizeof(Unit*); // reserves space for units
    buffer_count = uint16_t(allocator.buffer_count());
}


std::string sc_synthdef::dump(void) const
{
    using namespace std;

    stringstream stream;

    stream << "name " << name() << endl;

    stream << "constant: " << endl;
    for (uint i = 0; i != constants.size(); ++i)
        stream << "\t" << constants[i] << endl;

    stream << "parameters: " << endl;
    for (uint i = 0; i != parameters.size(); ++i)
        stream << "\t" << parameters[i] << endl;

    stream << "parameter names: " << endl;
    for (parameter_map_t::const_iterator it = parameter_map.begin();
         it != parameter_map.end(); ++it)
        stream << "\t" << it->first << " " << it->second << endl;

    stream << "ugens: " << endl;
    for (uint i = 0; i != graph.size(); ++i) {
        stream << "\t" << graph[i].name << ": rate " << graph[i].rate
               << endl;
        stream << "\tinputs:" << endl;
        for (uint j = 0; j != graph[i].input_specs.size(); ++j) {
            stream << "\t\t" << graph[i].input_specs[j].source << " " << graph[i].input_specs[j].index << endl;
        }
    }

    return stream.str();
}


} /* namespace nova */
