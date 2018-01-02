/*
 NewPyrObjectPtr.cpp
 SuperCollider
 
 Created by Scott Wilson on 02/01/2018.
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

#include <stdio.h>
#include "NewPyrObjectPtr.h"
#include "GC.h"

// eventually this increment should be in the GC itself probably to prevent circumvention
// but for now this allows backwards compatibility
void NewPyrObjectPtr::incrementGC()
{
	mGC->incrementUnreachableCount();
}

void NewPyrObjectPtr::decrementGC()
{
	mGC->decrementUnreachableCount();
}

// a NewPyrObjectPtr is either white or released and useless
// so we can assume here that this is safe.
PyrObjectHdr* NewPyrObjectPtr::releaseAndWriteNew(PyrObject* parent)
{
	PyrObjectHdr *released = this->release();
	mGC->GCWriteNew((PyrObjectHdr*)parent, released);
	return released;
}

NewPyrObjectPtr::~NewPyrObjectPtr()
{
	// don't delete pyrObj just check that it's been used
	// We cannot throw from the destructor, so just post an error and terminate
	// sclang is basically burnt toast at this stage anyway, or will be soon.
	if(!mMadeReachable) {
		PyrMethod *meth = gMainVMGlobals->primitiveMethod;
		char *lastPrimitiveClass, *lastPrimitiveMethod;
		lastPrimitiveClass = slotRawSymbol(&slotRawClass(&meth->ownerclass)->name)->name;
		lastPrimitiveMethod = slotRawSymbol(&meth->name)->name;
		
		printf("ERROR: Unused PyrObject created in primitive called from %s:%s. Terminating...\n", lastPrimitiveClass, lastPrimitiveMethod);
		std::terminate();
	}
}
