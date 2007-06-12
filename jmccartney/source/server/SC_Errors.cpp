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


#include "SC_Errors.h"

const char *SC_ErrorString(SCErr err)
{
	switch (err) {
			case kSCErr_None : return "none";
			case kSCErr_Failed : return "failed";
			case kSCErr_NodeNotFound : return "Node not found";
			case kSCErr_GroupNotFound : return "Group not found";
			case kSCErr_SynthDefNotFound : return "NodeDef not found";
			case kSCErr_BufBusy : return "Buffer operation already pending";
			case kSCErr_NoSuchCommand : return "no such command";
			case kSCErr_WrongArgType : return "wrong argument types";
			case kSCErr_IndexOutOfRange : return "index out of range";
			case kSCErr_AccessDenied : return "access denied";
			case kSCErr_NoReplyPort : return "no reply port";
			case kSCErr_InvalidControlIndex : return "invalid control index";

			case kSCErr_AlreadyLoggedIn : return "already logged in";
			case kSCErr_NotLoggedIn : return "not logged in";
			case kSCErr_TooManyUsers : return "no more users";

			case kSCErr_UnsupportedHeaderFormat : return "unsupported header format";
			case kSCErr_UnsupportedSampleFormat : return "unsupported sample format";

			default : return "unknown error";
	}
}


