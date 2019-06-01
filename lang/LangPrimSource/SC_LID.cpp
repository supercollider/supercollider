/*
    Linux Input Device support.
    Copyright (c) 2004 stefan kersten.
    modifications by Marije Baalman 2006-9

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

    Linux Input Device interface, 2004 <sk>
*/

#include <atomic>

#include "SCBase.h"
#include "VMGlobals.h"
#include "PyrSymbolTable.h"
#include "PyrInterpreter.h"
#include "PyrKernel.h"

#include "PyrPrimitive.h"
#include "PyrObjectProto.h"
#include "PyrPrimitiveProto.h"
#include "PyrKernelProto.h"
#include "SC_InlineUnaryOp.h"
#include "SC_InlineBinaryOp.h"
#include "PyrSched.h"
#include "GC.h"
#include "SC_LanguageClient.h"

#if HAVE_LID
#    include <errno.h>
#    include <fcntl.h>
#    include <linux/input.h>
#    include <stdio.h>
#    include <string.h>
#    include <sys/select.h>
#    include <sys/types.h>
#    include <unistd.h>

#    define BITS_PER_LONG (sizeof(long) * 8)
#    define NBITS(x) ((((x)-1) / BITS_PER_LONG) + 1)
#    define OFF(x) ((x) % BITS_PER_LONG)
#    define BIT(x) (1UL << OFF(x))
#    define LONG(x) ((x) / BITS_PER_LONG)
#    define TEST_BIT(array, bit) (((array)[LONG(bit)] >> OFF(bit)) & 1)

extern bool compiledOK;

static PyrSymbol* s_inputDeviceClass = 0;
static PyrSymbol* s_inputDeviceInfoClass = 0;
static PyrSymbol* s_absInfoClass = 0;
static PyrSymbol* s_handleEvent = 0;
static PyrSymbol* s_readError = 0;

// =====================================================================
// SC_LID

struct SC_LID {
    SC_LID(PyrObject* obj);
    ~SC_LID();

    int open(const char* path);
    int close();

    bool isEventTypeSupported(int evtType);
    bool isEventCodeSupported(int evtType, int evtCode);

    int getName(char* buf, size_t size);
    int getInfo(struct input_id* info, char* bufPhys, size_t sizePhys, char* bufUniq, size_t sizeUniq);
    int getKeyState(int evtCode);
    int getAbsInfo(int evtCode, struct input_absinfo* info);

    int setLedState(int evtCode, int evtValue, int evtType);

    int grab(int flag);
    void handleEvent(struct input_event& evt, std::atomic<bool> const& shouldBeRunning);
    void readError(std::atomic<bool> const& shouldBeRunning);

    static PyrObject* getObject(PyrSlot* slot) {
        return isKindOfSlot(slot, s_inputDeviceClass->u.classobj) ? slotRawObject(slot) : 0;
    }

    static SC_LID* getDevice(PyrObject* obj) { return (SC_LID*)slotRawPtr(&obj->slots[0]); }

    SC_LID* m_next;
    PyrObject* m_obj;
    int m_fd;
    int m_lastEventType;
    unsigned long m_eventTypeCaps[NBITS(EV_MAX)];
    unsigned long m_eventCodeCaps[NBITS(KEY_MAX)];
    unsigned long m_keyState[NBITS(KEY_MAX)];
};

// =====================================================================
// SC_LIDManager

struct SC_LIDManager {
public:
    static SC_LIDManager& instance();

    int start();
    int stop();

    int add(SC_LID* dev);
    int remove(SC_LID* dev);

private:
    SC_LIDManager();
    ~SC_LIDManager();

    enum { kStop, kAdd, kRemove };

    struct Command {
        int id;
        union {
            SC_LID* dev;
        } arg;
    };

    int sendCommand(const Command& cmd);
    void devicesChanged();
    bool asyncAddDevice(SC_LID* dev);
    bool asyncRemoveDevice(SC_LID* dev);
    void loop();

    static void* threadFunc(void*);

