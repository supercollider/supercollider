/*
	SuperColliderAU Copyright (c) 2006 Gerard Roma.

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

#ifndef _OSCMessages_
#define _OSCMessages_
#include "scsynthsend.h"
#include <CoreFoundation/CFString.h>

static int kDefaultNodeId = 1000;

class OSCMessages   {
	public:
			OSCMessages();
			char* initTree;
			char* synthDef;
			size_t parameterMessage(small_scpacket *packet,CFStringRef name, float value);
            size_t createSynthMessage(small_scpacket *packet, CFStringRef name);
            small_scpacket sendTickMessage(int64 oscTime, int bus);
			small_scpacket initTreeMessage();
            small_scpacket quitMessage();
			small_scpacket noteMessage(int64 oscTime, int note, int velocity);
};
#endif