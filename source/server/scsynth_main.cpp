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
	printf(
		"supercollider_synth  options:\n"
		"   -u <udp-port-number>    a port number 0-65535\n"
		"   -t <tcp-port-number>    a port number 0-65535\n"
		//"   -v <name-of-output-device>         a device name\n"
		"   -c <number-of-control-bus-channels> (default %d)\n"
		"   -a <number-of-audio-bus-channels>   (default %d)\n"
		"   -i <number-of-input-bus-channels>   (default %d)\n"
		"   -o <number-of-output-bus-channels>  (default %d)\n"
		"   -z <block-size>                     (default %d)\n"
		"   -b <number-of-sample-buffers>       (default %d)\n"
		"   -n <max-number-of-nodes>            (default %d)\n"
		"   -d <max-number-of-synth-defs>       (default %d)\n"
		"   -m <real-time-memory-size>          (default %d)\n"
		"   -w <number-of-wire-buffers>         (default %d)\n"
		"   -l <max-logins>                  (default %d)\n"
		"          maximum number of named return addresses stored\n"
		"          also maximum number of tcp connections accepted\n"
		"   -p <session-password>\n"
		"          When using TCP, the session password must be the first command sent.\n"
		"          The default is no password.\n"
		"          UDP ports never require passwords, so for security use TCP.\n"
		"\nTo quit, send a 'quit' command via UDP or TCP, or press ctrl-C.\n\n",
		kDefaultWorldOptions.mNumAudioBusChannels, 
		kDefaultWorldOptions.mNumInputBusChannels,
		kDefaultWorldOptions.mNumOutputBusChannels,
		kDefaultWorldOptions.mNumControlBusChannels,
		kDefaultWorldOptions.mBufLength,
		kDefaultWorldOptions.mNumBuffers,
		kDefaultWorldOptions.mMaxNodes,
		kDefaultWorldOptions.mMaxGraphDefs,
		kDefaultWorldOptions.mRealTimeMemorySize,
		kDefaultWorldOptions.mMaxWireBufs,
		kDefaultWorldOptions.mMaxLogins
	);
	exit(0);
}

int main(int argc, char* argv[]);
int main(int argc, char* argv[])
{	
	
    setlinebuf(stdout);
	
	int udpPortNum = -1;
	int tcpPortNum = -1;

	WorldOptions options = kDefaultWorldOptions;
	
	for (int i=1; i<argc; i+=2) {
		if (argv[i][0] != '-' || argv[i][1] == 0 || strchr("utaioczblndpmw", argv[i][1]) == 0) {
			printf("ERROR: Invalid option %s\n", argv[i]);
			Usage();
		}
		if (i+1 >= argc) {
			printf("ERROR: Argument expected after option %s\n", argv[i]);
			Usage();
		}
		switch (argv[i][1]) {
			case 'u' :
				udpPortNum = atoi(argv[i+1]);
				break;
			case 't' :
				tcpPortNum = atoi(argv[i+1]);
				break;
			case 'a' :
				options.mNumAudioBusChannels = atoi(argv[i+1]);
				break;
			case 'i' :
				options.mNumInputBusChannels = atoi(argv[i+1]);
				break;
			case 'o' :
				options.mNumOutputBusChannels = atoi(argv[i+1]);
				break;
			case 'c' :
				options.mNumControlBusChannels = atoi(argv[i+1]);
				break;
			case 'z' :
				options.mBufLength = NEXTPOWEROFTWO(atoi(argv[i+1]));
				break;
			case 'b' :
				options.mNumBuffers = NEXTPOWEROFTWO(atoi(argv[i+1]));
				break;
			case 'l' :
				options.mMaxLogins = NEXTPOWEROFTWO(atoi(argv[i+1]));
				break;
			case 'n' :
				options.mMaxNodes = NEXTPOWEROFTWO(atoi(argv[i+1]));
				break;
			case 'd' :
				options.mMaxGraphDefs = NEXTPOWEROFTWO(atoi(argv[i+1]));
				break;
			case 'p' :
				options.mPassword = argv[i+1];
				break;		
			case 'm' :
				options.mRealTimeMemorySize = atoi(argv[i+1]);
				break;		
			case 'w' :
				options.mMaxWireBufs = atoi(argv[i+1]);
				break;		
			default: Usage();
		}
	}
	if (udpPortNum == -1 && tcpPortNum == -1) {
		printf("ERROR: There must be a -u and/or a -t option.\n");
		Usage();
	}
	if (options.mNumInputBusChannels + options.mNumOutputBusChannels > options.mNumAudioBusChannels) {
		printf("ERROR: number of audio bus channels < inputs + outputs.\n");
		Usage();
	}
	
	struct World *world = World_New(&options);
	if (!world) return 1;
	
	if (udpPortNum >= 0) World_OpenUDP(world, udpPortNum);
	if (tcpPortNum >= 0) World_OpenTCP(world, tcpPortNum, options.mMaxLogins, 8);
	
	printf("SuperCollider 3 server ready..\n");
	fflush(stdout);
	
	World_WaitForQuit(world);
	    	    
	return 0;
}

