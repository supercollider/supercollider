//   SuperCollider real time audio synthesis system
//     Copyright (c) 2002 James McCartney. All rights reserved.
//   http://www.audiosynth.com

//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.

//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.

//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the Free Software
//   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA

// Contains the most common definitions.

#ifndef _SCBASE_
#define _SCBASE_

#include <limits.h>
#include <stdio.h>
#ifdef _WIN32
# include <stdlib.h>
# define PATH_MAX _MAX_PATH
#endif

#include "SC_BoundsMacros.h"
#include "SC_Types.h"
#include "PyrErrors.h"
#include "AllocPools.h"
#include "SC_Export.h"

void postfl(const char *fmt, ...);
void post(const char *fmt, ...);
void error(const char *fmt, ...);
void postText(const char *text, long length);
void postChar(char c);
void flushPostBuf();
void setPostFile(FILE *file); // If file is not NULL, causes all posted text to also be written to the file.

void debugf(char *fmt, ...);
void pprintf(unsigned char *str, char *fmt, ...);

SC_DLLEXPORT_C void schedInit();
SC_DLLEXPORT_C void init_OSC(int port);
SC_DLLEXPORT_C bool pyr_init_mem_pools(int runtime_space, int runtime_grow);

SC_DLLEXPORT_C void schedRun();
SC_DLLEXPORT_C void schedStop();
SC_DLLEXPORT_C bool compileLibrary();
SC_DLLEXPORT_C void runLibrary(struct PyrSymbol* selector);
SC_DLLEXPORT_C void runInterpreter(struct VMGlobals *g, struct PyrSymbol *selector, int numArgsPushed);

SC_DLLEXPORT_C struct VMGlobals* scGlobals();

SC_DLLEXPORT_C struct PyrSymbol* getsym(const char *inName);
SC_DLLEXPORT_C struct PyrSymbol* getmetasym(const char *name);
SC_DLLEXPORT_C struct PyrSymbol* findsym(const char *name);

#endif
