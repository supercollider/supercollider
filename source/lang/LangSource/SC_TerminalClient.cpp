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

#include <PyrObject.h>
#include <PyrKernel.h>
#include <PyrPrimitive.h>
#include <GC.h>
#include <VMGlobals.h>

#include <libraryConfig.h>

#include <ctype.h>
#include <fcntl.h>
#include <string.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <unistd.h>

void setPostFile(FILE* file);
extern "C" int vpost(const char *fmt, va_list vargs);
void post(const char *fmt, ...);
void postfl(const char *fmt, ...);
void postText(const char *text, long length);
void postChar(char c);
void error(const char *fmt, ...);
void flushPostBuf();

void closeAllGUIScreens();
void initGUI();
void initGUIPrimitives();
void initSCViewPrimitives();
void initCocoaFilePrimitives();
void initRendezvousPrimitives();
void initSpeechPrimitives();

long scMIDIout(int port, int len, int statushi, int chan, int data1, int data2);

extern PyrString* newPyrStringN(class PyrGC *gc, long length, long flags, long collect);

extern long compiledOK;
extern pthread_mutex_t gLangMutex;

static PyrSymbol* s_interpretCmdLine = 0;
static PyrSymbol* s_interpretPrintCmdLine = 0;
static PyrSymbol* s_stop = 0;
static PyrSymbol* s_tick = 0;

static char gInputInterpretDelimiter = 0x1b;			// ^[
static char gInputInterpretPrintDelimiter = 0x0c;		// ^L

static FILE* gPostFile = stdout;

// =====================================================================
// SC_StringBuffer
// =====================================================================

SC_StringBuffer::SC_StringBuffer(size_t initialSize)
	: mCapacity(0), mPtr(0), mData(0)
{
	growBy(initialSize);
}

SC_StringBuffer::SC_StringBuffer(const SC_StringBuffer& other)
	: mCapacity(0), mPtr(0), mData(0)
{
	growBy(other.getSize());
	append(other.getData(), other.getSize());
}

SC_StringBuffer::~SC_StringBuffer()
{
	free(mData);
}

void SC_StringBuffer::append(const char* src, size_t size)
{
	if (size > 0) {
		size_t remaining = getRemaining();
		if (size > remaining) {
			growBy(size - remaining);
		}
		memcpy(mPtr, src, size);
		mPtr += size;
	}
}

void SC_StringBuffer::append(char c)
{
	append(&c, sizeof(c));
}

void SC_StringBuffer::append(const char* str)
{
	append(str, strlen(str));
}

void SC_StringBuffer::vprintf(const char* fmt, va_list ap)
{
	va_list ap2;
	size_t remaining = getRemaining();

	// Calling vsnprintf may invalidate vargs, so keep a copy
#ifdef __va_copy
	__va_copy(ap2, ap);
#else
	ap2 = ap;
#endif

	// NOTE: This only works since glibc 2.0.6!
	int size = vsnprintf(mPtr, remaining, fmt, ap);
	va_end(ap);

	// size returned excludes trailing \0
	if (size++ > 0) {
		if ((size_t)size > remaining) {
			growBy(size - remaining);
			vsnprintf(mPtr, size, fmt, ap2);
		}
		mPtr += size-1; // omit trailing \0
	}

	va_end(ap2);
}

void SC_StringBuffer::printf(const char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vprintf(fmt, ap);
	va_end(ap);
}

void SC_StringBuffer::growBy(size_t request)
{
	size_t oldSize = getSize();
	size_t newCapacity = mCapacity + ((request + (size_t)kGrowAlign) & (size_t)~kGrowMask);

// 	post("%s: mCapacity %u, request %u, newCapacity %u\n",
// 		 __PRETTY_FUNCTION__, mCapacity, request, newCapacity);
	assert((newCapacity >= (mCapacity + request)) && ((newCapacity & kGrowMask) == 0));

	char* newData = (char*)realloc(mData, newCapacity);
	if (newData) {
		mData = newData;
		mCapacity = newCapacity;
		mPtr = mData + oldSize;
	} else {
		throw std::runtime_error("SC_StringBuffer: memory allocation failure");
	}
}

// =====================================================================
// SC_LibraryConfigFile
//    simple library configuration file parser
// =====================================================================

const char* kSC_LibraryConfigFileName = ".sclang.cfg";

class SC_LibraryConfigFile
{
public:
    SC_LibraryConfigFile()
		: mFile(0)
	{ }

    bool open(const char* pathName);
    void read(const char* fileName, LibraryConfig* libConf);
    void close();

protected:
	enum State
	{
		kBegin,
		kAction,
		kPath,
		kEscape,
		kEnvVar,
		kEnvVarName,
		kEnd
	};

