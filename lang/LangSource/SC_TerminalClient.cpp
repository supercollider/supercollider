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
#ifdef SC_QT
#    include "../../QtCollider/LanguageClient.h"
#endif

#include <boost/bind.hpp>

#ifdef _WIN32
#    include "SC_Win32Utils.h"
#    include <io.h>
#    include <windows.h>
#    include <ioapiset.h>
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
#include "PyrLexer.h"
#include "PyrSlot.h"
#include "VMGlobals.h"
#include "SC_Filesystem.hpp"
#include "SC_LanguageConfig.hpp"
#include "SC_Version.hpp"

#include <boost/filesystem/operations.hpp>
#include <boost/program_options.hpp>

#include <iostream>

namespace bpo = boost::program_options;

static FILE* gPostDest = stdout;

#ifdef _WIN32
static UINT gOldCodePage; // for remembering the old codepage when we switch to UTF-8
#endif

SC_TerminalClient::SC_TerminalClient(const char* name):
    SC_LanguageClient(name),
    mReturnCode(0),
    mUseReadline(false),
    mWork(mIoService),
    mTimer(mIoService),
#ifndef _WIN32
    mStdIn(mInputService, STDIN_FILENO)
#else
    mStdIn(mInputService, GetStdHandle(STD_INPUT_HANDLE))
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

static std::string memArgValueToString(int value) {
    constexpr size_t bufSize = 128;
    char buf[bufSize];
    SC_LanguageClient::snprintMemArg(buf, bufSize, value);
    return buf;
}

static bpo::options_description makeSclangOptionsDesc(SC_TerminalClient::Options& opts) {
    SC_TerminalClient::Options defaultOpts;
    bpo::options_description result("Options");

    // clang-format off
    result.add_options()
        ("version,v", "Print version information and exit")
        ("help,h", "Display this message and exit")
        ("runtime-dir,d", bpo::value<std::string>(&opts.mRuntimeDirStdString), "Set runtime directory")
        ("daemon,D", bpo::bool_switch(&opts.mDaemon), "Enter daemon mode (no input)")
        ("heap-growth,g", bpo::value<std::string>()->default_value(memArgValueToString(defaultOpts.mMemGrow)), "Set heap growth in bytes. You can use 'k' and 'm' suffixes to mean kilobytes and megabytes respectively.")
        ("config,l", bpo::value<std::string>(&opts.mLibraryConfigFile), "Library configuration file")
        ("heap-size,m", bpo::value<std::string>()->default_value(memArgValueToString(defaultOpts.mMemSpace)), "Set initial heap size. You can use 'k' and 'm' suffixes to mean kilobytes and megabytes respectively.")
        ("call-main-run,r", bpo::bool_switch(&opts.mCallRun), "Call Main.run on startup")
        ("call-main-stop,s", bpo::bool_switch(&opts.mCallStop), "Call Main.stop on shutdown")
        ("udp-port,u", bpo::value<std::string>()->default_value(std::to_string(defaultOpts.mPort)), "Set UDP listening port (0-65535)")
        ("ide-name,i", bpo::value<std::string>(&opts.mIdeName)->default_value(SC_Filesystem::instance().getIdeName()), "Specify IDE name (for enabling IDE-specific class code)")
        ("standalone,a", bpo::bool_switch(&opts.mStandalone), "Standalone mode (exclude SCClassLibrary and user and system Extension folders from compilation paths)")
        ;
    // clang-format on

    return result;
}

void SC_TerminalClient::printUsage() const {
    Options opts;
    auto optionsDesc = makeSclangOptionsDesc(opts);
    std::cout
        << "Usage:\n    " << getName() << " [options] [file [args...]]\n\n"
        << optionsDesc
        << "\nIf [file] is provided, it is run as SuperCollider code.\n"
           "Additional arguments are available in `thisProcess.argv`; otherwise they have no effect"
        << std::endl;
}

void SC_TerminalClient::printVersion() const {
    fprintf(stdout, "sclang %s (%s)\n", SC_VersionString().c_str(), SC_BuildString().c_str());
}

