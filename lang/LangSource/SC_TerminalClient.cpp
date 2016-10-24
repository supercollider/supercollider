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
#include "../../QtCollider/LanguageClient.h"
#endif

#include <boost/bind.hpp>

#ifdef _WIN32
# define __GNU_LIBRARY__
# include "getopt.h"
# include "SC_Win32Utils.h"
# include <io.h>
# include <windows.h>
# include <ioapiset.h>
#endif

#ifdef __APPLE__
#include "../../common/SC_Apple.hpp"
#endif

#ifdef HAVE_READLINE
# include <readline/readline.h>
# include <readline/history.h>
# include <signal.h>
#endif

#include "GC.h"
#include "PyrKernel.h"
#include "PyrPrimitive.h"
#include "PyrLexer.h"
#include "PyrSlot.h"
#include "VMGlobals.h"
#include "SC_DirUtils.h"   // for gIdeName
#include "SC_LanguageConfig.hpp"
#include "SC_Version.hpp"

static FILE* gPostDest = stdout;

SC_TerminalClient::SC_TerminalClient(const char* name)
	: SC_LanguageClient(name),
	  mReturnCode(0),
	  mUseReadline(false),
      mWork(mIoService),
	  mTimer(mIoService),
#ifndef _WIN32
	  mStdIn(mInputService, STDIN_FILENO)
#else
	  mStdIn(mInputService, GetStdHandle(STD_INPUT_HANDLE))
#endif
{}

SC_TerminalClient::~SC_TerminalClient()
{}

void SC_TerminalClient::postText(const char* str, size_t len)
{
	fwrite(str, sizeof(char), len, gPostDest);
}

void SC_TerminalClient::postFlush(const char* str, size_t len)
{
	fwrite(str, sizeof(char), len, gPostDest);
	fflush(gPostDest);
}

void SC_TerminalClient::postError(const char* str, size_t len)
{
	fprintf(gPostDest, "ERROR: ");
	fwrite(str, sizeof(char), len, gPostDest);
}

void SC_TerminalClient::flush()
{
	fflush(gPostDest);
}

void SC_TerminalClient::printUsage()
{
	Options opt;

	const size_t bufSize = 128;
	char memGrowBuf[bufSize];
	char memSpaceBuf[bufSize];

	snprintMemArg(memGrowBuf, bufSize, opt.mMemGrow);
	snprintMemArg(memSpaceBuf, bufSize, opt.mMemSpace);

	fprintf(stdout, "Usage:\n   %s [options] [file..] [-]\n\n", getName());
	fprintf(stdout,
			"Options:\n"
			"   -v                             Print supercollider version and exit\n"
			"   -d <path>                      Set runtime directory\n"
			"   -D                             Enter daemon mode (no input)\n"
			"   -g <memory-growth>[km]         Set heap growth (default %s)\n"
			"   -h                             Display this message and exit\n"
			"   -l <path>                      Set library configuration file\n"
			"   -m <memory-space>[km]          Set initial heap size (default %s)\n"
			"   -r                             Call Main.run on startup\n"
			"   -s                             Call Main.stop on shutdown\n"
			"   -u <network-port-number>       Set UDP listening port (default %d)\n"
			"   -i <ide-name>                  Specify IDE name (for enabling IDE-specific class code, default \"%s\")\n"
			"   -a                             Standalone mode (exclude SCClassLibrary and user and system Extensions folders from search path)\n",
			memGrowBuf,
			memSpaceBuf,
			opt.mPort,
			gIdeName
		);
}