	bool parseLine(const char* fileName, int lineNumber, const char* line, LibraryConfig* libConf);

private:
	FILE* mFile;
};

bool SC_LibraryConfigFile::open(const char* pathName)
{
	this->close();
	mFile = fopen(pathName, "r");
	return mFile != 0;
}

void SC_LibraryConfigFile::close()
{
	if (mFile) {
		fclose(mFile);
		mFile = 0;
	}
}

bool SC_LibraryConfigFile::parseLine(const char* fileName, int lineNumber, const char* line, LibraryConfig* libConf)
{
	char action = 0;
	SC_StringBuffer path;
	SC_StringBuffer envVarName;
	
	bool error = false;
	State state = kBegin;

	while (true) {
		// NOTE: in some parser states the character just read is
		// written back to be consumed by the following state in the
		// next iteration; this may be slightly inefficient, but makes
		// control flow more obvious.

		char c = *line++;

		if (!c || error || ((c == '#') && (state != kEscape))) {
			break;
		}
		
		switch (state) {
			case kBegin:
				if (!isspace(c)) {
					line--;
					state = kAction;
				}
				break;
			case kAction:
				if ((c == '+') || (c == '-')) {
					action = c;
					state = kPath;
				} else {
					fprintf(stderr, "%s,%d: invalid action '%c'\n", fileName, lineNumber, c);
					error = true;
				}
				break;
			case kPath:
				if (c == '\\') {
					state = kEscape;
				} else if (c == '$') {
					state = kEnvVar;
				} else if (isspace(c)) {
					state = kEnd;
				} else {
					path.append(c);
				}
				break;
			case kEscape:
				path.append(c);
				state = kPath;
				break;
			case kEnvVar:
				if (isalpha(c)) {
					line--;
					state = kEnvVarName;
					envVarName.reset();
				} else {
					fprintf(stderr, "%s,%d: empty variable reference\n", fileName, lineNumber);
					error = true;
				}
				break;
			case kEnvVarName:
				if (isalpha(c) || (c == '_')) {
					envVarName.append(c);
				} else {
					envVarName.finish();
					char* envVarValue = getenv(envVarName.getData());
					if (envVarValue) {
						line--;
						state = kPath;
						path.append(envVarValue);
					} else {
						fprintf(stderr, "%s,%d: undefined variable '%s'\n", fileName, lineNumber, envVarName.getData());
						error = true;
					}
				}
				break;
			case kEnd:
				if (!isspace(c)) {
					fprintf(stderr, "%s,%d: trailing garbage\n", fileName, lineNumber);
					error = true;
				}
				break;
			default:
				fprintf(stderr, "%s,%d: [internal error] invalid parser state %d\n", fileName, lineNumber, state);
				error = true;
		}
	}

	if (!error && action && (path.getSize() > 0)) {
		path.finish();
		char realPath[MAXPATHLEN];
		if (unixStandardizePath(path.getData(), realPath) != 0) {
			char* str = (char*)malloc((strlen(realPath) + 1) * sizeof(char));
			if (str) {
				strcpy(str, realPath);
				if (action == '+') {
					libConf->addIncludedDirectory(str);
				} else {
					libConf->addExcludedDirectory(str);
				}
				return true;
			}
		}
	}

	return false;
}

void SC_LibraryConfigFile::read(const char* fileName, LibraryConfig* libConf)
{
	if (!mFile) return;

	size_t lineNumber = 1;
    SC_StringBuffer line;

    while (true) {
		char c = fgetc(mFile);
		bool eof = c == EOF;

		if (eof || (c == '\n')) {
			line.finish();
            parseLine(fileName, lineNumber, line.getData(), libConf);
            line.reset();
			lineNumber++;
			if (eof) break;
        } else {
            line.append(c);
        }
    }
}

// =====================================================================
// SC_TerminalClient
// =====================================================================

SC_TerminalClient::SC_TerminalClient()
	: mShouldBeRunning(false), mExitCode(0), mScratch(0)
{
}

SC_TerminalClient::~SC_TerminalClient()
{
}

static int parseMemArg(const char* arg)
{
	long value, factor = 1;
	char* endPtr = 0;
	
	if (*arg == '\0')
		return -1;

	value = strtol(arg, &endPtr, 0);

	char spec = *endPtr++;
	if (spec != '\0') {
		if (*endPtr != '\0')
			// trailing characters
			return -1;

		switch (spec) {
			case 'k':
				factor = 1024;
				break;
			case 'm':
				factor = 1024 * 1024;
				break;
			default:
				// invalid mem spec
				return -1;
		}
	}

	return value * factor;
}

