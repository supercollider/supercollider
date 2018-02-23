/*
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
/*

Primitives for Unix.

*/

#include <cstring>
#include <errno.h>
#include <signal.h>

#include "PyrPrimitive.h"
#include "PyrObject.h"
#include "PyrKernel.h"
#include "PyrSched.h"
#include "VMGlobals.h"
#include "GC.h"
#include "SC_RGen.h"
#include "SC_Filesystem.hpp"
#include "sc_popen.h"
#include "SCBase.h"

#include "SC_Lock.h"

#include <vector>
#include <boost/filesystem.hpp>

#include "../../dyncall/dynload/dynload.h"
#include "../../dyncall/dyncall/dyncall.h"

#ifdef _WIN32
#include "SC_Win32Utils.h"
#else
#include <libgen.h>
#endif

namespace bfs = boost::filesystem;

extern bool compiledOK;
PyrSymbol* s_unixCmdAction;

int prString_System(struct VMGlobals *g, int numArgsPushed);
int prString_System(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a = g->sp;

	char cmdline[1024];
	int err = slotStrVal(a, cmdline, 1023);
	if (err) return err;

	int res = system(cmdline);
	SetInt(a, res);

	return errNone;
}

int prString_Basename(struct VMGlobals *g, int numArgsPushed);
int prString_Basename(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a = g->sp;

	char path[PATH_MAX];
	int err = slotStrVal(a, path, PATH_MAX);
	if (err) return err;

	char *basename0 = basename(path);

	int size = strlen(basename0);
	PyrString *strobj = newPyrStringN(g->gc, size, 0, true);
	memcpy(strobj->s, basename0, size);

	SetObject(a, strobj);

	return errNone;
}

int prString_Dirname(struct VMGlobals *g, int numArgsPushed);
int prString_Dirname(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a = g->sp;

	char path[PATH_MAX];
	int err = slotStrVal(a, path, PATH_MAX);
	if (err) return err;

	char *dirname0 = dirname(path);

	int size = strlen(dirname0);
	PyrString *strobj = newPyrStringN(g->gc, size, 0, true);
	memcpy(strobj->s, dirname0, size);

	SetObject(a, strobj);

	return errNone;
}

struct sc_process {
	pid_t pid;
	FILE *stream;
	bool postOutput;
};

static void string_popen_thread_func(struct sc_process *process)
{
	FILE *stream = process->stream;
	pid_t pid = process->pid;
	char buf[1024];

	while (process->postOutput) {
		char *string = fgets(buf, 1024, stream);
		if (!string) break;
		postText(string, strlen(string));
	}

	int res;
	res = sc_pclose(stream, pid);
	res = WEXITSTATUS(res);

	delete process;

	gLangMutex.lock();
	if(compiledOK) {
		VMGlobals *g = gMainVMGlobals;
		++g->sp;  SetObject(g->sp, class_string);
		++g->sp; SetInt(g->sp, res);
		++g->sp; SetInt(g->sp, pid);
		runInterpreter(g, s_unixCmdAction, 3);
	}
	gLangMutex.unlock();
}

int prString_POpen(struct VMGlobals *g, int numArgsPushed);
int prString_POpen(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a = g->sp - 1;
	PyrSlot *b = g->sp;

	if (!isKindOfSlot(a, class_string)) return errWrongType;

	char *cmdline = new char[slotRawObject(a)->size + 1];
	slotStrVal(a, cmdline, slotRawObject(a)->size + 1);

#ifdef SC_IPHONE
	SetInt(a, 0);
	return errNone;
#endif

	sc_process *process = new sc_process;
	process->stream = sc_popen(cmdline, &process->pid, "r");
	setvbuf(process->stream, 0, _IONBF, 0);
	pid_t pid = process->pid;

	process->postOutput = IsTrue(b);

	delete [] cmdline;

	if(process->stream == NULL) {
		delete process;
		return errFailed;
	}

	SC_Thread thread(std::bind(string_popen_thread_func, process));
	thread.detach();

	SetInt(a, pid);
	return errNone;
}

