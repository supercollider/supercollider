//   SuperCollider real time audio synthesis system
//     Copyright (c) 2002 James McCartney. All rights reserved.
//   http://www.audiosynth.com

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

#include "SCBase.h"
#include "PyrFileUtils.h"

# include "PyrSymbol.h"
# include "SFHeaders.h"

#include <string.h>
#include <fcntl.h>

bool filelen(FILE *file, size_t *length);
bool filelen(FILE *file, size_t *length)
{	// does not preserve file pointer
	fpos_t pos;
	if (fseek(file, 0, SEEK_END)) return true;
	if (fgetpos(file, &pos)) return true;

#ifdef SC_LINUX
	// sk: hack alert!
	*length = pos.__pos;
#else
	*length = pos;
#endif

	return false;
}

extern short gHomeVol;
extern long gHomeDir;

#ifdef NOCLASSIC
long setTypeCreator(unsigned char *filename, long type, long creator)
{
	FSSpec fsspec;
	short saveVol;
	long saveDir;
	long err, bytespersamp;
	int sampleSize, numchannels;
	FInfo finfo;


	HGetVol(NULL, &saveVol, &saveDir);
	HSetVol(NULL, gHomeVol, gHomeDir);
	err = FSMakeFSSpec(gHomeVol, 0L, filename, &fsspec);
	if (err) {
		postfl("IOError: %d\n", err);
		HSetVol(NULL, saveVol, saveDir);
		return errFailed;
	}
	FSpGetFInfo(&fsspec, &finfo);
	finfo.fdType = type;
	finfo.fdCreator = creator;
	FSpSetFInfo(&fsspec, &finfo);
	HSetVol(NULL, saveVol, saveDir);
	return errNone;
}
#endif



int sampleFormatFromSymbol(PyrSymbol *inSymbol, int inHeaderFormat)
{
	bool isRIFF = inHeaderFormat == RIFF_sound_file;

	char *name = inSymbol->name;
	if (name[0] == 'i') {
		if (name[3] == '8') return isRIFF ? snd_8_unsigned : snd_8_linear;
		if (name[3] == '1') return isRIFF ? snd_16_linear_little_endian : snd_16_linear;
		if (name[3] == '2') return isRIFF ? snd_24_linear_little_endian : snd_24_linear;
		if (name[3] == '3') return isRIFF ? snd_32_linear_little_endian : snd_32_linear;
	}
	if (name[0] == 'f') return isRIFF ? snd_32_float_little_endian : snd_32_float;
	if (name[0] == 'd') return isRIFF ? snd_64_double_little_endian : snd_64_double;
	if (name[0] == 'm') return snd_8_mulaw;
	if (name[0] == 'a') return snd_8_alaw;
	return snd_unsupported;
}

int headerFormatFromSymbol(PyrSymbol *inSymbol)
{
	char *name = inSymbol->name;
	if (strcmp(name, "AIFF")==0) return AIFF_sound_file;
	if (strcmp(name, "AIFC")==0) return AIFC_sound_file;
	if (strcmp(name, "RIFF")==0) return RIFF_sound_file;
	if (strcmp(name, "WAVE")==0) return RIFF_sound_file;
	if (strcmp(name, "WAV" )==0) return RIFF_sound_file;
	if (strcmp(name, "Sun" )==0) return NeXT_sound_file;
	if (strcmp(name, "SD2" )==0) return SD2_sound_file;
	if (strcmp(name, "IRCAM")==0) return IRCAM_sound_file;
	if (strcmp(name, "NeXT")==0) return NeXT_sound_file;
	if (strcmp(name, "Next")==0) return NeXT_sound_file;
	if (strcmp(name, "NEXT")==0) return NeXT_sound_file;
	if (strcmp(name, "Raw")==0) return raw_sound_file;
	if (strcmp(name, "raw")==0) return raw_sound_file;
	if (strcmp(name, "RAW")==0) return raw_sound_file;
	if (strcmp(name, "vorbis")==0) return vorbis_sound_file;
	if (strcmp(name, "FLAC")==0) return flac_sound_file;
	//if (strcmp(name, "BICSF")==0) return BICSF_sound_file;
	return unsupported_sound_file;
}



