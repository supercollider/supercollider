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

#ifndef SCIDE_SC_SYNTAX_HIGHLIGHTER_HPP_INCLUDED
#define SCIDE_SC_SYNTAX_HIGHLIGHTER_HPP_INCLUDED

#include "tokens.hpp"

#include <QSyntaxHighlighter>
#include <QVector>

namespace ScIDE {

namespace Settings { class Manager; }

class Main;

enum SyntaxFormat
{
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

struct SyntaxRule
{
    SyntaxRule(): type(Token::Unknown) {}
    SyntaxRule( Token::Type t, const QString &s ): type(t), expr(s) {}

    Token::Type type;
    QRegExp expr;
};

class SyntaxHighlighterGlobals : public QObject
{
    Q_OBJECT

public:
    SyntaxHighlighterGlobals( Main *, Settings::Manager * settings );

    inline const QTextCharFormat * formats() const
    {
        return mFormats;
    }

    inline const QTextCharFormat & format( SyntaxFormat type ) const
    {
        return mFormats[type];
    }

    inline static const SyntaxHighlighterGlobals * instance() { Q_ASSERT(mInstance); return mInstance; }

public Q_SLOTS:
    void applySettings( Settings::Manager * );

Q_SIGNALS:
    void syntaxFormatsChanged();

private:
    friend class SyntaxHighlighter;

    void initSyntaxRules();
    void initKeywords();
    void initBuiltins();
    void applySettings( Settings::Manager*, const QString &key, SyntaxFormat );

    QTextCharFormat mFormats[FormatCount];
    QVector<SyntaxRule> mInCodeRules;
    QRegExp mInSymbolRegexp, mInStringRegexp;

    static SyntaxHighlighterGlobals *mInstance;
};

class SyntaxHighlighter:
    public QSyntaxHighlighter
{
    Q_OBJECT

    static const int inCode = 0;
    static const int inString = 1;
    static const int inSymbol = 2;
    static const int inComment = 100;
    // NOTE: Integers higher than inComment are reserved for multi line comments,
    // and indicate the comment nesting level!

public:
    SyntaxHighlighter(QTextDocument *parent = 0);

private:
    void highlightBlock(const QString &text);
    void highlightBlockInCode(const QString& text, int & currentIndex, int & currentState);
    void highlightBlockInString(const QString& text, int & currentIndex, int & currentState);
    void highlightBlockInSymbol(const QString& text, int & currentIndex, int & currentState);
    void highlightBlockInComment(const QString& text, int & currentIndex, int & currentState);

    Token::Type findMatchingRule(QString const & text, int & currentIndex, int & lengthOfMatch);

    const SyntaxHighlighterGlobals *mGlobals;
};

}

#endif
