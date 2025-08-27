#include "SC_ServerBootDelayWarning.h"

#ifdef _WIN32
#    include <cassert>
#    include <iostream>
#    include <chrono>

ServerBootDelayWarningTimer::~ServerBootDelayWarningTimer() {
    if (mThread.joinable()) {
        stop();
    }
}

void ServerBootDelayWarningTimer::start() {
    mThread = std::thread([this]() {
        // display warning if server has not booted in time
        std::unique_lock<std::mutex> lock(mMutex);

        if (!mCondVar.wait_for(lock, std::chrono::seconds(10), [this] { return mIsServerBooted; })) {
            std::cout << "Server: possible boot delay." << std::endl;
            std::cout << "On some Windows-based machines, Windows Defender sometimes ";
            std::cout << "delays server boot by one minute." << std::endl;
            std::cout << "You can add scsynth.exe and/or supernova.exe *processes* to ";
            std::cout << "Windows Defender exclusion list to avoid this delay. It's safe." << std::endl;
        }
    });
}

void ServerBootDelayWarningTimer::stop() {
    assert(mThread.joinable());
    {
        std::lock_guard<std::mutex> lock(mMutex);
        mIsServerBooted = true;
    }
    mCondVar.notify_all();
    mThread.join();
}

#endif //_WIN32
