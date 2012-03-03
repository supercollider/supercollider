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

namespace ScIDE {

class SyntaxFormatContainer
{
public:
    SyntaxFormatContainer(void);

private:
    friend class SyntaxHighlighter;
    QTextCharFormat keywordFormat, buildinsFormat, primitiveFormat, classFormat, commentFormat, stringFormat,
        symbolFormat, charFormat, numberLiteralFormat, plainFormat;
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
        FormatChar,
        FormatString,
        FormatFloat,
        FormatHexInt,
        FormatRadixFloat,
        FormatSingleLineComment,
        FormatMultiLineCommentStart,

        FormatNone
    } highligherFormat;

public:
    SyntaxHighlighter(QTextDocument *parent = 0);

private:
    void highlightBlock(const QString &text);
    void initKeywords(void);
    void initBuildins(void);

    highligherFormat findCurrentFormat(QString const & text, int & currentIndex, int & lengthOfMatch);

    QRegExp classRegexp;
    QRegExp keywordRegexp;
    QRegExp buildinsRegexp;
    QRegExp primitiveRegexp;
    QRegExp symbolRegexp;
    QRegExp charRegexp;
    QRegExp stringRegexp;
    QRegExp floatRegexp;
    QRegExp hexIntRegexp, radixFloatRegex;
    QRegExp commentStartRegexp, commentEndRegexp;
    QRegExp singleLineCommentRegexp;
};

}

#endif
