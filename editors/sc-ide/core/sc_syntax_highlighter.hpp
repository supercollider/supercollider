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

#include <QSyntaxHighlighter>
#include <QVector>
#include <QSettings>

namespace ScIDE {

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

class SyntaxHighlighterGlobals : public QObject
{
    Q_OBJECT

public:
    SyntaxHighlighterGlobals( Main * );

    inline const QTextCharFormat * formats() const
    {
        return mFormats;
    }

    inline const QTextCharFormat * defaultFormats() const
    {
        return mDefaultFormats;
    }

    inline const QTextCharFormat & format( SyntaxFormat type ) const
    {
        return mFormats[type];
    }

    inline const QTextCharFormat & defaultFormat( SyntaxFormat type ) const
    {
        return mDefaultFormats[type];
    }

    inline static const SyntaxHighlighterGlobals * instance() { Q_ASSERT(mInstance); return mInstance; }

public Q_SLOTS:
    void applySettings( QSettings * );

Q_SIGNALS:
    void syntaxFormatsChanged();

private:
    void applySettings( QSettings*, const QString &key, SyntaxFormat );

    QTextCharFormat mFormats[FormatCount];
    QTextCharFormat mDefaultFormats[FormatCount];

    static SyntaxHighlighterGlobals *mInstance;
};

class SyntaxHighlighter:
    public QSyntaxHighlighter
{
    Q_OBJECT

    enum SyntaxRule
    {
        SymbolArgRule,
        ClassRule,
        KeywordRule,
        BuiltinRule,
        PrimitiveRule,
        SymbolRule,
        StringRule,
        CharRule,
        RadixFloatRule,
        FloatRule,
        HexIntRule,
        EnvVarRule,

        SingleLineCommentRule,
        MultiLineCommentStartRule,

        NoRule
    };

    static const int inCode = 0;
    static const int inString = 1;
    static const int inSymbol = 2;
    static const int inComment = 100;
    // NOTE: Integers higher than inComment are reserved for multi line comments,
    // and indicate the comment nesting level!

public:
    struct BracketInfo
    {
        BracketInfo( char c, int pos ) : character(c), position(pos) {}
        char character;
        int position;
    };

    struct BlockData : public QTextBlockUserData
    {
        QVector<BracketInfo*> brackets;
    };

public:
    SyntaxHighlighter(QTextDocument *parent = 0);

private:
    void highlightBlock(const QString &text);
    void highlightBlockInCode(const QString& text, int & currentIndex, int & currentState);
    void highlightBlockInString(const QString& text, int & currentIndex, int & currentState);
    void highlightBlockInSymbol(const QString& text, int & currentIndex, int & currentState);
    void highlightBlockInComment(const QString& text, int & currentIndex, int & currentState);
    void initKeywords(void);
    void initBuildins(void);

    SyntaxRule findMatchingRule(QString const & text, int & currentIndex, int & lengthOfMatch);

    QRegExp classRegexp, keywordRegexp, buildinsRegexp, primitiveRegexp, symbolRegexp, symbolContentRegexp,
        charRegexp, stringRegexp, stringContentRegexp, floatRegexp, hexIntRegexp, radixFloatRegex,
        commentStartRegexp, commentEndRegexp, singleLineCommentRegexp, envVarRegexp, symbolArgRegexp;

    const SyntaxHighlighterGlobals *mGlobals;
    QVector<QRegExp> mRegexps;
};

}

#endif
