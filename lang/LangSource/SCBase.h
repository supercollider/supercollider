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

// Contains the most common definitions.

#pragma once

#include <limits.h>
#include <stdio.h>
#include <cstdint>

#ifdef _WIN32
#    include <stdlib.h>
#    ifndef PATH_MAX
#        define PATH_MAX _MAX_PATH
#    endif
#endif

#include "SC_BoundsMacros.h"
#include "SC_Types.h"
#include "PyrErrors.h"
#include "AllocPools.h"
#include "SC_Export.h"

/**
 * Normally, the client runs in its own main thread which has already access to GUI primitives.
 * This is not the case for the wasm client, which is executing code in a dedicated thread in order to
 * prevent locking of the main thread (aka the browser thread) which results in a browser tab freeze, see
 * https://emscripten.org/docs/porting/pthreads.html#blocking-on-the-main-browser-thread
 */
constexpr bool DEFAULT_THREAD_IS_MAIN_THREAD =
#ifdef __EMSCRIPTEN__
    false;
#else
    true;
#endif

void postfl(const char* fmt, ...);
void post(const char* fmt, ...);
void error(const char* fmt, ...);
void postText(const char* text, std::size_t length);
void postChar(char c);
void flushPostBuf();
void setPostFile(FILE* file); // If file is not NULL, causes all posted text to also be written to the file.

void debugf(char* fmt, ...);
void pprintf(unsigned char* str, char* fmt, ...);

SCLANG_DLLEXPORT_C void schedInit();
SCLANG_DLLEXPORT_C void schedCleanup();
SCLANG_DLLEXPORT_C void init_OSC(int port);
SCLANG_DLLEXPORT_C void cleanup_OSC();
SCLANG_DLLEXPORT_C bool pyr_init_mem_pools(int runtime_space, int runtime_grow);

SCLANG_DLLEXPORT_C void schedRun();
SCLANG_DLLEXPORT_C void schedStop();
SCLANG_DLLEXPORT_C void schedClear();

// MAIN ENTRY POINT OF SC LANG.
SCLANG_DLLEXPORT_C bool compileLibrary(bool wasCompiledPreviously, bool standalone);
/**
 *  All exceptions are caught, except \c FatalInterpreterErrors .
 *  \param runsInMainThread:  determines if this is called from the main thread,
 *  which is the default case except for the WASM client which runs the AppClock on the main thread
 *  but everything else in a dedicated \c gSclangWasmThread
 *  \see DEFAULT_THREAD_IS_MAIN_THREAD
 */
SCLANG_DLLEXPORT_C void runLibrary(struct PyrSymbol* selector,
                                   const bool runsInMainThread = DEFAULT_THREAD_IS_MAIN_THREAD);
SCLANG_DLLEXPORT_C void runInterpreter(struct VMGlobals* g, struct PyrSymbol* selector, int numArgsPushed);
SCLANG_DLLEXPORT_C void shutdownLibrary(bool wasCompiledPreviously);
SCLANG_DLLEXPORT_C void setCommandLine(const char* txt, size_t txtSize, const char* filePath, int lineNumber = 0,
                                       int column = 0);


SCLANG_DLLEXPORT_C struct VMGlobals* scGlobals();

SCLANG_DLLEXPORT_C struct PyrSymbol* getsym(const char* inName);
SCLANG_DLLEXPORT_C struct PyrSymbol* getmetasym(const char* name);
SCLANG_DLLEXPORT_C struct PyrSymbol* findsym(const char* name);
