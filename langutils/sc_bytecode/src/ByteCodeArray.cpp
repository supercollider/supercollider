/*
    SuperCollider real time audio synthesis system
    Copyright (c) 2002 James McCartney. All rights reserved.
    http://www.audiosynth.com

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
*/

#include "sc_bytecode/ByteCodeArray.h"

void CompilingBytecodes::consume(CompilingBytecodes&& other) {
    const auto append = [](auto& to, const auto& from) {
        to.raw().insert(to.raw().end(), from.raw().begin(), from.raw().end());
    };
    append(m_data.codes, other.m_data.codes);
    append(m_data.locations, other.m_data.locations);
    append(m_data.sizeOfCodes, other.m_data.sizeOfCodes);
}

[[nodiscard]] size_t CompilingBytecodes::length() const noexcept { return m_data.codes.size(); }

[[nodiscard]] CompilingBytecodes::ByteIndex CompilingBytecodes::next_index() const noexcept {
    return m_data.codes.next_index();
}

[[nodiscard]] CompilingBytecodes::Data CompilingBytecodes::finish() && noexcept {
    assert(m_data.locations.size() == m_data.sizeOfCodes.size());
    Data out { std::move(m_data) }; // this empties this->data.
    return out;
}

void CompilingBytecodes::backSetByte(ByteIndex index, Byte newValue) noexcept {
    assert(m_data.codes.contains(index));
    m_data.codes[index] = newValue;
}

void CompilingBytecodes::assertEmpty() const noexcept {
    assert(m_data.codes.empty());
    assert(m_data.locations.empty());
    assert(m_data.sizeOfCodes.empty());
}
