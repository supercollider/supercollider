/*
    HIDAPI based HID support.
    Copyright (c) 2013 Marije Baalman, Tim Blechmann

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

*/

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

#include <atomic>
#include <cstring>

#include "SC_LanguageClient.h"

extern bool compiledOK;

#ifdef HAVE_HIDAPI

#    if 1
static inline void trace(const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vpost(fmt, ap);
    va_end(ap);
}
#    else
static inline void trace(...) {}

#    endif


// needed for string conversions
#    include <wchar.h>
#    include <cstdlib>

#    include <hidapi.h>
#    include <hidapi_parser.h>

#    include <map>

typedef std::map<int, hid_dev_desc*> hid_map_t;

// helper function to convert from unicode to ansi
char* wchar_to_char(wchar_t* wchs) {
    if (wchs == nullptr)
        return nullptr;

    int len = wcslen(wchs) + 1;
    char* chs = (char*)malloc(sizeof(char) * len);
    std::wcstombs(chs, wchs, len);
    return chs;
}

wchar_t* char_to_wchar(char* chs) {
    if (chs == nullptr)
        return nullptr;

    int len = std::strlen(chs) + 1;
    wchar_t* wchs = (wchar_t*)malloc(sizeof(wchar_t) * len);
    std::mbstowcs(wchs, chs, len);
    return wchs;
}

static PyrSymbol* s_hidapi = nullptr;
static PyrSymbol* s_hidElementData = nullptr;
static PyrSymbol* s_hidDeviceData = nullptr;
static PyrSymbol* s_hidClosed = nullptr;


class SC_HID_API_Threadpool {
    typedef std::map<hid_dev_desc*, std::thread> ThreadMap;

public:
    void openDevice(hid_dev_desc* desc, std::atomic<bool>& shouldBeRunning) {
        trace("open device %p\n");
        std::lock_guard<std::mutex> lock(guard);
        if (map.find(desc) != map.end())
            // thread already polling device
            return;

        std::thread deviceThread([=, &shouldBeRunning] {
            trace("start polling thread for %d\n", desc);

            while (true) {
                unsigned char buf[256];

                int res = hid_read_timeout(desc->device, buf, sizeof(buf), 250);
                if (res > 0) {
                    hid_parse_input_report(buf, res, desc);
                } else if (res == -1) {
                    trace("device thread interrupted \n");
                    hid_throw_readerror(desc);
                    trace("device thread closed device \n");
                    return;
                }
            }
            std::lock_guard<std::mutex> lock_(guard);
            auto it = map.find(desc);
            std::thread thisThread = std::move(it->second);
            map.erase(it);
            thisThread.detach();
        });

        map.emplace(desc, std::move(deviceThread));
    }

    void closeDevice(hid_dev_desc* desc) {
        std::thread thread;
        {
            std::lock_guard<std::mutex> lock(guard);
            auto it = map.find(desc);
            if (it == map.end()) {
                std::printf("device already closed %p\n", desc->device);
                return;
            }
            thread = std::move(it->second);
        }

        thread.detach();
        hid_close_device(desc);
        trace("close device: interrupted \n");
    }

private:
    ThreadMap map;
    std::mutex guard;
};

struct SC_HID_APIManager {
public:
    static SC_HID_APIManager& instance();

    int init();
    int closeAll();

    int build_devicelist();
    int free_devicelist();

    int open_device_path(const char* path, int vendor, int product);
    int open_device(int vendor, int product, char* serial_number = NULL);
    int close_device(int joy_idx);
    void close_all_devices();

    struct hid_dev_desc* get_device(int joy_idx);

    void setPyrObject(PyrObject* obj);

    SC_HID_APIManager();
    ~SC_HID_APIManager();

    int initialize_hidapi();

    void elementData(int, struct hid_device_element*);
    void deviceData(int, struct hid_dev_desc*);
    void deviceRepetitiveReadError(int, struct hid_dev_desc*);

    struct hid_device_info* devinfos;

protected:
    void handleDevice(int, struct hid_dev_desc*, std::atomic<bool> const& shouldBeRunning);
    void handleElement(int, struct hid_device_element*, std::atomic<bool> const& shouldBeRunning);
    void deviceClosed(int, struct hid_dev_desc*, std::atomic<bool> const& shouldBeRunning);

private:
    hid_map_t hiddevices; // declares a vector of hiddevices
    int number_of_hids;

