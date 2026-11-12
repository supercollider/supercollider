//  Server program options
//
//  Copyright (C) 2009, 2010 Tim Blechmann, 2026 Christof Ressi
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

#include "SC_WorldOptions.h"

#include <optional>
#include <string>
#include <vector>

#include <boost/asio/ip/address.hpp>

class ServerProgramOptions : public WorldOptions {
public:
    /** @brief parse command line arguments
     *  @returns whether the program should run (`true`) or exit (`false`).
     *  @note returning `false` does *not* indicate an error! This is rather
     *  used for options like `--version` or `--help`. The program should
     *  exit with `EXIT_SUCCESS`.
     *  @throws std::runtime_error if an error occurs.
     */
    bool parse(int argc, char* argv[]);

    // options not included in WorldOptions
    std::optional<int> mUdpPort {};
    std::optional<int> mTcpPort {};
    std::string mBindAddressString { "127.0.0.1" };
    boost::asio::ip::address mBindAddress;

    // Supernova options
#ifdef SUPERNOVA
    uint32_t mNumThreads = 0;
#    if defined(__linux__)
    bool mThreadPinning = true;
#    else
    bool mThreadPinning = false;
#    endif
    uint8_t mUseSystemClock = true;
#endif

private:
    void validateOptions();

    // storage for string arguments
    std::string mPasswordString;
    std::string mRestrictedPathString;
    std::string mUGenPluginPathString;
    std::string mInDeviceNameString;
    std::string mOutDeviceNameString;
#ifdef __APPLE__
    std::string mInputStreams;
    std::string mOutputStreams;
#endif
    // non-real time arguments
    std::string mCommandFile;
    std::string mInputFile;
    std::string mOutputFile;
    std::string mHeaderFormat;
    std::string mSampleFormat;
};
