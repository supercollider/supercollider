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
#include <unistd.h>
#include <string.h>

static const char kInputInterpretDelimiter = 0x1b;			// ^[
static const char kInputInterpretPrintDelimiter = 0x0c;		// ^L

SC_TerminalClient::SC_TerminalClient()
	: SC_LanguageClient("sclang"),
	  mShouldBeRunning(false)
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

void SC_TerminalClient::run(int argc, char** argv)
{
	int i;
	int err;

	SC_StringBuffer cmdLine;
	bool callRun = false;
	bool callStop = false;

	init(argc, argv);

	for (i = 1; i < argc; i++) {
		char* opt = argv[i];
		char* optarg = i < (argc - 1) ? argv[i+1] : 0;

		if (!((opt[0] == '-') && opt[1])) break;

		switch (opt[1]) {
			case 'c':
				if (optarg) {
					i++;
					cmdLine.append(optarg);
					cmdLine.append(";\n");
				} else {
					errorOptionArgExpected(opt);
				}
				break;
			case 'r': callRun = true; break;
			case 's': callStop = true; break;
			default:
				errorOptionInvalid(opt);
		}
	}

	// startup library
	compileLibrary();

	// call Main::run
	if (callRun) runMain();

	// execute cmdline
	if (!cmdLine.isEmpty()) {
		setCmdLine(cmdLine);
		interpretCmdLine();
	}

	// execute file(s) and/or read commands from stdin
	bool enterMainLoop = false;
	argc -= i;
	argv += i;
	if (argc > 0) {
		for (int i=0; i < argc; i++) {
			const char* fileName = argv[i];
			if (strcmp(fileName, "-") == 0) {
				enterMainLoop = true;
				break;
			} else {
				executeFile(fileName);
			}
		}
	} else {
		enterMainLoop = cmdLine.isEmpty();
	}

	if (enterMainLoop) {
		// setup tick thread
		mShouldBeRunning = true;
		if (pthread_create(&mTickThread, 0, tickThreadFunc, this) == -1) {
			error("%s", strerror(errno));
		}

		// enter main loop
		readCommands(stdin);

		// wait for tick thread to exit
		mShouldBeRunning = false;
		pthread_join(mTickThread, 0);
	}

	// call Main::stop
	if (callStop) stopMain();

	// shutdown library
	shutdownLibrary();
}

void SC_TerminalClient::printUsage(FILE* file)
{
	fprintf(file, "   %s [options] [file..] [-]\n", getName());
}

void SC_TerminalClient::printOptions(FILE* file)
{
	SC_LanguageClient::printOptions(file);
	fprintf(file,
			"\nTerminal Options:\n"
			"   -c <expression>                Execute expression\n"
			"   -r                             Call Main::run on startup\n"
			"   -s                             Call Main::stop on startup\n"
		);
}

int SC_TerminalClient::readCommands(FILE* inputFile)
{
    SC_StringBuffer cmdLine(128);

	while (true) {
		int c = fgetc(inputFile);

		if (c == EOF) {
			break;
		} else if ((c == kInputInterpretDelimiter) ||
				   (c == kInputInterpretPrintDelimiter)) {
			setCmdLine(cmdLine);
			runLibrary(c == kInputInterpretDelimiter
					   ? s_interpretCmdLine : s_interpretPrintCmdLine);
			flushPostBuf();
			cmdLine.reset();
		} else {
			cmdLine.append((char)c);
		}
	}

    return ferror(inputFile) ? 1 : 0;
}

void* SC_TerminalClient::tickThreadFunc(void* data)
{
	SC_TerminalClient* self = (SC_TerminalClient*)data;
	struct timespec ts = { 0, 50000000 }; // 50 ms
	
	while (self->mShouldBeRunning) {
		self->tick();
		nanosleep(&ts, 0);
	}

	return 0;
}

// EOF
