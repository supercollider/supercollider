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


SCErr meth_none(World *inWorld, int inSize, char *inData, ReplyAddress *inReply);
SCErr meth_none(World *inWorld, int inSize, char *inData, ReplyAddress *inReply)
{	
	return kSCErr_None;
}

SCErr meth_b_alloc(World *inWorld, int inSize, char *inData, ReplyAddress *inReply);
SCErr meth_b_alloc(World *inWorld, int inSize, char *inData, ReplyAddress *inReply)
{	
	sc_msg_iter msg(inSize, inData);
	int bufindex = msg.geti();
	SndBuf* buf = World_GetBuf(inWorld, bufindex);
	if (buf->shared) return kSCErr_SharedBuf;
	
	CallSequencedCommand(BufAllocCmd, inWorld, inSize, inData, inReply);
	return kSCErr_None;
}

SCErr meth_b_free(World *inWorld, int inSize, char *inData, ReplyAddress *inReply);
SCErr meth_b_free(World *inWorld, int inSize, char *inData, ReplyAddress *inReply)
{
	sc_msg_iter msg(inSize, inData);
	int bufindex = msg.geti();
	SndBuf* buf = World_GetBuf(inWorld, bufindex);
	if (buf->shared) return kSCErr_SharedBuf;
	
	CallSequencedCommand(BufFreeCmd, inWorld, inSize, inData, inReply);
	
	return kSCErr_None;
}

SCErr meth_b_close(World *inWorld, int inSize, char *inData, ReplyAddress *inReply);
SCErr meth_b_close(World *inWorld, int inSize, char *inData, ReplyAddress *inReply)
{
	CallSequencedCommand(BufCloseCmd, inWorld, inSize, inData, inReply);
	
	return kSCErr_None;
}

SCErr meth_b_allocRead(World *inWorld, int inSize, char *inData, ReplyAddress *inReply);
SCErr meth_b_allocRead(World *inWorld, int inSize, char *inData, ReplyAddress *inReply)
{
	sc_msg_iter msg(inSize, inData);
	int bufindex = msg.geti();
	SndBuf* buf = World_GetBuf(inWorld, bufindex);
	if (buf->shared) return kSCErr_SharedBuf;
	
	CallSequencedCommand(BufAllocReadCmd, inWorld, inSize, inData, inReply);
	
	return kSCErr_None;
}

SCErr meth_b_read(World *inWorld, int inSize, char *inData, ReplyAddress *inReply);
SCErr meth_b_read(World *inWorld, int inSize, char *inData, ReplyAddress *inReply)
{
	CallSequencedCommand(BufReadCmd, inWorld, inSize, inData, inReply);
	
	return kSCErr_None;
}

SCErr meth_b_write(World *inWorld, int inSize, char *inData, ReplyAddress *inReply);
SCErr meth_b_write(World *inWorld, int inSize, char *inData, ReplyAddress *inReply)
{
	CallSequencedCommand(BufWriteCmd, inWorld, inSize, inData, inReply);

	return kSCErr_None;
}

SCErr meth_b_zero(World *inWorld, int inSize, char *inData, ReplyAddress *inReply);
SCErr meth_b_zero(World *inWorld, int inSize, char *inData, ReplyAddress *inReply)
{
	CallSequencedCommand(BufZeroCmd, inWorld, inSize, inData, inReply);
	return kSCErr_None;
}


SCErr meth_u_cmd(World *inWorld, int inSize, char *inData, ReplyAddress *inReply);
SCErr meth_u_cmd(World *inWorld, int inSize, char *inData, ReplyAddress* /*inReply*/)
{
	return Unit_DoCmd(inWorld, inSize, inData);	
};

SCErr meth_cmd(World *inWorld, int inSize, char *inData, ReplyAddress *inReply);
SCErr meth_cmd(World *inWorld, int inSize, char *inData, ReplyAddress* /*inReply*/)
{
	return PlugIn_DoCmd(inSize, inData);	
};


/*
SCErr meth_n_cmd(World *inWorld, int inSize, char *inData, ReplyAddress *inReply);
SCErr meth_n_cmd(World *inWorld, int inSize, char *inData, ReplyAddress *inReply)
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

SCErr meth_s_trace(World *inWorld, int inSize, char *inData, ReplyAddress *inReply);
SCErr meth_s_trace(World *inWorld, int inSize, char *inData, ReplyAddress* /*inReply*/)
{
	sc_msg_iter msg(inSize, inData);	
	int32 nodeID = msg.geti();
	Graph *graph = World_GetGraph(inWorld, nodeID);
	if (!graph) return kSCErr_NodeNotFound;
		
	Graph_Trace(graph);

	return kSCErr_None;
}

