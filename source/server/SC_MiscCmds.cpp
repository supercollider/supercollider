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


#include "SC_Lib.h"
#include "SC_ComPort.h"
#include "SC_CoreAudio.h"
#include "SC_HiddenWorld.h"
#include "SC_Graph.h"
#include "SC_GraphDef.h"
#include "SC_Group.h"
#include "SC_UnitDef.h"
#include <stdexcept>
#include "SC_Lib_Cintf.h"
#include "SC_SequencedCommand.h"
#include <new.h>
#include "SC_Prototypes.h"

// returns number of bytes in an OSC string.
int OSCstrlen(char *strin);


SCErr meth_bufAlloc(World *inWorld, int inSize, char *inData, ReplyAddress *inReply);
SCErr meth_bufAlloc(World *inWorld, int inSize, char *inData, ReplyAddress *inReply)
{	
	//printf("data %08X size %d\n", inData, inSize);
	CallSequencedCommand(BufAllocCmd, inWorld, inSize, inData, inReply);
	
	return kSCErr_None;
}

/*SCErr meth_bufShmAlloc(World *inWorld, int inSize, char *inData, ReplyAddress *inReply);
SCErr meth_bufShmAlloc(World *inWorld, int inSize, char *inData, ReplyAddress *inReply)
{	
	//printf("data %08X size %d\n", inData, inSize);
	CallSequencedCommand(BufShmAllocCmd, inWorld, inSize, inData, inReply);
	
	return kSCErr_None;
}*/

SCErr meth_bufFree(World *inWorld, int inSize, char *inData, ReplyAddress *inReply);
SCErr meth_bufFree(World *inWorld, int inSize, char *inData, ReplyAddress *inReply)
{
	CallSequencedCommand(BufFreeCmd, inWorld, inSize, inData, inReply);
	
	return kSCErr_None;
}

SCErr meth_bufAllocRead(World *inWorld, int inSize, char *inData, ReplyAddress *inReply);
SCErr meth_bufAllocRead(World *inWorld, int inSize, char *inData, ReplyAddress *inReply)
{
	CallSequencedCommand(BufAllocReadCmd, inWorld, inSize, inData, inReply);
	
	return kSCErr_None;
}

SCErr meth_bufRead(World *inWorld, int inSize, char *inData, ReplyAddress *inReply);
SCErr meth_bufRead(World *inWorld, int inSize, char *inData, ReplyAddress *inReply)
{
	CallSequencedCommand(BufReadCmd, inWorld, inSize, inData, inReply);
	
	return kSCErr_None;
}

SCErr meth_bufWrite(World *inWorld, int inSize, char *inData, ReplyAddress *inReply);
SCErr meth_bufWrite(World *inWorld, int inSize, char *inData, ReplyAddress *inReply)
{
	CallSequencedCommand(BufWriteCmd, inWorld, inSize, inData, inReply);

	return kSCErr_None;
}

SCErr meth_bufZero(World *inWorld, int inSize, char *inData, ReplyAddress *inReply);
SCErr meth_bufZero(World *inWorld, int inSize, char *inData, ReplyAddress *inReply)
{
	CallSequencedCommand(BufZeroCmd, inWorld, inSize, inData, inReply);
	return kSCErr_None;
}


SCErr meth_unitCmd(World *inWorld, int inSize, char *inData, ReplyAddress *inReply);
SCErr meth_unitCmd(World *inWorld, int inSize, char *inData, ReplyAddress* /*inReply*/)
{
	return Unit_DoCmd(inWorld, inSize, inData);	
};

SCErr meth_defCmd(World *inWorld, int inSize, char *inData, ReplyAddress *inReply);
SCErr meth_defCmd(World *inWorld, int inSize, char *inData, ReplyAddress* /*inReply*/)
{
	return PlugIn_DoCmd(inSize, inData);	
};


