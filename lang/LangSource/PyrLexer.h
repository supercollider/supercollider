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


#pragma once

#include "PyrObject.h"
#include "PyrSlot.h"
#include "PyrSymbol.h"
#include "SC_Export.h"
#include "SCBase.h"
#include <filesystem>

extern int characterIndexInCurrentLine, currentLineCount, startOfCurrentLineCharacterIndex;
extern int* linestarts;

struct ClassExtFile {
    struct ClassExtFile* next;
    PyrSymbol* fileSym;
    int startPos, endPos, lineOffset;
};

typedef struct classdep {
    struct classdep* next;
    struct classdep* superClassDep;
    struct classdep* subclasses;
    PyrSymbol* className;
    PyrSymbol* superClassName;
    PyrSymbol* fileSym;
    int startPos, endPos, lineOffset;
} ClassDependancy;

extern PyrSymbol* gCompilingFilenameSym;

ClassDependancy* newClassDependancy(PyrSymbol* className, PyrSymbol* superClassName, PyrSymbol* fileSym, int startPos,
                                    int endPos, int lineOffset);
bool parseOneClass(PyrSymbol* fileSym);
void initPassOne();
void finiPassOne();
bool passOne();
void buildDepTree();
void traverseFullDepTree();
void traverseDepTree(ClassDependancy* classdep, int level);
void traverseFullDepTree2();
void traverseDepTree2(ClassDependancy* classdep, int level);
void compileClassExtensions();
void compileClass(PyrSymbol* fileSym, int startPos, int endPos, int lineOffset);


struct FatalInterpreterError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

// All exceptions are caught, except FatalInterpreterErrors
SCLANG_DLLEXPORT_C void runLibrary(PyrSymbol* selector);

void interpretCmdLine(const char* textbuf, int textlen, char* methodname);


struct EmptyLexerResult {};

// This is used in the parser and will become the type of yyloc, the location data for each token.
struct LocationType {
    // TODO: consider making these small types.
    // absolute must be large, but the other two could be smaller, perhaps uint16_t?
    struct Part {
        int absolute, lineNumber, offsetInLine;
    };
    using Begin = Part;
    using End = Part;

    Begin begin;
    End end;

    [[nodiscard]] constexpr static LocationType range(LocationType left, LocationType right) {
        return {
            left.begin,
            right.end,
        };
    }
};

// This macro defines the default rule for how to combine location types. It is used in the parser.
#define YYLLOC_DEFAULT(Current, Rhs, N)                                                                                \
    do                                                                                                                 \
        if ((N) == 0) {                                                                                                \
            (Current) = YYRHSLOC(Rhs, 0);                                                                              \
        } else {                                                                                                       \
            (Current) = LocationType::range(YYRHSLOC(Rhs, 1), YYRHSLOC(Rhs, N));                                       \
        }                                                                                                              \
    while (0)


int input();
int input0();
void unput(int c);
void unput0(int c);

void finiLexer();
bool startLexer(char* filename);
void startLexerCmdLine(char* textbuf, int textbuflen);
int yylex();
void yyerror(const char* s);
void fatal();
bool isValidSourceFileName(const std::filesystem::path& path);
bool passOne_ProcessOneFile(const std::filesystem::path& path);

std::filesystem::path relativeToCompileDir(const std::filesystem::path&);

void initLexer();

int processfloat(char* token, int sawpi);
int processint(char* token);
int processchar(int c);

void postErrorLine(int linenum, int start, int charpos);
bool scanForClosingBracket();
void parseClasses();

extern int parseFailed;
extern bool compilingCmdLine;
extern bool compilingCmdLineErrorWindow;
extern bool compiledOK;

#define MAXYYLEN 81920

extern int gNumCompiledFiles;
extern int gClassCompileOrderNum;
extern ClassDependancy** gClassCompileOrder;
extern char curfilename[PATH_MAX];

extern int runcount;

extern const char* binopchars;
extern char yytext[MAXYYLEN];
extern char curfilename[PATH_MAX];

extern int yylen;
extern int lexCmdLine;
extern bool compilingCmdLine;
extern bool compilingCmdLineErrorWindow;
extern intptr_t gParserResult;

extern int currentLineCount, characterIndexInCurrentLine, startOfCurrentLineCharacterIndex;
extern int* linestarts;
extern int maxlinestarts;

extern char* text;

struct PyrString;
extern PyrString* gCurrentCompilingTextAsSCString;

extern int textlen;
extern int textpos;
extern int parseFailed;
extern bool compiledOK;
extern int radixcharpos, decptpos;


int rtf2txt(char* txt);
int html2txt(char* txt);
