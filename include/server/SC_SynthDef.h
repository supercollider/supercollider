/*
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

#ifndef _SynthDef_
#define _SynthDef_

#include "SC_Types.h"
#include "sc_msg_iter.h"

typedef void (*NodeDtorFunc)(struct Node* inNode);

// Node definition names can be 255 characters long.
const unsigned int kSCNodeDefNameLen = 64;
const unsigned int kSCNodeDefNameByteLen = 64 * sizeof(int32);

struct NodeDef
{
	int32 mName[kSCNodeDefNameLen];
	int32 mHash;

	size_t mAllocSize;
};
typedef struct NodeDef NodeDef;

extern NodeDef gGroupNodeDef;

void GroupNodeDef_Init();
void NodeDef_Dump(NodeDef *inNodeDef);

#endif
