/*
 *  OSC_Packet.h
 *  SC3synth
 *
 *  Created by James McCartney on Sat Aug 24 2002.
 *  Copyright (c) 2001 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _OSC_Packet_
#define _OSC_Packet_

#include "SC_Reply.h"

struct OSC_Packet
{
	char *mData;
	int32 mSize;
	bool mIsBundle;

	ReplyAddress mReplyAddr;
};

#endif
