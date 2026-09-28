/*
    Commandline interpreter interface.
    Copyright (c) 2003-2006 stefan kersten.
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

#include "SC_TerminalClient.h"

#include "SC_CLIOptions.hpp"
#include "SC_LanguageClient.h"

#include <cstdlib>

#ifdef SC_QT
#    include "../../QtCollider/LanguageClient.h"
#endif

#include <boost/bind/bind.hpp>

#ifdef _WIN32
#    include <windows.h>
#    include <iostream> // for cerr
#endif

#ifdef __APPLE__
#    include "../../common/SC_Apple.hpp"
#endif

#ifdef HAVE_READLINE
#    include <readline/readline.h>
#    include <readline/history.h>
#    include <signal.h>
#endif

#include "GC.h"
#include "PyrKernel.h"
#include "PyrPrimitive.h"
#include "PyrSlot.h"
#include "VMGlobals.h"
#include "SC_Filesystem.hpp"
#include "SC_LanguageConfig.hpp"
#include "SC_Version.hpp"

#include <filesystem>

using namespace boost::placeholders;

static FILE* gPostDest = stdout;

#ifdef _WIN32
static UINT gOldCodePage; // for remembering the old codepage when we switch to UTF-8
#endif

SC_TerminalClient::SC_TerminalClient(const std::string& name):
    SC_LanguageClient(name),
    mWork(boost::asio::make_work_guard(mIoContext)),
    mTimer(mIoContext),
#ifndef _WIN32
    mStdIn(mInputContext, STDIN_FILENO)
#else
    mStdIn(mInputContext, GetStdHandle(STD_INPUT_HANDLE))
#endif
{
}

SC_TerminalClient::~SC_TerminalClient() {}

void SC_TerminalClient::postText(const char* str, size_t len) { fwrite(str, sizeof(char), len, gPostDest); }

void SC_TerminalClient::postFlush(const char* str, size_t len) {
    fwrite(str, sizeof(char), len, gPostDest);
    fflush(gPostDest);
}

void SC_TerminalClient::postError(const char* str, size_t len) {
    fprintf(gPostDest, "ERROR: ");
    fwrite(str, sizeof(char), len, gPostDest);
}

void SC_TerminalClient::flush() { fflush(gPostDest); }

int SC_TerminalClient::run(int argc, char** argv) {
    Options& opt = mOptions;

    SC_CLI::CLIOptions cliOptions;

    if (const auto errorCode = cliOptions.parse(argc, argv, mOptions)) {
        return *errorCode;
    }

    // initialize runtime
    initRuntime(opt);

    // Create config directory so that it can be used by Quarks, etc. See #2919.
    if (!opt.mStandalone && opt.mLibraryConfigFile.empty())
        std::filesystem::create_directories(SC_Filesystem::instance().getDirectory(SC_Filesystem::DirName::UserConfig));

    // startup library

    if (!compileLibrary(opt.mStandalone)) {
        post("ERROR: Library has not been compiled successfully.\n");
        shutdownLibrary();
        flush();
        shutdownRuntime();
        return EXIT_FAILURE;
    }

    // enter main loop
    if (!cliOptions.mInputFile.empty())
        executeFile(cliOptions.mInputFile);
    if (opt.mCallRun)
        runMain();

    if (opt.mDaemon) {
        daemonLoop();
    } else {
        startInput();
        commandLoop();
        endInput();
    }

    if (opt.mCallStop)
        stopMain();

    // shutdown library
    shutdownLibrary();
    flush();

    shutdownRuntime();

    return mExitCode;
}

bool SC_TerminalClient::recompileLibrary() { return SC_LanguageClient::recompileLibrary(mOptions.mStandalone); }

static PyrSymbol* resolveMethodSymbol(bool silent) {
    if (silent)
        return s_interpretCmdLine;
    else
        return s_interpretPrintCmdLine;
}

void SC_TerminalClient::interpretInput() {
    CmdLine cmdLine;
    if (!tryPopCmdLine(cmdLine)) {
        postfl("ERROR: interpretInput() called with empty cmdline queue!\n");
        return;
    }

    const char* fileName = !cmdLine.fileName.empty() ? cmdLine.fileName.c_str() : nullptr;
    const int lineNumber = !cmdLine.lineNumber.empty() ? std::atoi(cmdLine.lineNumber.c_str()) : 0;
    const int column = !cmdLine.column.empty() ? std::atoi(cmdLine.column.c_str()) : 0;

    setCmdLine(cmdLine.code.data(), cmdLine.code.size(), fileName, lineNumber, column);

    runLibrary(resolveMethodSymbol(cmdLine.silent));

    flush();

#ifdef HAVE_READLINE
    if (mUseReadline)
        mReadlineSem.post();
#endif
}

void SC_TerminalClient::onLibraryStartup() {
    SC_LanguageClient::onLibraryStartup();
    int base, index = 0;
    base = nextPrimitiveIndex();
    definePrimitive(base, index++, "_Argv", &SC_TerminalClient::prArgv, 1, 0);
    definePrimitive(base, index++, "_Exit", &SC_TerminalClient::prExit, 1, 0);
    definePrimitive(base, index++, "_AppClock_SchedNotify", &SC_TerminalClient::prScheduleChanged, 1, 0);
    definePrimitive(base, index++, "_Recompile", &SC_TerminalClient::prRecompile, 1, 0);
}

void SC_TerminalClient::sendSignal(Signal sig) {
    switch (sig) {
    case sig_input:
        // postfl("sig_input\n");
        boost::asio::post(mIoContext, [this] { this->interpretInput(); });
        break;

    case sig_recompile:
        // postfl("sig_recompile\n");
        boost::asio::post(mIoContext, [this] { this->recompileLibrary(); });
        break;

    case sig_sched:
        // postfl("sig_sched\n");
        boost::asio::post(mIoContext, [this] { this->tick(boost::system::error_code()); });
        break;

    case sig_stop:
        // postfl("sig_stop\n");
        boost::asio::post(mIoContext, [this] { this->stopMain(); });
        break;
    }
}

void SC_TerminalClient::onQuit(int exitCode) {
    postfl("main: quit request %i\n", exitCode);
    mExitCode = exitCode;
    stop();
}

extern void ElapsedTimeToChrono(double elapsed, std::chrono::system_clock::time_point& out_time_point);

/**
 * \brief Repeatedly calls the main AppClock loop.
 *
 * This method does the following:
 *
 * - Wraps \c tickLocked (locking \c gLangMutex before and after), which in
 *   turn calls \c runLibrary.
 * - Schedules to call itself again using a \c boost::asio::basic_waitable_timer
 *   (\c mTimer).
 *
 * Since it calls itself, a single call to \c tick will cause the method to
 * keep calling itself autonomously.
 *
 * If \c tick is called again externally while a timer is running, any
 * previously scheduled \c tick call is canceled. This forces a premature \c
 * tick, which will schedule itself again, and so on.
 *
 * The timed calls to \c tick are used for events scheduled on the AppClock.
 * The interruption feature is used when sclang receives unanticipated events
 * such as inbound OSC messages.
 */