/*
SCErr meth_nodeCmd(World *inWorld, int inSize, char *inData, ReplyAddress *inReply);
SCErr meth_nodeCmd(World *inWorld, int inSize, char *inData, ReplyAddress *inReply)
{
	sc_msg_iter msg(inSize, inData);	
	int32 nodeID = msg.geti();
	Node *node = World_GetNode(inWorld, nodeID);
	if (!node) return kSCErr_NodeNotFound;
		
	char *args = msg.rdpos;
	int arglen = msg.remain();
	
//!!	(node->mDef->fNodeCmd)(node, arglen, args);

	return kSCErr_None;
}
*/

SCErr meth_synthTrace(World *inWorld, int inSize, char *inData, ReplyAddress *inReply);
SCErr meth_synthTrace(World *inWorld, int inSize, char *inData, ReplyAddress* /*inReply*/)
{
	sc_msg_iter msg(inSize, inData);	
	int32 nodeID = msg.geti();
	Graph *graph = World_GetGraph(inWorld, nodeID);
	if (!graph) return kSCErr_NodeNotFound;
		
	Graph_Trace(graph);

	return kSCErr_None;
}

SCErr meth_nodeRun(World *inWorld, int inSize, char *inData, ReplyAddress *inReply);
SCErr meth_nodeRun(World *inWorld, int inSize, char *inData, ReplyAddress* /*inReply*/)
{
	sc_msg_iter msg(inSize, inData);	
	int32 nodeID = msg.geti();
	Node *node = World_GetNode(inWorld, nodeID);
	if (!node) return kSCErr_NodeNotFound;
		
	int32 run = msg.geti();

	Node_SetRun(node, run);

	return kSCErr_None;
}

SCErr meth_nodeMap(World *inWorld, int inSize, char *inData, ReplyAddress *inReply);
SCErr meth_nodeMap(World *inWorld, int inSize, char *inData, ReplyAddress* /*inReply*/)
{
	//printf("meth_nodeMap\n");
	sc_msg_iter msg(inSize, inData);	
	int id = msg.geti();
	//printf("id %d\n", id);
	Node *node = World_GetNode(inWorld, id);
	if (!node) return kSCErr_NodeNotFound;
	
	while (msg.remain() >= 8) {
		if (msg.nextTag('i') == 's') {
			int32* name = msg.gets4();
			int bus = msg.geti();
			Node_MapControl(node, name, 0, bus);
		} else {
			int32 index = msg.geti();
			float32 bus = msg.geti();
			Node_MapControl(node, index, bus);
		}
	}
	return kSCErr_None;
}

SCErr meth_nodeSet(World *inWorld, int inSize, char *inData, ReplyAddress *inReply);
SCErr meth_nodeSet(World *inWorld, int inSize, char *inData, ReplyAddress* /*inReply*/)
{
	//printf("meth_nodeSet\n");
	sc_msg_iter msg(inSize, inData);	
	int id = msg.geti();
	//printf("id %d\n", id);
	Node *node = World_GetNode(inWorld, id);
	if (!node) return kSCErr_NodeNotFound;
	
	while (msg.remain() >= 8) {
		if (msg.nextTag('i') == 's') {
			int32* name = msg.gets4();
			float32 value = msg.getf();
			Node_SetControl(node, name, 0, value);
		} else {
			int32 index = msg.geti();
			float32 value = msg.getf();
			Node_SetControl(node, index, value);
		}
	}
	return kSCErr_None;
}

