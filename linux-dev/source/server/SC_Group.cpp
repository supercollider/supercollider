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

Group* Group_New(World *inWorld, int32 inID)
{	
	Group *group = (Group*)Node_New(inWorld, &gGroupNodeDef, inID);
	group->mNode.mCalcFunc = (NodeCalcFunc)&Group_Calc;
	group->mNode.mIsGroup = true;
	group->mHead = 0;
	group->mTail = 0;
	inWorld->mNumGroups++;
	return group;
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

void Group_AddHead (Group *s, Node *child)
{
	child->mPrev = 0;
	child->mNext = s->mHead;
	if (s->mHead) { s->mHead->mPrev = child; s->mHead = child; }
	else s->mHead = s->mTail = child;
	child->mParent = s;
}

void Group_AddTail (Group *s, Node *child)
{
	child->mPrev = s->mTail;
	child->mNext = 0;
	if (s->mTail) { s->mTail->mNext = child; s->mTail = child; }
	else s->mHead = s->mTail = child;
	child->mParent = s;
}

void Group_Insert(Group *s, Node *child, int index)
{
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

void Group_MapControl(Group *inGroup, int inIndex, int inBus)
{
	Node *child = inGroup->mHead;
	while (child) {
		Node *next = child->mNext;
		Node_MapControl(child, inIndex, inBus);
		child = next;
	}			
}

void Group_MapControl(Group *inGroup, int32 inHash, int32 *inName, int inIndex, int inBus)
{
	Node *child = inGroup->mHead;
	while (child) {
		Node *next = child->mNext;
		Node_MapControl(child, inHash, inName, inIndex, inBus);
		child = next;
	}			
}

void Group_SetControl(Group *inGroup, int inIndex, float inValue)
{
	Node *child = inGroup->mHead;
	while (child) {
		Node *next = child->mNext;
		Node_SetControl(child, inIndex, inValue);
		child = next;
	}			
}

void Group_SetControl(Group *inGroup, int32 inHash, int32 *inName, int inIndex, float inValue)
{
	Node *child = inGroup->mHead;
	while (child) {
		Node *next = child->mNext;
		Node_SetControl(child, inHash, inName, inIndex, inValue);
		child = next;
	}			
}
