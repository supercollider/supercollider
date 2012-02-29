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
{
  connect( _editor, SIGNAL(blockCountChanged(int)), this, SLOT(updateContentsWidth()) );
}

void LineIndicator::updateContentsWidth()
{
  _contentsWidth = calcContentsWidth();
  Q_EMIT( contentsWidthChanged() );
}

void LineIndicator::paintEvent( QPaintEvent *e )
{ _editor->paintLineIndicator(e); }

int LineIndicator::calcContentsWidth()
{
  int digits = 1;
  int lineCount = qMax( 1, _editor->blockCount() );
  while( lineCount >= 10 ) {
    lineCount /= 10;
    ++digits;
  }

  return 4 + fontMetrics().width('9') * digits;
}



CodeEditor::CodeEditor( QWidget *parent ) : QPlainTextEdit( parent )
{
  _lineIndicator = new LineIndicator( this );
  connect( _lineIndicator, SIGNAL( contentsWidthChanged() ),
           this, SLOT( updateLayout() ) );
  connect( this, SIGNAL(updateRequest(QRect,int)),
           this, SLOT(updateLineIndicator(QRect,int)) );
  _lineIndicator->updateContentsWidth();
}

void CodeEditor::updateLayout()
{
  setViewportMargins( _lineIndicator->contentsWidth(), 0, 0, 0 );
}

void CodeEditor::updateLineIndicator( QRect r, int dy )
{
  if (dy)
    _lineIndicator->scroll(0, dy);
  else
    _lineIndicator->update(0, r.y(), _lineIndicator->contentsWidth(), r.height() );
}

void CodeEditor::resizeEvent( QResizeEvent *e )
{
  QPlainTextEdit::resizeEvent( e );

  QRect cr = contentsRect();
  _lineIndicator->setGeometry( cr.left(), cr.top(), _lineIndicator->contentsWidth(), cr.height() );
}

void CodeEditor::paintLineIndicator( QPaintEvent *e )
{
  QPainter painter( _lineIndicator );
  painter.fillRect( e->rect(), palette().color( QPalette::Button ) );

  QTextBlock block = firstVisibleBlock();
  int blockNumber = block.blockNumber();
  int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
  int bottom = top + (int) blockBoundingRect(block).height();

  while (block.isValid() && top <= e->rect().bottom()) {
    if (block.isVisible() && bottom >= e->rect().top()) {
        QString number = QString::number(blockNumber + 1);
        painter.setPen(Qt::black);
        painter.drawText(0, top, _lineIndicator->contentsWidth() - 2, fontMetrics().height(),
                        Qt::AlignRight, number);
    }

    block = block.next();
    top = bottom;
    bottom = top + (int) blockBoundingRect(block).height();
    ++blockNumber;
  }
}

} // namespace ScIDE
