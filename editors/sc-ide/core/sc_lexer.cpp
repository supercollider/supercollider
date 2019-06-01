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

#include "sc_lexer.hpp"
#include <QDebug>

namespace ScIDE {

QVector<ScLexer::LexicalRule> ScLexer::mLexicalRules;

void ScLexer::initLexicalRules() {
    /* NOTE:

    The lexical analysis algorithm demands that all regexps
    start with a caret "^", to only match at beginning of string.

    Order is important:
    -- floatRegexp is subset of radixFloatRegex -> must come later
    -- classRegexp and primitiveRegexp are subsets of symbolArgRegexp -> must come later

    */

    mLexicalRules << LexicalRule(Token::WhiteSpace, "^\\s+");

    initKeywordsRules();
    initBuiltinsRules();

    mLexicalRules << LexicalRule(Token::RadixFloat, "^\\b\\d+r[0-9a-zA-Z]*(\\.[0-9A-Z]*)?");

    // Never heard of this one? Check the "Literals" help file :)
    mLexicalRules << LexicalRule(Token::ScaleDegreeFloat, "^\\b\\d+(s+|b+|[sb]\\d+)\\b");

    // do not include leading "-" in Float, as left-to-right algorithm does
    // not know whether it is not rather a binary operator
    mLexicalRules << LexicalRule(Token::Float, "^\\b((\\d+(\\.\\d+)?([eE][-+]?\\d+)?(pi)?)|pi)\\b");

    mLexicalRules << LexicalRule(Token::HexInt, "^\\b0x(\\d|[a-f]|[A-F])+");

    mLexicalRules << LexicalRule(Token::SymbolArg, "^\\b[A-Za-z_]\\w*\\:");

    mLexicalRules << LexicalRule(Token::Name, "^[a-z]\\w*");

    mLexicalRules << LexicalRule(Token::Class, "^\\b[A-Z]\\w*");

    mLexicalRules << LexicalRule(Token::Primitive, "^\\b_\\w+");

    mLexicalRules << LexicalRule(Token::Symbol, "^\\\\\\w*");

    mLexicalRules << LexicalRule(Token::Char, "^\\$\\\\?.");

    mLexicalRules << LexicalRule(Token::EnvVar, "^~\\w+");


    mLexicalRules << LexicalRule(Token::SingleLineComment, "^//[^\r\n]*");

    mLexicalRules << LexicalRule(Token::MultiLineCommentStart, "^/\\*");

    mLexicalRules << LexicalRule(Token::Operator, "^[\\+-\\*/&\\|\\^%<>=]+");
}

void ScLexer::initKeywordsRules() {
    QStringList keywords;
    keywords << "arg"
             << "classvar"
             << "const"
             << "super"
             << "this"
             << "var";

    QString keywordPattern = QStringLiteral("^\\b(%1)\\b").arg(keywords.join("|"));
    mLexicalRules << LexicalRule(Token::Keyword, keywordPattern);
}

void ScLexer::initBuiltinsRules() {
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
             << "topEnvironment";

    QString builtinsPattern = QStringLiteral("^\\b(%1)\\b").arg(builtins.join("|"));
    mLexicalRules << LexicalRule(Token::Builtin, builtinsPattern);
}

Token::Type ScLexer::nextToken(int& lengthResult) {
    Q_ASSERT(mOffset < mText.size());

    switch (mState) {
    case InCode:
        return nextTokenInCode(lengthResult);
    case InString:
        return nextTokenInString(lengthResult);
    case InSymbol:
        return nextTokenInSymbol(lengthResult);
    default:
        if (mState >= InComment)
            return nextTokenInComment(lengthResult);
    }

    lengthResult = 0;
    return Token::Unknown;
}

Token::Type ScLexer::nextTokenInCode(int& lengthResult) {
    static QString openingBrackets("({[");
    static QString closingBrackets(")}]");
    static QChar stringMark('\"');
    static QChar symbolMark('\'');

    QChar currentChar = mText[mOffset];

    Token::Type type = Token::Unknown;
    int length = 1;

    if (currentChar == stringMark) {
        type = Token::StringMark;
        mState = InString;
    } else if (currentChar == symbolMark) {
        type = Token::SymbolMark;
        if (mOffset + 1 < mText.size()) // line break ends a symbol
            mState = InSymbol;
    } else if (openingBrackets.contains(currentChar)) {
        type = Token::OpeningBracket;
    } else if (closingBrackets.contains(currentChar)) {
        type = Token::ClosingBracket;
    } else {
        QVector<LexicalRule>::const_iterator it = mLexicalRules.constBegin();
        QVector<LexicalRule>::const_iterator end = mLexicalRules.constEnd();

        for (; it != end; ++it) {
            LexicalRule const& rule = *it;
            int matchIndex = rule.expr.indexIn(mText, mOffset, QRegExp::CaretAtOffset);
            // a guard to ensure all regexps match only at beginning of string:
            Q_ASSERT(matchIndex <= mOffset);
            if (matchIndex != -1) {
                type = rule.type;
                length = rule.expr.matchedLength();
                break;
            }
        }

        if (type == Token::MultiLineCommentStart)
            mState = InComment;
    }

    length = qMax(length, 1); // process at least 1 char
    mOffset += length;

    lengthResult = length;
    return type;
}

Token::Type ScLexer::nextTokenInString(int& length) {
    // TODO: Provide simple tokenization by word delimiters

    static QChar stringMark('"');
    static QChar escapeChar('\\');

    length = 0;
    Token::Type type = Token::Unknown;

    int textLength = mText.length();
    while (mOffset < textLength) {
        QChar chr = mText[mOffset];
        ++mOffset;
        if (chr == escapeChar) {
            ++mOffset;
        } else if (chr == stringMark) {
            length = 1;
            type = Token::StringMark;
            mState = InCode;
            break;
        }
    }

    mOffset = qMin(mOffset, textLength);
    return type;
}

Token::Type ScLexer::nextTokenInSymbol(int& length) {
    // TODO: Provide simple tokenization by word delimiters

    static QChar symbolMark('\'');
    static QChar escapeChar('\\');

    length = 0;
    Token::Type type = Token::Unknown;

    int textLength = mText.length();
    while (mOffset < textLength) {
        QChar chr = mText[mOffset];
        ++mOffset;
        if (chr == escapeChar) {
            ++mOffset;
        } else if (chr == symbolMark) {
            length = 1;
            type = Token::SymbolMark;
            break;
        }
    }

    mOffset = qMin(mOffset, textLength);
    mState = InCode;
    return type;
}

Token::Type ScLexer::nextTokenInComment(int& lengthResult) {
    // TODO: Provide tokenization using (some) lexical rules for code

    int index = mOffset;
    int maxIndex = mText.size() - 1;

    static const QString commentStart("/*");
    static const QString commentEnd("*/");

    int commentStartIndex = -2;
    int commentEndIndex = -2;

    while (index < maxIndex) {
        if ((commentStartIndex == -2) || (commentStartIndex < index))
            if (commentStartIndex != -1)
                commentStartIndex = mText.indexOf(commentStart, index);

        if ((commentEndIndex == -2) || (commentEndIndex < index))
            if (commentEndIndex != -1)
                commentEndIndex = mText.indexOf(commentEnd, index);

        if (commentStartIndex == -1) {
            if (commentEndIndex == -1) {
                index = maxIndex;
            } else {
                index = commentEndIndex + 2;
                --mState;
            }
        } else {
            if (commentEndIndex == -1) {
                index = commentStartIndex + 2;
                ++mState;
            } else {
                if (commentStartIndex < commentEndIndex) {
                    index = commentStartIndex + 2;
                    ++mState;
                } else {
                    index = commentEndIndex + 2;
                    --mState;
                }
            }
        }
        if (mState < InComment) {
            mState = InCode;
            break;
        }
    }

    if (mState == InCode) {
        mOffset = index;
        lengthResult = 2;
        return Token::MultiLineCommentEnd;
    }

    mOffset = mText.size();
    lengthResult = 0;
    return Token::Unknown;
}

}
