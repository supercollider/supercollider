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


#include "SC_Group.h"
#include "SC_SynthDef.h"
#include "SC_World.h"
#include "SC_WorldOptions.h"
#include "SC_Errors.h"
#include <stdio.h>
#include <stdexcept>
#include <limits.h>
#include "SC_Prototypes.h"
#include "SC_HiddenWorld.h"
#include "Unroll.h"

void Node_StateMsg(Node* inNode, int inState);

// create a new node
int Node_New(World *inWorld, NodeDef *def, int32 inID, Node** outNode)
{
	if (inID < 0) {
		if (inID == -1) { // -1 means generate an id for the event
			HiddenWorld* hw = inWorld->hw;
			inID = hw->mHiddenID = (hw->mHiddenID - 8) | 0x80000000;
		} else {
			return kSCErr_ReservedNodeID;
		}
	}

	if (World_GetNode(inWorld, inID)) {
		return kSCErr_DuplicateNodeID;
	}

	Node* node = (Node*)World_Alloc(inWorld, def->mAllocSize);

	node->mWorld = inWorld;
	node->mDef = def;
	node->mParent = 0;
	node->mPrev = 0;
	node->mNext = 0;
	node->mIsGroup = false;

	node->mID = inID;
	node->mHash = Hash(inID);
	if (!World_AddNode(inWorld, node)) {
		World_Free(inWorld, node);
		return kSCErr_TooManyNodes;
	}

	inWorld->hw->mRecentID = inID;

	*outNode = node;

	return kSCErr_None;
}

// node destructor
void Node_Dtor(Node *inNode)
{
	Node_StateMsg(inNode, kNode_End);
	Node_Remove(inNode);
	World *world = inNode->mWorld;
	world->hw->mNodeLib->Remove(inNode);
	World_Free(world, inNode);
}

// remove a node from a group
void Node_Remove(Node* s)
{
	Group *group = s->mParent;

	if (s->mPrev) s->mPrev->mNext = s->mNext;
	else if (group) group->mHead = s->mNext;

	if (s->mNext) s->mNext->mPrev = s->mPrev;
	else if (group) group->mTail = s->mPrev;

	s->mPrev = s->mNext = 0;
	s->mParent = 0;
}

void Node_RemoveID(Node *inNode)
{
	if (inNode->mID == 0) return; // failed

	World* world = inNode->mWorld;
	if (!World_RemoveNode(world, inNode)) {
		int err = kSCErr_Failed; // shouldn't happen..
		throw err;
	}

	HiddenWorld* hw = world->hw;
	int id = hw->mHiddenID = (hw->mHiddenID - 8) | 0x80000000;
	inNode->mID = id;
	inNode->mHash = Hash(id);
	if (!World_AddNode(world, inNode)) {
		scprintf("mysterious failure in Node_RemoveID\n");
		Node_Delete(inNode);
		// enums are uncatchable. must throw an int.
		int err = kSCErr_Failed; // shouldn't happen..
		throw err;
	}

	//inWorld->hw->mRecentID = id;
}

// delete a node
void Node_Delete(Node* inNode)
{
	if (inNode->mID == 0) return; // failed
	if (inNode->mIsGroup) Group_Dtor((Group*)inNode);
	else Graph_Dtor((Graph*)inNode);
}

// add a node after another one
void Node_AddAfter(Node* s, Node *afterThisOne)
{
	if (!afterThisOne->mParent || s->mID == 0) return; // failed

	s->mParent = afterThisOne->mParent;
	s->mPrev = afterThisOne;
	s->mNext = afterThisOne->mNext;

	if (afterThisOne->mNext) afterThisOne->mNext->mPrev = s;
	else s->mParent->mTail = s;
	afterThisOne->mNext = s;
}

// add a node before another one
void Node_AddBefore(Node* s, Node *beforeThisOne)
{
	if (!beforeThisOne->mParent || s->mID == 0) return; // failed

	s->mParent = beforeThisOne->mParent;
	s->mPrev = beforeThisOne->mPrev;
	s->mNext = beforeThisOne;

	if (beforeThisOne->mPrev) beforeThisOne->mPrev->mNext = s;
	else s->mParent->mHead = s;
	beforeThisOne->mPrev = s;
}

void Node_Replace(Node* s, Node *replaceThisOne)
{
	//scprintf("->Node_Replace\n");
	Group *group = replaceThisOne->mParent;
	if (!group) return; // failed
	if (s->mID == 0) return;

	s->mParent = group;
	s->mPrev = replaceThisOne->mPrev;
	s->mNext = replaceThisOne->mNext;

	if (s->mPrev) s->mPrev->mNext = s;
	else group->mHead = s;

	if (s->mNext) s->mNext->mPrev = s;
	else group->mTail = s;

	replaceThisOne->mPrev = replaceThisOne->mNext = 0;
	replaceThisOne->mParent = 0;

	Node_Delete(replaceThisOne);
	//scprintf("<-Node_Replace\n");
}

