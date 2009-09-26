/*
	SuperColliderAU Copyright (c) 2006 Gerard Roma.

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

#ifndef _SCProcess_
#define _SCProcess_

#include "SC_WorldOptions.h"
#include "SC_World.h"
#include "SC_HiddenWorld.h"

#include "Resources.h"
#include "OSCMessages.h"

#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>

#include "SC_CoreAudio.h"
#include "SC_AU.h"

class SCProcess {

public:
	SCProcess();
    CFStringRef synthName;
    int portNum;
	void startUp(WorldOptions options, CFStringRef pluginsPath,  CFStringRef synthdefsPath, int preferredPort);
	void makeSynth();
	void sendParamChangeMessage(CFStringRef name, float value);
	void sendNote(int64 oscTime, int note, int velocity);
    void sendTick(int64 oscTime, int bus);
    void quit();
    void run(const AudioBufferList* in, AudioBufferList* out, UInt32 inFramesToProcess,  AudioTimeStamp inTimeStamp, Float64 sampleRate,int64 oscTime);

private:
    World* world;
    int findNextFreeUdpPort(int startNum);

};

#endif