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
#include "Hash.h"
#include "sc_msg_iter.h"
#include <string.h>
#include <stdio.h>
#include "SC_Prototypes.h"
#include "SC_Str4.h"
#include "SC_World.h"
#include "SC_Errors.h"
#include "scsynthsend.h"

NodeDef gGroupNodeDef;

void Group_Dtor(Group *inGroup)
{
	Group_DeleteAll(inGroup);
	inGroup->mNode.mWorld->mNumGroups--;
	Node_Dtor(&inGroup->mNode);
}

void GroupNodeDef_Init()
{
	str4cpy(gGroupNodeDef.mName, "group");
	gGroupNodeDef.mHash = Hash(gGroupNodeDef.mName);
	gGroupNodeDef.mAllocSize = sizeof(Group);
}

int Group_New(World *inWorld, int32 inID, Group** outGroup)
{	
	Group *group;
	int err = Node_New(inWorld, &gGroupNodeDef, inID, (Node**)&group);
	if (err) return err;
	
	group->mNode.mCalcFunc = (NodeCalcFunc)&Group_Calc;
	group->mNode.mIsGroup = true;
	group->mHead = 0;
	group->mTail = 0;
	inWorld->mNumGroups++;
	*outGroup = group;
	
	return kSCErr_None;
}

void Group_Calc(Group *inGroup)
{
	Node *child = inGroup->mHead;
	while (child) {
        Node *next = child->mNext;
		(*child->mCalcFunc)(child);
		child = next;
	}			
}

void Group_CalcTrace(Group *inGroup)
{
	scprintf("TRACE Group %d\n", inGroup->mNode.mID);
	Node *child = inGroup->mHead;
	while (child) {
        Node *next = child->mNext;
		scprintf("   %d %s\n", child->mID, (char*)child->mDef->mName);
		(*child->mCalcFunc)(child);
		child = next;
	}			
	inGroup->mNode.mCalcFunc = (NodeCalcFunc)&Group_Calc;
}

void Group_Trace(Group *inGroup)
{
	if (inGroup->mNode.mCalcFunc == (NodeCalcFunc)&Group_Calc) {
		inGroup->mNode.mCalcFunc = (NodeCalcFunc)&Group_CalcTrace;
	}
}

void Group_DumpNodeTree(Group *inGroup)
{
	static int tabCount = 0;
	if(tabCount == 0) scprintf("NODE TREE Group %d\n", inGroup->mNode.mID);
	tabCount++;
	Node *child = inGroup->mHead;
	while (child) {
        Node *next = child->mNext;
		for(int i = 0; i < tabCount; i ++) scprintf("   "); // small 'tabs'
		scprintf("%d %s\n", child->mID, (char*)child->mDef->mName);
		if (child->mIsGroup) {
			Group_DumpTree((Group*)child);
		}
		(*child->mCalcFunc)(child);
		child = next;
	}			
	tabCount--;
}

void Group_CalcDumpTree(Group *inGroup)
{
	Group_DumpNodeTree(inGroup);
	inGroup->mNode.mCalcFunc = (NodeCalcFunc)&Group_Calc;
}

void Group_DumpTree(Group* inGroup)
{
	if (inGroup->mNode.mCalcFunc == (NodeCalcFunc)&Group_Calc) {
		inGroup->mNode.mCalcFunc = (NodeCalcFunc)&Group_CalcDumpTree;
	}
}

// count the children for this group (deep)
void Group_CountNodes(Group* inGroup, int* count)
{
	Node *child = inGroup->mHead;
	while (child) {
        Node *next = child->mNext;
		(*count)++;
		if (child->mIsGroup) {
			Group_CountNodes((Group*)child, count);
		}
		child = next;
	}	
}

void Group_QueryTree(Group* inGroup, big_scpacket *packet)
{
	packet->addtag('i');
	packet->addi(inGroup->mNode.mID);
	
	// count the children for this group, but not their children (shallow)
	int count = 0;
	Node *child = inGroup->mHead;
	while (child) {
        Node *next = child->mNext;
		count++;
		child = next;
	}
	
	packet->addtag('i');
	packet->addi(count);
	
	packet->addtag('s');
	packet->adds("group");
	
	// now iterate over the children
	// id, numChildren, defname
	child = inGroup->mHead;
	while (child) {
        Node *next = child->mNext;
		if (child->mIsGroup) {
			Group_QueryTree((Group*)child, packet);
		} else {
			packet->addtag('i');
			packet->addtag('i');
			packet->addtag('s');
			packet->addi(child->mID);
			packet->addi(0);
			packet->adds((char*)child->mDef->mName);
		}
		child = next;
	}	
}

void Group_DeleteAll(Group *inGroup)
{
	Node *child = inGroup->mHead;
	while (child) {
        Node *next = child->mNext;
        child->mPrev = child->mNext = 0;
        child->mParent = 0;
		Node_Delete(child);
		child = next;
	}
	inGroup->mHead = inGroup->mTail = 0;
}

void Group_DeepFreeGraphs(Group *inGroup)
{
	Node *child = inGroup->mHead;
	while (child) {
        Node *next = child->mNext;
		if (child->mIsGroup) {
			Group_DeepFreeGraphs((Group*)child);
		} else {
			Node_Remove(child);
			Node_Delete(child);
		}
		child = next;
	}
}

void Group_AddHead (Group *s, Node *child)
{
	if (child->mID == 0) return; // failed
	child->mPrev = 0;
	child->mNext = s->mHead;
	if (s->mHead) { s->mHead->mPrev = child; s->mHead = child; }
	else s->mHead = s->mTail = child;
	child->mParent = s;
}

void Group_AddTail (Group *s, Node *child)
{
	if (child->mID == 0) return; // failed
	child->mPrev = s->mTail;
	child->mNext = 0;
	if (s->mTail) { s->mTail->mNext = child; s->mTail = child; }
	else s->mHead = s->mTail = child;
	child->mParent = s;
}

void Group_Insert(Group *s, Node *child, int index)
{
	if (child->mID == 0) return; // failed
	if (index <= 0) Group_AddHead(s, child);
	else {
		Node *before = s->mHead;
		for (int i=0; i<index; ++i) {
			before = before->mNext;
			if (!before) {
				Group_AddTail(s, child);
				return;
			}
		}
		Node_AddBefore(child, before);
	}
}

void Group_MapControl(Group *inGroup, uint32 inIndex, uint32 inBus)
{
	Node *child = inGroup->mHead;
	while (child) {
		Node *next = child->mNext;
		Node_MapControl(child, inIndex, inBus);
		child = next;
	}			
}

void Group_MapControl(Group *inGroup, int32 inHash, int32 *inName, uint32 inIndex, uint32 inBus)
{
	Node *child = inGroup->mHead;
	while (child) {
		Node *next = child->mNext;
		Node_MapControl(child, inHash, inName, inIndex, inBus);
		child = next;
	}			
}

void Group_SetControl(Group *inGroup, uint32 inIndex, float inValue)
{
	Node *child = inGroup->mHead;
	while (child) {
		Node *next = child->mNext;
		Node_SetControl(child, inIndex, inValue);
		child = next;
	}			
}

void Group_SetControl(Group *inGroup, int32 inHash, int32 *inName, uint32 inIndex, float inValue)
{
	Node *child = inGroup->mHead;
	while (child) {
		Node *next = child->mNext;
		Node_SetControl(child, inHash, inName, inIndex, inValue);
		child = next;
	}			
}
