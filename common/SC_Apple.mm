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

#import <Cocoa/Cocoa.h>

namespace SC {
namespace Apple {

void disableAppNap ( )
{
	// Does the App Nap API even exist on this Mac?
	if ([[NSProcessInfo processInfo] respondsToSelector:@selector(beginActivityWithOptions:reason:)]) {
		// If the API exists, then disable App Nap...
		
		// From NSProcessInfo.h:
		// NSActivityIdleSystemSleepDisabled = (1ULL << 20),
		// NSActivityUserInitiated = (0x00FFFFFFULL | NSActivityIdleSystemSleepDisabled),
		// NSActivityLatencyCritical = 0xFF00000000ULL
		
		uint64_t options = (0x00FFFFFFULL | (1ULL << 20)) | 0xFF00000000ULL;

		// NSActivityLatencyCritical | NSActivityUserInitiated
	    [[NSProcessInfo processInfo] beginActivityWithOptions: options reason:@"avoiding audio hiccups and reducing latency"];
	}	
}

} // namespace Apple
} // namespace SC
