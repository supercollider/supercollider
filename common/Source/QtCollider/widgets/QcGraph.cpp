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

#include "QcGraph.h"

#include <QPainter>
#include <QMouseEvent>
#include <QApplication>

#include <cmath>

QcGraph::QcGraph() :
  _thumbSize( QSize( 8, 8 ) ),
  _fillColor( QColor(0,0,0) ),
  _strokeColor( QColor(0,0,0) ),
  _selColor( QColor(255,0,0) ),
  _gridColor( QColor(200,200,200) ),
  _drawLines( true ),
  _drawRects( true ),
  _editable( true ),
  _step( 0.f ),
  _gridOn( false ),
  selIndex( -1 ),
  dragIndex( -1 )
{
  setAttribute( Qt::WA_OpaquePaintEvent, true );
}

VariantList QcGraph::value() const
{
  VariantList x;
  VariantList y;
  Q_FOREACH( Element e, _elems ) {
    QPointF val = e.value;
    x.data.append( val.x() );
    y.data.append( val.y() );
  }

  VariantList values;
  values.data.append( QVariant::fromValue<VariantList>(x) );
  values.data.append( QVariant::fromValue<VariantList>(y) );

  return values;
}

float QcGraph::currentX() const
{
  if( selIndex < 0 ) return 0.f;
  Element e = _elems[selIndex];
  return e.value.x();
}

float QcGraph::currentY() const
{
  if( selIndex < 0 ) return 0.f;
  Element e = _elems[selIndex];
  return e.value.y();
}

void QcGraph::setValue( const VariantList &list )
{
  if( list.data.count() != 2 ) return;
  VariantList xList = list.data[0].value<VariantList>();
  VariantList yList = list.data[1].value<VariantList>();

  int newc = qMin( xList.data.count(), yList.data.count() );
  if( !newc ) return;

  int c = _elems.count();
  while( c > newc )
  {
    _elems.removeLast();
    --c;
  }

  int i;
  for( i = 0; i < newc; ++i )
  {
    QPointF val( xList.data[i].value<float>(),
                yList.data[i].value<float>() );
    if( i < c ) {
      Element e = _elems[i];
      setValue( e, val );
      _elems.replace( i, e );
    }
    else {
      Element e;
      setValue( e, val );
      _elems.append( e );
    }
  }

  selIndex = -1;

  update();
}

void QcGraph::setStrings( const VariantList &list )
{
  int strc = list.data.count();
  int c = _elems.count();
  int i;
  for( i = 0; i < c && i < strc; ++i ) {
    Element e = _elems[i];
    e.text = list.data[i].toString();
    _elems.replace( i, e );
  }
  update();
}

void QcGraph::setCurrentX( float f )
{
  if( selIndex < 0 ) return;
  Element e = _elems[selIndex];
  QPointF val = e.value;
  val.setX( f );
  setValue( e, val );
  _elems.replace( selIndex, e );
  update();
}

void QcGraph::setCurrentY( float f )
{
  if( selIndex < 0 ) return;
  Element e = _elems[selIndex];
  QPointF val = e.value;
  val.setY( f );
  setValue( e, val );
  _elems.replace( selIndex, e );
  update();
}

void QcGraph::setStep( float f )
{
  _step = qMax( 0.f, f );

  int c = _elems.count();
  if( !c ) return;

  int i;
  for( i = 0; i < c; ++i ) {
    Element e = _elems[i];
    setValue( e, e.value );
    _elems.replace( i, e );
  }

  update();
}

inline void QcGraph::setValue( Element &e, const QPointF& pt )
{
  float x = pt.x();
  float y = pt.y();

  if( _step > 0.f ) {
    x = round( x / _step ) * _step;
    y = round( y / _step ) * _step;
  }

  x = qMax( 0.f, qMin( 1.f, x ) );
  y = qMax( 0.f, qMin( 1.f, y ) );
  e.value = QPointF( x, y );
}

QPointF QcGraph::pos( const QPointF & value )
{
  float x, y, xRange, yRange;

  xRange = width() - _thumbSize.width();
  yRange = height() - _thumbSize.height();

  if( xRange > 0 )
    x = ( value.x() * xRange ) + ( _thumbSize.width() * 0.5f );
  else
    x = 0.f;

  if( yRange > 0 )
    y = ( value.y() * -yRange ) - ( _thumbSize.height() * 0.5f ) + height();
  else
    y = 0.f;

  return QPointF( x, y );
}

QPointF QcGraph::value( const QPointF & pos )
{
  float x, y, xRange, yRange;

  xRange = width() - _thumbSize.width();
  yRange = height() - _thumbSize.height();

  if( xRange > 0 )
    x = ( pos.x() - ( _thumbSize.width() * 0.5f ) ) / xRange;
  else
    x = 0.f;

  if( yRange > 0 )
    y = ( pos.y() + ( _thumbSize.height() * 0.5f ) - height() ) / -yRange;
  else
    y = 0.f;

  return QPointF( x, y );
}