SCErr meth_n_run(World *inWorld, int inSize, char *inData, ReplyAddress *inReply);
SCErr meth_n_run(World *inWorld, int inSize, char *inData, ReplyAddress* /*inReply*/)
{
	sc_msg_iter msg(inSize, inData);	
	
	while (msg.remain()) {
		int32 nodeID = msg.geti();
		Node *node = World_GetNode(inWorld, nodeID);
		if (!node) return kSCErr_NodeNotFound;
			
		int32 run = msg.geti();
	
		Node_SetRun(node, run);
	}
	
	return kSCErr_None;
}

SCErr meth_n_map(World *inWorld, int inSize, char *inData, ReplyAddress *inReply);
SCErr meth_n_map(World *inWorld, int inSize, char *inData, ReplyAddress* /*inReply*/)
{
	sc_msg_iter msg(inSize, inData);	
	int id = msg.geti();
	Node *node = World_GetNode(inWorld, id);
	if (!node) return kSCErr_NodeNotFound;
	
	while (msg.remain() >= 8) {
		if (msg.nextTag('i') == 's') {
			int32* name = msg.gets4();
			int bus = msg.geti();
			Node_MapControl(node, name, 0, bus);
		} else {
			int32 index = msg.geti();
			int32 bus = msg.geti();
			Node_MapControl(node, index, bus);
		}
	}
	return kSCErr_None;
}

