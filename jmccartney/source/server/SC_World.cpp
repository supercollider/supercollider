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


#include "SC_World.h"
#include "SC_WorldOptions.h"
#include "SC_HiddenWorld.h"
#include "SC_InterfaceTable.h"
#include "SC_AllocPool.h"
#include "SC_GraphDef.h"
#include "SC_UnitDef.h"
#include "SC_BufGen.h"
#include "SC_Node.h"
#include "SC_CoreAudio.h"
#include "SC_Group.h"
#include "SC_Errors.h"
#include <stdio.h>
#include "SC_Prototypes.h"
#include "SC_Samp.h"
#include "SC_ComPort.h"

InterfaceTable gInterfaceTable;
extern HashTable<struct UnitDef, Malloc> *gUnitDefLib;
extern HashTable<struct BufGen, Malloc> *gBufGenLib;
extern HashTable<PlugInCmd, Malloc> *gPlugInCmds;

int sndfileFormatInfoFromStrings(struct SF_INFO *info, 
	const char *headerFormatString, const char *sampleFormatString);


////////////////////////////////////////////////////////////////////////////////

int32 timeseed();

bool SendMsgToEngine(World *inWorld, FifoMsg& inMsg);
bool SendMsgFromEngine(World *inWorld, FifoMsg& inMsg);

void InterfaceTable_Init();
void InterfaceTable_Init()
{
	printf("InterfaceTable_Init\n");
	InterfaceTable *ft = &gInterfaceTable;
	
	ft->mSine = gSine;
	ft->mSecant = gInvSine;
	ft->mSineSize = kSineSize;
	ft->mSineWavetable = gSineWavetable;
	
	ft->fPrint = &printf;
	
	ft->fRanSeed = &timeseed;

	ft->fGraphEnd = &Graph_End;
	
	ft->fDefineUnit = &UnitDef_Create;
	ft->fDefineBufGen = &BufGen_Create;
	ft->fClearUnitOutputs = &Unit_ZeroOutputs;

	ft->fNRTAlloc = &malloc;
	ft->fNRTRealloc = &realloc;
	ft->fNRTFree = &free;

	ft->fRTAlloc = &World_Alloc;
	ft->fRTRealloc = &World_Realloc;
	ft->fRTFree = &World_Free;
	
	ft->fNodeRun = &Node_SetRun;
	
	ft->fSendTrigger = &Node_SendTrigger;
	
	ft->fDefineUnitCmd = &UnitDef_AddCmd;
	ft->fDefinePlugInCmd = &PlugIn_DefineCmd;
	
	ft->fSendMsgFromRT = &SendMsgFromEngine;
	ft->fSendMsgToRT = &SendMsgToEngine;
	
	ft->fSndFileFormatInfoFromStrings = &sndfileFormatInfoFromStrings;
	
	ft->fGetNode = &World_GetNode;
	ft->fGetGraph = &World_GetGraph;
}

void initialize_library();
void initializeScheduler();

