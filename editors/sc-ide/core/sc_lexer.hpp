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

#pragma once

#include "../widgets/code_editor/tokens.hpp"

#include <QVector>
#include <QRegExp>
#include <QString>

namespace ScIDE {

class ScLexer {
public:
    enum State {
        InCode = 0,
        InString = 1,
        InSymbol = 2,
        InComment = 100
        // NOTE: Values higher than InComment are reserved for multi line comments,
        // and indicate the comment nesting level!
    };

    static void initLexicalRules();

private:
    struct LexicalRule {
        LexicalRule(): type(Token::Unknown) {}
        LexicalRule(Token::Type t, const QString& s): type(t), expr(s) {}

        Token::Type type;
        QRegExp expr;
    };

    static void initKeywordsRules();
    static void initBuiltinsRules();

    static QVector<LexicalRule> mLexicalRules;

public:
    ScLexer(const QString& text, int offset = 0, int state = InCode): mText(text), mOffset(offset), mState(state) {}

    const QString& text() const { return mText; }

    int state() const { return mState; }
    void setState(int state) { mState = state; }

    int offset() const { return mOffset; }
    void setOffset(int offset) { mOffset = offset; }

    Token::Type nextToken(int& length);

private:
    Token::Type nextTokenInCode(int& length);
    Token::Type nextTokenInString(int& length);
    Token::Type nextTokenInSymbol(int& length);
    Token::Type nextTokenInComment(int& length);

    const QString& mText;
    int mOffset;
    int mState;
};

}
