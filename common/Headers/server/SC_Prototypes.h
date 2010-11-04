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


#ifndef _SC_Prototypes_
#define _SC_Prototypes_

#include <ctype.h> // for size_t

#include "SC_Types.h"
#include "scsynthsend.h"

////////////////////////////////////////////////////////////////////////

// replacement for calloc.
// calloc lazily zeroes memory on first touch. This is good for most purposes, but bad for realtime audio.
void* zalloc(size_t n, size_t size);

////////////////////////////////////////////////////////////////////////

void World_Run(struct World *inWorld);
void World_Start(World *inWorld);
void World_SetSampleRate(struct World *inWorld, double inSampleRate);

extern "C" {
void World_Cleanup(World *inWorld);
void* World_Alloc(struct World *inWorld, size_t inByteSize);
void* World_Realloc(struct World *inWorld, void *inPtr, size_t inByteSize);
void World_Free(struct World *inWorld, void *inPtr);
void World_NRTLock(World *world);
void World_NRTUnlock(World *world);
}

size_t World_TotalFree(struct World *inWorld);
size_t World_LargestFreeChunk(struct World *inWorld);


int32 GetKey(struct Node *inNode);
int32 GetHash(struct Node *inNode);
bool World_AddNode(struct World *inWorld, struct Node* inNode);
bool World_RemoveNode(struct World *inWorld, struct Node* inNode);

extern "C" {
struct Node* World_GetNode(struct World *inWorld, int32 inID);
struct Graph* World_GetGraph(struct World *inWorld, int32 inID);
}

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
int PlugIn_DoCmd(struct World *inWorld, int inSize, char *inArgs, struct ReplyAddress *inReply);

int32 *GetKey(struct GraphDef *inGraphDef);
int32 GetHash(struct GraphDef *inGraphDef);
void World_AddGraphDef(struct World *inWorld, struct GraphDef* inGraphDef);
void World_RemoveGraphDef(struct World *inWorld, struct GraphDef* inGraphDef);
struct GraphDef* World_GetGraphDef(struct World *inWorld, int32* inKey);
void World_FreeAllGraphDefs(World *inWorld);
void GraphDef_Free(GraphDef *inGraphDef);
void GraphDef_Define(World *inWorld, GraphDef *inList);
void GraphDef_FreeOverwritten(World *inWorld);

SCErr bufAlloc(struct SndBuf* buf, int numChannels, int numFrames, double sampleRate);

////////////////////////////////////////////////////////////////////////

void Rate_Init(struct Rate *inRate, double inSampleRate, int inBufLength);

////////////////////////////////////////////////////////////////////////

#define GRAPHDEF(inGraph) ((GraphDef*)((inGraph)->mNode.mDef))
#define GRAPH_PARAM_TABLE(inGraph) (GRAPHDEF(inGraph)->mParamSpecTable)

int Graph_New(struct World *inWorld, struct GraphDef *def, int32 inID, struct sc_msg_iter* args, struct Graph** outGraph,bool argtype=true);
void Graph_Ctor(struct World *inWorld, struct GraphDef *inGraphDef, struct Graph *graph, struct sc_msg_iter *msg,bool argtype);
void Graph_Dtor(struct Graph *inGraph);
int  Graph_GetControl(struct Graph* inGraph, uint32 inIndex, float& outValue);
int  Graph_GetControl(struct Graph* inGraph, int32 inHash, int32 *inName, uint32 inIndex, float& outValue);
void Graph_SetControl(struct Graph* inGraph, uint32 inIndex, float inValue);
void Graph_SetControl(struct Graph* inGraph, int32 inHash, int32 *inName, uint32 inIndex, float inValue);
void Graph_MapControl(Graph* inGraph, uint32 inIndex, uint32 inBus);
void Graph_MapControl(Graph* inGraph, int32 inHash, int32 *inName, uint32 inIndex, uint32 inBus);
void Graph_MapAudioControl(Graph* inGraph, uint32 inIndex, uint32 inBus);
void Graph_MapAudioControl(Graph* inGraph, int32 inHash, int32 *inName, uint32 inIndex, uint32 inBus);
void Graph_Trace(Graph *inGraph);
void Graph_RemoveID(World* inWorld, Graph *inGraph);

////////////////////////////////////////////////////////////////////////