World* World_New(WorldOptions *inOptions)
{	
	World *world = 0;
	try {
		static bool gLibInitted = false;
		if (!gLibInitted) {
			InterfaceTable_Init();
			initialize_library();
			initializeScheduler();
			gLibInitted = true;
		}
	
		
		world = (World*)malloc(sizeof(World));
		printf("World_Init %08X\n", (int)world);
		world->hw = (HiddenWorld*)malloc(sizeof(HiddenWorld));
		world->hw->mAllocPool = new AllocPool(malloc, free, inOptions->mRealTimeMemorySize * 1024, 0);
		world->hw->mQuitProgram = new SC_Semaphore(0);
	
		extern Malloc gMalloc;
	
		HiddenWorld *hw = world->hw;
		hw->mGraphDefLib = new HashTable<GraphDef, Malloc>(&gMalloc, inOptions->mMaxGraphDefs, false);
		hw->mNodeLib = new IntHashTable<Node, AllocPool>(hw->mAllocPool, inOptions->mMaxNodes, false);
		hw->mUsers = (ReplyAddress*)malloc(inOptions->mMaxLogins * sizeof(ReplyAddress));
		hw->mNumUsers = 0;
		hw->mMaxUsers = inOptions->mMaxLogins;
		
		world->mNumUnits = 0;
		world->mNumGraphs = 0;
		world->mNumGroups = 0;
	
		world->mBufCounter = 0;
		world->mBufLength = inOptions->mBufLength;
		world->mNumAudioBusChannels = inOptions->mNumAudioBusChannels;
		world->mNumControlBusChannels = inOptions->mNumControlBusChannels;
		world->mNumInputs = inOptions->mNumInputBusChannels;
		world->mNumOutputs = inOptions->mNumOutputBusChannels;
		world->mAudioBus = (float*)malloc(inOptions->mNumAudioBusChannels * inOptions->mBufLength * sizeof(float));
		world->mControlBus = (float*)malloc(inOptions->mNumControlBusChannels * sizeof(float));
		
		world->mAudioBusTouched = (int32*)malloc(inOptions->mNumAudioBusChannels * sizeof(int32));
		world->mControlBusTouched = (int32*)malloc(inOptions->mNumControlBusChannels * sizeof(int32));
		
		size_t sndbufsize = inOptions->mNumBuffers * sizeof(SndBuf);
		world->mSndBufs = (SndBuf*)malloc(sndbufsize);
		world->mSndBufsNonRealTimeMirror = (SndBuf*)malloc(sndbufsize);
		
		int numsamples = world->mBufLength * world->mNumAudioBusChannels;
		
		size_t audiobussize = numsamples * sizeof(float);
		world->mAudioBus = (float*)malloc(audiobussize);
		
		size_t controlbussize = world->mNumControlBusChannels * sizeof(float);
		world->mControlBus = (float*)malloc(controlbussize);
		
		memset(world->mAudioBus, 0, audiobussize);
		memset(world->mControlBus, 0, controlbussize);
		memset(world->mSndBufs, 0, sndbufsize);
		memset(world->mSndBufsNonRealTimeMirror, 0, sndbufsize);
		
		GroupNodeDef_Init();
		
		world->mTopGroup = Group_New(world, 0);
		
		world->ft = &gInterfaceTable;
		
		world->mRGen.init(timeseed());
		
		if (inOptions->mPassword) {
			strncpy(world->hw->mPassword, inOptions->mPassword, 31);
			world->hw->mPassword[31] = 0;
		} else {
			strcpy(world->hw->mPassword, "go");
		}
		
		hw->mMaxWireBufs = inOptions->mMaxWireBufs;
		hw->mWireBufSpace = 0;
	
		hw->mAudioDriver = new SC_CoreAudioDriver(world);
	
		//printf("->GraphDef_LoadDir\n");
		GraphDef *list = 0;
		list = GraphDef_LoadDir(world, "synthdefs", list);
		GraphDef_Define(world, list);
	
		if (!hw->mAudioDriver->Setup()) {
			printf("could not initialize audio.\n");
			return 0;
		}
		if (!hw->mAudioDriver->Start()) {
			printf("start audio failed.\n");
			return 0;
		}
	} catch (std::exception& exc) {
		printf("Exception in World_New: %s\n", exc.what());
		free(world); // need a real clean up routine..
		return 0;
	} catch (...) {
	}
	return world;
}

void World_OpenUDP(struct World *inWorld, int inPort)
{
	try {
		new SC_UdpInPort(inWorld, inPort);
	} catch (std::exception& exc) {
		printf("Exception in World_OpenUDP: %s\n", exc.what());
	} catch (...) {
	}
}

void World_OpenTCP(struct World *inWorld, int inPort, int inMaxConnections, int inBacklog)
{
	try {
		new SC_TcpInPort(inWorld, inPort, inMaxConnections, inBacklog);
	} catch (std::exception& exc) {
		printf("Exception in World_OpenTCP: %s\n", exc.what());
	} catch (...) {
	}
}

void World_WaitForQuit(struct World *inWorld)
{
	try {
		inWorld->hw->mQuitProgram->Acquire();
		inWorld->hw->mAudioDriver->Stop();
	} catch (std::exception& exc) {
		printf("Exception in World_WaitForQuit: %s\n", exc.what());
	} catch (...) {
	}
}

