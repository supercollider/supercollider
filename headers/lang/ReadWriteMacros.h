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
#include "SC_Endian.h"
#include <stdio.h>
#include <string.h>

template <class T>
class SC_IOStream 
{
protected:
    T s;
public:
    SC_IOStream() : s(0) {}
    SC_IOStream(T inStream) : s(inStream) {}

    void SetStream(T inStream) { s = inStream; }
    T GetStream() { return s; }
    
    // core routines
    void readData(char *data, int size);
    uint8 readUInt8();

    void writeData(char *data, int size);
    void writeUInt8(uint8 inInt);
 
    // built using core routines
    void writeInt8(int8 inInt)
    {
        writeUInt8((uint8)inInt);
    }
    
    void writeInt16_be(int16 inInt)
    {
        writeUInt8((uint8)(inInt >> 8));
        writeUInt8(inInt);
    }
    
    void writeInt16_le(int16 inInt)
    {
        writeUInt8((uint8)inInt);
        writeUInt8((uint8)(inInt >> 8));
    }
    
    void writeInt32_be(int32 inInt)
    {
        writeUInt8((uint8)(inInt >> 24));
        writeUInt8((uint8)(inInt >> 16));
        writeUInt8((uint8)(inInt >> 8));
        writeUInt8((uint8)inInt);
    }
    
    void writeInt32_le(int32 inInt)
    {
        writeUInt8((uint8)inInt);
        writeUInt8((uint8)(inInt >> 8));
        writeUInt8((uint8)(inInt >> 16));
        writeUInt8((uint8)(inInt >> 24));
    }

#if BYTE_ORDER == BIG_ENDIAN
    void writeFloat_be(float inFloat)
#else
    void writeFloat_le(float inFloat)
#endif
    {
	union {
		float f;
		uint8 c[4];
	} u;
        u.f = inFloat;
        writeUInt8(u.c[0]);
        writeUInt8(u.c[1]);
        writeUInt8(u.c[2]);
        writeUInt8(u.c[3]);
    }

#if BYTE_ORDER == BIG_ENDIAN
    void writeFloat_le(float inFloat)
#else
    void writeFloat_be(float inFloat)
#endif
    {
	union {
		float f;
		uint8 c[4];
	} u;
        u.f = inFloat;
        writeUInt8(u.c[3]);
        writeUInt8(u.c[2]);
        writeUInt8(u.c[1]);
        writeUInt8(u.c[0]);
    }


#if BYTE_ORDER == BIG_ENDIAN
    void writeDouble_be(double inDouble)
#else
    void writeDouble_le(double inDouble)
#endif
    {
	union {
		double f;
		uint8 c[8];
	} u;
        u.f = inDouble;
        writeUInt8(u.c[0]);
        writeUInt8(u.c[1]);
        writeUInt8(u.c[2]);
        writeUInt8(u.c[3]);
        writeUInt8(u.c[4]);
        writeUInt8(u.c[5]);
        writeUInt8(u.c[6]);
        writeUInt8(u.c[7]);
    }

#if BYTE_ORDER == BIG_ENDIAN
    void writeDouble_le(double inDouble)
#else
    void writeDouble_be(double inDouble)
#endif
    {
	union {
		double f;
		uint8 c[8];
	} u;
        u.f = inDouble;
        writeUInt8(u.c[7]);
        writeUInt8(u.c[6]);
        writeUInt8(u.c[5]);
        writeUInt8(u.c[4]);
        writeUInt8(u.c[3]);
        writeUInt8(u.c[2]);
        writeUInt8(u.c[1]);
        writeUInt8(u.c[0]);
    }


    int8 readInt8()
    {
        return (int8)readUInt8();
    }
    
    int16 readInt16_be()
    {
        uint8 a = readUInt8();
        uint8 b = readUInt8();
        return (int16)((a << 8) | b);
    }
    
    int16 readInt16_le()
    {
        uint8 a = readUInt8();
        uint8 b = readUInt8();
        return (int16)((b << 8) | a);
    }
    
    int32 readInt32_be()
    {
        uint8 a = readUInt8();
        uint8 b = readUInt8();
        uint8 c = readUInt8();
        uint8 d = readUInt8();
        return (int32)((a << 24) | (b << 16) | (c << 8) | d);
    }
    
    int32 readInt32_le()
    {
        uint8 a = readUInt8();
        uint8 b = readUInt8();
        uint8 c = readUInt8();
        uint8 d = readUInt8();
        return (int32)((d << 24) | (c << 16) | (b << 8) | a);
    }

#if BYTE_ORDER == BIG_ENDIAN
    float readFloat_be()
#else
    float readFloat_le()
#endif
    {
	union {
		float f;
		uint8 c[4];
	} u;
        u.c[0] = readUInt8();
        u.c[1] = readUInt8();
        u.c[2] = readUInt8();
        u.c[3] = readUInt8();
        return u.f;
    }

#if BYTE_ORDER == BIG_ENDIAN
    float readFloat_le()
#else
    float readFloat_be()
#endif
    {
	union {
		float f;
		uint8 c[4];
	} u;
        u.c[3] = readUInt8();
        u.c[2] = readUInt8();
        u.c[1] = readUInt8();
        u.c[0] = readUInt8();
        return u.f;
    }


#if BYTE_ORDER == BIG_ENDIAN
    double readDouble_be()
#else
    double readDouble_le()
#endif
    {
	union {
		double f;
		uint8 c[8];
	} u;
        u.c[0] = readUInt8();
        u.c[1] = readUInt8();
        u.c[2] = readUInt8();
        u.c[3] = readUInt8();
        u.c[4] = readUInt8();
        u.c[5] = readUInt8();
        u.c[6] = readUInt8();
        u.c[7] = readUInt8();
        return u.f;
    }

#if BYTE_ORDER == BIG_ENDIAN
    double readDouble_le()
#else
    double readDouble_be()
#endif
    {
	union {
		double f;
		uint8 c[8];
	} u;
        u.c[7] = readUInt8();
        u.c[6] = readUInt8();
        u.c[5] = readUInt8();
        u.c[4] = readUInt8();
        u.c[3] = readUInt8();
        u.c[2] = readUInt8();
        u.c[1] = readUInt8();
        u.c[0] = readUInt8();
        return u.f;
    }
    
    void readSymbol(char *outString)
    {
            int length = readUInt8();
            readData(outString, length);
            outString[length] = 0;
    }

    void writeSymbol(char *inString)
    {
            int32 length = strlen(inString);
            writeUInt8((uint8)length);
            writeData(inString, length);
    }
};


// core routines
inline void SC_IOStream<FILE*>::readData(char *data, int size)
{
    fread(data, 1, size, s);
}

inline uint8 SC_IOStream<FILE*>::readUInt8()
{
    return (uint8)fgetc(s);
}

inline void SC_IOStream<FILE*>::writeData(char *data, int size)
{
    fwrite(data, 1, size, s);
}

inline void SC_IOStream<FILE*>::writeUInt8(uint8 inInt)
{
    fputc(inInt, s);
}

// core routines
inline void SC_IOStream<char*>::readData(char *data, int size)
{
    memcpy(data, s, size);
    s += size;
}
inline uint8 SC_IOStream<char*>::readUInt8()
{
    return (uint8)*s++;
}

inline void SC_IOStream<char*>::writeData(char *data, int size)
{
    memcpy(s, data, size);
        s += size;
}

inline void SC_IOStream<char*>::writeUInt8(uint8 inInt)
{
    *s++ = (inInt & 255);
}


#endif

