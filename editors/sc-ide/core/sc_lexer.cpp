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

namespace ScIDE {

QVector<ScLexer::LexicalRule> ScLexer::mLexicalRules;

void ScLexer::initLexicalRules()
{
    /* NOTE:

    The lexical analysis algorithm demands that all regexps
    start with a caret "^", to only match at beginning of string.

    Order is important:
    -- floatRegexp is subset of radixFloatRegex -> must come later
    -- classRegexp and primitiveRegexp are subsets of symbolArgRegexp -> must come later

    */

    mLexicalRules << LexicalRule( Token::WhiteSpace, "^\\s+" );

    initKeywordsRules();
    initBuiltinsRules();

    mLexicalRules << LexicalRule( Token::SymbolArg, "^\\b[A-Za-z_]\\w*\\:" );

    mLexicalRules << LexicalRule( Token::Name, "^[a-z]\\w*" );

    mLexicalRules << LexicalRule( Token::Class, "^\\b[A-Z]\\w*" );

    mLexicalRules << LexicalRule( Token::Primitive, "^\\b_\\w+" );

    mLexicalRules << LexicalRule( Token::Symbol, "^(\\\\\\w*|\\'([^\\'\\\\]*(\\\\.[^\\'\\\\]*)*)\\')" );

    mLexicalRules << LexicalRule( Token::Char, "^\\$\\\\?." );

    mLexicalRules << LexicalRule( Token::EnvVar, "^~\\w+" );

    mLexicalRules << LexicalRule( Token::RadixFloat, "^\\b\\d+r[0-9a-zA-Z]*(\\.[0-9A-Z]*)?" );

    // do not include leading "-" in Float, as left-to-right algorithm does
    // not know whether it is not rather a binary operator
    mLexicalRules << LexicalRule( Token::Float, "^\\b((\\d+(\\.\\d+)?([eE][-+]?\\d+)?(pi)?)|pi)" );

    mLexicalRules << LexicalRule( Token::HexInt, "^\\b0(x|X)(\\d|[a-f]|[A-F])+" );

    mLexicalRules << LexicalRule( Token::SingleLineComment, "^//[^\r\n]*" );

    mLexicalRules << LexicalRule( Token::MultiLineCommentStart, "^/\\*" );
}

void ScLexer::initKeywordsRules()
{
    QStringList keywords;
    keywords << "arg"
             << "classvar"
             << "const"
             << "super"
             << "this"
             << "var";

    QString keywordPattern = QString("^\\b(%1)\\b").arg(keywords.join("|"));
    mLexicalRules << LexicalRule(Token::Keyword, keywordPattern);
}

void ScLexer::initBuiltinsRules()
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
    mLexicalRules << LexicalRule(Token::Builtin, builtinsPattern);
}

Token::Type ScLexer::nextToken ( QString const & text, int offset, int & lengthOfMatch )
{
    static QString openingBrackets("({[");
    static QString closingBrackets(")}]");
    static QChar stringMark('\"');
    static QChar symbolMark('\'');

    QChar currentChar = text[offset];

    if (currentChar == stringMark) {
        lengthOfMatch = 1;
        return Token::StringMark;
    }

    if (currentChar == symbolMark) {
        lengthOfMatch = 1;
        return Token::SymbolMark;
    }

    if (openingBrackets.contains(currentChar)) {
        lengthOfMatch = 1;
        return Token::OpeningBracket;
    }

    if (closingBrackets.contains(currentChar)) {
        lengthOfMatch = 1;
        return Token::ClosingBracket;
    }

    int matchLength = 0;
    Token::Type matchType = Token::Unknown;

    QVector<LexicalRule>::const_iterator it  = mLexicalRules.constBegin();
    QVector<LexicalRule>::const_iterator end = mLexicalRules.constEnd();

    for (; it != end; ++it) {
        LexicalRule const & rule = *it;
        int matchIndex = rule.expr.indexIn(text, offset, QRegExp::CaretAtOffset);
        // a guard to ensure all regexps match only at beginning of string:
        Q_ASSERT(matchIndex <= offset);
        if (matchIndex != -1) {
            matchType = rule.type;
            matchLength = rule.expr.matchedLength();
            break;
        }
    }

    lengthOfMatch = matchLength;
    return matchType;
}

}
