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


#include "SC_UnitDef.h"
#include "SC_World.h"
#include "SC_Unit.h"
#include "SC_InterfaceTable.h"
#include "SC_Prototypes.h"
#include "SC_Str4.h"
#include <stdlib.h>

bool UnitDef_Create(char *inName, size_t inAllocSize, UnitCtorFunc inCtor, UnitDtorFunc inDtor)
{
	if (strlen(inName) >= kSCNameByteLen) return false;

	UnitDef *unitDef = (UnitDef*)malloc(sizeof(UnitDef));
	if (!unitDef) return false;

	str4cpy(unitDef->mUnitDefName, inName);
	unitDef->mHash = Hash(unitDef->mUnitDefName);
	
	//printf("UnitDef_Create %s %08X %08X\n", inName, inCtor, 0);
	
	unitDef->mAllocSize = inAllocSize;
	unitDef->mUnitCtorFunc = inCtor;
	unitDef->mUnitDtorFunc = inDtor;
	unitDef->mCmds = 0;
	
	if (!AddUnitDef(unitDef)) {
		free(unitDef);
		return false;
	}
	return true;
}


#include "SC_Lib_Cintf.h"

bool UnitDef_AddCmd(char *inUnitDefName, char *inCmdName, UnitCmdFunc inFunc)
{
	if (strlen(inUnitDefName) >= kSCNameByteLen) return false;
	int32 unitDefName[kSCNameLen];
	memset(unitDefName, 0, kSCNameByteLen);
	strcpy((char*)unitDefName, inUnitDefName);
	
	if (strlen(inCmdName) >= kSCNameByteLen) return false;
	
	UnitDef* unitDef = GetUnitDef(unitDefName);
	if (!unitDef) return false;
	
	if (!unitDef->mCmds) {
		unitDef->mCmds = new HashTable<UnitCmd, Malloc>(&gMalloc, 4, true);
	}
	
	UnitCmd *cmd = new UnitCmd();
	memset(cmd->mCmdName, 0, kSCNameByteLen);
	strcpy((char*)cmd->mCmdName, inCmdName);

	cmd->mFunc = inFunc;
	cmd->mHash = Hash(cmd->mCmdName);
	unitDef->mCmds->Add(cmd);
		
	return true;
}

bool PlugIn_DefineCmd(char *inCmdName, PlugInCmdFunc inFunc, void *inUserData)
{
	
	if (strlen(inCmdName) >= kSCNameByteLen) return false;
	
	PlugInCmd *cmd = new PlugInCmd();
	memset(cmd->mCmdName, 0, kSCNameByteLen);
	strcpy((char*)cmd->mCmdName, inCmdName);

	cmd->mFunc = inFunc;
	cmd->mHash = Hash(cmd->mCmdName);
	cmd->mUserData = inUserData;
	AddPlugInCmd(cmd);
		
	return true;
}

#include "sc_msg_iter.h"
#include "SC_Graph.h"

int Unit_DoCmd(World *inWorld, int inSize, char *inData)
{
	sc_msg_iter msg(inSize, inData);
	int nodeID = msg.geti();
	Graph* graph = World_GetGraph(inWorld, nodeID);
	if (!graph) return kSCErr_NodeNotFound;
	
	int unitID = msg.geti();
	if (unitID < 0 || unitID >= graph->mNumUnits) return kSCErr_IndexOutOfRange;
	
	Unit *unit = graph->mUnits[unitID];
	
	UnitDef* unitDef = unit->mUnitDef;
	
	int32 *cmdName = msg.gets4();
	if (!cmdName) return kSCErr_Failed;
	
	if (!unitDef->mCmds) return kSCErr_Failed;
	UnitCmd *cmd = unitDef->mCmds->Get(cmdName);
	if (!cmd) return kSCErr_Failed;
	
	(cmd->mFunc)(unit, &msg);
	
	return kSCErr_None;
}

int PlugIn_DoCmd(int inSize, char *inData)
{	
	sc_msg_iter msg(inSize, inData);

	int32 *cmdName = msg.gets4();
	if (!cmdName) return kSCErr_Failed;
	
	PlugInCmd *cmd = GetPlugInCmd(cmdName);
	if (!cmd) return kSCErr_Failed;
	
	(cmd->mFunc)(cmd->mUserData, &msg);
	
	return kSCErr_None;
}

