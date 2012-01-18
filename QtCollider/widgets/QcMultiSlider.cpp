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

#include "QcMultiSlider.h"
#include "../QcWidgetFactory.h"
#include "../style/routines.hpp"

#include <QApplication>
#include <QMouseEvent>
#include <QPainter>

#include <cmath>

QC_DECLARE_QWIDGET_FACTORY(QcMultiSlider);

QcMultiSlider::QcMultiSlider() :
  QtCollider::Style::Client(this),
  _currentIndex(0),
  _selectionSize(1),
  roundStep( 0.f ),
  editable( true ),
  ort( Qt::Vertical),
  elastic( false ),
  thumbSize( QSizeF( 12.f, 12.f ) ),
  gap( 1 ),
  drawRects( true ),
  drawLines( false ),
  isFilled( false ),
  highlight( false ),
  startIndex( 0 )
{
  setFocusPolicy( Qt::StrongFocus );
  setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
  connect( this, SIGNAL(modified()), this, SLOT(update()) );
  connect( this, SIGNAL(interacted()), this, SLOT(doAction()) );
}

void QcMultiSlider::setSliderCount( int newSize )
{
  while( newSize > _values.size() )
    _values.append( 0.f );
  while( newSize < _values.size() )
    _values.removeLast();
  update();
}


VariantList QcMultiSlider::values() const
{
  VariantList list;
  Q_FOREACH( float val, _values )
    list.data << QVariant( val );
  return list;
}

void QcMultiSlider::setValues( const VariantList & newValues )
{
  _values.clear();
  Q_FOREACH( QVariant var, newValues.data ) {
    // FIXME Seems there is a crash in case _values are out of range and
    // not clipped
    float value = var.value<float>();
    if( roundStep > 0.f )
        value = round( value / roundStep ) * roundStep;
    _values.append( qMax( 0.f, qMin( 1.f, value ) ) );
  }
  update();
}

void QcMultiSlider::setReference( const VariantList &reference )
{
  _ref.clear();
  Q_FOREACH( QVariant var, reference.data ) {
    _ref.append( qMax( 0.f, qMin( 1.f, var.value<float>() ) ) );
  }
  update();
}

float QcMultiSlider::value() const
{
  if( _currentIndex >= 0 && _currentIndex < _values.count() )
    return _values[_currentIndex];
  else
    return 0.f;
}

void QcMultiSlider::setValue( float f )
{
  if( _currentIndex >= 0 && _currentIndex < _values.count() )
    setValue( _currentIndex, f );

  update();
}

void QcMultiSlider::setStepSize( float f ) {
  if( roundStep == f ) return;
  roundStep = f;
  if( f == 0.f ) return;
  int c = _values.count();
  int i;
  for( i=0; i<c; ++i ) setValue( i, _values[i] );
  update();
}

void QcMultiSlider::setIndex( int i ) {
  if( i >= 0 && i < _values.count() ) {
    _currentIndex = i;
    _selectionSize = 1;
    update();
  }
}

void QcMultiSlider::setSelectionSize( int i ) {
  _selectionSize = qMin(i, _values.count() - _currentIndex);
  update();
}

inline void QcMultiSlider::setValue( int index, float value )
{
  if( roundStep > 0.f )
      value = round( value / roundStep ) * roundStep;
  _values.replace( index, qMax( 0.f, qMin( 1.f, value ) ) );
}

QRect QcMultiSlider::contentsRect()
{
  return QtCollider::Style::sunkenContentsRect(rect()).adjusted(2, 2,-2,-2);
}

QRectF QcMultiSlider::valueRect()
{
  QRectF r( QtCollider::Style::sunkenContentsRect(rect()).adjusted(2, 2,-2,-2) );
  if( !isFilled ) {
    double half_hnd = thumbSize.height() * 0.5;
    if( ort == Qt::Horizontal )
      return r.adjusted( half_hnd, 0, -half_hnd, 0 );
    else
      return r.adjusted( 0, half_hnd, 0, -half_hnd );
  }
  return r;
}