void SC_TerminalClient::tick(const boost::system::error_code& error) {
    /*
    Even if the timeout is canceled, this callback will always fire (see docs
    for boost::asio::basic_waitable_timer).

    Distinguishing between a canceled and successful callback is done by
    inspecting the error. If it turns out this method was called because of a
    cancelled timer, we need to bail out and let the tick call that
    interrupted us take over.

    If we aren't the result of a canceled timer call, we're either the result
    of a scheduled timer expiry, or we *are* the interruption and we need to
    cancel any scheduled tick calls. Calling mTimer.cancel() if the error is
    a success error code handles both cases.

    Previously, instead of this if/else block, this was just a call to
    mTimer.cancel(). This was causing this method to rapidly call itself
    excessively, hogging the CPU. See discussion at #2144.
    */
    if (error == boost::system::errc::success) {
        mTimer.cancel();
    } else {
        return;
    }

    double secs;
    lock();
    bool haveNext = tickLocked(&secs);
    unlock();

    flush();

    std::chrono::system_clock::time_point nextAbsTime;
    ElapsedTimeToChrono(secs, nextAbsTime);

    if (haveNext) {
        mTimer.expires_at(nextAbsTime);
        mTimer.async_wait(boost::bind(&SC_TerminalClient::tick, this, _1));
    }
}

void SC_TerminalClient::commandLoop() { mIoContext.run(); }

void SC_TerminalClient::daemonLoop() { commandLoop(); }

#ifdef HAVE_READLINE

