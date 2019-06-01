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

#include "stdio.h"

int gMissingNodeID;

void SC_ErrorString(SCErr err, char returnString[]) {
    switch (err) {
    case kSCErr_None:
        sprintf(returnString, "none");
        break;
    case kSCErr_Failed:
        sprintf(returnString, "failed");
        break;
    case kSCErr_NodeNotFound:
        sprintf(returnString, "Node %i not found", gMissingNodeID);
        break;
    case kSCErr_TargetNodeNotFound:
        sprintf(returnString, "target Node %i not found", gMissingNodeID);
        break;
    case kSCErr_GroupNotFound:
        sprintf(returnString, "Group %i not found", gMissingNodeID);
        break;
    case kSCErr_SynthDefNotFound:
        sprintf(returnString, "SynthDef not found");
        break;
    case kSCErr_NoSuchCommand:
        sprintf(returnString, "no such command");
        break;
    case kSCErr_WrongArgType:
        sprintf(returnString, "wrong argument type");
        break;
    case kSCErr_IndexOutOfRange:
        sprintf(returnString, "index out of range");
        break;
    case kSCErr_AccessDenied:
        sprintf(returnString, "access denied");
        break;
    case kSCErr_NoReplyPort:
        sprintf(returnString, "no reply port");
        break;
    case kSCErr_InvalidControlIndex:
        sprintf(returnString, "invalid control index");
        break;

    case kSCErr_AlreadyLoggedIn:
        sprintf(returnString, "already logged in");
        break;
    case kSCErr_NotLoggedIn:
        sprintf(returnString, "not logged in");
        break;
    case kSCErr_TooManyUsers:
        sprintf(returnString, "too many users");
        break;
    case kSCErr_TooManyNodes:
        sprintf(returnString, "too many nodes");
        break;
    case kSCErr_DuplicateNodeID:
        sprintf(returnString, "duplicate node ID");
        break;
    case kSCErr_ReservedNodeID:
        sprintf(returnString, "negative node IDs are reserved");
        break;
    case kSCErr_ReplaceRootGroup:
        sprintf(returnString, "can't replace group 0");
        break;
    case kSCErr_OutOfRealTimeMemory:
        sprintf(returnString, "out of real time memory");
        break;

    case kSCErr_UnsupportedHeaderFormat:
        sprintf(returnString, "unsupported header format");
        break;
    case kSCErr_UnsupportedSampleFormat:
        sprintf(returnString, "unsupported sample format");
        break;

    case kSCErr_BufGenNotFound:
        sprintf(returnString, "buf gen routine not found");
        break;

    default:
        sprintf(returnString, "unknown error");
    }
}