int prString_LoadLibrarySymbols(struct VMGlobals *g, int numArgsPushed);
int prString_LoadLibrarySymbols(struct VMGlobals *g, int numArgsPushed)
{
    PyrSlot *a = g->sp;
    
    if (!isKindOfSlot(a, class_string)) return errWrongType;
    
    char *libpath = new char[slotRawObject(a)->size + 1];
    slotStrVal(a, libpath, slotRawObject(a)->size + 1);
    
    DLSyms* syms = dlSymsInit(libpath);
    if (syms) {
        int symCount = dlSymsCount(syms);

        PyrObject *obj = newPyrArray(g->gc, symCount, 0, true);
        obj->size = 0;
        PyrSlot *array = obj->slots;
        SetObject(a, obj);

        for (int i = 0; i < symCount; ++i) {
            const char* symbol = dlSymsName(syms, i);
            if (symbol) {
                int size = strlen(symbol); // Discard trailing newline
                PyrString *symString = newPyrStringN(g->gc, size, 0, true);
                memcpy(symString->s, symbol, size);
                SetObject(&array[i], symString);
                g->gc->GCWrite(obj, symString); // we know matched_string is white so we can use GCWriteNew
                obj->size++;
            }
        }
        
        dlSymsCleanup(syms);
    }
    
    delete [] libpath;

    return errNone;
}

struct DcLibrary
    : boost::noncopyable
{
    DcLibrary(const char* libpath) {
        lib = dlLoadLibrary(libpath);
    }
    
    ~DcLibrary() {
        //if (lib) { dlFreeLibrary(lib); }
    }
    
    DLLib* operator->() const { return lib; }
    DLLib* operator*() const {  return lib; }
    operator bool() const {  return !!lib; }

    DLLib*  lib;
};

struct DcVM
    : boost::noncopyable
{
    DcVM() {
        vm = dcNewCallVM(4096);
        if (vm) {
            dcMode(vm, DC_CALL_C_DEFAULT);
            dcReset(vm);
        }
    }
    
    ~DcVM() {
        if (vm) dcFree(vm);
    }
    
    DCCallVM* operator->() const { return vm; }
    DCCallVM* operator*() const { return vm; }
    operator bool() const {  return !!vm; }

    DCCallVM* vm;
};

