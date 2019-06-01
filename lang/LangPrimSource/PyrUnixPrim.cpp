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

#include <tuple>
#include <vector>

#include "PyrObject.h"
#include "PyrPrimitive.h"
#include "PyrObject.h"
#include "PyrKernel.h"
#include "PyrSched.h"
#include "PyrFilePrim.h"
#include "VMGlobals.h"
#include "GC.h"
#include "SC_RGen.h"
#include "SC_Filesystem.hpp"
#include "sc_popen.h"
#include "SCBase.h"

#include "SC_Lock.h"

#include <boost/filesystem.hpp>

#ifdef _WIN32
#    include "SC_Win32Utils.h"
#else
#    include <libgen.h>
#endif

namespace bfs = boost::filesystem;

extern bool compiledOK;
PyrSymbol* s_unixCmdAction;

int prString_System(struct VMGlobals* g, int numArgsPushed);
int prString_System(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* a = g->sp;

    char cmdline[1024];
    int err = slotStrVal(a, cmdline, 1023);
    if (err)
        return err;

    int res = system(cmdline);
    SetInt(a, res);

    return errNone;
}

int prString_Basename(struct VMGlobals* g, int numArgsPushed);
int prString_Basename(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* a = g->sp;

    char path[PATH_MAX];
    int err = slotStrVal(a, path, PATH_MAX);
    if (err)
        return err;

    char* basename0 = basename(path);

    int size = strlen(basename0);
    PyrString* strobj = newPyrStringN(g->gc, size, 0, true);
    memcpy(strobj->s, basename0, size);

    SetObject(a, strobj);

    return errNone;
}

int prString_Dirname(struct VMGlobals* g, int numArgsPushed);
int prString_Dirname(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* a = g->sp;

    char path[PATH_MAX];
    int err = slotStrVal(a, path, PATH_MAX);
    if (err)
        return err;

    char* dirname0 = dirname(path);

    int size = strlen(dirname0);
    PyrString* strobj = newPyrStringN(g->gc, size, 0, true);
    memcpy(strobj->s, dirname0, size);

    SetObject(a, strobj);

    return errNone;
}

static void string_popen_thread_func(pid_t pid, FILE* stream, bool postOutput) {
    char buf[1024];

    setvbuf(stream, 0, _IONBF, 0);

    while (postOutput) {
        char* string = fgets(buf, 1024, stream);
        if (!string)
            break;
        postText(string, strlen(string));
    }

    int res;
    res = sc_pclose(stream, pid);
    res = WEXITSTATUS(res);

    gLangMutex.lock();
    if (compiledOK) {
        VMGlobals* g = gMainVMGlobals;
        ++g->sp;
        SetObject(g->sp, class_string);
        ++g->sp;
        SetInt(g->sp, res);
        ++g->sp;
        SetInt(g->sp, pid);
        runInterpreter(g, s_unixCmdAction, 3);
    }
    gLangMutex.unlock();
}

int prString_POpen(struct VMGlobals* g, int numArgsPushed);
int prString_POpen(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* callerSlot = g->sp - 1;
    PyrSlot* postOutputSlot = g->sp;

    int error;
    std::string cmdline;
    std::tie(error, cmdline) = slotStrStdStrVal(callerSlot);
    if (error != errNone)
        return error;

#ifdef SC_IPHONE
    SetInt(a, 0);
    return errNone;
#endif

    pid_t pid;
    FILE* stream;
    std::tie(pid, stream) = sc_popen(std::move(cmdline), "r");
    if (stream != nullptr) {
        SC_Thread thread(std::bind(string_popen_thread_func, pid, stream, IsTrue(postOutputSlot)));
        thread.detach();
        SetInt(callerSlot, pid);
        return errNone;
    } else
        return errFailed;
}

int prArrayPOpen(struct VMGlobals* g, int numArgsPushed);
int prArrayPOpen(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* callerSlot = g->sp - 1;
    PyrSlot* postOutputSlot = g->sp;

#ifdef SC_IPHONE
    SetInt(a, 0);
    return errNone;
#endif

    if (NotObj(callerSlot))
        return errWrongType;

    PyrObject* argsColl = slotRawObject(callerSlot);
    // argsColl must be a collection
    if (!(slotRawInt(&argsColl->classptr->classFlags) & classHasIndexableInstances))
        return errNotAnIndexableObject;

    // collection must contain at least one string: the path of executable to run
    if (argsColl->size < 1)
        return errFailed;

    int error;
    std::vector<std::string> strings;
    std::tie(error, strings) = PyrCollToVectorStdString(argsColl);
    if (error != errNone)
        return error;

    pid_t pid;
    FILE* stream;
    std::tie(pid, stream) = sc_popen_argv(strings, "r");
    if (stream != nullptr) {
        SC_Thread thread(std::bind(string_popen_thread_func, pid, stream, IsTrue(postOutputSlot)));
        thread.detach();
        SetInt(callerSlot, pid);
        return errNone;
    } else {
        return errFailed;
    }
}

int prPidRunning(VMGlobals* g, int numArgsPushed);
int prPidRunning(VMGlobals* g, int numArgsPushed) {
    PyrSlot* a;

    a = g->sp;

#ifdef _WIN32
    HANDLE handle;

    handle = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, slotRawInt(a));
    if (handle) {
        unsigned long exitCode;

        if (GetExitCodeProcess(handle, &exitCode) == 0)
            SetFalse(a);
        else if (exitCode == STILL_ACTIVE)
            SetTrue(a);

        CloseHandle(handle);
    } else
        SetFalse(a);
