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

struct BracketInfo
{
    BracketInfo() {}
    BracketInfo( char c, int pos ) : character(c), position(pos) {}
    BracketInfo (BracketInfo const & rhs):
        character(rhs.character), position(rhs.position)
    {}

    char character;
    int position;
};

struct TextBlockData : public QTextBlockUserData
{
    std::vector<BracketInfo> brackets;
};

class BracketIterator
{
private:
    BracketIterator() {}
    QTextBlock blk;
    int idx;

public:
    bool isValid() { return idx >= 0; }
    const QTextBlock & block() { return blk; }

    static BracketIterator leftOf( const QTextBlock & block, int pos )
    {
        BracketIterator it;
        it.blk = block;
        it.idx = -1;

        while(it.blk.isValid())
        {
            TextBlockData *data =
                static_cast<TextBlockData*>(it.blk.userData());

            it.idx = data ? data->brackets.size() : 0;

            while(it.idx--)
            {
                BracketInfo const & bracket = data->brackets[it.idx];
                if( pos < 0 || bracket.position < pos )
                    return it;
            }

            pos = -1; // match on first bracket in next block;
            it.blk = it.blk.previous();
        }

        return it;
    }

    static BracketIterator rightOf( const QTextBlock & block, int pos )
    {
        BracketIterator it;
        it.blk = block;
        it.idx = -1;

        while(it.blk.isValid())
        {
            TextBlockData *data =
                static_cast<TextBlockData*>(it.blk.userData());

            int n = data->brackets.size();

            while(++it.idx < n)
            {
                BracketInfo const & bracket = data->brackets[it.idx];
                if( bracket.position >= pos )
                    return it;
            }

            it.idx = -1;
            pos = -1; // match right on first bracket in next block;
            it.blk = it.blk.next();
        }

        return it;
    }

    static BracketIterator around( const QTextBlock & block, int pos )
    {
        BracketIterator it;
        it.blk = block;
        it.idx = -1;

        if(!block.isValid())
            return it;

        TextBlockData *data =
            static_cast<TextBlockData*>(block.userData());

        if (!data)
            return it;

        int n = data->brackets.size();
        for( int i = 0; i < n; ++i )
        {
            BracketInfo const & bracket = data->brackets[i];
            if(bracket.position > pos) {
                return it;
            }
            else if(bracket.position == pos - 1 || bracket.position == pos)
            {
                it.idx = i;
                break;
            }
        }

        return it;
    }

    BracketIterator& operator ++()
    {
        while(blk.isValid())
        {
            TextBlockData *data =
                static_cast<TextBlockData*>(blk.userData());

            if (data)
            {
                int n = data->brackets.size();
                if(++idx < n)
                    return *this;
            }

            idx = -1;
            blk = blk.next();
        }
        return *this;
    }

    BracketIterator& operator --()
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
            TextBlockData *data =
                static_cast<TextBlockData*>(blk.userData());

            if(data)
                idx = data->brackets.size() - 1;

            if (idx < 0)
                continue;

            // we have a valid idx
            break;
        }

        return *this;
    }

    char character()
    {
        Q_ASSERT(blk.isValid() && idx >= 0);

        TextBlockData *data =
            static_cast<TextBlockData*>(blk.userData());

        Q_ASSERT(data);

        BracketInfo const & bracket = data->brackets[idx];
        return bracket.character;
    }

    int position()
    {
        Q_ASSERT(blk.isValid() && idx >= 0);

        TextBlockData *data =
            static_cast<TextBlockData*>(blk.userData());

        Q_ASSERT(data);

        BracketInfo const & bracket = data->brackets[idx];
        return bracket.position + blk.position();
    }
};

} // namespace ScIDE

#endif // SCIDE_WIDGETS_CODE_EDITOR_BRACKETS_HPP_INCLUDED
