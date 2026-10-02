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
#include "SC_Lock.h"

#include <boost/asio.hpp>
#ifdef HAVE_READLINE
#    include <boost/sync/semaphore.hpp>
#endif

#include <vector>

// =====================================================================
// SC_TerminalClient - command line sclang client.
// =====================================================================

// TODO: move locks & thread out of the header, possibly using pimpl
class SCLANG_DLLEXPORT SC_TerminalClient : public SC_LanguageClient {
public:
    enum Signal {
        sig_input = 0x01, /** there is new input */
        sig_sched = 0x02, /** something has been scheduled */
        sig_recompile = 0x04, /** class lib recompilation requested */
        sig_stop = 0x08 /** call Main:-stop */
    };

    struct Options : public SC_LanguageClient::Options {
        std::string mLibraryConfigFile;
        bool mDaemon = false;
        bool mCallRun = false;
        bool mCallStop = false;
        bool mStandalone = false;
        std::vector<std::string> mArgs;
    };

    SC_TerminalClient(const std::string& name);
    virtual ~SC_TerminalClient();

    const Options& options() const { return mOptions; }

    int run(int argc, char** argv);

    /** \brief recompile the Class Library
     *
     *  Always called on the main thread, typically after receiving
     *  the \c sig_recompile signal.
     */
    bool recompileLibrary();

    virtual void postText(const char* str, size_t len);
    virtual void postFlush(const char* str, size_t len);
    virtual void postError(const char* str, size_t len);
    virtual void flush();

    /** \brief Requests an action to be taken on the main thread.
     *  \note It may be called from any thread, with the interpreter locked or unlocked.
     */
    virtual void sendSignal(Signal code);

    /** \brief stop the main loop */
    void stop() { mIoContext.stop(); }

    void setExitCode(int code) { mExitCode = code; }

protected:
    // --------------------------------------------------------------

    /** \brief interpret interactive code
     *  \note subclasses should call this on the main thread after receiving sig_input.
     */
    void interpretInput();

    // --------------------------------------------------------------

    /** \brief Language requested the application to quit
     *  \note It may be called from any thread, and with interpreter locked.
     */
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
    enum class ParseState { FileName, LineNumber, Column, Text, Error, Done };

    struct CmdLine {
        std::string fileName;
        std::string lineNumber;
        std::string column;
        std::string code;
        bool silent = false;
    };

    static constexpr size_t inputBufferSize = 1024;

    // NOTE: called from input thread:
#ifdef HAVE_READLINE
    static void readlineInit();
    static void readlineFunc(SC_TerminalClient*);
    static int readlineRecompile(int, int);
    static void readlineCmdLine(char* cmdLine);
#endif

    void startInput();
    void endInput();

    void inputThreadFn();

    void startInputRead();
    void onInputRead(const boost::system::error_code& error, std::size_t bytes_transferred);

    void handleInput(const char* newData, size_t size);

    void pushCmdLine(CmdLine&& cmdLine);
    bool tryPopCmdLine(CmdLine& cmdLine);

protected:
    // app-clock io context
    boost::asio::io_context mIoContext;

private:
    int mExitCode = 0;
    bool mUseReadline = false;
    Options mOptions;

    boost::asio::executor_work_guard<boost::asio::io_context::executor_type> mWork;
    boost::asio::basic_waitable_timer<std::chrono::system_clock> mTimer;

    // input io service
    boost::asio::io_context mInputContext;
    SC_Thread mInputThread;

    std::vector<char> mInputBuffer;
    CmdLine mNewCmdLine;
    ParseState mParseState = ParseState::Done;
    std::vector<CmdLine> mCmdLineQueue;
    SC_Lock mCmdLineQueueMutex;
#ifndef _WIN32
    boost::asio::posix::stream_descriptor mStdIn;
#else
    boost::asio::windows::object_handle mStdIn;
#endif
#ifdef HAVE_READLINE
    boost::sync::semaphore mReadlineSem;
#endif
};
