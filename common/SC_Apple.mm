/************************************************************************
 *
 * Copyright 2013 Seth Nickell <snickell@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 ************************************************************************/

#include "SC_Apple.hpp"
#include <atomic>

#import <Cocoa/Cocoa.h>

namespace SC { namespace Apple {

void disableAppNap() {
    // Does the App Nap API even exist on this Mac?
    if ([[NSProcessInfo processInfo] respondsToSelector:@selector(beginActivityWithOptions:reason:)]) {
        // If the API exists, then disable App Nap...

        // From NSProcessInfo.h:
        // NSActivityIdleSystemSleepDisabled = (1ULL << 20),
        // NSActivityUserInitiated = (0x00FFFFFFULL | NSActivityIdleSystemSleepDisabled),
        // NSActivityLatencyCritical = 0xFF00000000ULL

        uint64_t options = (0x00FFFFFFULL | (1ULL << 20)) | 0xFF00000000ULL;

        // NSActivityLatencyCritical | NSActivityUserInitiated
        [[NSProcessInfo processInfo] beginActivityWithOptions:options
                                                       reason:@"avoiding audio hiccups and reducing latency"];
    }
}

namespace EventLoop {

static std::atomic_bool g_running;

void setup() {
    // The following code would transform the process into a foreground application.
    // For now it's the plugin's responsibility to do this (early or lazily)
    // because we don't want to always show an icon in the docker.
    // ProcessSerialNumber psn = { 0, kCurrentProcess };
    // TransformProcessType(&psn, kProcessTransformToForegroundApplication);

    // Create NSApplication
    [NSApplication sharedApplication];
}

void run() {
#if 0
    // this doesn't work...
    [NSApp run];
#else
    // Kudos to https://www.cocoawithlove.com/2009/01/demystifying-nsapplication-by.html
    NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];

    [NSApp finishLaunching];
    g_running = true;

    while (g_running) {
        [pool release];
        pool = [[NSAutoreleasePool alloc] init];
        NSEvent* event = [NSApp nextEventMatchingMask:NSAnyEventMask
                                            untilDate:[NSDate distantFuture]
                                               inMode:NSDefaultRunLoopMode
                                              dequeue:YES];
        if (event) {
            [NSApp sendEvent:event];
            [NSApp updateWindows];
        }
    }
    [pool release];
#endif
}

void quit() {
    // break from event loop instead of [NSApp terminate:nil]
    g_running = false;
    // send dummy event to wake up event loop
    NSEvent* event = [NSEvent otherEventWithType:NSApplicationDefined
                                        location:NSMakePoint(0, 0)
                                   modifierFlags:0
                                       timestamp:0
                                    windowNumber:0
                                         context:nil
                                         subtype:0
                                           data1:0
                                           data2:0];
    [NSApp postEvent:event atStart:NO];
}

} // EventLoop

} // namespace Apple
} // namespace SC
