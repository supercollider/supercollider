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

#include "QcRangeSlider.h"
#include "../QcWidgetFactory.h"
#include "../style/routines.hpp"

#include <QKeyEvent>
#include <QMouseEvent>
#include <QApplication>
#include <QPainter>

#define HND 20

QC_DECLARE_QWIDGET_FACTORY(QcRangeSlider);

QcRangeSlider::QcRangeSlider() :
  QtCollider::Style::Client(this),
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
    setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
  }
  else {
    setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Expanding );
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
  return ( _ort == Qt::Horizontal ? QSize( 150, 20 ) : QSize( 20, 150 ) );
}

QSize QcRangeSlider::minimumSizeHint() const
{
  return ( _ort == Qt::Horizontal ? QSize( 30, 20 ) : QSize( 20, 30 ) );
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

QRect QcRangeSlider::valueRect()
{
  int half_hnd = HND * 0.5;
  if( _ort == Qt::Horizontal )
    return thumbRect().adjusted( half_hnd, 1, -half_hnd, -1 );
  else
    return thumbRect().adjusted( 1, half_hnd, -1, -half_hnd);
}

QRect QcRangeSlider::thumbRect()
{
  using namespace QtCollider::Style;
  QRect contRect( sunkenContentsRect(rect()) );
  if( _ort == Qt::Horizontal ) {
    double valRange = contRect.width() - HND;
    double left = _lo * valRange;
    double right = _hi * valRange;  + HND;
    return QRect( left + contRect.x(), contRect.y(), right - left, contRect.height() );
  }
  else {
    double valRange = contRect.height() - HND;
    int up = (1.0 - _hi) * valRange;
    int down = (1.0 - _lo) * valRange; down += HND;
    return QRect( contRect.x(), contRect.y() + up, contRect.width(), down - up );
  }
}

double QcRangeSlider::valueFromPos( const QPoint& pos )
{
  using namespace QtCollider::Style;

  bool horiz = _ort == Qt::Horizontal;

  QSize margins = horiz ? QSize(HND,0) : QSize(0,HND);
  QRect valBounds = marginsRect( sunkenContentsRect(rect()), margins );

  return horiz ? xValue( pos.x(), valBounds ) : yValue( pos.y(), valBounds );
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
  if( !e->buttons() ) return;

  if( mouseMode == Drag ) {
      QPoint pt = e->pos() + dragOffset;
      float val = valueFromPos( pt );
      float dif;
      if( _ort == Qt::Horizontal )
        dif = val - _lo;
      else
        dif = val - _hi;

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
  using namespace QtCollider::Style;

  QRect r;
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
  else if( (r = valueRect()).contains( e->pos() ) ){
    mouseMode = Drag;
    dragOffset = r.topLeft() - e->pos();
  }
  else {
    _lo = _hi = qBound(0.0, valueFromPos( e->pos() ), 1.0);
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

inline void drawMarker( QPainter *p, const QPalette &plt, Qt::Orientation ort, bool first, const QPoint & pt, int len )
{
  p->save();
  p->setRenderHint( QPainter::Antialiasing, false );

  QPen pen(plt.color(QPalette::ButtonText));
  pen.setCapStyle(Qt::FlatCap);

  bool vert = ort == Qt::Vertical;

  if(vert) {
    pen.setWidth(1); p->setPen(pen);
    p->drawPoint( QPoint( pt.x() + len * 0.5, first ? pt.y() - 5 : pt.y() + 5 ) );

    pen.setWidth(2); p->setPen(pen);
    QLine line( pt, QPoint(pt.x() + len, pt.y()) );
    p->drawLine(line);

    line.translate(0,1);
    pen.setColor(plt.color(QPalette::Light));
    pen.setWidth(1);
    p->setPen(pen);
    p->drawLine(line);
  } else {
    pen.setWidth(1); p->setPen(pen);
    p->drawPoint( QPoint( first ? pt.x() - 5 : pt.x() + 5, pt.y() + len * 0.5 ) );

    pen.setWidth(2); p->setPen(pen);
    QLine line( pt, QPoint(pt.x(), pt.y() + len));
    p->drawLine(line);

    line.translate(1,0);
    pen.setColor(plt.color(QPalette::Light));
    pen.setWidth(1);
    p->setPen(pen);
    p->drawLine(line);
  }
  p->restore();
}

void QcRangeSlider::paintEvent ( QPaintEvent *e )
{
  using namespace QtCollider::Style;

  Q_UNUSED(e);

  QPainter p(this);
  p.setRenderHint( QPainter::Antialiasing, true );

  QPalette plt = palette();

  RoundRect frame(rect(), 3);
  QColor baseColor( grooveColor() );
  drawSunken( &p, plt, frame, baseColor, hasFocus() ? focusColor() : QColor() );

  QRect contRect( sunkenContentsRect(rect()) );

  QRect hndRect;
  QRect valRect;
  int half_hnd = HND * 0.5;
  bool horiz = _ort == Qt::Horizontal;

  if( horiz ) {
    double valRange = contRect.width() - HND;
    int left = _lo * valRange;
    int right = _hi * valRange; right += HND;
    hndRect = QRect( left + contRect.x(), contRect.y(), right - left, contRect.height() );
    valRect = hndRect.adjusted( half_hnd, 1, -half_hnd, -1 );
  }
  else {
    double valRange = contRect.height() - HND;
    int up = (1.0 - _hi) * valRange;
    int down = (1.0 - _lo) * valRange; down += HND;
    hndRect = QRect( contRect.x(), contRect.y() + up, contRect.width(), down - up );
    valRect = hndRect.adjusted( 1, half_hnd, -1, -half_hnd);
  }

  // handle

  RoundRect handle(hndRect, 2);
  drawRaised( &p, plt, handle, plt.color(QPalette::Button).lighter(115) );

  // markers

  if( horiz ) {
    int mark_len = hndRect.height() - 4;

    QPoint pt( hndRect.left() + half_hnd, hndRect.top() + 2 );
    drawMarker( &p, plt, _ort, true, pt, mark_len );

    pt.setX( hndRect.right() - half_hnd + 1 );
    drawMarker( &p, plt, _ort, false, pt, mark_len );
  } else {
    int mark_len = hndRect.width() - 4;
    QPoint pt( hndRect.left() + 2, hndRect.top() + half_hnd );
    drawMarker( &p, plt, _ort, true, pt, mark_len );

    pt.setY( hndRect.bottom() - half_hnd + 1 );
    drawMarker( &p, plt, _ort, false, pt, mark_len );
  }

  // value region

  if( horiz ? valRect.width() > 2 : valRect.height() > 2 ) {
    p.setRenderHint( QPainter::Antialiasing, false );

    QColor c( plt.color(QPalette::ButtonText) );
    c.setAlpha(50);

    p.setPen(Qt::NoPen);
    p.setBrush(c);
    p.drawRect(valRect.adjusted( 1, 1, -1, -1 ));
  }
}
