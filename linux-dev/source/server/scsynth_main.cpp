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


#include "SC_WorldOptions.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <math.h>

#include "clz.h"
#include <stdexcept>
#include <sys/wait.h>

void Usage();
void Usage()
{
	scprintf(
		"supercollider_synth  options:\n"
		"   -u <udp-port-number>    a port number 0-65535\n"
		"   -t <tcp-port-number>    a port number 0-65535\n"
		"   -c <number-of-control-bus-channels> (default %d)\n"
		"   -a <number-of-audio-bus-channels>   (default %d)\n"
		"   -i <number-of-input-bus-channels>   (default %d)\n"
		"   -o <number-of-output-bus-channels>  (default %d)\n"
		"   -z <block-size>                     (default %d)\n"
		"   -Z <hardware-buffer-size>           (default %d)\n"
		"   -b <number-of-sample-buffers>       (default %d)\n"
		"   -n <max-number-of-nodes>            (default %d)\n"
		"   -d <max-number-of-synth-defs>       (default %d)\n"
		"   -m <real-time-memory-size>          (default %d)\n"
		"   -w <number-of-wire-buffers>         (default %d)\n"
		"   -r <number-of-random-seeds>         (default %d)\n"
		"   -l <max-logins>                  (default %d)\n"
		"          maximum number of named return addresses stored\n"
		"          also maximum number of tcp connections accepted\n"
		"   -p <session-password>\n"
		"          When using TCP, the session password must be the first command sent.\n"
		"          The default is no password.\n"
		"          UDP ports never require passwords, so for security use TCP.\n"
		"   -N <cmd-filename> <input-filename> <output-filename> <sample-rate> <header-format> <sample-format>\n"
		"\nTo quit, send a 'quit' command via UDP or TCP, or press ctrl-C.\n\n",
		kDefaultWorldOptions.mNumControlBusChannels,
		kDefaultWorldOptions.mNumAudioBusChannels, 
		kDefaultWorldOptions.mNumInputBusChannels,
		kDefaultWorldOptions.mNumOutputBusChannels,
		kDefaultWorldOptions.mBufLength,
		kDefaultWorldOptions.mPreferredHardwareBufferFrameSize,
		kDefaultWorldOptions.mNumBuffers,
		kDefaultWorldOptions.mMaxNodes,
		kDefaultWorldOptions.mMaxGraphDefs,
		kDefaultWorldOptions.mRealTimeMemorySize,
		kDefaultWorldOptions.mMaxWireBufs,
		kDefaultWorldOptions.mNumRGens,
		kDefaultWorldOptions.mMaxLogins
	);
	exit(1);
}

#define checkNumArgs(n) \
	if (i+n > argc) { \
		if (n == 2) scprintf("ERROR: Argument expected after option %s\n", argv[j]); \
		else scprintf("ERROR: More arguments expected after option %s\n", argv[j]); \
		Usage(); \
	} \
	i += n;
	

int main(int argc, char* argv[]);
int main(int argc, char* argv[])
{	
	
    setlinebuf(stdout);
	
	int udpPortNum = -1;
	int tcpPortNum = -1;

	WorldOptions options = kDefaultWorldOptions;
	
	for (int i=1; i<argc;) {
		if (argv[i][0] != '-' || argv[i][1] == 0 || strchr("utaioczblndpmwZ", argv[i][1]) == 0) {
			scprintf("ERROR: Invalid option %s\n", argv[i]);
			Usage();
		}
		int j = i;
		switch (argv[j][1]) {
			case 'u' :
				checkNumArgs(2);
				udpPortNum = atoi(argv[j+1]);
				break;
			case 't' :
				checkNumArgs(2);
				tcpPortNum = atoi(argv[j+1]);
				break;
			case 'a' :
				checkNumArgs(2);
				options.mNumAudioBusChannels = atoi(argv[j+1]);
				break;
			case 'i' :
				checkNumArgs(2);
				options.mNumInputBusChannels = atoi(argv[j+1]);
				break;
			case 'o' :
				checkNumArgs(2);
				options.mNumOutputBusChannels = atoi(argv[j+1]);
				break;
			case 'c' :
				checkNumArgs(2);
				options.mNumControlBusChannels = atoi(argv[j+1]);
				break;
			case 'z' :
				checkNumArgs(2);
				options.mBufLength = NEXTPOWEROFTWO(atoi(argv[j+1]));
				break;
			case 'Z' :
				checkNumArgs(2);
				options.mPreferredHardwareBufferFrameSize = NEXTPOWEROFTWO(atoi(argv[j+1]));
				break;
			case 'b' :
				checkNumArgs(2);
				options.mNumBuffers = NEXTPOWEROFTWO(atoi(argv[j+1]));
				break;
			case 'l' :
				checkNumArgs(2);
				options.mMaxLogins = NEXTPOWEROFTWO(atoi(argv[j+1]));
				break;
			case 'n' :
				checkNumArgs(2);
				options.mMaxNodes = NEXTPOWEROFTWO(atoi(argv[j+1]));
				break;
			case 'd' :
				checkNumArgs(2);
				options.mMaxGraphDefs = NEXTPOWEROFTWO(atoi(argv[j+1]));
				break;
			case 'p' :
				checkNumArgs(2);
				options.mPassword = argv[j+1];
				break;
			case 'm' :
				checkNumArgs(2);
				options.mRealTimeMemorySize = atoi(argv[j+1]);
				break;
			case 'w' :
				checkNumArgs(2);
				options.mMaxWireBufs = atoi(argv[j+1]);
				break;
			case 'r' :
				checkNumArgs(2);
				options.mNumRGens = atoi(argv[j+1]);
				break;
			case 'N' :
// -N cmd-filename input-filename output-filename sample-rate header-format sample-format
				checkNumArgs(7);
				options.mNonRealTimeCmdFilename    = (strcmp(argv[j+1], "_")==0) ? 0 : argv[j+1];
				options.mNonRealTimeInputFilename  = (strcmp(argv[j+2], "_")==0) ? 0 : argv[j+2];
				options.mNonRealTimeOutputFilename = argv[j+3];
				options.mNonRealTimeSampleRate = (int)atof(argv[j+4]);
				options.mNonRealTimeOutputHeaderFormat = argv[j+5];
				options.mNonRealTimeOutputSampleFormat = argv[j+6];
				break;
			default: Usage();
		}
	}
	if (udpPortNum == -1 && tcpPortNum == -1) {
		scprintf("ERROR: There must be a -u and/or a -t option.\n");
		Usage();
	}
	if (options.mNumInputBusChannels + options.mNumOutputBusChannels > options.mNumAudioBusChannels) {
		scprintf("ERROR: number of audio bus channels < inputs + outputs.\n");
		Usage();
	}
	
	struct World *world = World_New(&options);
	if (!world) return 1;
	
	if (udpPortNum >= 0) World_OpenUDP(world, udpPortNum);
	if (tcpPortNum >= 0) World_OpenTCP(world, tcpPortNum, options.mMaxLogins, 8);
	
	scprintf("SuperCollider 3 server ready..\n");
	fflush(stdout);
	
	World_WaitForQuit(world);
	    	    
	return 0;
}

