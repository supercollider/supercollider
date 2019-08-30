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
#include "../../core/settings/theme.hpp"

#include <QApplication>

namespace ScIDE {

SyntaxHighlighterGlobals* SyntaxHighlighterGlobals::mInstance = 0;

SyntaxHighlighterGlobals::SyntaxHighlighterGlobals(Main* main, Settings::Manager* settings): QObject(main) {
    Q_ASSERT(mInstance == 0);
    mInstance = this;

    ScLexer::initLexicalRules();

    // initialize formats from settings:
    applySettings(settings);

    connect(main, SIGNAL(applySettingsRequest(Settings::Manager*)), this, SLOT(applySettings(Settings::Manager*)));
}

void SyntaxHighlighterGlobals::applySettings(Settings::Manager* s) {
    applySettings(s, "whitespace", WhitespaceFormat);
    applySettings(s, "keyword", KeywordFormat);
    applySettings(s, "built-in", BuiltinFormat);
    applySettings(s, "primitive", PrimitiveFormat);
    applySettings(s, "class", ClassFormat);
    applySettings(s, "number", NumberFormat);
    applySettings(s, "symbol", SymbolFormat);
    applySettings(s, "env-var", EnvVarFormat);
    applySettings(s, "string", StringFormat);
    applySettings(s, "char", CharFormat);
    applySettings(s, "comment", CommentFormat);

    Q_EMIT(syntaxFormatsChanged());
}

void SyntaxHighlighterGlobals::applySettings(Settings::Manager* s, const QString& key, SyntaxFormat type) {
    mFormats[type] = s->getThemeVal(key);
}

SyntaxHighlighter::SyntaxHighlighter(QTextDocument* parent): QSyntaxHighlighter(parent) {
    mGlobals = SyntaxHighlighterGlobals::instance();

    connect(mGlobals, SIGNAL(syntaxFormatsChanged()), this, SLOT(rehighlight()));
}

void SyntaxHighlighter::highlightBlockInCode(ScLexer& lexer) {
    TextBlockData* blockData = static_cast<TextBlockData*>(currentBlockUserData());
    Q_ASSERT(blockData);

    const QTextCharFormat* formats = mGlobals->formats();

    do {
        int tokenPosition = lexer.offset();
        int tokenLength;
        Token::Type tokenType = lexer.nextToken(tokenLength);

        switch (tokenType) {
        case Token::WhiteSpace:
            setFormat(tokenPosition, tokenLength, formats[WhitespaceFormat]);
            break;

        case Token::Class:
            setFormat(tokenPosition, tokenLength, formats[ClassFormat]);
            break;

        case Token::Builtin:
            setFormat(tokenPosition, tokenLength, formats[BuiltinFormat]);
            break;

        case Token::Primitive:
            setFormat(tokenPosition, tokenLength, formats[PrimitiveFormat]);
            break;

        case Token::Keyword:
            setFormat(tokenPosition, tokenLength, formats[KeywordFormat]);
            break;

        case Token::Symbol:
            setFormat(tokenPosition, tokenLength, formats[SymbolFormat]);
            break;

        case Token::SymbolArg:
            // Don't highlight the trailing ':'
            setFormat(tokenPosition, tokenLength - 1, formats[SymbolFormat]);
            break;

        case Token::EnvVar:
            setFormat(tokenPosition, tokenLength, formats[EnvVarFormat]);
            break;

        case Token::Char:
            setFormat(tokenPosition, tokenLength, formats[CharFormat]);
            break;

        case Token::Float:
        case Token::HexInt:
        case Token::ScaleDegreeFloat:
        case Token::RadixFloat:
            setFormat(tokenPosition, tokenLength, formats[NumberFormat]);
            break;

        case Token::SingleLineComment:
            setFormat(tokenPosition, tokenLength, formats[CommentFormat]);
            break;

        case Token::MultiLineCommentStart:
            setFormat(tokenPosition, tokenLength, formats[CommentFormat]);
            break;

        case Token::StringMark:
            setFormat(tokenPosition, tokenLength, formats[StringFormat]);
            break;

        case Token::SymbolMark:
            setFormat(tokenPosition, tokenLength, formats[SymbolFormat]);
            break;

        default:;
        }

        if ((tokenType != Token::WhiteSpace) && (tokenType != Token::SingleLineComment)
            && (tokenType != Token::MultiLineCommentStart)) {
            Token token(tokenType, tokenPosition, tokenLength);
            if (token.length == 1)
                token.character = lexer.text()[tokenPosition].toLatin1();
            blockData->tokens.push_back(token);
        }

    } while (lexer.state() == ScLexer::InCode && lexer.offset() < lexer.text().size());
}

void SyntaxHighlighter::highlightBlockInString(ScLexer& lexer) {
    int originalOffset = lexer.offset();
    int tokenLength;
    Token::Type tokenType = lexer.nextToken(tokenLength);
    int range = lexer.offset() - originalOffset;
    setFormat(originalOffset, range, mGlobals->format(StringFormat));

    if (tokenType == Token::Unknown)
        return;

    Q_ASSERT(tokenType == Token::StringMark);
    Token token(tokenType, lexer.offset() - 1, 1);
    token.character = '"';

    TextBlockData* blockData = static_cast<TextBlockData*>(currentBlockUserData());
    Q_ASSERT(blockData);
    blockData->tokens.push_back(token);
}

void SyntaxHighlighter::highlightBlockInSymbol(ScLexer& lexer) {
    int originalOffset = lexer.offset();
    int tokenLength;
    Token::Type tokenType = lexer.nextToken(tokenLength);
    int range = lexer.offset() - originalOffset;
    setFormat(originalOffset, range, mGlobals->format(SymbolFormat));

    if (tokenType == Token::Unknown)
        return;

    Q_ASSERT(tokenType == Token::SymbolMark);
    Token token(tokenType, lexer.offset() - 1, 1);
    token.character = '\'';

    TextBlockData* blockData = static_cast<TextBlockData*>(currentBlockUserData());
    Q_ASSERT(blockData);
    blockData->tokens.push_back(token);
}

void SyntaxHighlighter::highlightBlockInComment(ScLexer& lexer) {
    int originalOffset = lexer.offset();
    int tokenLength;
    lexer.nextToken(tokenLength);
    int range = lexer.offset() - originalOffset;
    setFormat(originalOffset, range, mGlobals->format(CommentFormat));
}

void SyntaxHighlighter::highlightBlock(const QString& text) {
    TextBlockData* blockData = static_cast<TextBlockData*>(currentBlockUserData());
    if (!blockData) {
        blockData = new TextBlockData;
        blockData->tokens.reserve(8);
        setCurrentBlockUserData(blockData);
    } else {
        blockData->tokens.clear();
    }

    int previousState = previousBlockState();
    if (previousState == -1)
        previousState = ScLexer::InCode;

    ScLexer lexer(text, 0, previousState);

    while (lexer.offset() < text.size()) {
        switch (lexer.state()) {
        case ScLexer::InCode:
            highlightBlockInCode(lexer);
            break;

        case ScLexer::InString:
            highlightBlockInString(lexer);
            break;

        case ScLexer::InSymbol:
            highlightBlockInSymbol(lexer);
            break;

        default:
            if (lexer.state() >= ScLexer::InComment)
                highlightBlockInComment(lexer);
        }
    }

    setCurrentBlockState(lexer.state());
}

}
