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

#pragma once

#ifndef NO_LIBSNDFILE

#    include "SC_Errors.h"

// on Windows, enable Windows libsndfile prototypes in order to access sf_wchar_open.
// See sndfile.h, lines 739-752. Note that order matters: this has to be the first include of sndfile.h
#    ifdef _WIN32
#        include "SC_Codecvt.hpp" // utf8_cstr_to_utf16_wstring
#        include <windows.h>
#        define ENABLE_SNDFILE_WINDOWS_PROTOTYPES 1
#    endif // _WIN32
#    include <sndfile.h>
#    include <sndfile.hh>

#    include <filesystem>

#    include <boost/algorithm/string/predicate.hpp> // iequals

namespace fs = std::filesystem;
// case-insensitive string comparison
using boost::iequals;

/** @brief takes a header format string and returns the corresponding SF_FORMAT constant.
 *  @param name The header format name as a C-string.
 *  @returns a SF_FORMAT constant on success, 0 if the format string is invalid.
 */
inline int headerFormatFromString(const char* name) {
    if (!name)
        return SF_FORMAT_AIFF;
    if (iequals(name, "AIFF"))
        return SF_FORMAT_AIFF;
    if (iequals(name, "AIFC"))
        return SF_FORMAT_AIFF;
    if (iequals(name, "RIFF"))
        return SF_FORMAT_WAV;
    if (iequals(name, "WAVEX"))
        return SF_FORMAT_WAVEX;
    if (iequals(name, "WAVE"))
        return SF_FORMAT_WAV;
    if (iequals(name, "WAV"))
        return SF_FORMAT_WAV;
    if (iequals(name, "Sun"))
        return SF_FORMAT_AU;
    if (iequals(name, "IRCAM"))
        return SF_FORMAT_IRCAM;
    if (iequals(name, "NeXT"))
        return SF_FORMAT_AU;
    if (iequals(name, "raw"))
        return SF_FORMAT_RAW;
    if (iequals(name, "MAT4"))
        return SF_FORMAT_MAT4;
    if (iequals(name, "MAT5"))
        return SF_FORMAT_MAT5;
    if (iequals(name, "PAF"))
        return SF_FORMAT_PAF;
    if (iequals(name, "SVX"))
        return SF_FORMAT_SVX;
    if (iequals(name, "NIST"))
        return SF_FORMAT_NIST;
    if (iequals(name, "VOC"))
        return SF_FORMAT_VOC;
    if (iequals(name, "W64"))
        return SF_FORMAT_W64;
    if (iequals(name, "PVF"))
        return SF_FORMAT_PVF;
    if (iequals(name, "XI"))
        return SF_FORMAT_XI;
    if (iequals(name, "HTK"))
        return SF_FORMAT_HTK;
    if (iequals(name, "SDS"))
        return SF_FORMAT_SDS;
    if (iequals(name, "AVR"))
        return SF_FORMAT_AVR;
    if (iequals(name, "SD2"))
        return SF_FORMAT_SD2;
    if (iequals(name, "FLAC"))
        return SF_FORMAT_FLAC;
#    if defined(SNDFILE_HAS_VORBIS) || defined(SNDFILE_HAS_OPUS)
    if (iequals(name, "OGG"))
        return SF_FORMAT_OGG;
#    endif
    if (iequals(name, "CAF"))
        return SF_FORMAT_CAF;
    if (iequals(name, "RF64"))
        return SF_FORMAT_RF64;
#    ifdef SNDFILE_HAS_MPEG
    if (iequals(name, "MPEG"))
        return SF_FORMAT_MPEG;
#    endif
    return 0;
}

/** @brief takes a sample format string and returns the corresponding SF_FORMAT constant.
 *  @param name The sample format name as a C-string.
 *  @returns a SF_FORMAT constant on success, 0 if the format string is invalid.
 */
