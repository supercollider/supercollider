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
# include "getopt.h"
# include "SC_Win32Utils.h"
# include <io.h>
#else
# include <sys/param.h>
# include <sys/poll.h>
# include <unistd.h>
# ifdef HAVE_READLINE
#   include <readline/readline.h>
#   include <readline/history.h>
#   include <signal.h>
# endif
#endif

#include <string.h>
#include <time.h>

#include "GC.h"
#include "PyrKernel.h"
#include "PyrPrimitive.h"
#include "PyrSlot.h"
#include "VMGlobals.h"
#include "SC_DirUtils.h"   // for gIdeName

static FILE* gPostDest = stdout;

static const int ticks_per_second = 50; // every 20 milliseconds

SC_TerminalClient::SC_TerminalClient(const char* name)
	: SC_LanguageClient(name),
	  mShouldBeRunning(false),
	  mReturnCode(0)
{
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
	bool success;
	if (opt.mLibraryConfigFile) {
		readLibraryConfig(opt.mLibraryConfigFile, opt.mLibraryConfigFile);
	} else {
		readDefaultLibraryConfig();
	}

	// initialize runtime
	initRuntime(opt);

	// startup library
	mShouldBeRunning = true;
	compileLibrary();

	// enter main loop
	if (codeFile) executeFile(codeFile);
	if (opt.mCallRun) runMain();

	if (opt.mDaemon) daemonLoop();
	else commandLoop();

	if (opt.mCallStop) stopMain();

	// shutdown library
	shutdownLibrary();
	flush();

	return mReturnCode;
}

void SC_TerminalClient::quit(int code)
{
	mReturnCode = code;
	mShouldBeRunning = false;
}

bool SC_TerminalClient::readCmdLine(int fd, SC_StringBuffer& cmdLine)
{
	const int bufSize = 256;
	char buf[bufSize];

	int n = read(fd, buf, bufSize);

	if (n > 0) {
		char* ptr = buf;
		while (n--) {
			char c = *ptr++;
			if (c == kInterpretCmdLine) {
				interpretCmdLine(s_interpretCmdLine, cmdLine);
			} else if (c == kInterpretPrintCmdLine) {
				interpretCmdLine(s_interpretPrintCmdLine, cmdLine);
			} else {
				cmdLine.append(c);
			}
		}
		return true;
	}

	if (n == 0) {
		quit(0);
	} else if (errno != EAGAIN) {
		perror(getName());
		quit(1);
	}

	return false;
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

#ifdef HAVE_READLINE
// Function called by readline "up to ten times a second" while waiting for input
int sc_rl_ticker();
int sc_rl_ticker(){
	SC_TerminalClient::instance()->tick();
	return 0;
}

void sc_rl_signalhandler(int sig);
void sc_rl_signalhandler(int sig){
	// ensure ctrl-C clears line rather than quitting (ctrl-D will quit nicely)
	rl_replace_line("", 0);
	rl_reset_line_state();
	rl_crlf();
	rl_redisplay();
}

int sc_rl_mainstop(int i1, int i2);
int sc_rl_mainstop(int i1, int i2){
	SC_TerminalClient::instance()->stopMain();
	// We also push a newline so that there's some UI feedback
	rl_reset_line_state();
	rl_crlf();
	rl_redisplay();
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
#endif

void SC_TerminalClient::commandLoop()
{
#ifndef SC_WIN32

#ifdef HAVE_READLINE
	if(strcmp(gIdeName, "none") == 0){ // Other clients (emacs, vim, ...) won't want to interact through rl
		// Set up rl for sclang-specific nicenesses
		rl_readline_name = "sclang";
		rl_event_hook = &sc_rl_ticker;
		rl_set_keyboard_input_timeout(1e6/ticks_per_second);
		rl_basic_word_break_characters = " \t\n\"\\'`@><=;|&{}().";
		//rl_attempted_completion_function = sc_rl_completion;
		rl_bind_key(0x02, &sc_rl_mainstop); // TODO 0x02 is ctrl-B; ctrl-. would be nicer but keycode not working here (plain "." is 46 (0x2e))

		signal(SIGINT, &sc_rl_signalhandler);
		const char *prompt = "sc3> ";
		char *cmdLine;
		while (shouldBeRunning()) {
			tick();
			while((cmdLine = readline(prompt)) != NULL){
				if(*cmdLine!=0){
					// If line wasn't empty, store it so that uparrow retrieves it
					add_history(cmdLine);
					interpretCmdLine(s_interpretPrintCmdLine, cmdLine);
				}
				free(cmdLine);
			}
			if(cmdLine == NULL){
				printf("\nExiting sclang (ctrl-D)\n");
				quit(0);
			}
		}
	}else{
#endif
	const int fd = 0;
	struct pollfd pfds = { fd, POLLIN, 0 };
	SC_StringBuffer cmdLine;

	if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1) {
		perror(getName());
		quit(1);
		return;
	}

	while (shouldBeRunning()) {
		tick();
		int nfds = poll(&pfds, POLLIN, 1000/ticks_per_second);
		if (nfds > 0) {
			while (readCmdLine(fd, cmdLine));
#ifdef SC_DARWIN
			if(pfds.revents == POLLNVAL){
				// we reach here when reading directly from CLI, but not if being piped data! (osx 10.4.11 and 10.5.7 at least)
				usleep(1e6/ticks_per_second);
			}
#endif
		} else if (nfds == -1 && errno != EINTR) {
			perror(getName());
			quit(1);
			return;
		}
	}

#ifdef HAVE_READLINE
	} // end gIdeName!="none"
#endif

#else
  assert(0);
#endif
}

#ifdef SC_WIN32
# define nanosleep(tv, tz) win32_nanosleep(tv, tz)
#endif

void SC_TerminalClient::daemonLoop()
{
	struct timespec tv = { 0, 1e9 / ticks_per_second };

	while (shouldBeRunning()) {
		tick(); // also flushes post buffer
		if (nanosleep(&tv, 0) == -1) {
			perror(getName());
			quit(1);
			break;
		}
	}
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

	((SC_TerminalClient*)SC_LanguageClient::instance())->quit(code);

	return errNone;
}

void SC_TerminalClient::onLibraryStartup()
{
	int base, index = 0;
	base = nextPrimitiveIndex();
	definePrimitive(base, index++, "_Argv", &SC_TerminalClient::prArgv, 1, 0);
	definePrimitive(base, index++, "_Exit", &SC_TerminalClient::prExit, 1, 0);
}

// EOF
