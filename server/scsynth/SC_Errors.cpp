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


#include "SC_Errors.h"

const char *SC_ErrorString(SCErr err)
{
	switch (err) {
			case kSCErr_None : return "none";
			case kSCErr_Failed : return "failed";
			case kSCErr_NodeNotFound : return "Node not found";
			case kSCErr_TargetNodeNotFound : return "target Node not found";
			case kSCErr_GroupNotFound : return "Group not found";
			case kSCErr_SynthDefNotFound : return "SynthDef not found";
			case kSCErr_NoSuchCommand : return "no such command";
			case kSCErr_WrongArgType : return "wrong argument type";
			case kSCErr_IndexOutOfRange : return "index out of range";
			case kSCErr_AccessDenied : return "access denied";
			case kSCErr_NoReplyPort : return "no reply port";
			case kSCErr_InvalidControlIndex : return "invalid control index";

			case kSCErr_AlreadyLoggedIn : return "already logged in";
			case kSCErr_NotLoggedIn : return "not logged in";
			case kSCErr_TooManyUsers : return "too many users";
			case kSCErr_TooManyNodes : return "too many nodes";
			case kSCErr_DuplicateNodeID : return "duplicate node ID";
			case kSCErr_ReservedNodeID : return "negative node IDs are reserved";
			case kSCErr_OutOfRealTimeMemory : return "out of real time memory";

			case kSCErr_UnsupportedHeaderFormat : return "unsupported header format";
			case kSCErr_UnsupportedSampleFormat : return "unsupported sample format";

			case kSCErr_BufGenNotFound : return "buf gen routine not found";

			default : return "unknown error";
	}
}


