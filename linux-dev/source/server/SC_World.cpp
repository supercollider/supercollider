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
#include "SC_StringParser.h"

InterfaceTable gInterfaceTable;
PrintFunc gPrint = 0;

extern HashTable<struct UnitDef, Malloc> *gUnitDefLib;
extern HashTable<struct BufGen, Malloc> *gBufGenLib;
extern HashTable<PlugInCmd, Malloc> *gPlugInCmds;

extern "C" {
int sndfileFormatInfoFromStrings(struct SF_INFO *info, 
	const char *headerFormatString, const char *sampleFormatString);
bool SendMsgToEngine(World *inWorld, FifoMsg& inMsg);
bool SendMsgFromEngine(World *inWorld, FifoMsg& inMsg);
}

////////////////////////////////////////////////////////////////////////////////

// replacement for calloc. 
// calloc lazily zeroes memory on first touch. This is good for most purposes, but bad for realtime audio.
void *zalloc(size_t n, size_t size)
{
	void* ptr = malloc(n * size);
	memset(ptr, 0, size);
	return ptr;
}

////////////////////////////////////////////////////////////////////////////////

void InterfaceTable_Init();
void InterfaceTable_Init()
{
	InterfaceTable *ft = &gInterfaceTable;
	
	ft->mSine = gSine;
	ft->mCosecant = gInvSine;
	ft->mSineSize = kSineSize;
	ft->mSineWavetable = gSineWavetable;
	
	ft->fPrint = &scprintf;
	
	ft->fRanSeed = &timeseed;

	ft->fNodeEnd = &Node_End;
	
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
	
	ft->fNRTLock = &World_NRTLock;
	ft->fNRTUnlock = &World_NRTUnlock;
		
bool HasAltivec();
	ft->mAltivecAvailable = HasAltivec();

	ft->fGroup_DeleteAll = &Group_DeleteAll;
	ft->fDoneAction = &Unit_DoneAction;

}

void initialize_library();
void initializeScheduler();
void World_NonRealTimeSynthesis(struct World *world, WorldOptions *inOptions);

