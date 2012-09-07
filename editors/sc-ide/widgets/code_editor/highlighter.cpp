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

    initSyntaxRules();

    // initialize formats from settings:
    applySettings(settings);

    connect(main, SIGNAL(applySettingsRequest(Settings::Manager*)),
            this, SLOT(applySettings(Settings::Manager*)));
}

void SyntaxHighlighterGlobals::initSyntaxRules()
{
    /* NOTE:

    The highlighting algorithm demands that all regexps
    start with a caret "^", to only match at beginning of string.

    Order is important:
    -- floatRegexp is subset of radixFloatRegex -> must come later
    -- classRegexp and primitiveRegexp are subsets of symbolArgRegexp -> must come later

    */

    mInCodeRules << SyntaxRule( Token::WhiteSpace, "^\\s+" );

    initKeywords();
    initBuiltins();

    mInCodeRules << SyntaxRule( Token::SymbolArg, "^\\b[A-Za-z_]\\w*\\:" );

    mInCodeRules << SyntaxRule( Token::Name, "^[a-z]\\w*" );

    mInCodeRules << SyntaxRule( Token::Class, "^\\b[A-Z]\\w*" );

    mInCodeRules << SyntaxRule( Token::Primitive, "^\\b_\\w+" );

    mInCodeRules << SyntaxRule( Token::Symbol, "^(\\\\\\w*|\\'([^\\'\\\\]*(\\\\.[^\\'\\\\]*)*)\\')" );

    mInCodeRules << SyntaxRule( Token::Char, "^\\$\\\\?." );

    mInCodeRules << SyntaxRule( Token::String, "^\"([^\"\\\\]*(\\\\.[^\"\\\\]*)*)\"" );

    mInCodeRules << SyntaxRule( Token::EnvVar, "^~\\w+" );

    mInCodeRules << SyntaxRule( Token::RadixFloat, "^\\b\\d+r[0-9a-zA-Z]*(\\.[0-9A-Z]*)?" );

    // do not include leading "-" in float highlighting, as there's no clear
    // rule whether it is not rather a binary operator
    mInCodeRules << SyntaxRule( Token::Float, "^\\b((\\d+(\\.\\d+)?([eE][-+]?\\d+)?(pi)?)|pi)" );

    mInCodeRules << SyntaxRule( Token::HexInt, "^\\b0(x|X)(\\d|[a-f]|[A-F])+" );

    mInCodeRules << SyntaxRule( Token::SingleLineComment, "^//[^\r\n]*" );

    mInCodeRules << SyntaxRule( Token::MultiLineCommentStart, "^/\\*" );

    mInSymbolRegexp.setPattern("([^\'\\\\]*(\\\\.[^\'\\\\]*)*)");
    mInStringRegexp.setPattern("([^\"\\\\]*(\\\\.[^\"\\\\]*)*)");
}

void SyntaxHighlighterGlobals::initKeywords()
{
    QStringList keywords;
    keywords << "arg"
             << "classvar"
             << "const"
             << "super"
             << "this"
             << "var";

    QString keywordPattern = QString("^\\b(%1)\\b").arg(keywords.join("|"));
    mInCodeRules << SyntaxRule(Token::Keyword, keywordPattern);
}

