/*
	Commandline interpreter interface.
	Copyright (c) 2003-2006 stefan kersten.

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

#include <errno.h>
#include <fcntl.h>

#ifdef SC_WIN32
# define __GNU_LIBRARY__
# include "getopt.h"
# include "SC_Win32Utils.h"
# include <io.h>
# include <windows.h>
#else
# include <sys/param.h>
# include <sys/poll.h>
# include <unistd.h>
#endif

#ifdef HAVE_READLINE
# include <readline/readline.h>
# include <readline/history.h>
# include <signal.h>
#endif


#include <string.h>
#include <time.h>

#include "GC.h"
#include "PyrKernel.h"
#include "PyrPrimitive.h"
#include "PyrLexer.h"
#include "PyrSlot.h"
#include "VMGlobals.h"
#include "SC_DirUtils.h"   // for gIdeName
#include "SC_LanguageConfig.hpp"

#define STDIN_FD 0

static FILE* gPostDest = stdout;

static const int ticks_per_second = 50; // every 20 milliseconds

SC_TerminalClient::SC_TerminalClient(const char* name)
	: SC_LanguageClient(name),
	  mShouldBeRunning(false),
	  mReturnCode(0),
	  mUseReadline(false),
	  mSignals(0)
{
	pthread_cond_init (&mCond, NULL);
	pthread_mutex_init(&mSignalMutex, NULL);
	pthread_mutex_init(&mInputMutex, NULL);
	pthread_cond_init(&mInputCond, NULL);
}

SC_TerminalClient::~SC_TerminalClient()
{
	pthread_cond_destroy (&mCond);
	pthread_mutex_destroy(&mSignalMutex);
	pthread_mutex_destroy(&mInputMutex);
	pthread_cond_destroy(&mInputCond);
}

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
			"   -d <path>                      Set runtime directory\n"
			"   -D                             Enter daemon mode (no input)\n"
			"   -g <memory-growth>[km]         Set heap growth (default %s)\n"
			"   -h                             Display this message and exit\n"
			"   -l <path>                      Set library configuration file\n"
			"   -m <memory-space>[km]          Set initial heap size (default %s)\n"
			"   -r                             Call Main.run on startup\n"
			"   -s                             Call Main.stop on shutdown\n"
			"   -u <network-port-number>       Set UDP listening port (default %d)\n"
			"   -i <ide-name>                  Specify IDE name (for enabling IDE-specific class code, default \"%s\")\n",
			memGrowBuf,
			memSpaceBuf,
			opt.mPort,
			gIdeName
		);
}

bool SC_TerminalClient::parseOptions(int& argc, char**& argv, Options& opt)
{
	const char* optstr = ":d:Dg:hl:m:rsu:i:";
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
	if (opt.mLibraryConfigFile) {
		int argLength = strlen(opt.mLibraryConfigFile);
		SC_LanguageConfig::readLibraryConfigYAML(opt.mLibraryConfigFile);
	} else
		SC_LanguageConfig::readDefaultLibraryConfig();

	// initialize runtime
	initRuntime(opt);

	// startup library
	mShouldBeRunning = true;
	compileLibrary();

	// enter main loop
	if (codeFile) executeFile(codeFile);
	if (opt.mCallRun) runMain();

	if (opt.mDaemon) {
		daemonLoop();
	}
	else {
		initInput();
		if( shouldBeRunning() ) startInput();
		if( shouldBeRunning() ) commandLoop();
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

void SC_TerminalClient::quit(int code)
{
	mReturnCode = code;
	mShouldBeRunning = false;
}

void SC_TerminalClient::interpretCmdLine(PyrSymbol* method, SC_StringBuffer& cmdLine)
{
	setCmdLine(cmdLine);
	cmdLine.reset();
	runLibrary(method);
	flush();
}

void SC_TerminalClient::interpretCmdLine(PyrSymbol* method, const char* cmdLine)
{
	setCmdLine(cmdLine);
	runLibrary(method);
	flush();
}


void SC_TerminalClient::interpretCmdLine(PyrSymbol* method, const char *cmdLine, size_t size)
{
	setCmdLine(cmdLine, size);
	runLibrary(method);
	flush();
}

// WARNING: Call with input locked!
void SC_TerminalClient::interpretInput()
{
	char *data = mInputBuf.getData();
	int c = mInputBuf.getSize();
	int i = 0;
	while( i < c ) {
		switch (data[i]) {
		case kInterpretCmdLine:
			interpretCmdLine(s_interpretCmdLine, data, i);
			break;
		case kInterpretPrintCmdLine:
			interpretCmdLine(s_interpretPrintCmdLine, data, i);
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
	if( mUseReadline ) pthread_cond_signal( &mInputCond );
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
	lockSignal();
	mSignals |= sig;
	pthread_cond_signal( &mCond );
	unlockSignal();
}

void SC_TerminalClient::onQuit( int exitCode )
{
	lockSignal();
	postfl("main: quit request %i\n", exitCode);
	quit( exitCode );
	pthread_cond_signal( &mCond );
	unlockSignal();
}

extern void ElapsedTimeToTimespec(double elapsed, struct timespec *spec);

void SC_TerminalClient::commandLoop()
{
	bool haveNext = false;
	struct timespec nextAbsTime;

	lockSignal();

	while( shouldBeRunning() )
	{

		while ( mSignals ) {
			int sig = mSignals;

			unlockSignal();

			if (sig & sig_input) {
				//postfl("input\n");
				lockInput();
				interpretInput();
				// clear input signal, as we've processed anything signalled so far.
				lockSignal();
				mSignals &= ~sig_input;
				unlockSignal();
				unlockInput();
			}

			if (sig & sig_sched) {
				//postfl("tick\n");
				double secs;
				lock();
				haveNext = tickLocked( &secs );
				// clear scheduler signal, as we've processed all items scheduled up to this time.
				// and will enter the wait according to schedule.
				lockSignal();
				mSignals &= ~sig_sched;
				unlockSignal();
				unlock();

				flush();

				//postfl("tick -> next time = %f\n", haveNext ? secs : -1);
				ElapsedTimeToTimespec( secs, &nextAbsTime );
			}

			if (sig & sig_stop) {
				stopMain();
				lockSignal();
				mSignals &= ~sig_stop;
				unlockSignal();
			}

			if (sig & sig_recompile) {
				recompileLibrary();
				lockSignal();
				mSignals &= ~sig_recompile;
				unlockSignal();
			}

			lockSignal();
		}

		if( !shouldBeRunning() ) {
			break;
		}
		else if( haveNext ) {
			int result = pthread_cond_timedwait( &mCond, &mSignalMutex, &nextAbsTime );
			if( result == ETIMEDOUT ) mSignals |= sig_sched;
		}
		else {
			pthread_cond_wait( &mCond, &mSignalMutex );
		}
	}

	unlockSignal();
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

	if(*cmdLine!=0){
		// If line wasn't empty, store it so that uparrow retrieves it
		add_history(cmdLine);
		int len = strlen(cmdLine);

		client->lockInput();
		client->mInputBuf.append(cmdLine, len);
		client->mInputBuf.append(kInterpretPrintCmdLine);
		client->sendSignal(sig_input);
		// Wait for input to be processed,
		// so that its output is displayed before readline prompt.
		if (client->mInputShouldBeRunning)
			pthread_cond_wait( &client->mInputCond, &client->mInputMutex );
		client->unlockInput();
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

#ifndef _WIN32

void *SC_TerminalClient::readlineFunc( void *arg )
{
	readlineInit();

	SC_TerminalClient *client = static_cast<SC_TerminalClient*>(arg);

	fd_set fds;
	FD_ZERO(&fds);

	while(true) {
		FD_SET(STDIN_FD, &fds);
		FD_SET(client->mInputCtlPipe[0], &fds);

		if( select(FD_SETSIZE, &fds, NULL, NULL, NULL) < 0 ) {
			if( errno == EINTR ) continue;
			postfl("readline: select() error:\n%s\n", strerror(errno));
			client->onQuit(1);
			break;
		}

		if( FD_ISSET(client->mInputCtlPipe[0], &fds) ) {
			postfl("readline: quit requested\n");
			break;
		}

		if( FD_ISSET(STDIN_FD, &fds) ) {
			rl_callback_read_char();
		}
	}

	postfl("readline: stopped.\n");

	return NULL;
}

#else

void *SC_TerminalClient::readlineFunc( void *arg )
{
	readlineInit();

	SC_TerminalClient *client = static_cast<SC_TerminalClient*>(arg);

	HANDLE hStdIn = GetStdHandle(STD_INPUT_HANDLE);
	HANDLE hnds[] = { client->mQuitInputEvent, hStdIn };

	bool shouldRun = true;
	while (shouldRun) {
		DWORD result = WaitForMultipleObjects( 2, hnds, false, INFINITE );

		if( result == WAIT_FAILED ) {
			postfl("readline: wait error.\n");
			client->onQuit(1);
			break;
		}

		int hIndex = result - WAIT_OBJECT_0;

		if( hIndex == 0 ) {
			postfl("readline: quit requested.\n");
			break;
		}

		if( hIndex == 1 ) {
			rl_callback_read_char();
		}
	}

	postfl("readline: stopped.\n");

	return NULL;
}
#endif // !_WIN32

#endif // HAVE_READLINE

#ifndef _WIN32

void *SC_TerminalClient::pipeFunc( void *arg )
{
	SC_TerminalClient *client = static_cast<SC_TerminalClient*>(arg);

	ssize_t bytes;
	const size_t toRead = 256;
	char buf[toRead];
	SC_StringBuffer stack;

	fd_set fds;
	FD_ZERO(&fds);

	bool shouldRead = true;
	while(shouldRead) {
		FD_SET(STDIN_FD, &fds);
		FD_SET(client->mInputCtlPipe[0], &fds);

		if( select(FD_SETSIZE, &fds, NULL, NULL, NULL) < 0 ) {
			if( errno == EINTR ) continue;
			postfl("pipe-in: select() error: %s\n", strerror(errno));
			client->onQuit(1);
			break;
		}

		if( FD_ISSET(client->mInputCtlPipe[0], &fds) ) {
			postfl("pipe-in: quit requested\n");
			break;
		}

		if( FD_ISSET(STDIN_FD, &fds) ) {

			while(true) {
				bytes = read( STDIN_FD, buf, toRead );

				if( bytes > 0 ) {
					client->pushCmdLine( stack, buf, bytes );
				}
				else if( bytes == 0 ) {
					postfl("pipe-in: EOF. Will quit.\n");
					client->onQuit(0);
					shouldRead = false;
					break;
				}
				else {
					if( errno == EAGAIN ) {
						break; // no more to read this time;
					}
					else if( errno != EINTR ){
						postfl("pipe-in: read() error: %s\n", strerror(errno));
						client->onQuit(1);
						shouldRead = false;
						break;
					}
				}
			}
		}
	}

	postfl("pipe-in: stopped.\n");

	return NULL;
}

#else

void *SC_TerminalClient::pipeFunc( void *arg )
{
	SC_TerminalClient *client = static_cast<SC_TerminalClient*>(arg);
	SC_StringBuffer stack;
	HANDLE hStdIn = GetStdHandle(STD_INPUT_HANDLE);
	char buf[256];
	while(1) {
		DWORD n;
		BOOL ok = ReadFile( hStdIn, &buf, 256, &n, NULL );
		if(ok) {
			client->pushCmdLine( stack, buf, n );
		}
		else {
			postfl("pipe-in: ERROR (ReadFile): %i\n", GetLastError());
			client->onQuit(1);
			break;
		}
	}
	return NULL;
}

#endif

void SC_TerminalClient::pushCmdLine( SC_StringBuffer &buf, const char *newData, size_t size)
{
	lockInput();

	bool signal = false;

	while (size--) {
		char c = *newData++;
		switch (c) {
		case kRecompileLibrary:
		case kInterpretCmdLine:
		case kInterpretPrintCmdLine:
			mInputBuf.append( buf.getData(), buf.getSize() );
			mInputBuf.append(c);
			signal = true;
			buf.reset();
			break;

		default:
			buf.append(c);
		}
	}

	if(signal) sendSignal(sig_input);

	unlockInput();
}




void SC_TerminalClient::initInput()
{

#ifndef _WIN32
	if( pipe( mInputCtlPipe ) == -1 ) {
		postfl("Error creating pipe for input thread control:\n%s\n", strerror(errno));
		quit(1);
	}
#else
	mQuitInputEvent = CreateEvent( NULL, false, false, NULL );
	if( mQuitInputEvent == NULL ) {
		postfl("Error creating event for input thread control.\n");
		quit(1);
	}
#endif

#ifdef HAVE_READLINE
	if (strcmp(gIdeName, "none") == 0) {
		// Other clients (emacs, vim, ...) won't want to interact through rl
		mUseReadline = true;
		return;
	}
#endif

#ifndef _WIN32
	if( fcntl( STDIN_FD, F_SETFL, O_NONBLOCK ) == -1 ) {
		postfl("Error setting up non-blocking pipe reading:\n%s\n", strerror(errno));
		quit(1);
	}
#endif // !_WIN32
}


void SC_TerminalClient::startInput()
{
	mInputShouldBeRunning = true;
#ifdef HAVE_READLINE
	if( mUseReadline )
		pthread_create( &mInputThread, NULL, &SC_TerminalClient::readlineFunc, this );
	else
#endif
		pthread_create( &mInputThread, NULL, &SC_TerminalClient::pipeFunc, this );
}

void SC_TerminalClient::endInput()
{
	// NOTE: On Windows, there is no way to safely interrupt
	// the pipe-reading thread. So just quit and let it die.

#ifdef _WIN32
	if (mUseReadline) {
#endif
	// wake up the input thread in case it is waiting
	// for input to be processed
	lockInput();
		mInputShouldBeRunning = false;
		pthread_cond_signal( &mInputCond );
	unlockInput();

#ifndef _WIN32
	char c = 'q';
	ssize_t bytes = write( mInputCtlPipe[1], &c, 1 );
	if( bytes < 1 )
		postfl("WARNING: could not send quit command to input thread.\n");
#else
	SetEvent( mQuitInputEvent );
#endif

	postfl("main: waiting for input thread to join...\n");
	pthread_join( mInputThread, NULL );

#ifdef _WIN32
	} // if (mUseReadline)
#endif
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
		g->gc->GCWrite(argvObj, (PyrObject*)str);
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
// EOF
