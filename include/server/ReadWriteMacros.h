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


#ifndef _ReadWriteMacros_
#define _ReadWriteMacros_

#include "SC_Types.h"
#include <stdio.h>
#include <string.h>
#include <stdexcept>

inline int32 readInt8(FILE *file)
{
	int32 res = fgetc(file);

	return res;
}

inline uint32 readUInt8(FILE *file)
{
    uint8 res = (uint8)fgetc(file);
    return (uint32)res;
}

inline int32 readInt16_be(FILE *file)
{
	int32 c = fgetc(file);
	int32 d = fgetc(file);

	int32 res = ((c & 255) << 8) | (d & 255);
	return res;
}

inline int32 readInt32_be(FILE *file)
{
	int32 a = fgetc(file);
	int32 b = fgetc(file);
	int32 c = fgetc(file);
	int32 d = fgetc(file);

	int32 res = ((a & 255) << 24) | ((b & 255) << 16) | ((c & 255) << 8) | (d & 255);
	return res;
}

inline float readFloat_be(FILE *file)
{
	union {
		float f;
		int32 i;
	} u;
	u.i = readInt32_be(file);
	//post("readFloat %g\n", u.f);
	return u.f;
}

inline void readData(FILE *file, char *outData, size_t inLength)
{
	size_t read = fread(outData, 1, inLength, file);
	if (read != inLength)
		throw std::runtime_error("readData: read != inLength");
}

inline int32 readInt8(char *&buf)
{
	int32 res = *buf++;
	return res;
}

inline uint32 readUInt8(char *&buf)
{
    uint8 res = (uint8)*buf++;
    return (uint32)res;
}

inline int32 readInt16_be(char *&buf)
{
	int32 c = readInt8(buf);
	int32 d = readInt8(buf);

	int32 res = ((c & 255) << 8) | (d & 255);
	return res;
}

inline int32 readInt32_be(char *&buf)
{
	int32 a = readInt8(buf);
	int32 b = readInt8(buf);
	int32 c = readInt8(buf);
	int32 d = readInt8(buf);

	int32 res = ((a & 255) << 24) | ((b & 255) << 16) | ((c & 255) << 8) | (d & 255);
	return res;
}

inline float readFloat_be(char *&buf)
{
	union {
		float f;
		int32 i;
	} u;
	u.i = readInt32_be(buf);
	//post("readFloat %g\n", u.f);
	return u.f;
}

inline void readData(char *&buf, char *outData, size_t inLength)
{
	memcpy(outData, buf, inLength);
	buf += inLength;
}


#endif