void QcGraph::paintEvent( QPaintEvent * )
{
  QPainter p( this );

  p.fillRect( rect(), palette().color( QPalette::Base ) );

  //draw grid;
  if( _gridOn ) {
    p.setPen( _gridColor );

    float dx = _gridMetrics.x();
    float dy = _gridMetrics.y();

    float contentsW = width() - _thumbSize.width();
    if( contentsW > 0.f && dx > 0.f && dx < 1.f ) {
      dx *= contentsW;
      float x = _thumbSize.width() * 0.5f;
      while( x < width() ) {
        p.drawLine( x, 0, x, height() );
        x += dx;
      }
    }

    float contentsH = height() - _thumbSize.height();
    if( contentsH > 0.f && dy > 0.f && dy < 1.f ) {
      dy *= contentsH;
      float y = height() - ( _thumbSize.height() * 0.5f );
      while( y >= 0.f ) {
        p.drawLine( 0, y, width(), y );
        y -= dy;
      }
    }
  }

  int c = _elems.count();
  if( !c ) return;

  p.setPen( _strokeColor );

  QPointF pt = pos( _elems[0].value );
  QRectF rect; rect.setSize( _thumbSize );
  int i;

  // draw lines;
  if( _drawLines ) {
    QPainterPath lines;
    lines.moveTo( pt );
    for( i = 1; i < c; ++i ) {
      pt = pos( _elems[i].value );
      lines.lineTo( pt );
    }

    p.setBrush( Qt::NoBrush );
    p.drawPath( lines );
  }

  // draw rects and strings
  if( _drawRects ) {
    for( i = 0; i < c; ++i ) {
      Element e = _elems[i];

      if( i == selIndex )
        p.setBrush( _selColor );
      else
        p.setBrush( _fillColor );

      pt = pos( e.value );

      rect.moveCenter( pt );
      p.drawRect( rect.adjusted(0,0,-1,-1) );

      QString text = e.text;
      if( !text.isEmpty() ) {
        p.drawText( rect, Qt::AlignCenter, text );
      }
    }
  }
}

void QcGraph::mousePressEvent( QMouseEvent *ev )
{
  int c = _elems.count();
  if( !c ) return;

  QPointF mpos = ev->pos();

  QRectF r;
  r.setSize( _thumbSize );

  int i;
  for( i = 0; i < c; ++i ) {
    Element e = _elems[i];
    QPointF pt = pos( e.value );
    r.moveCenter( pt );
    if( r.contains( mpos ) ) {
      dragIndex = selIndex = i;
      dragDelta = mpos - pt;
      update();
      return;
    }
  }

  dragIndex = -1;
  update();
}

void QcGraph::mouseMoveEvent( QMouseEvent *ev )
{
  if( dragIndex < 0 || dragIndex >= _elems.count() ) return;
  if( !_editable ) return;

  Element e = _elems[dragIndex];
  if( !e.editable ) return;

  QPointF mpos = ev->pos() - dragDelta;
  setValue( e, value( mpos ) );
  _elems.replace( dragIndex, e );
  update();
  doAction( ev->modifiers() );
}

void  QcGraph::keyPressEvent( QKeyEvent *event )
{
  if( selIndex < 0 ) return;

  Element e = _elems[selIndex];
  bool edit = _editable && e.editable;
  QPointF val = e.value;

  switch( event->key() ) {
    case Qt::Key_Up:

      if( edit ) val.setY( val.y() + _step );
      break;

    case Qt::Key_Down:

      if( edit ) val.setY( val.y() - _step );
      break;

    case Qt::Key_Right:

      if( event->modifiers() & Qt::ShiftModifier ) {
        if( edit ) val.setX( val.x() + _step );
      }
      else { setIndex( selIndex+1 ); return; }
      break;

    case Qt::Key_Left:

      if( event->modifiers() & Qt::ShiftModifier ) {
        if( edit ) val.setX( val.x() - _step );
      }
      else { setIndex( selIndex-1 ); return; }
      break;
  }

  if( val != e.value ) {
    setValue( e, val );
    _elems.replace( selIndex, e );
    update();
    doAction( event->modifiers() );
  }
}

void QcGraph::doAction( Qt::KeyboardModifiers mods )
{
  Qt::KeyboardModifier ctrlMod =
#ifdef Q_OS_MAC
      Qt::MetaModifier;
#else
      Qt::ControlModifier;
#endif

  if( mods & ctrlMod )
    QApplication::postEvent( this, new ScMethodCallEvent( "doMetaAction" ) );
  else
    QApplication::postEvent( this, new ScMethodCallEvent( "doAction" ) );
}