int Node_New(struct World *inWorld, struct NodeDef *def, int32 inID, struct Node **outNode);
void Node_Dtor(struct Node *inNode);
void Node_Remove(struct Node* s);
void Node_Delete(struct Node* inNode);
void Node_AddAfter(struct Node* s, struct Node *afterThisOne);
void Node_AddBefore(struct Node* s, struct Node *beforeThisOne);
void Node_Replace(struct Node* s, struct Node *replaceThisOne);
void Node_SetControl(Node* inNode, int inIndex, float inValue);
void Node_SetControl(Node* inNode, int32 inHash, int32 *inName, int inIndex, float inValue);
void Node_MapControl(Node* inNode, int inIndex, int inBus);
void Node_MapControl(Node* inNode, int32 inHash, int32 *inName, int inIndex, int inBus);
void Node_MapAudioControl(Node* inNode, int inIndex, int inBus);
void Node_MapAudioControl(Node* inNode, int32 inHash, int32 *inName, int inIndex, int inBus);
void Node_StateMsg(Node* inNode, int inState);
void Node_Trace(Node* inNode);
void Node_SendReply(Node* inNode, int replyID, const char* cmdName, int numArgs, const float* values);
void Node_SendReply(Node* inNode, int replyID, const char* cmdName, float value);

extern "C" {
void Node_SetRun(Node* inNode, int inRun);
void Node_SendTrigger(Node* inNode, int triggerID, float value);
void Node_End(struct Node* inNode);
void Node_NullCalc(struct Node* inNode);
void Unit_DoneAction(int doneAction, struct Unit* unit);
}

////////////////////////////////////////////////////////////////////////

extern "C" {
void Group_Calc(Group *inGroup);
void Graph_Calc(struct Graph *inGraph);
}

int Group_New(World *inWorld, int32 inID, Group** outGroup);
void Group_Dtor(Group *inGroup);
void Group_DeleteAll(Group *inGroup);
void Group_DeepFreeGraphs(Group *inGroup);
void Group_AddHead (Group *s, Node *child);
void Group_AddTail (Group *s, Node *child);
void Group_Insert(Group *s, Node *child, int inIndex);
void Group_SetControl(struct Group* inGroup, uint32 inIndex, float inValue);
void Group_SetControl(struct Group *inGroup, int32 inHash, int32 *inName, uint32 inIndex, float inValue);
void Group_MapControl(Group* inGroup, uint32 inIndex, uint32 inBus);
void Group_MapControl(Group* inGroup, int32 inHash, int32 *inName, uint32 inIndex, uint32 inBus);
void Group_MapAudioControl(Group* inGroup, uint32 inIndex, uint32 inBus);
void Group_MapAudioControl(Group* inGroup, int32 inHash, int32 *inName, uint32 inIndex, uint32 inBus);
void Group_Trace(Group* inGroup);
void Group_DumpTree(Group* inGroup);
void Group_DumpTreeAndControls(Group* inGroup);
void Group_CountNodeTags(Group* inGroup, int* count);
void Group_CountNodeAndControlTags(Group* inGroup, int* count, int* controlAndDefCount);
void Group_QueryTree(Group* inGroup, big_scpacket* packet);
void Group_QueryTreeAndControls(Group* inGroup, big_scpacket *packet);

////////////////////////////////////////////////////////////////////////

struct Unit* Unit_New(struct World *inWorld, struct UnitSpec *inUnitSpec, char*& memory);
void Unit_EndCalc(struct Unit *inUnit, int inNumSamples);
void Unit_End(struct Unit *inUnit);

void Unit_Dtor(struct Unit *inUnit);

extern "C" {
void Unit_ZeroOutputs(struct Unit *inUnit, int inNumSamples);
}

////////////////////////////////////////////////////////////////////////

void SendDone(struct ReplyAddress *inReply, const char *inCommandName);
void SendDoneWithIntValue(struct ReplyAddress *inReply, const char *inCommandName, int value);
void SendFailure(struct ReplyAddress *inReply, const char *inCommandName, const char *errString);
void SendFailureWithBufnum(struct ReplyAddress *inReply, const char *inCommandName, const char *errString, uint32 index);
void ReportLateness(struct ReplyAddress *inReply, float32 seconds);
void DumpReplyAddress(struct ReplyAddress *inReplyAddress);
int32 Hash(struct ReplyAddress *inReplyAddress);

////////////////////////////////////////////////////////////////////////

extern "C" {
int32 server_timeseed();
}

////////////////////////////////////////////////////////////////////////

typedef bool (*AsyncStageFn)(World *inWorld, void* cmdData);
typedef void (*AsyncFreeFn)(World *inWorld, void* cmdData);

int PerformAsynchronousCommand
	(
		World *inWorld,
		void* replyAddr,
		const char* cmdName,
		void *cmdData,
		AsyncStageFn stage2, // stage2 is non real time
		AsyncStageFn stage3, // stage3 is real time - completion msg performed if stage3 returns true
		AsyncStageFn stage4, // stage4 is non real time - sends done if stage4 returns true
		AsyncFreeFn cleanup,
		int completionMsgSize,
		void* completionMsgData
	);

////////////////////////////////////////////////////////////////////////

#endif