    // language interface
    PyrObject* m_obj;

    std::atomic<bool> m_running;
    std::atomic<bool> mShouldBeRunning;
    SC_HID_API_Threadpool mThreads;
};


static void hid_element_cb(struct hid_device_element* el, void* data) {
    SC_HID_APIManager::instance().elementData(*((int*)data), el);
}

static void hid_descriptor_cb(struct hid_dev_desc* dd, void* data) {
    SC_HID_APIManager::instance().deviceData(*((int*)data), dd);
}

void hid_readerror_cb(hid_dev_desc* dd, void* data) {
    SC_HID_APIManager::instance().deviceRepetitiveReadError(*((int*)data), dd);
}


void SC_HID_APIManager::deviceData(int id, struct hid_dev_desc* dd) { handleDevice(id, dd, mShouldBeRunning); }

void SC_HID_APIManager::elementData(int id, struct hid_device_element* ele) {
    handleElement(id, ele, mShouldBeRunning);
}

void SC_HID_APIManager::deviceRepetitiveReadError(int id, struct hid_dev_desc* dd) {
    deviceClosed(id, dd, mShouldBeRunning);
    // 	hiddevices.erase( id );
}


void SC_HID_APIManager::setPyrObject(PyrObject* obj) { m_obj = obj; }

void SC_HID_APIManager::close_all_devices() {
    for (auto elem : hiddevices)
        mThreads.closeDevice(elem.second);

    hiddevices.clear();
}

int SC_HID_APIManager::open_device_path(const char* path, int vendor, int product) { //
    struct hid_dev_desc* newdevdesc;
    newdevdesc = hid_open_device_path(path, vendor, product);
    if (!newdevdesc) {
        post("HIDAPI : Unable to open device %s: %d, %d\n", path, vendor, product);
        return -1;
    } else {
        hiddevices.insert(std::pair<int, hid_dev_desc*>(number_of_hids, newdevdesc));
        newdevdesc->index = number_of_hids;

        hid_set_descriptor_callback(newdevdesc, (hid_descriptor_callback)hid_descriptor_cb, &newdevdesc->index);
        hid_set_readerror_callback(newdevdesc, (hid_device_readerror_callback)hid_readerror_cb, &newdevdesc->index);
        hid_set_element_callback(newdevdesc, (hid_element_callback)hid_element_cb, &newdevdesc->index);

        number_of_hids++;

        mThreads.openDevice(newdevdesc, mShouldBeRunning);

        return newdevdesc->index;
    }
}

int SC_HID_APIManager::open_device(int vendor, int product, char* serial_number) { //
    struct hid_dev_desc* newdevdesc;
    if (serial_number != NULL) {
        wchar_t* serialW = char_to_wchar(serial_number);
        newdevdesc = hid_open_device(vendor, product, serialW);
        free(serialW);
    } else {
        newdevdesc = hid_open_device(vendor, product, NULL);
    }
    if (!newdevdesc) {
        post("HIDAPI: Unable to open device %d, %d %s\n", vendor, product, serial_number);
        return -1;
    } else {
        hiddevices.insert(std::pair<int, hid_dev_desc*>(number_of_hids, newdevdesc));
        newdevdesc->index = number_of_hids;

        hid_set_descriptor_callback(newdevdesc, (hid_descriptor_callback)hid_descriptor_cb, &newdevdesc->index);
        hid_set_readerror_callback(newdevdesc, (hid_device_readerror_callback)hid_readerror_cb, &newdevdesc->index);
        hid_set_element_callback(newdevdesc, (hid_element_callback)hid_element_cb, &newdevdesc->index);

        number_of_hids++;

        mThreads.openDevice(newdevdesc, mShouldBeRunning);

        return newdevdesc->index;
    }
}

int SC_HID_APIManager::close_device(int joy_idx) {
    struct hid_dev_desc* hidtoclose = get_device(joy_idx);
    if (hidtoclose == NULL) {
        post("HIDAPI: could not find device to close %d\n", joy_idx);
        return 1; // not a fatal error
    } else {
        mThreads.closeDevice(hidtoclose);
        hiddevices.erase(joy_idx);
    }

    return 1;
}

