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


#include "SC_WorldOptions.h"
#include "SC_Version.hpp"
#include "SC_EventLoop.hpp"
#include <cstdint>
#include <cstring>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <math.h>
#include "clz.h"
#include <stdexcept>
#ifdef _WIN32
#    include <winsock2.h>
#    include <vector>
#else
#    include <sys/wait.h>
#endif

#include <boost/program_options.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/join.hpp>

#ifdef _WIN32

// according to this page: http://www.mkssoftware.com/docs/man3/setlinebuf.3.asp
// setlinebuf is equivalent to the setvbuf call below.
inline int setlinebuf(FILE* stream) { return setvbuf(stream, (char*)0, _IONBF, 0); }

#endif

namespace bpo = boost::program_options;

namespace {

// std::string versions of WorldOptions fields to ease integration with boost::program_options
struct WorldOptionsStrings {
    std::string mPassword;
    std::string mNonRealTimeCmdFilename;
    std::string mNonRealTimeInputFilename;
    std::string mNonRealTimeOutputFilename;
    std::string mNonRealTimeOutputHeaderFormat;
    std::string mNonRealTimeOutputSampleFormat;
    std::string mInputStreamsEnabled;
    std::string mOutputStreamsEnabled;
    std::string mInDeviceName;
    std::string mUGensPluginPath;
    std::string mOutDeviceName;
    std::string mRestrictedPath;
};

struct ScsynthOptions : WorldOptions {
    uint32_t mUdpPort = 0;
    uint32_t mTcpPort = 0;
    std::string mSocketAddress = "127.0.0.1";

    WorldOptionsStrings stringOpts;
};

} // anonymous namespace

