/************************************************************************
*
* Copyright 2010-2012 Jakob Leben (jakob.leben@gmail.com)
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
#include "../QcWidgetFactory.h"

#include <QPainter>

QC_DECLARE_QWIDGET_FACTORY(QcLevelIndicator);

QcLevelIndicator::QcLevelIndicator() :
  QtCollider::Style::Client(this),
  _value( 0.f ), _warning(0.6), _critical(0.8),
  _peak( 0.f ), _drawPeak( false ),
  _ticks(0), _majorTicks(0),
  _clipped(false)
{
  _clipTimer = new QTimer( this );
  _clipTimer->setSingleShot(true);
  _clipTimer->setInterval( 1000 );
  connect( _clipTimer, SIGNAL(timeout()), this, SLOT(clipTimeout()) );
}

void QcLevelIndicator::clipTimeout()
{
  _clipped = false;
  update();
}


void QcLevelIndicator::paintEvent( QPaintEvent *e )
{
  QPainter p(this);

  QPalette plt = palette();

  bool vertical = height() >= width();

  float groove = vertical ? width() : height();
  if( _ticks || _majorTicks ) groove -= 6;

  float length = vertical ? height() : width();

  float colorValue = _drawPeak ? _peak : _value;

  if( colorValue > _critical ) {
    _clipTimer->stop();
    _clipped = true;
  }
  else if( _clipped && !_clipTimer->isActive() ) {
    _clipTimer->start();
  }

  QColor c;
  if( _clipped )
    c = QColor(255,100,0);
  else if( colorValue > _warning )
    c = QColor( 255, 255, 0 );
  else
    c = QColor( 0, 255, 0 );

  p.fillRect( vertical ? QRectF(0,0,groove,height()) : QRectF(0,0,width(),groove),
              grooveColor() );

  QRectF r;

  if( vertical ) {
    r.setWidth( groove );
    r.setY( (1.f - _value) * length );
    r.setBottom( height() );
  }
  else {
    r.setHeight( groove );
    r.setRight( _value * length );
  }

  p.fillRect( r, c );

#if 0

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
#endif

  if( _drawPeak && _peak > 0.f ) {
    float peak = vertical ? _peak : 1 - _peak;

    // compensate for border and peak line width
    float val = (1.f - peak)
          * ( length  - 4 )
          + 2;
    QPen pen( QColor( 255, 200, 0 ) );
    pen.setWidth( 2 );
    pen.setCapStyle( Qt::FlatCap );
    p.setPen( pen );
    if( vertical )
      p.drawLine( 0.f, val, groove, val );
    else
      p.drawLine( val, 0.f, val, groove );
  }

  if( _ticks ) {
    QPen pen( plt.color(QPalette::WindowText) );
    pen.setCapStyle( Qt::FlatCap );
    p.setPen(pen);
    float dVal = ( _ticks > 1 ) ? ( length-1) / (float)(_ticks-1) : 0.f;
    float t = 0;
    while( t < _ticks ) {
      float v = t * dVal;
      if( vertical )
        p.drawLine( groove, v, width(), v );
      else
        p.drawLine( v, groove, v, height() );
      t++;
    }
  }

  if( _majorTicks ) {
    QPen pen ( plt.color(QPalette::WindowText) );
    pen.setCapStyle( Qt::FlatCap );
    pen.setWidth( 3 );
    p.setPen( pen );
    float dVal = ( _majorTicks > 1 ) ? (length-3) / (float)(_majorTicks-1) : 0.f;
    float t = 0;
    while( t < _majorTicks ) {
      float v = (int) (t * dVal) + 1;
      if( vertical )
        p.drawLine( groove, v, width(), v );
      else
        p.drawLine( v, groove, v, height() );
      t++;
    }
  }
}
