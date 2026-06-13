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

SCBool UnitDef_Create(const char* inName, size_t inAllocSize, UnitCtorFunc inCtor, UnitDtorFunc inDtor,
                      uint32 inFlags) {
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

template <typename Func> static SCBool UnitDef_DoAddCmd(const char* inUnitDefName, const char* inCmdName, Func inFunc) {
    if (strlen(inUnitDefName) >= kSCNameByteLen || strlen(inCmdName) >= kSCNameByteLen)
        return false;

    int32 unitDefName[kSCNameLen];
    strncpy((char*)unitDefName, inUnitDefName, kSCNameByteLen);

    UnitDef* unitDef = GetUnitDef(unitDefName);
    if (!unitDef)
        return false;

    if (!unitDef->mCmds)
        unitDef->mCmds = new HashTable<UnitCmd, Malloc>(&gMalloc, 4, true);

    UnitCmd* cmd = new UnitCmd();
    strncpy((char*)cmd->mCmdName, inCmdName, kSCNameByteLen);

    if constexpr (std::is_same_v<Func, UnitCmdFuncEx>) {
        cmd->mFuncEx = inFunc;
        cmd->mHasFuncEx = true;
    } else {
        cmd->mFunc = inFunc;
        cmd->mHasFuncEx = false;
    }
    cmd->mHash = Hash(cmd->mCmdName);
    unitDef->mCmds->Add(cmd);

    return true;
}

SCBool UnitDef_AddCmd(const char* inUnitDefName, const char* inCmdName, UnitCmdFunc inFunc) {
    return UnitDef_DoAddCmd(inUnitDefName, inCmdName, inFunc);
}

SCBool UnitDef_AddCmdEx(const char* inUnitDefName, const char* inCmdName, UnitCmdFuncEx inFunc) {
    return UnitDef_DoAddCmd(inUnitDefName, inCmdName, inFunc);
}

SCBool PlugIn_DefineCmd(const char* inCmdName, PlugInCmdFunc inFunc, void* inUserData) {
    if (strlen(inCmdName) >= kSCNameByteLen)
        return false;

    PlugInCmd* cmd = new PlugInCmd();
    strncpy((char*)cmd->mCmdName, inCmdName, kSCNameByteLen);

    cmd->mFunc = inFunc;
    cmd->mHash = Hash(cmd->mCmdName);
    cmd->mUserData = inUserData;
    AddPlugInCmd(cmd);

    return true;
}

void Graph_FirstCalc(Graph* inGraph);
void Graph_NullFirstCalc(Graph* inGraph);
void Graph_QueueUnitCmd(Graph* inGraph, int inSize, const char* inData, const ReplyAddress* inReplyAddress);

SCErr Unit_DoCmd(World* inWorld, int inSize, const char* inData, ReplyAddress* inReplyAddress) {
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
        Graph_QueueUnitCmd(graph, inSize, inData, inReplyAddress);
    } else {
        Unit_RunCommand(cmd, unit, &msg, inReplyAddress);
    }

    return kSCErr_None;
}

void Unit_RunCommand(const UnitCmd* cmd, Unit* unit, sc_msg_iter* msg, ReplyAddress* inReplyAddr) {
    if (cmd->mHasFuncEx) {
        cmd->mFuncEx(unit, msg, inReplyAddr);
    } else {
        cmd->mFunc(unit, msg);
    }
}

SCErr PlugIn_DoCmd(World* inWorld, int inSize, char* inData, ReplyAddress* inReply) {
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