SCErr meth_n_set(World *inWorld, int inSize, char *inData, ReplyAddress *inReply);
SCErr meth_n_set(World *inWorld, int inSize, char *inData, ReplyAddress* /*inReply*/)
{
	sc_msg_iter msg(inSize, inData);	
	int id = msg.geti();
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

SCErr meth_n_setn(World *inWorld, int inSize, char *inData, ReplyAddress *inReply);
SCErr meth_n_setn(World *inWorld, int inSize, char *inData, ReplyAddress* /*inReply*/)
{
	sc_msg_iter msg(inSize, inData);	
	int id = msg.geti();
	Node *node = World_GetNode(inWorld, id);
	if (!node) return kSCErr_NodeNotFound;

	while (msg.remain()) {
		if (msg.nextTag('i') == 's') {
			int32* name = msg.gets4();
			int32 n = msg.geti();
			for (int i=0; msg.remain() && i<n; ++i) {
				float32 value = msg.getf();
				Node_SetControl(node, name, i, value);
			}
		} else {
			int32 index = msg.geti();
			int32 n = msg.geti();
			for (int i=0; msg.remain() && i<n; ++i) {
				float32 value = msg.getf();
				Node_SetControl(node, index+i, value);
			}
		}
	}
	return kSCErr_None;
}

SCErr meth_n_fill(World *inWorld, int inSize, char *inData, ReplyAddress *inReply);
SCErr meth_n_fill(World *inWorld, int inSize, char *inData, ReplyAddress* /*inReply*/)
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


SCErr meth_d_load(World *inWorld, int inSize, char *inData, ReplyAddress *inReply);
SCErr meth_d_load(World *inWorld, int inSize, char *inData, ReplyAddress *inReply)
{
	CallSequencedCommand(LoadSynthDefCmd, inWorld, inSize, inData, inReply);
	
	return kSCErr_None;

}

SCErr meth_d_recv(World *inWorld, int inSize, char *inData, ReplyAddress *inReply);
SCErr meth_d_recv(World *inWorld, int inSize, char *inData, ReplyAddress *inReply)
{
	CallSequencedCommand(RecvSynthDefCmd, inWorld, inSize, inData, inReply);
	
	return kSCErr_None;

}

SCErr meth_d_loadDir(World *inWorld, int inSize, char *inData, ReplyAddress *inReply);
SCErr meth_d_loadDir(World *inWorld, int inSize, char *inData, ReplyAddress *inReply)
{
	CallSequencedCommand(LoadSynthDefDirCmd, inWorld, inSize, inData, inReply);
	
	return kSCErr_None;
}

SCErr meth_d_freeAll(World *inWorld, int inSize, char *inData, ReplyAddress *inReply);
SCErr meth_d_freeAll(World *inWorld, int /*inSize*/, char */*inData*/, ReplyAddress* /*inReply*/)
{
	World_FreeAllGraphDefs(inWorld);
	return kSCErr_None;
}


SCErr meth_s_new(World *inWorld, int inSize, char *inData, ReplyAddress *inReply);
SCErr meth_s_new(World *inWorld, int inSize, char *inData, ReplyAddress* /*inReply*/)
{
	sc_msg_iter msg(inSize, inData);	
	int32 *defname = msg.gets4();
	int32 nodeID = msg.geti();
	int32 addAction = msg.geti();
	int32 addTargetID = msg.geti();

	GraphDef *def = World_GetGraphDef(inWorld, defname);
	if (!def) return kSCErr_SynthDefNotFound;
	

	Graph *graph = 0;
	switch (addAction) {
		case 0 : {
			Group *group = World_GetGroup(inWorld, addTargetID);
			if (!group) return kSCErr_GroupNotFound;
			graph = (Graph*)Node_New(inWorld, &def->mNodeDef, nodeID, &msg);
			if (!graph) return kSCErr_NodeNotFound;
			Group_AddHead(group, &graph->mNode);
		} break;
		case 1 : {
			Group *group = World_GetGroup(inWorld, addTargetID);
			if (!group) return kSCErr_GroupNotFound;
			graph = (Graph*)Node_New(inWorld, &def->mNodeDef, nodeID, &msg);
			if (!graph) return kSCErr_NodeNotFound;
			Group_AddTail(group, &graph->mNode);
		} break;
		case 2 : {
			Node *beforeThisNode = World_GetNode(inWorld, addTargetID);
			if (!beforeThisNode) return kSCErr_NodeNotFound;
			graph = (Graph*)Node_New(inWorld, &def->mNodeDef, nodeID, &msg);
			if (!graph) return kSCErr_NodeNotFound;
			Node_AddBefore(&graph->mNode, beforeThisNode);
		} break;
		case 3 : {
			Node *afterThisNode = World_GetNode(inWorld, addTargetID);
			if (!afterThisNode) return kSCErr_NodeNotFound;
			graph = (Graph*)Node_New(inWorld, &def->mNodeDef, nodeID, &msg);
			if (!graph) return kSCErr_NodeNotFound;
			Node_AddAfter(&graph->mNode, afterThisNode);
		} break;
		default: return kSCErr_Failed;
	}
	Node_StateMsg(&graph->mNode, kNode_Go);
	
		
	return kSCErr_None;
}

SCErr meth_g_new(World *inWorld, int inSize, char *inData, ReplyAddress *inReply);
SCErr meth_g_new(World *inWorld, int inSize, char *inData, ReplyAddress* /*inReply*/)
{
	sc_msg_iter msg(inSize, inData);	
	while (msg.remain()) {
		int32 newGroupID = msg.geti();
		int32 addAction = msg.geti();
		int32 addTargetID = msg.geti();
			
		Group *newGroup = 0;
		switch (addAction) {
			case 0 : {
				Group *group = World_GetGroup(inWorld, addTargetID);
				if (!group) return kSCErr_GroupNotFound;
				newGroup = Group_New(inWorld, newGroupID);
				if (!newGroup) return kSCErr_Failed;
				Group_AddHead(group, &newGroup->mNode);
			} break;
			case 1 : {
				Group *group = World_GetGroup(inWorld, addTargetID);
				if (!group) return kSCErr_GroupNotFound;
				newGroup = Group_New(inWorld, newGroupID);
				if (!newGroup) return kSCErr_Failed;
				Group_AddTail(group, &newGroup->mNode);
			} break;
			case 2 : {
				Node *beforeThisNode = World_GetNode(inWorld, addTargetID);
				if (!beforeThisNode) return kSCErr_NodeNotFound;
				newGroup = Group_New(inWorld, newGroupID);
				if (!newGroup) return kSCErr_Failed;
				Node_AddBefore(&newGroup->mNode, beforeThisNode);
			} break;
			case 3 : {
				Node *afterThisNode = World_GetNode(inWorld, addTargetID);
				if (!afterThisNode) return kSCErr_NodeNotFound;
				newGroup = Group_New(inWorld, newGroupID);
				if (!newGroup) return kSCErr_Failed;
				Node_AddAfter(&newGroup->mNode, afterThisNode);
			} break;
			default: return kSCErr_Failed;
		}
	
		Node_StateMsg(&newGroup->mNode, kNode_Go);
	}
	
	return kSCErr_None;
}


SCErr meth_n_free(World *inWorld, int inSize, char *inData, ReplyAddress *inReply);
SCErr meth_n_free(World *inWorld, int inSize, char *inData, ReplyAddress* /*inReply*/)
{
	sc_msg_iter msg(inSize, inData);	

	while (msg.remain()) {
		int32 nodeID = msg.geti();
		Node *node = World_GetNode(inWorld, nodeID);
		if (!node) return kSCErr_NodeNotFound;
	
		Node_Delete(node);
	}
	
	return kSCErr_None;
}

SCErr meth_n_before(World *inWorld, int inSize, char *inData, ReplyAddress *inReply);
SCErr meth_n_before(World *inWorld, int inSize, char *inData, ReplyAddress* /*inReply*/)
{
	sc_msg_iter msg(inSize, inData);	
	while (msg.remain()) {
		int32 nodeID = msg.geti();
		Node *node = World_GetNode(inWorld, nodeID);
		if (!node) return kSCErr_NodeNotFound;
	
		Node *beforeThisOne = World_GetNode(inWorld, msg.geti());
		if (!beforeThisOne) return kSCErr_NodeNotFound;
	
		//Group *prevGroup = node->mParent;
			
		Node_Remove(node);
		Node_AddBefore(node, beforeThisOne);
	
		//if (node->mParent != prevGroup) {
			Node_StateMsg(node, kNode_Move);
		//}
	}
	return kSCErr_None;
}

SCErr meth_n_after(World *inWorld, int inSize, char *inData, ReplyAddress *inReply);
SCErr meth_n_after(World *inWorld, int inSize, char *inData, ReplyAddress* /*inReply*/)
{
	sc_msg_iter msg(inSize, inData);	
	while (msg.remain()) {
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
	}
	return kSCErr_None;
}

SCErr meth_g_head(World *inWorld, int inSize, char *inData, ReplyAddress *inReply);
SCErr meth_g_head(World *inWorld, int inSize, char *inData, ReplyAddress* /*inReply*/)
{
	sc_msg_iter msg(inSize, inData);	
	while (msg.remain()) {
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
	}
	return kSCErr_None;
}

SCErr meth_g_tail(World *inWorld, int inSize, char *inData, ReplyAddress *inReply);
SCErr meth_g_tail(World *inWorld, int inSize, char *inData, ReplyAddress* /*inReply*/)
{
	sc_msg_iter msg(inSize, inData);	
	while (msg.remain()) {
		Group *group = World_GetGroup(inWorld, msg.geti());
		if (!group) return kSCErr_GroupNotFound;
	
		int32 nodeID = msg.geti();
		Node *node = World_GetNode(inWorld, nodeID);
		if (!node) return kSCErr_NodeNotFound;
	
		//Group *prevGroup = node->mParent;
	
		Node_Remove(node);
		Group_AddTail(group, node);
	
		//if (group != prevGroup) {
			Node_StateMsg(node, kNode_Move);
		//}
	}
	return kSCErr_None;
}

SCErr meth_g_insert(World *inWorld, int inSize, char *inData, ReplyAddress *inReply);
SCErr meth_g_insert(World *inWorld, int inSize, char *inData, ReplyAddress* /*inReply*/)
{
	sc_msg_iter msg(inSize, inData);	
	while (msg.remain()) {
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
	}
	return kSCErr_None;
}

SCErr meth_g_freeAll(World *inWorld, int inSize, char *inData, ReplyAddress *inReply);
SCErr meth_g_freeAll(World *inWorld, int inSize, char *inData, ReplyAddress* /*inReply*/)
{
	sc_msg_iter msg(inSize, inData);	
	while (msg.remain()) {
		Group *group = World_GetGroup(inWorld, msg.geti());
		if (!group) return kSCErr_GroupNotFound;
	
		Group_DeleteAll(group);
	}
	return kSCErr_None;
}

SCErr meth_status(World *inWorld, int inSize, char *inData, ReplyAddress *inReply);
SCErr meth_status(World *inWorld, int inSize, char *inData, ReplyAddress *inReply)
{
	CallSequencedCommand(AudioStatusCmd, inWorld, inSize, inData, inReply);
	return kSCErr_None;
}

SCErr meth_quit(World *inWorld, int inSize, char *inData, ReplyAddress *inReply);
SCErr meth_quit(World *inWorld, int inSize, char *inData, ReplyAddress *inReply)
{
	CallSequencedCommand(AudioQuitCmd, inWorld, inSize, inData, inReply);
	return kSCErr_None;
}

SCErr meth_b_set(World *inWorld, int inSize, char *inData, ReplyAddress *inReply);
SCErr meth_b_set(World *inWorld, int inSize, char *inData, ReplyAddress* /*inReply*/)
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

SCErr meth_b_setn(World *inWorld, int inSize, char *inData, ReplyAddress *inReply);
SCErr meth_b_setn(World *inWorld, int inSize, char *inData, ReplyAddress* /*inReply*/)
{		
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
		
		if (end < 0 || start >= numSamples) continue;
		
		start = sc_clip(start, 0, numSamples-1);
		end   = sc_clip(end,   0, numSamples-1);

		for (int i=start; msg.remain() && i<=end; ++i) {
			float32 value = msg.getf();
			data[i] = value;
		}
	}

	return kSCErr_None;
}


