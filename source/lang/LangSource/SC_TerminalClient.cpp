// copyright 2003 stefan kersten <steve@k-hornz.de>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA

#include "SC_TerminalClient.h"

#include <errno.h>
#include <fcntl.h>
#include <sys/param.h>
#include <sys/poll.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#include "PyrPrimitive.h"
#include "PyrSlot.h"
#include "VMGlobals.h"

SC_TerminalClient::SC_TerminalClient(const char* name)
	: SC_LanguageClient(name),
	  mShouldBeRunning(false),
	  mReturnCode(0)
{
	setPostFile(stdout);
}

void SC_TerminalClient::post(const char *fmt, va_list ap, bool error)
{
	vfprintf(getPostFile(), fmt, ap);
}

void SC_TerminalClient::post(char c)
{
	fputc(c, getPostFile());
}

void SC_TerminalClient::post(const char* str, size_t len)
{
	fwrite(str, sizeof(char), len, getPostFile());
}

void SC_TerminalClient::flush()
{
	fflush(getPostFile());
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
			"   -u <network-port-number>       Set UDP listening port (default %d)\n",
			memGrowBuf,
			memSpaceBuf,
			opt.mPort
		);
}

bool SC_TerminalClient::parseOptions(int& argc, char**& argv, Options& opt)
{
	const char* optstr = ":d:Dg:hl:m:rsu:";
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
				if (!parseMemArg(optarg, &opt.mMemGrow))
					goto optArgInvalid;
				break;
			case 'h':
				goto help;
			case 'l':
				opt.mLibraryConfigFile = optarg;
				break;
			case 'm':
				if (!parseMemArg(optarg, &opt.mMemSpace))
					goto optArgInvalid;
				break;
			case 'r':
				opt.mCallRun = true;
				break;
			case 's':
				opt.mCallStop = true;
				break;
			case 'u':
				if (!parsePortArg(optarg, &opt.mPort))
					goto optArgInvalid;
				break;
			case '?':
				goto optInvalid;
			case ':':
				goto optArgExpected;
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

	opt.mArgc = argc;
	opt.mArgv = argv;

	// read library configuration file 
	bool success;
	if (opt.mLibraryConfigFile) {
		success = readLibraryConfig(opt.mLibraryConfigFile, opt.mLibraryConfigFile);
	} else {
		success = readDefaultLibraryConfig();
	}
	if (!success) {
		::post("%s: error reading library configuration file\n", getName());
		return 1;
	}

	// initialize runtime
	initRuntime(opt);

	// startup library
	compileLibrary();

	// enter daemon mode or read commands from stdin
	bool enterMainLoop;
	bool readCommands;

	if (argc > 0) {
		enterMainLoop = opt.mDaemon;
		readCommands = !opt.mDaemon;
		for (int i=0; i < argc; i++) {
			// execute file
			// - denotes input from stdin
			const char* fileName = argv[i];
			if (strcmp(fileName, "-") == 0) {
				enterMainLoop = readCommands = true;
				break;
			} else {
				executeFile(fileName);
			}
		}
	} else {
		enterMainLoop = true;
		readCommands = !opt.mDaemon;
	}

	if (opt.mCallRun) runMain();

	if (enterMainLoop) {
		mShouldBeRunning = true;
		if (readCommands) commandLoop();
		else daemonLoop();
	}

	if (opt.mCallStop) stopMain();

	// shutdown library
	shutdownLibrary();
	flush();

	return mReturnCode;
}

void SC_TerminalClient::quit(int code)
{
	mShouldBeRunning = false;
	mReturnCode = code;
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

void SC_TerminalClient::commandLoop()
{
	const int fd = 0;
	struct pollfd pfds[1] = { fd, POLLIN, 0 };
	SC_StringBuffer cmdLine;

	if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1) {
		perror(getName());
		quit(1);
		return;
	}

	while (shouldBeRunning()) {
		tick();
		int nfds = poll(pfds, 1, 50);
		if (nfds > 0) {
			while (readCmdLine(fd, cmdLine));
		} else if (nfds == -1) {
			perror(getName());
			quit(1);
			return;
		}
	}
}

void SC_TerminalClient::daemonLoop()
{
	struct timespec tv = { 0, 500000 };

	while (shouldBeRunning()) {
		tick(); // also flushes post buffer
		if (nanosleep(&tv, 0) == -1) {
			perror(getName());
			quit(1);
			break;
		}
	}
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
	definePrimitive(base, index++, "_Exit", &SC_TerminalClient::prExit, 1, 0);
}

// EOF