void World_SetSampleRate(World *inWorld, double inSampleRate)
{
	//printf("World_SetSampleRate %08X %g %d\n", inWorld, inSampleRate, inWorld->mBufLength);
	inWorld->mSampleRate = inSampleRate;
	Rate_Init(&inWorld->mFullRate, inSampleRate, inWorld->mBufLength);
	Rate_Init(&inWorld->mBufRate, inSampleRate / inWorld->mBufLength, 1);
}

////////////////////////////////////////////////////////////////////////////////

void* World_Alloc(World *inWorld, size_t inByteSize)
{
	return inWorld->hw->mAllocPool->Alloc(inByteSize);
}

void* World_Realloc(World *inWorld, void *inPtr, size_t inByteSize)
{
	return inWorld->hw->mAllocPool->Realloc(inPtr, inByteSize);
}

size_t World_TotalFree(World *inWorld)
{
	return inWorld->hw->mAllocPool->TotalFree();
}

size_t World_LargestFreeChunk(World *inWorld)
{
	return inWorld->hw->mAllocPool->LargestFreeChunk();
}

void World_Free(World *inWorld, void *inPtr)
{
	inWorld->hw->mAllocPool->Free(inPtr);
}

////////////////////////////////////////////////////////////////////////////////

int32 *GetKey(GraphDef *inGraphDef)
{
	return inGraphDef->mNodeDef.mName;
}

int32 GetHash(GraphDef *inGraphDef)
{
	return inGraphDef->mNodeDef.mHash;
}

bool World_AddGraphDef(World *inWorld, GraphDef* inGraphDef)
{
	return inWorld->hw->mGraphDefLib->Add(inGraphDef);
}

bool World_RemoveGraphDef(World *inWorld, GraphDef* inGraphDef)
{
	bool res =  inWorld->hw->mGraphDefLib->Remove(inGraphDef);
	return res;
}

bool World_FreeGraphDef(World *inWorld, GraphDef* inGraphDef)
{
	bool res =  inWorld->hw->mGraphDefLib->Remove(inGraphDef);
	if (res) GraphDef_Free(inGraphDef);
	return res;
}

void World_FreeAllGraphDefs(World *inWorld)
{
	HashTable<struct GraphDef, Malloc>* lib = inWorld->hw->mGraphDefLib;
	int size = lib->TableSize();
	for (int i=0; i<size; ++i) {
		GraphDef *def = lib->AtIndex(i);
		if (def) GraphDef_Free(def);
	}
	lib->MakeEmpty();
}

GraphDef* World_GetGraphDef(World *inWorld, int32* inKey)
{
	//printf("graph lib size %d\n", inWorld->hw->mGraphDefLib->NumItems());
	//printf("graph lib maxsize %d\n", inWorld->hw->mGraphDefLib->MaxItems());
	//printf("graph lib table size %d\n", inWorld->hw->mGraphDefLib->TableSize());

	return inWorld->hw->mGraphDefLib->Get(inKey);
}

////////////////////////////////////////////////////////////////////////////////

int32 *GetKey(UnitDef *inUnitDef)
{
	return inUnitDef->mUnitDefName;
}

int32 GetHash(UnitDef *inUnitDef)
{
	return inUnitDef->mHash;
}

bool AddUnitDef(UnitDef* inUnitDef)
{
	return gUnitDefLib->Add(inUnitDef);
}

bool RemoveUnitDef(UnitDef* inUnitDef)
{
	return gUnitDefLib->Remove(inUnitDef);
}

UnitDef* GetUnitDef(int32* inKey)
{
	return gUnitDefLib->Get(inKey);
}

////////////////////////////////////////////////////////////////////////////////

int32 *GetKey(BufGen *inBufGen)
{
	return inBufGen->mBufGenName;
}

int32 GetHash(BufGen *inBufGen)
{
	return inBufGen->mHash;
}

bool AddBufGen(BufGen* inBufGen)
{
	return gBufGenLib->Add(inBufGen);
}

bool RemoveBufGen(BufGen* inBufGen)
{
	return gBufGenLib->Remove(inBufGen);
}

BufGen* GetBufGen(int32* inKey)
{
	return gBufGenLib->Get(inKey);
}

////////////////////////////////////////////////////////////////////////////////

