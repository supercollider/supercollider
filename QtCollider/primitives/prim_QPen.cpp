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

#include "PrimitiveDefiner.h"
#include "../QcPen.h"
#include "../Slot.h"

namespace QtCollider {
  static Pen *currentPen = 0;
  //static QMutex penMutex;
}

void QtCollider::beginPen( Pen *pen ) {
  delete pen->score;

  pen->path = QPainterPath();

  Pen::Environment env;
  env.setPen( QColor(0,0,0) );
  env.setBrush( QColor(0,0,0) );
  env.setAntiAliasing( true );

  pen->env = env;

  pen->scoreData.clear();

  pen->score = new QDataStream( &pen->scoreData, QIODevice::ReadWrite );

  currentPen = pen;
}

void QtCollider::endPen()
{
  currentPen = 0;
}

using namespace QtCollider;

int QPen_Save (struct VMGlobals *, int)
{
  QtCollider::currentPen->save();
  return errNone;
}

int QPen_Restore (struct VMGlobals *, int)
{
  QtCollider::currentPen->restore();
  return errNone;
}

int QPen_Clear (struct VMGlobals *, int)
{
  QtCollider::currentPen->clear();
  return errNone;
}

int QPen_FillColor (struct VMGlobals *g, int)
{
  QColor color = Slot::toColor( g->sp );
  QtCollider::currentPen->setFillColor( color );
  return errNone;
}

int QPen_StrokeColor (struct VMGlobals *g, int)
{
  QColor color = Slot::toColor( g->sp );
  QtCollider::currentPen->setStrokeColor( color );
  return errNone;
}

int QPen_Width ( struct VMGlobals *g, int )
{
  float width;
  if( slotFloatVal( g->sp, &width ) ) return errWrongType;
  QtCollider::currentPen->setWidth( width );
  return errNone;
}

int QPen_AntiAliasing (struct VMGlobals *g, int)
{
  bool b = IsTrue( g->sp );
  if( !b && !IsFalse( g->sp ) ) return errWrongType;
  QtCollider::currentPen->setAntiAliasing(b);
  return errNone;
}

int QPen_SetFont (struct VMGlobals *g, int)
{
  QtCollider::currentPen->setFont( Slot::toFont( g->sp ) );
  return errNone;
}

int QPen_Translate ( struct VMGlobals *g, int )
{
  float x, y;
  if( slotFloatVal( g->sp-1, &x ) ) return errWrongType;
  if( slotFloatVal( g->sp, &y ) ) return errWrongType;
  QtCollider::currentPen->translate( x, y );
  return errNone;
}

int QPen_Scale ( struct VMGlobals *g, int )
{
  float x, y;
  if( slotFloatVal( g->sp-1, &x ) ) return errWrongType;
  if( slotFloatVal( g->sp, &y ) ) return errWrongType;
  QtCollider::currentPen->scale( x, y );
  return errNone;
}

int QPen_Shear ( struct VMGlobals *g, int )
{
  float x, y;
  if( slotFloatVal( g->sp-1, &x ) ) return errWrongType;
  if( slotFloatVal( g->sp, &y ) ) return errWrongType;
  QtCollider::currentPen->shear( x, y );
  return errNone;
}

int QPen_Rotate ( struct VMGlobals *g, int )
{
  float angle, x, y;
  if( slotFloatVal( g->sp-2, &angle ) ) return errWrongType;
  if( slotFloatVal( g->sp-1, &x ) ) return errWrongType;
  if( slotFloatVal( g->sp, &y ) ) return errWrongType;
  QtCollider::currentPen->rotate( angle, x, y );
  return errNone;
}

int QPen_Transform ( struct VMGlobals *g, int )
{
  VariantList list = Slot::toVariantList( g->sp );
  if( list.data.count() < 6 ) return errWrongType;
  float f[6];
  int i = 6;
  while( i ) {
    --i;
    QVariant var = list.data[i];
    if( !var.canConvert( QVariant::Double ) ) return errWrongType;
    f[i] = list.data[i].value<float>();
  }
  QTransform t( f[0], f[1], f[2], f[3], f[4], f[5] );
  QtCollider::currentPen->setTransform( t );
  return errNone;
}

int QPen_MoveTo( struct VMGlobals *g, int )
{
  QPointF point = Slot::toPoint( g->sp );
  QtCollider::currentPen->moveTo( point );
  return errNone;
}

int QPen_LineTo( struct VMGlobals *g, int )
{
  QPointF point = Slot::toPoint( g->sp );
  QtCollider::currentPen->lineTo( point );
  return errNone;
}

int QPen_CubicTo( struct VMGlobals *g, int )
{
  QPointF endPoint = Slot::toPoint( g->sp-2 );
  QPointF cPoint1 = Slot::toPoint( g->sp-1 );
  QPointF cPoint2 = Slot::toPoint( g->sp-0 );
  QtCollider::currentPen->cubicTo( cPoint1, cPoint2, endPoint );
  return errNone;
}

int QPen_QuadTo( struct VMGlobals *g, int )
{
  QPointF endPoint = Slot::toPoint( g->sp-1 );
  QPointF cPoint = Slot::toPoint( g->sp-0 );
  QtCollider::currentPen->quadTo( cPoint, endPoint );
  return errNone;
}

