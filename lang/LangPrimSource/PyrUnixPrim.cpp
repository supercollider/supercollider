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
#include <boost/date_time/posix_time/posix_time_io.hpp>

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

#include <filesystem>

#ifdef _WIN32
#    include "SC_Win32Utils.h"
#else
#    include <libgen.h>
#endif

namespace fs = std::filesystem;

extern bool compiledOK;
PyrSymbol* s_unixCmdAction;

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

    setvbuf(stream, nullptr, _IONBF, 0);

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
    std::tie(pid, stream) = sc_popen_shell(std::move(cmdline), "r");
    if (stream != nullptr) {
        SC_Thread thread(std::bind(string_popen_thread_func, pid, stream, IsTrue(postOutputSlot)));
        thread.detach();
        SetInt(callerSlot, pid);
        return errNone;
    } else
        return errFailed;
}

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

int prUnix_Errno(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* a = g->sp;

    SetInt(a, errno);

    return errNone;
}

#include <time.h>

// Set only Date instance variables related to the tm struct: YMD, HMS
// and weekday (not rawSeconds)
static void fillSlotsFromTimeStruct(PyrSlot* result, struct tm* tm) {
    PyrSlot* slots = slotRawObject(result)->slots;

    SetInt(slots + 0, tm->tm_year + 1900);
    SetInt(slots + 1, tm->tm_mon + 1); // 0 based month
    SetInt(slots + 2, tm->tm_mday);
    SetInt(slots + 3, tm->tm_hour);
    SetInt(slots + 4, tm->tm_min);
    SetInt(slots + 5, tm->tm_sec);
    SetInt(slots + 6, tm->tm_wday);
}

// Set all Date instance variables (including rawSeconds)
static void fillAllSlotsFromTime(PyrSlot* result, struct tm* tm, std::chrono::system_clock::time_point const& now) {
    fillSlotsFromTimeStruct(result, tm);

    PyrSlot* slots = slotRawObject(result)->slots;
    using namespace std::chrono;
    auto const secondsPerTick = static_cast<double>(system_clock::period::num) / system_clock::period::den;
    SetFloat(slots + 7, duration_cast<system_clock::duration>(now.time_since_epoch()).count() * secondsPerTick);
}

int prLocalTime(struct VMGlobals* g, int numArgsPushed) {
    using namespace std::chrono;
    system_clock::time_point now = system_clock::now();
    time_t now_time_t = system_clock::to_time_t(now);
    struct tm* tm = localtime(&now_time_t);

    fillAllSlotsFromTime(g->sp, tm, now);
    return errNone;
}

int prGMTime(struct VMGlobals* g, int numArgsPushed) {
    using namespace std::chrono;
    system_clock::time_point now = system_clock::now();
    time_t now_time_t = system_clock::to_time_t(now);
    struct tm* tm = gmtime(&now_time_t);

    fillAllSlotsFromTime(g->sp, tm, now);
    return errNone;
}

int prDateFromString(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* dateObjectSlot = g->sp - 2;
    PyrSlot* timeSlot = g->sp - 1;
    PyrSlot* formatSlot = g->sp;

    const auto len = 1024;
    char timeString[len];
    if (slotStrVal(timeSlot, timeString, len))
        return errWrongType;
    char formatString[len];
    if (slotStrVal(formatSlot, formatString, len))
        return errWrongType;

    using namespace boost::posix_time;
    auto facet = new time_input_facet(); // will be owned/deleted by the locale
    facet->format(formatString);
    std::istringstream ss(timeString);
    ss.imbue(std::locale(ss.getloc(), facet));
    ptime pt;
    if (!(ss >> pt)) {
        error("time parsing failed\n");
        return errFailed;
    }
    auto tm0 = to_tm(pt);

    tm0.tm_isdst = -1; // attempt to determine if Daylight Saving Time in effect
    auto timePoint = std::chrono::system_clock::from_time_t(mktime(&tm0));
    fillAllSlotsFromTime(dateObjectSlot, &tm0, timePoint);
    return errNone;
}

