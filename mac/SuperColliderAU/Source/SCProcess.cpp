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

#include "SCProcess.h"

void* scThreadFunc(void* arg);

void* scThreadFunc(void* arg)
{
    World* world  = (World*)arg;
    World_WaitForQuit(world);
    return 0;
}

void null_reply_func(struct ReplyAddress* /*addr*/, char* /*msg*/, int /*size*/);

SCProcess::SCProcess()
{
    portNum = 0;
}

int SCProcess::findNextFreeUdpPort(int startNum){
	int server_socket = -1;
	struct sockaddr_in mBindSockAddr;
	int numberOfTries=50;
	int port = startNum;
	if ((server_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		scprintf("failed to create udp socket\n");
		return -1;
	}

	bzero((char *)&mBindSockAddr, sizeof(mBindSockAddr));
	mBindSockAddr.sin_family = AF_INET;
	mBindSockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	mBindSockAddr.sin_port = htons(port);
	const int on = 1;
	setsockopt( server_socket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

	while (bind(server_socket, (struct sockaddr *)&mBindSockAddr, sizeof(mBindSockAddr)) < 0) {
		if(--numberOfTries <0 || (errno != EADDRINUSE)) {
			scprintf("unable to bind udp socket\n");
			return -1;
		}
        port++;
        mBindSockAddr.sin_port = htons(port);
	}
	close(server_socket);
	return port;
}


void SCProcess::startUp(WorldOptions options, CFStringRef pluginsPath,  CFStringRef synthdefsPath,  int preferredPort){

    pthread_t scThread;
    char stringBuffer[PATH_MAX] ;
	OSCMessages messages;

    CFStringGetCString(pluginsPath, stringBuffer, sizeof(stringBuffer), kCFStringEncodingUTF8);
    setenv("SC_PLUGIN_PATH", stringBuffer, 1);
    CFStringGetCString(synthdefsPath, stringBuffer, sizeof(stringBuffer), kCFStringEncodingUTF8);
    setenv("SC_SYNTHDEF_PATH", stringBuffer, 1);
    this->portNum = findNextFreeUdpPort(preferredPort);
    world = World_New(&options);
    //world->mDumpOSC=2;
    if (world) {
        if (this->portNum >= 0) World_OpenUDP(world, this->portNum);
        pthread_create (&scThread, NULL, scThreadFunc, (void*)world);
    }
    if (world->mRunning){
        small_scpacket packet = messages.initTreeMessage();
        World_SendPacket(world, 16, (char*)packet.buf, null_reply_func);
    }
}


void SCProcess::makeSynth(){
    if (world->mRunning){
        OSCMessages messages;
        small_scpacket packet;
        size_t messageSize =  messages.createSynthMessage(&packet, synthName);
        World_SendPacket(world,messageSize,  (char*)packet.buf, null_reply_func);
    }
}

void SCProcess::sendParamChangeMessage(CFStringRef name, float value){
    OSCMessages messages;
    if(synthName){
        small_scpacket packet;
        size_t messageSize =messages.parameterMessage(&packet, name,value);
        World_SendPacket(world, messageSize, (char*)packet.buf, null_reply_func);
    }
}

void SCProcess::sendTick(int64 oscTime, int bus){
    OSCMessages messages;
    small_scpacket packet = messages.sendTickMessage(oscTime, bus);
    World_SendPacket(world, 40, (char*)packet.buf, null_reply_func);

}

void SCProcess::sendNote(int64 oscTime, int note, int velocity){
    OSCMessages messages;
    small_scpacket packet = messages.noteMessage(oscTime, note, velocity);
    World_SendPacket(world, 92, (char*)packet.buf, null_reply_func);
}


void SCProcess::run(const AudioBufferList* in, AudioBufferList* out,  UInt32 inFramesToProcess, AudioTimeStamp inTimeStamp, Float64 sampleRate, int64 oscTime){
    if (world->mRunning){
    	SC_AUAudioDriver* driver = (SC_AUAudioDriver*)this->world->hw->mAudioDriver;
		//AUCallback(driver,(AudioBufferList*)in, out, &inTimeStamp, inFramesToProcess, sampleRate, oscTime );
        driver->Callback(in, out, &inTimeStamp, inFramesToProcess, sampleRate, oscTime );
    }
}

void SCProcess::quit(){
    OSCMessages messages;
    if (world && world->mRunning){
         small_scpacket packet = messages.quitMessage();
         World_SendPacket(world, 8,(char*)packet.buf, null_reply_func);
    }
}