static void sc_rl_cleanlf(void) {
    rl_reset_line_state();
    rl_crlf();
    rl_redisplay();
}

static void sc_rl_signalhandler(int sig) {
    // ensure ctrl-C clears line rather than quitting (ctrl-D will quit nicely)
    rl_replace_line("", 0);
    sc_rl_cleanlf();
#    ifdef _WIN32
    // need to re-instate the handler on windows
    signal(SIGINT, &sc_rl_signalhandler);
#    endif
}

static int sc_rl_mainstop(int i1, int i2) {
    static_cast<SC_TerminalClient*>(SC_LanguageClient::instance())->sendSignal(SC_TerminalClient::sig_stop);
    sc_rl_cleanlf(); // We also push a newline so that there's some UI feedback
    return 0;
}

/*
// Completion from sclang dictionary TODO
char ** sc_rl_completion (const char *text, int start, int end);
char ** sc_rl_completion (const char *text, int start, int end){
    char **matches = (char **)NULL;
    printf("sc_rl_completion(%s, %i, %i)\n", text, start, end);
    return matches;
}
*/

int SC_TerminalClient::readlineRecompile(int i1, int i2) {
    static_cast<SC_TerminalClient*>(SC_LanguageClient::instance())->sendSignal(sig_recompile);
    sc_rl_cleanlf();
    return 0;
}

void SC_TerminalClient::readlineCmdLine(char* cmdLine) {
    SC_TerminalClient* client = static_cast<SC_TerminalClient*>(instance());

    if (cmdLine == nullptr) {
        postfl("\nExiting sclang (ctrl-D)\n");
        client->onQuit(0);
        return;
    }

    if (*cmdLine != 0) {
        // If line wasn't empty, store it so that uparrow retrieves it
        add_history(cmdLine);

        // push to queue and notify main thread
        CmdLine cmd;
        cmd.code = cmdLine;
        client->pushCmdLine(std::move(cmd));
        client->sendSignal(sig_input);

        // wait for cmdline to finish to avoid garbled console output. See interpretInput()
        client->mReadlineSem.wait();
    }
}

void SC_TerminalClient::readlineInit() {
    // Setup readline
    rl_readline_name = "sclang";
    rl_basic_word_break_characters = " \t\n\"\\'`@><=;|&{}().";
    // rl_attempted_completion_function = sc_rl_completion;
    rl_bind_key(CTRL('t'), &sc_rl_mainstop);
    rl_bind_key(CTRL('x'), &readlineRecompile);
    rl_callback_handler_install("sc3> ", &readlineCmdLine);

    // Set our handler for SIGINT that will clear the line instead of terminating.
    // NOTE: We prevent readline from setting its own signal handlers,
    // to not override ours.
    rl_catch_signals = 0;
#    ifndef _WIN32
    struct sigaction sact;
    memset(&sact, 0, sizeof(struct sigaction));
    sact.sa_handler = &sc_rl_signalhandler;
    sigaction(SIGINT, &sact, nullptr);
#    else
    signal(SIGINT, &sc_rl_signalhandler);
#    endif
}

#endif // HAVE_READLINE

void SC_TerminalClient::startInputRead() {
#ifndef _WIN32
    if (mUseReadline)
        mStdIn.async_read_some(boost::asio::null_buffers(), boost::bind(&SC_TerminalClient::onInputRead, this, _1, _2));
    else
        mStdIn.async_read_some(boost::asio::buffer(mInputBuffer),
                               boost::bind(&SC_TerminalClient::onInputRead, this, _1, _2));
#else
    mStdIn.async_wait([&](const boost::system::error_code& error) {
        if (error)
            onInputRead(error, 0);
        else {
            if (!mUseReadline) {
                DWORD bytes_transferred;
                ::ReadFile(GetStdHandle(STD_INPUT_HANDLE), mInputBuffer.data(), mInputBuffer.size(), &bytes_transferred,
                           nullptr);
                onInputRead(error, bytes_transferred);
            } else {
                onInputRead(error, 0);
            }
        }
    });
#endif
}

