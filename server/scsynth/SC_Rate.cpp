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


#include "SC_Rate.h"
#include "SC_Constants.h"
#include "SC_Prototypes.h"

void Rate_Init(Rate* inRate, double inSampleRate, int inBufLength) {
    inRate->mSampleRate = inSampleRate;
    inRate->mSampleDur = 1. / inRate->mSampleRate;
    inRate->mRadiansPerSample = twopi / inRate->mSampleRate;

    inRate->mBufLength = inBufLength;
    inRate->mBufDuration = inRate->mBufLength / inRate->mSampleRate;
    inRate->mBufRate = 1. / inRate->mBufDuration;
    inRate->mSlopeFactor = 1. / inRate->mBufLength;
    inRate->mFilterLoops = inRate->mBufLength / 3;
    inRate->mFilterRemain = inRate->mBufLength % 3;
    if (inRate->mFilterLoops == 0.)
        inRate->mFilterSlope = 0.;
    else
        inRate->mFilterSlope = 1. / inRate->mFilterLoops;
}
