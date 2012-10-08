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

#include "highlighter.hpp"
#include "../../core/main.hpp"
#include "../../core/settings/manager.hpp"

#include <QApplication>

namespace ScIDE {

SyntaxHighlighterGlobals * SyntaxHighlighterGlobals::mInstance = 0;

SyntaxHighlighterGlobals::SyntaxHighlighterGlobals( Main *main, Settings::Manager * settings ):
    QObject(main)
{
    Q_ASSERT(mInstance == 0);
    mInstance = this;

    ScLexer::initLexicalRules();
    mInSymbolRegexp.setPattern("([^\'\\\\]*(\\\\.[^\'\\\\]*)*)");
    mInStringRegexp.setPattern("([^\"\\\\]*(\\\\.[^\"\\\\]*)*)");

    // initialize formats from settings:
    applySettings(settings);

    connect(main, SIGNAL(applySettingsRequest(Settings::Manager*)),
            this, SLOT(applySettings(Settings::Manager*)));
}

void SyntaxHighlighterGlobals::applySettings( Settings::Manager *s )
{
    QString key("IDE/editor/highlighting");
    applySettings( s, key + "/normal", PlainFormat );
    applySettings( s, key + "/keyword", KeywordFormat );
    applySettings( s, key + "/built-in", BuiltinFormat );
    applySettings( s, key + "/primitive", PrimitiveFormat );
    applySettings( s, key + "/class", ClassFormat );
    applySettings( s, key + "/number", NumberFormat );
    applySettings( s, key + "/symbol", SymbolFormat );
    applySettings( s, key + "/env-var", EnvVarFormat );
    applySettings( s, key + "/string", StringFormat );
    applySettings( s, key + "/char", CharFormat );
    applySettings( s, key + "/comment", CommentFormat );

    Q_EMIT(syntaxFormatsChanged());
}

void SyntaxHighlighterGlobals::applySettings( Settings::Manager *s, const QString &key, SyntaxFormat type )
{
    mFormats[type] = s->value(key).value<QTextCharFormat>();
}

SyntaxHighlighter::SyntaxHighlighter(QTextDocument *parent):
    QSyntaxHighlighter( parent )
{
    mGlobals = SyntaxHighlighterGlobals::instance();

    connect(mGlobals, SIGNAL(syntaxFormatsChanged()), this, SLOT(rehighlight()));
}

void SyntaxHighlighter::highlightBlockInCode(const QString& text, int & currentIndex, int & currentState)
{
    TextBlockData *blockData = static_cast<TextBlockData*>(currentBlockUserData());
    Q_ASSERT(blockData);

    const QTextCharFormat * formats = mGlobals->formats();

    do {
        int lengthOfMatch;
        Token::Type tokenType = mLexer.nextToken(text, currentIndex, lengthOfMatch);

        switch (tokenType)
        {
        case Token::Class:
            setFormat(currentIndex, lengthOfMatch, formats[ClassFormat]);
            break;

        case Token::Builtin:
            setFormat(currentIndex, lengthOfMatch, formats[BuiltinFormat]);
            break;

        case Token::Primitive:
            setFormat(currentIndex, lengthOfMatch, formats[PrimitiveFormat]);
            break;

        case Token::Keyword:
            setFormat(currentIndex, lengthOfMatch, formats[KeywordFormat]);
            break;

        case Token::Symbol:
            setFormat(currentIndex, lengthOfMatch, formats[SymbolFormat]);
            break;

        case Token::SymbolArg:
            // Don't highlight the trailing ':'
            setFormat(currentIndex, lengthOfMatch-1, formats[SymbolFormat]);
            break;

        case Token::EnvVar:
            setFormat(currentIndex, lengthOfMatch, formats[EnvVarFormat]);
            break;

        case Token::Char:
            setFormat(currentIndex, lengthOfMatch, formats[CharFormat]);
            break;

        case Token::Float:
        case Token::HexInt:
        case Token::RadixFloat:
            setFormat(currentIndex, lengthOfMatch, formats[NumberFormat]);
            break;

        case Token::SingleLineComment:
            setFormat(currentIndex, lengthOfMatch, formats[CommentFormat]);
            break;

        case Token::MultiLineCommentStart:
            currentState = inComment;
            setFormat(currentIndex, lengthOfMatch, formats[CommentFormat]);
            break;

        case Token::StringMark:
            currentState = inString;
            setFormat(currentIndex, lengthOfMatch, formats[StringFormat]);
            break;

        case Token::SymbolMark:
            currentState = inSymbol;
            setFormat(currentIndex, lengthOfMatch, formats[SymbolFormat]);
            break;

        default:
            ;
        }

        lengthOfMatch =  qMax( lengthOfMatch, 1 ); // process at least 1 character

        if ( (tokenType != Token::WhiteSpace) &&
             (tokenType != Token::SingleLineComment) &&
             (tokenType != Token::MultiLineCommentStart) )
        {
            Token token(tokenType, currentIndex, lengthOfMatch);
            if (token.length == 1)
                token.character = text[currentIndex].toAscii();
            blockData->tokens.push_back( token );
        }

        currentIndex += lengthOfMatch;

    } while (currentState == inCode && currentIndex < text.size());
}

void SyntaxHighlighter::highlightBlockInString(const QString& text, int& currentIndex, int& currentState)
{
    const QRegExp &expr = mGlobals->mInStringRegexp;
    int matchIndex = expr.indexIn(text, currentIndex);
    if (matchIndex == -1)
        assert(false);

    int matchLength = expr.matchedLength();
    setFormat(currentIndex, matchLength, mGlobals->format(StringFormat));
    currentIndex += matchLength;
    if (currentIndex == text.size()) {
        // end of block
        currentState = inString;
        return;
    }

    static const QChar endOfString('\"');
    if (text[currentIndex] == endOfString)
        currentState = inCode;

    setFormat(currentIndex, 1, mGlobals->format(StringFormat));
    ++currentIndex;
    return;
}

void SyntaxHighlighter::highlightBlockInSymbol(const QString& text, int& currentIndex, int& currentState)
{
    const QRegExp &expr = mGlobals->mInSymbolRegexp;
    int matchIndex = expr.indexIn(text, currentIndex);
    if (matchIndex == -1)
        assert(false);

    int matchLength = expr.matchedLength();
    setFormat(currentIndex, matchLength, mGlobals->format(SymbolFormat));
    currentIndex += matchLength;
    if (currentIndex == text.size()) {
        // end of block
        currentState = inSymbol;
        return;
    }

    static const QChar endOfSymbol('\'');
    if (text[currentIndex] == endOfSymbol)
        currentState = inCode;

    setFormat(currentIndex, 1, mGlobals->format(SymbolFormat));
    ++currentIndex;
    return;
}

void SyntaxHighlighter::highlightBlockInComment(const QString& text, int& currentIndex, int& currentState)
{
    int index = currentIndex;
    int maxIndex = text.size() - 1;

    static const QString commentStart("/*");
    static const QString commentEnd("*/");

    int commentStartIndex = -2;
    int commentEndIndex   = -2;

    while(index < maxIndex) {
        if ((commentStartIndex == -2) || (commentStartIndex < index))
            if (commentStartIndex != -1)
                commentStartIndex = text.indexOf(commentStart, index);

        if ((commentEndIndex == -2) || (commentEndIndex < index))
            if (commentEndIndex != -1)
                commentEndIndex   = text.indexOf(commentEnd, index);

        if (commentStartIndex == -1) {
            if (commentEndIndex == -1) {
                index = maxIndex;
            } else {
                index = commentEndIndex + 2;
                --currentState;
            }
        } else {
            if (commentEndIndex == -1) {
                index = commentStartIndex + 2;
                ++currentState;
            } else {
                if (commentStartIndex < commentEndIndex) {
                    index = commentStartIndex + 2;
                    ++currentState;
                } else {
                    index = commentEndIndex + 2;
                    --currentState;
                }
            }
        }
        if (currentState < inComment) {
            currentState = inCode;
            break;
        }
    }

    if(currentState == inCode) {
        setFormat(currentIndex, index - currentIndex, mGlobals->format(CommentFormat));
        currentIndex = index;
    }
    else {
        setFormat(currentIndex, text.size() - currentIndex, mGlobals->format(CommentFormat));
        currentIndex = text.size();
    }

    return;
}

void SyntaxHighlighter::highlightBlock(const QString& text)
{
    int currentIndex = 0;

    int currentState = previousBlockState();
    if (currentState == -1)
        currentState = 0;

    TextBlockData *blockData = static_cast<TextBlockData*>(currentBlockUserData());
    if(!blockData) {
        blockData = new TextBlockData;
        blockData->tokens.reserve(8);
        setCurrentBlockUserData(blockData);
    }
    else {
        blockData->tokens.clear();
    }

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

        default:
            if(currentState >= inComment)
                highlightBlockInComment(text, currentIndex, currentState);
        }
    }
    setCurrentBlockState(currentState);
}

}
