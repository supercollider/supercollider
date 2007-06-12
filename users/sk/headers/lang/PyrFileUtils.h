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
/*

Some utils for file i/o.

*/

#ifndef _PYRFILEUTIL_H_
#define _PYRFILEUTIL_H_

int headerFormatFromSymbol(struct PyrSymbol *inSymbol);
int sampleFormatFromSymbol(struct PyrSymbol *inSymbol, int inHeaderFormat);

#ifdef __MAC__

#include <Files.h>

long setTypeCreator(unsigned char *filename, long type, long creator);
bool filelen(FILE *file, size_t *length);

int allocasync(int fildes, int count, IOParam *pb, IOCompletionUPP completionFunc);
int createasync(unsigned char *path, int oflag, HParamBlockRec *hpb, IOCompletionUPP completionFunc);
int openasync(unsigned char *path, int oflag, HParamBlockRec *hpb, IOCompletionUPP completionFunc);
int closeasync(int fildes, HParamBlockRec *hpb, IOCompletionUPP completionFunc);
int writeasync(int fildes, const char *buf, int count, IOParam *pb, IOCompletionUPP completionFunc);
int readasync(int fildes, char *buf, int count, IOParam *pb, IOCompletionUPP completionFunc);
int seekwriteasync(int fildes, const char *buf, int count, int pos, IOParam *pb, IOCompletionUPP completionFunc);
int seekreadasync(int fildes, char *buf, int count, int pos, IOParam *pb, IOCompletionUPP completionFunc);


#endif
#endif
