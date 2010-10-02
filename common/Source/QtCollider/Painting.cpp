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

#include <QPainter>
#include <QPaintDevice>
#include <QWidget>

#include <PyrPrimitive.h>
#include <VMGlobals.h>

#include "Painting.h"
#include "QObjectProxy.h"
#include "Common.h"

static QPainter painter;
static QPainterPath path;

namespace QtCollider {
  void beginPainting( QPaintDevice *device )
  {
    if( !painter.begin( device ) )
      qscErrorMsg( "Cannot begin painting\n");
    path = QPainterPath();
    painter.setRenderHint( QPainter::Antialiasing, true );
    QColor black( 0,0,0 );
    painter.setPen( black );
    painter.setBrush( black );
  }

  void endPainting()
  {
    painter.end();
  }

  QPainter *globalPainter()
  {
    return &painter;
  }
}

int QPen_Begin (struct VMGlobals *g, int)
{
  //if( painter.isActive() ) return errFailed;
  //if( !isKindOfSlot( g->sp, s_QObject->u.classobj ) ) return errWrongType;
  //if( IS_OBJECT_NIL( g->sp ) ) return errFailed;

  QObjectProxy *proxy =
    static_cast<QObjectProxy*>( slotRawPtr( slotRawObject( g->sp )->slots ) );
  QWidget *widget = qobject_cast<QWidget*>( proxy->object() );
  if( !widget ) return errWrongType;

  QtCollider::beginPainting( widget );

  return errNone;
}

int QPen_End (struct VMGlobals *, int)
{
  QtCollider::endPainting();

  return errNone;
}

int QPen_Save (struct VMGlobals *, int)
{
  painter.save();
  return errNone;
}

int QPen_Restore (struct VMGlobals *, int)
{
  painter.restore();
  return errNone;
}

int QPen_Clear (struct VMGlobals *, int)
{
  path = QPainterPath();
  return errNone;
}

int QPen_FillColor (struct VMGlobals *g, int)
{
  QColor color = Slot::toColor( g->sp );
  painter.setBrush( color );
  return errNone;
}

int QPen_StrokeColor (struct VMGlobals *g, int)
{
  QColor color = Slot::toColor( g->sp );
  painter.setPen( color );
  return errNone;
}

int QPen_Width ( struct VMGlobals *g, int )
{
  float width;
  if( slotFloatVal( g->sp, &width ) ) return errWrongType;
  QPen pen = painter.pen();
  pen.setWidthF( width );
  painter.setPen( pen );
  return errNone;
}

int QPen_Clip (struct VMGlobals *, int)
{
  painter.setClipPath( path );
  path = QPainterPath();
  return errNone;
}

int QPen_AntiAliasing (struct VMGlobals *g, int)
{
  bool b = IsTrue( g->sp );
  if( !b && !IsFalse( g->sp ) ) return errWrongType;
  painter.setRenderHint( QPainter::Antialiasing, b );
  return errNone;
}

int QPen_SetFont (struct VMGlobals *g, int)
{
  painter.setFont( Slot::toFont( g->sp ) );
  return errNone;
}

int QPen_Translate ( struct VMGlobals *g, int )
{
  float x, y;
  if( slotFloatVal( g->sp-1, &x ) ) return errWrongType;
  if( slotFloatVal( g->sp, &y ) ) return errWrongType;
  painter.translate( x, y );
  return errNone;
}

int QPen_Scale ( struct VMGlobals *g, int )
{
  float x, y;
  if( slotFloatVal( g->sp-1, &x ) ) return errWrongType;
  if( slotFloatVal( g->sp, &y ) ) return errWrongType;
  painter.scale( x, y );
  return errNone;
}

int QPen_Shear ( struct VMGlobals *g, int )
{
  float x, y;
  if( slotFloatVal( g->sp-1, &x ) ) return errWrongType;
  if( slotFloatVal( g->sp, &y ) ) return errWrongType;
  painter.shear( x, y );
  return errNone;
}

int QPen_Rotate ( struct VMGlobals *g, int )
{
  float angle, x, y;
  if( slotFloatVal( g->sp-2, &angle ) ) return errWrongType;
  if( slotFloatVal( g->sp-1, &x ) ) return errWrongType;
  if( slotFloatVal( g->sp, &y ) ) return errWrongType;
  painter.rotate( angle );
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
  QTransform transform( f[0], f[1], f[2], f[3], f[4], f[5] );
  painter.setWorldTransform( transform );
  return errNone;
}

int QPen_MoveTo( struct VMGlobals *g, int )
{
  QPointF point = Slot::toPoint( g->sp );
  path.moveTo( point );
  return errNone;
}

int QPen_LineTo( struct VMGlobals *g, int )
{
  QPointF point = Slot::toPoint( g->sp );
  path.lineTo( point );
  return errNone;
}

int QPen_CubicTo( struct VMGlobals *g, int )
{
  QPointF endPoint = Slot::toPoint( g->sp-2 );
  QPointF cPoint1 = Slot::toPoint( g->sp-1 );
  QPointF cPoint2 = Slot::toPoint( g->sp-0 );
  path.cubicTo( cPoint1, cPoint2, endPoint );
  return errNone;
}

int QPen_QuadTo( struct VMGlobals *g, int )
{
  QPointF endPoint = Slot::toPoint( g->sp-1 );
  QPointF cPoint = Slot::toPoint( g->sp-0 );
  path.quadTo( cPoint, endPoint );
  return errNone;
}

int QPen_AddRect (struct VMGlobals *g, int)
{
  QRectF r = Slot::toRect( g->sp );
  path.addRect( r );
  return errNone;
}

