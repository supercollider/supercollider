#include "SC_LanguageClient.h"
#include "SC_LibraryConfig.h"
#include "PyrObject.h"
#include "PyrKernel.h"
#include "PyrPrimitive.h"
#include "GC.h"
#include "VMGlobals.h"

#include <sys/stat.h>
#include <stdexcept>

void closeAllGUIScreens();
void initGUI();
void initGUIPrimitives();
void initSCViewPrimitives();
void initCocoaFilePrimitives();
void initRendezvousPrimitives();
void initSpeechPrimitives();

long scMIDIout(int port, int len, int statushi, int chan, int data1, int data2);

extern PyrString* newPyrStringN(class PyrGC *gc, long length, long flags, long collect);

// =====================================================================
// SC_LanguageClient
// =====================================================================

const char* kSC_LibraryConfigFileName = ".sclang.cfg";

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
		throw std::runtime_error("SC_LanguageClient already running");
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

	// change to runtime directory
	if (opt.mRuntimeDir) chdir(opt.mRuntimeDir);

	// read default library config file
	// this should probably be done in a derived client
	char* home = getenv("HOME");
	if (home) {
		SC_StringBuffer tmpBuf(64);
		tmpBuf.reset();
		tmpBuf.appendf("%s/%s", home, kSC_LibraryConfigFileName);
		tmpBuf.finish();
		readLibraryConfig(tmpBuf.getData(), kSC_LibraryConfigFileName);
	}
}

bool SC_LanguageClient::readLibraryConfig(const char* filePath, const char* fileName)
{
	SC_LibraryConfigFile file(&::post);
	if (!fileName) fileName = filePath;
	if (file.open(filePath)) {
		bool err = SC_LibraryConfig::readLibraryConfig(file, fileName);
		file.close();
		return err;
	}
	return false;
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
    if (isLibraryCompiled()) {
		lock();
		if (isLibraryCompiled()) {
			VMGlobals *g = gMainVMGlobals;
			
			PyrString* strobj = newPyrStringN(g->gc, size, 0, true);
			memcpy(strobj->s, buf, size);

			SetObject(&g->process->interpreter.uoi->cmdLine, strobj);
			g->gc->GCWrite(g->process->interpreter.uo, strobj);
		}
		unlock();
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
	lock();
    ::runLibrary(symbol);
	unlock();
}

void SC_LanguageClient::runLibrary(const char* methodName)
{
	lock();
    ::runLibrary(getsym(methodName));
	unlock();
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

void SC_LanguageClient::error(const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
	mScratch.reset();
	mScratch.appendf("%s: ", getName());
	mScratch.vappendf(fmt, ap);
	mScratch.finish();
	throw std::runtime_error(mScratch.getData());
}

void SC_LanguageClient::errorOptionInvalid(const char* opt)
{
	mScratch.reset();
	mScratch.appendf("%s: invalid option %s", getName(), opt);
	mScratch.finish();
	throw std::runtime_error(mScratch.getData());
}

void SC_LanguageClient::errorOptionArgExpected(const char* opt)
{
	mScratch.reset();
	mScratch.appendf("%s: missing argument for option %s", getName(), opt);
	mScratch.finish();
	throw std::runtime_error(mScratch.getData());
}

void SC_LanguageClient::errorOptionArgInvalid(const char* opt, const char* arg)
{
	mScratch.reset();
	mScratch.appendf("%s: invalid argument for option %s -- %s", getName(), opt, arg);
	mScratch.finish();
	throw std::runtime_error(mScratch.getData());
}

void SC_LanguageClient::tick()
{
    if (trylock()) {
		if (isLibraryCompiled()) {
			::runLibrary(s_tick);
		}
		unlock();
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
	exit(EXIT_SUCCESS);	// debatable

 optArgExpected:
	errorOptionArgExpected(opt);

 optArgInvalid:
	errorOptionArgInvalid(opt, optarg);
}

// =====================================================================
// library functions
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