bool SC_TerminalClient::parseOptions(int argc, char** argv, Options& opts) {
    auto optionsDesc = makeSclangOptionsDesc(opts);

    bpo::positional_options_description positionalOpts;
    positionalOpts.add("file", 1);
    positionalOpts.add("additional-args", -1);

    // These are kept in a separate options_description to hide them from the usage message
    bpo::options_description positionalOptsDesc;
    // clang-format off
    positionalOptsDesc.add_options()
        ("file", bpo::value<std::string>(&opts.mCodeFile), "")
        ("additional-args", bpo::value<std::vector<std::string>>(&opts.mAdditionalArgs), "")
        ;
    // clang-format on
    optionsDesc.add(positionalOptsDesc);

    auto&& quitEarly = [this](int returnCode) {
        quit(returnCode);
        return false;
    };

    auto&& fatalError = [this, &quitEarly](const char* msg) {
        ::post("Error while parsing command line: %s\nUse -h to see usage.\n", msg);
        return quitEarly(1);
    };

    bpo::variables_map vm;
    try {
        // If successful, populates `opts`
        store(bpo::command_line_parser(argc, argv).options(optionsDesc).positional(positionalOpts).run(), vm);
        notify(vm);
    } catch (bpo::error const& e) {
        return fatalError(e.what());
    }

    // Semantic conditions that could cause us to terminate the program early
    if (vm.count("help")) {
        printUsage();
        return quitEarly(0);
    }

    if (vm.count("version")) {
        printVersion();
        return quitEarly(0);
    }

    if (!parseMemArg(vm["heap-size"].as<std::string>().c_str(), &opts.mMemSpace)) {
        return fatalError("could not parse heap-size (-m) option");
    }

    if (!parseMemArg(vm["heap-growth"].as<std::string>().c_str(), &opts.mMemGrow)) {
        return fatalError("could not parse heap-growth (-g) option");
    }

    if (!parsePortArg(vm["udp-port"].as<std::string>().c_str(), &opts.mPort)) {
        return fatalError("could not parse udp-port (-u) option");
    }

    // Cannot change type of mRuntimeDir; it's in the public API.
    // mRuntimeDir is never modified, so the const_cast is safe.
    if (!opts.mRuntimeDirStdString.empty())
        opts.mRuntimeDir = const_cast<char*>(opts.mRuntimeDirStdString.c_str());

    return true;
}

int SC_TerminalClient::run(int argc, char** argv) {
    Options& opt = mOptions;

    if (!parseOptions(argc, argv, opt)) {
        return mReturnCode;
    }

    SC_Filesystem::instance().setIdeName(opt.mIdeName);

    // read library configuration file
    if (!opt.mLibraryConfigFile.empty())
        SC_LanguageConfig::setConfigPath(opt.mLibraryConfigFile.c_str());
    SC_LanguageConfig::readLibraryConfig(opt.mStandalone);

    // initialize runtime
    initRuntime(opt);

    // Create config directory so that it can be used by Quarks, etc. See #2919.
    if (!opt.mStandalone && opt.mLibraryConfigFile.empty())
        boost::filesystem::create_directories(
            SC_Filesystem::instance().getDirectory(SC_Filesystem::DirName::UserConfig));

    // startup library
    compileLibrary(opt.mStandalone);

    // enter main loop
    if (!opt.mCodeFile.empty())
        executeFile(opt.mCodeFile.c_str());
    if (opt.mCallRun)
        runMain();

    if (opt.mDaemon) {
        daemonLoop();
    } else {
        initInput();
        startInput();
        commandLoop();
        endInput();
        cleanupInput();
    }

    if (opt.mCallStop)
        stopMain();

    // shutdown library
    shutdownLibrary();
    flush();

    shutdownRuntime();

    return mReturnCode;
}

void SC_TerminalClient::recompileLibrary() { SC_LanguageClient::recompileLibrary(mOptions.mStandalone); }

void SC_TerminalClient::quit(int code) { mReturnCode = code; }

static PyrSymbol* resolveMethodSymbol(bool silent) {
    if (silent)
        return s_interpretCmdLine;
    else
        return s_interpretPrintCmdLine;
}

void SC_TerminalClient::interpretCmdLine(const char* cmdLine, bool silent) {
    setCmdLine(cmdLine);
    runLibrary(resolveMethodSymbol(silent));
    flush();
}


void SC_TerminalClient::interpretCmdLine(const char* cmdLine, size_t size, bool silent) {
    setCmdLine(cmdLine, size);
    runLibrary(resolveMethodSymbol(silent));
    flush();
}

// Note: called only if the input thread does not perform an asynchronous read operation
void SC_TerminalClient::interpretInput() {
    char* data = mInputBuf.getData();
    int c = mInputBuf.getSize();
    int i = 0;
    while (i < c) {
        switch (data[i]) {
        case kInterpretCmdLine:
            interpretCmdLine(data, i, true);
            break;
        case kInterpretPrintCmdLine:
            interpretCmdLine(data, i, false);
            break;

        case kRecompileLibrary:
            recompileLibrary();
            break;

        default:
            ++i;
            continue;
        }

        data += i + 1;
        c -= i + 1;
        i = 0;
    }
    mInputBuf.reset();

    if (mUseReadline)
        mReadlineSem.post();
    else
        startInputRead();
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
        mIoService.post(boost::bind(&SC_TerminalClient::interpretInput, this));
        break;

    case sig_recompile:
        mIoService.post(boost::bind(&SC_TerminalClient::recompileLibrary, this));
        break;

    case sig_sched:
        mIoService.post(boost::bind(&SC_TerminalClient::tick, this, boost::system::error_code()));
        break;

    case sig_stop:
        mIoService.post(boost::bind(&SC_TerminalClient::stopMain, this));
        break;
    }
}