void QcMultiSlider::mousePressEvent( QMouseEvent *e )
{
  using namespace QtCollider::Style;

  moveOrigin = e->pos();
  int c = _values.count();

  if( !c ) return;

  QRectF r( valueRect() );

  int i;
  float val;

  if( ort == Qt::Vertical ) {
    float step = elastic ? (float) r.width() / c : thumbSize.width() + gap;
    i = step > 0.f ? (moveOrigin.x() - r.x()) / step : 0;
    val = yValue((qreal)moveOrigin.y(), r);
  }
  else {
    float step = elastic ? (float) height() / c : thumbSize.width() + gap;
    i = step > 0.f ? (moveOrigin.y() - r.y()) / step : moveOrigin.y();
    val = xValue((qreal)moveOrigin.x(), r);
  }

  i += startIndex;

  if( i >= 0 && i < c ) {
    _currentIndex = i;
    _selectionSize = 1;

    if( editable ) setValue( i, val );
    if( editable || highlight ) Q_EMIT( modified() );

    Q_EMIT( interacted() );
  }
}

void QcMultiSlider::mouseMoveEvent( QMouseEvent *e )
{
  using namespace QtCollider::Style;

  if( !e->buttons() ) return;

  int c = _values.count();

  QPoint pos = e->pos();

  if( !c ) {
    moveOrigin = pos;
    return;
  }

  QRectF r( valueRect() );

  float xOrig, yOrig, xDest, yDest, xStep;
  int i;

  if( ort == Qt::Vertical ) {
    xOrig = moveOrigin.x();
    yOrig = yValue(moveOrigin.y(), r);
    xDest = pos.x();
    yDest = yValue(pos.y(), r);
    xStep = elastic ? (float) r.width() / c : thumbSize.width() + gap;
    i = floor( (xOrig - r.left()) / xStep );
  }
  else {
    xOrig = moveOrigin.y();
    yOrig = xValue(moveOrigin.x(), r);
    xDest = pos.y();
    yDest = xValue(pos.x(), r);
    xStep = elastic ? (float) r.height() / c : thumbSize.width() + gap;
    i = floor( (xOrig - r.top()) / xStep );
  }

  float x = (i * xStep) - xOrig;
  float xDif = xDest - xOrig;
  float k = xDif != 0.f ? ( yDest - yOrig ) / xDif : 0.f;
  float yStep = xStep * k;

  i += startIndex;

  if( xDif > 0.f )
  {
    x+=xStep;
    float y = x * k + yOrig;
    while( i < c && x < xDif ) {
      if( i >= 0 && editable ) {
        setValue( i, y );
      }
      y += yStep;
      x+=xStep;
      i++;
    }
  }
  else if( xDif < 0.f )
  {
    float y = x * k + yOrig;
    while( i >= 0 && x > xDif ) {
      if( i < c && editable ) {
        setValue( i, y );
      }
      y -= yStep;
      x-=xStep;
      i--;
    }
  }

  if( i < c && i >= 0 ) {
    _currentIndex = i;
    _selectionSize = 1;

    if( editable ) setValue( i, yDest );
    if( editable || highlight ) Q_EMIT( modified() );
  }

  moveOrigin = pos;

  Q_EMIT( interacted() );
}