bool SC_TerminalClient::parseOptions(int& argc, char**& argv, Options& opt)
{
	const char* optstr = ":d:Dg:hl:m:rsu:i:av";
	int c;

	// inhibit error reporting
	opterr = 0;

	while ((c = getopt(argc, argv, optstr)) != -1) {
		switch (c) {
			case 'd':
				opt.mRuntimeDir = optarg;
				break;
			case 'D':
				opt.mDaemon = true;
				break;
			case 'g':
				if (!parseMemArg(optarg, &opt.mMemGrow)) {
					optopt = c;
					goto optArgInvalid;
				}
				break;
			case 'h':
				goto help;
			case 'l':
				opt.mLibraryConfigFile = optarg;
				break;
			case 'm':
				if (!parseMemArg(optarg, &opt.mMemSpace)) {
					optopt = c;
					goto optArgInvalid;
				}
				break;
			case 'r':
				opt.mCallRun = true;
				break;
			case 'v':
				fprintf(stdout, "sclang %s (%s)\n", SC_VersionString().c_str(), SC_BuildString().c_str());
				quit(0);
				return false;
				break;
			case 's':
				opt.mCallStop = true;
				break;
			case 'u':
				if (!parsePortArg(optarg, &opt.mPort)) {
					optopt = c;
					goto optArgInvalid;
				}
				break;
			case '?':
				goto optInvalid;
				break;
			case ':':
				goto optArgExpected;
				break;
			case 'i':
				gIdeName = optarg;
				break;
			case 'a':
				opt.mStandalone = true;
				break;
			default:
				::post("%s: unknown error (getopt)\n", getName());
				quit(255);
				return false;
		}
	}

	argv += optind;
	argc -= optind;

	return true;

 help:
	printUsage();
	quit(0);
	return false;

 optInvalid:
	::post("%s: invalid option -%c\n", getName(), optopt);
	quit(1);
	return false;

 optArgExpected:
	::post("%s: missing argument for option -%c\n", getName(), optopt);
	quit(1);
	return false;

 optArgInvalid:
	::post("%s: invalid argument for option -%c -- %s\n", getName(), optopt, optarg);
	quit(1);
	return false;
}

int SC_TerminalClient::run(int argc, char** argv)
{
	Options& opt = mOptions;

	if (!parseOptions(argc, argv, opt)) {
		return mReturnCode;
	}

	// finish argv processing
	const char* codeFile = 0;

	if (argc > 0) {
		codeFile = argv[0];
		opt.mDaemon = true;
		argv++; argc--;
	}

	opt.mArgc = argc;
	opt.mArgv = argv;

	// read library configuration file
	if (opt.mLibraryConfigFile)
		SC_LanguageConfig::setConfigFile(opt.mLibraryConfigFile);
	SC_LanguageConfig::readLibraryConfig(opt.mStandalone);

	// initialize runtime
	initRuntime(opt);

	// startup library
	compileLibrary(opt.mStandalone);

	// enter main loop
	if (codeFile) executeFile(codeFile);
	if (opt.mCallRun) runMain();

	if (opt.mDaemon) {
		daemonLoop();
	}
	else {
		initInput();
		startInput();
		commandLoop();
		endInput();
		cleanupInput();
	}

	if (opt.mCallStop) stopMain();

	// shutdown library
	shutdownLibrary();
	flush();

	shutdownRuntime();

	return mReturnCode;
}

void SC_TerminalClient::recompileLibrary()
{
    SC_LanguageClient::recompileLibrary(mOptions.mStandalone);
}

void SC_TerminalClient::quit(int code)
{
	mReturnCode = code;
}

static PyrSymbol * resolveMethodSymbol(bool silent)
{
	if (silent)
		return s_interpretCmdLine;
	else
		return s_interpretPrintCmdLine;
}

void SC_TerminalClient::interpretCmdLine(const char* cmdLine, bool silent)
{
	setCmdLine(cmdLine);
	runLibrary(resolveMethodSymbol(silent));
	flush();
}


void SC_TerminalClient::interpretCmdLine(const char *cmdLine, size_t size, bool silent)
{
	setCmdLine(cmdLine, size);
	runLibrary(resolveMethodSymbol(silent));
	flush();
}

// Note: called only if the input thread does not perform an asynchronous read operation
void SC_TerminalClient::interpretInput()
{
	char *data = mInputBuf.getData();
	int c = mInputBuf.getSize();
	int i = 0;
	while( i < c ) {
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

		data += i+1;
		c -= i+1;
		i = 0;
	}
	mInputBuf.reset();

	if (mUseReadline)
		mReadlineSem.post();
	else
		startInputRead();
}

void SC_TerminalClient::onLibraryStartup()
{
	SC_LanguageClient::onLibraryStartup();
	int base, index = 0;
	base = nextPrimitiveIndex();
	definePrimitive(base, index++, "_Argv", &SC_TerminalClient::prArgv, 1, 0);
	definePrimitive(base, index++, "_Exit", &SC_TerminalClient::prExit, 1, 0);
	definePrimitive(base, index++, "_AppClock_SchedNotify",
		&SC_TerminalClient::prScheduleChanged, 1, 0);
	definePrimitive(base, index++, "_Recompile", &SC_TerminalClient::prRecompile, 1, 0);
}

