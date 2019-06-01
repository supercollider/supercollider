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
#include "SC_GraphDef.h"
#include "Hash.h"
#include "sc_msg_iter.h"
#include <string.h>
#include <stdio.h>
#include "SC_Prototypes.h"
#include "SC_Str4.h"
#include "SC_World.h"
#include "SC_WorldOptions.h"
#include "SC_Errors.h"
#include "scsynthsend.h"

NodeDef gGroupNodeDef;

void Group_Dtor(Group* inGroup) {
    Group_DeleteAll(inGroup);
    inGroup->mNode.mWorld->mNumGroups--;
    Node_Dtor(&inGroup->mNode);
}

void GroupNodeDef_Init() {
    str4cpy(gGroupNodeDef.mName, "group");
    gGroupNodeDef.mHash = Hash(gGroupNodeDef.mName);
    gGroupNodeDef.mAllocSize = sizeof(Group);
}

int Group_New(World* inWorld, int32 inID, Group** outGroup) {
    Group* group;
    int err = Node_New(inWorld, &gGroupNodeDef, inID, (Node**)&group);
    if (err)
        return err;

    group->mNode.mCalcFunc = (NodeCalcFunc)&Group_Calc;
    group->mNode.mIsGroup = true;
    group->mHead = 0;
    group->mTail = 0;
    inWorld->mNumGroups++;
    *outGroup = group;

    return kSCErr_None;
}

void Group_Calc(Group* inGroup) {
    Node* child = inGroup->mHead;
    while (child) {
        Node* next = child->mNext;
        (*child->mCalcFunc)(child);
        child = next;
    }
}

void Group_CalcTrace(Group* inGroup) {
    scprintf("TRACE Group %d\n", inGroup->mNode.mID);
    Node* child = inGroup->mHead;
    while (child) {
        Node* next = child->mNext;
        scprintf("   %d %s\n", child->mID, (char*)child->mDef->mName);
        (*child->mCalcFunc)(child);
        child = next;
    }
    inGroup->mNode.mCalcFunc = (NodeCalcFunc)&Group_Calc;
}

void Group_Trace(Group* inGroup) {
    if (inGroup->mNode.mCalcFunc == (NodeCalcFunc)&Group_Calc) {
        inGroup->mNode.mCalcFunc = (NodeCalcFunc)&Group_CalcTrace;
    }
}

void Group_DumpNodeTree(Group* inGroup) {
    static int tabCount = 0;
    if (tabCount == 0)
        scprintf("NODE TREE Group %d\n", inGroup->mNode.mID);
    tabCount++;
    Node* child = inGroup->mHead;
    while (child) {
        Node* next = child->mNext;
        for (int i = 0; i < tabCount; i++)
            scprintf("   "); // small 'tabs'
        scprintf("%d %s\n", child->mID, (char*)child->mDef->mName);
        if (child->mIsGroup) {
            Group_DumpTree((Group*)child);
        }
        (*child->mCalcFunc)(child);
        child = next;
    }
    tabCount--;
}

void Group_DumpNodeTreeAndControls(Group* inGroup) {
    static int tabCount = 0;
    if (tabCount == 0)
        scprintf("NODE TREE Group %d\n", inGroup->mNode.mID);
    tabCount++;
    Node* child = inGroup->mHead;
    while (child) {
        Node* next = child->mNext;
        int i;
        for (i = 0; i < tabCount; i++)
            scprintf("   "); // small 'tabs'
        scprintf("%d %s", child->mID, (char*)child->mDef->mName); // def will be 'group' if it's a group
        if (child->mIsGroup) {
            Group_DumpTreeAndControls((Group*)child);
        } else {
            Graph* childGraph = (Graph*)child;
            int numControls = childGraph->mNumControls;
            if (numControls > 0) {
                scprintf("\n ");
                for (i = 0; i < tabCount; i++)
                    scprintf("   ");
                char** names;
                names = new char*[numControls];

                for (i = 0; i < numControls; i++) {
                    names[i] = NULL;
                }

                // check the number of named controls and stash their names at the correct index
                GraphDef* def = (GraphDef*)(child->mDef);
                int numNamedControls = def->mNumParamSpecs;
                for (i = 0; i < numNamedControls; i++) {
                    ParamSpec* paramSpec = def->mParamSpecs + i;
                    names[paramSpec->mIndex] = (char*)paramSpec->mName;
                }

                // now print the names and values in index order, checking for mappings
                for (i = 0; i < numControls; i++) {
                    float* ptr = childGraph->mControls + i;

                    if (names[i]) {
                        scprintf(" %s: ", names[i]);
                    } else {
                        scprintf(" ", names[i]);
                    }
                    // the ptr in nMapControls should be the same as the control itself, if not, it's mapped.
                    if ((childGraph->mMapControls[i]) != ptr) {
                        int bus;
                        if (childGraph->mControlRates[i] == 2) {
                            bus = (childGraph->mMapControls[i]) - (child->mWorld->mAudioBus);
                            bus = (int)((float)bus / child->mWorld->mBufLength);
                            scprintf("a%d", bus);
                        } else {
                            bus = (childGraph->mMapControls[i]) - (child->mWorld->mControlBus);
                            scprintf("c%d", bus);
                        }
                        // scprintf("bus: %d\n", bus);
                    } else {
                        scprintf("%.14g", *ptr);
                    }
                }

                delete[] names;
            }
        }
        scprintf("\n");
        (*child->mCalcFunc)(child);
        child = next;
    }
    tabCount--;
}