static bpo::options_description makeScsynthOptionsDesc(ScsynthOptions& opts) {
    ScsynthOptions defaultOpts;
    bpo::options_description result("scsynth options");

    // clang-format off
    result.add_options()
        ("help,h", "show this help message")
        ("udp-port,u", bpo::value<uint32_t>(&opts.mUdpPort)->default_value(defaultOpts.mUdpPort), "UDP port")
        ("tcp-port,t", bpo::value<uint32_t>(&opts.mTcpPort)->default_value(defaultOpts.mTcpPort), "TCP port")
        ("control-busses,c", bpo::value<uint32_t>(&opts.mNumControlBusChannels)->default_value(defaultOpts.mNumControlBusChannels), "number of control busses")
        ("audio-busses,a", bpo::value<uint32_t>(&opts.mNumAudioBusChannels)->default_value(defaultOpts.mNumAudioBusChannels), "number of audio busses")
        ("block-size,z", bpo::value<uint32_t>(&opts.mBufLength)->default_value(defaultOpts.mBufLength), "audio block size")
        ("hardware-buffer-size,Z", bpo::value<uint32_t>(&opts.mPreferredHardwareBufferFrameSize)->default_value(defaultOpts.mPreferredHardwareBufferFrameSize), "hardware buffer size")
        ("use-system-clock,C", bpo::value<uint16_t>(), "unused")
        ("samplerate,S", bpo::value<uint32_t>(&opts.mPreferredSampleRate)->default_value(defaultOpts.mPreferredSampleRate), "hardware sample rate")
        ("buffers,b", bpo::value<uint32_t>(&opts.mNumBuffers)->default_value(defaultOpts.mNumBuffers), "number of sample buffers")
        ("max-nodes,n", bpo::value<uint32_t>(&opts.mMaxNodes)->default_value(defaultOpts.mMaxNodes), "maximum number of server nodes")
        ("max-synthdefs,d", bpo::value<uint32_t>(&opts.mMaxGraphDefs)->default_value(defaultOpts.mMaxGraphDefs), "maximum number of synthdefs")
        ("rt-memory,m", bpo::value<uint32_t>(&opts.mRealTimeMemorySize)->default_value(defaultOpts.mRealTimeMemorySize), "size of real-time memory pool in kb")
        ("wires,w", bpo::value<uint32_t>(&opts.mMaxWireBufs)->default_value(defaultOpts.mMaxWireBufs), "number of wire buffers")
        ("randomseeds,r", bpo::value<uint32_t>(&opts.mNumRGens)->default_value(defaultOpts.mNumRGens), "number of random number generators")
        ("load-synthdefs,D", bpo::value<uint32_t>(&opts.mLoadGraphDefs)->default_value(defaultOpts.mLoadGraphDefs), "load synthdefs? (1 or 0)")
        ("rendezvous,R", bpo::value<uint16_t>()->default_value(1), "publish to Rendezvous? (1 or 0)")
        ("max-logins,l", bpo::value<uint32_t>(&opts.mMaxLogins)->default_value(defaultOpts.mMaxLogins), "maximum number of named return addresses")
        ("password,p", bpo::value<std::string>(&opts.stringOpts.mPassword),
                                                            "When using TCP, the session password must be the first command sent.\n"
                                                            "The default is no password.\n"
                                                            "UDP ports never require passwords, so for security use TCP.")
        ("nrt,N", bpo::value<std::vector<std::string>>()->multitoken(), "nrt synthesis <cmd-filename> <input-filename> <output-filename> <sample-rate> <header-format> <sample-format>")
        ("memory-locking,L", "enable memory locking")
        ("version,v", "print version information and exit")
        ("hardware-device-name,H", bpo::value<std::vector<std::string>>()->multitoken(), "hardware device name")
        ("verbose,V", bpo::value<int>(&opts.mVerbosity)->default_value(defaultOpts.mVerbosity),
             "0 is normal behaviour.\n"
             "-1 suppresses informational messages.\n"
             "-2 suppresses informational and many error messages, as well as messages from Poll.\n")
#ifdef _WIN32
        ("ugen-search-path,U", bpo::value<std::vector<std::string>>(), "A list of paths separated by `;`.\n"
                                                            "If specified, standard paths are NOT searched for plugins.\nMay be specified several times.")
#else
        ("ugen-search-path,U", bpo::value<std::vector<std::string>>(), "A list of paths separated by `:`.\n"
                                                            "If specified, standard paths are NOT searched for plugins.\nMay be specified several times.")
#endif
        ("restricted-path,P", bpo::value<std::string>(&opts.stringOpts.mRestrictedPath), "if specified, prevents file-accessing OSC commands from accessing files outside <restricted-path>")
        ("socket-address,B", bpo::value<std::string>()->default_value("127.0.0.1"),
             "Bind the UDP or TCP socket to this address. Set to 0.0.0.0 to listen on all interfaces.")
        ("inchannels,i", bpo::value<uint32_t>(&opts.mNumInputBusChannels)->default_value(defaultOpts.mNumInputBusChannels), "number of input channels")
        ("outchannels,o", bpo::value<uint32_t>(&opts.mNumOutputBusChannels)->default_value(defaultOpts.mNumOutputBusChannels), "number of output channels")
#ifdef __APPLE__
        ("input-streams-enabled,I", bpo::value<std::string>(&opts.stringOpts.mInputStreamsEnabled), "input streams enabled")
        ("output-streams-enabled,O", bpo::value<std::string>(&opts.stringOpts.mOutputStreamsEnabled), "output streams enabled")
#endif // __APPLE__
        ;
    // clang-format on

    return result;
}

static void printUsage() {
    ScsynthOptions scsynthOpts;
    auto opts = makeScsynthOptionsDesc(scsynthOpts);
    std::ostringstream oss;
    opts.print(oss);
    scprintf("%s\nTo quit, send a 'quit' command via UDP or TCP, or press ctrl-C.\n\n", oss.str().c_str());
}

