/************************************************************************
*
* Copyright 2010 Jakob Leben (jakob.leben@gmail.com)
*
* This file is part of SuperCollider Qt GUI.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
************************************************************************/

#include "QcCanvas.h"
#include "../Painting.h"
#include "../Common.h"

#include <QPainter>

QcCanvas::QcCanvas( QWidget *parent )
: QWidget( parent ), paint( false ), repaintNeeded( true )
{
  //_bkgColor = palette().color( QPalette::Background );
}

void QcCanvas::refresh()
{
  repaintNeeded = true;
  update();
}

void QcCanvas::customEvent( QEvent *e )
{
  if( e->type() == (QEvent::Type) QtCollider::Event_Refresh ) {
    e->accept();
    refresh();
  }
}

void QcCanvas::resizeEvent( QResizeEvent * )
{
  refresh();
}

void QcCanvas::paintEvent( QPaintEvent * )
{
  if( paint && repaintNeeded ) {
    _pixmap = QPixmap( size() );
    _pixmap.fill( QColor(0,0,0,0) );

    QPainter pixPainter( &_pixmap );

    Q_EMIT( painting(&pixPainter) );

    repaintNeeded = false;
  }

  QPainter p(this);
  if( _bkgColor.isValid() ) p.fillRect( rect(), _bkgColor );
  if( paint ) p.drawPixmap( rect(), _pixmap );
}