struct hid_dev_desc* SC_HID_APIManager::get_device(int joy_idx) {
    auto iterator = hiddevices.find(joy_idx);
    if (iterator == hiddevices.end()) {
        post("HIDAPI : device was not open %d\n", joy_idx);
        return NULL;
    }

    return hiddevices.find(joy_idx)->second;
}

SC_HID_APIManager& SC_HID_APIManager::instance() {
    static SC_HID_APIManager instance;
    return instance;
}

SC_HID_APIManager::SC_HID_APIManager(): m_running(false) { number_of_hids = 0; }

SC_HID_APIManager::~SC_HID_APIManager() { close_all_devices(); }

int SC_HID_APIManager::init() {
    number_of_hids = 0;
    mShouldBeRunning = true;
    if (!m_running)
        initialize_hidapi();

    return m_running ? errNone : errFailed;
}

int SC_HID_APIManager::closeAll() {
    m_running = false;
    mShouldBeRunning = false;
    close_all_devices();
    return errNone;
}

int SC_HID_APIManager::initialize_hidapi() {
    m_running = false;

    if (hid_init()) {
        post("Unable to initialize hidapi\n");
        return errFailed;
    }

    m_running = true;
    return errNone;
}

int SC_HID_APIManager::build_devicelist() {
    struct hid_device_info* cur_dev;
    devinfos = hid_enumerate(0x0, 0x0);

    cur_dev = devinfos;
    int count = 0;
    while (cur_dev) {
        count++;
        cur_dev = cur_dev->next;
    }
    return count;
}

int SC_HID_APIManager::free_devicelist() {
    hid_free_enumeration(devinfos);
    devinfos = NULL;
    return errNone;
}

void SC_HID_APIManager::deviceClosed(int joy_idx, struct hid_dev_desc* dd, std::atomic<bool> const& shouldBeRunning) {
    int status = lockLanguageOrQuit(shouldBeRunning);
    if (status == EINTR)
        return;
    if (status) {
        trace("error when locking language (%d)\n", status);
        return;
    }
    if (compiledOK) {
        VMGlobals* g = gMainVMGlobals;
        g->canCallOS = false;
        ++g->sp;
        SetObject(g->sp, s_hidapi->u.classobj); // set the class HID_API
        ++g->sp;
        SetInt(g->sp, joy_idx);
        runInterpreter(g, s_hidClosed, 2);
        g->canCallOS = false;
    }
    gLangMutex.unlock();
}

void SC_HID_APIManager::handleElement(int joy_idx, struct hid_device_element* ele,
                                      std::atomic<bool> const& shouldBeRunning) {
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
        SetObject(g->sp, s_hidapi->u.classobj); // set the class HID_API
        ++g->sp;
        SetInt(g->sp, joy_idx);
        ++g->sp;
        SetInt(g->sp, ele->index);
        ++g->sp;
        SetInt(g->sp, ele->usage_page);
        ++g->sp;
        SetInt(g->sp, ele->usage);
        ++g->sp;
        SetInt(g->sp, ele->value);
        ++g->sp;
        SetFloat(g->sp, hid_element_map_logical(ele));
        ++g->sp;
        SetFloat(g->sp, hid_element_map_physical(ele));
        ++g->sp;
        SetInt(g->sp, ele->array_value);
        runInterpreter(g, s_hidElementData, 9);
        g->canCallOS = false;
    }
    gLangMutex.unlock();
}

void SC_HID_APIManager::handleDevice(int joy_idx, struct hid_dev_desc* devd, std::atomic<bool> const& shouldBeRunning) {
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
        SetObject(g->sp, s_hidapi->u.classobj); // set the class HID_API
        ++g->sp;
        SetInt(g->sp, joy_idx);
        ++g->sp;
        SetInt(g->sp, devd->device_collection->num_elements);
        runInterpreter(g, s_hidDeviceData, 3);
        g->canCallOS = false;
    }
    gLangMutex.unlock();
}

// ----------  primitive calls: ---------------

