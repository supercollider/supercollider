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

#include <libraryConfig.h>

#include <ctype.h>
#include <fcntl.h>
#include <string.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <unistd.h>

#include <PyrObject.h>
#include <PyrKernel.h>
#include <PyrPrimitive.h>
#include <GC.h>
#include <VMGlobals.h>

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

static const char kInputInterpretDelimiter = 0x1b;			// ^[
static const char kInputInterpretPrintDelimiter = 0x0c;		// ^L

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

void SC_StringBuffer::vappendf(const char* fmt, va_list ap)
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

void SC_StringBuffer::appendf(const char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vappendf(fmt, ap);
	va_end(ap);
}

void SC_StringBuffer::growBy(size_t request)
{
	size_t oldSize = getSize();
	size_t newCapacity = mCapacity + ((request + (size_t)kGrowAlign) & (size_t)~kGrowMask);

// 	fprintf(stderr, "%s: mCapacity %u, request %u, newCapacity %u\n",
// 			__PRETTY_FUNCTION__, mCapacity, request, newCapacity);
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
		int c = fgetc(mFile);
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
// SC_LanguageClient
// =====================================================================

SC_LanguageClient* SC_LanguageClient::gInstance = 0;

PyrSymbol* SC_LanguageClient::s_interpretCmdLine = 0;
PyrSymbol* SC_LanguageClient::s_interpretPrintCmdLine = 0;
PyrSymbol* SC_LanguageClient::s_run = 0;
PyrSymbol* SC_LanguageClient::s_stop = 0;
static PyrSymbol* s_tick;

SC_LanguageClient::SC_LanguageClient(const char* name)
	: mName(0), mPostFile(stdout), mScratch(0), mRunning(false)
{
	if (gInstance) {
		fprintf(stderr, "sclang [internal error]: language already running. bailing out.\n");
		exit(EXIT_FAILURE);
	}

	mName = strdup(name);
	gInstance = this;
}

SC_LanguageClient::~SC_LanguageClient()
{
	free(mName);
	gInstance = 0;
}

void SC_LanguageClient::printUsage()
{
	FILE* file = mPostFile ? mPostFile : stdout;
	fprintf(file, "Usage:\n");
	printUsage(file);
	fprintf(file, "\n");
	printOptions(file);
	fprintf(file, "\n");
}

void SC_LanguageClient::init(int& argc, char**& argv)
{
	Options opt;
	parseOptions(argc, argv, opt);
	init(opt);
}

void SC_LanguageClient::init(const Options& opt)
{
	mOptions = opt;

	char* homeDir = getenv("HOME");
	SC_StringBuffer tmpBuf(64);

	// read library config file
	if (!homeDir) homeDir = "";
	
	tmpBuf.reset();
	tmpBuf.appendf("%s/%s", homeDir, kSC_LibraryConfigFileName);
	tmpBuf.finish();

	SC_LibraryConfigFile configFile;
	if (configFile.open(tmpBuf.getData())) {
		extern LibraryConfig* gLibraryConfig;
		gLibraryConfig = new LibraryConfig();
		configFile.read(kSC_LibraryConfigFileName, gLibraryConfig);
	} else {
		fprintf(stderr,
				"sclang: could not read library configuration file.\n"
				"sclang: you should create ~/.sclang.cfg and make it readable.\n"
			);
		exit(EXIT_FAILURE);
	}

	// change to runtime directory
	if (opt.mRuntimeDir) chdir(opt.mRuntimeDir);
}

void SC_LanguageClient::compileLibrary()
{
	// start virtual machine
	if (!mRunning) {
		mRunning = true;
		pyr_init_mem_pools(mOptions.mMemSpace, mOptions.mMemGrow);
		init_OSC(mOptions.mPort);
		schedInit();
	}

    ::compileLibrary();

	// execute user setup file
	char* homeDir = getenv("HOME");
	if (homeDir) {
		SC_StringBuffer tmpBuf(64);
		tmpBuf.reset();
		tmpBuf.appendf("%s/.sclang.sc", homeDir);
		tmpBuf.finish();

		struct stat stat_buf;
		if ((stat(tmpBuf.getData(), &stat_buf) == 0)
			&& S_ISREG(stat_buf.st_mode)
			&& (stat_buf.st_mode & S_IRUSR)) {
			executeFile(tmpBuf.getData());
		}
	}
}

extern void ::shutdownLibrary();
void SC_LanguageClient::shutdownLibrary()
{
	::shutdownLibrary();
	flush();
}

void SC_LanguageClient::recompileLibrary()
{
	shutdownLibrary();
	compileLibrary();
}

void SC_LanguageClient::setCmdLine(const char* buf, size_t size)
{
    if (compiledOK) {
		pthread_mutex_lock(&gLangMutex);
		if (compiledOK) {
			VMGlobals *g = gMainVMGlobals;
			
			PyrString* strobj = newPyrStringN(g->gc, size, 0, true);
			memcpy(strobj->s, buf, size);

			SetObject(&g->process->interpreter.uoi->cmdLine, strobj);
			g->gc->GCWrite(g->process->interpreter.uo, strobj);
		}
		pthread_mutex_unlock(&gLangMutex);
    }
}

void SC_LanguageClient::setCmdLine(const char* str)
{
	setCmdLine(str, strlen(str));
}

void SC_LanguageClient::setCmdLine(const SC_StringBuffer& strBuf)
{
	setCmdLine(strBuf.getData(), strBuf.getSize());
}

void SC_LanguageClient::setCmdLinef(const char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	mScratch.reset();
	mScratch.vappendf(fmt, ap);
	va_end(ap);
	setCmdLine(mScratch);
}

void SC_LanguageClient::runLibrary(PyrSymbol* symbol)
{
    pthread_mutex_lock(&gLangMutex);
    ::runLibrary(symbol);
    pthread_mutex_unlock(&gLangMutex);
}

void SC_LanguageClient::runLibrary(const char* methodName)
{
    pthread_mutex_lock(&gLangMutex);
    ::runLibrary(getsym(methodName));
    pthread_mutex_unlock(&gLangMutex);
}

void SC_LanguageClient::executeFile(const char* fileName)
{
	setCmdLinef("thisProcess.interpreter.executeFile(\"%s\")", fileName);
	runLibrary(s_interpretCmdLine);
}

void SC_LanguageClient::printUsage(FILE* file)
{
	fprintf(file, "  %s [options]\n", getName());
}

void SC_LanguageClient::printOptions(FILE* file)
{
	SC_LanguageClient::Options opt;
	
	const size_t bufSize = 128;
	char memGrowBuf[bufSize];
	char memSpaceBuf[bufSize];

	snprintMemArg(memGrowBuf, bufSize, opt.mMemGrow);
	snprintMemArg(memSpaceBuf, bufSize, opt.mMemSpace);

	fprintf(file,
			"Common Options:\n"
			"   -d <path>                      Set runtime directory\n"
			"   -g <memory-growth>[km]         Set heap growth (default %s)\n"
			"   -h                             Display this message and exit\n"
			"   -m <memory-space>[km]          Set initial heap size (default %s)\n"
			"   -u <network-port-number>       Set UDP listening port (default %d)\n",
			memGrowBuf,
			memSpaceBuf,
			opt.mPort
		);
}

void SC_LanguageClient::errorOptionInvalid(const char* opt)
{
	fprintf(stderr, "%s: invalid option %s\n", getName(), opt);
	exit(EXIT_FAILURE);
}

void SC_LanguageClient::errorOptionArgExpected(const char* opt)
{
	fprintf(stderr, "%s: missing argument for option %s\n", getName(), opt);
	exit(EXIT_FAILURE);
}

void SC_LanguageClient::errorOptionArgInvalid(const char* opt, const char* arg)
{
	fprintf(stderr, "%s: invalid argument for option %s -- %s\n", getName(), opt, arg);
	exit(EXIT_FAILURE);
}

void SC_LanguageClient::tick()
{
    if (pthread_mutex_trylock(&gLangMutex) == 0) {
		if (compiledOK) {
			::runLibrary(s_tick);
		}
		pthread_mutex_unlock(&gLangMutex);
    }
    flush();
}

void SC_LanguageClient::onLibraryStartup()
{
}

void SC_LanguageClient::onLibraryShutdown()
{
}

void SC_LanguageClient::onInterpStartup()
{
}

void SC_LanguageClient::snprintMemArg(char* dst, size_t size, int arg)
{
	int rem = arg;
	int mod = 0;
	char* modstr = "";

	while (((rem % 1024) == 0) && (mod < 4)) {
		rem /= 1024;
		mod++;
	}

	switch (mod) {
		case 0: modstr = ""; break;
		case 1: modstr = "k"; break;
		case 2: modstr = "m"; break;
		case 3: modstr = "g"; break;
		default: rem = arg; modstr = ""; break;
	}

	snprintf(dst, size, "%d%s", rem, modstr);
}

int SC_LanguageClient::parseMemArg(const char* arg)
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

int SC_LanguageClient::parsePortArg(const char* arg)
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

void SC_LanguageClient::parseOptions(int& argc, char**& argv, Options& options)
{
	char* opt;
	char* optarg;
	int nargs;

	int i = 1;
	while (i < argc) {
		opt = argv[i];
		optarg = i < (argc - 1) ? argv[i+1] : 0;
		nargs = 0;

		if (!((opt[0] == '-') && opt[1] && strchr("dghmu", opt[1]))) {
			// not an option
			i++;
			continue;
		}

		// index before
		switch (opt[1]) {
			case 'd':
				if (optarg) {
					nargs++;
					options.mRuntimeDir = optarg;
				} else {
					goto optArgExpected;
				}
				break;
			case 'g':
				if (optarg) {
					nargs++;
					options.mMemGrow = parseMemArg(optarg);
					if (options.mMemGrow < 0) {
						goto optArgInvalid;
					}
				} else {
					goto optArgExpected;
				}
				break;
			case 'h':
				goto help;
				break;
			case 'm':
				if (optarg) {
					nargs++;
					options.mMemSpace = parseMemArg(optarg);
					if (options.mMemSpace < 0) {
						goto optArgInvalid;
					}
				} else {
					goto optArgExpected;
				}
				break;
			case 'u':
				if (optarg) {
					nargs++;
					options.mPort = parsePortArg(optarg);
					if (options.mPort < 0) {
						goto optArgInvalid;
					}
				} else {
					goto optArgExpected;
				}
				break;
		}

		// shuffle options
		int j, k;
		for (j=i, k=i+nargs+1; k < argc; j++, k++) {
			argv[j] = argv[k];
		}
		argc -= nargs+1;
	}

	return;

 help:
	printUsage();
	exit(EXIT_SUCCESS);

 optArgExpected:
	errorOptionArgExpected(opt);

 optArgInvalid:
	errorOptionArgInvalid(opt, optarg);
}

// =====================================================================
// SC_TerminalClient
// =====================================================================

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

int SC_TerminalClient::run(int argc, char** argv)
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
		err = pthread_create(&mTickThread, 0, tickThreadFunc, this);
		if (err < 0) {
			perror("sclang:");
			return 1;
		}

		// enter main loop
		err = readCommands(stdin);

		// wait for tick thread to exit
		mShouldBeRunning = false;
		pthread_join(mTickThread, 0);
		
		return err;
	}

	// call Main::stop
	if (callStop) stopMain();

	// shutdown library
	shutdownLibrary();

	return 0;
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

