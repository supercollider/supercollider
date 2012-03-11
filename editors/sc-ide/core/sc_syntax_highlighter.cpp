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

#include <cassert>
#include <algorithm>

#include "sc_syntax_highlighter.hpp"

namespace ScIDE {

SyntaxFormatContainer::SyntaxFormatContainer(void)
{
    keywordFormat.setFontWeight(QFont::Bold);
    keywordFormat.setForeground(Qt::magenta);

    buildinsFormat = keywordFormat;
    primitiveFormat = keywordFormat;

    classFormat.setForeground(Qt::green);
    numberLiteralFormat.setForeground(Qt::green);

    commentFormat.setForeground(QColor(220,0,0));

    stringFormat.setForeground(Qt::blue);

    symbolFormat.setForeground(Qt::magenta);
    charFormat = symbolFormat;

    envVarFormat = symbolFormat;
}

SyntaxFormatContainer gSyntaxFormatContainer;


SyntaxHighlighter::SyntaxHighlighter(QTextDocument *parent):
    QSyntaxHighlighter( parent )
{
    // FIXME: later we can cache the regexps
    initKeywords();
    initBuildins();

    classRegexp.setPattern("\\b[A-Z]\\w*\\b");

    primitiveRegexp.setPattern("\\b_\\w+\\b");
    symbolRegexp.setPattern("(\\\\\\w*|\\'([^\\'\\\\]*(\\\\.[^\\'\\\\]*)*)\\')");
    symbolContentRegexp.setPattern("([^\'\\\\]*(\\\\.[^\'\\\\]*)*)");
    charRegexp.setPattern("\\b\\$(\\w|(\\\\\\S))\\b");
    stringRegexp.setPattern("\"([^\"\\\\]*(\\\\.[^\"\\\\]*)*)\"");
    stringContentRegexp.setPattern("([^\"\\\\]*(\\\\.[^\"\\\\]*)*)");

    envVarRegexp.setPattern("~\\w+");
    symbolArgRegexp.setPattern("\\b[a-z]\\w*\\:");

    floatRegexp.setPattern("\\b-?((\\d*\\.?\\d+([eE][-+]?\\d+)?(pi)?)|pi)\\b");
    hexIntRegexp.setPattern("\\b0(x|X)(\\d|[a-f]|[A-F])+\\b");
    radixFloatRegex.setPattern("\\b(\\d)+r(\\d|[a-zA-Z])+(.(\\d|[a-zA-Z]))?\\b");

    singleLineCommentRegexp.setPattern("//[^\r\n]*");
    commentStartRegexp = QRegExp("/\\*");
    commentEndRegexp = QRegExp("\\*/");
}


void SyntaxHighlighter::initKeywords(void)
{
    QStringList keywords;
    keywords << "arg"
             << "classvar"
             << "const"
             << "super"
             << "this"
             << "thisFunction"
             << "thisFunctionDef"
             << "thisMethod"
             << "thisProcess"
             << "thisThread"
             << "thisThread"
             << "var";

    QString keywordPattern = QString("\\b(%1)\\b").arg(keywords.join("|"));
    keywordRegexp.setPattern(keywordPattern);
}

void SyntaxHighlighter::initBuildins(void)
{
    QStringList buildins;
    buildins << "false"
             << "inf"
             << "nil"
             << "true";

    QString buildinsPattern = QString("\\b(%1)\\b").arg(buildins.join("|"));
    buildinsRegexp.setPattern(buildinsPattern);
}

SyntaxHighlighter::highligherFormat SyntaxHighlighter::findCurrentFormat(const QString& text,
                                                                         int& currentIndex, int& lengthOfMatch)
{
    QVector<int> matchIndices, matchLengths;

    QVector<QRegExp> regexps;
    regexps << classRegexp << keywordRegexp << buildinsRegexp << primitiveRegexp << symbolRegexp << stringRegexp
            << charRegexp  << floatRegexp << hexIntRegexp << radixFloatRegex << envVarRegexp << symbolArgRegexp
            << singleLineCommentRegexp << commentStartRegexp;

    int i = 0;
    foreach(QRegExp expression, regexps) {
        int matchIndex  = expression.indexIn(text, currentIndex);
        int matchLength = (matchIndex != -1) ? expression.matchedLength()
                                             : -1;
        matchIndices << matchIndex;
        matchLengths << matchLength;

        if (matchIndex == currentIndex) {
            // first character matches
            lengthOfMatch = matchLengths.back();
            return static_cast<highligherFormat>(i);
        }

        ++i;
    }

    // find the first matching regexp
    QVector<int>::iterator minimumElementIterator = std::min_element(matchIndices.begin(), matchIndices.end());
    int minimumIndex = *minimumElementIterator;
    if (minimumIndex == -1) {
        lengthOfMatch = 0;
        // no match
        return FormatNone;
    }

    int minElementIndex = matchIndices.indexOf(minimumIndex);

    currentIndex = minimumIndex;
    lengthOfMatch = matchLengths[minElementIndex];
    return static_cast<highligherFormat>(minElementIndex);
}

void SyntaxHighlighter::highlightBlockInCode(const QString& text, int & currentIndex, int & currentState)
{
    do {
        if (text[currentIndex] == '\"') {
            currentState = inString;
            setFormat(currentIndex, 1, gSyntaxFormatContainer.stringFormat);
            currentIndex += 1;
            return;
        }

        if (text[currentIndex] == '\'') {
            currentState = inSymbol;
            setFormat(currentIndex, 1, gSyntaxFormatContainer.symbolFormat);
            currentIndex += 1;
            return;
        }

        int lenghtOfMatch;
        highligherFormat format = findCurrentFormat(text, currentIndex, lenghtOfMatch);

        switch (format)
        {
        case FormatClass:
            setFormat(currentIndex, lenghtOfMatch, gSyntaxFormatContainer.classFormat);
            break;

        case FormatBuiltin:
            setFormat(currentIndex, lenghtOfMatch, gSyntaxFormatContainer.buildinsFormat);
            break;

        case FormatPrimitive:
            setFormat(currentIndex, lenghtOfMatch, gSyntaxFormatContainer.primitiveFormat);
            break;

        case FormatKeyword:
            setFormat(currentIndex, lenghtOfMatch, gSyntaxFormatContainer.keywordFormat);
            break;

        case FormatSymbol:
        case FormatSymbolArg:
            setFormat(currentIndex, lenghtOfMatch, gSyntaxFormatContainer.symbolFormat);
            break;

        case FormatEnvVar:
            setFormat(currentIndex, lenghtOfMatch, gSyntaxFormatContainer.envVarFormat);
            break;

        case FormatString:
            setFormat(currentIndex, lenghtOfMatch, gSyntaxFormatContainer.stringFormat);
            break;

        case FormatChar:
            setFormat(currentIndex, lenghtOfMatch, gSyntaxFormatContainer.charFormat);
            break;

        case FormatSingleLineComment:
            setFormat(currentIndex, lenghtOfMatch, gSyntaxFormatContainer.commentFormat);
            break;

        case FormatFloat:
        case FormatHexInt:
        case FormatRadixFloat:
            setFormat(currentIndex, lenghtOfMatch, gSyntaxFormatContainer.numberLiteralFormat);
            break;

        case FormatMultiLineCommentStart:
            setFormat(currentIndex, lenghtOfMatch, gSyntaxFormatContainer.commentFormat);
            currentIndex += lenghtOfMatch;
            currentState = inComment;
            return;

        case FormatNone:
            currentIndex += 1;

        default:
            ;
        }

        currentIndex += lenghtOfMatch;
    } while (currentIndex < text.size());
}

void SyntaxHighlighter::highlightBlockInString(const QString& text, int& currentIndex, int& currentState)
{
    int matchIndex = stringContentRegexp.indexIn(text, currentIndex);
    if (matchIndex == -1)
        assert(false);

    int matchLength = stringContentRegexp.matchedLength();
    setFormat(currentIndex, matchLength, gSyntaxFormatContainer.stringFormat);
    currentIndex += matchLength;
    if (currentIndex == text.size()) {
        // end of block
        currentState = inString;
        return;
    }

    if (text[currentIndex] == QChar('\"'))
        currentState = inCode;

    setFormat(currentIndex, 1, gSyntaxFormatContainer.stringFormat);
    ++currentIndex;
    return;
}

void SyntaxHighlighter::highlightBlockInSymbol(const QString& text, int& currentIndex, int& currentState)
{
    int matchIndex = symbolContentRegexp.indexIn(text, currentIndex);
    if (matchIndex == -1)
        assert(false);

    int matchLength = symbolContentRegexp.matchedLength();
    setFormat(currentIndex, matchLength, gSyntaxFormatContainer.symbolFormat);
    currentIndex += matchLength;
    if (currentIndex == text.size()) {
        // end of block
        currentState = inSymbol;
        return;
    }

    if (text[currentIndex] == QChar('\''))
        currentState = inCode;

    setFormat(currentIndex, 1, gSyntaxFormatContainer.symbolFormat);
    ++currentIndex;
    return;
}

void SyntaxHighlighter::highlightBlockInComment(const QString& text, int& currentIndex, int& currentState)
{
    /* TODO: comments can be nested */

    int commentEndIndex = text.indexOf(QString("*/"), currentIndex);
    if (commentEndIndex == -1) {
        setFormat(currentIndex, text.size() - currentIndex, gSyntaxFormatContainer.commentFormat);
        currentIndex = text.size();
        return;
    }

    setFormat(currentIndex, commentEndIndex - currentIndex + 2, gSyntaxFormatContainer.commentFormat);
    currentIndex = commentEndIndex + 2;
    currentState = inCode;

    return;
}

void SyntaxHighlighter::highlightBlock(const QString& text)
{
    int currentIndex = 0;

    int currentState = previousBlockState();
    if (currentState == -1)
        currentState = 0;

    while (currentIndex < text.size()) {
        switch (currentState) {
        case inCode:
            highlightBlockInCode(text, currentIndex, currentState);
            break;

        case inString:
            highlightBlockInString(text, currentIndex, currentState);
            break;

        case inSymbol:
            highlightBlockInSymbol(text, currentIndex, currentState);
            break;

        case inComment:
            highlightBlockInComment(text, currentIndex, currentState);
            break;
        }
    }
    setCurrentBlockState(currentState);
}

}
