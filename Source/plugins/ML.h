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

#include "SC_PlugIn.h"

//global data

//helpful constants
//#define PI 3.1415926535898f
//#define TWOPI 6.28318530717952646f 

extern InterfaceTable *ft;

//shared data for MFCC, Loudness etc
extern int eqlbandbins[43];
extern int eqlbandsizes[42];
extern float contours[42][11];
extern double phons[11];

#include "BeatTrack.h"
#include "Loudness.h"
#include "Onsets.h"

//#include "Kitch.h"
//#include "TD_Features.h"
//#include "MFCC.h"
//#include "Tartini.h"