int32 *GetKey(PlugInCmd *inPlugInCmd)
{
	return inPlugInCmd->mCmdName;
}

int32 GetHash(PlugInCmd *inPlugInCmd)
{
	return inPlugInCmd->mHash;
}

bool AddPlugInCmd(PlugInCmd* inPlugInCmd)
{
	return gPlugInCmds->Add(inPlugInCmd);
}

bool RemovePlugInCmd(PlugInCmd* inPlugInCmd)
{
	return gPlugInCmds->Remove(inPlugInCmd);
}

PlugInCmd* GetPlugInCmd(int32* inKey)
{
	return gPlugInCmds->Get(inKey);
}

////////////////////////////////////////////////////////////////////////////////

int32 GetKey(Node *inNode)
{
	return inNode->mID;
}

int32 GetHash(Node *inNode)
{
	return inNode->mHash;
}

bool World_AddNode(World *inWorld, Node* inNode)
{
	return inWorld->hw->mNodeLib->Add(inNode);
}

bool World_RemoveNode(World *inWorld, Node* inNode)
{
	return inWorld->hw->mNodeLib->Remove(inNode);
}

Node* World_GetNode(World *inWorld, int32 inID)
{
	return inWorld->hw->mNodeLib->Get(inID);
}

Graph* World_GetGraph(World *inWorld, int32 inID)
{
	Node *node = World_GetNode(inWorld, inID);
	if (!node) return 0;
	return node->mIsGroup ? 0 : (Graph*)node;
}

Group* World_GetGroup(World *inWorld, int32 inID)
{
	Node *node = World_GetNode(inWorld, inID);
	if (!node) return 0;
	return node->mIsGroup ? (Group*)node : 0;
}

////////////////////////////////////////////////////////////////////////////////

void World_Run(World *inWorld)
{
	// run top group
	Group_Calc(inWorld->mTopGroup);
}

void World_Start(World *inWorld)
{
	inWorld->mBufCounter = 0;
	for (int i=0; i<inWorld->mNumAudioBusChannels; ++i) inWorld->mAudioBusTouched[i] = -1;
	for (int i=0; i<inWorld->mNumControlBusChannels; ++i) inWorld->mControlBusTouched[i] = -1;
	
	inWorld->hw->mWireBufSpace = (float*)malloc(inWorld->hw->mMaxWireBufs * inWorld->mBufLength * sizeof(float));
	
	inWorld->hw->mTriggers.MakeEmpty();
	inWorld->hw->mNodeEnds.MakeEmpty();
}

void World_Stop(World *inWorld)
{
	Group_DeleteAll(inWorld->mTopGroup);

	free(inWorld->hw->mWireBufSpace);
	
}

////////////////////////////////////////////////////////////////////////////////


inline int32 BUFMASK(int32 x)
{
	return (1 << (31 - CLZ(x))) - 1;
}

SCErr bufAlloc(SndBuf* buf, int numChannels, int numFrames)
{		
	long numSamples = numFrames * numChannels;
	size_t size = numSamples * sizeof(float);
	//printf("bufAlloc b %ld  s %ld  f %d  c %d\n", size, numSamples, numFrames, numChannels);
	
	buf->data = (float*)malloc(size);
	if (!buf->data) return kSCErr_Failed;
	memset(buf->data, 0, size);
	
	buf->channels = numChannels;
	buf->frames   = numFrames;
	buf->samples  = numSamples;
	buf->mask     = BUFMASK(numSamples); // for delay lines
	buf->mask1    = buf->mask - 1;	// for oscillators

	return kSCErr_None;
}

int sampleFormatFromString(const char* name);
int sampleFormatFromString(const char* name)
{		
	size_t len = strlen(name);
	if (len < 1) return 0;
	
	if (name[0] == 'u') {
		if (len < 4) return 0;
		if (name[4] == '8') return SF_FORMAT_PCM_U8;
		return 0;
	} else if (name[0] == 'i') {
		if (len < 4) return 0;
		if (name[3] == '8') return SF_FORMAT_PCM_S8;
		else if (name[3] == '1') return SF_FORMAT_PCM_16;
		else if (name[3] == '2') return SF_FORMAT_PCM_24;
		else if (name[3] == '3') return SF_FORMAT_PCM_32;
	} else if (name[0] == 'f') {
		return SF_FORMAT_FLOAT;
	} else if (name[0] == 'd') {
		return SF_FORMAT_DOUBLE;
	} else if (name[0] == 'm') {
		return SF_FORMAT_ULAW;
	} else if (name[0] == 'a') {
		return SF_FORMAT_ALAW;
	}
	return 0;
}

