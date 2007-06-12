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
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/


#ifndef _SC_Lib_Cintf_
#define _SC_Lib_Cintf_

#include "SC_Lib.h"
#include "SC_Reply.h"

typedef SCErr (*SC_CommandFunc)(struct World *inWorld, int inSize, char *inData, ReplyAddress *inReply);

class SC_LibCmd : public SC_NamedObj
{
public:
	SC_LibCmd(SC_CommandFunc inFunc);

	SCErr Perform(struct World *inWorld, int inSize, char *inData, ReplyAddress *inReply);

private:
	SC_CommandFunc mFunc;
};

extern Malloc gMalloc;
extern HashTable<class SC_LibCmd, Malloc> *gCmdLib;

void initialize_library();
SCErr NewCommand(const char *inPath, SC_CommandFunc inFunc);

#endif

