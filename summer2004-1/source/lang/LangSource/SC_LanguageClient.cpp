#include "SC_LanguageClient.h"
#include "SC_LibraryConfig.h"

#include <unistd.h>

#include "PyrObject.h"
#include "PyrKernel.h"
#include "PyrPrimitive.h"
#include "GC.h"
#include "VMGlobals.h"

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

SC_LanguageClient* SC_LanguageClient::gInstance = 0;

PyrSymbol* SC_LanguageClient::s_interpretCmdLine = 0;
PyrSymbol* SC_LanguageClient::s_interpretPrintCmdLine = 0;
PyrSymbol* SC_LanguageClient::s_run = 0;
PyrSymbol* SC_LanguageClient::s_stop = 0;
static PyrSymbol* s_tick = 0;

SC_LanguageClient::SC_LanguageClient(const char* name)
	: mName(0),
	  mPostFile(stdout),
	  mScratch(0),
	  mRunning(false)
{
	if (gInstance) {
		fprintf(stderr, "SC_LanguageClient already running\n");
		abort();
	}

	mName = strdup(name);
	gInstance = this;
}

SC_LanguageClient::~SC_LanguageClient()
{
	free(mName);
	gInstance = 0;
}

void SC_LanguageClient::initRuntime(const Options& opt)
{
	// start virtual machine
	if (!mRunning) {
		mRunning = true;
		if (opt.mRuntimeDir) chdir(opt.mRuntimeDir);
		pyr_init_mem_pools(opt.mMemSpace, opt.mMemGrow);
		init_OSC(opt.mPort);
		schedInit();
		onInitRuntime();
	}
}

bool SC_LanguageClient::readLibraryConfig(const char* filePath, const char* fileName)
{
	SC_LibraryConfigFile file(&::post);
	if (!fileName) fileName = filePath;
	if (file.open(filePath)) {
		bool success = SC_LibraryConfig::readLibraryConfig(file, fileName);
		file.close();
		return success;
	}
	return false;
}

bool SC_LanguageClient::readDefaultLibraryConfig()
{
	char* paths[3] = { ".sclang.cfg", "~/.sclang.cfg", "/etc/sclang.cfg" };

	char ipath[PATH_MAX];
	char opath[PATH_MAX];

	for (int i=0; i < 3; i++) {
		snprintf(ipath, PATH_MAX, paths[i]);
		if (unixStandardizePath(ipath, opath)) {
			SC_LibraryConfigFile file(&::post);
			if (!file.open(opath)) continue;
			bool success = SC_LibraryConfig::readLibraryConfig(file, opath);
			file.close();
			if (success) return true;
		}
	}

	return false;
}

void SC_LanguageClient::compileLibrary()
{
	::compileLibrary();
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

bool SC_LanguageClient::parseMemArg(const char* arg, int* res)
{
	long value, factor = 1;
	char* endPtr = 0;
	
	if (*arg == '\0') return false;

	value = strtol(arg, &endPtr, 0);

	char spec = *endPtr++;
	if (spec != '\0') {
		if (*endPtr != '\0')
			// trailing characters
			return false;

		switch (spec) {
			case 'k':
				factor = 1024;
				break;
			case 'm':
				factor = 1024 * 1024;
				break;
			default:
				// invalid mem spec
				return false;
		}
	}

	*res = value * factor;

	return true;
}

bool SC_LanguageClient::parsePortArg(const char* arg, int* res)
{
	long value;
	char* endPtr;

	if (*arg == '\0') return false;
	
	value = strtol(arg, &endPtr, 0);

	if ((*endPtr != '\0') || (value < 0) || (value > 65535))
		return false;

	*res = value;

	return true;
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

void SC_LanguageClient::onInitRuntime()
{
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
