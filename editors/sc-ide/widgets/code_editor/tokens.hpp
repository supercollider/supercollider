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

#ifndef SCIDE_WIDGETS_CODE_EDITOR_BRACKETS_HPP_INCLUDED
#define SCIDE_WIDGETS_CODE_EDITOR_BRACKETS_HPP_INCLUDED

#include <QTextBlock>
#include <vector>

namespace ScIDE
{

struct Token
{
    enum Type
    {
        Unknown = 0,

        WhiteSpace,

        Keyword,
        Builtin,
        Primitive,
        Name,
        Class,
        Symbol,
        String,
        Char,
        RadixFloat,
        Float,
        HexInt,
        EnvVar,
        SymbolArg,

        SingleLineComment,
        MultiLineCommentStart,

        OpeningBracket,
        ClosingBracket
    };

    Token() {}
    Token( Type t, int pos, int len = 0, char c = 0 ):
        type(t), position(pos), length(len), character(c) {}
    Token (Token const & rhs):
        type(rhs.type),
        position(rhs.position),
        length(rhs.length),
        character(rhs.character)
    {}

    Type type;
    int position;
    int length;
    char character;
};

struct TextBlockData : public QTextBlockUserData
{
    std::vector<Token> tokens;
};

class TokenIterator
{
private:
    QTextBlock blk;
    int idx;
    TextBlockData *data;

public:
    TokenIterator(): idx(-1) {}
    bool isValid() { return idx >= 0; }
    const QTextBlock & block() { return blk; }
    const Token & operator *()
    {
        Q_ASSERT(blk.isValid());
        Q_ASSERT(idx >= 0);
        Q_ASSERT(data);

        return data->tokens[idx];
    }

    const Token * operator ->()
    {
        Q_ASSERT(blk.isValid());
        Q_ASSERT(idx >= 0);
        Q_ASSERT(data);

        return &data->tokens[idx];
    }

    TokenIterator( const QTextBlock & block ):
        blk(block),
        idx(-1)
    {
        if (block.isValid()) {
            data = static_cast<TextBlockData*>(block.userData());
            if (data && !data->tokens.empty())
                idx = 0;
        }
    }

    TokenIterator( const QTextBlock & block, int pos ):
        blk(block)
    {
        if (block.isValid()) {
            data = static_cast<TextBlockData*>(block.userData());
            idx = data ? data->tokens.size() : 0;
            while (idx--)
            {
                const Token & token = data->tokens[idx];
                if (token.position > pos)
                    continue;
                else if (token.position == pos || token.position + token.length > pos)
                    break;
            }
        }
        else
            idx = -1;
    }

    static TokenIterator leftOf( const QTextBlock & block, int pos )
    {
        TokenIterator it;
        it.blk = block;
        it.idx = -1;

        while(it.blk.isValid())
        {
            it.data = static_cast<TextBlockData*>(it.blk.userData());

            it.idx = it.data ? it.data->tokens.size() : 0;

            while(it.idx--)
            {
                Token const & token = it.data->tokens[it.idx];
                if( pos < 0 || token.position < pos )
                    return it;
            }

            pos = -1; // match on first token in next block;
            it.blk = it.blk.previous();
        }

        return it;
    }

    static TokenIterator rightOf( const QTextBlock & block, int pos )
    {
        TokenIterator it;
        it.blk = block;
        it.idx = -1;

        while(it.blk.isValid())
        {
            it.data = static_cast<TextBlockData*>(it.blk.userData());

            int n = it.data->tokens.size();

            while(++it.idx < n)
            {
                Token const & token = it.data->tokens[it.idx];
                if( token.position >= pos )
                    return it;
            }

            it.idx = -1;
            pos = -1; // match right on first token in next block;
            it.blk = it.blk.next();
        }

        return it;
    }

    static TokenIterator around( const QTextBlock & block, int pos )
    {
        TokenIterator it;
        it.blk = block;
        it.idx = -1;

        if(!block.isValid())
            return it;

        it.data = static_cast<TextBlockData*>(block.userData());

        if (!it.data)
            return it;

        int n = it.data->tokens.size();
        for( int i = 0; i < n; ++i )
        {
            Token const & token = it.data->tokens[i];
            if(token.position > pos) {
                return it;
            }
            else if(token.position == pos - 1 || token.position == pos)
            {
                it.idx = i;
                break;
            }
        }

        return it;
    }

    TokenIterator& operator ++()
    {
        if(idx < 0)
            return *this;

        do
        {
            if(idx < 0)
                data = static_cast<TextBlockData*>(blk.userData());

            if (data)
            {
                int n = data->tokens.size();
                if(++idx < n)
                    return *this;
            }

            idx = -1;
            blk = blk.next();
        }
        while( blk.isValid() );

        return *this;
    }

    TokenIterator& operator --()
    {
        if(idx > 0)
        {
            --idx;
            return *this;
        }
        else if( idx < 0 )
        {
            return *this;
        }

        idx = -1;
        while( (blk = blk.previous()).isValid() )
        {
            data = static_cast<TextBlockData*>(blk.userData());

            if(data)
                idx = data->tokens.size() - 1;

            if (idx < 0)
                continue;

            // we have a valid idx
            break;
        }

        return *this;
    }

    TokenIterator previous()
    {
        TokenIterator it(*this);
        --it;
        return it;
    }

    TokenIterator next()
    {
        TokenIterator it(*this);
        ++it;
        return it;
    }

    char character()
    {
        return (*this)->character;
    }

    int position()
    {
        return (*this)->position + blk.position();
    }

    Token::Type type()
    {
        return isValid() ? (*this)->type : Token::Unknown;
    }
};

} // namespace ScIDE

#endif // SCIDE_WIDGETS_CODE_EDITOR_BRACKETS_HPP_INCLUDED
