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

#include <QPlainTextEdit>
#include <QGraphicsScene>
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
    void hideMouseCursor(QKeyEvent *);


public slots:
    void applySettings( Settings::Manager * );
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

protected slots:
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

    QTextCharFormat mSearchResultTextFormat;

    QList<QTextEdit::ExtraSelection> mSearchSelections;
};

} // namespace ScIDE

#endif // SCIDE_WIDGETS_CODE_EDITOR_EDITOR_HPP_INCLUDED