void SC_TerminalClient::onInputRead(const boost::system::error_code& error, std::size_t bytes_transferred) {
    if (error == boost::asio::error::operation_aborted) {
        postfl("SCLang Input: Quit requested\n");
        return;
    }

    if (error == boost::asio::error::eof) {
        postfl("SCLang Input: EOF. Will quit.\n");
        onQuit(0);
        return;
    }

    if (error) {
        postfl("SCLang Input: %s.\n", error.message().c_str());
        onQuit(1);
        return;
    }

    if (!error) {
#if HAVE_READLINE
        if (mUseReadline)
            rl_callback_read_char();
        else
#endif
            handleInput(mInputBuffer.data(), bytes_transferred);

        // read more input data
        startInputRead();
    }
}

void SC_TerminalClient::inputThreadFn() {
#if HAVE_READLINE
    if (mUseReadline)
        readlineInit();
#endif

    startInputRead();

    auto work = boost::asio::make_work_guard(mInputContext);
    mInputContext.run();
}

/** @brief parse and handle input data
 *
 *  This is called by \c onInputRead() when data has been received on the input
 *  thread and no error has occurred. It handles special control characters
 *  according to our protocol (see \c InputProtocol) and sends the appropriate
 *  signals to the main thread.
 */
void SC_TerminalClient::handleInput(const char* newData, size_t size) {
    // postfl("received %d bytes\n", size);
    // postfl("%s", std::string(newData, size).c_str());
    for (size_t i = 0; i < size; ++i) {
        char c = newData[i];
        if (c == RecompileLibrary) {
            // postfl("input: RecompileLibrary\n");
            // let's reset the input parser in case of a previous error
            mNewCmdLine = {};
            mParseState = ParseState::Done;
            // notify main thread
            sendSignal(sig_recompile);
        } else if (c == StartOfHeader) {
            // postfl("input: StartOfHeader\n");
            if (mParseState == ParseState::Done) {
                // receive file name
                assert(mNewCmdLine.fileName.empty());
                mParseState = ParseState::FileName;
            } else {
                mParseState = ParseState::Error;
                postfl("ERROR: handleInput: StartOfText out of context.\n");
            }
        } else if (c == RecordDelimiter) {
            // postfl("input: RecordDelimiter\n");
            if (mParseState == ParseState::FileName) {
                // -> continue with line number
                mParseState = ParseState::LineNumber;
            } else if (mParseState == ParseState::LineNumber) {
                // -> continue with column
                mParseState = ParseState::Column;
            } else {
                postfl("ERROR: handleInput: RecordDelimiter out of context.\n");
                mParseState = ParseState::Error;
            }
        } else if (c == StartOfText) {
            // postfl("input: StartOfText\n");
            if (mParseState == ParseState::Column) {
                // -> continue with text
                assert(mNewCmdLine.code.empty());
                mParseState = ParseState::Text;
            } else {
                postfl("ERROR: handleInput: StartOfText out of context.\n");
                mParseState = ParseState::Error;
            }
        } else if (c == InterpretCmdLine || c == InterpretPrintCmdLine) {
            const bool silent = (c == InterpretCmdLine);
            // postfl("input: Interpret%sCmdLine\n", silent ? "" : "Print");
            if (mParseState == ParseState::Text) {
                mNewCmdLine.silent = silent;
                // cmdline finished -> move to queue and notify main thread
                pushCmdLine(std::move(mNewCmdLine));
                sendSignal(sig_input);
            } else {
                // just drop the current cmdline
                postfl("ERROR: handleInput: InterpretCmdLine out of context.\n");
            }
            // reset parser!
            mNewCmdLine = {};
            mParseState = ParseState::Done;
        } else {
            // normal character
            switch (mParseState) {
            case ParseState::FileName:
                mNewCmdLine.fileName.push_back(c);
                break;
            case ParseState::LineNumber:
                mNewCmdLine.lineNumber.push_back(c);
                break;
            case ParseState::Column:
                mNewCmdLine.column.push_back(c);
                break;
            case ParseState::Text:
                mNewCmdLine.code.push_back(c);
                break;
            case ParseState::Done:
                // first character after Interpret(Print)CmdLine (without header)
                // -> transition to Text!
                mParseState = ParseState::Text;
                mNewCmdLine.code.push_back(c);
                break;
            default:
                // ignore
                break;
            }
        }
    }
}

void SC_TerminalClient::pushCmdLine(CmdLine&& cmdLine) {
    lock_guard lock(mCmdLineQueueMutex);
    mCmdLineQueue.push_back(std::move(cmdLine));
}

