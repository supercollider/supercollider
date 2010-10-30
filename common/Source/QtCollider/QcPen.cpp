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

#include "QcPen.h"
#include "Common.h"

#include <iostream>
#include <cstdio>

using namespace QtCollider;

QDataStream & operator<< ( QDataStream & s, QtCollider::Pen::Environment & e )
{
  int flags = e.flags;
  s << flags;
  if( flags & Pen::HasTransform ) s << e.transform;
  if( flags & Pen::HasPen ) s << e.pen;
  if( flags & Pen::HasBrush ) s << e.brush;
  if( flags & Pen::HasAntiAliasing ) s << e.antiAliasing;
  e.flags = 0;
  return s;
}

QDataStream & operator>> ( QDataStream & s, QtCollider::Pen::Environment & e )
{
  int flags;
  s >> flags;
  if( flags & Pen::HasTransform ) s >> e.transform;
  if( flags & Pen::HasPen ) s >> e.pen;
  if( flags & Pen::HasBrush ) s >> e.brush;
  if( flags & Pen::HasAntiAliasing ) s >> e.antiAliasing;
  e.flags = flags;
  return s;
}

void Pen::applyEnvironment( QPainter *p, const Environment &e )
{
  int flags = e.flags;
  if( flags & Pen::HasTransform ) {
    p->setTransform( e.transform * baseTransform );
  }
  if( flags & Pen::HasPen ) p->setPen( e.pen );
  if( flags & Pen::HasBrush ) p->setBrush( e.brush );
  if( flags & Pen::HasAntiAliasing ) p->setRenderHint( QPainter::Antialiasing, e.antiAliasing );
}

void Pen::reset() {
  env = Environment();
  path = QPainterPath();
  envs.clear();
}

void Pen::save()
{
  int action = Save;
  *score << action;
  *score << env;
  envs.push( env );
}

void Pen::restore()
{
  if( envs.count() ) {
    int action = Restore;
    *score << action;
    env = envs.pop();
  }
}

void Pen::draw( int style )
{
  int action;

  switch ( style ) {
    case 0:
    case 1:
      action = Fill; break;
    case 2:
      action = Stroke; break;
    case 3:
    case 4:
    default:
      action = FillAndStroke;
  }

  *score << action;
  *score << env;
  *score << path;

  path = QPainterPath();
}

void Pen::clip()
{
  int action = Clip;
  *score << action;
  *score << path;
}

void Pen::paint( QPainter *p )
{
  QDataStream stream( scoreData );
  int action;
  Environment env;
  QPainterPath path;

  QPen pen;
  QBrush brush;
  QRectF r;
  QPointF pt;
  QString str;

  p->setTransform( baseTransform );

  while( !stream.atEnd() ) {
    stream >> action;
    switch( action ) {

      case Restore:

        p->restore();
        break;

      case Save:

        stream >> env;
        applyEnvironment( p, env );
        p->save();
        break;

      case Clip:

        stream >> path;
        p->setClipPath( path );
        break;

      case Fill:
      case Stroke:
      case FillAndStroke:

        stream >> env;
        stream >> path;

        applyEnvironment( p, env );

        pen = p->pen();
        brush = p->brush();
        if( action == Fill ) p->setPen( Qt::NoPen );
        else if( action == Stroke ) p->setBrush( Qt::NoBrush );

        p->drawPath( path );

        p->setPen( pen );
        p->setBrush( brush );

        break;

      case StringAtPoint:

        stream >> env >> pt >> str;
        applyEnvironment( p, env );
        r = QFontMetrics( p->font() ).boundingRect( str );
        p->drawText( pt - r.topLeft(), str );
        break;

      case StringInRect:

        stream >> env >> r >> str;
        applyEnvironment( p, env );
        p->drawText( r, Qt::AlignTop | Qt::AlignLeft, str );
        break;

      default:

        qcErrorMsg( "Unknown Pen action! Aborting drawing." );
        path = QPainterPath();
        return;
    }
    path = QPainterPath();
  }
}

void Pen::addWedge( const QPointF &center,
                    float radius, float startAngle, float sweepAngle)
{
  path.moveTo( center );
  QRectF r;
  r.setSize( QSizeF( 2*radius, 2*radius ) );
  r.moveCenter( center );
  path.arcTo( r, startAngle, sweepAngle );
  path.closeSubpath();
}

void Pen::addAnnularWedge( const QPointF &center,
                           float innerRadius, float outerRadius,
                           float startAngle, float sweepAngle )
{
  QPainterPath annularWedge;
  annularWedge.moveTo( center );
  QRectF r;
  r.setSize( QSizeF( 2*outerRadius, 2*outerRadius ) );
  r.moveCenter( center );
  annularWedge.arcTo( r, startAngle, sweepAngle );
  annularWedge.closeSubpath();
  QPainterPath circle;
  QRectF r2( 0,0,2*innerRadius, 2*innerRadius );
  r2.moveCenter( center );
  circle.addEllipse( r2 );

  //FIXME this is VERY slow!!
  path.addPath( annularWedge - circle );
}

void Pen::stringAtPoint( const QPointF &pt, const QString &str )
{
  int action = StringAtPoint;
  *score << action << env << pt << str;
}

void Pen::stringInRect( const QRectF &rect, const QString &str )
{
  int action = StringInRect;
  *score << action << env << rect << str;
}