    pthread_t m_thread;
    pthread_mutex_t m_mutex;
    std::atomic<bool> m_running;
    std::atomic<bool> mShouldBeRunning;
    int m_cmdFifo[2];
    int m_nfds;
    fd_set m_fds;
    SC_LID* m_devices;
};

// =====================================================================
// SC_LID

SC_LID::SC_LID(PyrObject* obj): m_next(0), m_obj(obj), m_fd(-1), m_lastEventType(-1) { SetPtr(obj->slots + 0, this); }

SC_LID::~SC_LID() {
    if (m_fd != -1)
        ::close(m_fd);
}

int SC_LID::open(const char* path) {
    m_fd = ::open(path, O_RDWR);

    if (m_fd == -1) {
        error("LID (1): %s\n", strerror(errno));
        return errFailed;
    }

    memset(m_eventTypeCaps, 0, sizeof(m_eventTypeCaps));
    if (ioctl(m_fd, EVIOCGBIT(0, EV_MAX), m_eventTypeCaps) == -1) {
        error("LID (2): %s\n", strerror(errno));
        return errFailed;
    }

    memset(m_keyState, 0, sizeof(m_keyState));
    if (ioctl(m_fd, EVIOCGKEY(sizeof(m_keyState)), m_keyState) == -1) {
        error("LID (3): %s\n", strerror(errno));
        return errFailed;
    }

    return SC_LIDManager::instance().add(this);
}

int SC_LID::close() {
    SetNil(m_obj->slots + 0);
    return SC_LIDManager::instance().remove(this);
}

bool SC_LID::isEventTypeSupported(int evtType) { return TEST_BIT(m_eventTypeCaps, evtType); }

bool SC_LID::isEventCodeSupported(int evtType, int evtCode) {
    if (evtType != m_lastEventType) {
        m_lastEventType = evtType;
        memset(m_eventCodeCaps, 0, sizeof(m_eventCodeCaps));
        if (ioctl(m_fd, EVIOCGBIT(evtType, KEY_MAX), m_eventCodeCaps) == -1) {
            post("LID failed to check event code (error %s)\n", strerror(errno));
            return false;
        }
    }
    return TEST_BIT(m_eventCodeCaps, evtCode);
}

int SC_LID::getName(char* buf, size_t size) {
    if (ioctl(m_fd, EVIOCGNAME(size), buf) == -1) {
        error("LID (5): %s\n", strerror(errno));
        return errFailed;
    }
    return errNone;
}

int SC_LID::getInfo(struct input_id* info, char* bufPhys, size_t sizePhys, char* bufUniq, size_t sizeUniq) {
    if (ioctl(m_fd, EVIOCGID, info) == -1) {
        error("LID (6): %s\n", strerror(errno));
        return errFailed;
    }
    if (ioctl(m_fd, EVIOCGPHYS(sizePhys), bufPhys) == -1) {
        // 		strcpy( sizePhys, strerror(errno));
        post("LID could not retrieve physical location (error: %s)\n", strerror(errno));
        // 		return errFailed;
    }
    if (ioctl(m_fd, EVIOCGUNIQ(sizeUniq), bufUniq) == -1) {
        // 		strcpy( strerror(errno), sizeof( strerror(errno)), sizeUniq );
        post("LID could not get unique identifier (error: %s)\n", strerror(errno));
        // 		return errFailed;
    }

    return errNone;
}

int SC_LID::getKeyState(int evtCode) { return TEST_BIT(m_keyState, evtCode); }

int SC_LID::getAbsInfo(int evtCode, struct input_absinfo* info) {
    if (ioctl(m_fd, EVIOCGABS(evtCode), info) == -1) {
        error("LID (9): %s\n", strerror(errno));
        return errFailed;
    }
    return errNone;
}

int SC_LID::setLedState(int evtCode, int evtValue, int evtType) { // added by marije baalman
    struct input_event ev;
    // 	post( "set led state called, putting event" );
    ev.code = evtCode;
    ev.value = evtValue;
    ev.type = evtType;
    // 	post( "m_fd %i", m_fd );
    // 	post( "code %i, value %i ", evtCode, evtValue );
    if (write(m_fd, &ev, sizeof(struct input_event)) == -1) {
        // 		post( "error writing event" );
        return errFailed;
    }
    return errNone;
}

