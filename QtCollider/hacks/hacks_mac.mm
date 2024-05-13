/************************************************************************
 *
 * Copyright 2011-2012 Jakob Leben (jakob.leben@gmail.com)
 *
 * This file is part of SuperCollider Qt GUI.
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

#include "hacks_mac.hpp"

#import <Cocoa/Cocoa.h>

namespace QtCollider { namespace Mac {

bool isKeyWindow(QWidget* w) { return [[reinterpret_cast<NSView*>(w->winId()) window] isKeyWindow]; }

bool IsCmdPeriodKeyDown(void* event) {
    NSEvent* nsevent = reinterpret_cast<NSEvent*>(event);
    if ([nsevent type] == NSKeyDown) {
        if ([nsevent modifierFlags] & NSCommandKeyMask) {
            NSString* chars = [nsevent charactersIgnoringModifiers];
            return [chars characterAtIndex:0] == 0x2E; // Unicode encoding for period
        }
    }
    return false;
}

bool IsCmdPeriodKeyUp(void* event) {
    NSEvent* nsevent = reinterpret_cast<NSEvent*>(event);
    if ([nsevent type] == NSKeyUp) {
        if ([nsevent modifierFlags] & NSCommandKeyMask) {
            NSString* chars = [nsevent charactersIgnoringModifiers];
            return [chars characterAtIndex:0] == 0x2E;
        }
    }
    return false;
}

bool AlwaysShowScrollbars() { return ([NSScroller preferredScrollerStyle] == NSScrollerStyleLegacy); }

void DisableAutomaticWindowTabbing() {
    if ([NSWindow respondsToSelector:@selector(allowsAutomaticWindowTabbing)]) {
        [NSWindow setAllowsAutomaticWindowTabbing:NO];
    }
}

void activateApp() { [NSApp activateIgnoringOtherApps:YES]; }

} // namespace Mac
} // namespace QtCollider