SCErr meth_nodeSetN(World *inWorld, int inSize, char *inData, ReplyAddress *inReply);
SCErr meth_nodeSetN(World *inWorld, int inSize, char *inData, ReplyAddress* /*inReply*/)
{
	sc_msg_iter msg(inSize, inData);	
	int id = msg.geti();
	//printf("id %d\n", id);
	Node *node = World_GetNode(inWorld, id);
	if (!node) return kSCErr_NodeNotFound;

	while (msg.remain()) {
		if (msg.nextTag('i') == 's') {
			int32* name = msg.gets4();
			int32 n = msg.geti();
			for (int i=0; i<n; ++i) {
				float32 value = msg.getf();
				Node_SetControl(node, name, i, value);
			}
		} else {
			int32 index = msg.geti();
			int32 n = msg.geti();
			for (int i=0; i<n; ++i) {
				float32 value = msg.getf();
				Node_SetControl(node, index+i, value);
			}
		}
	}
	return kSCErr_None;
}

SCErr meth_nodeFill(World *inWorld, int inSize, char *inData, ReplyAddress *inReply);
SCErr meth_nodeFill(World *inWorld, int inSize, char *inData, ReplyAddress* /*inReply*/)
{
	sc_msg_iter msg(inSize, inData);	
	int id = msg.geti();
	Node *node = World_GetNode(inWorld, id);
	if (!node) return kSCErr_NodeNotFound;

	while (msg.remain() >= 12) 
	{
		if (msg.nextTag('i') == 's') {
			int32 index = msg.geti();
			int32 n = msg.geti();
			float32 value = msg.getf();
			
			for (int i=0; i<n; ++i) {
				Node_SetControl(node, index+i, value);
			}
		} else {
			int32* name = msg.gets4();
			int32 n = msg.geti();
			float32 value = msg.getf();
			
			for (int i=0; i<n; ++i) {
				Node_SetControl(node, name, i, value);
			}
		}
	}

	return kSCErr_None;
}


SCErr meth_defLoad(World *inWorld, int inSize, char *inData, ReplyAddress *inReply);
SCErr meth_defLoad(World *inWorld, int inSize, char *inData, ReplyAddress *inReply)
{
	CallSequencedCommand(LoadSynthDefCmd, inWorld, inSize, inData, inReply);
	
	return kSCErr_None;

}

SCErr meth_defLoadDir(World *inWorld, int inSize, char *inData, ReplyAddress *inReply);
SCErr meth_defLoadDir(World *inWorld, int inSize, char *inData, ReplyAddress *inReply)
{
	CallSequencedCommand(LoadSynthDefDirCmd, inWorld, inSize, inData, inReply);
	
	return kSCErr_None;
}

SCErr meth_defFreeAll(World *inWorld, int inSize, char *inData, ReplyAddress *inReply);
SCErr meth_defFreeAll(World *inWorld, int /*inSize*/, char */*inData*/, ReplyAddress* /*inReply*/)
{
	printf("meth_defFreeAll\n");

	World_FreeAllGraphDefs(inWorld);
	return kSCErr_None;
}


SCErr meth_synthNew(World *inWorld, int inSize, char *inData, ReplyAddress *inReply);
SCErr meth_synthNew(World *inWorld, int inSize, char *inData, ReplyAddress* /*inReply*/)
{
	//printf("->meth_synthNew\n");
	sc_msg_iter msg(inSize, inData);	
	int32 *defname = msg.gets4();
	int32 nodeID = msg.geti();
	int32 groupID = msg.geti();
	//printf("meth_synthNew '%s' %d %d\n", defname, nodeID, groupID);

	GraphDef *def = World_GetGraphDef(inWorld, defname);
	//printf("def %08X\n", def);
	if (!def) return kSCErr_SynthDefNotFound;

	Group *group = World_GetGroup(inWorld, groupID);
	if (!group) return kSCErr_GroupNotFound;
	
	//printf("*SynthNew %d %d\n", nodeID, arglen);
	Graph *graph = (Graph*)Node_New(inWorld, &def->mNodeDef, nodeID, &msg);
	if (!graph) return kSCErr_NodeNotFound;

	Group_AddTail(group, &graph->mNode);
	Node_StateMsg(&graph->mNode, kNode_Go);
	
	//printf("graph %08X\n", graph);
		
	return kSCErr_None;
}

