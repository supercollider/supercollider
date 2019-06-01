// emacs:		-*- c++ -*-
// file:		SC_StringBuffer.cpp
// copyright:	2003 stefan kersten <steve@k-hornz.de>
// cvs:			$Id$

// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301
// USA

#include "SC_StringBuffer.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdexcept>

#ifdef _WIN32
#    define vsnprintf _vsnprintf
#    include <stdio.h>
#    include <stdarg.h>
#endif

#include <string>

SC_StringBuffer::SC_StringBuffer(size_t initialSize): mCapacity(0), mPtr(0), mData(0) { growBy(initialSize); }

SC_StringBuffer::SC_StringBuffer(const SC_StringBuffer& other): mCapacity(0), mPtr(0), mData(0) {
    growBy(other.getSize());
    append(other.getData(), other.getSize());
}

SC_StringBuffer::~SC_StringBuffer() { free(mData); }

void SC_StringBuffer::append(const char* src, size_t size) {
    if (size > 0) {
        size_t remaining = getRemaining();
        if (size > remaining) {
            growBy(size - remaining);
        }
        memcpy(mPtr, src, size);
        mPtr += size;
    }
}

void SC_StringBuffer::append(char c) { append(&c, sizeof(c)); }

void SC_StringBuffer::append(const char* str) { append(str, strlen(str)); }

void SC_StringBuffer::vappendf(const char* fmt, va_list ap) {
    va_list ap2;
    size_t remaining = getRemaining();

    // Calling vsnprintf may invalidate vargs, so keep a copy
#ifdef __va_copy
    __va_copy(ap2, ap);
#else
    ap2 = ap;
#endif

    // NOTE: This only works since glibc 2.0.6!
    int size = vsnprintf(mPtr, remaining, fmt, ap);
    va_end(ap);

    // size returned excludes trailing \0
    if (size++ > 0) {
        if ((size_t)size > remaining) {
            growBy(size - remaining);
            vsnprintf(mPtr, size, fmt, ap2);
        }
        mPtr += size - 1; // omit trailing \0
    }

    va_end(ap2);
}

void SC_StringBuffer::appendf(const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vappendf(fmt, ap);
    va_end(ap);
}

void SC_StringBuffer::growBy(size_t request) {
    size_t oldSize = getSize();
    size_t newCapacity = mCapacity + ((request + (size_t)kGrowAlign) & (size_t)~kGrowMask);

    // 	fprintf(stderr, "%s: mCapacity %u, request %u, newCapacity %u\n",
    // 			__PRETTY_FUNCTION__, mCapacity, request, newCapacity);
    assert((newCapacity >= (mCapacity + request)) && ((newCapacity & kGrowMask) == 0));

    char* newData = (char*)realloc(mData, newCapacity);
    if (newData) {
        mData = newData;
        mCapacity = newCapacity;
        mPtr = mData + oldSize;
    } else
        throw std::runtime_error(std::string("SC_StringBuffer: memory allocation failure"));
}

// EOF
