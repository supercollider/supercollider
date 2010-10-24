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

#include "QcLevelIndicator.h"

#include <QPainter>

void QcLevelIndicator::paintEvent( QPaintEvent *e )
{
  QPainter p(this);

  QPalette plt = palette();
  QRectF r;
  r.setWidth( width() );

  float h = height();
  float y = 0.f;
  float v = 1.f;

  if( v > _value ) {
    y = (1.f - _value) * h;
    r.setBottom( y );
    p.fillRect( r, QColor( 130,130,130 ) );
    v = _value;
  }

  if( v > _critical ) {
    r.moveTop( y );
    y = (1.f - _critical) * h;
    r.setBottom( y );
    p.fillRect( r, QColor(255,100,0) );
    v = _critical;
  }

  if( v > _warning ) {
    r.moveTop( y );
    y = (1.f - _warning) * h;
    r.setBottom( y );
    p.fillRect( r, QColor( 255, 255, 0 ) );
    v = _warning;
  }

  if( v > 0.f ) {
    r.moveTop( y );
    r.setBottom( h );
    p.fillRect( r, QColor( 0, 255, 0 ) );
  }

  if( _drawPeak ) {
    y = (1.f - _peak) * (h - 4) + 2;
    QPen pen( QColor( 255, 200, 0 ) );
    pen.setWidth( 2 );
    p.setPen( pen );
    p.drawLine( 0.f, y, width(), y );
  }

  r = rect().adjusted(0,0,-1,-1);
  p.setBrush( Qt::NoBrush );
  p.setPen( plt.color( QPalette::Dark ) );
  p.drawRect( r );
}