SCErr meth_groupNew(World *inWorld, int inSize, char *inData, ReplyAddress *inReply);
SCErr meth_groupNew(World *inWorld, int inSize, char *inData, ReplyAddress* /*inReply*/)
{
	sc_msg_iter msg(inSize, inData);	
	int32 newGroupID = msg.geti();
	int32 addGroupID = msg.geti();
	//printf("meth_groupNew %d %d\n", newGroupID, addGroupID);
		
	Group *newGroup = Group_New(inWorld, newGroupID);
	if (!newGroup) return kSCErr_Failed;
	//printf("newGroup %08X\n", newGroup);
	
	Group *addGroup = World_GetGroup(inWorld, addGroupID);
	if (!addGroup) return kSCErr_GroupNotFound;
	//printf("addGroup %08X\n", addGroup);
	Group_AddTail(addGroup, &newGroup->mNode);

	Node_StateMsg(&newGroup->mNode, kNode_Go);
	//printf("kSCErr_None\n");

	return kSCErr_None;
}


SCErr meth_nodeDelete(World *inWorld, int inSize, char *inData, ReplyAddress *inReply);
SCErr meth_nodeDelete(World *inWorld, int inSize, char *inData, ReplyAddress* /*inReply*/)
{
	//printf("meth_nodeDelete\n");
	sc_msg_iter msg(inSize, inData);	

	int32 nodeID = msg.geti();
	Node *node = World_GetNode(inWorld, nodeID);
	if (!node) return kSCErr_NodeNotFound;

	Node_Delete(node);

	return kSCErr_None;
}

SCErr meth_nodeAddBefore(World *inWorld, int inSize, char *inData, ReplyAddress *inReply);
SCErr meth_nodeAddBefore(World *inWorld, int inSize, char *inData, ReplyAddress* /*inReply*/)
{
	sc_msg_iter msg(inSize, inData);	
	int32 nodeID = msg.geti();
	Node *node = World_GetNode(inWorld, nodeID);
	if (!node) return kSCErr_NodeNotFound;

	Node *beforeThisOne = World_GetNode(inWorld, msg.geti());
	if (!beforeThisOne) return kSCErr_NodeNotFound;

	Group *prevGroup = node->mParent;
		
	Node_Remove(node);
	Node_AddBefore(node, beforeThisOne);

	if (node->mParent != prevGroup) {
		Node_StateMsg(node, kNode_Move);
	}
	return kSCErr_None;
}

SCErr meth_nodeAddAfter(World *inWorld, int inSize, char *inData, ReplyAddress *inReply);
SCErr meth_nodeAddAfter(World *inWorld, int inSize, char *inData, ReplyAddress* /*inReply*/)
{
	sc_msg_iter msg(inSize, inData);	
	int32 nodeID = msg.geti();
	Node *node = World_GetNode(inWorld, nodeID);
	if (!node) return kSCErr_NodeNotFound;

	Node *afterThisOne = World_GetNode(inWorld, msg.geti());
	if (!afterThisOne) return kSCErr_NodeNotFound;

	Group *prevGroup = node->mParent;
	
	Node_Remove(node);
	Node_AddBefore(node, afterThisOne);

	if (node->mParent != prevGroup) {
		Node_StateMsg(node, kNode_Move);
	}
	return kSCErr_None;
}

SCErr meth_groupAddHead(World *inWorld, int inSize, char *inData, ReplyAddress *inReply);
SCErr meth_groupAddHead(World *inWorld, int inSize, char *inData, ReplyAddress* /*inReply*/)
{
	sc_msg_iter msg(inSize, inData);	
	Group *group = World_GetGroup(inWorld, msg.geti());
	if (!group) return kSCErr_GroupNotFound;

	int32 nodeID = msg.geti();
	Node *node = World_GetNode(inWorld, nodeID);
	if (!node) return kSCErr_NodeNotFound;

	Group *prevGroup = node->mParent;

	Node_Remove(node);
	
	Group_AddHead(group, node);
	
	if (group != prevGroup) {
		Node_StateMsg(node, kNode_Move);
	}
	return kSCErr_None;
}

