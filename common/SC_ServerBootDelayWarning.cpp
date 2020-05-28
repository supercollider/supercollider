// Add a warning when Windows Defender delays scsynth boot by 60+ seconds
// cf. github issue #4368
#include "SC_ServerBootDelayWarning.h"

#ifdef _WIN32
#    include <iostream>
#    include <thread>
#    include <mutex>
#    include <chrono>
#    include <condition_variable>

static bool g_isServerBooted = false;
static std::mutex g_scsynthBootWarningMutex;
static std::condition_variable g_scsynthBootWarningConditionVariable;
static std::thread* g_bootWarningThread;

static void displayWarningIfServerHasNotBooted() {
    std::unique_lock<std::mutex> lock(g_scsynthBootWarningMutex);

    if (!g_scsynthBootWarningConditionVariable.wait_for(lock, std::chrono::seconds(10),
                                                        [] { return g_isServerBooted; })) {
        std::cout << "Server: possible boot delay.\n";
        std::cout << "On some Windows-based machines, Windows Defender sometimes delays server boot by one minute.\n";
        std::cout << "You can add scsynth.exe and/or supernova.exe *processes* to Windows Defender exclusion list ";
        std::cout << "to disable this check. It's safe.\n";
    }
}
#endif //_WIN32

void startServerBootDelayWarningTimer() {
#ifdef _WIN32
    g_bootWarningThread = new std::thread(displayWarningIfServerHasNotBooted);
#endif //_WIN32
}

void stopServerBootDelayWarningTimer() {
#ifdef _WIN32
    {
        const std::lock_guard<std::mutex> lock(g_scsynthBootWarningMutex);
        g_isServerBooted = true;
    }
    g_scsynthBootWarningConditionVariable.notify_all();
    g_bootWarningThread->join();
    delete g_bootWarningThread;
#endif //_WIN32
}