int SC_LID::grab(int flag) {
    if (ioctl(m_fd, EVIOCGRAB, flag) == -1) {
        error("LID (10): %s\n", strerror(errno));
        return errFailed;
    }
    return errNone;
}

void SC_LID::handleEvent(struct input_event& evt, std::atomic<bool> const& shouldBeRunning) {
    if (evt.type != EV_SYN) {
        int status = lockLanguageOrQuit(shouldBeRunning);
        if (status == EINTR)
            return;
        if (status) {
            postfl("error when locking language (%d)\n", status);
            return;
        }

        if (compiledOK) {
            VMGlobals* g = gMainVMGlobals;
            g->canCallOS = false;
            ++g->sp;
            SetObject(g->sp, m_obj);
            ++g->sp;
            SetInt(g->sp, evt.type);
            ++g->sp;
            SetInt(g->sp, evt.code);
            ++g->sp;
            SetInt(g->sp, evt.value);
            runInterpreter(g, s_handleEvent, 4);
            g->canCallOS = false;
        }
        gLangMutex.unlock();
    }
}

void SC_LID::readError(std::atomic<bool> const& shouldBeRunning) {
    int status = lockLanguageOrQuit(shouldBeRunning);
    if (status == EINTR)
        return;
    if (status) {
        postfl("error when locking language (%d)\n", status);
        return;
    }

    if (compiledOK) {
        VMGlobals* g = gMainVMGlobals;
        g->canCallOS = false;
        ++g->sp;
        SetObject(g->sp, m_obj);
        runInterpreter(g, s_readError, 1);
        g->canCallOS = false;
    }
    gLangMutex.unlock();
}

// =====================================================================
// SC_LIDManager

SC_LIDManager& SC_LIDManager::instance() {
    static SC_LIDManager instance;
    return instance;
}

SC_LIDManager::SC_LIDManager(): m_running(false), m_devices(0) {
    if (pipe(m_cmdFifo) == -1) {
        m_cmdFifo[0] = m_cmdFifo[1] = -1;
    }
    devicesChanged();
}

SC_LIDManager::~SC_LIDManager() {
    close(m_cmdFifo[0]);
    close(m_cmdFifo[1]);
}

int SC_LIDManager::start() {
    mShouldBeRunning = true;
    int err = pthread_create(&m_thread, 0, &threadFunc, this);
    if (err != 0)
        return errFailed;
    return errNone;
}

int SC_LIDManager::stop() {
    if (m_running == false)
        return errNone;

    Command cmd;

    cmd.id = kStop;
    int err = sendCommand(cmd);
    if (err)
        return err;

    mShouldBeRunning = false;
    err = pthread_join(m_thread, 0);
    if (err != 0)
        return errFailed;

    return errNone;
}

int SC_LIDManager::add(SC_LID* dev) {
    Command cmd;
    cmd.id = kAdd;
    cmd.arg.dev = dev;
    return sendCommand(cmd);
}

int SC_LIDManager::remove(SC_LID* dev) {
    Command cmd;
    cmd.id = kRemove;
    cmd.arg.dev = dev;
    return sendCommand(cmd);
}

int SC_LIDManager::sendCommand(const Command& cmd) {
    return write(m_cmdFifo[1], &cmd, sizeof(cmd)) == sizeof(cmd) ? errNone : errFailed;
}

void SC_LIDManager::devicesChanged() {
    int fdMax = m_cmdFifo[0];

    FD_ZERO(&m_fds);
    FD_SET(fdMax, &m_fds);

    SC_LID* dev = m_devices;
    while (dev) {
        int fd = dev->m_fd;
        if (fd != -1) {
            FD_SET(fd, &m_fds);
            if (fd > fdMax)
                fdMax = fd;
        }
        dev = dev->m_next;
    }

    m_nfds = fdMax + 1;
}

bool SC_LIDManager::asyncAddDevice(SC_LID* dev) {
    if (dev->m_next)
        return false;
    dev->m_next = m_devices;
    m_devices = dev;
    devicesChanged();
    return true;
}