void Group_CalcDumpTree(Group* inGroup) {
    Group_DumpNodeTree(inGroup);
    inGroup->mNode.mCalcFunc = (NodeCalcFunc)&Group_Calc;
}

void Group_DumpTree(Group* inGroup) {
    if (inGroup->mNode.mCalcFunc == (NodeCalcFunc)&Group_Calc) {
        inGroup->mNode.mCalcFunc = (NodeCalcFunc)&Group_CalcDumpTree;
    }
}

void Group_CalcDumpTreeAndControls(Group* inGroup) {
    Group_DumpNodeTreeAndControls(inGroup);
    inGroup->mNode.mCalcFunc = (NodeCalcFunc)&Group_Calc;
}

void Group_DumpTreeAndControls(Group* inGroup) {
    if (inGroup->mNode.mCalcFunc == (NodeCalcFunc)&Group_Calc) {
        inGroup->mNode.mCalcFunc = (NodeCalcFunc)&Group_CalcDumpTreeAndControls;
    }
}

// count the children for this group (deep)
void Group_CountNodeTags(Group* inGroup, int* count) {
    Node* child = inGroup->mHead;
    while (child) {
        Node* next = child->mNext;
        (*count) += 2;
        if (child->mIsGroup) {
            Group_CountNodeTags((Group*)child, count);
        } else {
            (*count)++;
        } // for the defname
        child = next;
    }
}

// count the children for this group (deep)
void Group_CountNodeAndControlTags(Group* inGroup, int* count, int* controlAndDefCount) {
    Node* child = inGroup->mHead;
    while (child) {
        Node* next = child->mNext;
        (*count)++;
        if (child->mIsGroup) {
            Group_CountNodeAndControlTags((Group*)child, count, controlAndDefCount);
        } else {
            //(*controlCount) += ((Graph*)child)->mNumControls + ((GraphDef*)(child->mDef))->mNumParamSpecs + 1;
            (*controlAndDefCount) +=
                ((Graph*)child)->mNumControls * 2 + 2; // def, numControls, name or index and value for each
        }
        child = next;
    }
}

void Group_QueryTree(Group* inGroup, big_scpacket* packet) {
    packet->addtag('i');
    packet->addi(inGroup->mNode.mID);

    // count the children for this group, but not their children (shallow)
    int count = 0;
    Node* child = inGroup->mHead;
    while (child) {
        Node* next = child->mNext;
        count++;
        child = next;
    }

    packet->addtag('i');
    packet->addi(count); // numChildren; for a Group >= 0

    // now iterate over the children
    // id, numChildren, defname
    child = inGroup->mHead;
    while (child) {
        Node* next = child->mNext;
        if (child->mIsGroup) {
            Group_QueryTree((Group*)child, packet);
        } else {
            packet->addtag('i'); // nodeID
            packet->addtag('i'); // numChildren
            packet->addtag('s'); // defname
            packet->addi(child->mID);
            packet->addi(-1);
            packet->adds((char*)child->mDef->mName);
        }
        child = next;
    }
}

