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

#include <QPalette>

#include <PyrObject.h>
#include <PyrKernel.h>
#include <GC.h>
#include <VMGlobals.h>

#include "Slot.h"
#include "QObjectProxy.h"
#include "Common.h"

static QPalette::ColorRole paletteColorRoles[] = {
  QPalette::Window,
  QPalette::WindowText,
  QPalette::Button,
  QPalette::ButtonText,
  QPalette::Base,
  QPalette::Text,
  QPalette::Highlight,
  QPalette::HighlightedText,
};

int Slot::setRect( PyrSlot *slot, const QRectF &r )
{
  if( !isKindOfSlot( slot, s_rect->u.classobj ) ) {
    return errWrongType;
  }

  PyrSlot *slots = slotRawObject( slot )->slots;
  SetFloat( slots+0, r.x() );
  SetFloat( slots+1, r.y() );
  SetFloat( slots+2, r.width() );
  SetFloat( slots+3, r.height() );

  return errNone;
}

void Slot::setString( PyrSlot *slot, const QString& arg )
{
  const char *pszArg = arg.toStdString().c_str();
  PyrString *str = newPyrString( gMainVMGlobals->gc, pszArg, 0, true );
  SetObject( slot, str );
}

void Slot::setVariantList( PyrSlot *slot, const VariantList& varList )
{
  VMGlobals *g = gMainVMGlobals;

  int count = varList.data.count();

  PyrObject *array = newPyrArray( g->gc, count, 0, true );

  int i;
  PyrSlot *s = array->slots;
  for( i = 0; i < count; ++i, ++s ) {
    if( Slot::setVariant( s, varList.data[i] ) ) {
      qscDebugMsg( "WARNING: Could not set a slot of array\n" );
    }
    array->size++;
    g->gc->GCWrite( array, s );
  }

  SetObject( slot, array );
}

int Slot::setVariant( PyrSlot *slot, const QVariant &val )
{
  bool b_val;
  int type = val.userType();

  switch( type ) {
    case QMetaType::Bool:
        b_val = val.toBool();
        if( b_val ) SetTrue( slot );
        else SetFalse( slot );
        return errNone;
    case QMetaType::QRect:
        return Slot::setRect( slot, val.toRect() );
    case QMetaType::QString:
        Slot::setString( slot, val.toString() );
        return errNone;
    case QMetaType::Float:
    case QMetaType::Double:
        SetFloat( slot, val.value<float>() );
        return errNone;
    case QMetaType::Int:
        SetInt( slot, val.toInt() );
        return errNone;
    default:
        if( type == qMetaTypeId<VariantList>() ) {
          Slot::setVariantList( slot, val.value<VariantList>() );
        }
        else {
          return errFailed;
        }
        return errNone;
  }
}

bool Slot::toBool( PyrSlot *slot )
{
  return IsTrue( slot );
}

int Slot::toInt( PyrSlot *slot )
{
  int i;
  if( slotIntVal( slot, &i ) ) return 0;
  return i;
}

float Slot::toFloat( PyrSlot *slot )
{
  float f;
  if( slotFloatVal( slot, &f ) ) return 0.f;
  return f;
}

QString Slot::toString( PyrSlot *slot )
{
  if( IsSym(slot) ) {
    return QString( slotRawSymbol(slot)->name );
  }
  else if( isKindOfSlot( slot, class_string ) ) {
    int len = slotRawObject( slot )->size;
    return QString::fromAscii( slotRawString(slot)->s, len );
  }
  return QString();
}

QPointF Slot::toPoint( PyrSlot *slot )
{
  if( !isKindOfSlot( slot, s_point->u.classobj ) ) {
    return QPointF();
  }
  PyrSlot *slots = slotRawObject( slot )->slots;
  float x, y;
  int err;
  err = slotFloatVal( slots+0, &x ); if( err ) return QPointF();
  err = slotFloatVal( slots+1, &y ); if( err ) return QPointF();
  return QPointF( x, y );
}

QRectF Slot::toRect( PyrSlot *slot )
{
  if( !isKindOfSlot( slot, s_rect->u.classobj ) ) {
    return QRectF();
  }

  PyrSlot *slots = slotRawObject( slot )->slots;
  float bounds[4];
  for( int i=0; i<4; ++i )
  {
    int err = slotFloatVal(slots + i, &bounds[i]);
    if( err ) return QRectF();
  }

  return QRectF( bounds[0], bounds[1], bounds[2], bounds[3] );
}

QColor Slot::toColor( PyrSlot *slot )
{
  if( !isKindOfSlot( slot, s_color->u.classobj ) )
    return QColor();

  PyrSlot *slots = slotRawObject(slot)->slots;

  float r,g,b,a;
  r = g = b = a = 0.f;
  int err;
  err = slotFloatVal(slots+0, &r);
  if (err) return err;
  err = slotFloatVal(slots+1, &g);
  if (err) return err;
  err = slotFloatVal(slots+2, &b);
  if (err) return err;
  err = slotFloatVal(slots+3, &a);
  if (err) return err;
  return QColor( r*255, g*255, b*255, a*255 );
}

