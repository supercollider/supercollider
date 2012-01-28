/************************************************************************
*
* Copyright 2011-2012 Jakob Leben (jakob.leben@gmail.com)
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

#include "QcKnob.hpp"
#include "../QcWidgetFactory.h"
#include "../style/routines.hpp"

#include <QMouseEvent>
#include <QPainter>

using namespace QtCollider;

QC_DECLARE_QWIDGET_FACTORY(QcKnob);

QcKnob::QcKnob() :
  Style::Client(this),
  _value(0.f),
  _mode(0),
  _step(0.01),
  _centered(false)
{
  setFocusPolicy( Qt::StrongFocus );
  setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum );
}

void QcKnob::setValue( float val )
{
  _value = qBound( 0.f, val, 1.f );
  update();
}

void QcKnob::mousePressEvent( QMouseEvent *e )
{
  if( _mode == 0 ) {
    setValue( value( e->pos() ) );
    Q_EMIT( action() );
  }
  _prevPos = e->pos();
}

void QcKnob::mouseMoveEvent( QMouseEvent *e )
{
  if( _mode == 0 ) {
      setValue( value( e->pos() ) );
      Q_EMIT( action() );
  }
  else {
    int dif;
    dif = _mode == 1 ?
          e->pos().x() - _prevPos.x() :
          _prevPos.y() - e->pos().y();
    if( dif != 0 ) {
      float step = _step;
      modifyStep( &step );
      setValue( _value + dif * step );
      Q_EMIT( action() );
    }
  }
  _prevPos = e->pos();
}

void QcKnob::paintEvent( QPaintEvent * )
{
  using namespace QtCollider::Style;

  QPainter p(this);

  const QPalette &plt( palette() );
  QRect bounds( rect() );

  p.setRenderHint( QPainter::Antialiasing, true );

  int sz = qMin( bounds.width(), bounds.height() );
  float sz_2 = sz * 0.5f;
  QPointF center = QRectF(bounds).center();

  //p.rotate( -250 );
  p.save(); // center coordinate system
  p.translate( center );

  QRectF r( -sz_2, -sz_2, sz, sz );

  p.setBrush( Qt::NoBrush );

  QPen pen;
  pen.setWidth( 2 );

  r.adjust( 3, 3, -3, -3 );

  QColor cGroove = plt.color( QPalette::Window ).lighter( 115 );
  QColor cVal = plt.color( QPalette::ButtonText );

  p.save(); // groove & value indicator rotation

  p.rotate( -250.f );

  float valAng = _value * -320.f;

  if( _centered ) {
    float min = qMin( valAng, -160.f );
    float max = qMax( valAng, -160.f );

    pen.setColor( cGroove );
    p.setPen( pen );
    p.drawArc( r, -320.f * 16.f, (min + 320.f) * 16.f );
    p.drawArc( r, max * 16.f, -max * 16.f );

    pen.setColor( plt.color(QPalette::WindowText) );
    p.setPen( pen );
    p.drawArc( r, min * 16.f, (max - min) * 16.f );
  }
  else {
    pen.setColor( cGroove );
    p.setPen( pen );
    p.drawArc( r, -320.f * 16.f, (valAng + 320.f) * 16.f );

    pen.setColor( plt.color(QPalette::WindowText) );
    p.setPen( pen );
    p.drawArc( r, valAng * 16.f, -valAng * 16.f );
  }

  p.restore(); // groove & value indicator rotation

  r.adjust( 3, 3, -3, -3 );
  Ellipse shKnob(r);
  drawRaised( &p, plt, shKnob, plt.color( QPalette::Button ).lighter(105) );

  p.save(); // knob decoration scaling

  float factor = r.width() * 0.05;
  p.scale( factor, factor );

  p.save(); // knob pos indicator rotation

  p.rotate( 20 - valAng );

  QLineF line( 0, 0, 0, 9.9 );

  pen.setColor(cVal);
  pen.setWidth( 1.5 );
  pen.setCapStyle( Qt::FlatCap );
  p.setPen( pen );

  p.drawLine( line );

  p.restore(); // knob pos indicator rotation

  p.setBrush(cVal);
  p.setPen( Qt::NoPen );

  p.drawEllipse( QRectF( -4, -4, 8, 8 ) );

  p.restore(); // knob decoration scaling

  if( hasFocus() ) {
    QPen pen( focusColor() );
    pen.setWidth(2);
    p.setPen(pen);
    p.setBrush(Qt::NoBrush);
    p.drawEllipse(r.adjusted(1,1,-1,-1));
  }

  p.restore(); // center coordinate system
}

float QcKnob::value( const QPoint &pt )
{
  float angle = QLineF( rect().center(), pt ).angle();
  if( angle > 270.f ) angle -= 290;
  else angle += 70;
  return (320.f - angle) / 320.f;
}
