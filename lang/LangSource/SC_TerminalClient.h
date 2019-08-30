/*  -*- c++ -*-
    Commandline interpreter interface.
    Copyright (c) 2003 2004 stefan kersten.
    Copyright (c) 2013 tim blechmann.

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

#pragma once

#include "SC_LanguageClient.h"
#include "SC_StringBuffer.h"
#include "SC_Lock.h"

#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/sync/semaphore.hpp>

// =====================================================================
// SC_TerminalClient - command line sclang client.
// =====================================================================

// TODO: move locks & thread out of the header, possibly using pimpl
class SCLANG_DLLEXPORT SC_TerminalClient : public SC_LanguageClient {
public:
    enum {
        kInterpretCmdLine = 0x1b,
        kInterpretPrintCmdLine = 0x0c,
        kRecompileLibrary = 0x18 // ctrl+x
    };

    enum Signal {
        sig_input = 0x01, // there is new input
        sig_sched = 0x02, // something has been scheduled
        sig_recompile = 0x04, // class lib recompilation requested
        sig_stop = 0x08 // call Main:-stop
    };

    struct Options : public SC_LanguageClient::Options {
        Options():
            mLibraryConfigFile(0),
            mDaemon(false),
            mCallRun(false),
            mCallStop(false),
            mStandalone(false),
            mArgc(0),
            mArgv(0) {}

        const char* mLibraryConfigFile;
        bool mDaemon;
        bool mCallRun;
        bool mCallStop;
        bool mStandalone;
        int mArgc;
        char** mArgv;
    };

    SC_TerminalClient(const char* name);
    virtual ~SC_TerminalClient();

    const Options& options() const { return mOptions; }

    int run(int argc, char** argv);
    void quit(int code);
    void recompileLibrary();

    virtual void postText(const char* str, size_t len);
    virtual void postFlush(const char* str, size_t len);
    virtual void postError(const char* str, size_t len);
    virtual void flush();

    // Requests an action to be taken on the main thread.
    // NOTE: It may be called from any thread, and with interpreter locked.
    virtual void sendSignal(Signal code);

    void stop() { mIoService.stop(); }

protected:
    bool parseOptions(int& argc, char**& argv, Options& opt);
    void printUsage();

    void interpretCmdLine(const char* cmdLine, bool silent);
    void interpretCmdLine(const char* buf, size_t size, bool silent);

    // --------------------------------------------------------------

    // NOTE: Subclasses should call from main thread
    // after receiving sig_input
    void interpretInput();

    // --------------------------------------------------------------

    // Language requested the application to quit
    // NOTE: It may be called from any thread, and with interpreter locked.
    virtual void onQuit(int exitCode);

    // See super class
    virtual void onLibraryStartup();

    // --------------------------------------------------------------

    // NOTE: Subclasses should override:
    virtual void commandLoop();
    virtual void daemonLoop();

    // --------------------------------------------------------------

    static int prArgv(struct VMGlobals* g, int);
    static int prExit(struct VMGlobals* g, int);
    static int prScheduleChanged(struct VMGlobals*, int);
    static int prRecompile(struct VMGlobals*, int);

    void tick(const boost::system::error_code& error);

private:
    // NOTE: called from input thread:
#ifdef HAVE_READLINE
    static void readlineInit();
    static void readlineFunc(SC_TerminalClient*);
    static int readlineRecompile(int, int);
    static void readlineCmdLine(char* cmdLine);
#endif
    static void* pipeFunc(void*);
    void pushCmdLine(const char* newData, size_t size);

    void initInput();
    void startInput();
    void endInput();
    void cleanupInput();

    int mReturnCode;
    Options mOptions;

    // app-clock io service
protected:
    boost::asio::io_service mIoService;

private:
    boost::asio::io_service::work mWork;
    boost::asio::basic_waitable_timer<std::chrono::system_clock> mTimer;

    // input io service
    boost::asio::io_service mInputService;
    SC_Thread mInputThread;
    void inputThreadFn();

    static const size_t inputBufferSize = 256;
    boost::array<char, inputBufferSize> inputBuffer;
    SC_StringBuffer mInputThrdBuf;
    SC_StringBuffer mInputBuf;
#ifndef _WIN32
    boost::asio::posix::stream_descriptor mStdIn;
#else
    boost::asio::windows::object_handle mStdIn;
#endif
    void startInputRead();
    void onInputRead(const boost::system::error_code& error, std::size_t bytes_transferred);

    // command input
    bool mUseReadline;
    boost::sync::semaphore mReadlineSem;
};
