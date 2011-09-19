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

#include "QcRangeSlider.h"
#include "../QcWidgetFactory.h"

#include <QKeyEvent>
#include <QMouseEvent>
#include <QApplication>
#include <QPainter>

static QcWidgetFactory<QcRangeSlider> factory;

QcRangeSlider::QcRangeSlider() :
  _lo( 0.f ),
  _hi( 1.f ),
  _step( 0.01f ),
  mouseMode( None )
{
  setFocusPolicy( Qt::StrongFocus );
  setOrientation( Qt::Vertical );
}

void QcRangeSlider::setOrientation( Qt::Orientation o )
{
  _ort = o;

  if( _ort == Qt::Horizontal ) {
    setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Preferred );
  }
  else {
    setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Expanding );
  }

  updateGeometry();
  update();
}

void QcRangeSlider::setLoValue( float val )
{
  val = qMax( 0.f, qMin( 1.f, val ) );
  if( val <= _hi ) {
    _lo = val;
  }
  else {
    _lo = _hi;
    _hi = val;
  }
  update();
}

void QcRangeSlider::setHiValue( float val )
{
  val = qMax( 0.f, qMin( 1.f, val ) );
  if( val >= _lo ) {
    _hi = val;
  }
  else {
    _hi = _lo;
    _lo = val;
  }
  update();
}

QSize QcRangeSlider::sizeHint() const
{
  return ( _ort == Qt::Horizontal ? QSize( 100, 20 ) : QSize( 20, 100 ) );
}

QSize QcRangeSlider::minimumSizeHint() const
{
  return ( _ort == Qt::Horizontal ? QSize( 30, 10 ) : QSize( 10, 30 ) );
}

void QcRangeSlider::increment( double factor )
{
  moveBy( factor * _step );
}

void QcRangeSlider::decrement( double factor )
{
  moveBy( -factor * _step );
}

void QcRangeSlider::increment()
{
  float step = _step;
  modifyStep( &step );
  moveBy( step );
}

void QcRangeSlider::decrement()
{
  float step = _step;
  modifyStep( &step );
  moveBy( -step );
}

QRect QcRangeSlider::thumbRect()
{
  QRect r;

  if( _ort == Qt::Horizontal ) {
    r.setX( _lo * width() );
    r.setRight( _hi * width() );
    r.setHeight( height() );
  }
  else {
    r.setY( (1.f - _hi) * height() );
    r.setBottom( (1.f - _lo ) * height() );
    r.setWidth( width() );
  }

  return r;
}

float QcRangeSlider::valueFromPos( const QPoint& pos )
{
  float val = _ort == Qt::Horizontal ?
              (float)pos.x() / width() :
              1.f - ((float)pos.y() / height());
  return val;
}

void QcRangeSlider::moveBy( float dif )
{
  if( _lo + dif < 0.f ) {
    _hi += 0.f - _lo;
    _lo = 0.f;
  }
  else if( _hi + dif > 1.f ) {
    _lo += 1.f - _hi;
    _hi = 1.f;
  }
  else {
    _lo += dif;
    _hi += dif;
  }
  update();
}

void QcRangeSlider::mouseMoveEvent ( QMouseEvent * e )
{
  if( mouseMode == Drag ) {
      QPoint pt = e->pos() + dragOffset;
      float dif;
      if( _ort == Qt::Horizontal )
        dif = ( (float)pt.x() / width() ) - _lo;
      else
        dif = 1.f - ( (float)pt.y() / height() ) - _hi;

      if( dif == 0.f ) return;

      moveBy( dif );
  }
  else {
    float val = valueFromPos( e->pos() );
    if( mouseMode == SetLo ) {
      if( val > _hi ) mouseMode = SetHi;
      setLoValue( val );
    }
    else if( mouseMode == SetHi ) {
      if( val < _lo ) mouseMode = SetLo;
      setHiValue( val );
    }
  }

  Q_EMIT( action() );
}

void QcRangeSlider::mousePressEvent ( QMouseEvent * e )
{
  QRect r = thumbRect();
  if( e->modifiers() & Qt::ShiftModifier ) {
    float center = (_hi + _lo) * 0.5;
    float val = valueFromPos( e->pos() );
    if( val < center ) {
      mouseMode = SetLo;
      setLoValue( val );
    }
    else {
      mouseMode = SetHi;
      setHiValue( val );
    }
    Q_EMIT( action() );
  }
  else if( r.contains( e->pos() ) ){
    mouseMode = Drag;
    dragOffset = r.topLeft() - e->pos();
  }
  else {
    _lo = _hi = valueFromPos( e->pos() );
    update();
    mouseMode = SetHi;
    Q_EMIT( action() );
  }
}

void QcRangeSlider::mouseReleaseEvent ( QMouseEvent * e )
{
  Q_UNUSED(e);
  mouseMode = None;
}

void QcRangeSlider::keyPressEvent ( QKeyEvent *e )
{
  switch( e->key() ) {
    case Qt::Key_Up:
    case Qt::Key_Right:
      increment(); break;
    case Qt::Key_Down:
    case Qt::Key_Left:
      decrement(); break;
    case Qt::Key_A:
      _lo = 0.f; _hi = 1.f; update(); break;
    case Qt::Key_N:
      _lo = 0.f; _hi = 0.f; update(); break;
    case Qt::Key_X:
      _lo = 1.f; _hi = 1.f; update(); break;
    case Qt::Key_C:
      _lo = 0.5; _hi = 0.5; update(); break;
    default:
      return QWidget::keyPressEvent( e );
  }
  Q_EMIT( action() );
}

void QcRangeSlider::paintEvent ( QPaintEvent *e )
{
  Q_UNUSED(e);
  QPainter p;
  p.begin(this);

  QPalette plt = palette();

  p.setBrush( plt.color( QPalette::Base ) );
  p.setPen( plt.color( QPalette::Mid ) );
  p.drawRect( rect().adjusted(0,0,-1,-1) );

  p.fillRect( thumbRect(), plt.color( QPalette::Text ) );
  p.end();
}
