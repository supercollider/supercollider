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

#include "QcSlider2D.h"
#include "../QcWidgetFactory.h"

#include <QKeyEvent>
#include <QMouseEvent>
#include <QApplication>
#include <QPainter>

static QcWidgetFactory<QcSlider2D> factory;

QcSlider2D::QcSlider2D() :
  _x( 0.f ),
  _y( 0.f ),
  _thumbSize( QSize( 12, 12 ) ),
  _step( 0.01f )
{
  setFocusPolicy( Qt::StrongFocus );
  setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
}

QRect QcSlider2D::thumbRect()
{
  QRect r;

  int xPos = _x * (width() - _thumbSize.width());
  int yPos = (1.f - _y) * (height() - _thumbSize.height());

  r.setX( xPos );
  r.setY( yPos );
  r.setSize( _thumbSize );

  return r;
}

QPointF QcSlider2D::valueFromPos( const QPoint pos )
{
  float x = (float) (pos.x() - (_thumbSize.width() * 0.5f) ) /
            (width() - _thumbSize.width());
  float y = (float) (height() - pos.y() - (_thumbSize.width() * 0.5f) ) /
            ( height() - _thumbSize.height() );
  return QPointF( x, y );
}

void QcSlider2D::setValue( const QPointF val, bool doAction )
{
  float x = qMax( 0.f, qMin( 1.f, (float)val.x() ) );
  float y= qMax( 0.f, qMin( 1.f, (float)val.y() ) );
  if( x != _x || y != _y ) {
    _x = x;
    _y = y;
    update();
    if( doAction ) Q_EMIT( action() );
  }
}

void QcSlider2D::mouseMoveEvent ( QMouseEvent * ev )
{
  setValue( valueFromPos( ev->pos() ) );
}

void QcSlider2D::mousePressEvent ( QMouseEvent * ev )
{
  setValue( valueFromPos( ev->pos() ) );
}

void QcSlider2D::keyPressEvent ( QKeyEvent *e )
{
  float step = _step;
  switch( e->key() ) {
    case Qt::Key_Up:
      modifyStep( &step );
      setValue( QPointF( _x, _y + step ) ); break;
    case Qt::Key_Down:
      modifyStep( &step );
      setValue( QPointF( _x, _y - step ) ); break;
    case Qt::Key_Right:
      modifyStep( &step );
      setValue( QPointF( _x + step, _y ) ); break;
    case Qt::Key_Left:
      modifyStep( &step );
      setValue( QPointF( _x - step, _y ) ); break;
    case Qt::Key_N:
      setValue( QPointF( 0.f, 0.f ) ); break;
    case Qt::Key_X:
      setValue( QPointF( 1.f, 1.f ) ); break;
    case Qt::Key_C:
      setValue( QPointF( 0.5f, 0.5f ) ); break;
    case Qt::Key_R:
      Q_EMIT( randomize() );
      break;
    default: QWidget::keyPressEvent( e );
  }
}

void QcSlider2D::paintEvent ( QPaintEvent *e )
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