// =====================================================================
// Library functions
// =====================================================================

void setPostFile(FILE* file)
{
	SC_LanguageClient::instance()->setPostFile(file);
}

int vpost(const char *fmt, va_list ap)
{
	SC_LanguageClient::instance()->post(fmt, ap, false);
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
    SC_LanguageClient::instance()->post(fmt, ap, false);
    SC_LanguageClient::instance()->flush();
}

void postText(const char *str, long len)
{
	SC_LanguageClient::instance()->post(str, len);
}

void postChar(char c)
{
	SC_LanguageClient::instance()->post(c);
}

void error(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    SC_LanguageClient::instance()->post(fmt, ap, true);
}

void flushPostBuf(void)
{
	SC_LanguageClient::instance()->flush();
}

void closeAllGUIScreens()
{
	SC_LanguageClient::instance()->onLibraryShutdown();
}

void initGUI()
{
	SC_LanguageClient::instance()->onInterpStartup();
}

void initGUIPrimitives()
{
	SC_LanguageClient::s_interpretCmdLine = getsym("interpretCmdLine");
	SC_LanguageClient::s_interpretPrintCmdLine = getsym("interpretPrintCmdLine");
	SC_LanguageClient::s_run = getsym("run");
	SC_LanguageClient::s_stop = getsym("stop");
    s_tick = getsym("tick");
	SC_LanguageClient::instance()->onLibraryStartup();
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
