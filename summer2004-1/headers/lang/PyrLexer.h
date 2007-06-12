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


#ifndef _PYRLEXER_H_
#define _PYRLEXER_H_

#include "PyrSymbol.h"

extern int charno, lineno, linepos;
extern int *linestarts;

struct ClassExtFile {
	struct ClassExtFile *next;
	PyrSymbol *fileSym;
};

typedef struct classdep {
	struct classdep *next;
	struct classdep *superClassDep;
	struct classdep *subclasses;
	PyrSymbol *className;
	PyrSymbol *superClassName;
	PyrSymbol *fileSym;
} ClassDependancy;

extern PyrSymbol *gCompilingFileSym;

ClassDependancy* newClassDependancy(PyrSymbol *className, PyrSymbol *superClassName, 
	PyrSymbol *fileSym);
bool parseOneClass(PyrSymbol *fileSym);
void initPassOne();
void finiPassOne();
bool passOne();
void buildDepTree();
void traverseFullDepTree();
void traverseDepTree(ClassDependancy *classdep, int level);
void traverseFullDepTree2();
void traverseDepTree2(ClassDependancy *classdep, int level);
void compileClassExtensions();
void compileFileSym(PyrSymbol *fileSym);

void runLibrary(PyrSymbol* selector);

void interpretCmdLine(const char *textbuf, int textlen, char *methodname);


int input();
int input0();
void unput(int c);
void unput0(int c);

void finiLexer() ;
bool startLexer(char* filename) ;
void startLexerCmdLine(char *textbuf, int textbuflen);
int yylex() ;
void yyerror(char *s) ;
void fatal() ;
bool isValidSourceFileName(char *filename);
bool passOne_ProcessOneFile(char *filename, int level);

extern void asRelativePath(char *inPath,char *outPath);

void initLexer();
void init_SuperCollider();

int processbinop(char *token);
int processident(char *token);
int processfloat(char *token, int sawpi);
int processint(char *token);
int processchar(int c);
int processintradix(char *s);
int processfloatradix(char *s);
int processhex(char *s);
int processsymbol(char *token);
int processstring(char *token);
int processkeywordbinop(char *token);

void postErrorLine(int linenum, int start, int charpos);
bool scanForClosingBracket();
void parseClasses();

extern int parseFailed;
extern bool compilingCmdLine;
extern bool compilingCmdLineErrorWindow;
extern bool compiledOK;

#define MAXYYLEN 8192

extern int gNumCompiledFiles;
extern int gClassCompileOrderNum;
extern ClassDependancy **gClassCompileOrder;
extern char curfilename[PATH_MAX];

extern int runcount;

extern char *binopchars;
extern char yytext[MAXYYLEN];
extern char linebuf[256];
extern char curfilename[PATH_MAX];

extern int yylen;
extern int lexCmdLine;
extern bool compilingCmdLine;
extern bool compilingCmdLineErrorWindow;
extern long zzval;

extern int lineno, charno, linepos;
extern int *linestarts;
extern int maxlinestarts;

extern char *text;
extern int textlen;
extern int textpos;
extern int parseFailed;
extern bool compiledOK;
extern int radixcharpos, decptpos;

#endif
