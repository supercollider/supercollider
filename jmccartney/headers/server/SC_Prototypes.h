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


#ifndef _SC_Prototypes_
#define _SC_Prototypes_

#include "SC_Types.h"

////////////////////////////////////////////////////////////////////////

void World_Run(struct World *inWorld);
void World_Start(World *inWorld);
void World_Stop(World *inWorld);
void World_SetSampleRate(struct World *inWorld, double inSampleRate);

void* World_Alloc(struct World *inWorld, size_t inByteSize);
void* World_Realloc(struct World *inWorld, void *inPtr, size_t inByteSize);
void World_Free(struct World *inWorld, void *inPtr);
size_t World_TotalFree(struct World *inWorld);
size_t World_LargestFreeChunk(struct World *inWorld);

int32 GetKey(struct Node *inNode);
int32 GetHash(struct Node *inNode);
bool World_AddNode(struct World *inWorld, struct Node* inNode);
bool World_RemoveNode(struct World *inWorld, struct Node* inNode);
struct Node* World_GetNode(struct World *inWorld, int32 inID);
struct Graph* World_GetGraph(struct World *inWorld, int32 inID);
struct Group* World_GetGroup(struct World *inWorld, int32 inID);

int32 *GetKey(struct UnitDef *inUnitDef);
int32 GetHash(struct UnitDef *inUnitDef);
bool AddUnitDef(struct UnitDef* inUnitDef);
bool RemoveUnitDef(struct UnitDef* inUnitDef);
struct UnitDef* GetUnitDef(int32* inKey);

int32 *GetKey(struct BufGen *inBufGen);
int32 GetHash(struct BufGen *inBufGen);
bool AddBufGen(struct BufGen* inBufGen);
bool RemoveBufGen(struct BufGen* inBufGen);
struct BufGen* GetBufGen(int32* inKey);

int32 *GetKey(struct PlugInCmd *inPlugInCmd);
int32 GetHash(struct PlugInCmd *inPlugInCmd);
bool AddPlugInCmd(struct PlugInCmd* inPlugInCmd);
bool RemovePlugInCmd(struct PlugInCmd* inPlugInCmd);
struct PlugInCmd* GetPlugInCmd(int32* inKey);
int PlugIn_DoCmd(int inSize, char *inArgs);

int32 *GetKey(struct GraphDef *inGraphDef);
int32 GetHash(struct GraphDef *inGraphDef);
bool World_AddGraphDef(struct World *inWorld, struct GraphDef* inGraphDef);
bool World_FreeGraphDef(struct World *inWorld, struct GraphDef* inGraphDef);
bool World_RemoveGraphDef(struct World *inWorld, struct GraphDef* inGraphDef);
struct GraphDef* World_GetGraphDef(struct World *inWorld, int32* inKey);
void World_FreeAllGraphDefs(World *inWorld);
void GraphDef_Free(GraphDef *inGraphDef);
void GraphDef_Define(World *inWorld, GraphDef *inList);
void GraphDef_FreeOverwritten(World *inWorld);

SCErr bufAlloc(struct SndBuf* buf, int numChannels, int numFrames);

////////////////////////////////////////////////////////////////////////

void Rate_Init(struct Rate *inRate, double inSampleRate, int inBufLength);

void Dimension_Init(struct Dimension *inDimension, int inWidth, int inHeight);

////////////////////////////////////////////////////////////////////////

#define GRAPHDEF(inGraph) ((GraphDef*)((inGraph)->mNode.mDef))

void Graph_Ctor(struct World *inWorld, struct GraphDef *inGraphDef, struct Graph *graph, struct sc_msg_iter *msg);
void Graph_Dtor(struct Graph *inGraph);
void Graph_Calc(struct Graph *inGraph);
void Graph_End(struct Graph* inGraph);
void Graph_SetControl(struct Graph* inGraph, int inIndex, float inValue);
void Graph_SetControl(struct Graph* inGraph, int32 *inName, int inIndex, float inValue);
void Graph_MapControl(Graph* inGraph, int inIndex, int inBus);
void Graph_MapControl(Graph* inGraph, int32 *inName, int inIndex, int inBus);
void Graph_Trace(Graph *inGraph);

////////////////////////////////////////////////////////////////////////

Node* Node_New(struct World *inWorld, struct NodeDef *def, int32 inID, struct sc_msg_iter* args);
void Node_Dtor(struct Node *inNode);
void Node_Remove(struct Node* s);
void Node_Delete(struct Node* inNode);
void Node_AddAfter(struct Node* s, struct Node *afterThisOne);
void Node_AddBefore(struct Node* s, struct Node *beforeThisOne);
void Node_SetControl(Node* inNode, int inIndex, float inValue);
void Node_SetControl(Node* inNode, int32 *inName, int inIndex, float inValue);
void Node_SetRun(Node* inNode, int inRun);
void Node_MapControl(Node* inNode, int inIndex, int inBus);
void Node_MapControl(Node* inNode, int32 *inName, int inIndex, int inBus);
void Node_SendTrigger(Node* inNode, int triggerID, float value);
void Node_StateMsg(Node* inNode, int inState);

////////////////////////////////////////////////////////////////////////

Group* Group_New(World *inWorld, int32 inID);
void Group_Dtor(Group *inGroup);
void Group_Calc(Group *inGroup);
void Group_DeleteAll(Group *inGroup);
void Group_AddHead (Group *s, Node *child);
void Group_AddTail (Group *s, Node *child);
void Group_Insert(Group *s, Node *child, int inIndex);
void Group_SetControl(struct Group* inGroup, int inIndex, float inValue);
void Group_SetControl(struct Group *inGroup, int32 *inName, int inIndex, float inValue);
void Group_MapControl(Group* inGroup, int inIndex, int inBus);
void Group_MapControl(Group* inGroup, int32 *inName, int inIndex, int inBus);

////////////////////////////////////////////////////////////////////////

struct Unit* Unit_New(struct World *inWorld, struct UnitSpec *inUnitSpec, char*& memory);
void Unit_ZeroOutputs(struct Unit *inUnit, int inNumSamples);
void Unit_EndCalc(struct Unit *inUnit, int inNumSamples);
void Unit_End(struct Unit *inUnit);

void Unit_Dtor(struct Unit *inUnit);

////////////////////////////////////////////////////////////////////////

void SendDone(struct ReplyAddress *inReply, char *inCommandName);
void SendFailure(struct ReplyAddress *inReply, char *inCommandName, char *errString);
void ReportLateness(struct ReplyAddress *inReply, float32 seconds);
void DumpReplyAddress(struct ReplyAddress *inReplyAddress);
int32 Hash(struct ReplyAddress *inReplyAddress);

////////////////////////////////////////////////////////////////////////

#endif

