/*
    SuperCollider Qt IDE
    Copyright (c) 2026 Jordan Henderson
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

#include <QString>

namespace ScIDE {

class Document;

// Formats with langutils' sc_format, see langutils/sc_format/README.md.
namespace CodeFormatter {

// The indentation the editor uses, for the IDE's own options file when it is made.
struct Indentation {
    bool spaces;
    int width;
};

struct Result {
    enum Status {
        Formatted, // text is the whole document
        Unparsed, // a line of the range is left as written around a parse error
        Failed // message says why
    };
    Status status;
    QString text;
    QString message;
    bool keepsIndent; // the options leave indentation as written
};

// Formats lines first to last of the document's text, counting from 0, with the document's options file. Without
// rewrites, only whitespace changes, not tokens.
Result format(Document*, const QString& text, int firstLine, int lastLine, Indentation, bool rewrites);

// The nearest `.sc-format.scd` in the document's directory or above it, or empty if there is none or the document
// isn't saved.
QString optionsFileFor(Document*);

// The options file for documents without one. Made with the editor's indentation if missing, empty if it can't be.
QString defaultOptionsFile(Indentation);

// Makes an options file listing every option, commented out. Returns why it couldn't, or empty.
QString createOptionsFile(const QString& path);

}

}