bool SC_LIDManager::asyncRemoveDevice(SC_LID* dev) {
    SC_LID *prev = 0, *cur = m_devices;

    while (cur) {
        if (cur == dev) {
            if (prev)
                prev->m_next = dev->m_next;
            else
                m_devices = dev->m_next;
            dev->m_next = 0;
            delete dev;
            devicesChanged();
            return true;
        }
        prev = cur;
        cur = cur->m_next;
    }

    return false;
}

void* SC_LIDManager::threadFunc(void* arg) {
    ((SC_LIDManager*)arg)->loop();
    return 0;
}

void SC_LIDManager::loop() {
    m_running = true;
    post("LID: event loop started\n");

    while (true) {
        fd_set fds;
        memcpy(&fds, &m_fds, sizeof(fd_set));
        int n = select(m_nfds, &fds, 0, 0, 0);
        if (n == -1) {
            if (errno == EINTR)
                continue;
            post("LID: error in input handler: %s\n", strerror(errno));
            goto quit;
        } else if (n > 0) {
            if (FD_ISSET(m_cmdFifo[0], &fds)) {
                Command cmd;
                --n;
                int err = read(m_cmdFifo[0], &cmd, sizeof(cmd));
                if (err == -1) {
                    if (errno != EINTR) {
                        post("LID: error in input handler: %s\n", strerror(errno));
                        goto quit;
                    }
                } else {
                    switch (cmd.id) {
                    case kStop:
                        goto quit;
                    case kAdd:
                        if (asyncAddDevice(cmd.arg.dev)) {
                            post("LID: added device %p\n", cmd.arg);
                        } else {
                            post("LID: cannot add device\n");
                        }
                        break;
                    case kRemove:
                        if (asyncRemoveDevice(cmd.arg.dev)) {
                            post("LID: removed device %p\n", cmd.arg);
                        } else {
                            post("LID: couldn't remove device\n");
                        }
                        break;
                    default:
                        post("LID: unknown command in input handler\n");
                    }
                }
            }
            if (n > 0) {
                SC_LID* dev = m_devices;
                while (dev) {
                    int fd = dev->m_fd;
                    if (FD_ISSET(fd, &fds)) {
                        struct input_event evt;
                        if (read(fd, &evt, sizeof(evt)) == sizeof(evt)) {
                            dev->handleEvent(evt, mShouldBeRunning);
                        } else {
                            dev->readError(mShouldBeRunning);
                        }
                    }
                    if (!mShouldBeRunning)
                        goto quit;
                    dev = dev->m_next;
                }
            }
        }
    }

quit:
    m_running = false;
    post("LID: event loop stopped\n");
}

// =====================================================================
// Primitive Interface

int prLID_Open(VMGlobals* g, int numArgsPushed) {
    PyrSlot* args = g->sp - 1;
    int err;

    PyrObject* obj = SC_LID::getObject(args + 0);
    if (!obj)
        return errWrongType;

    char path[PATH_MAX];
    err = slotStrVal(args + 1, path, sizeof(path));
    if (err)
        return err;

    SC_LID* dev = new SC_LID(obj);
    err = dev->open(path);
    if (err) {
        delete dev;
        return err;
    }

    return errNone;
}

int prLID_Close(VMGlobals* g, int numArgsPushed) {
    PyrSlot* args = g->sp;

    PyrObject* obj = SC_LID::getObject(args + 0);
    if (!obj)
        return errWrongType;

    SC_LID* dev = SC_LID::getDevice(obj);
    if (!dev)
        return errFailed;

    return dev->close();
}

int prLID_EventTypeSupported(VMGlobals* g, int numArgsPushed) {
    PyrSlot* args = g->sp - 1;
    int evtType;
    int err;

    if (!g->canCallOS)
        return errCantCallOS;

    PyrObject* obj = SC_LID::getObject(args + 0);
    if (!obj)
        return errWrongType;

    err = slotIntVal(args + 1, &evtType);
    if (err)
        return err;

    SC_LID* dev = SC_LID::getDevice(obj);
    if (!dev)
        return errFailed;

    SetBool(args, dev->isEventTypeSupported(evtType));

    return errNone;
}

