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

#ifndef SCIDE_WIDGETS_CODE_EDITOR_EDITOR_HPP_INCLUDED
#define SCIDE_WIDGETS_CODE_EDITOR_EDITOR_HPP_INCLUDED

#include "tokens.hpp"

#include <QGraphicsScene>
#include <QPlainTextEdit>
#include <QTextBlock>
#include <QList>

namespace ScIDE {

namespace Settings { class Manager; }

class Document;

class GenericCodeEditor : public QPlainTextEdit
{
    Q_OBJECT

    friend class LineIndicator;

public:
    GenericCodeEditor (Document *, QWidget * parent = NULL);

    Document *document() { return mDoc; }
    QTextDocument *textDocument() { return QPlainTextEdit::document(); }
    void setDocument( Document * );
    bool showWhitespace();
    bool find( const QRegExp &expr, QTextDocument::FindFlags options = 0);
    bool replace( const QRegExp &expr, const QString &replacement, QTextDocument::FindFlags options = 0);
    int findAll( const QRegExp &expr, QTextDocument::FindFlags options = 0 );
    int replaceAll( const QRegExp &expr, const QString &replacement,
                    QTextDocument::FindFlags options = 0 );

    void showPosition( int charPosition, int selectionLength );
    QString symbolUnderCursor();

protected:
    virtual void keyPressEvent( QKeyEvent * );
    virtual void wheelEvent( QWheelEvent * );
    virtual void dragEnterEvent( QDragEnterEvent * );

public Q_SLOTS:
    void zoomIn(int steps = 1);
    void zoomOut(int steps = 1);
    void resetFontSize();
    void setShowWhitespace(bool);
    void clearSearchHighlighting();
    void toggleOverwriteMode();
    void copyLineUp();
    void copyLineDown();
    void moveLineUp();
    void moveLineDown();
    void gotoPreviousEmptyLine();
    void gotoNextEmptyLine();

protected Q_SLOTS:
    void updateLayout();
    void updateLineIndicator( QRect, int );
    void onDocumentFontChanged();

protected:
    void resizeEvent( QResizeEvent * );
    void paintLineIndicator( QPaintEvent * );
    virtual void updateExtraSelections();
    virtual void indentCurrentRegion() {}

    void zoomFont(int steps);

    void copyUpDown(bool up);
    void moveLineUpDown(bool up);
    void gotoEmptyLineUpDown(bool up);

    void hideMouseCursor();

    class LineIndicator *mLineIndicator;
    QGraphicsScene *mOverlay;
    QWidget *mOverlayWidget;

    Document *mDoc;
    QList<QTextEdit::ExtraSelection> mSearchSelections;
};

class CodeEditor : public GenericCodeEditor
{
    Q_OBJECT

public:
    CodeEditor( Document *, QWidget *parent = 0 );
    void setIndentWidth( int );
    QTextCursor currentRegion();
    QTextCursor regionAtCursor( const QTextCursor & );
    void blinkCode( const QTextCursor & c );


public Q_SLOTS:
    void setSpaceIndent(bool on) { mSpaceIndent = on; }
    void applySettings( Settings::Manager * );
    void indent();
    void triggerAutoCompletion();
    void triggerMethodCallAid();
    void toggleComment();
    void gotoPreviousBlock();
    void gotoNextBlock();
    void selectCurrentRegion();
    void gotoNextRegion();
    void gotoPreviousRegion();
    bool openDocumentation();
    void openDefinition();
    void findReferences();

protected:
    virtual bool event( QEvent * );
    virtual void keyPressEvent( QKeyEvent * );
    virtual void mouseReleaseEvent ( QMouseEvent * );
    virtual void mouseDoubleClickEvent( QMouseEvent * );
    virtual void mouseMoveEvent( QMouseEvent * );

private Q_SLOTS:
    void matchBrackets();

private:
    struct BracketMatch {
        TokenIterator first;
        TokenIterator second;
    };

    void matchBracket( const TokenIterator & bracket, BracketMatch & match );
    void updateExtraSelections();
    void indentCurrentRegion();

    void toggleCommentSingleLine( QTextCursor );
    void toggleCommentSingleLine();
    void toggleCommentSelection();
    void addSingleLineComment( QTextCursor, int indentationLevel );
    void removeSingleLineComment( QTextCursor );

    QTextCursor blockAtCursor(const QTextCursor &); // text cursor should point to bracket!

    int indentedStartOfLine( const QTextBlock & );
    void indent( const QTextCursor & );
    QTextBlock indent( const QTextBlock & b, int level );
    QString makeIndentationString( int level );
    int indentationLevel( const QTextCursor & );

    QTextCursor cursorAt( const TokenIterator, int offset = 0 );

    int mIndentWidth;
    bool mSpaceIndent;
    int mBlinkDuration;
    QTextCharFormat mBracketHighlight;

    QList<QTextEdit::ExtraSelection> mBracketSelections;
    bool mMouseBracketMatch;

    class AutoCompleter *mAutoCompleter;
};

} // namespace ScIDE

#endif // SCIDE_WIDGETS_CODE_EDITOR_EDITOR_HPP_INCLUDED