int prHID_API_Initialize(VMGlobals* g, int numArgsPushed) {
    PyrSlot* args = g->sp - numArgsPushed + 1;
    PyrSlot* self = args + 0;

    SC_HID_APIManager::instance().setPyrObject(slotRawObject(self));
    // initialize HID_APIManager
    return SC_HID_APIManager::instance().init();
}

int prHID_API_CloseAll(VMGlobals* g, int numArgsPushed) {
    // close all devices, and cleanup manager
    return SC_HID_APIManager::instance().closeAll();
}

int prHID_API_BuildDeviceList(VMGlobals* g, int numArgsPushed) {
    PyrSlot* args = g->sp - numArgsPushed + 1;
    PyrSlot* self = args + 0;
    // no arguments

    const char emptyString[] = "";

    // iterate over available devices and return info to language to populate the list there
    int result = SC_HID_APIManager::instance().build_devicelist();
    if (result > 0) {
        PyrObject* allDevsArray = newPyrArray(g->gc, result * sizeof(PyrObject), 0, true);
        SetObject(self, allDevsArray);

        struct hid_device_info* cur_dev = SC_HID_APIManager::instance().devinfos;
        while (cur_dev) {
            PyrObject* devInfo = newPyrArray(g->gc, 11 * sizeof(PyrObject), 0, true);
            SetObject(allDevsArray->slots + allDevsArray->size++, devInfo);
            g->gc->GCWriteNew(allDevsArray, devInfo); // we know devInfo is white so we can use GCWriteNew

            SetInt(devInfo->slots + devInfo->size++, cur_dev->vendor_id);
            SetInt(devInfo->slots + devInfo->size++, cur_dev->product_id);

            PyrString* dev_path_name = newPyrString(g->gc, cur_dev->path, 0, true);
            SetObject(devInfo->slots + devInfo->size++, dev_path_name);
            g->gc->GCWriteNew(devInfo, dev_path_name); // we know dev_path_name is white so we can use GCWriteNew

            const char* mystring;
            if (cur_dev->serial_number != NULL)
                mystring = wchar_to_char(cur_dev->serial_number);
            else
                mystring = emptyString;

            PyrString* dev_serial = newPyrString(g->gc, mystring, 0, true);
            SetObject(devInfo->slots + devInfo->size++, dev_serial);
            g->gc->GCWriteNew(devInfo, dev_serial); // we know dev_serial is white so we can use GCWriteNew

            if (mystring != emptyString)
                free((void*)mystring);
            if (cur_dev->manufacturer_string != NULL)
                mystring = wchar_to_char(cur_dev->manufacturer_string);
            else
                mystring = emptyString;

            PyrString* dev_man_name = newPyrString(g->gc, mystring, 0, true);
            SetObject(devInfo->slots + devInfo->size++, dev_man_name);
            g->gc->GCWriteNew(devInfo, dev_man_name); // we know dev_man_name is white so we can use GCWriteNew

            if (mystring != emptyString)
                free((void*)mystring);

            if (cur_dev->product_string != NULL)
                mystring = wchar_to_char(cur_dev->product_string);
            else
                mystring = emptyString;

            PyrString* dev_prod_name = newPyrString(g->gc, mystring, 0, true);
            SetObject(devInfo->slots + devInfo->size++, dev_prod_name);
            g->gc->GCWriteNew(devInfo, dev_prod_name); // we know dev_prod_name is white so we can use GCWriteNew

            if (mystring != emptyString)
                free((void*)mystring);

            SetInt(devInfo->slots + devInfo->size++, cur_dev->release_number);
            SetInt(devInfo->slots + devInfo->size++, cur_dev->interface_number);

            SetInt(devInfo->slots + devInfo->size++, cur_dev->usage_page);
            SetInt(devInfo->slots + devInfo->size++, cur_dev->usage);

            cur_dev = cur_dev->next;
        }

        SC_HID_APIManager::instance().free_devicelist();
    } else {
        // send back info that no devices were found, or empty array
        SetInt(self, 0);
    }
    return errNone;
}