inline int sampleFormatFromString(const char* name) {
    if (!name)
        return SF_FORMAT_PCM_16;

    size_t len = strlen(name);
    if (len < 1)
        return 0;

    if (iequals(name, "uint8")) {
        return SF_FORMAT_PCM_U8; // uint8
    } else if (iequals(name, "int8")) {
        return SF_FORMAT_PCM_S8; // int8
    } else if (iequals(name, "int16")) {
        return SF_FORMAT_PCM_16; // int16
    } else if (iequals(name, "int24")) {
        return SF_FORMAT_PCM_24; // int24
    } else if (iequals(name, "int32")) {
        return SF_FORMAT_PCM_32; // int32
    } else if (iequals(name, "float")) {
        return SF_FORMAT_FLOAT; // float
    } else if (iequals(name, "double")) {
        return SF_FORMAT_DOUBLE; // double
    } else if (iequals(name, "mulaw") || iequals(name, "ulaw")) {
        return SF_FORMAT_ULAW; // mulaw ulaw
    } else if (iequals(name, "alaw")) {
        return SF_FORMAT_ALAW; // alaw
    }
#    ifdef SNDFILE_HAS_VORBIS
    if (iequals(name, "vorbis")) {
        return SF_FORMAT_VORBIS; // vorbis
    }
#    endif
#    ifdef SNDFILE_HAS_OPUS
    if (iequals(name, "opus")) {
        return SF_FORMAT_OPUS; // opus
    }
#    endif
#    ifdef SNDFILE_HAS_MPEG
    if (iequals(name, "mp1")) {
        return SF_FORMAT_MPEG_LAYER_I; // mpeg 1 layer 1
    } else if (iequals(name, "mp2")) {
        return SF_FORMAT_MPEG_LAYER_II; // mpeg 1 layer 2
    } else if (iequals(name, "mp3")) {
        return SF_FORMAT_MPEG_LAYER_III; // mpeg 2 layer 3 (mp3)
    }
#    endif
    return 0;
}

/** @brief initializes a SF_INFO struct with the given header and sample format.
 *  @param info [out] the SF_INFO struct to be initialized. @note only sets the `format` member!
 *  @param headerFormatString [in] the header format as a C-string
 *  @param sampleFormatString [in] the sample format as a C-string
 *  @returns a SCErr error code.
 */
inline SCErr sndfileFormatInfoFromStrings(SF_INFO& info, const char* headerFormatString,
                                          const char* sampleFormatString) {
    int headerFormat = headerFormatFromString(headerFormatString);
    if (!headerFormat)
        return kSCErr_Failed;

    int sampleFormat = sampleFormatFromString(sampleFormatString);
    if (!sampleFormat)
        return kSCErr_Failed;

    info.format = (unsigned int)(headerFormat | sampleFormat);
    return kSCErr_None;
}

// ------------------------------ platform-specific functions ------------------------------
#    ifdef _WIN32

/** @brief Opens a soundfile from a native file path */
inline SNDFILE* sndfileOpenFromNativeString(LPCWSTR wpath, int mode, SF_INFO* sfinfo) {
    return sf_wchar_open(wpath, mode, sfinfo);
}

/** @brief Safely opens a sound file using a UTF-8 C-string on any platform */
inline SNDFILE* sndfileOpenFromCStr(const char* utf8Path, int mode, SF_INFO* sfinfo) {
    // convert to wchar first
    const std::wstring wpath = SC_Codecvt::utf8_cstr_to_utf16_wstring(utf8Path);
    return sndfileOpenFromNativeString(wpath.c_str(), mode, sfinfo);
}

/** @brief Safely creates a handle using a UTF-8 C-string on any platform */
inline SndfileHandle makeSndfileHandle(const char* utf8Path, int mode = SFM_READ, int format = 0, int channels = 0,
                                       int samplerate = 0) {
    // convert to wchar first
    const std::wstring wpath = SC_Codecvt::utf8_cstr_to_utf16_wstring(utf8Path);
    return SndfileHandle(wpath.c_str(), mode, format, channels, samplerate);
}

#    else // not _WIN32

/** @brief Opens a soundfile from a native file path */
inline SNDFILE* sndfileOpenFromNativeString(const char* path, int mode, SF_INFO* sfinfo) {
    return sf_open(path, mode, sfinfo);
}

/** @brief Safely opens a sound file using a UTF-8 C-string on any platform */
inline SNDFILE* sndfileOpenFromCStr(const char* utf8Path, int mode, SF_INFO* sfinfo) {
    return sndfileOpenFromNativeString(utf8Path, mode, sfinfo); // just forward
}

/** @brief Safely creates a handle using a UTF-8 C-string on any platform */
inline SndfileHandle makeSndfileHandle(const char* utf8Path, int mode = SFM_READ, int format = 0, int channels = 0,
                                       int samplerate = 0) {
    return SndfileHandle(utf8Path, mode, format, channels, samplerate); // just forward
}

#    endif // _WIN32

/** @brief Safely opens a sound file using a std:filesystem::path on any platform */
inline SNDFILE* sndfileOpenFromPath(const fs::path& path, int mode, SF_INFO* sfinfo) {
    return sndfileOpenFromNativeString(path.native().c_str(), mode, sfinfo);
}

/** @brief Safely creates a sound file handle using a std:filesystem::path on any platform */
inline SndfileHandle makeSndfileHandle(const fs::path& path, int mode = SFM_READ, int format = 0, int channels = 0,
                                       int samplerate = 0) {
    return SndfileHandle(path.native().c_str(), mode, format, channels, samplerate);
}

#endif /* NO_LIBSNDFILE */
