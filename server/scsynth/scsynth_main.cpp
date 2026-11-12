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


#include "SC_ServerProgramOptions.hpp"
#include "SC_EventLoop.hpp"
#include "SC_ServerBootDelayWarning.h"

#include <cstring>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <math.h>
#ifdef _WIN32
#    include <winsock2.h>
#    include <vector>
#    include "SC_Codecvt.hpp"
#else
#    include <unistd.h> // for _POSIX_MEMLOCK
#    include <sys/wait.h>
#endif

#ifdef __COBALT__
#    include "XenomaiLock.h"
static XenomaiInitializer xenomaiInitializer;
#endif // __COBALT__

#ifdef _WIN32

// according to this page: http://www.mkssoftware.com/docs/man3/setlinebuf.3.asp
// setlinebuf is equivalent to the setvbuf call below.
inline int setlinebuf(FILE* stream) { return setvbuf(stream, (char*)0, _IONBF, 0); }

#endif

int scsynth_main(int argc, char** argv) {
    setlinebuf(stdout);

    ServerProgramOptions options;

    try {
        if (!options.parse(argc, argv))
            return 0;
    } catch (const std::exception& e) {
        scprintf("Error parsing command line arguments:\n%s\n\n", e.what());
        scprintf("Use the -h/--help option for help.\n");
        return EXIT_FAILURE;
    }

    ServerBootDelayWarningTimer bootDelayWarningTimer;
    bootDelayWarningTimer.start();

    EventLoop::setup();

    World* world = World_New(&options);
    if (!world)
        return EXIT_FAILURE;

    if (!options.mRealTime) {
        // Server is ready!
        bootDelayWarningTimer.stop();
#ifdef NO_LIBSNDFILE
        return EXIT_FAILURE;
#else
        try {
            World_NonRealTimeSynthesis(world, &options);
            return 0;
        } catch (const std::exception& exc) {
            scprintf("NRT synthesis failed: %s\n", exc.what());
            World_Cleanup(world, true);
            return EXIT_FAILURE;
        }
#endif
    }

    if (options.mUdpPort.has_value()) {
        if (!World_OpenUDP(world, options.mBindAddressString.c_str(), *options.mUdpPort)) {
            World_Cleanup(world, true);
            return EXIT_FAILURE;
        }
    }
    if (options.mTcpPort.has_value()) {
        if (!World_OpenTCP(world, options.mBindAddressString.c_str(), *options.mTcpPort, options.mMaxLogins, 8)) {
            World_Cleanup(world, true);
            return EXIT_FAILURE;
        }
    }

    // Server is ready!
    bootDelayWarningTimer.stop();

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
    std::vector<std::string> utf8_args;
    std::vector<char*> argv;
    utf8_args.reserve(argc);
    argv.reserve(argc + 1);

    for (int i = 0; i < argc; ++i) {
        utf8_args.push_back(SC_Codecvt::utf16_to_utf8_str(wargv[i]));
    }
    for (auto& str : utf8_args) {
        argv.push_back(str.data());
    }
    argv.push_back(nullptr); // argv termination

    // set codepage to UTF-8 and remember the old codepage
    auto oldCodePage = GetConsoleOutputCP();
    if (!SetConsoleOutputCP(65001))
        scprintf("WARNING: could not set codepage to UTF-8\n");

    // run main
    int result = scsynth_main(argc, argv.data());

    // clean up winsock
    WSACleanup();
    // reset codepage from UTF-8
    SetConsoleOutputCP(oldCodePage);

    return result;
}

#else

#    ifdef __EMSCRIPTEN__
// wasm build uses exported c function as entry
int main(int argc, char** argv) { return 0; }
#    else

int main(int argc, char** argv) { return scsynth_main(argc, argv); };

#    endif
#endif //_WIN32