int prHID_API_Open(VMGlobals* g, int numArgsPushed) {
    PyrSlot* args = g->sp - numArgsPushed + 1;
    PyrSlot* self = args + 0;
    PyrSlot* arg1 = args + 1;
    PyrSlot* arg2 = args + 2;
    PyrSlot* arg3 = args + 3;

    int err;
    char path[256];
    int vendorid;
    int productid;
    // 	char serial_number[256]; // could also use serial_number as specification to open device, but this is not
    // working yet

    err = slotIntVal(arg1, &vendorid);
    if (err != errNone)
        return err;

    err = slotIntVal(arg2, &productid);
    if (err != errNone)
        return err;

    int result;

    err = slotStrVal(arg3, path, sizeof(path));
    if (err)
        return err;
    result = SC_HID_APIManager::instance().open_device_path(path, vendorid, productid);

    /*
    // could also use serial_number as specification to open device, but this is not working yet
  if ( NotNil( arg3 ) ){
    err = slotStrVal(arg3, serial_number, sizeof(serial_number));
    if (err) return err;
    // open device
    result = SC_HID_APIManager::instance().open_device( vendorid, productid, serial_number );
  } else {
      // open device
    result = SC_HID_APIManager::instance().open_device( vendorid, productid, NULL );
  }
*/
    SetInt(self, result);

    return errNone;
}

int prHID_API_Close(VMGlobals* g, int numArgsPushed) {
    PyrSlot* args = g->sp - numArgsPushed + 1;
    PyrSlot* self = args + 0;
    PyrSlot* arg = args + 1;

    int err;
    int joyid;

    err = slotIntVal(arg, &joyid);
    if (err != errNone)
        return err;

    int result = SC_HID_APIManager::instance().close_device(joyid);

    SetInt(self, result);

    return errNone;
}

int prHID_API_GetInfo(VMGlobals* g, int numArgsPushed) {
    PyrSlot* args = g->sp - numArgsPushed + 1;
    PyrSlot* self = args + 0;
    PyrSlot* arg = args + 1;

    int err;
    int joyid;

    err = slotIntVal(arg, &joyid);
    if (err != errNone)
        return err;
    const char emptyString[] = "";

    struct hid_dev_desc* devdesc = SC_HID_APIManager::instance().get_device(joyid);
    struct hid_device_info* cur_dev = devdesc->info;

    if (cur_dev != NULL) {
        PyrObject* devInfo = newPyrArray(g->gc, 9 * sizeof(PyrObject), 0, true);
        SetObject(self, devInfo);

        SetInt(devInfo->slots + devInfo->size++, cur_dev->vendor_id);
        SetInt(devInfo->slots + devInfo->size++, cur_dev->product_id);

        PyrString* dev_path_name = newPyrString(g->gc, cur_dev->path, 0, true);
        SetObject(devInfo->slots + devInfo->size++, dev_path_name);
        g->gc->GCWriteNew(devInfo, dev_path_name); // we know dev_path_name is white so we can use GCWriteNew

        const char* mystring;
        if (cur_dev->serial_number != NULL) {
            mystring = wchar_to_char(cur_dev->serial_number);
        } else {
            mystring = emptyString;
        }

        PyrString* dev_serial = newPyrString(g->gc, mystring, 0, true);
        SetObject(devInfo->slots + devInfo->size++, dev_serial);
        g->gc->GCWriteNew(devInfo, dev_serial); // we know dev_serial is white so we can use GCWriteNew

        if (mystring != emptyString)
            free((void*)mystring);

        if (cur_dev->manufacturer_string != NULL) {
            mystring = wchar_to_char(cur_dev->manufacturer_string);
        } else {
            mystring = emptyString;
        }
        PyrString* dev_man_name = newPyrString(g->gc, mystring, 0, true);
        SetObject(devInfo->slots + devInfo->size++, dev_man_name);
        g->gc->GCWriteNew(devInfo, dev_man_name); // we know dev_man_name is white so we can use GCWriteNew
        if (mystring != emptyString)
            free((void*)mystring);


        if (cur_dev->product_string != NULL) {
            mystring = wchar_to_char(cur_dev->product_string);
        } else {
            mystring = emptyString;
        }
        PyrString* dev_prod_name = newPyrString(g->gc, mystring, 0, true);
        SetObject(devInfo->slots + devInfo->size++, dev_prod_name);
        g->gc->GCWriteNew(devInfo, dev_prod_name); // we know dev_prod_name is white so we can use GCWriteNew
        if (mystring != emptyString)
            free((void*)mystring);

        SetInt(devInfo->slots + devInfo->size++, cur_dev->release_number);
        SetInt(devInfo->slots + devInfo->size++, cur_dev->interface_number);
    } else {
        SetInt(self, 0);
    }
    return errNone;
}