int prString_CallLibrarySymbol(struct VMGlobals *g, int numArgsPushed);
int prString_CallLibrarySymbol(struct VMGlobals *g, int numArgsPushed)
{
    PyrSlot *thisSlot       = g->sp - 3;
    PyrSlot *symbolSlot     = g->sp - 2;
    PyrSlot *signatureSlot  = g->sp - 1;
    PyrSlot *argsSlot       = g->sp - 0;

    if (!isKindOfSlot(thisSlot, class_string)) return errWrongType;
    if (!isKindOfSlot(symbolSlot, class_string)) return errWrongType;
    if (!isKindOfSlot(signatureSlot, class_string)) return errWrongType;
    if (!isKindOfSlot(argsSlot, class_array)) return errWrongType;

    char* libpath = new char[slotRawObject(thisSlot)->size + 1];
    slotStrVal(thisSlot, libpath, slotRawObject(thisSlot)->size + 1);

    char* symbol = new char[slotRawObject(symbolSlot)->size + 1];
    slotStrVal(symbolSlot, symbol, slotRawObject(symbolSlot)->size + 1);

    char* signature = new char[slotRawObject(signatureSlot)->size + 1];
    slotStrVal(signatureSlot, signature, slotRawObject(signatureSlot)->size + 1);

    PyrObject* args = slotRawObject(argsSlot);

    DcLibrary lib(libpath);
    if (lib) {
        void* funcPtr = dlFindSymbol(*lib, symbol);
        
        if (funcPtr) {
            DcVM vm;
                    
            int i = 0;
            for (; i < args->size; ++i) {
                char sigType = signature[i];
                
                if (sigType == 0 || args->size <= i) {
                    // We've either run out of signature, or run out of arguments
                    return errFailed;
                }
                
                PyrSlot argSlot;
                getIndexedSlot(args, &argSlot, i);
                
                switch (sigType) {
                    case 'B': {
                        if (IsTrue(&argSlot)) {
                            dcArgBool(*vm, true);
                        } else if (IsFalse(&argSlot)) {
                            dcArgBool(*vm, false);
                        } else {
                            return errWrongType;
                        }
                        break;
                    }
                    case 'i': {
                        int i = 0;
                        if (slotVal(&argSlot, &i)) {
                            return errWrongType;
                        }
                        dcArgInt(*vm, (DCint)i);
                        break;
                    }
                    case 'I': {
                        uint ui = 0;
                        if (slotVal(&argSlot, &ui)) {
                            return errWrongType;
                        }
                        dcArgInt(*vm, (DCint)(DCuint)i);
                        break;
                    }
                    case 'j': {
                        long l = 0;
                        if (slotVal(&argSlot, &l)) {
                            return errWrongType;
                        }
                        dcArgLong(*vm, (DClong)i);
                        break;
                    }
                    case 'J': {
                        unsigned long ul = 0;
                        if (slotVal(&argSlot, &ul)) {
                            return errWrongType;
                        }
                        dcArgLong(*vm, (DClong)(DCulong)ul);
                        break;
                    }
                    case 'f': {
                        float f = 0;
                        if (slotVal(&argSlot, &f)) {
                            return errWrongType;
                        }
                        dcArgFloat(*vm, (DCfloat)f);
                        break;
                    }
                    case 'd': {
                        double d = 0;
                        if (slotVal(&argSlot, &d)) {
                            return errWrongType;
                        }
                        dcArgDouble(*vm, (DCdouble)d);
                        break;
                    }
                    case 'Z': {
                        if (!isKindOfSlot(&argSlot, class_array)) return errWrongType;
                        char* argStr = new char[slotRawObject(&argSlot)->size + 1];
                        slotStrVal(signatureSlot, signature, slotRawObject(signatureSlot)->size + 1);
                        dcArgPointer(*vm, argStr);
                    }
                    default:
                        return errFailed;
                }
            }
                
            i++; // skip ")" in signature
            char returnType = signature[i];
            if (returnType == 0) return errFailed;
            
            switch (returnType) {
                case 'B': {
                    SetBool(thisSlot, dcCallBool(*vm, funcPtr));
                    break;
                }
                case 'i': {
                    SetInt(thisSlot, dcCallInt(*vm, funcPtr));
                    break;
                }
                case 'I': {
                    SetInt(thisSlot, (DCuint)dcCallInt(*vm, funcPtr));
                    break;
                }
                case 'j': {
                    SetInt(thisSlot, dcCallLong(*vm, funcPtr));
                    break;
                }
                case 'J': {
                    SetInt(thisSlot, (DCulong)dcCallLong(*vm, funcPtr));
                    break;
                }
                case 'f': {
                    SetFloat(thisSlot, dcCallFloat(*vm, funcPtr));
                    break;
                }
                case 'd': {
                    SetFloat(thisSlot, dcCallDouble(*vm, funcPtr));
                    break;
                }
                case 'Z': {
                    const char* result = (const char*)dcCallPointer(*vm, funcPtr);
                    int size = strlen(result);
                    PyrString *strobj = newPyrStringN(g->gc, size, 0, true);
                    memcpy(strobj->s, result, size);
                    SetObject(thisSlot, strobj);
                    break;
                }
                default:
                    return errWrongType;
            }
        }
    }
    
    delete [] libpath;
    
    return errNone;
}