int prLID_EventCodeSupported(VMGlobals* g, int numArgsPushed) {
    PyrSlot* args = g->sp - 2;
    int evtType, evtCode;
    int err;

    if (!g->canCallOS)
        return errCantCallOS;

    PyrObject* obj = SC_LID::getObject(args + 0);
    if (!obj)
        return errWrongType;

    err = slotIntVal(args + 1, &evtType);
    if (err)
        return err;

    err = slotIntVal(args + 2, &evtCode);
    if (err)
        return err;

    SC_LID* dev = SC_LID::getDevice(obj);
    if (!dev)
        return errFailed;

    SetBool(args, dev->isEventCodeSupported(evtType, evtCode));

    return errNone;
}

int prLID_GetInfo(VMGlobals* g, int numArgsPushed) {
    PyrSlot* args = g->sp - 1;
    int err;

    PyrObject* obj = SC_LID::getObject(args + 0);
    if (!obj)
        return errWrongType;

    if (!isKindOfSlot(args + 1, s_inputDeviceInfoClass->u.classobj))
        return errWrongType;
    PyrObject* infoObj = slotRawObject(&args[1]);

    SC_LID* dev = SC_LID::getDevice(obj);
    if (!dev)
        return errFailed;

    char name[128];
    err = dev->getName(name, sizeof(name));
    if (err)
        return err;

    struct input_id info;
    char namePhys[128];
    char nameUniq[128];
    err = dev->getInfo(&info, namePhys, sizeof(namePhys), nameUniq, sizeof(nameUniq));
    if (err)
        return err;

    SetSymbol(infoObj->slots + 0, getsym(name));
    SetInt(infoObj->slots + 1, info.bustype);
    SetInt(infoObj->slots + 2, info.vendor);
    SetInt(infoObj->slots + 3, info.product);
    SetInt(infoObj->slots + 4, info.version);
    SetSymbol(infoObj->slots + 5, getsym(namePhys));
    SetSymbol(infoObj->slots + 6, getsym(nameUniq));

    slotCopy(&args[0], &args[1]);

    return errNone;
}

int prLID_GetKeyState(VMGlobals* g, int numArgsPushed) {
    PyrSlot* args = g->sp - 1;
    int evtCode;
    int err;

    PyrObject* obj = SC_LID::getObject(args + 0);
    if (!obj)
        return errWrongType;

    err = slotIntVal(args + 1, &evtCode);
    if (err)
        return err;

    SC_LID* dev = SC_LID::getDevice(obj);
    if (!dev)
        return errFailed;

    SetInt(args, dev->getKeyState(evtCode));

    return errNone;
}

int prLID_GetAbsInfo(VMGlobals* g, int numArgsPushed) {
    PyrSlot* args = g->sp - 2;
    int evtCode;
    int err;

    PyrObject* obj = SC_LID::getObject(args + 0);
    if (!obj)
        return errWrongType;

    err = slotIntVal(args + 1, &evtCode);
    if (err)
        return err;

    if (!isKindOfSlot(args + 2, s_absInfoClass->u.classobj))
        return errWrongType;
    PyrObject* infoObj = slotRawObject(&args[2]);

    SC_LID* dev = SC_LID::getDevice(obj);
    if (!dev)
        return errFailed;

    struct input_absinfo info;
    err = dev->getAbsInfo(evtCode, &info);
    if (err)
        return err;

    SetInt(infoObj->slots + 0, info.value);
    SetInt(infoObj->slots + 1, info.minimum);
    SetInt(infoObj->slots + 2, info.maximum);
    SetInt(infoObj->slots + 3, info.fuzz);
    SetInt(infoObj->slots + 4, info.flat);

    slotCopy(&args[0], &args[2]);

    return errNone;
}

int prLID_Grab(VMGlobals* g, int numArgsPushed) {
    PyrSlot* args = g->sp - 1;

    PyrObject* obj = SC_LID::getObject(args + 0);
    if (!obj)
        return errWrongType;

    SC_LID* dev = SC_LID::getDevice(obj);
    if (!dev)
        return errFailed;

    return dev->grab(IsTrue(args + 1));
}

