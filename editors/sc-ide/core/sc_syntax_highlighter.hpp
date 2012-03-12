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

namespace ScIDE {

class SyntaxFormatContainer
{
public:
    SyntaxFormatContainer(void);

private:
    friend class SyntaxHighlighter;
    QTextCharFormat keywordFormat, buildinsFormat, primitiveFormat, classFormat, commentFormat, stringFormat,
        symbolFormat, charFormat, numberLiteralFormat, envVarFormat, plainFormat;
};

extern SyntaxFormatContainer gSyntaxFormatContainer;

class SyntaxHighlighter:
    public QSyntaxHighlighter
{
    Q_OBJECT

    typedef enum
    {
        FormatClass,
        FormatKeyword,
        FormatBuiltin,
        FormatPrimitive,
        FormatSymbol,
        FormatString,
        FormatChar,
        FormatFloat,
        FormatHexInt,
        FormatRadixFloat,
        FormatEnvVar,
        FormatSymbolArg,

        FormatSingleLineComment,
        FormatMultiLineCommentStart,

        FormatNone
    } highligherFormat;

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

    highligherFormat findCurrentFormat(QString const & text, int & currentIndex, int & lengthOfMatch);

    QRegExp classRegexp, keywordRegexp, buildinsRegexp, primitiveRegexp, symbolRegexp, symbolContentRegexp,
        charRegexp, stringRegexp, stringContentRegexp, floatRegexp, hexIntRegexp, radixFloatRegex,
        commentStartRegexp, commentEndRegexp, singleLineCommentRegexp, envVarRegexp, symbolArgRegexp;
};

}

#endif