// Given a Date structure, calculate and fill in any missing
// or invalid properties, ignoring/overwriting the contents of
// rawSeconds.  If some of the (MD, HMS) instance variables are
// nil, give them default values.  If year is nil, throw an error.
int prDateResolve(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* a = g->sp;
    PyrSlot* slots = slotRawObject(a)->slots;

    struct tm tm0 {};

    if (slotIntVal(slots + 0, &tm0.tm_year)) {
        error("year must be a valid Integer\n");
        return errWrongType;
    }
    tm0.tm_year -= 1900; // year in tm is relative to 1900

    if (slotIntVal(slots + 1, &tm0.tm_mon)) {
        tm0.tm_mon = 0; // default to January
    } else {
        tm0.tm_mon--; // month in tm is 0-based
    }
    if (slotIntVal(slots + 2, &tm0.tm_mday)) {
        tm0.tm_mday = 1; // default to first of month
    }
    if (slotIntVal(slots + 3, &tm0.tm_hour)) {
        tm0.tm_hour = 0; // default to 0 hours
    }
    if (slotIntVal(slots + 4, &tm0.tm_min)) {
        tm0.tm_min = 0; // default to 0 minutes
    }
    if (slotIntVal(slots + 5, &tm0.tm_sec)) {
        tm0.tm_sec = 0; // default to 0 seconds
    }
    tm0.tm_isdst = -1; // attempt to determine if Daylight Saving Time in effect

    time_t tt = mktime(&tm0);
    if (tt == -1) {
        error("no valid time\n");
        return errFailed;
    }

    // Fill in the missing weekday and rawSeconds fields.
    // Also, update the other fields, which may have changed if
    // they were previously outside valid ranges (this allows
    // for simple date math, such as adding or subtracing day,
    // years or months, then calling ResolveDate to revalidate).
    auto timePoint = std::chrono::system_clock::from_time_t(tt);
    fillAllSlotsFromTime(a, &tm0, timePoint);
    return errNone;
}

// Given only the rawSeconds property in a Date structure, calculate
// and fill in the rest of its properties (YMD, HMS and dayOfWeek).
int prDateResolveFromRawSeconds(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* a = g->sp;
    PyrSlot* slots = slotRawObject(a)->slots;

    double rawSeconds {};
    if (slotDoubleVal(slots + 7, &rawSeconds))
        return errWrongType;

    using namespace std::chrono;

    // Perform the reverse operation of fillAllSlotsFromTime()
    // to get the time_point from (double) seconds
    auto const ticksPerSecond = static_cast<double>(system_clock::period::den) / system_clock::period::num;
    auto elapsed = system_clock::duration(static_cast<system_clock::rep>(rawSeconds * ticksPerSecond));
    auto epoch = system_clock::time_point();
    auto timePoint = epoch + elapsed;

    auto tt = system_clock::to_time_t(timePoint);
    auto tm = localtime(&tt);

    // Set everything except rawSeconds
    fillSlotsFromTimeStruct(a, tm);
    return errNone;
}

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

int prTimeSeed(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* a = g->sp;
    SetInt(a, timeseed());
    return errNone;
}

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
    definePrimitive(base, index++, "_Date_FromString", prDateFromString, 3, 0);
    definePrimitive(base, index++, "_Date_Resolve", prDateResolve, 1, 0);
    definePrimitive(base, index++, "_Date_ResolveFromRawSeconds", prDateResolveFromRawSeconds, 1, 0);
    definePrimitive(base, index++, "_AscTime", prAscTime, 1, 0);
    definePrimitive(base, index++, "_prStrFTime", prStrFTime, 2, 0);
    definePrimitive(base, index++, "_TimeSeed", prTimeSeed, 1, 0);
    definePrimitive(base, index++, "_PidRunning", prPidRunning, 1, 0);
    definePrimitive(base, index++, "_GetPid", prGetPid, 1, 0);
    definePrimitive(base, index++, "_ArrayPOpen", prArrayPOpen, 2, 0);
}