int prArrayPOpen(struct VMGlobals *g, int numArgsPushed);
int prArrayPOpen(struct VMGlobals *g, int numArgsPushed)
{
	PyrObject *obj;

	PyrSlot *a = g->sp - 1;
	PyrSlot *b = g->sp;

#ifdef SC_IPHONE
	SetInt(a, 0);
	return errNone;
#endif

	if (NotObj(a)) return errWrongType;

	obj = slotRawObject(a);
	if (!(slotRawInt(&obj->classptr->classFlags) & classHasIndexableInstances))
		return errNotAnIndexableObject;

	if( obj->size < 1)
		return errFailed;

	PyrSlot filenameSlot;
	getIndexedSlot(obj, &filenameSlot, 0);
	if (!isKindOfSlot(&filenameSlot, class_string)) return errWrongType;
	char filename[PATH_MAX];
	if (slotRawObject(&filenameSlot)->size > PATH_MAX - 1) return errFailed;
	slotStrVal(&filenameSlot, filename, slotRawObject(&filenameSlot)->size + 1);

	std::vector<char *> argv (obj->size + 1);

	bfs::path p;
	p /= filename;
	std::string filenameOnly = p.filename().string();
	std::vector<char> vfilenameOnly(filenameOnly.begin(), filenameOnly.end());
	vfilenameOnly.push_back('\0');

	argv[0] = vfilenameOnly.data();
	argv[obj->size] = NULL;

	if(obj->size > 1) {
		for (int i=1; i<obj->size; ++i) {
			PyrSlot argSlot;
			getIndexedSlot(obj, &argSlot, i);
			if (!isKindOfSlot(&argSlot, class_string)) return errWrongType;
			char *arg = new char[slotRawObject(&argSlot)->size + 1];
			slotStrVal(&argSlot, arg, slotRawObject(&argSlot)->size + 1);
			argv[i] = arg;
		}
	}

	sc_process *process = new sc_process;
	process->stream = sc_popen_argv(filename, argv.data(), &process->pid, "r");
	setvbuf(process->stream, 0, _IONBF, 0);
	pid_t pid = process->pid;

	process->postOutput = IsTrue(b);

	if(process->stream == NULL) {
		delete process;
		return errFailed;
	}

	SC_Thread thread(std::bind(string_popen_thread_func, process));
	thread.detach();

	for (int i=1; i<obj->size; ++i) {
		delete [] argv[i];
	}

	SetInt(a, pid);
	return errNone;

}

int prPidRunning(VMGlobals *g, int numArgsPushed);
int prPidRunning(VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a;

	a = g->sp;

#ifdef _WIN32
	HANDLE handle;

	handle = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, slotRawInt(a));
	if(handle) {
		unsigned long exitCode;

		if(GetExitCodeProcess(handle, &exitCode) == 0)
			SetFalse(a);
		else if(exitCode == STILL_ACTIVE)
			SetTrue(a);

		CloseHandle(handle);
	}
	else
		SetFalse(a);
#else
	if(kill(slotRawInt(a), 0) == 0)
		SetTrue(a);
	else
		SetFalse(a);
#endif

	return errNone;
}

int prUnix_Errno(struct VMGlobals *g, int numArgsPushed);
int prUnix_Errno(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a = g->sp;

	SetInt(a, errno);

	return errNone;
}

#include <time.h>

static void fillSlotsFromTime(PyrSlot * result, struct tm* tm, std::chrono::system_clock::time_point const & now)
{
	PyrSlot *slots = slotRawObject(result)->slots;

	SetInt(slots+0, tm->tm_year + 1900);
	SetInt(slots+1, tm->tm_mon + 1); // 0 based month ??
	SetInt(slots+2, tm->tm_mday);
	SetInt(slots+3, tm->tm_hour);
	SetInt(slots+4, tm->tm_min);
	SetInt(slots+5, tm->tm_sec);
	SetInt(slots+6, tm->tm_wday);
	SetFloat(slots+7, std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count() * 1.0e-9);
}

int prLocalTime(struct VMGlobals *g, int numArgsPushed)
{
	using namespace std::chrono;
	system_clock::time_point now = system_clock::now();
	time_t now_time_t = system_clock::to_time_t(now);
	struct tm* tm = localtime(&now_time_t);

	fillSlotsFromTime(g->sp, tm,  now);

	return errNone;
}

int prGMTime(struct VMGlobals *g, int numArgsPushed)
{
	using namespace std::chrono;
	system_clock::time_point now = system_clock::now();
	time_t now_time_t = system_clock::to_time_t(now);
	struct tm* tm = gmtime(&now_time_t);

	fillSlotsFromTime(g->sp, tm, now);
	return errNone;
}