// Options are parsed both to a new ScsynthOptions but also stored in `vm`. `vm` must be destroyed after the return
// value.
static ScsynthOptions parseScsynthArgs(int argc, char** argv) {
    ScsynthOptions options;
    auto optionsDesc = makeScsynthOptionsDesc(options);

    auto fatalError = [](const char* msg) {
        scprintf("ERROR: Could not parse command line: %s\n", msg);
        printUsage();
        std::exit(1);
    };

    bpo::variables_map vm;
    try {
        // If successful, populates `options`
        store(bpo::command_line_parser(argc, argv).options(optionsDesc).run(), vm);
    } catch (bpo::error const& e) {
        fatalError(e.what());
    }

    if (vm.count("help")) {
        printUsage();
        std::exit(0);
    }

    if (vm.count("version")) {
        scprintf("scsynth %s (%s)\n", SC_VersionString().c_str(), SC_BuildString().c_str());
        std::exit(0);
    }

    // Manually parse NRT arguments
    if (vm.count("nrt")) {
        auto&& nrtOptions = vm["nrt"].as<std::vector<std::string>>();
        if (nrtOptions.size() != 6)
            fatalError("nrt option requires 6 arguments <cmd-filename> <input-filename> <output-filename> "
                       "<sample-rate> <header-format> <sample-format>");

        options.mRealTime = false;
        options.stringOpts.mNonRealTimeCmdFilename = nrtOptions[0];
        options.stringOpts.mNonRealTimeInputFilename = nrtOptions[1];
        options.stringOpts.mNonRealTimeOutputFilename = nrtOptions[2];
        try {
            options.mPreferredSampleRate = boost::lexical_cast<uint32_t>(nrtOptions[3]);
        } catch (boost::bad_lexical_cast const& e) {
            fatalError("third argument to nrt option must be an integer");
        }
        options.stringOpts.mNonRealTimeOutputHeaderFormat = nrtOptions[4];
        options.stringOpts.mNonRealTimeOutputSampleFormat = nrtOptions[5];
    }

    // 2 args: assign to in and out respectively
    // 1 arg: assign to both in and out
    if (vm.count("hardware-device-name")) {
        auto&& hwName = vm["hardware-device-name"].as<std::vector<std::string>>();
        if (hwName.size() != 1 && hwName.size() != 2)
            fatalError("hardware-device-name option requires 1 or 2 arguments");

        options.stringOpts.mInDeviceName = hwName[0];
        options.stringOpts.mOutDeviceName = hwName.back();
    }

    // Join together all ugen search paths with platform specific separator
    if (vm.count("ugen-search-path")) {
        auto&& ugen = vm["hardware-device-name"].as<std::vector<std::string>>();
#ifdef _WIN32
        options.stringOpts.mUGensPluginPath = boost::algorithm::join(ugen, ";");
#else
        options.stringOpts.mUGensPluginPath = boost::algorithm::join(ugen, ":");
#endif // _WIN32
    }

    // Round some args up to the nearest power of 2
    options.mBufLength = NEXTPOWEROFTWO(options.mBufLength);
    options.mPreferredSampleRate = NEXTPOWEROFTWO(options.mPreferredSampleRate);
    options.mNumBuffers = NEXTPOWEROFTWO(options.mNumBuffers);
    options.mMaxNodes = NEXTPOWEROFTWO(options.mMaxNodes);
    options.mMaxGraphDefs = NEXTPOWEROFTWO(options.mMaxGraphDefs);
    options.mMaxLogins = NEXTPOWEROFTWO(options.mMaxLogins);

    // Set boolean options manually
    options.mRendezvous = vm["rendezvous"].as<uint16_t>() > 0;
    options.mMemoryLocking = vm.count("memory-locking") > 0;

    // Manually populate const char* fields from the corresponding string option
    // Use a macro to avoid typos and for readability
#define POPULATE_STRING_FIELD(_field_) options._field_ = options.stringOpts._field_.c_str()

    POPULATE_STRING_FIELD(mPassword);
    POPULATE_STRING_FIELD(mNonRealTimeCmdFilename);
    POPULATE_STRING_FIELD(mNonRealTimeInputFilename);
    POPULATE_STRING_FIELD(mNonRealTimeOutputFilename);
    POPULATE_STRING_FIELD(mNonRealTimeOutputHeaderFormat);
    POPULATE_STRING_FIELD(mNonRealTimeOutputSampleFormat);
    POPULATE_STRING_FIELD(mInputStreamsEnabled);
    POPULATE_STRING_FIELD(mOutputStreamsEnabled);
    POPULATE_STRING_FIELD(mInDeviceName);
    POPULATE_STRING_FIELD(mUGensPluginPath);
    POPULATE_STRING_FIELD(mOutDeviceName);
    POPULATE_STRING_FIELD(mRestrictedPath);

#undef POPULATE_STRING_FIELD

    return options;
}