static int parsePortArg(const char* arg)
{
	long value;
	char* endPtr;

	if (*arg == '\0')
		return -1;
	
	value = strtol(arg, &endPtr, 0);

	if ((*endPtr != '\0') || (value < 0) || (value > 65535))
		return -1;

	return value;
}

void printUsage()
{
	SC_TerminalClient::Options opt;

	fprintf(stdout,
			"usage:\n"
			"   sclang [options] [--] [file ..] [-]\n"
			"\n"
			"options:\n"
			"   -c <expression>                         execute expression\n"
			"   -d <path>                               set sclang runtime directory\n"
			"   -g <memory-growth>[km]                  set heap growth (default %d)\n"
			"   -h                                      display this message and exit\n"
			"   -m <memory-space>[km]                   set initial heap size (default %d)\n"
			"   -r                                      call Main::run on startup\n"
			"   -s                                      call Main::stop on exit\n"
			"   -u <network-port-number>                set listening port (default %d)\n",
			opt.mMemGrow,
			opt.mMemSpace,
			opt.mPort
		);
}

static bool parseOptions(int argc, char** argv, SC_TerminalClient::Options& opt, int* exitCode)
{
	*exitCode = 0;

	while (true) {
		int c = getopt(argc, argv, "+c:d:g:hm:rsu:");
		if (c == -1)
			break;
		switch (c) {
			case 'c':
				if (strlen(optarg) > 0)
					opt.mCmdLine.printf("%s ; ", optarg);
				break;
			case 'd':
				opt.mRuntimeDir = optarg;
				break;
			case 'g':
				opt.mMemGrow = parseMemArg(optarg);
				if (opt.mMemGrow < 0)
					goto invalidOptionArg;
				break;
			case 'h':
				printUsage();
				return false;
			case 'm':
				opt.mMemSpace = parseMemArg(optarg);
				if (opt.mMemSpace < 0)
					goto invalidOptionArg;
				break;
			case 'r':
				opt.mCallRun = true;
				break;
			case 's':
				opt.mCallStop = true;
				break;
			case 'u':
				opt.mPort = parsePortArg(optarg);
				if (opt.mPort < 0)
					goto invalidOptionArg;
				break;
			default:
				goto invalidOption;
		}
	}

	opt.mArgc = argc - optind;
	opt.mArgv = argv + optind;

	return true;
	
 invalidOption:
	printUsage();
	*exitCode = 1;
	return false;
	
 invalidOptionArg:
	fprintf(stderr, "Invalid option argument -- %s\n", optarg);
	*exitCode = 2;
	return false;
}

int SC_TerminalClient::run(int argc, char** argv)
{
	// Parse options
	Options opt;
	if (!parseOptions(argc, argv, opt, &mExitCode)) {
		return mExitCode;
	}

	return run(opt);
}

int SC_TerminalClient::run(Options& opt)
{
	int err;
	
	char* homeDir = getenv("HOME");
	SC_StringBuffer tmpBuf(64);

	if (homeDir) {
		tmpBuf.reset();
		tmpBuf.printf("%s/%s", homeDir, kSC_LibraryConfigFileName);
		tmpBuf.finish();
		SC_LibraryConfigFile configFile;
		if (configFile.open(tmpBuf.getData())) {
			extern LibraryConfig* gLibraryConfig;
			gLibraryConfig = new LibraryConfig();
			configFile.read(kSC_LibraryConfigFileName, gLibraryConfig);
		} else {
			fprintf(stderr, "No library configuration file.\n");
			return 1;
		}
	}

	if (opt.mRuntimeDir) {
		chdir(opt.mRuntimeDir);
	}
	
    // Start virtual machine
    pyr_init_mem_pools(opt.mMemSpace, opt.mMemGrow);
    init_OSC(opt.mPort);
    schedInit();
    compileLibrary();

    // Setup tick thread
	mShouldBeRunning = true;
    err = pthread_create(&mTickThread, 0, tickThreadFunc, this);
    if (err < 0) {
		perror("sclang:");
		return 1;
    }

	// Execute user setup file
	if (homeDir) {
		tmpBuf.reset();
		tmpBuf.printf("%s/.sclang.sc", homeDir);
		tmpBuf.finish();

		struct stat stat_buf;
		if ((stat(tmpBuf.getData(), &stat_buf) == 0)
			&& S_ISREG(stat_buf.st_mode)
			&& (stat_buf.st_mode & S_IRUSR)) {
			executeFile(tmpBuf.getData());
		}
	}

	// Call Main::run
	if (opt.mCallRun) {
		runLibrary(s_run);
	}
	
	// Execute cmdline
	if (!opt.mCmdLine.isEmpty()) {
		setCmdLine(opt.mCmdLine);
		runLibrary(s_interpretCmdLine);
	}
	
	// Execute file(s) and/or read commands from stdin
	if (opt.mArgc > 0) {
		for (int i=0; i < opt.mArgc; i++) {
			char* fileName = opt.mArgv[i];
			if (strcmp(fileName, "-") == 0) {
				readCommands(stdin);
				break;
			} else {
				executeFile(fileName);
			}
		}
	} else {
		if (opt.mCmdLine.isEmpty()) {
			readCommands(stdin);
		}
	}
	
	// Call Main::stop
	if (opt.mCallStop) {
		runLibrary(s_stop);
	}

	// Shutdown library
	mShouldBeRunning = false;
	extern void shutdownLibrary();
	shutdownLibrary();

	// Wait for tick thread to exit
	pthread_join(mTickThread, 0);

	flushPostBuf();

    return mExitCode;
}

