/************************************************************************
*
* Copyright 2010 Jakob Leben (jakob.leben@gmail.com)
*
* Copyright 2010 Ivan Leben (ivan.leben@gmail.com) (QPen_ArcTo)
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

#include "primitives.h"
#include "../painting.h"
#include "../Slot.h"

#include <QPainter>
#include <QVector2D>
#include <QVector3D>
#include <cmath>

namespace QtCollider
{

template <int FN( PyrSlot*, PyrSlot*, VMGlobals* )>
class QPenPrimitive
{
public:
  QPenPrimitive ( const char *name, int argc ) {
    LangPrimitiveData d;
    d.mediator = &mediate;
    d.name = strdup(name);
    d.argc = argc;
    langPrimitives().append( d );
  }
private:
  static int mediate( VMGlobals *g, int i ) {
    if( !globalPainter() ) {
      qcErrorMsg( QString("Usage of QPen is not allowed at this point!") );
      return errFailed;
    }
    PyrSlot *stack = g->sp - i + 1;
    int ret = (*FN)( stack, i > 1 ? stack+1 : 0, g );
    return ret;
  }
};

}

#define QC_QPEN_PRIMITIVE( name, argc, receiver, args, global ) \
  int name ( receiver, args, global ); \
  static QtCollider::QPenPrimitive<&name> p_##name( "_" #name, argc ); \
  int name ( receiver, args, global )

static bool announced = false;
static QPainter *painter = 0;
static QPainterPath path;

namespace QtCollider {
  void announcePainting() { announced = true; }
  bool paintingAnnounced() { return announced; }

  bool beginPainting( QPainter *p )
  {
    if( painter ) {
      qcErrorMsg( QString("Painting already in progress!") );
      return false;
    }

    painter = p;

    painter->setRenderHint( QPainter::Antialiasing, true );
    QColor black( 0,0,0 );
    QPen pen( black );
    pen.setCapStyle( Qt::FlatCap );
    painter->setPen( pen );
    painter->setBrush( black );

    path = QPainterPath();

    return true;
  }

  void endPainting()
  {
    painter = 0;
    announced = false;
  }

  QPainter *globalPainter() { return painter; }
}

typedef QVector2D vec2;
typedef QVector3D vec3;
static const double PI = 3.14159265358979323846264338327950288419717;

inline static qreal globalAngle( const vec2 &v )
{
  //assuming v is normalized
  qreal cosa = v.x();
  qreal sina = -v.y();
  return sina >= 0.0 ? acosf(cosa) : 2.0*PI - acosf(cosa);
}

inline static qreal vectorAngle( const vec2 &v1, const vec2 &v2 )
{
  //assuming v1,v2 are normalized
  return acosf( vec2::dotProduct( v1, v2 ) );
}

inline static qreal signedAngle( const vec2 &v1, const vec2 &v2 )
{
  //assuming v1,v2 are normalized
  qreal a = vectorAngle( v1, v2 );
  vec3 c = vec3::crossProduct( vec3(v1), vec3(v2) );
  return c.z() > 0.0 ? a : -a;
}

inline static qreal radToDeg( qreal rad )
{
  return rad * 180.0 / PI;
}

using namespace QtCollider;

QC_QPEN_PRIMITIVE( QPen_Save, 0, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  painter->save();
  return errNone;
}

QC_QPEN_PRIMITIVE( QPen_Restore, 0, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  painter->restore();
  return errNone;
}

QC_QPEN_PRIMITIVE( QPen_Clear, 0, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  path = QPainterPath();
  return errNone;
}

QC_QPEN_PRIMITIVE( QPen_FillColor, 1, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  QColor color = Slot::toColor( a );
  painter->setBrush( color );
  return errNone;
}

QC_QPEN_PRIMITIVE( QPen_StrokeColor, 1, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  QColor color = Slot::toColor( a );
  QPen pen = painter->pen();
  pen.setColor( color );
  painter->setPen( pen );
  return errNone;
}

QC_QPEN_PRIMITIVE( QPen_Width, 1, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  float width;
  if( slotFloatVal( a, &width ) ) return errWrongType;
  QPen pen = painter->pen();
  pen.setWidthF( width );
  painter->setPen( pen );
  return errNone;
}

QC_QPEN_PRIMITIVE( QPen_SetJoinStyle, 1, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  int style = Slot::toInt(a);
  QPen pen = painter->pen();

  switch( style ) {
    case 0:
      pen.setJoinStyle( Qt::MiterJoin ); break;
    case 1:
      pen.setJoinStyle( Qt::RoundJoin ); break;
    case 2:
      pen.setJoinStyle( Qt::BevelJoin ); break;
    default:
      return errFailed;
  }

  painter->setPen( pen );

  return errNone;
}

QC_QPEN_PRIMITIVE( QPen_SetCapStyle, 1, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  int style = Slot::toInt(a);
  QPen pen = painter->pen();

  switch( style ) {
    case 0:
      pen.setCapStyle( Qt::FlatCap ); break;
    case 1:
      pen.setCapStyle( Qt::RoundCap ); break;
    case 2:
      pen.setCapStyle( Qt::SquareCap ); break;
    default:
      return errFailed;
  }

  painter->setPen( pen );

  return errNone;
}

QC_QPEN_PRIMITIVE( QPen_SetDashPattern, 1, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  if( !IsObj( a ) ) return errWrongType;
  PyrObject *obj = slotRawObject( a );
  if( obj->classptr != class_FloatArray ) return errWrongType;
  PyrFloatArray *farray = reinterpret_cast<PyrFloatArray*>(obj);

  int s = farray->size;
  float *f = farray->f;
  QVector<qreal> pattern;

  int i = 1;
  while( i<s ) {
    pattern << *f << *(f+1);
    f += 2;
    i += 2;
  }

  if( pattern.size() ) {
    QPen pen = painter->pen();
    pen.setDashPattern( pattern );
    painter->setPen( pen );
  }

  return errNone;
}

QC_QPEN_PRIMITIVE( QPen_SetOpacity, 1, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  float opacity = Slot::toFloat(a);
  painter->setOpacity( opacity );
  return errNone;
}

QC_QPEN_PRIMITIVE( QPen_Clip, 0, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  painter->setClipPath( path );
  path = QPainterPath();
  return errNone;
}

QC_QPEN_PRIMITIVE( QPen_AntiAliasing, 1, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  bool b = IsTrue( a );
  if( !b && !IsFalse( a ) ) return errWrongType;
  painter->setRenderHint( QPainter::Antialiasing, b );
  return errNone;
}

QC_QPEN_PRIMITIVE( QPen_SetFont, 1, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  painter->setFont( Slot::toFont( a ) );
  return errNone;
}

QC_QPEN_PRIMITIVE( QPen_Translate, 2, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  float x, y;
  if( slotFloatVal( a, &x ) ) return errWrongType;
  if( slotFloatVal( a+1, &y ) ) return errWrongType;
  painter->translate( x, y );
  return errNone;
}

QC_QPEN_PRIMITIVE( QPen_Scale, 2, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  float x, y;
  if( slotFloatVal( a, &x ) ) return errWrongType;
  if( slotFloatVal( a+1, &y ) ) return errWrongType;
  painter->scale( x, y );
  return errNone;
}

QC_QPEN_PRIMITIVE( QPen_Shear, 2, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  float x, y;
  if( slotFloatVal( a, &x ) ) return errWrongType;
  if( slotFloatVal( a+1, &y ) ) return errWrongType;
  painter->shear( x, y );
  return errNone;
}

QC_QPEN_PRIMITIVE( QPen_Rotate, 3, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  float angle, x, y;
  if( slotFloatVal( a, &angle ) ) return errWrongType;
  if( slotFloatVal( a+1, &x ) ) return errWrongType;
  if( slotFloatVal( a+2, &y ) ) return errWrongType;

  painter->translate( x, y );
  painter->rotate( angle );
  painter->translate( -x, -y );

  return errNone;
}

QC_QPEN_PRIMITIVE( QPen_Transform, 1, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  VariantList list = Slot::toVariantList( a );
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
  painter->setWorldTransform( transform );
  return errNone;
}

QC_QPEN_PRIMITIVE( QPen_MoveTo, 1, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  QPointF point = Slot::toPoint( a );
  path.moveTo( point );
  return errNone;
}

QC_QPEN_PRIMITIVE( QPen_LineTo, 1, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  QPointF point = Slot::toPoint( a );
  path.lineTo( point );
  return errNone;
}

QC_QPEN_PRIMITIVE( QPen_CubicTo, 3, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  QPointF endPoint = Slot::toPoint( a );
  QPointF cPoint1 = Slot::toPoint( a+1 );
  QPointF cPoint2 = Slot::toPoint( a+2 );
  path.cubicTo( cPoint1, cPoint2, endPoint );
  return errNone;
}

QC_QPEN_PRIMITIVE( QPen_QuadTo, 2, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  QPointF endPoint = Slot::toPoint( a );
  QPointF cPoint = Slot::toPoint( a+1 );
  path.quadTo( cPoint, endPoint );
  return errNone;
}

QC_QPEN_PRIMITIVE( QPen_ArcTo, 3, PyrSlot *r, PyrSlot *arg, VMGlobals *g )
{
  QPointF pt1 = Slot::toPoint( arg );
  QPointF pt2 = Slot::toPoint( arg+1 );
  float radius;
  if( slotFloatVal( arg+2, &radius ) ) return errWrongType;
  radius = qMax( 0.f, radius );

  vec2 a( path.currentPosition() );
  vec2 b( pt1 );
  vec2 c( pt2 );

  vec2 va = (a - b).normalized();
  vec2 vc = (c - b).normalized();
  vec2 m = (va + vc).normalized();

  qreal lineAngle = vectorAngle( va, vc );
  qreal dm = radius / sinf( lineAngle * 0.5f );
  qreal dv = radius / tanf( lineAngle * 0.5f );

  vec2 center = b + dm * m;
  vec2 start = b + dv * va;
  vec2 end = b + dv * vc;

  vec2 vs = (start - center).normalized();
  vec2 ve = (end - center).normalized();

  qreal arcAngle = signedAngle( ve, vs );
  qreal arcStart = globalAngle( vs );

  path.lineTo( start.x(), start.y() );

  path.arcTo( center.x() - radius, center.y() - radius, 2*radius, 2*radius,
              radToDeg( arcStart ), radToDeg( arcAngle ) );

  // The current SC API does not want to pull the line to the last point.
  // Personally, I think it would be better:

  // path.lineTo( pt2 );

  return errNone;
}

QC_QPEN_PRIMITIVE( QPen_AddRect, 1, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  QRectF rect = Slot::toRect( a );
  path.addRect( rect );
  return errNone;
}

QC_QPEN_PRIMITIVE( QPen_AddRoundedRect, 3, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  float radiusX, radiusY;
  QRectF rect;

  rect = Slot::toRect( a );
  if( slotFloatVal( a+1, &radiusX ) ) return errWrongType;
  if( slotFloatVal( a+2, &radiusY ) ) return errWrongType;

  path.addRoundedRect( rect, radiusX, radiusY );
  return errNone;
}

QC_QPEN_PRIMITIVE( QPen_AddEllipse, 1, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  QRectF rect = Slot::toRect( a );
  path.addEllipse( rect );
  return errNone;
}

QC_QPEN_PRIMITIVE( QPen_AddArc, 4, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  QPointF center = Slot::toPoint( a );
  float radius, startAngle, arcAngle;
  if( slotFloatVal( a+1, &radius ) ) return errWrongType;
  if( slotFloatVal( a+2, &startAngle ) ) return errWrongType;
  if( slotFloatVal( a+3, &arcAngle ) ) return errWrongType;

  // have to swap angle direction for sinf()
  QPointF start( radius * cosf( startAngle ), -radius * sinf( startAngle ) );
  start += center;

  QRectF rect;
  rect.setSize( QSizeF( 2*radius, 2*radius ) );
  rect.moveCenter( center );

  path.moveTo( start );
  path.arcTo( rect, radToDeg( startAngle ), radToDeg( arcAngle ) );

  return errNone;
}

QC_QPEN_PRIMITIVE( QPen_AddWedge, 4, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  QPointF center = Slot::toPoint( a );
  float radius, startAngle, sweepLength;
  if( slotFloatVal( a+1, &radius ) ) return errWrongType;
  if( slotFloatVal( a+2, &startAngle ) ) return errWrongType;
  if( slotFloatVal( a+3, &sweepLength ) ) return errWrongType;
  path.moveTo( center );
  QRectF rect;
  rect.setSize( QSizeF( 2*radius, 2*radius ) );
  rect.moveCenter( center );
  path.arcTo( rect, startAngle, sweepLength );
  path.closeSubpath();
  return errNone;
}

QC_QPEN_PRIMITIVE( QPen_AddAnnularWedge, 5, PyrSlot *, PyrSlot *a, VMGlobals *g )
{
  QPointF c = Slot::toPoint( a );
  float innerRadius, outerRadius, startAngle, arcAngle;
  if( slotFloatVal( a+1, &innerRadius ) ) return errWrongType;
  if( slotFloatVal( a+2, &outerRadius ) ) return errWrongType;
  if( slotFloatVal( a+3, &startAngle ) ) return errWrongType;
  if( slotFloatVal( a+4, &arcAngle ) ) return errWrongType;

  float saDeg = radToDeg( startAngle );
  float aaDeg = radToDeg( arcAngle );

  QPointF start( outerRadius * cosf( startAngle ), -outerRadius * sinf( startAngle ) );
  start += c;
  path.moveTo( start );

  QPointF pt( innerRadius, innerRadius );
  path.arcTo( QRectF( c - pt, c + pt ), saDeg, aaDeg );

  pt = QPointF( outerRadius, outerRadius );
  path.arcTo( QRectF( c - pt, c + pt ), saDeg + aaDeg, -aaDeg );

  return errNone;
}

QC_QPEN_PRIMITIVE( QPen_Draw, 1, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  if( path.isEmpty() ) return errNone;

  int style = Slot::toInt( a );
  QPen pen = painter->pen();
  QBrush brush = painter->brush();

  switch ( style ) {
    case 0:
    case 1:
      painter->setPen( Qt::NoPen ); break;
    case 2:
      painter->setBrush( Qt::NoBrush ); break;
    case 3:
    case 4:
    default: ;
  }

  painter->drawPath( path );

  path = QPainterPath();

  painter->setPen( pen );
  painter->setBrush( brush );

  return errNone;
}

QC_QPEN_PRIMITIVE( QPen_FillAxialGradient, 4, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  QPointF pt1 = Slot::toPoint(a+0);
  QPointF pt2 = Slot::toPoint(a+1);
  QColor c1 = Slot::toColor(a+2);
  QColor c2 = Slot::toColor(a+3);

  QLinearGradient grad( pt1, pt2 );
  grad.setColorAt( 0, c1 );
  grad.setColorAt( 1, c2 );

  QPen pen = painter->pen();
  QBrush brush = painter->brush();

  painter->setPen( Qt::NoPen );
  painter->setBrush( grad );

  painter->drawPath( path );

  painter->setPen( pen );
  painter->setBrush( brush );

  path = QPainterPath();

  return errNone;
}

QC_QPEN_PRIMITIVE( QPen_FillRadialGradient, 6, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  QPointF pt1 = Slot::toPoint(a+0);
  QPointF pt2 = Slot::toPoint(a+1);
  float r1 = Slot::toFloat(a+2);
  float r2 = Slot::toFloat(a+3);
  QColor c1 = Slot::toColor(a+4);
  QColor c2 = Slot::toColor(a+5);

  QRadialGradient grad( pt2, r2, pt1 );
  grad.setColorAt( (r2 > 0 ? r1 / r2 : 0), c1 );
  grad.setColorAt( 1, c2 );

  QPen pen = painter->pen();
  QBrush brush = painter->brush();

  painter->setPen( Qt::NoPen );
  painter->setBrush( grad );

  painter->drawPath( path );

  painter->setPen( pen );
  painter->setBrush( brush );

  path = QPainterPath();

  return errNone;
}

QC_QPEN_PRIMITIVE( QPen_StringAtPoint, 4, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  QString str = Slot::toString( a );
  if( str.isEmpty() ) return errNone;
  QPointF pt = Slot::toPoint( a+1 );

  painter->save();
  if( NotNil( a+2 ) ) painter->setFont( Slot::toFont( a+2 ) );
  QPen pen( painter->pen() );
  pen.setColor( NotNil( a+3 ) ? Slot::toColor( a+3 ) : painter->brush().color() );
  painter->setPen( pen );

  QFont f( painter->font() );
  QFontMetrics fm( f );
  QRect rect = fm.boundingRect( str );
  painter->drawText( pt - rect.topLeft(), str );

  painter->restore();

  return errNone;
}

QC_QPEN_PRIMITIVE( QPen_StringInRect, 5, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  QString str = Slot::toString( a );
  if( str.isEmpty() ) return errNone;
  QRectF rect = Slot::toRect( a+1 );

  painter->save();
  if( NotNil( a+2 ) ) painter->setFont( Slot::toFont( a+2 ) );
  QPen pen( painter->pen() );
  pen.setColor( NotNil( a+3 ) ? Slot::toColor( a+3 ) : painter->brush().color() );
  painter->setPen( pen );

  Qt::Alignment align;
  if( NotNil(a+4) ) align = static_cast<Qt::Alignment>( Slot::toInt( a+4 ) );
  else align = Qt::AlignTop | Qt::AlignLeft;

  painter->drawText( rect, align, str );

  painter->restore();

  return errNone;
}