int QPen_AddRoundedRect (struct VMGlobals *g, int)
{
	PyrSlot *args = g->sp - 2;
	QRectF r = Slot::toRect( args+0 );
	float radiusX, radiusY;
	if( slotFloatVal( args+1, &radiusX ) ) return errWrongType;
	if( slotFloatVal( args+2, &radiusY ) ) return errWrongType;
	path.addRoundedRect( r, radiusX, radiusY );
	return errNone;
}

int QPen_AddEllipse (struct VMGlobals *g, int)
{
  QRectF r = Slot::toRect( g->sp );
  path.addEllipse( r );
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
  path.moveTo( center );
  QRectF r;
  r.setSize( QSizeF( 2*radius, 2*radius ) );
  r.moveCenter( center );
  path.arcTo( r, startAngle, sweepLength );
  path.closeSubpath();
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

  QPainterPath annularWedge;
  annularWedge.moveTo( center );
  QRectF r;
  r.setSize( QSizeF( 2*outerRadius, 2*outerRadius ) );
  r.moveCenter( center );
  annularWedge.arcTo( r, startAngle, sweepLength );
  annularWedge.closeSubpath();

  QPainterPath circle;
  QRectF r2( 0,0,2*innerRadius, 2*innerRadius );
  r2.moveCenter( center );
  circle.addEllipse( r2 );

  //FIXME this is VERY slow!!
  path.addPath( annularWedge - circle );

  return errNone;
}

int QPen_Draw (struct VMGlobals *g, int)
{
  if( path.isEmpty() ) return errNone;

  int style = Slot::toInt( g->sp );
  QPen pen = painter.pen();
  QBrush brush = painter.brush();

  switch ( style ) {
    case 0:
    case 1:
      painter.setPen( Qt::NoPen ); break;
    case 2:
      painter.setBrush( Qt::NoBrush ); break;
    case 3:
    case 4:
    default: ;
  }

  painter.drawPath( path );

  path = QPainterPath();

  painter.setPen( pen );
  painter.setBrush( brush );

  return errNone;
}

int QPen_StringAtPoint (struct VMGlobals *g, int)
{
  QString str = Slot::toString( g->sp-1 );
  if( str.isEmpty() ) return errNone;
  QPointF pt = Slot::toPoint( g->sp );

  QFont f( painter.font() );
  QFontMetrics fm( f );
  QRect r = fm.boundingRect( str );
  painter.drawText( pt - r.topLeft(), str );

  return errNone;
}

int QPen_StringInRect (struct VMGlobals *g, int)
{
  QString str = Slot::toString( g->sp-1 );
  if( str.isEmpty() ) return errNone;
  QRectF r = Slot::toRect( g->sp );

  painter.drawText( r, Qt::AlignTop | Qt::AlignLeft, str );

  return errNone;
}

void initQtPenPrimitives()
{
  qscDebugMsg( "Initializing painting primitives\n" );
  int base = nextPrimitiveIndex();
  int index = 0;

  definePrimitive(base, index++, "_QPen_Begin",
                  QPen_Begin, 2, 0);
  definePrimitive(base, index++, "_QPen_End",
                  QPen_End, 1, 0);
  definePrimitive(base, index++, "_QPen_Save",
                  QPen_Save, 1, 0);
  definePrimitive(base, index++, "_QPen_Restore",
                  QPen_Restore, 1, 0);
  definePrimitive(base, index++, "_QPen_Clear",
                  QPen_Clear, 1, 0);

  definePrimitive(base, index++, "_QPen_FillColor",
                  QPen_FillColor, 2, 0);
  definePrimitive(base, index++, "_QPen_StrokeColor",
                  QPen_StrokeColor, 2, 0);
  definePrimitive(base, index++, "_QPen_Width",
                  QPen_Width, 2, 0);
  definePrimitive(base, index++, "_QPen_Clip",
                  QPen_Clip, 1, 0);
  definePrimitive(base, index++, "_QPen_AntiAliasing",
                  QPen_AntiAliasing, 2, 0);
  definePrimitive(base, index++, "_QPen_SetFont",
                  QPen_SetFont, 2, 0);

  definePrimitive(base, index++, "_QPen_Translate",
                  QPen_Translate, 3, 0);
  definePrimitive(base, index++, "_QPen_Scale",
                  QPen_Scale, 3, 0);
  definePrimitive(base, index++, "_QPen_Shear",
                  QPen_Shear, 3, 0);
  definePrimitive(base, index++, "_QPen_Rotate",
                  QPen_Rotate, 4, 0);
  definePrimitive(base, index++, "_QPen_Transform",
                  QPen_Transform, 2, 0);


  definePrimitive(base, index++, "_QPen_MoveTo",
                  QPen_MoveTo, 2, 0);
  definePrimitive(base, index++, "_QPen_LineTo",
                  QPen_LineTo, 2, 0);
  definePrimitive(base, index++, "_QPen_CubicTo",
                  QPen_CubicTo, 4, 0);
  definePrimitive(base, index++, "_QPen_QuadTo",
                  QPen_QuadTo, 3, 0);
  definePrimitive(base, index++, "_QPen_AddRect",
                  QPen_AddRect, 2, 0);
  definePrimitive(base, index++, "_QPen_AddEllipse",
                  QPen_AddEllipse, 2, 0);
  definePrimitive(base, index++, "_QPen_AddWedge",
                  QPen_AddWedge, 5, 0);
  definePrimitive(base, index++, "_QPen_AddAnnularWedge",
                  QPen_AddAnnularWedge, 6, 0);
  definePrimitive(base, index++, "_QPen_Draw",
                  QPen_Draw, 2, 0);
  definePrimitive(base, index++, "_QPen_StringAtPoint",
                  QPen_StringAtPoint, 3, 0);
  definePrimitive(base, index++, "_QPen_StringInRect",
                  QPen_StringInRect, 3, 0);
}
