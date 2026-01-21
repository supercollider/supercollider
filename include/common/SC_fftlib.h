/*
SC_fftlib.h
An interface to abstract over different FFT libraries, for SuperCollider 3.
Copyright (c) 2008 Dan Stowell. All rights reserved.

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

// These specify the min & max FFT sizes expected (used when creating windows, also allocating some other arrays).
#define SC_FFT_MINSIZE 8
#define SC_FFT_LOG2_MINSIZE 3
#define SC_FFT_MAXSIZE 32768
#define SC_FFT_LOG2_MAXSIZE 15


// Note that things like *fftWindow actually allow for other sizes, to be created on user request.
#define SC_FFT_ABSOLUTE_MAXSIZE 262144
#define SC_FFT_LOG2_ABSOLUTE_MAXSIZE 18
#define SC_FFT_LOG2_ABSOLUTE_MAXSIZE_PLUS1 19

struct scfft;

typedef void* (*SCFFT_AllocFunc)(void* user, size_t size);
typedef void (*SCFFT_FreeFunc)(void* user, void* ptr);

struct SCFFT_Allocator {
    SCFFT_AllocFunc mAlloc;
    SCFFT_FreeFunc mFree;
    void* mUser;
};

enum SCFFT_Direction { kForward = 1, kBackward = 0 };

// These values are referred to from SC lang as well as in the following code - do not rearrange!
enum SCFFT_WindowFunction { kRectWindow = -1, kSineWindow = 0, kHannWindow = 1 };