QFont Slot::toFont( PyrSlot *slot )
{
  if( !isKindOfSlot( slot, getsym("QFont")->u.classobj ) )
    return QFont();

  PyrSlot *slots = slotRawObject(slot)->slots;

  QString family = Slot::toString( slots+0 );
  //NOTE we allow empty family field;
  int size = IsInt( slots+1 ) ? Slot::toInt( slots+1 ) : -1;
  bool bold = IsTrue( slots+2 );
  bool italic = IsTrue( slots+3 );

  QFont f( family, size, bold ? QFont::Bold : QFont::Normal, italic );
  f.setPixelSize( size );

  return f;
}

QPalette Slot::toPalette( PyrSlot *slot )
{
  if( !isKindOfSlot( slot, getsym("QPalette")->u.classobj ) )
    return QPalette();

  PyrSlot *slots = slotRawObject( slot )->slots;
  QPalette palette;
  int i;

  PyrClass *color_class = s_color->u.classobj;

  for( i=0; i<8; ++i, ++slots ) {
    if( !isKindOfSlot( slots, color_class ) ) continue;
    palette.setColor( paletteColorRoles[i], Slot::toColor(slots) );
  }

  return palette;
}

VariantList Slot::toVariantList( PyrSlot *slot )
{
  if ( !isKindOfSlot( slot, class_array ) ) return VariantList();

  PyrSlot *slots = slotRawObject( slot )->slots;
  int size = slotRawObject( slot )->size;
  VariantList list;
  for( int i = 0; i < size; ++i, ++slots )
    list.data << Slot::toVariant( slots );
  return list;
}

QObjectProxy* Slot::toObjectProxy( PyrSlot *slot )
{
  if( !isKindOfSlot( slot, getsym("QObject")->u.classobj ) ) return 0;
  QObjectProxy *proxy = 0;
  PyrSlot *slots = slotRawObject( slot )->slots;
  if( IsPtr( slots ) ) proxy = (QObjectProxy*) slotRawPtr( slots );
  return proxy;
}

QVariant Slot::toVariant( PyrSlot *slot )
{
  switch (GetTag(slot)) {
    case tagChar :
    case tagNil :
      return QVariant();
    case tagInt :
      return QVariant( toInt(slot) );
    case tagSym :
      return QVariant( toString(slot) );
    case tagFalse :
      return QVariant( false );
    case tagTrue :
      return QVariant( true );
    case tagObj :
    {
      if( isKindOfSlot( slot, class_string ) ) {
        return QVariant( toString(slot) );
      }
      else if( isKindOfSlot( slot, s_point->u.classobj ) ) {
        return QVariant( toPoint( slot ) );
      }
      else if( isKindOfSlot( slot, s_rect->u.classobj ) ) {
        return QVariant( toRect(slot) );
      }
      else if( isKindOfSlot( slot, s_color->u.classobj ) ) {
        return QVariant::fromValue<QColor>( toColor(slot) );
      }
      else if( isKindOfSlot( slot, getsym("QFont")->u.classobj ) ) {
        return QVariant::fromValue<QFont>( toFont(slot) );
      }
      else if( isKindOfSlot( slot, getsym("QPalette")->u.classobj ) ) {
        return QVariant::fromValue<QPalette>( toPalette(slot) );
      }
      else if( isKindOfSlot( slot, getsym("QObject")->u.classobj ) ) {
        return QVariant::fromValue<QObjectProxy*>( toObjectProxy(slot) );
      }
      else if( isKindOfSlot( slot, class_array ) ) {
        return QVariant::fromValue<VariantList>( toVariantList(slot) );
      }
      else {
        qscErrorMsg("Could not interpret slot!\n");
        return QVariant();
      }
    }
    default:
      return QVariant( toFloat( slot ) );
  }
}

void Slot::setData( PyrSlot *slot )
{
  switch (GetTag(slot)) {
    case tagChar :
    case tagNil :
      type = QMetaType::Void;
      ptr = 0;
      break;
    case tagInt :
      type = QMetaType::Int;
      ptr = new int( toInt(slot) );
      break;
    case tagSym :
      type = QMetaType::QString;
      ptr = new QString( toString(slot) );
      break;
    case tagFalse :
      type = QMetaType::Bool;
      ptr = new bool( false );
      break;
    case tagTrue :
      type = QMetaType::Bool;
      ptr = new bool( true );
      break;
    case tagObj :
    {
      if( isKindOfSlot( slot, class_string ) ) {
        type = QMetaType::QString;
        ptr = new QString( toString(slot) );
      }
      else if( isKindOfSlot( slot, s_point->u.classobj ) ) {
        type = QMetaType::QPointF;
        ptr = new QPointF( toPoint(slot) );
      }
      else if( isKindOfSlot( slot, s_rect->u.classobj ) ) {
        type = QMetaType::QRectF;
        ptr = new QRectF( toRect(slot) );
      }
      else {
        qscErrorMsg("Could not interpret slot!\n");
        type = QMetaType::Void;
        ptr = 0;
      }
      break;
    }
    default :
      type = QMetaType::Float;
      ptr = new float( toFloat(slot) );
  }
}
