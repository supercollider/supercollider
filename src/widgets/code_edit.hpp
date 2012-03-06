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

#ifndef SCIDE_WIDGETS_CODE_EDIT_HPP_INCLUDED
#define SCIDE_WIDGETS_CODE_EDIT_HPP_INCLUDED

#include "../doc_manager.hpp"

#include <QPlainTextEdit>

namespace ScIDE
{

class CodeEditor;

class LineIndicator : public QWidget
{
    Q_OBJECT

public:
    LineIndicator( CodeEditor *editor );
    int contentsWidth() { return _contentsWidth; }
Q_SIGNALS:
    void widthChanged();
public Q_SLOTS:
    void setLineCount( int );

private:
    void paintEvent( QPaintEvent *e );
    int widthForLineCount( int lineCount );

    CodeEditor *_editor;
    int _contentsWidth;
};


class CodeEditor : public QPlainTextEdit
{
    Q_OBJECT

    friend class LineIndicator;
public:
    CodeEditor( QWidget *parent = 0 );
    Document *document() { return mDoc; }
    void setDocument( Document * );
private Q_SLOTS:
    void updateLayout();
    void updateLineIndicator( QRect, int );
private:

    void resizeEvent( QResizeEvent * );
    void paintLineIndicator( QPaintEvent * );

    LineIndicator *_lineIndicator;
    Document *mDoc;
};

} // namespace ScIDE

#endif // SC_IDE_CODE_EDIT_HPP
