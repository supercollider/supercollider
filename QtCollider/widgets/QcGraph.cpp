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
#include "../QcWidgetFactory.h"

#include <QPainter>
#include <QMouseEvent>
#include <QApplication>

#include <cmath>

static QcWidgetFactory<QcGraph> factory;

QcGraph::QcGraph() :
  _thumbSize( QSize( 8, 8 ) ),
  _strokeColor( QColor(0,0,0) ),
  _selColor( QColor(255,0,0) ),
  _gridColor( QColor(225,225,225) ),
  _drawLines( true ),
  _drawRects( true ),
  _editable( true ),
  _step( 0.f ),
  _gridOn( false ),
  selIndex( -1 ),
  dragIndex( -1 )
{
  setFocusPolicy( Qt::StrongFocus );
  setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
}

VariantList QcGraph::value() const
{
  VariantList x;
  VariantList y;
  QList<QcGraphElement*> elems = _model.elements();
  Q_FOREACH( QcGraphElement* e, elems ) {
    QPointF val = e->value;
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
  QcGraphElement *e = _model.elementAt(selIndex);
  return e->value.x();
}

float QcGraph::currentY() const
{
  if( selIndex < 0 ) return 0.f;
  QcGraphElement *e = _model.elementAt(selIndex);
  return e->value.y();
}

void QcGraph::setValue( const VariantList &list )
{
  if( list.data.count() != 2 ) return;
  VariantList xList = list.data[0].value<VariantList>();
  VariantList yList = list.data[1].value<VariantList>();

  int newc = qMin( xList.data.count(), yList.data.count() );
  if( !newc ) return;

  int c = _model.elementCount();
  while( c > newc )
  {
    --c;
    _model.removeAt( c );
  }

  int i;
  for( i = 0; i < newc; ++i )
  {
    QPointF val( xList.data[i].value<float>(),
                yList.data[i].value<float>() );
    if( i < c ) {
      QcGraphElement *e = _model.elementAt(i);
      setValue( e, val );
    }
    else {
      QcGraphElement *e = new QcGraphElement();
      setValue( e, val );
      _model.append( e );
    }
  }

  selIndex = -1;

  update();
}

void QcGraph::setStrings( const VariantList &list )
{
  int strc = list.data.count();
  int c = _model.elementCount();
  int i;
  for( i = 0; i < c && i < strc; ++i ) {
    QcGraphElement *e = _model.elementAt(i);
    e->text = list.data[i].toString();
  }
  update();
}

void QcGraph::setCurves( const VariantList & curves )
{
  if( curves.data.size() == 1 ) {
    QVariant var = curves.data[0];
    Q_FOREACH( QcGraphElement *e, _model.elements() )
      e->setCurveType( var );
  }
  else if( curves.data.size() == _model.elementCount() ) {
    for( int i=0; i<curves.data.size(); ++i ) {
      QVariant var = curves.data[i];
      _model.elementAt(i)->setCurveType( var );
    }
  }
}

void QcGraph::setStringAt( int i, const QString & str )
{
  int c = _model.elementCount();
  if( i > 0 && i < c ) {
    QcGraphElement *e = _model.elementAt(i);
    e->text = str;
    update();
  }
}

void QcGraph::connectElements( int src, VariantList targets )
{
  int c = _model.elementCount();
  if( src < 0 || src >= c ) return;

  Q_FOREACH( QVariant var, targets.data ) {
    int trg = var.toInt();
    if( trg < 0 || trg >= c ) continue;
    _model.connect( src, trg );
  }

  update();
}

void QcGraph::setCurrentX( float f )
{
  if( selIndex < 0 ) return;
  QcGraphElement *e = _model.elementAt(selIndex);
  QPointF val = e->value;
  val.setX( f );
  setValue( e, val );
  update();
}

void QcGraph::setCurrentY( float f )
{
  if( selIndex < 0 ) return;
  QcGraphElement *e = _model.elementAt(selIndex);
  QPointF val = e->value;
  val.setY( f );
  setValue( e, val );
  update();
}

void QcGraph::setFillColor( const QColor & color )
{
  int c = _model.elementCount();
  for( int i=0; i<c; ++i ) {
    QcGraphElement *e = _model.elementAt(i);
    e->fillColor = color;
  }
  update();
}

void QcGraph::setFillColorAt( int i, const QColor & color )
{
  int c = _model.elementCount();
  if( i > 0 && i < c ) {
    QcGraphElement *e = _model.elementAt(i);
    e->fillColor = color;
    update();
  }
}

void QcGraph::setEditableAt( int i, bool b )
{
  int c = _model.elementCount();
  if( i > 0 && i < c ) {
    QcGraphElement *e = _model.elementAt(i);
    e->editable = b;
  }
}

void QcGraph::setStep( float f )
{
  _step = qMax( 0.f, f );

  QList<QcGraphElement*> elems = _model.elements();
  Q_FOREACH( QcGraphElement *e, elems ) {
    setValue( e, e->value );
  }

  if( elems.count() ) update();
}

inline void QcGraph::setValue( QcGraphElement * e, const QPointF& pt )
{
  float x = pt.x();
  float y = pt.y();

  if( _step > 0.f ) {
    x = round( x / _step ) * _step;
    y = round( y / _step ) * _step;
  }

  x = qMax( 0.f, qMin( 1.f, x ) );
  y = qMax( 0.f, qMin( 1.f, y ) );
  e->value = QPointF( x, y );
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
  QPalette plt( palette() );

  p.setPen( plt.color( QPalette::Mid ) );
  p.setBrush( Qt::NoBrush );
  p.drawRect( rect().adjusted(0,0,-1,-1) );

  QRect contentsRect( _thumbSize.width() * 0.5f, _thumbSize.height() * 0.5f,
                      width() - _thumbSize.width(), height() - _thumbSize.height() );
  p.setPen( _gridColor );
  p.setBrush( plt.color( QPalette::Base ) );
  p.drawRect( contentsRect );

  //draw grid;
  if( _gridOn ) {
    float dx = _gridMetrics.x();
    float dy = _gridMetrics.y();
    float cl = contentsRect.left();
    float cr = contentsRect.right();
    float ct = contentsRect.top();
    float cb = contentsRect.bottom();

    if( contentsRect.width() > 0.f && dx > 0.f && dx < 1.f ) {
      dx *= contentsRect.width();
      float x = cl;
      while( x < cr ) {
        p.drawLine( x, ct, x, cb );
        x += dx;
      }
    }

    if( contentsRect.height() > 0.f && dy > 0.f && dy < 1.f ) {
      dy *= contentsRect.height();
      float y = cb;
      while( y > ct ) {
        p.drawLine( cl, y, cr, y );
        y -= dy;
      }
    }
  }

  QList<QcGraphElement*> elems = _model.elements();

  int c = elems.count();
  if( !c ) return;

  p.setPen( _strokeColor );

  // draw lines;
  if( _drawLines ) {

    QPainterPath lines;
    QList<QcGraphModel::Connection> conns = _model.connections();

    if( conns.count() ) {

      Q_FOREACH( QcGraphModel::Connection c, conns ) {
        lines.moveTo( pos( c.a->value ) );
        lines.lineTo( pos( c.b->value ) );
      }

    }
    else {

      QPointF pt = pos( elems[0]->value );
      lines.moveTo( pt );
      int i;

      for( i = 1; i < c; ++i ) {
        pt = pos( elems[i]->value );
        lines.lineTo( pt );
      }

    }

    p.setBrush( Qt::NoBrush );
    p.drawPath( lines );

  }

  // draw rects and strings
  if( _drawRects ) {

    QRectF rect; rect.setSize( _thumbSize );
    QPointF pt;
    int i;

    for( i = 0; i < c; ++i ) {

      QcGraphElement *e = elems[i];

      if( i == selIndex )
        p.setBrush( _selColor );
      else
        p.setBrush( e->fillColor );

      pt = pos( e->value );

      rect.moveCenter( pt );
      p.drawRect( rect.adjusted(0,0,-1,-1) );

      QString text = e->text;
      if( !text.isEmpty() ) {
        p.drawText( rect, Qt::AlignCenter, text );
      }

    }

  }
}

void QcGraph::mousePressEvent( QMouseEvent *ev )
{
  QList<QcGraphElement*> elems = _model.elements();
  int c = elems.count();
  if( !c ) return;

  QPointF mpos = ev->pos();

  QRectF r;
  r.setSize( _thumbSize );

  int i;
  for( i = 0; i < c; ++i ) {
    QcGraphElement *e = elems[i];
    QPointF pt = pos( e->value );
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
  if( !_editable ) return;
  if( dragIndex < 0 || dragIndex >= _model.elementCount() ) return;

  QcGraphElement *e = _model.elementAt(dragIndex);
  if( !e->editable ) return;

  QPointF mpos = ev->pos() - dragDelta;
  setValue( e, value( mpos ) );
  update();
  doAction( ev->modifiers() );
}

void QcGraph::keyPressEvent( QKeyEvent *event )
{
  if( selIndex < 0 ) return;

  QcGraphElement *e = _model.elementAt(selIndex);
  bool edit = _editable && e->editable;
  QPointF val = e->value;

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

  if( val != e->value ) {
    setValue( e, val );
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
    Q_EMIT( metaAction() );
  else
    Q_EMIT( action() );
}