int prHID_API_GetNumberOfCollections(VMGlobals* g, int numArgsPushed) {
    PyrSlot* args = g->sp - numArgsPushed + 1;
    PyrSlot* self = args + 0;
    PyrSlot* arg = args + 1;

    int err;
    int joyid;

    err = slotIntVal(arg, &joyid);
    if (err != errNone)
        return err;

    struct hid_dev_desc* devdesc = SC_HID_APIManager::instance().get_device(joyid);
    struct hid_device_collection* cur_dev = devdesc->device_collection;

    if (cur_dev != NULL) {
        SetInt(self, cur_dev->num_collections);
    } else {
        SetInt(self, 0);
    }
    return errNone;
}

int prHID_API_GetCollectionInfo(VMGlobals* g, int numArgsPushed) {
    PyrSlot* args = g->sp - numArgsPushed + 1;
    PyrSlot* self = args + 0;
    PyrSlot* arg1 = args + 1;
    PyrSlot* arg2 = args + 2;

    int err;
    int joyid;
    int collectionid;

    err = slotIntVal(arg1, &joyid);
    if (err != errNone)
        return err;

    err = slotIntVal(arg2, &collectionid);
    if (err != errNone)
        return err;

    struct hid_dev_desc* devdesc = SC_HID_APIManager::instance().get_device(joyid);
    struct hid_device_collection* curdev = devdesc->device_collection;
    struct hid_device_collection* curcollection = curdev->first_collection;
    struct hid_device_collection* thiscollection = NULL;

    bool found = curcollection->index == collectionid;
    if (found) {
        thiscollection = curcollection;
    }
    while (curcollection != NULL && !found) {
        found = curcollection->index == collectionid;
        if (found) {
            thiscollection = curcollection;
        }
        curcollection = curcollection->next_collection;
    }

    if (thiscollection != NULL) {
        PyrObject* elInfo = newPyrArray(g->gc, 9 * sizeof(PyrObject), 0, true);
        SetObject(self, elInfo);

        SetInt(elInfo->slots + elInfo->size++, thiscollection->index);
        SetInt(elInfo->slots + elInfo->size++, thiscollection->type);
        SetInt(elInfo->slots + elInfo->size++, thiscollection->usage_page);
        SetInt(elInfo->slots + elInfo->size++, thiscollection->usage_index);

        if (thiscollection->parent_collection != NULL) {
            SetInt(elInfo->slots + elInfo->size++, thiscollection->parent_collection->index);
        } else {
            SetInt(elInfo->slots + elInfo->size++, -2);
        }

        SetInt(elInfo->slots + elInfo->size++, thiscollection->num_collections);

        if (thiscollection->first_collection != NULL) {
            SetInt(elInfo->slots + elInfo->size++, thiscollection->first_collection->index);
        } else {
            SetInt(elInfo->slots + elInfo->size++, -1);
        }

        SetInt(elInfo->slots + elInfo->size++, thiscollection->num_elements);

        if (thiscollection->first_element != NULL) {
            SetInt(elInfo->slots + elInfo->size++, thiscollection->first_element->index);
        } else {
            SetInt(elInfo->slots + elInfo->size++, -1);
        }

    } else {
        SetInt(self, 0);
    }
    return errNone;
}

int prHID_API_GetNumberOfElements(VMGlobals* g, int numArgsPushed) {
    PyrSlot* args = g->sp - numArgsPushed + 1;
    PyrSlot* self = args + 0;
    PyrSlot* arg = args + 1;

    int err;
    int joyid;

    err = slotIntVal(arg, &joyid);
    if (err != errNone)
        return err;

    struct hid_dev_desc* devdesc = SC_HID_APIManager::instance().get_device(joyid);
    struct hid_device_collection* cur_dev = devdesc->device_collection;

    if (cur_dev != NULL) {
        SetInt(self, cur_dev->num_elements);
    } else {
        SetInt(self, 0);
    }
    return errNone;
}