SCErr meth_b_fill(World *inWorld, int inSize, char *inData, ReplyAddress *inReply);
SCErr meth_b_fill(World *inWorld, int inSize, char *inData, ReplyAddress* /*inReply*/)
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

SCErr meth_b_gen(World *inWorld, int inSize, char *inData, ReplyAddress *inReply);
SCErr meth_b_gen(World *inWorld, int inSize, char *inData, ReplyAddress *inReply)
{		
	CallSequencedCommand(BufGenCmd, inWorld, inSize, inData, inReply);

	return kSCErr_None;
}


SCErr meth_c_set(World *inWorld, int inSize, char *inData, ReplyAddress *inReply);
SCErr meth_c_set(World *inWorld, int inSize, char *inData, ReplyAddress* /*inReply*/)
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

SCErr meth_c_setn(World *inWorld, int inSize, char *inData, ReplyAddress *inReply);
SCErr meth_c_setn(World *inWorld, int inSize, char *inData, ReplyAddress* /*inReply*/)
{		
	sc_msg_iter msg(inSize, inData);
	
	float *data = inWorld->mControlBus;
	int maxIndex = inWorld->mNumControlBusChannels;
			
	while (msg.remain()) {
		int32 start = msg.geti();
		int32 n = msg.geti();
		int32 end = start+n-1;
		
		if (end < 0 || start >= maxIndex) continue;
		
		start = sc_clip(start, 0, maxIndex-1);
		end   = sc_clip(end,   0, maxIndex-1);

		for (int i=start; msg.remain() && i<=end; ++i) {
			float32 value = msg.getf();
			data[i] = value;
		}
	}

	return kSCErr_None;
}


