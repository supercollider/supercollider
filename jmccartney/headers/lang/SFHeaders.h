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


#ifndef _SFHeaders_
#define _SFHeaders_

#include "SCBase.h"
#include "ReadWriteMacros.h"

enum {
	unsupported_sound_file = -1,
	AIFF_sound_file,
	AIFC_sound_file,
	RIFF_sound_file,
	NeXT_sound_file,
	IRCAM_sound_file,
	SD2_sound_file,
	raw_sound_file
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

	
class SFHeaderInfo
{
public:
	SFHeaderInfo();

	bool WriteHeader(FILE *inFile);
	
	void SetPath(char *inPath);
	double SampleRate() { return mSampleRate; }
	void SetSampleRate(double inSampleRate) { mSampleRate = inSampleRate; }
	void SetFormat(int inHeaderFormat, int inSampleFormat, int inCreator);
	
	double Freq() { return mFreq; }
	
	uint8 LoNote() { return mLoNote; }
	uint8 HiNote() { return mHiNote; }
	uint8 LoVeloc() { return mLoVeloc; }
	uint8 HiVeloc() { return mHiVeloc; }

	uint8 NumChannels() { return mNumChannels; }	
	int32 NumFramesInFile() { return mNumFramesInFile; }
	
	int makeSoundFileHeader(SC_IOStream<char*>& rw);

	int32 headerFileType();
	int headerSize();
	int sampleFormatSize();
	void newSoundFilePath();
	
	bool ReadSoundFileHeader(FILE *fp);

	int32 CalcFrameSize();
	bool SetTypeCreator();
	
	char mPath[256];
	double mSampleRate;
	double mFreq;
	int32 mNumFramesInFile;
	int32 mDataOffset;
	int32 mCreator;
	void *mExtraStuff;
	int16 mGain;
	int8 mHeaderFormat;
	int8 mSampleFormat;
	uint8 mNumChannels;
	uint8 mLoNote, mHiNote, mLoVeloc, mHiVeloc;
	
private:


	int make_AIFF_header(SC_IOStream<char*>& rw);
	int make_AIFC_header(SC_IOStream<char*>& rw);
	int make_RIFF_header(SC_IOStream<char*>& rw);
	int make_Next_header(SC_IOStream<char*>& rw);
	int make_IRCAM_header(SC_IOStream<char*>& rw);

	int sampleFormat_AIFF();
	int sampleFormat_AIFC();
	int sampleFormat_RIFF();
	int sampleFormat_Next();
	int sampleFormat_Next_inv(int inFormat);
	void writeCompressionFormat_AIFC(SC_IOStream<char*>& rw);
	
	bool read_AIFF_Header(SC_IOStream<FILE*>& rw);
	bool read_RIFF_Header(SC_IOStream<FILE*>& rw);
	bool read_Next_Header(SC_IOStream<FILE*>& rw);
};


extern char gDefaultSoundFolder[256];
extern char gDefaultSoundFilePath[256];

int sampleFormatSize(int inSampleFormat);

#endif
