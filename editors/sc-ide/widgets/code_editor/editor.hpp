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

namespace ScIDE
{

namespace Settings { class Manager; }

class Document;
class CodeEditor;
class AutoCompleter;

class LineIndicator : public QWidget
{
    Q_OBJECT

public:
    LineIndicator( CodeEditor *editor );
Q_SIGNALS:
    void widthChanged();
public Q_SLOTS:
    void setLineCount( int );
protected:
    virtual void changeEvent( QEvent * );
    virtual void paintEvent( QPaintEvent *e );
private:
    int widthForLineCount( int lineCount );

    CodeEditor *mEditor;
    int mLineCount;
};

class CodeEditor : public QPlainTextEdit
{
    Q_OBJECT

    friend class LineIndicator;

public:
    CodeEditor( QWidget *parent = 0 );
    Document *document() { return mDoc; }
    QTextDocument *textDocument() { return QPlainTextEdit::document(); }
    void setDocument( Document * );
    bool showWhitespace() { return mShowWhitespace; }
    void setIndentWidth( int );
    bool find( const QRegExp &expr, QTextDocument::FindFlags options = 0);
    bool replace( const QRegExp &expr, const QString &replacement, QTextDocument::FindFlags options = 0);
    int findAll( const QRegExp &expr, QTextDocument::FindFlags options = 0 );
    int replaceAll( const QRegExp &expr, const QString &replacement,
                    QTextDocument::FindFlags options = 0 );
    QTextCursor currentRegion();
    void blinkCode( const QTextCursor & c );
    void showPosition( int );

public Q_SLOTS:
    void zoomIn(int steps = 1);
    void zoomOut(int steps = 1);
    void indentMore() { indent(false); }
    void indentLess() { indent(true); }
    void setSpaceIndent(bool on) { mSpaceIndent = on; }
    void setShowWhitespace(bool);
    void clearSearchHighlighting();
    void applySettings( Settings::Manager * );

protected:
    virtual bool event( QEvent * );
    virtual void changeEvent( QEvent * );
    virtual void keyPressEvent( QKeyEvent * );
    virtual void mouseReleaseEvent ( QMouseEvent * );
    virtual void mouseDoubleClickEvent ( QMouseEvent * );
    virtual void mouseMoveEvent( QMouseEvent * );
    virtual void paintEvent( QPaintEvent * );

private Q_SLOTS:
    void updateLayout();
    void updateLineIndicator( QRect, int );
    void matchBrackets();
    void onOverlayChanged ( const QList<QRectF> & region );

private:
    struct BracketMatch {
        TokenIterator first;
        TokenIterator second;
    };

    void resizeEvent( QResizeEvent * );
    void paintLineIndicator( QPaintEvent * );
    void indent( bool less );
    void matchBracket( int pos, BracketMatch & match );
    int indentedStartOfLine( const QTextBlock & );
    void updateExtraSelections();

    LineIndicator *mLineIndicator;
    Document *mDoc;
    int mIndentWidth;
    bool mSpaceIndent;
    bool mShowWhitespace;
    QColor mBracketHighlight;

    QList<QTextEdit::ExtraSelection> mBracketSelections;
    QList<QTextEdit::ExtraSelection> mSearchSelections;
    bool mMouseBracketMatch;

    QGraphicsScene *mOverlay;

    AutoCompleter *mAutoCompleter;
};

} // namespace ScIDE

#endif // SCIDE_WIDGETS_CODE_EDITOR_EDITOR_HPP_INCLUDED
