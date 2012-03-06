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

#include "code_edit.hpp"

#include <QPainter>
#include <QPaintEvent>
#include <QTextBlock>

namespace ScIDE
{

LineIndicator::LineIndicator( CodeEditor *editor )
: QWidget( editor ), _editor(editor)
{}

void LineIndicator::setLineCount( int count )
{
  setFixedWidth( widthForLineCount(count) );
  Q_EMIT( widthChanged() );
}

void LineIndicator::paintEvent( QPaintEvent *e )
{ _editor->paintLineIndicator(e); }

int LineIndicator::widthForLineCount( int lineCount )
{
  int digits = 1;
  while( lineCount >= 10 ) {
    lineCount /= 10;
    ++digits;
  }

  return 4 + fontMetrics().width('9') * digits;
}

CodeEditor::CodeEditor( QWidget *parent ) :
    QPlainTextEdit( parent ),
    _lineIndicator( new LineIndicator(this) ),
    mDoc(0)
{
    _lineIndicator->move( contentsRect().topLeft() );

    connect( this, SIGNAL(blockCountChanged(int)),
             _lineIndicator, SLOT(setLineCount(int)) );

    connect( _lineIndicator, SIGNAL( widthChanged() ),
             this, SLOT( updateLayout() ) );

    connect( this, SIGNAL(updateRequest(QRect,int)),
             this, SLOT(updateLineIndicator(QRect,int)) );

    _lineIndicator->setLineCount(1);
}

void CodeEditor::setDocument( Document *doc )
{
    QTextDocument *tdoc = doc->textDocument();

    QFont fnt(font());
    fnt.setFamily("monospace");
    tdoc->setDefaultFont(fnt);
    tdoc->setDocumentLayout( new QPlainTextDocumentLayout(tdoc) );

    QPlainTextEdit::setDocument(tdoc);

    _lineIndicator->setLineCount( tdoc->blockCount() );

    mDoc = doc;
}

void CodeEditor::updateLayout()
{
  setViewportMargins( _lineIndicator->width(), 0, 0, 0 );
}

void CodeEditor::updateLineIndicator( QRect r, int dy )
{
  if (dy)
    _lineIndicator->scroll(0, dy);
  else
    _lineIndicator->update(0, r.y(), _lineIndicator->width(), r.height() );
}

void CodeEditor::resizeEvent( QResizeEvent *e )
{
  QPlainTextEdit::resizeEvent( e );

  QRect cr = contentsRect();
  _lineIndicator->resize( _lineIndicator->width(), cr.height() );
}

void CodeEditor::paintLineIndicator( QPaintEvent *e )
{
  QPalette plt( palette() );
  QRect r( e->rect() );
  QPainter p( _lineIndicator );

  p.fillRect( r, plt.color( QPalette::Button ) );
  p.setPen( plt.color(QPalette::Text) );
  p.drawLine( r.topRight(), r.bottomRight() );

  QTextBlock block = firstVisibleBlock();
  int blockNumber = block.blockNumber();
  int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
  int bottom = top + (int) blockBoundingRect(block).height();

  while (block.isValid() && top <= e->rect().bottom()) {
    if (block.isVisible() && bottom >= e->rect().top()) {
        QString number = QString::number(blockNumber + 1);
        p.drawText(0, top, _lineIndicator->width() - 2, fontMetrics().height(),
                        Qt::AlignRight, number);
    }

    block = block.next();
    top = bottom;
    bottom = top + (int) blockBoundingRect(block).height();
    ++blockNumber;
  }
}

} // namespace ScIDE
