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

#include "PyrSymbol.h"
#include "PyrObject.h"
#include "InitAlloc.h"
#include <stdarg.h>


static FILE *postfile = stdout;

void setPostFile(FILE *file) { postfile = file; }


void postfl(const char *fmt, ...);
void postfl(const char *fmt, ...)
{
	va_list vargs;
	va_start(vargs, fmt); 
	vfprintf(postfile, fmt, vargs);
        fflush(postfile);
}

extern "C" {
	int vpost(const char *fmt, va_list vargs);
}

int vpost(const char *fmt, va_list vargs)
{
	vfprintf(postfile, fmt, vargs);
        fflush(postfile);
}

void post(const char *fmt, ...);
void post(const char *fmt, ...)
{
	va_list vargs;
	va_start(vargs, fmt); 
	vfprintf(postfile, fmt, vargs);
    //fflush(postfile);
}

void error(const char *fmt, ...);
void error(const char *fmt, ...)
{
	fprintf(postfile, "ERROR: ");
	va_list vargs;
	va_start(vargs, fmt); 
	vfprintf(postfile, fmt, vargs);
        fflush(postfile);
}

void postText(const char *text, long length);
void postText(const char *text, long length)
{
	fwrite(text, sizeof(char), length, postfile);
}

void postChar(char c);
void postChar(char c)
{	
	fputc(c, postfile);
}

void flushPostBuf();
void flushPostBuf()
{
	fflush(postfile);
}

long scMIDIout(int port, int len, int statushi, int chan, int data1, int data2);
long scMIDIout(int port, int len, int statushi, int chan, int data1, int data2)
{
	return 0;
}

/*
void tellPlugInsAboutToCompile();
void tellPlugInsAboutToCompile()
{
}

void tellPlugInsAboutToRun();
void tellPlugInsAboutToRun()
{
}
*/

void closeAllGUIScreens();
void closeAllGUIScreens()
{
}

void initGUI();
void initGUI()
{
}

void initGUIPrimitives();
void initGUIPrimitives()
{
}

void initSCViewPrimitives();
void initSCViewPrimitives()
{
}

void initCocoaFilePrimitives();
void initCocoaFilePrimitives()
{
}

// CR ADDED
void initRendezvousPrimitives();
void initRendezvousPrimitives()
{
}

bool compileLibrary();
void runLibrary(PyrSymbol* selector); 

int main();
int main()
{
	
	pyr_init_mem_pools(2*1024*1024, 256*1024);
	init_OSC(57120);
	schedInit();

	compileLibrary();
	runLibrary(s_run); fflush(postfile);
                
	return 0;
}