// set a node's control so that it reads from a control bus - index argument
void Node_MapControl(Node* inNode, int inIndex, int inBus)
{
	if (inNode->mIsGroup) {
		Group_MapControl((Group*)inNode, inIndex, inBus);
	} else {
		Graph_MapControl((Graph*)inNode, inIndex, inBus);
	}
}

// set a node's control so that it reads from a control bus - name argument
void Node_MapControl(Node* inNode, int32 inHash, int32 *inName, int inIndex, int inBus)
{
	if (inNode->mIsGroup) {
		Group_MapControl((Group*)inNode, inHash, inName, inIndex, inBus);
	} else {
		Graph_MapControl((Graph*)inNode, inHash, inName, inIndex, inBus);
	}
}

// set a node's control so that it reads from a control bus - index argument
void Node_MapAudioControl(Node* inNode, int inIndex, int inBus)
{
	if (inNode->mIsGroup) {
		Group_MapAudioControl((Group*)inNode, inIndex, inBus);
	} else {
		Graph_MapAudioControl((Graph*)inNode, inIndex, inBus);
	}
}

// set a node's control so that it reads from a control bus - name argument
void Node_MapAudioControl(Node* inNode, int32 inHash, int32 *inName, int inIndex, int inBus)
{
	if (inNode->mIsGroup) {
		Group_MapAudioControl((Group*)inNode, inHash, inName, inIndex, inBus);
	} else {
		Graph_MapAudioControl((Graph*)inNode, inHash, inName, inIndex, inBus);
	}
}

// set a node's control value - index argument
void Node_SetControl(Node* inNode, int inIndex, float inValue)
{
	if (inNode->mIsGroup) {
		Group_SetControl((Group*)inNode, inIndex, inValue);
	} else {
		Graph_SetControl((Graph*)inNode, inIndex, inValue);
	}
}

// set a node's control value - name argument
void Node_SetControl(Node* inNode, int32 inHash, int32 *inName, int inIndex, float inValue)
{
	if (inNode->mIsGroup) {
		Group_SetControl((Group*)inNode, inHash, inName, inIndex, inValue);
	} else {
		Graph_SetControl((Graph*)inNode, inHash, inName, inIndex, inValue);
	}
}

// this function can be installed using Node_SetRun to cause a node to do nothing
// during its execution time.
void Node_NullCalc(struct Node* /*inNode*/)
{
}

void Graph_FirstCalc(Graph *inGraph);
void Graph_NullFirstCalc(Graph *inGraph);

// if inRun is zero then the node's calc function is set to Node_NullCalc,
// otherwise its normal calc function is installed.
void Node_SetRun(Node* inNode, int inRun)
{
	if (inRun) {
		if (inNode->mCalcFunc == &Node_NullCalc) {
			if (inNode->mIsGroup) {
				inNode->mCalcFunc = (NodeCalcFunc)&Group_Calc;
			} else {
				inNode->mCalcFunc = (NodeCalcFunc)&Graph_Calc;
			}
			Node_StateMsg(inNode, kNode_On);
		}
	} else {
		if (inNode->mCalcFunc != &Node_NullCalc) {
			if (!inNode->mIsGroup && inNode->mCalcFunc == (NodeCalcFunc)&Graph_FirstCalc) {
				inNode->mCalcFunc = (NodeCalcFunc)&Graph_NullFirstCalc;
			} else {
				inNode->mCalcFunc = (NodeCalcFunc)&Node_NullCalc;
			}
			Node_StateMsg(inNode, kNode_Off);
		}
	}
}


void Node_Trace(Node *inNode)
{
	if (inNode->mIsGroup) {
		Group_Trace((Group*)inNode);
	} else {
		Graph_Trace((Graph*)inNode);
	}
}

void Node_End(Node* inNode)
{
	inNode->mCalcFunc = (NodeCalcFunc)&Node_Delete;
}


// send a trigger from a node to a client program.
// this function puts the trigger on a FIFO which is harvested by another thread that
// actually does the sending.
void Node_SendTrigger(Node* inNode, int triggerID, float value)
{
	World *world = inNode->mWorld;
	if (!world->mRealTime) return;

	TriggerMsg msg;
	msg.mWorld = world;
	msg.mNodeID = inNode->mID;
	msg.mTriggerID = triggerID;
	msg.mValue = value;
	world->hw->mTriggers.Write(msg);
}