void SC_TerminalClient::quit(int exitCode)
{
	mExitCode = exitCode;
	mShouldBeRunning = false;
}

void SC_TerminalClient::tick()
{
    if (pthread_mutex_trylock(&gLangMutex) == 0) {
		if (compiledOK) {
			::runLibrary(s_tick);
		}
		pthread_mutex_unlock(&gLangMutex);
    }
    flushPostBuf();
}

void SC_TerminalClient::setCmdLine(const char* src, size_t size)
{
    if (compiledOK) {
		pthread_mutex_lock(&gLangMutex);
		if (compiledOK) {
			VMGlobals *g = gMainVMGlobals;
			
			PyrString* strobj = newPyrStringN(g->gc, size, 0, true);
			memcpy(strobj->s, src, size);

			SetObject(&g->process->interpreter.uoi->cmdLine, strobj);
			g->gc->GCWrite(g->process->interpreter.uo, strobj);
		}
		pthread_mutex_unlock(&gLangMutex);
    }
}

void SC_TerminalClient::setCmdLine(const SC_StringBuffer& strBuf)
{
	setCmdLine(strBuf.getData(), strBuf.getSize());
}

void SC_TerminalClient::setCmdLine(const char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	mScratch.reset();
	mScratch.vprintf(fmt, ap);
	va_end(ap);
	setCmdLine(mScratch);
}

void SC_TerminalClient::runLibrary(void* pyrSymbol)
{
    pthread_mutex_lock(&gLangMutex);
    ::runLibrary((PyrSymbol*)pyrSymbol);
    pthread_mutex_unlock(&gLangMutex);
}

void SC_TerminalClient::readCommands(FILE* inputFile)
{
    SC_StringBuffer cmdLine(128);

	while (true) {
		int c = fgetc(inputFile);

		if (c == EOF) {
			break;
		} else if ((c == gInputInterpretDelimiter) ||
				   (c == gInputInterpretPrintDelimiter)) {
			setCmdLine(cmdLine);
			runLibrary(c == gInputInterpretDelimiter
					   ? s_interpretCmdLine : s_interpretPrintCmdLine);
			flushPostBuf();
			cmdLine.reset();
		} else {
			cmdLine.append((char)c);
		}
	}

    mExitCode = ferror(inputFile) ? 1 : 0;
}

void SC_TerminalClient::executeFile(const char* fileName)
{
	setCmdLine("thisProcess.interpreter.executeFile(\"%s\")", fileName);
	runLibrary(s_interpretCmdLine);
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

// =====================================================================
// Library functions
// =====================================================================

void setPostFile(FILE* file)
{
	gPostFile = file;
}

int vpost(const char *fmt, va_list ap)
{
    vfprintf(gPostFile, fmt, ap);
	return 0;
}

void post(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt); 
    vpost(fmt, ap);
}

void postfl(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt); 
    vpost(fmt, ap);
    flushPostBuf();
}

void postText(const char *text, long len)
{
    fwrite(text, sizeof(char), len, gPostFile);
}

void postChar(char c)
{
    fputc(c, gPostFile);
}

void error(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vpost(fmt, ap);
}

void flushPostBuf(void)
{
    fflush(gPostFile);
}

void closeAllGUIScreens()
{
}

void initGUI()
{
}

void initGUIPrimitives()
{
	s_interpretCmdLine = getsym("interpretCmdLine");
	s_interpretPrintCmdLine = getsym("interpretPrintCmdLine");
	s_stop = getsym("stop");
    s_tick = getsym("tick");
}

void initSCViewPrimitives()
{
}

void initCocoaFilePrimitives()
{
}

void initRendezvousPrimitives()
{
}

void initSpeechPrimitives()
{
}

long scMIDIout(int port, int len, int statushi, int chan, int data1, int data2)
{
    return 0;
}

// EOF