static void World_LoadGraphDefs(World* world);
void World_LoadGraphDefs(World* world)
{
	GraphDef *list = 0;
	list = GraphDef_LoadDir(world, "synthdefs", list);
	GraphDef_Define(world, list);
		
	SC_StringParser sp(getenv("SC_SYNTHDEF_PATH"), ':');
	while (!sp.AtEnd()) {
		GraphDef *list = 0;
		char *path = const_cast<char *>(sp.NextToken());
		list = GraphDef_LoadDir(world, path, list);
		GraphDef_Define(world, list);
	}
}

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
	
		world = (World*)zalloc(1, sizeof(World));
		
		world->hw = (HiddenWorld*)zalloc(1, sizeof(HiddenWorld));

		world->hw->mAllocPool = new AllocPool(malloc, free, inOptions->mRealTimeMemorySize * 1024, 0);
		world->hw->mQuitProgram = new SC_Semaphore(0);
	
		extern Malloc gMalloc;
	
		HiddenWorld *hw = world->hw;
		hw->mGraphDefLib = new HashTable<GraphDef, Malloc>(&gMalloc, inOptions->mMaxGraphDefs, false);
		hw->mNodeLib = new IntHashTable<Node, AllocPool>(hw->mAllocPool, inOptions->mMaxNodes, false);
		hw->mUsers = (ReplyAddress*)zalloc(inOptions->mMaxLogins, sizeof(ReplyAddress));
		hw->mNumUsers = 0;
		hw->mMaxUsers = inOptions->mMaxLogins;
		hw->mHiddenID = -8;
		hw->mRecentID = -8;
		
		
		world->mNumUnits = 0;
		world->mNumGraphs = 0;
		world->mNumGroups = 0;
	
		world->mBufCounter = 0;
		world->mBufLength = inOptions->mBufLength;
		world->mSampleOffset = 0;
		world->mNumAudioBusChannels = inOptions->mNumAudioBusChannels;
		world->mNumControlBusChannels = inOptions->mNumControlBusChannels;
		world->mNumInputs = inOptions->mNumInputBusChannels;
		world->mNumOutputs = inOptions->mNumOutputBusChannels;
		
		world->mNumSharedSndBufs = inOptions->mNumSharedSndBufs;
		world->mSharedSndBufs = inOptions->mSharedSndBufs;
		
		world->mNumSharedControls = inOptions->mNumSharedControls;
		world->mSharedControls = inOptions->mSharedControls;

		int numsamples = world->mBufLength * world->mNumAudioBusChannels;
		world->mAudioBus = (float*)zalloc(numsamples, sizeof(float));

		world->mControlBus = (float*)zalloc(world->mNumControlBusChannels, sizeof(float));
		world->mSharedControls = (float*)zalloc(world->mNumSharedControls, sizeof(float));
		
		world->mAudioBusTouched = (int32*)zalloc(inOptions->mNumAudioBusChannels, sizeof(int32));
		world->mControlBusTouched = (int32*)zalloc(inOptions->mNumControlBusChannels, sizeof(int32));
		
		world->mNumSndBufs = inOptions->mNumBuffers;
		world->mSndBufs = (SndBuf*)zalloc(world->mNumSndBufs, sizeof(SndBuf));
		world->mSndBufsNonRealTimeMirror = (SndBuf*)zalloc(world->mNumSndBufs, sizeof(SndBuf));
				
		GroupNodeDef_Init();
		
		world->mTopGroup = Group_New(world, 0);
	
		world->mRealTime = inOptions->mRealTime;
		
		world->ft = &gInterfaceTable;
		
		world->mNumRGens = inOptions->mNumRGens;
		world->mRGen = new RGen[world->mNumRGens];
		for (int i=0; i<world->mNumRGens; ++i) {
			world->mRGen[i].init(timeseed());
		}
		
		world->mNRTLock = new SC_Lock();
		
		if (inOptions->mPassword) {
			strncpy(world->hw->mPassword, inOptions->mPassword, 31);
			world->hw->mPassword[31] = 0;
		} else {
			world->hw->mPassword[0] = 0;
		}
		
		hw->mMaxWireBufs = inOptions->mMaxWireBufs;
		hw->mWireBufSpace = 0;
	
		if (world->mRealTime) {
			hw->mAudioDriver = new SC_CoreAudioDriver(world);
			hw->mAudioDriver->SetPreferredHardwareBufferFrameSize(
					inOptions->mPreferredHardwareBufferFrameSize
			);
			
			World_LoadGraphDefs(world);

			if (!hw->mAudioDriver->Setup()) {
				scprintf("could not initialize audio.\n");
				return 0;
			}
			if (!hw->mAudioDriver->Start()) {
				scprintf("start audio failed.\n");
				return 0;
			}
		} else {	
			World_NonRealTimeSynthesis(world, inOptions);
			world = 0;
		}
	} catch (std::exception& exc) {
		scprintf("Exception in World_New: %s\n", exc.what());
		World_Cleanup(world); 
		return 0;
	} catch (...) {
	}
	return world;
}

bool nextOSCPacket(FILE *file, OSC_Packet *packet, int64& outTime)
{
	int32 msglen;
	if (!fread(&msglen, 1, sizeof(int32), file)) return true;
	if (msglen > 8192) 
		throw std::runtime_error("OSC packet too long. > 8192 bytes\n");
		
	fread(packet->mData, 1, msglen, file);
	if (strcmp(packet->mData, "#bundle")!=0)
			throw std::runtime_error("OSC packet not a bundle\n");
	
	packet->mSize = msglen;
	
	outTime = *(int64*)(packet->mData+8);
	return false;
}

void PerformOSCBundle(World *inWorld, OSC_Packet *inPacket);

