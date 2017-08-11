//  sndfile helper functions
//
//  Copyright (c) 2002 James McCartney. All rights reserved.
//  Copyright (C) 2012 Tim Blechmann
//  Copyright (C) 2017 Brian Heim
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; see the file COPYING.  If not, write to
//  the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
//  Boston, MA 02111-1307, USA.

#ifndef SC_SNDFILEHELPERS_HPP_INCLUDED
#define SC_SNDFILEHELPERS_HPP_INCLUDED

#include "SC_Errors.h"

#ifndef NO_LIBSNDFILE
#include "sndfile.h"
#include "string.h"

#include <boost/algorithm/string/predicate.hpp> // iequals

using boost::iequals;

static inline int headerFormatFromString(const char *name)
{
	if (!name) return SF_FORMAT_AIFF;
	if (iequals(name, "AIFF"))  return SF_FORMAT_AIFF;
	if (iequals(name, "AIFC"))  return SF_FORMAT_AIFF;
	if (iequals(name, "RIFF"))  return SF_FORMAT_WAV;
	if (iequals(name, "WAVEX")) return SF_FORMAT_WAVEX;
	if (iequals(name, "WAVE"))  return SF_FORMAT_WAV;
	if (iequals(name, "WAV" ))  return SF_FORMAT_WAV;
	if (iequals(name, "Sun" ))  return SF_FORMAT_AU;
	if (iequals(name, "IRCAM")) return SF_FORMAT_IRCAM;
	if (iequals(name, "NeXT"))  return SF_FORMAT_AU;
	if (iequals(name, "raw"))   return SF_FORMAT_RAW;
	if (iequals(name, "MAT4"))  return SF_FORMAT_MAT4;
	if (iequals(name, "MAT5"))  return SF_FORMAT_MAT5;
	if (iequals(name, "PAF"))   return SF_FORMAT_PAF;
	if (iequals(name, "SVX"))   return SF_FORMAT_SVX;
	if (iequals(name, "NIST"))  return SF_FORMAT_NIST;
	if (iequals(name, "VOC"))   return SF_FORMAT_VOC;
	if (iequals(name, "W64"))   return SF_FORMAT_W64;
	if (iequals(name, "PVF"))   return SF_FORMAT_PVF;
	if (iequals(name, "XI"))    return SF_FORMAT_XI;
	if (iequals(name, "HTK"))   return SF_FORMAT_HTK;
	if (iequals(name, "SDS"))   return SF_FORMAT_SDS;
	if (iequals(name, "AVR"))   return SF_FORMAT_AVR;
	if (iequals(name, "SD2"))   return SF_FORMAT_SD2;
	if (iequals(name, "FLAC"))  return SF_FORMAT_FLAC;
	// TODO allow other platforms to know vorbis once libsndfile 1.0.18 is established
	#if defined(__APPLE__) || defined(_WIN32) || LIBSNDFILE_1018
	if (iequals(name, "vorbis")) return SF_FORMAT_VORBIS;
	#endif
	if (iequals(name, "CAF"))   return SF_FORMAT_CAF;
	if (iequals(name, "RF64"))  return SF_FORMAT_RF64;
	return 0;
}

static inline int sampleFormatFromString(const char* name)
{
	if (!name) return SF_FORMAT_PCM_16;

	size_t len = strlen(name);
	if (len < 1) return 0;

	if (name[0] == 'u') {
		if (len < 5) return 0;
		if (name[4] == '8') return SF_FORMAT_PCM_U8; // uint8
		return 0;
	} else if (name[0] == 'i') {
		if (len < 4) return 0;
		if (name[3] == '8') return SF_FORMAT_PCM_S8;      // int8
		else if (name[3] == '1') return SF_FORMAT_PCM_16; // int16
		else if (name[3] == '2') return SF_FORMAT_PCM_24; // int24
		else if (name[3] == '3') return SF_FORMAT_PCM_32; // int32
	} else if (name[0] == 'f') {
		return SF_FORMAT_FLOAT; // float
	} else if (name[0] == 'd') {
		return SF_FORMAT_DOUBLE; // double
	} else if (name[0] == 'm' || name[0] == 'u') {
		return SF_FORMAT_ULAW; // mulaw ulaw
	} else if (name[0] == 'a') {
		return SF_FORMAT_ALAW; // alaw
	}
	return 0;
}

static inline int sndfileFormatInfoFromStrings(struct SF_INFO *info, const char *headerFormatString, const char *sampleFormatString)
{
	int headerFormat = headerFormatFromString(headerFormatString);
	if (!headerFormat) return kSCErr_Failed;

	int sampleFormat = sampleFormatFromString(sampleFormatString);
	if (!sampleFormat) return kSCErr_Failed;

	info->format = (unsigned int)(headerFormat | sampleFormat);
	return kSCErr_None;
}

#else

static inline int sndfileFormatInfoFromStrings(struct SF_INFO *info, const char *headerFormatString, const char *sampleFormatString)
{
	return kSCErr_Failed;
}

#endif /* NO_LIBSNDFILE */

#endif /* SC_SNDFILEHELPERS_HPP_INCLUDED */
