/*
    Abstract interpreter interface.
    Copyright (c) 2003 2004 stefan kersten.

    ====================================================================

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
#include "SC_Lock.h"
#include <cstring>
#include <string>
#include <cerrno>

#ifdef _WIN32
#    include <stdio.h>
#    include <direct.h>
#    ifndef PATH_MAX
#        define PATH_MAX _MAX_PATH
#    endif
#else
#    include <unistd.h>
#endif

#include "PyrObject.h"
#include "PyrKernel.h"
#include "PyrPrimitive.h"
#include "PyrSched.h"
#include "GC.h"
#include "VMGlobals.h"
#include "SCBase.h"
#include "SC_StringBuffer.h"

void closeAllGUIScreens();
void initGUI();
void initGUIPrimitives();

extern PyrString* newPyrStringN(class PyrGC* gc, std::int64_t length, std::int64_t flags, std::int64_t collect);

// =====================================================================
// SC_LanguageClient
// =====================================================================

SC_LanguageClient* gInstance = nullptr;
SC_Lock gInstanceMutex;

class HiddenLanguageClient {
public:
    HiddenLanguageClient(): mPostFile(nullptr), mScratch(0), mRunning(false) {}

    std::string mName;
    FILE* mPostFile;
    SC_StringBuffer mScratch;
    bool mRunning;
};

SC_LanguageClient::SC_LanguageClient(const std::string& name) {
    mHiddenClient = new HiddenLanguageClient;

    lockInstance();

    if (gInstance) {
        unlockInstance();
        fprintf(stderr, "SC_LanguageClient already running\n");
        abort();
    }

    mHiddenClient->mName = name;
    gInstance = this;

    unlockInstance();
}

SC_LanguageClient::~SC_LanguageClient() {
    lockInstance();
    gInstance = nullptr;
    delete mHiddenClient;
    unlockInstance();
}

void SC_LanguageClient::initRuntime(const Options& opt) {
    // start virtual machine
    if (!mHiddenClient->mRunning) {
        mHiddenClient->mRunning = true;
        if (!opt.mRuntimeDir.empty()) {
            if (chdir(opt.mRuntimeDir.c_str()) != 0)
                error("Cannot change to runtime directory: %s", strerror(errno));
        }
        pyr_init_mem_pools(opt.mMemSpace, opt.mMemGrow);
        init_OSC(opt.mPort);
        schedInit();
        onInitRuntime();
    }
}

void SC_LanguageClient::shutdownRuntime() {
    schedCleanup();
    cleanup_OSC();
}

void SC_LanguageClient::compileLibrary(bool standalone) { ::compileLibrary(standalone); }

extern void shutdownLibrary();
void SC_LanguageClient::shutdownLibrary() {
    ::shutdownLibrary();
    flush();
}

void SC_LanguageClient::recompileLibrary(bool standalone) { compileLibrary(standalone); }

void SC_LanguageClient::setCmdLine(const char* buf, size_t size) {
    if (isLibraryCompiled()) {
        lock();
        if (isLibraryCompiled()) {
            VMGlobals* g = gMainVMGlobals;

            PyrString* strobj = newPyrStringN(g->gc, size, 0, true);
            memcpy(strobj->s, buf, size);

            SetObject(&slotRawInterpreter(&g->process->interpreter)->cmdLine, strobj);
            g->gc->GCWriteNew(slotRawObject(&g->process->interpreter),
                              strobj); // we know strobj is white so we can use GCWriteNew
        }
        unlock();
    }
}

void SC_LanguageClient::setCmdLine(const char* str) { setCmdLine(str, strlen(str)); }

void SC_LanguageClient::setCmdLinef(const char* fmt, ...) {
    SC_StringBuffer& scratch = mHiddenClient->mScratch;
    va_list ap;
    va_start(ap, fmt);
    scratch.reset();
    scratch.vappendf(fmt, ap);
    va_end(ap);
    setCmdLine(scratch.getData());
}

void SC_LanguageClient::runLibrary(PyrSymbol* symbol) {
    lock();
    ::runLibrary(symbol);
    unlock();
}

void SC_LanguageClient::runLibrary(const char* methodName) {
    lock();
    ::runLibrary(getsym(methodName));
    unlock();
}

void SC_LanguageClient::executeFile(const std::string& fileName) {
    std::string escaped_file_name(fileName);
    int i = 0;
    while (i < escaped_file_name.size()) {
        if (escaped_file_name[i] == '\\')
            escaped_file_name.insert(++i, 1, '\\');
        ++i;
    }

    setCmdLinef("thisProcess.interpreter.executeFile(\"%s\")", escaped_file_name.c_str());
    runLibrary(s_interpretCmdLine);
}

void SC_LanguageClient::tick() {
    if (trylock()) {
        if (isLibraryCompiled()) {
            ::runLibrary(s_tick);
        }
        unlock();
    }
    flush();
}

bool SC_LanguageClient::tickLocked(double* nextTime) {
    if (isLibraryCompiled()) {
        ::runLibrary(s_tick);
    }

    return slotDoubleVal(&gMainVMGlobals->result, nextTime) == errNone;
}

void SC_LanguageClient::onInitRuntime() {}

void SC_LanguageClient::onLibraryStartup() {}

void SC_LanguageClient::onLibraryShutdown() {}

void SC_LanguageClient::onInterpStartup() {}

// runLibrary methods
void SC_LanguageClient::interpretCmdLine() { runLibrary(s_interpretCmdLine); }
void SC_LanguageClient::interpretPrintCmdLine() { runLibrary(s_interpretPrintCmdLine); }
void SC_LanguageClient::runMain() { runLibrary(s_run); }
void SC_LanguageClient::stopMain() { runLibrary(s_stop); }

// locks
void SC_LanguageClient::lock() { gLangMutex.lock(); }
bool SC_LanguageClient::trylock() { return gLangMutex.try_lock(); }
void SC_LanguageClient::unlock() { gLangMutex.unlock(); }

SC_LanguageClient* SC_LanguageClient::instance() { return gInstance; }

void SC_LanguageClient::lockInstance() { gInstanceMutex.lock(); }
void SC_LanguageClient::unlockInstance() { gInstanceMutex.unlock(); }

extern bool compiledOK;

const char* SC_LanguageClient::getName() const { return mHiddenClient->mName.c_str(); }

FILE* SC_LanguageClient::getPostFile() { return mHiddenClient->mPostFile; }
void SC_LanguageClient::setPostFile(FILE* file) { mHiddenClient->mPostFile = file; }

bool SC_LanguageClient::isLibraryCompiled() { return compiledOK; }

int SC_LanguageClient::run(int argc, char** argv) {
    throw std::runtime_error("SC_LanguageClient::run only supported on terminal client");
}

// =====================================================================
// library functions
// =====================================================================

void setPostFile(FILE* file) { SC_LanguageClient::instance()->setPostFile(file); }

int vpost(const char* fmt, va_list ap) {
    char buf[512];
    int n = vsnprintf(buf, sizeof(buf), fmt, ap);
    if (n > 0) {
        SC_LanguageClient* client = SC_LanguageClient::lockedInstance();
        if (client)
            client->postText(buf, sc_min(n, sizeof(buf) - 1));
        SC_LanguageClient::unlockInstance();
    }
    return 0;
}

void post(const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vpost(fmt, ap);
    va_end(ap);
}

void postfl(const char* fmt, ...) {
    char buf[512];
    va_list ap;
    va_start(ap, fmt);
    int n = vsnprintf(buf, sizeof(buf), fmt, ap);
    if (n > 0) {
        SC_LanguageClient* client = SC_LanguageClient::lockedInstance();
        if (client)
            client->postFlush(buf, sc_min(n, sizeof(buf) - 1));
        SC_LanguageClient::unlockInstance();
    }
    va_end(ap);
}

void postText(const char* str, std::int64_t len) {
    SC_LanguageClient* client = SC_LanguageClient::lockedInstance();
    if (client)
        client->postFlush(str, len);
    SC_LanguageClient::unlockInstance();
}

void postChar(char c) {
    SC_LanguageClient* client = SC_LanguageClient::lockedInstance();
    if (client)
        client->postFlush(&c, sizeof(char));
    SC_LanguageClient::unlockInstance();
}

void error(const char* fmt, ...) {
    char buf[512];
    va_list ap;
    va_start(ap, fmt);
    int n = vsnprintf(buf, sizeof(buf), fmt, ap);
    if (n > 0) {
        SC_LanguageClient* client = SC_LanguageClient::lockedInstance();
        if (client)
            client->postError(buf, sc_min(n, sizeof(buf) - 1));
        SC_LanguageClient::unlockInstance();
    }
    va_end(ap);
}

void flushPostBuf(void) { SC_LanguageClient::instance()->flush(); }

void closeAllGUIScreens() { SC_LanguageClient::instance()->onLibraryShutdown(); }

void initGUI() { SC_LanguageClient::instance()->onInterpStartup(); }

void initGUIPrimitives() { SC_LanguageClient::instance()->onLibraryStartup(); }

std::int64_t scMIDIout(int port, int len, int statushi, int chan, int data1, int data2);
std::int64_t scMIDIout(int port, int len, int statushi, int chan, int data1, int data2) { return 0; }
// EOF