void World_NonRealTimeSynthesis(struct World *world, WorldOptions *inOptions)
{
	World_LoadGraphDefs(world);
	const int kFileBufFrames = 8192;
	const int kBufMultiple = kFileBufFrames / world->mBufLength;

	// batch process non real time audio
	if (!inOptions->mNonRealTimeCmdFilename)
		throw std::runtime_error("Non real time command filename is NULL.\n");
		
	if (!inOptions->mNonRealTimeOutputFilename) 
		throw std::runtime_error("Non real time output filename is NULL.\n");
	
	SF_INFO inputFileInfo, outputFileInfo;
	float *inputFileBuf = 0;
	float *outputFileBuf = 0;
	int numInputChannels;
	int numOutputChannels;
	
	outputFileInfo.samplerate = inOptions->mNonRealTimeSampleRate;
	numOutputChannels = outputFileInfo.channels = world->mNumOutputs;
	world->hw->mNRTOutputFile = sf_open(inOptions->mNonRealTimeOutputFilename, SFM_WRITE, &outputFileInfo);
	if (!world->hw->mNRTOutputFile) 
		throw std::runtime_error("Couldn't open non real time output file.\n");
	
	outputFileBuf = (float*)calloc(1, world->mNumOutputs * kFileBufFrames * sizeof(float));
	
	if (inOptions->mNonRealTimeInputFilename) {
		world->hw->mNRTInputFile = sf_open(inOptions->mNonRealTimeInputFilename, SFM_READ, &inputFileInfo);
		if (!world->hw->mNRTInputFile) 
			throw std::runtime_error("Couldn't open non real time input file.\n");

		inputFileBuf = (float*)calloc(1, inputFileInfo.channels * kFileBufFrames * sizeof(float));
		
		if (world->mNumInputs != inputFileInfo.channels)
			scprintf("WARNING: input file channels didn't match number of inputs specified in options.\n");

		numInputChannels = world->mNumInputs = inputFileInfo.channels; // force it.

		if (inputFileInfo.samplerate != inOptions->mNonRealTimeSampleRate)
			scprintf("WARNING: input file sample rate does not equal output sample rate.\n");
			
	} else {
		world->hw->mNRTInputFile = 0;
	}
	
	FILE *cmdFile;
	if (inOptions->mNonRealTimeCmdFilename) {
		cmdFile = fopen(inOptions->mNonRealTimeCmdFilename, "r");
	} else cmdFile = stdin;
	if (!cmdFile) 
		throw std::runtime_error("Couldn't open non real time command file.\n");
		
	char msgbuf[8192];
	OSC_Packet packet;
	memset(&packet, 0, sizeof(packet));
	packet.mData = msgbuf;
	packet.mIsBundle = true;
	packet.mReplyAddr.mReplyFunc = null_reply_func;

	int64 schedTime;
	if (nextOSCPacket(cmdFile, &packet, schedTime))
		throw std::runtime_error("command file empty.\n");
	int64 prevTime = schedTime;
	
	World_SetSampleRate(world, inOptions->mNonRealTimeSampleRate);
	World_Start(world);
	
	int bufLength = world->mBufLength;
	int64 oscTime = 0;
	double oscToSamples = inOptions->mNonRealTimeSampleRate / pow(2.,32.);
	int64 oscInc = (int64)((double)bufLength / oscToSamples);
	
	bool run = true;
	int inBufStep = numInputChannels * bufLength;
	int outBufStep = numOutputChannels * bufLength;
	float* inputBuses    = world->mAudioBus + world->mNumOutputs * bufLength;
	float* outputBuses   = world->mAudioBus;
	int32* inputTouched  = world->mAudioBusTouched + world->mNumOutputs;
	int32* outputTouched = world->mAudioBusTouched;
	for (; run;) {
		int bufFramesCalculated = 0;
		float* inBufPos = inputFileBuf;
		float* outBufPos = outputFileBuf;
		
		if (world->hw->mNRTInputFile) {
			int framesRead = sf_readf_float(world->hw->mNRTInputFile, inputFileBuf, kFileBufFrames);
			if (framesRead < kFileBufFrames) {
				memset(inputFileBuf + framesRead * numInputChannels, 0, 
					(kFileBufFrames - framesRead) * numInputChannels * sizeof(float));
			}
		}
		
		for (int i=0; i<kBufMultiple && run; ++i) {
			int bufCounter = world->mBufCounter;
			
			// deinterleave input to input buses
			if (inputFileBuf) {
				float *inBus = inputBuses;
				for (int j=0; j<numInputChannels; ++j, inBus += bufLength) {
					float *inFileBufPtr = inBufPos + j;
					for (int k=0; k<bufLength; ++k) {
						inBus[k] = *inFileBufPtr;
						inFileBufPtr += numInputChannels;
					}
					inputTouched[j] = bufCounter;
				}
			}
			
			// execute ready commands
			int64 nextTime = oscTime + oscInc;
			
			while (schedTime <= nextTime) {
				world->mSampleOffset = (int)((double)(schedTime - oscTime) * oscToSamples);
				if (world->mSampleOffset < 0) world->mSampleOffset = 0;
				else if (world->mSampleOffset >= world->mBufLength) world->mSampleOffset = world->mBufLength-1;
	
				PerformOSCBundle(world, &packet);
				if (nextOSCPacket(cmdFile, &packet, schedTime)) { run = false; break; }
				if (schedTime < prevTime) {
					scprintf("ERROR: Packet time stamps out-of-order.\n");
					goto Bail;
				}
				prevTime = schedTime;
			}
			
			World_Run(world);
			
			// interleave output to output buffer
			float *outBus = outputBuses;
			for (int j=0; j<numOutputChannels; ++j, outBus += bufLength) {
				float *outFileBufPtr = outBufPos + j;
				for (int k=0; k<bufLength; ++k) {
					*outFileBufPtr = outBus[k];
					outFileBufPtr += numInputChannels;
				}
				outputTouched[j] = bufCounter;
			}
			bufFramesCalculated += bufLength;
			inBufPos += inBufStep;
			outBufPos += outBufStep;
			world->mBufCounter++;
		}
Bail:
		// write output
		sf_writef_float(world->hw->mNRTOutputFile, outputFileBuf, bufFramesCalculated);
	}
	
	sf_close(world->hw->mNRTOutputFile);
	if (world->hw->mNRTInputFile) sf_close(world->hw->mNRTInputFile);
	
	World_Cleanup(world);
}

