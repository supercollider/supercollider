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

#include <stdlib.h>

#include "SC_Endian.h" // first to avoid win32 IN clash

#include "SC_Graph.h"
#include "SC_InterfaceTable.h"
#include "SC_Lib_Cintf.h"
#include "SC_Prototypes.h"
#include "SC_Str4.h"
#include "SC_Unit.h"
#include "SC_UnitDef.h"
#include "SC_World.h"
#include "sc_msg_iter.h"

extern int gMissingNodeID;

bool UnitDef_Create(const char* inName, size_t inAllocSize, UnitCtorFunc inCtor, UnitDtorFunc inDtor, uint32 inFlags) {
    if (strlen(inName) >= kSCNameByteLen)
        return false;

    UnitDef* unitDef = (UnitDef*)malloc(sizeof(UnitDef));
    if (!unitDef)
        return false;

    str4cpy(unitDef->mUnitDefName, inName);
    unitDef->mHash = Hash(unitDef->mUnitDefName);

    unitDef->mAllocSize = inAllocSize;
    unitDef->mUnitCtorFunc = inCtor;
    unitDef->mUnitDtorFunc = inDtor;

    unitDef->mCmds = nullptr;
    unitDef->mFlags = inFlags;

    if (!AddUnitDef(unitDef)) {
        free(unitDef);
        return false;
    }
    return true;
}


bool UnitDef_AddCmd(const char* inUnitDefName, const char* inCmdName, UnitCmdFunc inFunc) {
    if (strlen(inUnitDefName) >= kSCNameByteLen)
        return false;
    int32 unitDefName[kSCNameLen];
    memset(unitDefName, 0, kSCNameByteLen);
    strcpy((char*)unitDefName, inUnitDefName);

    if (strlen(inCmdName) >= kSCNameByteLen)
        return false;

    UnitDef* unitDef = GetUnitDef(unitDefName);
    if (!unitDef)
        return false;

    if (!unitDef->mCmds)
        unitDef->mCmds = new HashTable<UnitCmd, Malloc>(&gMalloc, 4, true);

    UnitCmd* cmd = new UnitCmd();
    memset(cmd->mCmdName, 0, kSCNameByteLen);
    strcpy((char*)cmd->mCmdName, inCmdName);

    cmd->mFunc = inFunc;
    cmd->mHash = Hash(cmd->mCmdName);
    unitDef->mCmds->Add(cmd);

    return true;
}

bool PlugIn_DefineCmd(const char* inCmdName, PlugInCmdFunc inFunc, void* inUserData) {
    if (strlen(inCmdName) >= kSCNameByteLen)
        return false;

    PlugInCmd* cmd = new PlugInCmd();
    memset(cmd->mCmdName, 0, kSCNameByteLen);
    strcpy((char*)cmd->mCmdName, inCmdName);

    cmd->mFunc = inFunc;
    cmd->mHash = Hash(cmd->mCmdName);
    cmd->mUserData = inUserData;
    AddPlugInCmd(cmd);

    return true;
}

void Graph_FirstCalc(Graph* inGraph);
void Graph_NullFirstCalc(Graph* inGraph);
void Graph_QueueUnitCmd(Graph* inGraph, int inSize, const char* inData);

int Unit_DoCmd(World* inWorld, int inSize, char* inData) {
    sc_msg_iter msg(inSize, inData);
    int nodeID = msg.geti();
    gMissingNodeID = nodeID;
    Graph* graph = World_GetGraph(inWorld, nodeID);
    if (!graph)
        return kSCErr_NodeNotFound;

    uint32 unitID = msg.geti();
    if (unitID >= graph->mNumUnits)
        return kSCErr_IndexOutOfRange;

    Unit* unit = graph->mUnits[unitID];

    UnitDef* unitDef = unit->mUnitDef;

    int32* cmdName = msg.gets4();
    if (!cmdName)
        return kSCErr_Failed;

    if (!unitDef->mCmds)
        return kSCErr_Failed;
    UnitCmd* cmd = unitDef->mCmds->Get(cmdName);
    if (!cmd)
        throw std::runtime_error(std::string((char*)cmdName) + " not found");

    // only run unit command if the ctor has been called!
    if (graph->mNode.mCalcFunc == (NodeCalcFunc)&Graph_FirstCalc
        || graph->mNode.mCalcFunc == (NodeCalcFunc)&Graph_NullFirstCalc) {
        Graph_QueueUnitCmd(graph, inSize, inData);
    } else {
        (cmd->mFunc)(unit, &msg);
    }

    return kSCErr_None;
}

int PlugIn_DoCmd(World* inWorld, int inSize, char* inData, ReplyAddress* inReply) {
    sc_msg_iter msg(inSize, inData);

    int32* cmdName = msg.gets4();
    if (!cmdName)
        return kSCErr_Failed;

    PlugInCmd* cmd = GetPlugInCmd(cmdName);
    if (!cmd)
        throw std::runtime_error(std::string((char*)cmdName) + " not found");

    (cmd->mFunc)(inWorld, cmd->mUserData, &msg, (void*)inReply);

    return kSCErr_None;
}