void Group_QueryTreeAndControls(Group* inGroup, big_scpacket* packet) {
    packet->addtag('i');
    packet->addi(inGroup->mNode.mID);

    // count the children for this group, but not their children (shallow)
    int count = 0;
    Node* child = inGroup->mHead;
    while (child) {
        Node* next = child->mNext;
        count++;
        child = next;
    }

    packet->addtag('i');
    packet->addi(count);

    // now iterate over the children
    // id, numChildren, defname
    child = inGroup->mHead;
    while (child) {
        Node* next = child->mNext;
        if (child->mIsGroup) {
            Group_QueryTreeAndControls((Group*)child, packet);
        } else {
            packet->addtag('i');
            packet->addtag('i');
            packet->addtag('s');
            packet->addi(child->mID); // nodeID
            packet->addi(-1); // numChildren
            packet->adds((char*)child->mDef->mName); // defName

            Graph* childGraph = (Graph*)child;
            int numControls = childGraph->mNumControls;
            packet->addtag('i');
            packet->addi(numControls);

            char** names;
            names = new char*[numControls];
            int i;
            for (i = 0; i < numControls; i++) {
                names[i] = NULL;
            }

            // check the number of named controls and stash their names
            GraphDef* def = (GraphDef*)(child->mDef);
            int numNamedControls = def->mNumParamSpecs;
            for (i = 0; i < numNamedControls; i++) {
                ParamSpec* paramSpec = def->mParamSpecs + i;
                names[paramSpec->mIndex] = (char*)paramSpec->mName;
            }

            // now add the names and values in index order, checking for mappings
            for (i = 0; i < numControls; i++) {
                float* ptr = childGraph->mControls + i;

                if (names[i]) {
                    packet->addtag('s');
                    packet->adds(names[i]);
                } else {
                    packet->addtag('i');
                    packet->addi(i);
                }
                // the ptr in nMapControls should be the same as the control itself, if not, it's mapped.
                if ((childGraph->mMapControls[i]) != ptr) {
                    // it's mapped
                    int bus;
                    const size_t BUF_SIZE(10);
                    char buf[BUF_SIZE];
                    if (childGraph->mControlRates[i] == 2) {
                        bus = (childGraph->mMapControls[i]) - (child->mWorld->mAudioBus);
                        bus = (int)((float)bus / child->mWorld->mBufLength);
                        snprintf(buf, BUF_SIZE, "%c%d", 'a', bus);
                    } else {
                        bus = (childGraph->mMapControls[i]) - (child->mWorld->mControlBus);
                        snprintf(buf, BUF_SIZE, "%c%d", 'c', bus);
                    }
                    // scprintf("bus: %d\n", bus);
                    packet->addtag('s');
                    packet->adds(buf);
                } else {
                    packet->addtag('f');
                    packet->addf(*ptr);
                }
            }

            delete[] names;
        }
        child = next;
    }
}


void Group_DeleteAll(Group* inGroup) {
    Node* child = inGroup->mHead;
    while (child) {
        Node* next = child->mNext;
        child->mPrev = child->mNext = 0;
        child->mParent = 0;
        Node_Delete(child);
        child = next;
    }
    inGroup->mHead = inGroup->mTail = 0;
}

void Group_DeepFreeGraphs(Group* inGroup) {
    Node* child = inGroup->mHead;
    while (child) {
        Node* next = child->mNext;
        if (child->mIsGroup) {
            Group_DeepFreeGraphs((Group*)child);
        } else {
            Node_Remove(child);
            Node_Delete(child);
        }
        child = next;
    }
}

void Group_AddHead(Group* s, Node* child) {
    if (child->mID == 0)
        return; // failed
    child->mPrev = 0;
    child->mNext = s->mHead;
    if (s->mHead) {
        s->mHead->mPrev = child;
        s->mHead = child;
    } else
        s->mHead = s->mTail = child;
    child->mParent = s;
}

void Group_AddTail(Group* s, Node* child) {
    if (child->mID == 0)
        return; // failed
    child->mPrev = s->mTail;
    child->mNext = 0;
    if (s->mTail) {
        s->mTail->mNext = child;
        s->mTail = child;
    } else
        s->mHead = s->mTail = child;
    child->mParent = s;
}

void Group_Insert(Group* s, Node* child, int index) {
    if (child->mID == 0)
        return; // failed
    if (index <= 0)
        Group_AddHead(s, child);
    else {
        Node* before = s->mHead;
        for (int i = 0; i < index; ++i) {
            before = before->mNext;
            if (!before) {
                Group_AddTail(s, child);
                return;
            }
        }
        Node_AddBefore(child, before);
    }
}

void Group_MapControl(Group* inGroup, uint32 inIndex, uint32 inBus) {
    Node* child = inGroup->mHead;
    while (child) {
        Node* next = child->mNext;
        Node_MapControl(child, inIndex, inBus);
        child = next;
    }
}

void Group_MapControl(Group* inGroup, int32 inHash, int32* inName, uint32 inIndex, uint32 inBus) {
    Node* child = inGroup->mHead;
    while (child) {
        Node* next = child->mNext;
        Node_MapControl(child, inHash, inName, inIndex, inBus);
        child = next;
    }
}

void Group_MapAudioControl(Group* inGroup, uint32 inIndex, uint32 inBus) {
    Node* child = inGroup->mHead;
    while (child) {
        Node* next = child->mNext;
        Node_MapAudioControl(child, inIndex, inBus);
        child = next;
    }
}

void Group_MapAudioControl(Group* inGroup, int32 inHash, int32* inName, uint32 inIndex, uint32 inBus) {
    Node* child = inGroup->mHead;
    while (child) {
        Node* next = child->mNext;
        Node_MapAudioControl(child, inHash, inName, inIndex, inBus);
        child = next;
    }
}
void Group_SetControl(Group* inGroup, uint32 inIndex, float inValue) {
    Node* child = inGroup->mHead;
    while (child) {
        Node* next = child->mNext;
        Node_SetControl(child, inIndex, inValue);
        child = next;
    }
}

void Group_SetControl(Group* inGroup, int32 inHash, int32* inName, uint32 inIndex, float inValue) {
    Node* child = inGroup->mHead;
    while (child) {
        Node* next = child->mNext;
        Node_SetControl(child, inHash, inName, inIndex, inValue);
        child = next;
    }
}
