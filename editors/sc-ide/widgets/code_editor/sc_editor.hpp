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

#ifndef SCIDE_WIDGETS_CODE_EDITOR_SC_EDITOR_HPP_INCLUDED
#define SCIDE_WIDGETS_CODE_EDITOR_SC_EDITOR_HPP_INCLUDED

#include "editor.hpp"
#include "tokens.hpp"

#include <QTextBlock>
#include <utility>

namespace ScIDE {

typedef std::pair<TokenIterator, TokenIterator> BracketPair;

class ScCodeEditor : public GenericCodeEditor
{
    Q_OBJECT

public:
    static TokenIterator previousOpeningBracket(TokenIterator it);
    static TokenIterator nextClosingBracket(TokenIterator it);
    static void matchBracket( const TokenIterator & bracket, BracketPair & match );
    static void nextBracketPair( const TokenIterator & startIt, BracketPair & bracketPair );

public:
    ScCodeEditor( Document *, QWidget *parent = 0 );
    void setIndentWidth( int );
    QTextCursor currentRegion();
    void blinkCode( const QTextCursor & c );
    bool spaceIndent() { return mSpaceIndent; }

public slots:
    void applySettings( Settings::Manager * );
    void setSpaceIndent(bool on) { mSpaceIndent = on; }
    void indent();
    void triggerAutoCompletion();
    void triggerMethodCallAid();
    void toggleComment();
    void gotoPreviousBlock();
    void gotoNextBlock();
    void selectBlockAroundCursor();
    void selectCurrentRegion();
    void gotoNextRegion();
    void gotoPreviousRegion();
    bool openDocumentation();
    void openDefinition();
    void findReferences();
    void evaluateLine();
    void evaluateRegion();
    void evaluateDocument();

protected:
    virtual bool event( QEvent * );
    virtual void keyPressEvent( QKeyEvent * );
    virtual void mouseReleaseEvent ( QMouseEvent * );
    virtual void mouseDoubleClickEvent( QMouseEvent * );
    virtual void mouseMoveEvent( QMouseEvent * );
    virtual void dragEnterEvent( QDragEnterEvent * );
    virtual bool canInsertFromMimeData ( const QMimeData * data ) const;
    virtual void insertFromMimeData ( const QMimeData * data );

private slots:
    void matchBrackets();

private:
    QTextCursor cursorAt( const TokenIterator, int offset = 0 );
    QTextCursor selectionForPosition( int position );
    QTextCursor regionAroundCursor( const QTextCursor & );
    QTextCursor blockAroundCursor( const QTextCursor & );
    void moveToNextToken( QTextCursor &, QTextCursor::MoveMode );
    void moveToPreviousToken( QTextCursor &, QTextCursor::MoveMode );

    void updateExtraSelections();
    void indentCurrentRegion();

    void toggleCommentSingleLine( QTextCursor );
    void toggleCommentSingleLine();
    void toggleCommentSelection();
    void addSingleLineComment( QTextCursor, int indentationLevel );
    void removeSingleLineComment( QTextCursor );

    int indentedStartOfLine( const QTextBlock & );
    void indent( const QTextCursor & );
    QTextBlock indent( const QTextBlock & b, int level );
    QString makeIndentationString( int level );
    int indentationLevel( const QTextCursor & );
    void insertSpaceToNextTabStop( QTextCursor & );

    int mIndentWidth;
    bool mSpaceIndent;
    bool mStepForwardEvaluation;
    int mBlinkDuration;
    QTextCharFormat mBracketHighlight;
    QTextCharFormat mBracketMismatchFormat;

    QList<QTextEdit::ExtraSelection> mBracketSelections;
    bool mMouseBracketMatch;

    class AutoCompleter *mAutoCompleter;
};

} // namespace ScIDE

#endif // SCIDE_WIDGETS_CODE_EDITOR_SC_EDITOR_HPP_INCLUDED
