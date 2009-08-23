//  supercollider-style synthdef
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

#include "sc_synthdef.hpp"

#include <boost/integer/endian.hpp>

#include <iostream>

namespace nova
{
typedef boost::int16_t int16;
typedef boost::int32_t int32;
typedef boost::int8_t int8;

using boost::integer::big32_t;
using boost::integer::big16_t;
using boost::integer::big8_t;

namespace {

std::string read_pstring(std::ifstream & stream)
{
    char str[256+1];
    char name_size;
    stream.get(name_size);
    stream.read(str, name_size);
    str[int(name_size)] = 0;

    return std::string(str);
}

float read_float(std::ifstream & stream)
{
    big32_t data;

    union {
        int32_t i;
        float f;
    } cast;

    stream.read((char*)&data, 4);

    cast.i = data;

    return cast.f;
}

int8_t read_int8(std::ifstream & stream)
{
    big8_t data;
    stream.read((char*)&data, 1);
    return data;
}

int16_t read_int16(std::ifstream & stream)
{
    big16_t data;
    stream.read((char*)&data, 2);
    return data;
}

int16_t read_int32(std::ifstream & stream)
{
    big32_t data;
    stream.read((char*)&data, 4);
    return data;
}

} /* namespace */


std::vector<sc_synthdef> read_synthdef_file(std::string const & filename)
{
    using namespace std;

    ifstream stream;
    stream.open(filename.c_str());

    std::vector<sc_synthdef> ret;

    if (!stream.is_open()) {
        cout << "cannot open file " << filename << endl;
        return ret;
    }

    /* int32 header = */ read_int32(stream);
    /* int32 version = */ read_int32(stream);

    int16 definition_count = read_int16(stream);

    for (int i = 0; i != definition_count; ++i) {
        sc_synthdef def(stream);
        ret.push_back(def);
    }

    return ret;
}

sc_synthdef::unit_spec_t::unit_spec_t(std::ifstream & stream)
{
    name = read_pstring(stream);
    rate = read_int8(stream);
    int16 inputs = read_int16(stream);
    int16 outputs = read_int16(stream);
    special_index = read_int16(stream);

    for (int i = 0; i != inputs; ++i)
    {
        input_spec spec;
        spec.source = read_int16(stream);
        spec.index = read_int16(stream);

        input_specs.push_back(spec);
    }

    for (int i = 0; i != outputs; ++i)
    {
        char rate = read_int8(stream);
        output_specs.push_back(rate);
    }
}

sc_synthdef::sc_synthdef(std::ifstream & stream)
{
    read_synthdef(stream);
}

sc_synthdef::sc_synthdef(boost::filesystem::path const & path)
{
    std::ifstream stream(path.string().c_str());
    read_synthdef(stream);
}

void sc_synthdef::read_synthdef(std::ifstream & stream)
{
    using namespace std;

    /* read name */
    name_ = read_pstring(stream);

    /* read constants */
    int16_t consts = read_int16(stream);

    for (int i = 0; i != consts; ++i) {
        float data = read_float(stream);

        cout << "constant " << data << endl;

        constants.push_back(data);
    }

    /* read parameters */
    int16_t pars = read_int16(stream);
    cout << "parameters " << pars << endl;

    for (int i = 0; i != pars; ++i) {
        float data = read_float(stream);

        cout << "parameter " << data << endl;

        parameters.push_back(data);
    }

    /* read parameter names */
    int16_t par_names = read_int16(stream);

    for (int i = 0; i != par_names; ++i) {
        string data = read_pstring(stream);
        int16_t index = read_int16(stream);

        cout << data << " " << index << endl;

        parameter_map[data] = index;
    }

    int16_t ugens = read_int16(stream);

    for (int i = 0; i != ugens; ++i) {
        unit_spec_t data(stream);

        graph.push_back(data);
    }
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
    for (uint i = 0; i != graph.size(); ++i)
        stream << "\t" << graph[i].name << endl;

    return stream.str();
}


} /* namespace nova */