int prLID_Start(VMGlobals* g, int numArgsPushed) {
    // 	if (!g->canCallOS) return errCantCallOS;
    return SC_LIDManager::instance().start();
}

int prLID_Stop(VMGlobals* g, int numArgsPushed) {
    // 	if (!g->canCallOS) return errCantCallOS;
    return SC_LIDManager::instance().stop();
}

int prLID_SetLedState(VMGlobals* g, int numArgsPushed) {
    // 	post( "set led state primitive called" );
    PyrSlot* args = g->sp - 2;
    int evtCode, evtValue;
    int err;

    PyrObject* obj = SC_LID::getObject(args + 0);
    if (!obj)
        return errWrongType;

    err = slotIntVal(args + 1, &evtCode);
    if (err)
        return err;

    err = slotIntVal(args + 2, &evtValue);
    if (err)
        return err;

    SC_LID* dev = SC_LID::getDevice(obj);
    if (!dev)
        return errFailed;

    SetInt(args, dev->setLedState(evtCode, evtValue, EV_LED));
    return errNone;
}

int prLID_SetMscState(VMGlobals* g, int numArgsPushed) {
    // 	post( "set msc state primitive called\n" );
    PyrSlot* args = g->sp - 2;
    int evtCode, evtValue;
    int err;

    PyrObject* obj = SC_LID::getObject(args + 0);
    if (!obj)
        return errWrongType;

    err = slotIntVal(args + 1, &evtCode);
    if (err)
        return err;

    err = slotIntVal(args + 2, &evtValue);
    if (err)
        return err;

    SC_LID* dev = SC_LID::getDevice(obj);
    if (!dev)
        return errFailed;

    SetInt(args, dev->setLedState(evtCode, evtValue, EV_MSC));
    return errNone;
}

void SC_LIDInit() {
    int base, index;

    s_inputDeviceClass = getsym("LID");
    s_inputDeviceInfoClass = getsym("LIDInfo");
    s_absInfoClass = getsym("LIDAbsInfo");
    s_handleEvent = getsym("prHandleEvent");
    s_readError = getsym("prReadError");

    base = nextPrimitiveIndex();
    index = 0;

    definePrimitive(base, index++, "_LID_Open", prLID_Open, 2, 0);
    definePrimitive(base, index++, "_LID_Close", prLID_Close, 1, 0);
    definePrimitive(base, index++, "_LID_EventTypeSupported", prLID_EventTypeSupported, 2, 0);
    definePrimitive(base, index++, "_LID_EventCodeSupported", prLID_EventCodeSupported, 3, 0);
    definePrimitive(base, index++, "_LID_GetInfo", prLID_GetInfo, 2, 0);
    definePrimitive(base, index++, "_LID_GetKeyState", prLID_GetKeyState, 2, 0);
    definePrimitive(base, index++, "_LID_GetAbsInfo", prLID_GetAbsInfo, 3, 0);
    definePrimitive(base, index++, "_LID_Grab", prLID_Grab, 2, 0);
    definePrimitive(base, index++, "_LID_Start", prLID_Start, 1, 0);
    definePrimitive(base, index++, "_LID_Stop", prLID_Stop, 1, 0);
    definePrimitive(base, index++, "_LID_SetLedState", prLID_SetLedState, 3, 0); // added by Marije Baalman
    definePrimitive(base, index++, "_LID_SetMscState", prLID_SetMscState, 3, 0);
}

#else // !HAVE_LID
int prLID_Start(VMGlobals* g, int numArgsPushed) { return errNone; }

int prLID_Stop(VMGlobals* g, int numArgsPushed) { return errNone; }

void SC_LIDInit() {
    int base, index;

    base = nextPrimitiveIndex();
    index = 0;

    definePrimitive(base, index++, "_LID_Start", prLID_Start, 1, 0);
    definePrimitive(base, index++, "_LID_Stop", prLID_Stop, 1, 0);
}

#endif // HAVE_LID

void initLIDPrimitives() { SC_LIDInit(); }

// EOF