int QPen_AddRect (struct VMGlobals *g, int)
{
  QRectF r = Slot::toRect( g->sp );
  QtCollider::currentPen->addRect( r );
  return errNone;
}

int QPen_AddRoundedRect (struct VMGlobals *g, int)
{
  PyrSlot *args = g->sp - 2;
  QRectF r = Slot::toRect( args+0 );
  float radiusX, radiusY;
  if( slotFloatVal( args+1, &radiusX ) ) return errWrongType;
  if( slotFloatVal( args+2, &radiusY ) ) return errWrongType;
  QtCollider::currentPen->addRoundedRect( r, radiusX, radiusY );
  return errNone;
}

int QPen_AddEllipse (struct VMGlobals *g, int)
{
  QRectF r = Slot::toRect( g->sp );
  QtCollider::currentPen->addEllipse( r );
  return errNone;
}

int QPen_AddWedge( struct VMGlobals *g, int )
{
  PyrSlot *args = g->sp - 3;
  QPointF center = Slot::toPoint( args+0 );
  float radius, startAngle, sweepLength;
  if( slotFloatVal( args+1, &radius ) ) return errWrongType;
  if( slotFloatVal( args+2, &startAngle ) ) return errWrongType;
  if( slotFloatVal( args+3, &sweepLength ) ) return errWrongType;
  QtCollider::currentPen->addWedge( center, radius, startAngle, sweepLength );
  return errNone;
}

int QPen_AddAnnularWedge( struct VMGlobals *g, int )
{
  PyrSlot *args = g->sp - 4;
  QPointF center = Slot::toPoint( args+0 );
  float innerRadius, outerRadius, startAngle, sweepLength;
  if( slotFloatVal( args+1, &innerRadius ) ) return errWrongType;
  if( slotFloatVal( args+2, &outerRadius ) ) return errWrongType;
  if( slotFloatVal( args+3, &startAngle ) ) return errWrongType;
  if( slotFloatVal( args+4, &sweepLength ) ) return errWrongType;

  QtCollider::currentPen->addAnnularWedge( center,
                                           innerRadius, outerRadius,
                                           startAngle, sweepLength );

  return errNone;
}

int QPen_Draw (struct VMGlobals *g, int)
{
  int style = Slot::toInt( g->sp );
  QtCollider::currentPen->draw( style );

  return errNone;
}

int QPen_Clip (struct VMGlobals *, int)
{
  QtCollider::currentPen->clip();
  return errNone;
}

int QPen_StringAtPoint (struct VMGlobals *g, int)
{
  QString str = Slot::toString( g->sp-1 );
  if( str.isEmpty() ) return errNone;
  QPointF pt = Slot::toPoint( g->sp );

  QtCollider::currentPen->stringAtPoint( pt, str );

  return errNone;
}

int QPen_StringInRect (struct VMGlobals *g, int)
{
  QString str = Slot::toString( g->sp-1 );
  if( str.isEmpty() ) return errNone;
  QRectF r = Slot::toRect( g->sp );

  QtCollider::currentPen->stringInRect( r, str );

  return errNone;
}

void defineQPenPrimitives()
{
  qcDebugMsg( 1, "defining QPen primitives" );

  QtCollider::PrimitiveDefiner d;

  d.define( "_QPen_Save", QPen_Save, 1, 0);
  d.define( "_QPen_Restore", QPen_Restore, 1, 0 );
  d.define( "_QPen_Clear", QPen_Clear, 1, 0 );

  d.define( "_QPen_FillColor", QPen_FillColor, 2, 0 );
  d.define( "_QPen_StrokeColor", QPen_StrokeColor, 2, 0 );
  d.define( "_QPen_Width", QPen_Width, 2, 0 );
  d.define( "_QPen_Clip", QPen_Clip, 1, 0 );
  d.define( "_QPen_AntiAliasing", QPen_AntiAliasing, 2, 0 );
  d.define( "_QPen_SetFont", QPen_SetFont, 2, 0);

  d.define( "_QPen_Translate", QPen_Translate, 3, 0 );
  d.define( "_QPen_Scale", QPen_Scale, 3, 0 );
  d.define( "_QPen_Shear", QPen_Shear, 3, 0 );
  d.define( "_QPen_Rotate", QPen_Rotate, 4, 0 );
  d.define( "_QPen_Transform", QPen_Transform, 2, 0 );

  d.define( "_QPen_MoveTo", QPen_MoveTo, 2, 0 );
  d.define( "_QPen_LineTo", QPen_LineTo, 2, 0 );
  d.define( "_QPen_CubicTo", QPen_CubicTo, 4, 0 );
  d.define( "_QPen_QuadTo", QPen_QuadTo, 3, 0 );
  d.define( "_QPen_AddRect", QPen_AddRect, 2, 0 );
  d.define( "_QPen_AddRoundedRect", QPen_AddRoundedRect, 4, 0 );
  d.define( "_QPen_AddEllipse", QPen_AddEllipse, 2, 0 );
  d.define( "_QPen_AddWedge", QPen_AddWedge, 5, 0 );
  d.define( "_QPen_AddAnnularWedge", QPen_AddAnnularWedge, 6, 0 );
  d.define( "_QPen_Draw", QPen_Draw, 2, 0 );
  d.define( "_QPen_StringAtPoint", QPen_StringAtPoint, 3, 0 );
  d.define( "_QPen_StringInRect", QPen_StringInRect, 3, 0 );
}