SCErr meth_groupAddTail(World *inWorld, int inSize, char *inData, ReplyAddress *inReply);
SCErr meth_groupAddTail(World *inWorld, int inSize, char *inData, ReplyAddress* /*inReply*/)
{
	sc_msg_iter msg(inSize, inData);	
	Group *group = World_GetGroup(inWorld, msg.geti());
	if (!group) return kSCErr_GroupNotFound;

	int32 nodeID = msg.geti();
	Node *node = World_GetNode(inWorld, nodeID);
	if (!node) return kSCErr_NodeNotFound;

	Group *prevGroup = node->mParent;

	Node_Remove(node);
	Group_AddTail(group, node);

	if (group != prevGroup) {
		Node_StateMsg(node, kNode_Move);
	}
	return kSCErr_None;
}

SCErr meth_groupInsert(World *inWorld, int inSize, char *inData, ReplyAddress *inReply);
SCErr meth_groupInsert(World *inWorld, int inSize, char *inData, ReplyAddress* /*inReply*/)
{
	//printf("meth_groupInsert\n");
	sc_msg_iter msg(inSize, inData);	
	Group *group = World_GetGroup(inWorld, msg.geti());
	if (!group) return kSCErr_GroupNotFound;

	int32 nodeID = msg.geti();
	Node *node = World_GetNode(inWorld, nodeID);
	if (!node) return kSCErr_NodeNotFound;

	Group *prevGroup = node->mParent;

	int index = msg.geti();
	
	Node_Remove(node);
	Group_Insert(group, node, index);

	if (group != prevGroup) {
		Node_StateMsg(node, kNode_Move);
	}
	return kSCErr_None;
}

SCErr meth_groupDeleteAll(World *inWorld, int inSize, char *inData, ReplyAddress *inReply);
SCErr meth_groupDeleteAll(World *inWorld, int inSize, char *inData, ReplyAddress* /*inReply*/)
{
	sc_msg_iter msg(inSize, inData);	
	Group *group = World_GetGroup(inWorld, msg.geti());
	if (!group) return kSCErr_GroupNotFound;

	Group_DeleteAll(group);
	return kSCErr_None;
}

SCErr meth_audioStatus(World *inWorld, int inSize, char *inData, ReplyAddress *inReply);
SCErr meth_audioStatus(World *inWorld, int inSize, char *inData, ReplyAddress *inReply)
{
	CallSequencedCommand(AudioStatusCmd, inWorld, inSize, inData, inReply);
	return kSCErr_None;
}

SCErr meth_audioQuit(World *inWorld, int inSize, char *inData, ReplyAddress *inReply);
SCErr meth_audioQuit(World *inWorld, int inSize, char *inData, ReplyAddress *inReply)
{
	CallSequencedCommand(AudioQuitCmd, inWorld, inSize, inData, inReply);
	return kSCErr_None;
}

int64 oscTimeNow();


SCErr meth_bufSet(World *inWorld, int inSize, char *inData, ReplyAddress *inReply);
SCErr meth_bufSet(World *inWorld, int inSize, char *inData, ReplyAddress* /*inReply*/)
{
	sc_msg_iter msg(inSize, inData);
	int bufindex = msg.geti();
	SndBuf* buf = World_GetBuf(inWorld, bufindex);
	if (!buf) return kSCErr_Failed;
	
	float *data = buf->data;
	int numSamples = buf->samples;
	
	while (msg.remain() >= 8)
	{	
		int32 sampleIndex = msg.geti();
		float32 value = msg.getf();
		if (sampleIndex >= 0 && sampleIndex < numSamples) 
		{
			data[sampleIndex] = value;
		} else return kSCErr_IndexOutOfRange;
		
	}
	return kSCErr_None;
}