void SC_TerminalClient::onQuit(int exitCode) {
    postfl("main: quit request %i\n", exitCode);
    quit(exitCode);
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

void SC_TerminalClient::commandLoop() { mIoService.run(); }

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
        int len = strlen(cmdLine);

        client->mInputBuf.append(cmdLine, len);
        client->mInputBuf.append(kInterpretPrintCmdLine);
        client->sendSignal(sig_input);
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

    // FIXME: Implement the code below on Windows
#    ifndef _WIN32
    // Set our handler for SIGINT that will clear the line instead of terminating.
    // NOTE: We prevent readline from setting its own signal handlers,
    // to not override ours.
    rl_catch_signals = 0;
    struct sigaction sact;
    memset(&sact, 0, sizeof(struct sigaction));
    sact.sa_handler = &sc_rl_signalhandler;
    sigaction(SIGINT, &sact, nullptr);
#    endif
}

#endif // HAVE_READLINE

void SC_TerminalClient::startInputRead() {
#ifndef _WIN32
    if (mUseReadline)
        mStdIn.async_read_some(boost::asio::null_buffers(), boost::bind(&SC_TerminalClient::onInputRead, this, _1, _2));
    else
        mStdIn.async_read_some(boost::asio::buffer(inputBuffer),
                               boost::bind(&SC_TerminalClient::onInputRead, this, _1, _2));
#else
    mStdIn.async_wait([&](const boost::system::error_code& error) {
        if (error)
            onInputRead(error, 0);
        else {
            DWORD bytes_transferred;

            ::ReadFile(GetStdHandle(STD_INPUT_HANDLE), inputBuffer.data(), inputBuffer.size(), &bytes_transferred,
                       nullptr);

            onInputRead(error, bytes_transferred);
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
        if (mUseReadline) {
            rl_callback_read_char();
            startInputRead();
            return;
        }
#endif
        pushCmdLine(inputBuffer.data(), bytes_transferred);
    }
}

void SC_TerminalClient::inputThreadFn() {
#if HAVE_READLINE
    if (mUseReadline)
        readlineInit();
#endif

#ifdef _WIN32
    // make sure there's nothing on stdin before we launch the service
    // this fixes #4214
    DWORD bytesRead = 0;
    auto success = ReadFile(GetStdHandle(STD_INPUT_HANDLE), inputBuffer.data(), inputBuffer.size(), &bytesRead, NULL);

    if (success) {
        pushCmdLine(inputBuffer.data(), bytesRead);
    }
#endif

    startInputRead();

    boost::asio::io_service::work work(mInputService);
    mInputService.run();
}


void SC_TerminalClient::pushCmdLine(const char* newData, size_t size) {
    bool signal = false;
    while (size--) {
        char c = *newData++;
        switch (c) {
        case kRecompileLibrary:
        case kInterpretCmdLine:
        case kInterpretPrintCmdLine:
            mInputBuf.append(mInputThrdBuf.getData(), mInputThrdBuf.getSize());
            mInputBuf.append(c);
            signal = true;
            mInputThrdBuf.reset();
            break;

        default:
            mInputThrdBuf.append(c);
        }
    }

    if (signal)
        sendSignal(sig_input);
    else
        startInputRead();
}


void SC_TerminalClient::initInput() {
#ifdef HAVE_READLINE
    if (!SC_Filesystem::instance().usingIde()) {
        // Other clients (emacs, vim, ...) won't want to interact through rl
        mUseReadline = true;
        return;
    }
#endif
}


void SC_TerminalClient::startInput() {
    SC_Thread thread(std::bind(&SC_TerminalClient::inputThreadFn, this));
    mInputThread = std::move(thread);
}

void SC_TerminalClient::endInput() {
    mInputService.stop();
    mStdIn.cancel();
#ifdef _WIN32
    // Note this breaks Windows XP compatibility, since this function is only defined in Vista and later
    ::CancelIoEx(GetStdHandle(STD_INPUT_HANDLE), nullptr);
#endif
    postfl("main: waiting for input thread to join...\n");
    mInputThread.join();
    postfl("main: quitting...\n");
}

void SC_TerminalClient::cleanupInput() {
#ifdef HAVE_READLINE
    if (mUseReadline)
        rl_callback_handler_remove();
#endif
}

int SC_TerminalClient::prArgv(struct VMGlobals* g, int) {
    auto args = static_cast<const SC_TerminalClient*>(SC_TerminalClient::instance())->options().mAdditionalArgs;

    PyrSlot* argvSlot = g->sp;

    PyrObject* argvObj = newPyrArray(g->gc, args.size() * sizeof(PyrObject), 0, true);
    SetObject(argvSlot, argvObj); // this is okay here as we don't use the receiver

    for (size_t i = 0; i < args.size(); i++) {
        PyrString* str = newPyrString(g->gc, args[i].c_str(), 0, true);
        SetObject(argvObj->slots + i, str);
        argvObj->size++;
        g->gc->GCWriteNew(argvObj, (PyrObject*)str); // we know str is white so we can use GCWriteNew
    }

    return errNone;
}

int SC_TerminalClient::prExit(struct VMGlobals* g, int) {
    int code;

    int err = slotIntVal(g->sp, &code);
    if (err)
        return err;

    ((SC_TerminalClient*)SC_LanguageClient::instance())->onQuit(code);

    return errNone;
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
