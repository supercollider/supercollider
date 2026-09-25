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


#pragma once

#include "SC_Types.h"
#include <stdio.h>
#include <string.h>
#include <stdexcept>

inline int8 readInt8(FILE* file) { return (int8)fgetc(file); }

inline uint8 readUInt8(FILE* file) { return (uint8)fgetc(file); }

inline int16 readInt16_be(FILE* file) {
    uint16 c = (uint16)fgetc(file);
    uint16 d = (uint16)fgetc(file);

    uint16 res = ((c & 255) << 8) | (d & 255);
    return (int16)res;
}

inline int32 readInt32_be(FILE* file) {
    uint32 a = fgetc(file);
    uint32 b = fgetc(file);
    uint32 c = fgetc(file);
    uint32 d = fgetc(file);

    uint32 res = ((a & 255) << 24) | ((b & 255) << 16) | ((c & 255) << 8) | (d & 255);
    return (int32)res;
}

inline float readFloat_be(FILE* file) {
    union {
        float f;
        int32 i;
    } u;
    u.i = readInt32_be(file);
    // post("readFloat %g\n", u.f);
    return u.f;
}

inline void readData(FILE* file, char* outData, size_t inLength) {
    size_t read = fread(outData, 1, inLength, file);
    if (read != inLength)
        throw std::runtime_error("readData: read != inLength");
}

inline void checkBufferSpace(const char* buffer, const char* buffer_end, size_t wantBytes) {
    if (buffer + wantBytes > buffer_end)
        throw std::runtime_error("too little data");
}

inline int8 readInt8(const char*& buf, const char* end) {
    checkBufferSpace(buf, end, 1);
    return (int8)*buf++;
}

inline uint8 readUInt8(const char*& buf, const char* end) {
    checkBufferSpace(buf, end, 1);
    return (uint8)*buf++;
}

inline int16 readInt16_be(const char*& buf, const char* end) {
    checkBufferSpace(buf, end, 2);
    uint16 c = buf[0];
    uint16 d = buf[1];
    buf += 2;

    uint16 res = ((c & 255) << 8) | (d & 255);
    return (int16)res;
}

inline int32 readInt32_be(const char*& buf, const char* end) {
    checkBufferSpace(buf, end, 4);
    uint32 a = buf[0];
    uint32 b = buf[1];
    uint32 c = buf[2];
    uint32 d = buf[3];
    buf += 4;

    uint32 res = ((a & 255) << 24) | ((b & 255) << 16) | ((c & 255) << 8) | (d & 255);
    return (int32)res;
}

inline float readFloat_be(const char*& buf, const char* end) {
    union {
        float f;
        int32 i;
    } u;
    u.i = readInt32_be(buf, end);
    // post("readFloat %g\n", u.f);
    return u.f;
}

inline void readData(const char*& buf, const char* end, char* outData, size_t inLength) {
    checkBufferSpace(buf, end, inLength);
    memcpy(outData, buf, inLength);
    buf += inLength;
}