// Send a reply from a node to a client program.
//
// This function puts the reply on a FIFO which is harvested by another thread that
// actually does the sending.
//
// NOTE: Only to be called from the realtime thread.
void Node_SendReply(Node* inNode, int replyID, const char* cmdName, int numArgs,  const float* values)
{
	World *world = inNode->mWorld;
	if (!world->mRealTime) return;

	const int cmdNameSize = strlen(cmdName);
	void* mem = World_Alloc(world, cmdNameSize + numArgs*sizeof(float));
	if (mem == 0)
		return;

	NodeReplyMsg msg;
	msg.mWorld = world;
	msg.mNodeID = inNode->mID;
	msg.mID = replyID;
	msg.mValues = (float*)((char*)mem + cmdNameSize);
	memcpy(msg.mValues, values, numArgs*sizeof(float));
	msg.mNumArgs = numArgs;
	msg.mCmdName = (char*)mem;
	memcpy(msg.mCmdName, cmdName, cmdNameSize);
	msg.mCmdNameSize = cmdNameSize;
	msg.mRTMemory = mem;
	world->hw->mNodeMsgs.Write(msg);
}

void Node_SendReply(Node* inNode, int replyID, const char* cmdName, float value)
{
	Node_SendReply(inNode, replyID, cmdName, 1, &value);
}

// notify a client program of a node's state change.
// this function puts the message on a FIFO which is harvested by another thread that
// actually does the sending.
void Node_StateMsg(Node* inNode, int inState)
{
	if (inNode->mID < 0 && inState != kNode_Info) return; // no notification for negative IDs

	World *world = inNode->mWorld;
	if (!world->mRealTime) return;

	NodeEndMsg msg;
	msg.mWorld = world;
	msg.mNodeID = inNode->mID;
	msg.mGroupID = inNode->mParent ? inNode->mParent->mNode.mID : -1 ;
	msg.mPrevNodeID = inNode->mPrev ? inNode->mPrev->mID : -1 ;
	msg.mNextNodeID = inNode->mNext ? inNode->mNext->mID : -1 ;
	if (inNode->mIsGroup) {
		Group *group = (Group*)inNode;
		msg.mIsGroup = 1;
		msg.mHeadID = group->mHead ? group->mHead->mID : -1;
		msg.mTailID = group->mTail ? group->mTail->mID : -1;
	} else {
		msg.mIsGroup = 0;
		msg.mHeadID = -1;
		msg.mTailID = -1;
	}
	msg.mState = inState;
	world->hw->mNodeEnds.Write(msg);
}

#include "SC_Unit.h"

void Unit_DoneAction(int doneAction, Unit *unit)
{
	switch (doneAction)
	{
		case 1 :
			Node_SetRun(&unit->mParent->mNode, 0);
			break;
		case 2 :
			Node_End(&unit->mParent->mNode);
			break;
		case 3 :
		{
			Node_End(&unit->mParent->mNode);
			Node* prev = unit->mParent->mNode.mPrev;
			if (prev) Node_End(prev);
		} break;
		case 4 :
		{
			Node_End(&unit->mParent->mNode);
			Node* next = unit->mParent->mNode.mNext;
			if (next) Node_End(next);
		} break;
		case 5 :
		{
			Node_End(&unit->mParent->mNode);
			Node* prev = unit->mParent->mNode.mPrev;
			if (!prev) break;
			if (prev && prev->mIsGroup) Group_DeleteAll((Group*)prev);
			else Node_End(prev);
		} break;
		case 6 :
		{
			Node_End(&unit->mParent->mNode);
			Node* next = unit->mParent->mNode.mNext;
			if (!next) break;
			if (next->mIsGroup) Group_DeleteAll((Group*)next);
			else Node_End(next);
		} break;
		case 7 :
		{
			Node* node = &unit->mParent->mNode;
			while (node) {
				Node *prev = node->mPrev;
				Node_End(node);
				node = prev;
			}
		} break;
		case 8 :
		{
			Node* node = &unit->mParent->mNode;
			while (node) {
				Node *next = node->mNext;
				Node_End(node);
				node = next;
			}
		} break;
		case 9 :
		{
			Node_End(&unit->mParent->mNode);
			Node* prev = unit->mParent->mNode.mPrev;
			if (prev) Node_SetRun(prev, 0);
		} break;
		case 10 :
		{
			Node_End(&unit->mParent->mNode);
			Node* next = unit->mParent->mNode.mNext;
			if (next) Node_SetRun(next, 0);
		} break;
		case 11 :
		{
			Node_End(&unit->mParent->mNode);
			Node* prev = unit->mParent->mNode.mPrev;
			if (!prev) break;
			if (prev->mIsGroup) Group_DeepFreeGraphs((Group*)prev);
			else Node_End(prev);
		} break;
		case 12 :
		{
			Node_End(&unit->mParent->mNode);
			Node* next = unit->mParent->mNode.mNext;
			if (!next) break;
			if (next->mIsGroup) Group_DeepFreeGraphs((Group*)next);
			else Node_End(next);
		} break;
		case 13 :
		{
			Node* node = unit->mParent->mNode.mParent->mHead;
			while (node) {
				Node *next = node->mNext;
				Node_End(node);
				node = next;
			}
		} break;
		case 14 :
			Node_End(&unit->mParent->mNode.mParent->mNode);
			break;
	}
}