void SC_TerminalClient::sendSignal( Signal sig )
{
	switch (sig) {
	case sig_input:
		mIoService.post( boost::bind(&SC_TerminalClient::interpretInput, this) );
		break;

	case sig_recompile:
		mIoService.post( boost::bind(&SC_TerminalClient::recompileLibrary, this) );
		break;

	case sig_sched:
		mIoService.post( boost::bind(&SC_TerminalClient::tick, this, boost::system::error_code()) );
		break;

	case sig_stop:
		mIoService.post( boost::bind(&SC_TerminalClient::stopMain, this) );
		break;
	}
}

void SC_TerminalClient::onQuit( int exitCode )
{
	postfl("main: quit request %i\n", exitCode);
	quit( exitCode );
	stop();
}

extern void ElapsedTimeToChrono(double elapsed, std::chrono::system_clock::time_point & out_time_point);

void SC_TerminalClient::tick( const boost::system::error_code& error )
{
	mTimer.cancel();

	double secs;
	lock();
	bool haveNext = tickLocked( &secs );
	unlock();

	flush();

	std::chrono::system_clock::time_point nextAbsTime;
	ElapsedTimeToChrono( secs, nextAbsTime );

	if (haveNext) {
		mTimer.expires_at(nextAbsTime);
		mTimer.async_wait(boost::bind(&SC_TerminalClient::tick, this, _1));
	}
}

void SC_TerminalClient::commandLoop()
{
	mIoService.run();
}

void SC_TerminalClient::daemonLoop()
{
	commandLoop();
}

#ifdef HAVE_READLINE

static void sc_rl_cleanlf(void)
{
	rl_reset_line_state();
	rl_crlf();
	rl_redisplay();
}

static void sc_rl_signalhandler(int sig)
{
	// ensure ctrl-C clears line rather than quitting (ctrl-D will quit nicely)
	rl_replace_line("", 0);
	sc_rl_cleanlf();
}

static int sc_rl_mainstop(int i1, int i2)
{
	static_cast<SC_TerminalClient*>(SC_LanguageClient::instance())
		->sendSignal( SC_TerminalClient::sig_stop );
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

int SC_TerminalClient::readlineRecompile(int i1, int i2)
{
	static_cast<SC_TerminalClient*>(SC_LanguageClient::instance())->sendSignal(sig_recompile);
	sc_rl_cleanlf();
	return 0;
}

void SC_TerminalClient::readlineCmdLine( char *cmdLine )
{
	SC_TerminalClient *client = static_cast<SC_TerminalClient*>(instance());

	if( cmdLine == NULL ) {
		postfl("\nExiting sclang (ctrl-D)\n");
		client->onQuit(0);
		return;
	}

	if( *cmdLine != 0 ) {
		// If line wasn't empty, store it so that uparrow retrieves it
		add_history(cmdLine);
		int len = strlen(cmdLine);

		client->mInputBuf.append(cmdLine, len);
		client->mInputBuf.append(kInterpretPrintCmdLine);
		client->sendSignal(sig_input);
		client->mReadlineSem.wait();
	}
}

void SC_TerminalClient::readlineInit()
{
	// Setup readline
	rl_readline_name = "sclang";
	rl_basic_word_break_characters = " \t\n\"\\'`@><=;|&{}().";
	//rl_attempted_completion_function = sc_rl_completion;
	rl_bind_key(CTRL('t'), &sc_rl_mainstop);
	rl_bind_key(CTRL('x'), &readlineRecompile);
	rl_callback_handler_install( "sc3> ", &readlineCmdLine );

	// FIXME: Implement the code below on Windows
#ifndef _WIN32
	// Set our handler for SIGINT that will clear the line instead of terminating.
	// NOTE: We prevent readline from setting its own signal handlers,
	// to not override ours.
	rl_catch_signals = 0;
	struct sigaction sact;
	memset( &sact, 0, sizeof(struct sigaction) );
	sact.sa_handler = &sc_rl_signalhandler;
	sigaction( SIGINT, &sact, 0 );
#endif
}

#endif // HAVE_READLINE

void SC_TerminalClient::startInputRead()
{
#ifndef _WIN32
	if (mUseReadline)
		mStdIn.async_read_some(boost::asio::null_buffers(), boost::bind(&SC_TerminalClient::onInputRead, this, _1, _2));
	else
		mStdIn.async_read_some(boost::asio::buffer(inputBuffer), boost::bind(&SC_TerminalClient::onInputRead, this, _1, _2));
#else
	mStdIn.async_wait( [&] (const boost::system::error_code & error) {
		if(error)
			onInputRead(error, 0);
		else {
			DWORD bytes_transferred;

			::ReadFile(GetStdHandle(STD_INPUT_HANDLE),
					   inputBuffer.data(),
					   inputBuffer.size(),
					   &bytes_transferred,
					   nullptr);

			onInputRead(error, bytes_transferred);
		}
	});
#endif
}

void SC_TerminalClient::onInputRead(const boost::system::error_code &error, std::size_t bytes_transferred)
{
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
		pushCmdLine( inputBuffer.data(), bytes_transferred );
	}
}

