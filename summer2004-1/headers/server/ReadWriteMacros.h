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
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/


#ifndef _ReadWriteMacros_
#define _ReadWriteMacros_

#include "SC_Types.h"
#include <stdio.h>
#include <string.h>

inline void writeData(char *&buf, char *data, int size)
{
	memcpy(buf, data, size);
	buf += size;
}

inline void writeZero(char *&buf, int len)
{
	for (int i=0; i<len; ++i) *buf++ = 0;
}

inline void writeSkip(char *&buf, int len)
{
	buf += len;
}

inline void writeInt8(char *&buf, int8 inInt)
{
	*buf++ = (char)(inInt & 255);
}

inline void writeUInt8(char *&buf, uint8 inInt)
{
	*buf++ = (char)(inInt & 255);
}

inline void writeInt16_be(char *&buf, int16 inInt)
{
	*buf++ = (char)((inInt >> 8) & 255);
	*buf++ = (char)(inInt & 255);
}

inline void writeInt16_le(char *&buf, int16 inInt)
{
	*buf++ = (char)(inInt & 255);
	*buf++ = (char)((inInt >> 8) & 255);
}

inline void writeInt32_be(char *&buf, int32 inInt)
{
	*buf++ = (char)((inInt >> 24) & 255);
	*buf++ = (char)((inInt >> 16) & 255);
	*buf++ = (char)((inInt >> 8) & 255);
	*buf++ = (char)(inInt & 255);
}

inline void writeInt32_le(char *&buf, int32 inInt)
{
	*buf++ = (char)(inInt & 255);
	*buf++ = (char)((inInt >> 8) & 255);
	*buf++ = (char)((inInt >> 16) & 255);
	*buf++ = (char)((inInt >> 24) & 255);
}

inline void writeSymbol(char *&buf, char *inString)
{
	size_t length = strlen(inString);
	writeUInt8(buf, (uint8)length);
	memcpy(buf, inString, length);
	buf += length;
}

inline void writeString(char *&buf, char *inString, size_t inLength)
{
	writeInt32_be(buf, inLength);
	memcpy(buf, inString, inLength);
	buf += inLength;
}

inline void writeString(char *&buf, char *inString)
{
	size_t length = strlen(inString);
	writeString(buf, inString, length);
}



inline void writeData(FILE *file, char *data, size_t size)
{
	fwrite(data, 1, size, file);
}

inline void writeInt8(FILE *file, int8 inInt)
{
	fputc(inInt & 255, file);
}

inline void writeUInt8(FILE *file, uint8 inInt)
{
	fputc(inInt & 255, file);
}

inline void writeInt16_be(FILE *file, int16 inInt)
{
	fputc((inInt >> 8) & 255, file);
	fputc(inInt & 255, file);
}

inline void writeInt16_le(FILE *file, int16 inInt)
{
	fputc(inInt & 255, file);
	fputc((inInt >> 8) & 255, file);
}

inline void writeInt32_be(FILE *file, int32 inInt)
{
	fputc((inInt >> 24) & 255, file);
	fputc((inInt >> 16) & 255, file);
	fputc((inInt >> 8) & 255, file);
	fputc(inInt & 255, file);
}

inline void writeInt64_be(FILE *file, int64 inInt)
{
	writeInt32_be(file, (int32)((inInt >> 32) & 0x00000000FFFFFFFF));
	writeInt32_be(file, (int32)(inInt));
}

inline void writeInt32_le(FILE *file, int32 inInt)
{
	fputc(inInt & 255, file);
	fputc((inInt >> 8) & 255, file);
	fputc((inInt >> 16) & 255, file);
	fputc((inInt >> 24) & 255, file);
}

inline void writeFloat_be(FILE *file, float inFloat)
{
	union {
		float f;
		int32 i;
	} u;
	u.f = inFloat;
	writeInt32_be(file, u.i);
}

inline void writeFloat_le(FILE *file, float inFloat)
{
	union {
		float f;
		int32 i;
	} u;
	u.f = inFloat;
	writeInt32_le(file, u.i);
}

inline void writeSymbol(FILE *file, char *inString)
{
	size_t length = strlen(inString);
	writeUInt8(file, (uint8)length);
	fwrite(inString, 1, length, file);
}

inline void writeString(FILE *file, char *inString, size_t inLength)
{
	writeInt32_be(file, inLength);
	fwrite(inString, 1, inLength, file);
}

inline void writeString(FILE *file, char *inString)
{
	size_t length = strlen(inString);
	writeString(file, inString, length);
}

inline int32 readInt8(FILE *file)
{
	int32 res = fgetc(file);
	
	return res;
}

