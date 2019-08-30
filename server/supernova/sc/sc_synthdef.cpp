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
#include <iterator>
#include <fstream>
#include <sstream>

#include <boost/format.hpp>
#include <boost/endian/arithmetic.hpp>

#include "sc_synthdef.hpp"
#include "sc_ugen_factory.hpp"

#include "utilities/sized_array.hpp"
#include "utilities/exists.hpp"

namespace nova {

typedef std::int16_t int16;
typedef std::int32_t int32;
typedef std::int8_t int8;

using boost::endian::big_int16_t;
using boost::endian::big_int32_t;
using boost::endian::big_int8_t;

using std::size_t;

namespace {

void verify_synthdef_buffer(const char* buffer, const char* buffer_end) {
    if (buffer >= buffer_end)
        throw std::runtime_error("corrupted synthdef");
}

std::string read_pstring(const char*& buffer, const char* buffer_end) {
    verify_synthdef_buffer(buffer, buffer_end);

    char str[256 + 1];
    char name_size = *buffer++;
    verify_synthdef_buffer(buffer + name_size, buffer_end);

    memcpy(str, buffer, name_size);
    str[int(name_size)] = 0;

    buffer += name_size;
    return std::string(str);
}

float read_float(const char*& buffer, const char* buffer_end) {
    verify_synthdef_buffer(buffer, buffer_end);

    big_int32_t data = *(big_int32_t*)buffer;
    buffer += 4;

    union {
        int32_t i;
        float f;
    } cast;

    cast.i = data;
    return cast.f;
}

int8_t read_int8(const char*& buffer, const char* buffer_end) {
    verify_synthdef_buffer(buffer, buffer_end);

    big_int8_t data = *(big_int8_t*)buffer;
    buffer += 1;
    return data;
}


int16_t read_int16(const char*& buffer, const char* buffer_end) {
    verify_synthdef_buffer(buffer, buffer_end);

    big_int16_t data = *(big_int16_t*)buffer;
    buffer += 2;
    return data;
}

int32_t read_int32(const char*& buffer, const char* buffer_end) {
    verify_synthdef_buffer(buffer, buffer_end);

    big_int32_t data = *(big_int32_t*)buffer;
    buffer += 4;
    return data;
}

int32_t read_int(const char*& buffer, const char* buffer_end, int size) {
    if (size == 32)
        return read_int32(buffer, buffer_end);

    if (size == 16)
        return read_int16(buffer, buffer_end);
    assert(false);
    return 0;
}

} /* namespace */

std::vector<sc_synthdef> read_synthdefs(const char* buffer, const char* buffer_end) {
    /* int32 header = */ read_int32(buffer, buffer_end);
    int32 version = read_int32(buffer, buffer_end);

    int16 definition_count = read_int16(buffer, buffer_end);

    std::vector<sc_synthdef> ret;

    for (int i = 0; i != definition_count; ++i) {
        try {
#ifdef __clang__
            // clang does not like to emplace_back
            sc_synthdef def(buffer, buffer_end, version);
            ret.push_back(def);
#else
            ret.emplace_back(buffer, buffer_end, version);
#endif
        } catch (std::exception const& e) {
            std::cout << "Exception when reading synthdef: " << e.what() << std::endl;
        }
    }
    return ret;
}

std::vector<sc_synthdef> read_synthdef_file(boost::filesystem::path const& filename) {
    using namespace std;

    ifstream stream;
    stream.open(filename.string().c_str());

    if (!stream.is_open()) {
        cout << "cannot open file " << filename << endl;
        return std::vector<sc_synthdef>();
    }

    /* get length of file */
    stream.seekg(0, ios::end);
    size_t length = stream.tellg();
    stream.seekg(0, ios::beg);

    sized_array<char> buffer(length);
    stream.read(buffer.c_array(), length);
    stream.close();

    return read_synthdefs(buffer.begin(), buffer.end());
}

sc_synthdef::unit_spec_t::unit_spec_t(const char*& buffer, const char* buffer_end, int version) {
    const int short_int_size = (version == 1) ? 16 : 32;

    name = symbol(read_pstring(buffer, buffer_end));
    rate = read_int8(buffer, buffer_end);
    int32_t input_count = read_int(buffer, buffer_end, short_int_size);
    int32_t output_count = read_int(buffer, buffer_end, short_int_size);
    special_index = read_int16(buffer, buffer_end);

    for (int i = 0; i != input_count; ++i) {
        int32_t source = read_int(buffer, buffer_end, short_int_size);
        int32_t index = read_int(buffer, buffer_end, short_int_size);
        input_spec spec(source, index);
        input_specs.push_back(spec);
    }

    for (int i = 0; i != output_count; ++i) {
        char rate = read_int8(buffer, buffer_end);
        output_specs.push_back(rate);
    }
}

sc_synthdef::sc_synthdef(const char*& buffer, const char* buffer_end, int version) {
    read_synthdef(buffer, buffer_end, version);
}

void sc_synthdef::read_synthdef(const char*& buffer, const char* buffer_end, int version) {
    using namespace std;
    const int short_int_size = (version == 1) ? 16 : 32;

    /* read name */
    name_ = symbol(read_pstring(buffer, buffer_end));

    /* read constants */
    int32_t constant_count = read_int(buffer, buffer_end, short_int_size);
    constants.reserve(constant_count);

    for (int i = 0; i != constant_count; ++i) {
        float data = read_float(buffer, buffer_end);
        constants.push_back(data);
    }

    /* read parameters */
    int32_t par_count = read_int(buffer, buffer_end, short_int_size);
    parameters.reserve(par_count);

    for (int i = 0; i != par_count; ++i) {
        float data = read_float(buffer, buffer_end);
        parameters.push_back(data);
    }

    /* read parameter names */
    int32_t parameter_names_count = read_int(buffer, buffer_end, short_int_size);

    for (int i = 0; i != parameter_names_count; ++i) {
        symbol data = symbol(read_pstring(buffer, buffer_end));
        int32_t index = read_int(buffer, buffer_end, short_int_size);

        parameter_map[data] = index;
    }

    int32_t ugen_count = read_int(buffer, buffer_end, short_int_size);
    graph.reserve(ugen_count);

    for (int i = 0; i != ugen_count; ++i) {
        unit_spec_t data(buffer, buffer_end, version);
        graph.push_back(data);
    }

    prepare();
}

namespace {

template <typename Alloc = std::allocator<size_t>> class buffer_allocator {
    std::vector<size_t, Alloc> buffers; /* index: buffer id, value: last reference */

public:
    buffer_allocator(size_t size_hint) { buffers.reserve(size_hint); }

    /** allocate buffer for current ugen
     *
     *  reuse buffers, which are not used after the current ugen
     */
    int32_t allocate_buffer(size_t current_ugen) {
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
    int32_t allocate_buffer_noalias(size_t current_ugen) {
        for (size_t i = 0; i != buffers.size(); ++i) {
            if (buffers[i] < current_ugen)
                return i;
        }
        buffers.push_back(current_ugen);
        return buffers.size() - 1;
    }

    size_t buffer_count(void) const { return buffers.size(); }

    void set_last_reference(int32_t buffer_id, size_t ugen_index) { buffers[buffer_id] = ugen_index; }
};

} /* namespace */

void sc_synthdef::prepare(void) {
    // FIXME: this currently has quadratic complexity, as buffer_allocator::allocate has linear complexity
    memory_requirement_ = 0;

    const size_t number_of_ugens = graph.size();
    calc_unit_indices.reserve(number_of_ugens);

    // store the last references to each output buffer inside a std::map for faster lookup
    std::map<input_spec, size_t> last_buffer_references;

    for (auto it = graph.rbegin(); it != graph.rend(); ++it) {
        for (size_t i = 0; i != it->input_specs.size(); ++i) {
            input_spec const& in_spec = it->input_specs[i];

            if (it->rate == calc_DemandRate) {
                size_t ugen_index = number_of_ugens; // audio-inputs to demand-rate ugens will never be reused!
                last_buffer_references[in_spec] = ugen_index;
            } else {
                if (!exists(last_buffer_references, in_spec)) {
                    size_t ugen_index = std::distance(it, graph.rend()) - 1;
                    last_buffer_references.insert(std::make_pair(in_spec, ugen_index));
                }
            }
        }
    }

    buffer_allocator<> allocator(number_of_ugens / 8);

    for (size_t ugen_index = 0; ugen_index != number_of_ugens; ++ugen_index) {
        unit_spec_t& current_ugen_spec = graph[ugen_index];

        /* calc units are stored in an additional vector */
        switch (current_ugen_spec.rate) {
        case calc_BufRate:
        case calc_FullRate:
            calc_unit_indices.push_back(ugen_index);
        }

        memory_requirement_ += current_ugen_spec.memory_requirement();

        current_ugen_spec.buffer_mapping.resize(current_ugen_spec.output_specs.size());

        sc_ugen_def* ugen = sc_factory->find_ugen(current_ugen_spec.name);
        if (unlikely(ugen == nullptr)) {
            /* we cannot prepare the synthdef, if the ugens are not installed */
            boost::format frmt("Cannot load synth %1%: Unit generator %2% not installed");
            frmt % name_.c_str() % current_ugen_spec.name.c_str();

            throw std::runtime_error(frmt.str());
        }

        current_ugen_spec.prototype = ugen;

        const bool can_alias = !ugen->cant_alias();
        memory_requirement_ += ugen->memory_requirement();

        for (size_t output_index = 0; output_index != current_ugen_spec.output_specs.size(); ++output_index) {
            int32_t buffer_id;
            if (current_ugen_spec.output_specs[output_index] == 2) {
                /* find last reference to this buffer */
                size_t last_ref = ugen_index;
                input_spec this_input_spec(ugen_index, output_index);

                if (exists(last_buffer_references, this_input_spec))
                    last_ref = last_buffer_references[this_input_spec];

                buffer_id =
                    can_alias ? allocator.allocate_buffer(ugen_index) : allocator.allocate_buffer_noalias(ugen_index);

                allocator.set_last_reference(buffer_id, last_ref);
            } else
                buffer_id = -1;
            current_ugen_spec.buffer_mapping[output_index] = buffer_id;
        }
    }

    memory_requirement_ += (graph.size() + calc_unit_indices.size() + 1)
        * sizeof(Unit*); // reserves space for units (one more to allor prefetching)

    // memory that is required to fill the sc_synth data structure
    // see SC_Graph.h for corresponding data structure
    // mControls, mControlRates, mAudioBusOffset, mMapControls
    const size_t param_alloc_size = parameter_count() * (sizeof(float) + sizeof(int) + sizeof(int32) + sizeof(float*));
    // mWire
    const size_t constant_alloc_size = constants.size() * sizeof(Wire);
    const size_t ctor_alloc_size = param_alloc_size + constant_alloc_size;

    memory_requirement_ += ctor_alloc_size;

    buffer_count = uint32_t(allocator.buffer_count());

    calc_unit_indices.shrink_to_fit();
}


std::string sc_synthdef::dump(void) const {
    using namespace std;

    stringstream stream;

    stream << "name " << name().c_str() << endl;

    stream << "constant: " << endl;
    for (uint i = 0; i != constants.size(); ++i)
        stream << "\t" << constants[i] << endl;

    stream << "parameters: " << endl;
    for (uint i = 0; i != parameters.size(); ++i)
        stream << "\t" << parameters[i] << endl;

    stream << "parameter names: " << endl;
    for (const auto& elem : parameter_map)
        stream << "\t" << elem.first.c_str() << " " << elem.second << endl;

    stream << "ugens: " << endl;
    for (uint i = 0; i != graph.size(); ++i) {
        stream << "\t" << graph[i].name.c_str() << ": rate " << graph[i].rate << endl;
        stream << "\tinputs:" << endl;
        for (uint j = 0; j != graph[i].input_specs.size(); ++j) {
            stream << "\t\t" << graph[i].input_specs[j].source << " " << graph[i].input_specs[j].index << endl;
        }
    }

    return stream.str();
}


} /* namespace nova */