void SC_TerminalClient::inputThreadFn()
{
#if HAVE_READLINE
	if (mUseReadline)
		readlineInit();
#endif

	startInputRead();

	boost::asio::io_service::work work(mInputService);
	mInputService.run();
}


void SC_TerminalClient::pushCmdLine( const char *newData, size_t size)
{
	bool signal = false;
	while (size--) {
		char c = *newData++;
		switch (c) {
		case kRecompileLibrary:
		case kInterpretCmdLine:
		case kInterpretPrintCmdLine:
			mInputBuf.append( mInputThrdBuf.getData(), mInputThrdBuf.getSize() );
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


void SC_TerminalClient::initInput()
{
#ifdef HAVE_READLINE
	if (strcmp(gIdeName, "none") == 0) {
		// Other clients (emacs, vim, ...) won't want to interact through rl
		mUseReadline = true;
		return;
	}
#endif
}


void SC_TerminalClient::startInput()
{
	thread thread(std::bind(&SC_TerminalClient::inputThreadFn, this));
	mInputThread = std::move(thread);
}

void SC_TerminalClient::endInput()
{
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

void SC_TerminalClient::cleanupInput()
{
#ifdef HAVE_READLINE
	if( mUseReadline ) rl_callback_handler_remove();
#endif
}

int SC_TerminalClient::prArgv(struct VMGlobals* g, int)
{
	int argc = ((SC_TerminalClient*)SC_TerminalClient::instance())->options().mArgc;
	char** argv = ((SC_TerminalClient*)SC_TerminalClient::instance())->options().mArgv;

	PyrSlot* argvSlot = g->sp;

	PyrObject* argvObj = newPyrArray(g->gc, argc * sizeof(PyrObject), 0, true);
	SetObject(argvSlot, argvObj);

	for (int i=0; i < argc; i++) {
		PyrString* str = newPyrString(g->gc, argv[i], 0, true);
		SetObject(argvObj->slots+i, str);
		argvObj->size++;
		g->gc->GCWriteNew(argvObj, (PyrObject*)str); // we know str is white so we can use GCWriteNew
	}

	return errNone;
}

int SC_TerminalClient::prExit(struct VMGlobals* g, int)
{
	int code;

	int err = slotIntVal(g->sp, &code);
	if (err) return err;

	((SC_TerminalClient*)SC_LanguageClient::instance())->onQuit( code );

	return errNone;
}

int SC_TerminalClient::prScheduleChanged( struct VMGlobals *g, int numArgsPushed)
{
	static_cast<SC_TerminalClient*>(instance())->sendSignal(sig_sched);
	return errNone;
}

int SC_TerminalClient::prRecompile(struct VMGlobals *, int)
{
	static_cast<SC_TerminalClient*>(instance())->sendSignal(sig_recompile);
	return errNone;
}

SCLANG_DLLEXPORT SC_LanguageClient * createLanguageClient(const char * name)
{
	if (SC_LanguageClient::instance())
		return NULL;

#ifdef __APPLE__
	SC::Apple::disableAppNap();
#endif

#ifdef SC_QT
	return new QtCollider::LangClient(name);
#else
	return new SC_TerminalClient(name);
#endif
}

SCLANG_DLLEXPORT void destroyLanguageClient(class SC_LanguageClient * languageClient)
{
	delete languageClient;
}
