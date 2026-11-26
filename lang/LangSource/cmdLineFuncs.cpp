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

#include "SC_LanguageClient.h"

#ifdef _WIN32
#    include "SC_Codecvt.hpp"
#    include <string>
#    include <vector>
#endif

#ifndef __EMSCRIPTEN__
static int runLanguageClient(int argc, char** argv) {
    SC_LanguageClient* client = createLanguageClient("sclang");
    if (!client)
        return 1;
    int returnCode = client->run(argc, argv);
    destroyLanguageClient(client);
    return returnCode;
}
#endif

#if defined(_WIN32)
int wmain(int argc, wchar_t** wargv) {
    std::vector<std::string> utf8Args;
    std::vector<char*> argv;
    utf8Args.reserve(argc);
    argv.reserve(argc + 1);
    for (int i = 0; i < argc; ++i)
        utf8Args.push_back(SC_Codecvt::utf16_to_utf8_str(wargv[i]));
    for (auto& arg : utf8Args)
        argv.push_back(arg.data());
    argv.push_back(nullptr);
    return runLanguageClient(argc, argv.data());
}
#elif !defined(__EMSCRIPTEN__)
int main(int argc, char** argv) { return runLanguageClient(argc, argv); }
#else
// wasm build exposes bootInterpreter through function
// this is still part of the sclang build b/c of add_executable
int main(int, char**) { return 0; }
#endif
