#include "SC_ServerBootDelayWarning.h"

#ifdef _WIN32
#    include <iostream>
#    include <thread>
#    include <mutex>
#    include <chrono>
#    include <condition_variable>

static bool g_isServerBooted = false;
static std::mutex g_serverBootWarningMutex;
static std::condition_variable g_serverBootWarningConditionVariable;
static std::thread g_bootWarningThread;

static void displayWarningIfServerHasNotBooted() {
    std::unique_lock<std::mutex> lock(g_serverBootWarningMutex);

    if (!g_serverBootWarningConditionVariable.wait_for(lock, std::chrono::seconds(10),
                                                       [] { return g_isServerBooted; })) {
        std::cout << "Server: possible boot delay." << std::endl;
        std::cout << "On some Windows-based machines, Windows Defender sometimes ";
        std::cout << "delays server boot by one minute." << std::endl;
        std::cout << "You can add scsynth.exe and/or supernova.exe *processes* to ";
        std::cout << "Windows Defender exclusion list to avoid this delay. It's safe." << std::endl;
    }
}
#endif //_WIN32

void startServerBootDelayWarningTimer() {
#ifdef _WIN32
    g_bootWarningThread = std::thread(displayWarningIfServerHasNotBooted);
#endif //_WIN32
}

void stopServerBootDelayWarningTimer() {
#ifdef _WIN32
    {
        const std::lock_guard<std::mutex> lock(g_serverBootWarningMutex);
        g_isServerBooted = true;
    }
    g_serverBootWarningConditionVariable.notify_all();
    g_bootWarningThread.join();
#endif //_WIN32
}
