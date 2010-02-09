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


#ifndef _SC_Errors_
#define _SC_Errors_

typedef int SCErr;

const char *SC_ErrorString(SCErr err);

enum {
	kSCErr_None,
	kSCErr_Failed,
	kSCErr_NodeNotFound,
	kSCErr_TargetNodeNotFound,
	kSCErr_GroupNotFound,
	kSCErr_SynthDefNotFound,
	kSCErr_NoSuchCommand,
	kSCErr_WrongArgType,
	kSCErr_IndexOutOfRange,
	kSCErr_AccessDenied,
	kSCErr_NoReplyPort,
	kSCErr_InvalidControlIndex,
	kSCErr_AlreadyLoggedIn,
	kSCErr_NotLoggedIn,
	kSCErr_TooManyUsers,
	kSCErr_TooManyNodes,
	kSCErr_DuplicateNodeID,
	kSCErr_ReservedNodeID,
	kSCErr_OutOfRealTimeMemory,

	kSCErr_UnsupportedHeaderFormat,
	kSCErr_UnsupportedSampleFormat,

	kSCErr_BufGenNotFound,

	kSCErr_NumErrors
};

#endif