inline uint32 readUInt8(FILE *file)
{
	uint32 res = (uint32)fgetc(file);
	return res;
}

inline int32 readInt16_be(FILE *file)
{
	int32 c = fgetc(file);
	int32 d = fgetc(file);
	
	int32 res = ((c & 255) << 8) | (d & 255);
	return res;
}

inline int32 readInt16_le(FILE *file)
{
	int32 c = fgetc(file);
	int32 d = fgetc(file);
	
	int32 res = ((d & 255) << 8) | (c & 255);
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

inline int32 readInt32_le(FILE *file)
{
	int32 a = fgetc(file);
	int32 b = fgetc(file);
	int32 c = fgetc(file);
	int32 d = fgetc(file);
	
	int32 res = ((d & 255) << 24) | ((c & 255) << 16) | ((b & 255) << 8) | (a & 255);
	return res;
}

inline int64 readInt64_be(FILE *file)
{
	int64 hi = readInt32_be(file);
	int64 lo = readInt32_be(file);
	return (hi << 32) | (lo & 0x00000000FFFFFFFF);
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

inline float readFloat_le(FILE *file)
{
	union {
		float f;
		int32 i;
	} u;
	u.i = readInt32_le(file);
	//post("readFloat %g\n", u.f);
	return u.f;
}

inline void readString(FILE *file, char *outString, size_t inLength)
{
	fread(outString, 1, inLength, file);
	outString[inLength] = 0;
}

inline void readSymbol(FILE *file, char *outString)
{
	size_t length = (size_t)readUInt8(file);
	readString(file, outString, length);
}

inline void readData(FILE *file, char *outData, size_t inLength)
{
	fread(outData, 1, inLength, file);
}


inline int32 readInt8(char *&buf)
{
	int32 res = *buf++;
	return res;
}

inline uint32 readUInt8(char *&buf)
{
	uint32 res = *buf++;
	return res;
}

inline int32 readInt16_be(char *&buf)
{
	int32 c = readInt8(buf);
	int32 d = readInt8(buf);
	
	int32 res = ((c & 255) << 8) | (d & 255);
	return res;
}

inline int32 readInt16_le(char *&buf)
{
	int32 c = readInt8(buf);
	int32 d = readInt8(buf);
	
	int32 res = ((d & 255) << 8) | (c & 255);
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

inline int32 readInt32_le(char *&buf)
{
	int32 a = readInt8(buf);
	int32 b = readInt8(buf);
	int32 c = readInt8(buf);
	int32 d = readInt8(buf);
	
	int32 res = ((d & 255) << 24) | ((c & 255) << 16) | ((b & 255) << 8) | (a & 255);
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

inline float readFloat_le(char *&buf)
{
	union {
		float f;
		int32 i;
	} u;
	u.i = readInt32_le(buf);
	//post("readFloat %g\n", u.f);
	return u.f;
}

inline double readDouble_be(char *&buf)
{
	//post("readDouble\n");
	union {
		double f;
		uint8 c[8];
	} u;
	
	u.c[0] = (uint8)readInt8(buf);
	u.c[1] = (uint8)readInt8(buf);
	u.c[2] = (uint8)readInt8(buf);
	u.c[3] = (uint8)readInt8(buf);
	u.c[4] = (uint8)readInt8(buf);
	u.c[5] = (uint8)readInt8(buf);
	u.c[6] = (uint8)readInt8(buf);
	u.c[7] = (uint8)readInt8(buf);
	//post("readDouble %g  %08X %08X\n", u.f, u.f);
	return u.f;
}

inline double readDouble_le(char *&buf)
{
	//post("readDouble\n");
	union {
		double f;
		uint8 c[8];
	} u;
	
	u.c[7] = (uint8)readInt8(buf);
	u.c[6] = (uint8)readInt8(buf);
	u.c[5] = (uint8)readInt8(buf);
	u.c[4] = (uint8)readInt8(buf);
	u.c[3] = (uint8)readInt8(buf);
	u.c[2] = (uint8)readInt8(buf);
	u.c[1] = (uint8)readInt8(buf);
	u.c[0] = (uint8)readInt8(buf);

	//post("readDouble %g\n", u.f);
	return u.f;
}

inline void readString(char *&buf, char *outString, size_t inLength)
{
	memcpy(outString, buf, inLength);
	outString[inLength] = 0;
	buf += inLength;
}

inline void readSymbol(char *&buf, char *outString)
{
	size_t length = (size_t)readUInt8(buf);
	readString(buf, outString, length);
}

inline void readData(char *&buf, char *outData, size_t inLength)
{
	memcpy(outData, buf, inLength);
	buf += inLength;
}


#endif