int prHID_API_GetElementInfo(VMGlobals* g, int numArgsPushed) {
    PyrSlot* args = g->sp - numArgsPushed + 1;
    PyrSlot* self = args + 0;
    PyrSlot* arg1 = args + 1;
    PyrSlot* arg2 = args + 2;

    int err;
    int joyid;
    int elementid;

    err = slotIntVal(arg1, &joyid);
    if (err != errNone)
        return err;

    err = slotIntVal(arg2, &elementid);
    if (err != errNone)
        return err;

    struct hid_dev_desc* devdesc = SC_HID_APIManager::instance().get_device(joyid);
    struct hid_device_collection* curdev = devdesc->device_collection;
    struct hid_device_element* curelement = curdev->first_element;
    struct hid_device_element* thiselement = NULL;

    bool found = curelement->index == elementid;
    if (found) {
        thiselement = curelement;
    }
    while (curelement != NULL && !found) {
        found = curelement->index == elementid;
        if (found) {
            thiselement = curelement;
        }
        curelement = curelement->next;
    }

    if (thiselement != NULL) {
        PyrObject* elInfo = newPyrArray(g->gc, 18 * sizeof(PyrObject), 0, true);
        SetObject(self, elInfo);

        SetInt(elInfo->slots + elInfo->size++, thiselement->index);
        SetInt(elInfo->slots + elInfo->size++, thiselement->io_type);
        SetInt(elInfo->slots + elInfo->size++, thiselement->type);
        SetInt(elInfo->slots + elInfo->size++, thiselement->usage_page);
        SetInt(elInfo->slots + elInfo->size++, thiselement->usage);

        SetInt(elInfo->slots + elInfo->size++, thiselement->usage_min);
        SetInt(elInfo->slots + elInfo->size++, thiselement->usage_max);

        SetInt(elInfo->slots + elInfo->size++, thiselement->logical_min);
        SetInt(elInfo->slots + elInfo->size++, thiselement->logical_max);
        SetInt(elInfo->slots + elInfo->size++, thiselement->phys_min);
        SetInt(elInfo->slots + elInfo->size++, thiselement->phys_max);
        SetInt(elInfo->slots + elInfo->size++, thiselement->unit_exponent);

        SetInt(elInfo->slots + elInfo->size++, thiselement->unit);
        SetInt(elInfo->slots + elInfo->size++, thiselement->report_size);
        SetInt(elInfo->slots + elInfo->size++, thiselement->report_id);
        SetInt(elInfo->slots + elInfo->size++, thiselement->report_index);
        SetInt(elInfo->slots + elInfo->size++, thiselement->value);

        SetInt(elInfo->slots + elInfo->size++, thiselement->parent_collection->index);

    } else {
        SetInt(self, 0);
    }
    return errNone;
}

int prHID_API_SetElementOutput(VMGlobals* g, int numArgsPushed) {
    PyrSlot* args = g->sp - numArgsPushed + 1;
    PyrSlot* joyIdSlot = args + 1;
    PyrSlot* elementIdSlot = args + 2;
    PyrSlot* valueSlot = args + 3;

    int err;
    int joyid;
    int elementid;
    int value;

    err = slotIntVal(joyIdSlot, &joyid);
    if (err != errNone)
        return err;

    err = slotIntVal(elementIdSlot, &elementid);
    if (err != errNone)
        return err;

    err = slotIntVal(valueSlot, &value);
    if (err != errNone)
        return err;

    struct hid_dev_desc* devdesc = SC_HID_APIManager::instance().get_device(joyid);
    struct hid_device_collection* curdev = devdesc->device_collection;
    struct hid_device_element* curelement = curdev->first_element;
    struct hid_device_element* thiselement = NULL;

    if (devdesc != NULL) {
        bool found = false;
        while (curelement != NULL && !found) {
            found = (curelement->index == elementid) && (curelement->io_type == 2);
            if (found) {
                thiselement = curelement;
            }
            curelement = hid_get_next_output_element(curelement);
        }
        if (thiselement != NULL) {
            thiselement->value = value;
            hid_send_output_report(devdesc, thiselement->report_id);
        }
    }
    return errNone;
}

