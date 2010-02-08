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

//machine listening plug-ins adapted for SuperCollider core

#include "ML.h"
#include "FFT_UGens.h"

InterfaceTable *ft;

SCPolarBuf* ToPolarApx(SndBuf *buf)
{
	if (buf->coord == coord_Complex) {
		SCComplexBuf* p = (SCComplexBuf*)buf->data;
		int numbins = buf->samples - 2 >> 1;
		for (int i=0; i<numbins; ++i) {
			p->bin[i].ToPolarApxInPlace();
		}
		buf->coord = coord_Polar;
	}
	return (SCPolarBuf*)buf->data;
}

SCComplexBuf* ToComplexApx(SndBuf *buf)
{
	if (buf->coord == coord_Polar) {
		SCPolarBuf* p = (SCPolarBuf*)buf->data;
		int numbins = buf->samples - 2 >> 1;
		for (int i=0; i<numbins; ++i) {
			p->bin[i].ToComplexApxInPlace();
		}
		buf->coord = coord_Complex;
	}
	return (SCComplexBuf*)buf->data;
}




void init_SCComplex(InterfaceTable *inTable);

extern "C" void load(InterfaceTable *inTable)
{
	ft = inTable;

	init_SCComplex(inTable);

	DefineDtorCantAliasUnit(BeatTrack);
	DefineDtorUnit(Loudness);
	DefineDtorUnit(KeyTrack);

	DefineDtorUnit(MFCC);

	DefineDtorUnit(Onsets);

	DefineDtorCantAliasUnit(BeatTrack2);

	DefineSimpleUnit(SpecFlatness);
	DefineDtorUnit(SpecPcile);
	DefineSimpleUnit(SpecCentroid);
}

