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


#ifndef _SC_Lib_
#define _SC_Lib_

#include "SC_Errors.h"
#include "SC_Lock.h"
#include "SC_Types.h"
#include "Hash.h"
#include "HashTable.h"
#include <stdlib.h>
#include <string.h>

class SC_NamedObj
{
public:
	SC_NamedObj();
	virtual ~SC_NamedObj();

	const int32* Name() const { return mName; }
	void SetName(const char *inName);
	void SetName(const int32 *inName);

private:
	friend int32 GetHash(const SC_NamedObj *inObj);
	friend const int32* GetKey(const SC_NamedObj *inObj);

	int32 mName[kSCNameLen];
	int32 mHash;
};

inline int32 GetHash(const SC_NamedObj *inObj)
{
	return inObj->mHash;
}

inline const int32 *GetKey(const SC_NamedObj *inObj)
{
	return inObj->mName;
}

#endif