SCErr meth_bufSetN(World *inWorld, int inSize, char *inData, ReplyAddress *inReply);
SCErr meth_bufSetN(World *inWorld, int inSize, char *inData, ReplyAddress* /*inReply*/)
{		
	//printf("meth_bufSetN\n");
	sc_msg_iter msg(inSize, inData);
	int bufindex = msg.geti();
	SndBuf* buf = World_GetBuf(inWorld, bufindex);
	if (!buf) return kSCErr_Failed;
	
	float *data = buf->data;
	int numSamples = buf->samples;
			
	while (msg.remain()) {
		int32 start = msg.geti();
		int32 n = msg.geti();
		int32 end = start+n-1;
		//printf("setn %d %d %d\n", start, n, end);
		
		if (end < 0 || start >= numSamples) continue;
		
		start = sc_clip(start, 0, numSamples-1);
		end   = sc_clip(end,   0, numSamples-1);

		for (int i=start; i<=end; ++i) {
			float32 value = msg.getf();
			//printf("   %d %g\n", i, value);
			data[i] = value;
		}
	}

	return kSCErr_None;
}


SCErr meth_bufFill(World *inWorld, int inSize, char *inData, ReplyAddress *inReply);
SCErr meth_bufFill(World *inWorld, int inSize, char *inData, ReplyAddress* /*inReply*/)
{		
	sc_msg_iter msg(inSize, inData);
	int bufindex = msg.geti();
	SndBuf* buf = World_GetBuf(inWorld, bufindex);
	if (!buf) return kSCErr_Failed;
	
	float *data = buf->data;
	int numSamples = buf->samples;

	while (msg.remain() >= 12) 
	{
		int32 start = msg.geti();
		int32 n = msg.geti();
		float32 value = msg.getf();
		int32 end = start+n-1;
		
		if (end < 0 || start >= numSamples) continue;
		
		start = sc_clip(start, 0, numSamples-1);
		end   = sc_clip(end,   0, numSamples-1);
		
		for (int i=start; i<=end; ++i) data[i] = value;
	}

	return kSCErr_None;
}

SCErr meth_bufGen(World *inWorld, int inSize, char *inData, ReplyAddress *inReply);
SCErr meth_bufGen(World *inWorld, int inSize, char *inData, ReplyAddress *inReply)
{		
	CallSequencedCommand(BufGenCmd, inWorld, inSize, inData, inReply);

	return kSCErr_None;
}


SCErr meth_busSet(World *inWorld, int inSize, char *inData, ReplyAddress *inReply);
SCErr meth_busSet(World *inWorld, int inSize, char *inData, ReplyAddress* /*inReply*/)
{
	sc_msg_iter msg(inSize, inData);
	
	float *data = inWorld->mControlBus;
	int maxIndex = inWorld->mNumControlBusChannels;
	
	while (msg.remain() >= 8)
	{	
		int32 index = msg.geti();
		float32 value = msg.getf();
		if (index >= 0 && index < maxIndex) 
		{
			data[index] = value;
		} else return kSCErr_IndexOutOfRange;
		
	}
	return kSCErr_None;
}

SCErr meth_busSetN(World *inWorld, int inSize, char *inData, ReplyAddress *inReply);
SCErr meth_busSetN(World *inWorld, int inSize, char *inData, ReplyAddress* /*inReply*/)
{		
	//printf("meth_bufSetN\n");
	sc_msg_iter msg(inSize, inData);
	
	float *data = inWorld->mControlBus;
	int maxIndex = inWorld->mNumControlBusChannels;
			
	while (msg.remain()) {
		int32 start = msg.geti();
		int32 n = msg.geti();
		int32 end = start+n-1;
		//printf("setn %d %d %d\n", start, n, end);
		
		if (end < 0 || start >= maxIndex) continue;
		
		start = sc_clip(start, 0, maxIndex-1);
		end   = sc_clip(end,   0, maxIndex-1);

		for (int i=start; i<=end; ++i) {
			float32 value = msg.getf();
			//printf("   %d %g\n", i, value);
			data[i] = value;
		}
	}

	return kSCErr_None;
}


