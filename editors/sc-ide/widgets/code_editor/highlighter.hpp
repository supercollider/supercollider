/*
    SuperCollider Qt IDE
    Copyright (c) 2012 Jakob Leben & Tim Blechmann
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

#include "../../core/sc_lexer.hpp"

#include <QSyntaxHighlighter>
#include <QVector>

namespace ScIDE {

namespace Settings {
class Manager;
class Theme;
}

class Main;

enum SyntaxFormat {
    WhitespaceFormat,
    PlainFormat,
    ClassFormat,
    KeywordFormat,
    BuiltinFormat,
    PrimitiveFormat,
    SymbolFormat,
    StringFormat,
    CharFormat,
    NumberFormat,
    EnvVarFormat,
    CommentFormat,

    FormatCount
};

class SyntaxHighlighterGlobals : public QObject {
    Q_OBJECT

public:
    SyntaxHighlighterGlobals(Main*, Settings::Manager* settings);

    inline const QTextCharFormat* formats() const { return mFormats; }

    inline const QTextCharFormat& format(SyntaxFormat type) const { return mFormats[type]; }

    inline static const SyntaxHighlighterGlobals* instance() {
        Q_ASSERT(mInstance);
        return mInstance;
    }

public Q_SLOTS:
    void applySettings(Settings::Manager*);

Q_SIGNALS:
    void syntaxFormatsChanged();

private:
    friend class SyntaxHighlighter;

    void applySettings(Settings::Manager* s, const QString& key, SyntaxFormat);

    QTextCharFormat mFormats[FormatCount];

    static SyntaxHighlighterGlobals* mInstance;
};

class SyntaxHighlighter : public QSyntaxHighlighter {
    Q_OBJECT

public:
    SyntaxHighlighter(QTextDocument* parent = 0);

private:
    void highlightBlock(const QString& text);
    void highlightBlockInCode(ScLexer& lexer);
    void highlightBlockInString(ScLexer& lexer);
    void highlightBlockInSymbol(ScLexer& lexer);
    void highlightBlockInComment(ScLexer& lexer);

    const SyntaxHighlighterGlobals* mGlobals;
};

}