void QcMultiSlider::paintEvent( QPaintEvent *e )
{
  using namespace QtCollider::Style;

  Q_UNUSED(e);
  QPainter p(this);
  p.setRenderHint( QPainter::Antialiasing, true );

  QPalette pal = palette();

  RoundRect frame(rect(), 3);
  drawSunken( &p, pal, frame, pal.color(QPalette::Base), hasFocus() ? focusColor() : QColor() );

  if( !_values.count() ) return;

  QRect bounds( contentsRect() );
  p.setClipRect( bounds.adjusted(1,1,-1,-1) );

  QColor fillColor =
    _fillColor.isValid() ? _fillColor : pal.color( QPalette::Text );
  QColor strokeColor =
    _strokeColor.isValid() ? _strokeColor : pal.color( QPalette::Text );

  float iRange, vRange, iOrig, vOrig;
  if( ort == Qt::Vertical ) {
    iRange = bounds.width(); vRange = bounds.height();
    iOrig = bounds.left(); vOrig = bounds.top();
  } else {
    iRange = bounds.height(); vRange = bounds.width();
    iOrig = bounds.top(); vOrig = bounds.left();
  }

  float spacing = elastic ?
                  iRange / _values.count() :
                  thumbSize.width() + gap;

  float iOffset = - startIndex * spacing + (ort == Qt::Horizontal ? bounds.x() : bounds.y());

  float vOffset = isFilled ? 0.f : thumbSize.height() * 0.5f;
  if( ort == Qt::Vertical ) vOffset = vRange - vOffset;
  vOffset += vOrig;

  float vScale = vRange;
  if( !isFilled ) vScale -= thumbSize.height();
  if( ort == Qt::Vertical ) vScale = -vScale;

  float iPos;
  float vPos;
  float iSize;
  float vSize;

  float *x, *y, *w, *h;
  if( ort == Qt::Vertical ) {
    x = &iPos; y = &vPos; w = &iSize, h = &vSize;
  }
  else {
    x = &vPos; y = &iPos; w = &vSize, h = &iSize;
  }

  //highlight current index / selection

  if( highlight ) {
    iSize = _selectionSize * spacing;
    vSize = vRange;
    iPos =  _currentIndex * spacing + iOffset;
    vPos = 0.f;
    QRectF hlRect;
    hlRect.setWidth( *w );
    hlRect.setHeight( *h );
    hlRect.translate( *x, *y );

    QColor hlColor = fillColor;
    hlColor.setAlpha( 70 );
    p.fillRect( hlRect, hlColor );
  }

  p.setPen( strokeColor );

  if( drawLines ) {
    bool fill = isFilled & !drawRects;

    p.setRenderHint( QPainter::Antialiasing, true );
    if( fill ) p.setBrush( fillColor );

    QPainterPath path;

    // construct value line
    iPos = spacing * 0.5 + iOffset;
    vPos = ( _values[0] * vScale ) + vOffset;
    path.moveTo( *x, *y );

    int vc = _values.count();
    for( int i = 1; i < vc; ++i ) {
      iPos += spacing;
      vPos = ( _values[i] * vScale ) + vOffset;
      path.lineTo( *x, *y );
    }

    // construct reference line
    int rc = _ref.count();
    if( ( rc || fill ) && vc) {
      int i = vc - 1;
      vPos = ( (i < rc ? _ref[i] : 0.f) * vScale ) + vOffset;
      //NOTE re-use last iPos from value line;

      if( fill ) path.lineTo(*x,*y);
      else path.moveTo(*x,*y);

      while( --i >= 0 ) {
        iPos -= spacing;
        vPos = ( (i < rc ? _ref[i] : 0.f) * vScale ) + vOffset;
        path.lineTo(*x,*y);
      }

      if( fill ) path.closeSubpath();
    }

    p.drawPath( path );
  }

  if( drawRects ) {
    p.setRenderHint( QPainter::Antialiasing, false );
    p.setBrush( fillColor );

    QRectF r;
    // 1 pixel smaller because of stroke on right and bottom edge
    //FIXME erm, doesn't work right in direction of value
    iSize = elastic ?
            qMin( (qreal)spacing, thumbSize.width() ) :
            thumbSize.width();
    iSize -= 1;

    if( !isFilled ) {
      vSize = thumbSize.height() - 1;
      r.setSize( QSizeF( *w, *h ) );
    }

    iPos = spacing * 0.5 + iOffset;

    QPointF pt;
    int vc = _values.count();
    int rc = _ref.count();
    for( int i = 0; i < vc; ++i ) {
      if( isFilled ) {
        float ref = i < rc ? _ref[i] : 0.f;
        float val = _values[i];
        vPos = ((ref + val) * 0.5 * vScale) + vOffset;
        vSize = ( val - ref );
        vSize *= vSize > 0.f ? vScale : -vScale;
        r.setSize( QSizeF( *w, *h ) );
        pt.setX(*x); pt.setY(*y);
        r.moveCenter( pt );
        p.drawRect( r );
      }
      else {
        if( rc ) {
          float ref = i < rc ? _ref[i] : 0.f;
          vPos = ( ref * vScale ) + vOffset;
          pt.setX(*x); pt.setY(*y);
          r.moveCenter( pt );
          p.drawRect( r );
        }

        vPos = ( _values[i] * vScale ) + vOffset;
        pt.setX(*x); pt.setY(*y);
        r.moveCenter( pt );
        p.drawRect( r );
      }
      iPos += spacing;
    }
  }
}

void QcMultiSlider::doAction()
{
  Qt::KeyboardModifier ctrlMod =
#ifdef Q_OS_MAC
      Qt::MetaModifier;
#else
      Qt::ControlModifier;
#endif

  if( QApplication::keyboardModifiers() & ctrlMod )
    Q_EMIT( metaAction() );
  else
    Q_EMIT( action() );
}
