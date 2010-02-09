// SuperCollider real time audio synthesis system
//   Copyright (c) 2002 James McCartney. All rights reserved.
// http://www.audiosynth.com

//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.

//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.

//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the Free Software
//   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA

#ifndef _SFHeaders_
#define _SFHeaders_

// #include "SCBase.h"
// #include "ReadWriteMacros.h"

enum {
	unsupported_sound_file = -1,
	AIFF_sound_file,
	AIFC_sound_file,
	RIFF_sound_file,
	NeXT_sound_file,
	IRCAM_sound_file,
	SD2_sound_file,
	raw_sound_file,
	flac_sound_file,
	vorbis_sound_file
};

enum {
	snd_unsupported = -1,
	snd_no_snd,
	snd_16_linear,
	snd_8_mulaw,
	snd_8_linear,
	snd_32_float,
	snd_32_linear,
	snd_8_alaw,
	snd_8_unsigned,
	snd_24_linear,
	snd_64_double,
	snd_16_linear_little_endian,
	snd_32_linear_little_endian,
	snd_32_float_little_endian,
	snd_64_double_little_endian,
	snd_16_unsigned,
	snd_16_unsigned_little_endian,
	snd_24_linear_little_endian,
	snd_32_vax_float,
	snd_12_linear,
	snd_12_linear_little_endian,
	snd_12_unsigned,
	snd_12_unsigned_little_endian
};

#endif