void World_OpenUDP(struct World *inWorld, int inPort)
{
	try {
		new SC_UdpInPort(inWorld, inPort);
	} catch (std::exception& exc) {
		scprintf("Exception in World_OpenUDP: %s\n", exc.what());
	} catch (...) {
	}
}

void World_OpenTCP(struct World *inWorld, int inPort, int inMaxConnections, int inBacklog)
{
	try {
		new SC_TcpInPort(inWorld, inPort, inMaxConnections, inBacklog);
	} catch (std::exception& exc) {
		scprintf("Exception in World_OpenTCP: %s\n", exc.what());
	} catch (...) {
	}
}

void World_WaitForQuit(struct World *inWorld)
{
	try {
		inWorld->hw->mQuitProgram->Acquire();
		World_Cleanup(inWorld);
	} catch (std::exception& exc) {
		scprintf("Exception in World_WaitForQuit: %s\n", exc.what());
	} catch (...) {
	}
}

void World_SetSampleRate(World *inWorld, double inSampleRate)
{
	inWorld->mSampleRate = inSampleRate;
	Rate_Init(&inWorld->mFullRate, inSampleRate, inWorld->mBufLength);
	Rate_Init(&inWorld->mBufRate, inSampleRate / inWorld->mBufLength, 1);
}

////////////////////////////////////////////////////////////////////////////////

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
	if (inID == -1) inID = inWorld->hw->mRecentID;
	return inWorld->hw->mNodeLib->Get(inID);
}

Graph* World_GetGraph(World *inWorld, int32 inID)
{
	if (inID == -1) inID = inWorld->hw->mRecentID;
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
	inWorld->mRunning = true;
}