SCErr meth_c_fill(World *inWorld, int inSize, char *inData, ReplyAddress *inReply);
SCErr meth_c_fill(World *inWorld, int inSize, char *inData, ReplyAddress* /*inReply*/)
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


#define NEW_COMMAND(name) NewCommand(#name, cmd_##name, meth_##name)

void initMiscCommands();
void initMiscCommands()
{

// nrt
	NEW_COMMAND(none);		
	NEW_COMMAND(notify);		
	NEW_COMMAND(status);	
	NEW_COMMAND(quit);		
		
// async
	NEW_COMMAND(d_recv);		
	NEW_COMMAND(d_load);		
	NEW_COMMAND(d_loadDir);	
	NEW_COMMAND(d_freeAll);	

	NEW_COMMAND(s_new);			
	NEW_COMMAND(s_trace);
				
	NEW_COMMAND(n_free);		
	NEW_COMMAND(n_run);				

// sync	
	NEW_COMMAND(u_cmd);	
	NEW_COMMAND(cmd);	
		
	//NEW_COMMAND(n_cmd);		
	NEW_COMMAND(n_map);		
	NEW_COMMAND(n_set);		
	NEW_COMMAND(n_setn);		
	NEW_COMMAND(n_fill);		
	
// sync	
	NEW_COMMAND(n_before);		
	NEW_COMMAND(n_after);		

// sync	
	NEW_COMMAND(g_new);			
	NEW_COMMAND(g_head);		
	NEW_COMMAND(g_tail);		
	NEW_COMMAND(g_freeAll);		

// async
	NEW_COMMAND(b_alloc);		
	NEW_COMMAND(b_allocRead);	
		
// async
	NEW_COMMAND(b_read);		
	NEW_COMMAND(b_write);	
		
// async
	NEW_COMMAND(b_free);		
	NEW_COMMAND(b_close);		
	
// async
	NEW_COMMAND(b_zero);		
	NEW_COMMAND(b_set);		
	NEW_COMMAND(b_setn);		
	NEW_COMMAND(b_fill);			
	NEW_COMMAND(b_gen);			

	NEW_COMMAND(c_set);		
	NEW_COMMAND(c_setn);		
	NEW_COMMAND(c_fill);					
}


