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


#include "SC_Group.h"
#include "SC_SynthDef.h"
#include "SC_World.h"
#include <stdio.h>
#include <stdexcept>
#include <limits.h>
#include "SC_Prototypes.h"
#include "SC_HiddenWorld.h"

void Node_StateMsg(Node* inNode, int inState);

// create a new node
Node* Node_New(World *inWorld, NodeDef *def, int32 inID, sc_msg_iter* args)
{
	//printf("->Node_New %08X %d %d\n", inWorld, inID, def->mAllocSize);
	if (World_GetNode(inWorld, inID)) return 0;
	Node* node = (Node*)World_Alloc(inWorld, def->mAllocSize);
	//printf("node %08X\n", node);
	node->mWorld = inWorld;
	node->mDef = def;
	node->mParent = 0;
	node->mPrev = 0;
	node->mNext = 0;
	node->mIsGroup = false;

	node->mID = inID;
    node->mHash = Hash(inID);
	//printf("hash %d\n", node->mHash);
    if (!World_AddNode(inWorld, node)) {
        throw std::runtime_error("cannot add Node to table. duplicate name or table full.\n");
    }

    (*def->fCtor)(inWorld, def, node, args);
	//printf("<-Node_New\n");
	
	return node;
}


// node destructor
void Node_Dtor(Node *inNode)
{
	//printf("Node_Dtor %08X\n", inNode);		
	Node_StateMsg(inNode, kNode_End);
	Node_Remove(inNode);
	inNode->mWorld->hw->mNodeLib->Remove(inNode);
	World_Free(inNode->mWorld, inNode);
}

// remove a node from a group
void Node_Remove(Node* s) 
{
    //printf("Node_Remove %08X\n", s);
    Group *group = s->mParent;

    if (s->mPrev) s->mPrev->mNext = s->mNext;
    else if (group) group->mHead = s->mNext;
    
    if (s->mNext) s->mNext->mPrev = s->mPrev;
    else if (group) group->mTail = s->mPrev;
    
    s->mPrev = s->mNext = 0;
    s->mParent = 0;
}


// delete a node
void Node_Delete(Node* inNode)
{
	(*inNode->mDef->fDtor)(inNode);
}

// add a node after another one
void Node_AddAfter(Node* s, Node *afterThisOne) 
{
	if (!afterThisOne->mParent) return; // failed

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
	if (!beforeThisOne->mParent) return; // failed
	
	s->mParent = beforeThisOne->mParent;
	s->mPrev = beforeThisOne->mPrev;
	s->mNext = beforeThisOne;
	
	if (beforeThisOne->mPrev) beforeThisOne->mPrev->mNext = s;
	else s->mParent->mHead = s;
	beforeThisOne->mPrev = s;
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
void Node_MapControl(Node* inNode, int32 *inName, int inIndex, int inBus)
{
	if (inNode->mIsGroup) {
		Group_MapControl((Group*)inNode, inName, inIndex, inBus);
	} else {
		Graph_MapControl((Graph*)inNode, inName, inIndex, inBus);
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
void Node_SetControl(Node* inNode, int32 *inName, int inIndex, float inValue)
{
	if (inNode->mIsGroup) {
		Group_SetControl((Group*)inNode, inName, inIndex, inValue);
	} else {
		Graph_SetControl((Graph*)inNode, inName, inIndex, inValue);
	}
}

// this function can be installed using Node_SetRun to cause a node to do nothing
// during its execution time.
void Node_NullCalc(struct Node* inNode);
void Node_NullCalc(struct Node* /*inNode*/)
{
}

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
			inNode->mCalcFunc = &Node_NullCalc;
			Node_StateMsg(inNode, kNode_Off);
		}
	}
}

// send a trigger from a node to a client program.
// this function puts the trigger on a FIFO which is harvested by another thread that
// actually does the sending.
void Node_SendTrigger(Node* inNode, int triggerID, float value)
{
	TriggerMsg msg;
	msg.mWorld = inNode->mWorld;
	msg.mNodeID = inNode->mID;
	msg.mTriggerID = triggerID;
	msg.mValue = value;
	inNode->mWorld->hw->mTriggers.Write(msg);
}

// notify a client program of a node's state change.
// this function puts the message on a FIFO which is harvested by another thread that
// actually does the sending.
void Node_StateMsg(Node* inNode, int inState)
{
	NodeEndMsg msg;
	msg.mWorld = inNode->mWorld;
	msg.mNodeID = inNode->mID;
	if (inNode->mParent) {
		msg.mGroupID = inNode->mParent->mNode.mID;
	} else {
		msg.mGroupID = LONG_MIN;
	}
	msg.mState = inState;
	inNode->mWorld->hw->mNodeEnds.Write(msg);
}