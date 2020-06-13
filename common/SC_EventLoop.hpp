#pragma once

#include <functional>
#ifdef __APPLE__
#    include "SC_AppleEventLoop.hpp"
#    include <thread>
#endif

// For now, the event loop is only implemented for macOS where
// it is most needed (because the GUI event loop *must* run on the main thread).
// Later a Windows and Linux implementation might follow, but this would
// require a plugin API method to retrieve a handle to the GUI thread.
// (Windows and Linux don't have any notion of a "main thread" and therefore
// lack a method like "dispatch_get_main_queue".)

class EventLoop {
public:
    // This function must be called at the very beginning of the main thread!
    static void setup() {
#ifdef __APPLE__
        SC::Apple::EventLoop::setup();
#endif
    }

    // Run the event loop until 'waitFunction' returns.
    static void run(std::function<void()> waitFunction) {
#ifdef __APPLE__
        // this thread simply waits for 'waitFunction' to return,
        // after which it will ask the event loop to terminate.
        auto thread = std::thread([waitFunction]() {
            waitFunction();
            SC::Apple::EventLoop::quit();
        });
        thread.detach();

        SC::Apple::EventLoop::run();
#else
        waitFunction();
#endif
    }
};