void SyntaxHighlighterGlobals::initBuiltins()
{
    QStringList builtins;
    builtins << "false"
             << "inf"
             << "nil"
             << "true"
             << "thisFunction"
             << "thisFunctionDef"
             << "thisMethod"
             << "thisProcess"
             << "thisThread"
             << "currentEnvironment"
             << "topEnvironment"
                ;

    QString builtinsPattern = QString("^\\b(%1)\\b").arg(builtins.join("|"));
    mInCodeRules << SyntaxRule(Token::Builtin, builtinsPattern);
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

Token::Type SyntaxHighlighter::findMatchingRule (const QString& text, int& currentIndex, int& lengthOfMatch)
{
    int matchLength = -1;
    Token::Type matchType = Token::Unknown;

    QVector<SyntaxRule>::const_iterator it  = mGlobals->mInCodeRules.constBegin();
    QVector<SyntaxRule>::const_iterator end = mGlobals->mInCodeRules.constEnd();

    for (; it != end; ++it) {
        SyntaxRule const & rule = *it;
        int matchIndex = rule.expr.indexIn(text, currentIndex, QRegExp::CaretAtOffset);
        // a guard to ensure all regexps match only at beginning of string:
        assert(matchIndex <= currentIndex);
        if (matchIndex != -1) {
            matchType = rule.type;
            matchLength = rule.expr.matchedLength();
            break;
        }
    }

    lengthOfMatch = matchType == Token::Unknown ? 0 : matchLength;
    return matchType;
}

void SyntaxHighlighter::highlightBlockInCode(const QString& text, int & currentIndex, int & currentState)
{
    TextBlockData *blockData = static_cast<TextBlockData*>(currentBlockUserData());
    Q_ASSERT(blockData);

    const QTextCharFormat * formats = mGlobals->formats();

    do {
        static QString openingBrackets("({[");
        static QString closingBrackets(")}]");
        static QChar stringStart('\"');
        static QChar symbolStart('\'');

        QChar currentChar = text[currentIndex];

        if (currentChar == stringStart) {
            currentState = inString;
            setFormat(currentIndex, 1, formats[StringFormat]);
            blockData->tokens.push_back( Token(Token::String, currentIndex) );
            currentIndex += 1;
            return;
        }

        if (currentChar == symbolStart) {
            currentState = inSymbol;
            setFormat(currentIndex, 1, formats[SymbolFormat]);
            blockData->tokens.push_back( Token(Token::Symbol, currentIndex) );
            currentIndex += 1;
            return;
        }

        if (openingBrackets.contains(currentChar)) {
            blockData->tokens.push_back(
                Token( Token::OpeningBracket,
                       currentIndex, 1,
                       currentChar.toAscii() ) );
            ++currentIndex;
            continue;
        }

        if (closingBrackets.contains(currentChar)) {
            blockData->tokens.push_back(
                Token( Token::ClosingBracket,
                       currentIndex, 1,
                       currentChar.toAscii() ) );
            ++currentIndex;
            continue;
        }

        int lenghtOfMatch;
        Token::Type tokenType = findMatchingRule(text, currentIndex, lenghtOfMatch);

        switch (tokenType)
        {
        case Token::WhiteSpace:
            // avoid inserting a token
            currentIndex += lenghtOfMatch;
            continue;

        case Token::Class:
            setFormat(currentIndex, lenghtOfMatch, formats[ClassFormat]);
            break;

        case Token::Builtin:
            setFormat(currentIndex, lenghtOfMatch, formats[BuiltinFormat]);
            break;

        case Token::Primitive:
            setFormat(currentIndex, lenghtOfMatch, formats[PrimitiveFormat]);
            break;

        case Token::Keyword:
            setFormat(currentIndex, lenghtOfMatch, formats[KeywordFormat]);
            break;

        case Token::Symbol:
            setFormat(currentIndex, lenghtOfMatch, formats[SymbolFormat]);
            break;

        case Token::SymbolArg:
            // Omit the trailing ":" that was included in the regexp:
            setFormat(currentIndex, lenghtOfMatch-1, formats[SymbolFormat]);
            break;

        case Token::EnvVar:
            setFormat(currentIndex, lenghtOfMatch, formats[EnvVarFormat]);
            break;

        case Token::String:
            setFormat(currentIndex, lenghtOfMatch, formats[StringFormat]);
            break;

        case Token::Char:
            setFormat(currentIndex, lenghtOfMatch, formats[CharFormat]);
            break;

        case Token::Float:
        case Token::HexInt:
        case Token::RadixFloat:
            setFormat(currentIndex, lenghtOfMatch, formats[NumberFormat]);
            break;

        case Token::SingleLineComment:
            setFormat(currentIndex, lenghtOfMatch, formats[CommentFormat]);
            currentIndex += lenghtOfMatch;
            continue;

        case Token::MultiLineCommentStart:
            setFormat(currentIndex, lenghtOfMatch, formats[CommentFormat]);
            currentIndex += lenghtOfMatch;
            currentState = inComment;
            return;

        case Token::Unknown:
            blockData->tokens.push_back( Token(Token::Unknown, currentIndex, 1, currentChar.toAscii()) );
            currentIndex += 1;
            continue;

        default:
            ;
        }

        blockData->tokens.push_back( Token(tokenType, currentIndex, lenghtOfMatch) );

        currentIndex += lenghtOfMatch;

    } while (currentIndex < text.size());
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
