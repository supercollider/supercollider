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

#include "SCBase.h"
#include "PyrSymbol.h"
#include "SC_Export.h"
#include <stdexcept>

// Note: this file is used to implement the bindings from sc_lexer to the requirements of the bison parser.
// If you need to lex sc code, but don't need to compile it, you should use sc_lexer instead.

// Called from inside the bison parser.
int yylex();

// Called from inside the bison parser when an error occurs.
void yyerror(const char* s);

/// Often the result of a gc error, used to represent any unrecoverable state.
struct FatalInterpreterError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

// MAIN ENTRY POINTS OF SC LANG.

// All exceptions are caught, except FatalInterpreterErrors, which are printed and rethrown.
SCLANG_DLLEXPORT_C bool compileLibrary(bool was_compiled_previously, bool standalone);
SCLANG_DLLEXPORT_C void shutdownLibrary(bool was_compiled_previously);
SCLANG_DLLEXPORT_C void runLibrary(PyrSymbol* selector);
SCLANG_DLLEXPORT_C void setCommandLine(const char* txt, size_t txtSize, const char* filePath, int lineNumber,
                                       int column);