void World_Cleanup(World *world)
{
	if (!world) return;
	
	HiddenWorld *hw = world->hw;
	
	if (hw) hw->mAudioDriver->Stop();
	
	world->mRunning = false;

	if (world->mTopGroup) Group_DeleteAll(world->mTopGroup);
	
	if (hw) {
		free(hw->mWireBufSpace);
		delete hw->mAudioDriver;
	}
	delete world->mNRTLock;
	World_Free(world, world->mTopGroup);
	
	for (int i=0; i<world->mNumSndBufs; ++i) {
		SndBuf *nrtbuf = world->mSndBufsNonRealTimeMirror + i;
		SndBuf * rtbuf = world->mSndBufs + i;
		
		if (nrtbuf->data) free(nrtbuf->data);
		if (rtbuf->data && rtbuf->data != nrtbuf->data) free(rtbuf->data);
		
		if (nrtbuf->sndfile) sf_close(nrtbuf->sndfile);
		if (rtbuf->sndfile && rtbuf->sndfile != nrtbuf->sndfile) sf_close(rtbuf->sndfile);
	}
		
	free(world->mSndBufsNonRealTimeMirror);
	free(world->mSndBufs);
	
	free(world->mControlBusTouched);
	free(world->mAudioBusTouched);
	free(world->mControlBus);
	free(world->mAudioBus);
	delete [] world->mRGen;
	if (hw) {
	
		if (hw->mNRTInputFile) sf_close(hw->mNRTInputFile);
		if (hw->mNRTOutputFile) sf_close(hw->mNRTOutputFile);
		if (hw->mNRTCmdFile) fclose(hw->mNRTCmdFile);
		
		free(hw->mUsers);
		delete hw->mNodeLib;
		delete hw->mGraphDefLib;
		delete hw->mQuitProgram;
		delete hw->mAllocPool;
		free(hw);
	}
	free(world);
}


void World_NRTLock(World *world)
{
	world->mNRTLock->Lock();
}

void World_NRTUnlock(World *world)
{
	world->mNRTLock->Unlock();
}

////////////////////////////////////////////////////////////////////////////////


inline int32 BUFMASK(int32 x)
{
	return (1 << (31 - CLZ(x))) - 1;
}

SCErr bufAlloc(SndBuf* buf, int numChannels, int numFrames)
{		
	long numSamples = numFrames * numChannels;
	
	buf->data = (float*)zalloc(numSamples, sizeof(float));
	if (!buf->data) return kSCErr_Failed;
	
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
		if (len < 5) return 0;
		if (name[4] == '8') return SF_FORMAT_PCM_U8; // uint8
		return 0;
	} else if (name[0] == 'i') {
		if (len < 4) return 0;
		if (name[3] == '8') return SF_FORMAT_PCM_S8;      // int8
		else if (name[3] == '1') return SF_FORMAT_PCM_16; // int16
		else if (name[3] == '2') return SF_FORMAT_PCM_24; // int24
		else if (name[3] == '3') return SF_FORMAT_PCM_32; // int32
	} else if (name[0] == 'f') {
		return SF_FORMAT_FLOAT; // float
	} else if (name[0] == 'd') {
		return SF_FORMAT_DOUBLE; // double
	} else if (name[0] == 'm' || name[0] == 'u') {
		return SF_FORMAT_ULAW; // mulaw ulaw
	} else if (name[0] == 'a') {
		return SF_FORMAT_ALAW; // alaw
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
		case kNode_Info :
			packet.adds("/n_info");
			break;
	}
	if (mIsGroup) {
		packet.maketags(8);
		packet.addtag(',');
		packet.addtag('i');
		packet.addtag('i');
		packet.addtag('i');
		packet.addtag('i');
		packet.addtag('i');
		packet.addtag('i');
		packet.addtag('i');
		packet.addi(mNodeID);
		packet.addi(mGroupID);
		packet.addi(mPrevNodeID);
		packet.addi(mNextNodeID);
		packet.addi(mIsGroup);
		packet.addi(mHeadID);
		packet.addi(mTailID);
	} else {
		packet.maketags(6);
		packet.addtag(',');
		packet.addtag('i');
		packet.addtag('i');
		packet.addtag('i');
		packet.addtag('i');
		packet.addtag('i');
		packet.addi(mNodeID);
		packet.addi(mGroupID);
		packet.addi(mPrevNodeID);
		packet.addi(mNextNodeID);
		packet.addi(mIsGroup);
	}
	
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

void SetPrintFunc(PrintFunc func)
{
	gPrint = func;
}


int scprintf(const char *fmt, ...)
{
	va_list vargs;
	va_start(vargs, fmt); 
	
	if (gPrint) return (*gPrint)(fmt, vargs);
	else return vprintf(fmt, vargs);
}