int prAscTime(struct VMGlobals *g, int numArgsPushed);
int prAscTime(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a = g->sp;
	PyrSlot *slots = slotRawObject(a)->slots;

	if (IsNil(slots + 0)) {
		SetNil(a);
		return errNone;
	}

	struct tm tm0;

	if (slotIntVal(slots+0, &tm0.tm_year)) return errWrongType;
	tm0.tm_year -= 1900;
	if (slotIntVal(slots+1, &tm0.tm_mon)) return errWrongType;
	tm0.tm_mon -- ;
	if (slotIntVal(slots+2, &tm0.tm_mday)) return errWrongType;
	if (slotIntVal(slots+3, &tm0.tm_hour)) return errWrongType;
	if (slotIntVal(slots+4, &tm0.tm_min)) return errWrongType;
	if (slotIntVal(slots+5, &tm0.tm_sec)) return errWrongType;
	if (slotIntVal(slots+6, &tm0.tm_wday)) return errWrongType;

	const char *text = asctime(&tm0);

	int size = strlen(text) - 1; // Discard trailing newline
	PyrString *strobj = newPyrStringN(g->gc, size, 0, true);
	memcpy(strobj->s, text, size);

	SetObject(a, strobj);

	return errNone;
}

int prStrFTime(struct VMGlobals *g, int numArgsPushed);
int prStrFTime(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a = g->sp - 1;
	PyrSlot *b = g->sp;

	PyrSlot *slots = slotRawObject(a)->slots;

	if (IsNil(slots + 0)) {
		SetNil(a);
		return errNone;
	}

	struct tm tm0;

	if (slotIntVal(slots+0, &tm0.tm_year)) return errWrongType;
	tm0.tm_year -= 1900;
	if (slotIntVal(slots+1, &tm0.tm_mon)) return errWrongType;
	tm0.tm_mon --;
	if (slotIntVal(slots+2, &tm0.tm_mday)) return errWrongType;
	if (slotIntVal(slots+3, &tm0.tm_hour)) return errWrongType;
	if (slotIntVal(slots+4, &tm0.tm_min)) return errWrongType;
	if (slotIntVal(slots+5, &tm0.tm_sec)) return errWrongType;
	if (slotIntVal(slots+6, &tm0.tm_wday)) return errWrongType;

	char format[1024];
	if (slotStrVal(b, format, 1024)) return errWrongType;

	char buffer[1024];
	if (strftime(buffer, 1024, format, &tm0) != 0) {
		int size = strlen(buffer);
		PyrString *strobj = newPyrStringN(g->gc, size, 0, true);
		memcpy(strobj->s, buffer, size);

		SetObject(a, strobj);
	} else {
		error("could not convert the date to string with the give format");
		return errFailed;
	}
	return errNone;
}

int32 timeseed();

int prTimeSeed(struct VMGlobals *g, int numArgsPushed);
int prTimeSeed(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a = g->sp;
	SetInt(a, timeseed());
	return errNone;
}

int prGetPid(VMGlobals *g, int numArgsPushed);
int prGetPid(VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a = g->sp;
	SetInt(a,
#ifndef _WIN32
		getpid()
#else
		GetCurrentProcessId()
#endif
		);
	return errNone;
}


void initUnixPrimitives();
void initUnixPrimitives()
{
	int base, index = 0;

	base = nextPrimitiveIndex();

	s_unixCmdAction = getsym("doUnixCmdAction");

	definePrimitive(base, index++, "_String_System", prString_System, 1, 0);
	definePrimitive(base, index++, "_String_Basename", prString_Basename, 1, 0);
	definePrimitive(base, index++, "_String_Dirname", prString_Dirname, 1, 0);
	definePrimitive(base, index++, "_String_POpen", prString_POpen, 2, 0);
    definePrimitive(base, index++, "_StringLoadLibrarySymbols", prString_LoadLibrarySymbols, 1, 0);
    definePrimitive(base, index++, "_StringCallLibrarySymbol", prString_CallLibrarySymbol, 4, 0);
	definePrimitive(base, index++, "_Unix_Errno", prUnix_Errno, 1, 0);
	definePrimitive(base, index++, "_LocalTime", prLocalTime, 1, 0);
	definePrimitive(base, index++, "_GMTime", prGMTime, 1, 0);
	definePrimitive(base, index++, "_AscTime", prAscTime, 1, 0);
	definePrimitive(base, index++, "_prStrFTime", prStrFTime, 2, 0);
	definePrimitive(base, index++, "_TimeSeed", prTimeSeed, 1, 0);
	definePrimitive(base, index++, "_PidRunning", prPidRunning, 1, 0);
	definePrimitive(base, index++, "_GetPid", prGetPid, 1, 0);
	definePrimitive(base, index++, "_ArrayPOpen", prArrayPOpen, 2, 0);
    
}