int prHID_API_SetElementRepeat(VMGlobals* g, int numArgsPushed) {
    PyrSlot* args = g->sp - numArgsPushed + 1;
    PyrSlot* joyIdSlot = args + 1;
    PyrSlot* elementIdSlot = args + 2;
    PyrSlot* valueSlot = args + 3;

    int err;
    int joyid;
    int elementid;
    int value;

    err = slotIntVal(joyIdSlot, &joyid);
    if (err != errNone)
        return err;

    err = slotIntVal(elementIdSlot, &elementid);
    if (err != errNone)
        return err;

    err = slotIntVal(valueSlot, &value);
    if (err != errNone)
        return err;

    struct hid_dev_desc* devdesc = SC_HID_APIManager::instance().get_device(joyid);
    struct hid_device_collection* curdev = devdesc->device_collection;
    struct hid_device_element* curelement = curdev->first_element;
    struct hid_device_element* thiselement = NULL;

    if (devdesc != NULL) {
        bool found = false;
        while (curelement != NULL && !found) {
            found = (curelement->index == elementid) && (curelement->io_type == 1);
            if (found) {
                thiselement = curelement;
            }
            curelement = hid_get_next_input_element(curelement);
        }
        if (thiselement != NULL) {
            thiselement->repeat = value;
        }
    }
    return errNone;
}

void close_HID_API_Devices() { SC_HID_APIManager::instance().closeAll(); }

void initHIDAPIPrimitives() {
    int base, index;

    close_HID_API_Devices();

    s_hidapi = getsym("HID");

    base = nextPrimitiveIndex();
    index = 0;

    definePrimitive(base, index++, "_HID_API_Initialize", prHID_API_Initialize, 1,
                    0); // this initializes the hid subsystem
    definePrimitive(base, index++, "_HID_API_CloseAll", prHID_API_CloseAll, 1,
                    0); // this also cleans up and closes devices

    definePrimitive(base, index++, "_HID_API_BuildDeviceList", prHID_API_BuildDeviceList, 1,
                    0); // this gets device info about the various devices that are attached

    definePrimitive(base, index++, "_HID_API_OpenDevice", prHID_API_Open, 3, 0); // opens a specific device
    definePrimitive(base, index++, "_HID_API_CloseDevice", prHID_API_Close, 2, 0); // closes a specific device

    definePrimitive(base, index++, "_HID_API_GetInfo", prHID_API_GetInfo, 2, 0); // gets info about a specific device
    definePrimitive(base, index++, "_HID_API_GetNumberOfCollections", prHID_API_GetNumberOfCollections, 2,
                    0); // gets number of elements of a device
    definePrimitive(base, index++, "_HID_API_GetCollectionInfo", prHID_API_GetCollectionInfo, 3,
                    0); // gets info about a specific device element
    definePrimitive(base, index++, "_HID_API_GetNumberOfElements", prHID_API_GetNumberOfElements, 2,
                    0); // gets number of elements of a device
    definePrimitive(base, index++, "_HID_API_GetElementInfo", prHID_API_GetElementInfo, 4,
                    0); // gets info about a specific device element

    definePrimitive(base, index++, "_HID_API_SetElementOutput", prHID_API_SetElementOutput, 3,
                    0); // sets the output value of a specific device element, and sends the report
    definePrimitive(base, index++, "_HID_API_SetElementRepeat", prHID_API_SetElementRepeat, 3,
                    0); // sets the repeat property of a specific device element

    s_hidElementData = getsym("prHIDElementData"); // send back element data
    s_hidDeviceData = getsym("prHIDDeviceData"); // send back device data
    s_hidClosed = getsym("prHIDDeviceClosed"); // send back that device was closed
}

void deinitHIDAPIPrimitives() { SC_HID_APIManager::instance().closeAll(); }

#else // no HID API

void initHIDAPIPrimitives() {
    // other platforms?
}

void deinitHIDAPIPrimitives() {}

#endif /// HAVE_API_HID


// EOF
