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
SCErr NewCommand(const char *inPath, uint32 inCommandNumber, SC_CommandFunc inFunc);

// command numbers:
enum {
	cmd_none = 0,

	cmd_notify = 1,
	cmd_status = 2,
	cmd_quit = 3,
	cmd_cmd = 4,

	cmd_d_recv = 5,
	cmd_d_load = 6,
	cmd_d_loadDir = 7,
	cmd_d_freeAll = 8,

	cmd_s_new = 9,
	
	cmd_n_trace = 10,
	cmd_n_free = 11,
	cmd_n_run = 12,
	cmd_n_cmd = 13,
	cmd_n_map = 14,
	cmd_n_set = 15,
	cmd_n_setn = 16,
	cmd_n_fill = 17,
	cmd_n_before = 18,
	cmd_n_after = 19,

	cmd_u_cmd = 20,

	cmd_g_new = 21,
	cmd_g_head = 22,
	cmd_g_tail = 23,
	cmd_g_freeAll = 24,
	
	cmd_c_set = 25,
	cmd_c_setn = 26,
	cmd_c_fill = 27,

	cmd_b_alloc = 28,
	cmd_b_allocRead = 29,
	cmd_b_read = 30,
	cmd_b_write = 31,
	cmd_b_free = 32,
	cmd_b_close = 33,
	cmd_b_zero = 34,
	cmd_b_set = 35,
	cmd_b_setn = 36,
	cmd_b_fill = 37,
	cmd_b_gen = 38,
	
	cmd_dumpOSC = 39,

	cmd_c_get = 40,
	cmd_c_getn = 41,
	cmd_b_get = 42,
	cmd_b_getn = 43,
	cmd_s_get = 44,
	cmd_s_getn = 45,
	
	cmd_n_query = 46,
	cmd_b_query = 47,

	cmd_n_mapn = 48,
	cmd_s_noid = 49,

	cmd_g_deepFree = 50,
	cmd_clearSched = 51,

	cmd_sync = 52,

	NUMBER_OF_COMMANDS = 53
};

extern SC_LibCmd* gCmdArray[NUMBER_OF_COMMANDS];


#endif