#else
    if (kill(slotRawInt(a), 0) == 0)
        SetTrue(a);
    else
        SetFalse(a);
#endif

    return errNone;
}

int prUnix_Errno(struct VMGlobals* g, int numArgsPushed);
int prUnix_Errno(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* a = g->sp;

    SetInt(a, errno);

    return errNone;
}

#include <time.h>

static void fillSlotsFromTime(PyrSlot* result, struct tm* tm, std::chrono::system_clock::time_point const& now) {
    PyrSlot* slots = slotRawObject(result)->slots;

    SetInt(slots + 0, tm->tm_year + 1900);
    SetInt(slots + 1, tm->tm_mon + 1); // 0 based month ??
    SetInt(slots + 2, tm->tm_mday);
    SetInt(slots + 3, tm->tm_hour);
    SetInt(slots + 4, tm->tm_min);
    SetInt(slots + 5, tm->tm_sec);
    SetInt(slots + 6, tm->tm_wday);
    SetFloat(slots + 7, std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count() * 1.0e-9);
}

int prLocalTime(struct VMGlobals* g, int numArgsPushed) {
    using namespace std::chrono;
    system_clock::time_point now = system_clock::now();
    time_t now_time_t = system_clock::to_time_t(now);
    struct tm* tm = localtime(&now_time_t);

    fillSlotsFromTime(g->sp, tm, now);

    return errNone;
}

int prGMTime(struct VMGlobals* g, int numArgsPushed) {
    using namespace std::chrono;
    system_clock::time_point now = system_clock::now();
    time_t now_time_t = system_clock::to_time_t(now);
    struct tm* tm = gmtime(&now_time_t);

    fillSlotsFromTime(g->sp, tm, now);
    return errNone;
}

int prAscTime(struct VMGlobals* g, int numArgsPushed);
int prAscTime(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* a = g->sp;
    PyrSlot* slots = slotRawObject(a)->slots;

    if (IsNil(slots + 0)) {
        SetNil(a);
        return errNone;
    }

    struct tm tm0;

    if (slotIntVal(slots + 0, &tm0.tm_year))
        return errWrongType;
    tm0.tm_year -= 1900;
    if (slotIntVal(slots + 1, &tm0.tm_mon))
        return errWrongType;
    tm0.tm_mon--;
    if (slotIntVal(slots + 2, &tm0.tm_mday))
        return errWrongType;
    if (slotIntVal(slots + 3, &tm0.tm_hour))
        return errWrongType;
    if (slotIntVal(slots + 4, &tm0.tm_min))
        return errWrongType;
    if (slotIntVal(slots + 5, &tm0.tm_sec))
        return errWrongType;
    if (slotIntVal(slots + 6, &tm0.tm_wday))
        return errWrongType;

    const char* text = asctime(&tm0);

    int size = strlen(text) - 1; // Discard trailing newline
    PyrString* strobj = newPyrStringN(g->gc, size, 0, true);
    memcpy(strobj->s, text, size);

    SetObject(a, strobj);

    return errNone;
}

int prStrFTime(struct VMGlobals* g, int numArgsPushed);
int prStrFTime(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* a = g->sp - 1;
    PyrSlot* b = g->sp;

    PyrSlot* slots = slotRawObject(a)->slots;

    if (IsNil(slots + 0)) {
        SetNil(a);
        return errNone;
    }

    struct tm tm0;

    if (slotIntVal(slots + 0, &tm0.tm_year))
        return errWrongType;
    tm0.tm_year -= 1900;
    if (slotIntVal(slots + 1, &tm0.tm_mon))
        return errWrongType;
    tm0.tm_mon--;
    if (slotIntVal(slots + 2, &tm0.tm_mday))
        return errWrongType;
    if (slotIntVal(slots + 3, &tm0.tm_hour))
        return errWrongType;
    if (slotIntVal(slots + 4, &tm0.tm_min))
        return errWrongType;
    if (slotIntVal(slots + 5, &tm0.tm_sec))
        return errWrongType;
    if (slotIntVal(slots + 6, &tm0.tm_wday))
        return errWrongType;

    char format[1024];
    if (slotStrVal(b, format, 1024))
        return errWrongType;

    char buffer[1024];
    if (strftime(buffer, 1024, format, &tm0) != 0) {
        int size = strlen(buffer);
        PyrString* strobj = newPyrStringN(g->gc, size, 0, true);
        memcpy(strobj->s, buffer, size);

        SetObject(a, strobj);
    } else {
        error("could not convert the date to string with the give format");
        return errFailed;
    }
    return errNone;
}

int32 timeseed();

int prTimeSeed(struct VMGlobals* g, int numArgsPushed);
int prTimeSeed(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* a = g->sp;
    SetInt(a, timeseed());
    return errNone;
}

int prGetPid(VMGlobals* g, int numArgsPushed);
int prGetPid(VMGlobals* g, int numArgsPushed) {
    PyrSlot* a = g->sp;
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
void initUnixPrimitives() {
    int base, index = 0;

    base = nextPrimitiveIndex();

    s_unixCmdAction = getsym("doUnixCmdAction");

    definePrimitive(base, index++, "_String_System", prString_System, 1, 0);
    definePrimitive(base, index++, "_String_Basename", prString_Basename, 1, 0);
    definePrimitive(base, index++, "_String_Dirname", prString_Dirname, 1, 0);
    definePrimitive(base, index++, "_String_POpen", prString_POpen, 2, 0);
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
