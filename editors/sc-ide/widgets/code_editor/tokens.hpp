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

#include <QTextBlock>
#include <vector>

namespace ScIDE {

struct Token {
    enum Type {
        Unknown = 0,

        WhiteSpace,

        Keyword,
        Builtin,
        Primitive,
        Name,
        Class,
        Symbol,
        SymbolMark,
        StringMark,
        Char,
        RadixFloat,
        ScaleDegreeFloat,
        Float,
        HexInt,
        EnvVar,
        SymbolArg,

        SingleLineComment,
        MultiLineCommentStart,
        MultiLineCommentEnd,

        OpeningBracket,
        ClosingBracket,
        Operator
    };

    Token(Type t, int pos, int len = 0, char c = 0): type(t), positionInBlock(pos), length(len), character(c) {}
    Token(Token const& rhs):
        type(rhs.type),
        positionInBlock(rhs.positionInBlock),
        length(rhs.length),
        character(rhs.character) {}

    Type type;
    int positionInBlock;
    int length;
    char character;
};

struct TextBlockData : public QTextBlockUserData {
    std::vector<Token> tokens;
};

class TokenIterator {
    /*
    TokenIterator allows easy iteration over tokens in a Document (produced by the syntax highlighter).

    An iterator is valid as long as it refers to an existing token.

    An invalid iterator must not be dereferenced, incremented or decremented.

    Incrementing and decrementing an iterator makes it refer to the next and previous token
    in the document, respectively, regardless of which block the tokens reside in.

    If there is no next/previous token, then incrementing/decrementing an iterator will make it invalid.
    */

private:
    QTextBlock blk;
    int idx;
    TextBlockData* data;

public:
    TokenIterator(): idx(-1) {}
    bool isValid() const { return idx >= 0; }
    const QTextBlock& block() const { return blk; }

    bool operator==(TokenIterator& other) const { return blk == other.blk && idx == other.idx; }

    const Token& operator*() {
        Q_ASSERT(blk.isValid());
        Q_ASSERT(idx >= 0);
        Q_ASSERT(data);

        return data->tokens[idx];
    }

    const Token* operator->() const {
        Q_ASSERT(blk.isValid());
        Q_ASSERT(idx >= 0);
        Q_ASSERT(data);

        return &data->tokens[idx];
    }

    // Constructs an iterator for the first token at the given block.
    // If there's no token in the block, the new iterator is invalid.
    TokenIterator(const QTextBlock& block): blk(block), idx(-1) {
        if (block.isValid()) {
            data = static_cast<TextBlockData*>(block.userData());
            if (data && !data->tokens.empty())
                idx = 0;
        }
    }

    // Constructs an iterator for the token that starts at, or contains the given position.
    // If there is no such token, the new iterator is invalid.
    TokenIterator(const QTextBlock& block, int positionInBlock): blk(block) {
        if (block.isValid()) {
            data = static_cast<TextBlockData*>(block.userData());
            idx = data ? data->tokens.size() : 0;
            while (idx--) {
                const Token& token = data->tokens[idx];
                if (token.positionInBlock > positionInBlock)
                    continue;
                else if (token.positionInBlock == positionInBlock
                         || token.positionInBlock + token.length > positionInBlock)
                    break;
            }
        } else
            idx = -1;
    }

    // Return an iterator for the first token found left of given position, anywhere in the document.
    // If there is no such token, the returned iterator is invalid.
    static TokenIterator leftOf(const QTextBlock& block, int positionInBlock) {
        TokenIterator it;
        it.blk = block;
        it.idx = -1;

        while (it.blk.isValid()) {
            it.data = static_cast<TextBlockData*>(it.blk.userData());

            it.idx = it.data ? it.data->tokens.size() : 0;

            while (it.idx--) {
                Token const& token = it.data->tokens[it.idx];
                if (positionInBlock < 0 || token.positionInBlock < positionInBlock)
                    return it;
            }

            positionInBlock = -1; // match on first token in next block;
            it.blk = it.blk.previous();
        }

        return it;
    }

    // Return an iterator for the first token found at or right of given position, anywhere in the document.
    // If there is no such token, the returned iterator is invalid.
    static TokenIterator rightOf(const QTextBlock& block, int positionInBlock) {
        TokenIterator it;
        it.blk = block;
        it.idx = -1;

        while (it.blk.isValid()) {
            it.data = static_cast<TextBlockData*>(it.blk.userData());

            int n = it.data ? it.data->tokens.size() : 0;

            while (++it.idx < n) {
                Token const& token = it.data->tokens[it.idx];
                if (token.positionInBlock >= positionInBlock)
                    return it;
            }

            it.idx = -1;
            positionInBlock = -1; // match right on first token in next block;
            it.blk = it.blk.next();
        }

        return it;
    }

    // Return an iterator for the token at 'pos' or 'pos-1'.
    // If there is no such token, the returned iterator is invalid.
    static TokenIterator around(const QTextBlock& block, int positionInBlock) {
        TokenIterator it;
        it.blk = block;
        it.idx = -1;

        if (!block.isValid())
            return it;

        it.data = static_cast<TextBlockData*>(block.userData());

        if (!it.data)
            return it;

        int n = it.data->tokens.size();
        for (int i = 0; i < n; ++i) {
            Token const& token = it.data->tokens[i];
            if (token.positionInBlock > positionInBlock) {
                return it;
            } else if (token.positionInBlock == positionInBlock - 1 || token.positionInBlock == positionInBlock) {
                it.idx = i;
                break;
            }
        }

        return it;
    }

    TokenIterator& operator++() {
        if (idx < 0)
            return *this;

        do {
            if (idx < 0)
                data = static_cast<TextBlockData*>(blk.userData());

            if (data) {
                int n = data->tokens.size();
                if (++idx < n)
                    return *this;
            }

            idx = -1;
            blk = blk.next();
        } while (blk.isValid());

        return *this;
    }

    TokenIterator& operator--() {
        if (idx > 0) {
            --idx;
            return *this;
        } else if (idx < 0) {
            return *this;
        }

        idx = -1;
        while ((blk = blk.previous()).isValid()) {
            data = static_cast<TextBlockData*>(blk.userData());

            if (data)
                idx = data->tokens.size() - 1;

            if (idx < 0)
                continue;

            // we have a valid idx
            break;
        }

        return *this;
    }

    TokenIterator previous() const {
        TokenIterator it(*this);
        --it;
        return it;
    }

    TokenIterator next() const {
        TokenIterator it(*this);
        ++it;
        return it;
    }

    char character() const { return (*this)->character; }

    // A convenience function returning the global position in document of the token
    // referred to by this iterator
    int position() const { return (*this)->positionInBlock + blk.position(); }

    // A convenience function returning the type of token referred to by this iterator,
    // or Token::Unknown if the iterator is invalid
    Token::Type type() const { return isValid() ? (*this)->type : Token::Unknown; }
};

} // namespace ScIDE
