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

#include "ByteCodeArray.h"

void CompilingBytecodes::consume(CompilingBytecodes&& other) {
    m_data.codes.insert(m_data.codes.end(), other.m_data.codes.begin(), other.m_data.codes.end());
    m_data.startAndEndLocations.insert(m_data.startAndEndLocations.end(), other.m_data.startAndEndLocations.begin(),
                                       other.m_data.startAndEndLocations.end());
    m_data.sizeOfCodes.insert(m_data.sizeOfCodes.end(), other.m_data.sizeOfCodes.begin(),
                              other.m_data.sizeOfCodes.end());
}

[[nodiscard]] size_t CompilingBytecodes::length() const noexcept { return m_data.codes.size(); }

[[nodiscard]] CompilingBytecodes::Data CompilingBytecodes::finish() && noexcept {
    assert(m_data.startAndEndLocations.size() == m_data.sizeOfCodes.size() * 2); // twice as many.
    Data out { std::move(m_data) }; // this empties this->data.
    return out;
}

void CompilingBytecodes::backSetByte(size_t index, Byte newValue) noexcept {
    assert(index < m_data.codes.size());
    m_data.codes[index] = newValue;
}

void CompilingBytecodes::assertEmpty() const noexcept {
    assert(m_data.codes.empty());
    assert(m_data.startAndEndLocations.empty());
    assert(m_data.sizeOfCodes.empty());
}
