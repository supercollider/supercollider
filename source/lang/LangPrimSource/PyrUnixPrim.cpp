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

Primitives for Unix.

*/

#include "PyrPrimitive.h"
#include "PyrObject.h"
#include "PyrKernel.h"
#include "VMGlobals.h"
#include "SC_RGen.h"
#include <unistd.h>
#include <errno.h>
#include <pthread.h>

int prString_System(struct VMGlobals *g, int numArgsPushed);
int prString_System(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a = g->sp;

	char cmdline[1024];
	int err = slotStrVal(a, cmdline, 1023);
	if (err) return err;
	
	int res = system(cmdline);
	SetInt(a, res);
	
	return errNone;
}

void* string_popen_thread_func(void *data);
void* string_popen_thread_func(void *data)
{
        printf("->popen\n");
    char *cmdline = (char*)data;

    FILE *stream = popen(cmdline, "r");
    free(cmdline);
    if (!stream) return 0;

    int err = setvbuf(stream, 0, _IONBF, 0);
    printf("err %d\n", err);
//    setbuf(stream, 0);
//    printf("err %d\n", err);
    
    char buf[1024];
    
    while (true) {
        //printf("->fgets\n");
        char *string = fgets(buf, 1024, stream);
        //printf("<-fgets %d\n", string ? strlen(string) : -1);
        if (!string) break;
        postText(string, strlen(string));
    }
    int res = pclose(stream);
    post("RESULT = %d\n", res);
    return 0;
}   

int prString_POpen(struct VMGlobals *g, int numArgsPushed);
int prString_POpen(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a = g->sp;
	
        if (!isKindOfSlot(a, class_string)) return errWrongType;
        char *cmdline = (char*)malloc(a->uo->size + 1);
 	int err = slotStrVal(a, cmdline, a->uo->size + 1);
	if (err) return err;
      
        pthread_t thread;
        pthread_create(&thread, NULL, string_popen_thread_func, (void*)cmdline);
 	pthread_detach(thread);
	return errNone;
}

int prUnix_Errno(struct VMGlobals *g, int numArgsPushed);
int prUnix_Errno(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a = g->sp;

	SetInt(a, errno);
	
	return errNone;
}



void initUnixPrimitives();
void initUnixPrimitives()
{
	int base, index = 0;
		
	base = nextPrimitiveIndex();
	
	definePrimitive(base, index++, "_String_System", prString_System, 1, 0);
	definePrimitive(base, index++, "_String_POpen", prString_POpen, 1, 0);
	definePrimitive(base, index++, "_Unix_Errno", prUnix_Errno, 1, 0);
}