SCErr meth_busFill(World *inWorld, int inSize, char *inData, ReplyAddress *inReply);
SCErr meth_busFill(World *inWorld, int inSize, char *inData, ReplyAddress* /*inReply*/)
{		
	sc_msg_iter msg(inSize, inData);
	
	float *data = inWorld->mControlBus;
	int maxIndex = inWorld->mNumControlBusChannels;

	while (msg.remain() >= 12) 
	{
		int32 start = msg.geti();
		int32 n = msg.geti();
		float32 value = msg.getf();
		int32 end = start+n-1;
		
		if (end < 0 || start >= maxIndex) continue;
		
		start = sc_clip(start, 0, maxIndex-1);
		end   = sc_clip(end,   0, maxIndex-1);
		
		for (int i=start; i<=end; ++i) data[i] = value;
	}

	return kSCErr_None;
}

SCErr meth_notify(World *inWorld, int inSize, char *inData, ReplyAddress *inReply);
SCErr meth_notify(World *inWorld, int inSize, char *inData, ReplyAddress *inReply)
{
	CallSequencedCommand(NotifyCmd, inWorld, inSize, inData, inReply);
	return kSCErr_None;
}



void initMiscCommands();
void initMiscCommands()
{
	//printf("->initMiscCommands\n");

// nrt
	NewCommand("notify", meth_notify);		
	NewCommand("status", meth_audioStatus);	
	NewCommand("quit", meth_audioQuit);		
		
// async
	NewCommand("d_load", meth_defLoad);		
	NewCommand("d_loadDir", meth_defLoadDir);	
	NewCommand("d_freeAll", meth_defFreeAll);	

	NewCommand("s_new", meth_synthNew);			
	NewCommand("s_trace", meth_synthTrace);
				
	NewCommand("n_free", meth_nodeDelete);		
	NewCommand("n_run", meth_nodeRun);				

// sync	
	NewCommand("u_cmd", meth_unitCmd);	
	NewCommand("cmd", meth_defCmd);	
		
	//NewCommand("n_cmd", meth_nodeCmd);		
	NewCommand("n_map", meth_nodeMap);		
	NewCommand("n_set", meth_nodeSet);		
	NewCommand("n_setn", meth_nodeSetN);		
	NewCommand("n_fill", meth_nodeFill);		
	
// sync	
	NewCommand("n_before", meth_nodeAddBefore);		
	NewCommand("n_after", meth_nodeAddAfter);		

// sync	
	NewCommand("g_new", meth_groupNew);			
	NewCommand("g_head", meth_groupAddHead);		
	NewCommand("g_tail", meth_groupAddTail);		
	NewCommand("g_freeAll", meth_groupDeleteAll);		

// async
	NewCommand("b_alloc", meth_bufAlloc);		
	NewCommand("b_allocRead", meth_bufAllocRead);	
		
// async
	NewCommand("b_read", meth_bufRead);		
	NewCommand("b_write", meth_bufWrite);	
		
// async
	NewCommand("b_free", meth_bufFree);		
	
// async
	NewCommand("b_zero", meth_bufZero);		
	NewCommand("b_set", meth_bufSet);		
	NewCommand("b_setn", meth_bufSetN);		
	NewCommand("b_fill", meth_bufFill);			
	NewCommand("b_gen", meth_bufGen);			

	NewCommand("c_set", meth_busSet);		
	NewCommand("c_setn", meth_busSetN);		
	NewCommand("c_fill", meth_busFill);					

	//printf("<-initMiscCommands\n");
}


