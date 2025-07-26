// Add a warning when Windows Defender delays scsynth boot by 60+ seconds
// cf. github issue #4368

#pragma once

#ifdef _WIN32
#    include <condition_variable>
#    include <thread>
#    include <mutex>

class ServerBootDelayWarningTimer {
public:
    // Automatically stops the timer thread if necessary.
    ~ServerBootDelayWarningTimer();

    // Starts a timer that will display a warning message after
    // 10 seconds if the server has not booted, which may be due
    // to Windows Defender.
    // Must be called at the start of the server boot sequence.
    void start();
    // Call this at the end of the server boot sequence.
    void stop();

private:
    std::mutex mMutex;
    std::condition_variable mCondVar;
    std::thread mThread;
    bool mIsServerBooted = false;
};

#else

// Does nothing on non-Windows platforms
class ServerBootDelayWarningTimer {
public:
    void start() {}
    void stop() {}
};

#endif // _WIN32