bool SC_TerminalClient::tryPopCmdLine(CmdLine& cmdLine) {
    lock_guard lock(mCmdLineQueueMutex);
    if (!mCmdLineQueue.empty()) {
        cmdLine = std::move(mCmdLineQueue.front());
        mCmdLineQueue.erase(mCmdLineQueue.begin());
        return true;
    }
    return false;
}

void SC_TerminalClient::startInput() {
#ifdef HAVE_READLINE
    if (!SC_Filesystem::instance().usingIde()) {
        // Other clients (emacs, vim, ...) won't want to interact through rl
        mUseReadline = true;
    }
#endif

    mInputBuffer.resize(inputBufferSize);

    SC_Thread thread(std::bind(&SC_TerminalClient::inputThreadFn, this));
    mInputThread = std::move(thread);
}

void SC_TerminalClient::endInput() {
    mInputContext.stop();
    mStdIn.cancel();
#ifdef _WIN32
    // Note this breaks Windows XP compatibility, since this function is only defined in Vista and later
    ::CancelIoEx(GetStdHandle(STD_INPUT_HANDLE), nullptr);
#endif
    postfl("main: waiting for input thread to join...\n");
    mInputThread.join();
    postfl("main: quitting...\n");

#ifdef HAVE_READLINE
    if (mUseReadline)
        rl_callback_handler_remove();
#endif
}

int SC_TerminalClient::prArgv(struct VMGlobals* g, int) {
    auto& argv = static_cast<SC_TerminalClient*>(SC_TerminalClient::instance())->options().mArgs;
    size_t argc = argv.size();

    PyrSlot* argvSlot = g->sp;

    PyrObject* argvObj = newPyrArray(g->gc, argc * sizeof(PyrObject), 0, true);
    SetObject(argvSlot, argvObj); // this is okay here as we don't use the receiver

    for (int i = 0; i < argc; i++) {
        PyrString* str = newPyrString(g->gc, argv[i].c_str(), 0, true);
        SetObject(argvObj->slots + i, str);
        argvObj->size++;
        g->gc->GCWriteNew(argvObj, (PyrObject*)str); // we know str is white so we can use GCWriteNew
    }

    return errNone;
}

int SC_TerminalClient::prExit(struct VMGlobals* g, int numArgsPushed) {
    int code;

    int err = slotIntVal(g->sp, &code);
    if (err)
        return err;

    ((SC_TerminalClient*)SC_LanguageClient::instance())->onQuit(code);
    switchToThread(g, slotRawThread(&g->process->mainThread), tDone, &numArgsPushed);
    // return all the way out.
    // PyrSlot *bottom = g->gc->Stack()->slots;
    // slotCopy(bottom,g->sp);
    // g->sp = bottom; // ??!! pop everybody
    g->method = nullptr;
    g->block = nullptr;
    g->frame = nullptr;
    SetNil(g->sp);
    longjmp(g->escapeInterpreter, 3);
    // hmm need to fix this to work only on main thread. //!!!
    // g->sp = g->gc->Stack()->slots - 1;

    return errReturn;
}

int SC_TerminalClient::prScheduleChanged(struct VMGlobals* g, int numArgsPushed) {
    static_cast<SC_TerminalClient*>(instance())->sendSignal(sig_sched);
    return errNone;
}

int SC_TerminalClient::prRecompile(struct VMGlobals*, int) {
    static_cast<SC_TerminalClient*>(instance())->sendSignal(sig_recompile);
    return errNone;
}

SCLANG_DLLEXPORT SC_LanguageClient* createLanguageClient(const char* name) {
    if (SC_LanguageClient::instance())
        return nullptr;

#ifdef __APPLE__
    SC::Apple::disableAppNap();
#endif

#ifdef _WIN32
    // set codepage to UTF-8
    gOldCodePage = GetConsoleOutputCP();
    if (!SetConsoleOutputCP(65001))
        std::cerr << "WARNING: could not set codepage to UTF-8" << std::endl;
#endif

#ifdef SC_QT
    return new QtCollider::LangClient(name);
#else
    return new SC_TerminalClient(name);
#endif
}

SCLANG_DLLEXPORT void destroyLanguageClient(class SC_LanguageClient* languageClient) {
#ifdef _WIN32
    // reset codepage from UTF-8
    SetConsoleOutputCP(gOldCodePage);
#endif
    delete languageClient;
}