int scsynth_main(int argc, char** argv) {
    setlinebuf(stdout);

    EventLoop::setup();

    auto options = parseScsynthArgs(argc, argv);

    // parseScsynthArgs takes care of syntax/grammar errors, now we check semantic errors

    auto fatalError = [](const char* msg) {
        scprintf("ERROR: %s\n", msg);
        printUsage();
        std::exit(1);
    };

#if (_POSIX_MEMLOCK - 0) < 200112L
    if (options.mMemoryLocking)
        fatalError("memory locking not available");
#endif

    if (options.mUdpPort == 0 && options.mTcpPort == 0 && options.mRealTime) {
        fatalError("there must be a -u and/or a -t options, or -N for nonrealtime");
    }
    if (options.mNumInputBusChannels + options.mNumOutputBusChannels > options.mNumAudioBusChannels) {
        fatalError("number of audio bus channels < inputs + outputs");
    }

    if (options.mRealTime) {
        options.mSharedMemoryID = options.mUdpPort > 0 ? options.mUdpPort : options.mTcpPort;
    } else
        options.mSharedMemoryID = 0;

    struct World* world = World_New(&options);
    if (!world)
        return 1;

    if (!options.mRealTime) {
#ifdef NO_LIBSNDFILE
        fatalError("scsynth built without libsndfile support");
#else
        int exitCode = 0;
        try {
            World_NonRealTimeSynthesis(world, &options);
        } catch (std::exception& exc) {
            scprintf("%s\n", exc.what());
            exitCode = 1;
        }
        return exitCode;
#endif
    }

    if (options.mUdpPort > 0) {
        if (!World_OpenUDP(world, options.mSocketAddress.c_str(), options.mUdpPort)) {
            World_Cleanup(world, true);
            return 1;
        }
    }
    if (options.mTcpPort > 0) {
        if (!World_OpenTCP(world, options.mSocketAddress.c_str(), options.mTcpPort, options.mMaxLogins, 8)) {
            World_Cleanup(world, true);
            return 1;
        }
    }

    if (options.mVerbosity >= 0) {
#ifdef NDEBUG
        scprintf("SuperCollider 3 server ready.\n");
#else
        scprintf("SuperCollider 3 server ready (debug build).\n");
#endif
    }
    fflush(stdout);

    EventLoop::run([world]() { World_WaitForQuit(world, true); });

    return 0;
}

#ifdef _WIN32

int wmain(int argc, wchar_t** wargv) {
    // initialize winsock
    WSAData wsaData;
    int nCode;
    if ((nCode = WSAStartup(MAKEWORD(1, 1), &wsaData)) != 0) {
        scprintf("WSAStartup() failed with error code %d.\n", nCode);
        return 1;
    }

    // convert args to utf-8
    std::vector<char*> argv;
    for (int i = 0; i < argc; i++) {
        auto argSize = WideCharToMultiByte(CP_UTF8, 0, wargv[i], -1, nullptr, 0, nullptr, nullptr);
        argv.push_back(new char[argSize]);
        WideCharToMultiByte(CP_UTF8, 0, wargv[i], -1, argv[i], argSize, nullptr, nullptr);
    }

    // set codepage to UTF-8 and remember the old codepage
    auto oldCodePage = GetConsoleOutputCP();
    if (!SetConsoleOutputCP(65001))
        scprintf("WARNING: could not set codepage to UTF-8\n");

    // run main
    int result = scsynth_main(argv.size(), argv.data());

    // clean up winsock
    WSACleanup();
    // reset codepage from UTF-8
    SetConsoleOutputCP(oldCodePage);
    // clear vector with converted args
    for (auto* arg : argv)
        delete[] arg;

    return result;
}

#else

int main(int argc, char** argv) { return scsynth_main(argc, argv); };

#endif //_WIN32