int headerFormatFromString(const char *name);
int headerFormatFromString(const char *name)
{
	if (strcasecmp(name, "AIFF")==0) return SF_FORMAT_AIFF;
	if (strcasecmp(name, "AIFC")==0) return SF_FORMAT_AIFF;
	if (strcasecmp(name, "RIFF")==0) return SF_FORMAT_WAV;
	if (strcasecmp(name, "WAVE")==0) return SF_FORMAT_WAV;
	if (strcasecmp(name, "WAV" )==0) return SF_FORMAT_WAV;
	if (strcasecmp(name, "Sun" )==0) return SF_FORMAT_AU;
	if (strcasecmp(name, "IRCAM")==0) return SF_FORMAT_IRCAM;
	if (strcasecmp(name, "NeXT")==0) return SF_FORMAT_AU;
	if (strcasecmp(name, "raw")==0) return SF_FORMAT_RAW;
	return 0;
}

int sndfileFormatInfoFromStrings(struct SF_INFO *info, const char *headerFormatString, const char *sampleFormatString)
{
	int headerFormat = headerFormatFromString(headerFormatString);
	if (!headerFormat) return kSCErr_Failed;
	
	int sampleFormat = sampleFormatFromString(sampleFormatString);
	if (!sampleFormat) return kSCErr_Failed;
	
	info->format = (unsigned int)(headerFormat | sampleFormat);
	return kSCErr_None;
}

#include "scsynthsend.h"

void TriggerMsg::Perform()
{
	scpacket packet;
	packet.adds("/tr");
	packet.maketags(4);
	packet.addtag(',');
	packet.addtag('i');
	packet.addtag('i');
	packet.addtag('f');
	packet.addi(mNodeID);
	packet.addi(mTriggerID);
	packet.addf(mValue);

	ReplyAddress *users = mWorld->hw->mUsers;
	int numUsers = mWorld->hw->mNumUsers;
	for (int i=0; i<numUsers; ++i) {
		SendReply(users+i, packet.data(), packet.size());
	}
}

void NodeEndMsg::Perform()
{
	scpacket packet;
	switch (mState) {
		case kNode_Go :
			packet.adds("/n_go");
			break;
		case kNode_End :
			packet.adds("/n_end");
			break;
		case kNode_On :
			packet.adds("/n_on");
			break;
		case kNode_Off :
			packet.adds("/n_off");
			break;
		case kNode_Move :
			packet.adds("/n_move");
			break;
	}
	packet.maketags(3);
	packet.addtag(',');
	packet.addtag('i');
	packet.addtag('i');
	packet.addi(mNodeID);
	packet.addi(mGroupID);

	ReplyAddress *users = mWorld->hw->mUsers;
	int numUsers = mWorld->hw->mNumUsers;
	for (int i=0; i<numUsers; ++i) {
		SendReply(users+i, packet.data(), packet.size());
	}
}

void DeleteGraphDefMsg::Perform()
{
	GraphDef_Free(mDef);
}

void NotifyNoArgs(World *inWorld, char *inString);
void NotifyNoArgs(World *inWorld, char *inString)
{
	scpacket packet;
	packet.adds(inString);

	ReplyAddress *users = inWorld->hw->mUsers;
	int numUsers = inWorld->hw->mNumUsers;
	for (int i=0; i<numUsers; ++i) {
		SendReply(users+i, packet.data(), packet.size());
	}
}


bool SendMsgToEngine(World *inWorld, FifoMsg& inMsg)
{
	return inWorld->hw->mAudioDriver->SendMsgToEngine(inMsg);
}

bool SendMsgFromEngine(World *inWorld, FifoMsg& inMsg)
{
	return inWorld->hw->mAudioDriver->SendMsgFromEngine(inMsg);
}

