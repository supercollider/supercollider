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

#include <PyrObject.h>
#include <PyrKernel.h>
#include <GC.h>
#include <VMGlobals.h>

#include "Slot.h"
#include "QObjectProxy.h"
#include "Common.h"

#include <QPalette>
#include <QWidget>

using namespace QtCollider;

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

int Slot::setPoint( PyrSlot *slot, const QPointF &pt )
{
  if( !isKindOfSlot( slot, s_point->u.classobj ) ) {
    return errWrongType;
  }

  PyrSlot *slots = slotRawObject( slot )->slots;
  SetFloat( slots+0, pt.x() );
  SetFloat( slots+1, pt.y() );

  return errNone;
}

void Slot::setString( PyrSlot *slot, const QString& arg )
{
  const char *pszArg = arg.toStdString().c_str();
  PyrString *str = newPyrString( gMainVMGlobals->gc, pszArg, 0, true );
  SetObject( slot, str );
}

int Slot::setColor( PyrSlot *slot, const QColor &c )
{
  if( !isKindOfSlot( slot, s_color->u.classobj ) ) return errWrongType;

  PyrSlot *slots = slotRawObject( slot )->slots;

  SetFloat( slots+0, c.red() / 255.0 );
  SetFloat( slots+1, c.green() / 255.0 );
  SetFloat( slots+2, c.blue() / 255.0 );
  SetFloat( slots+3, c.alpha() / 255.0 );

  return errNone;
}

int Slot::setPalette( PyrSlot *slot, const QPalette &plt )
{
  if( !isKindOfSlot( slot, getsym("QPalette")->u.classobj ) ) return errWrongType;

  PyrSlot *s = slotRawObject( slot )->slots;

  for( int i=0; i<8; ++i, ++s ) {
    if( setColor( s, plt.color( paletteColorRoles[i] ) ) ) return errFailed;
  }

  return errNone;
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
      qcDebugMsg(1, "WARNING: Could not set one slot of array" );
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
    case QMetaType::QPoint:
    case QMetaType::QPointF:
        return Slot::setPoint( slot, val.toPointF() );
    case QMetaType::QRect:
    case QMetaType::QRectF:
        return Slot::setRect( slot, val.toRectF() );
    case QMetaType::QString:
        Slot::setString( slot, val.toString() );
        return errNone;
    case QMetaType::QColor:
         return Slot::setColor( slot, val.value<QColor>() );
    case QMetaType::QPalette:
        return Slot::setPalette( slot, val.value<QPalette>() );
    case QMetaType::Float:
    case QMetaType::Double:
        SetFloat( slot, val.value<float>() );
        return errNone;
    case QMetaType::Int:
        SetInt( slot, val.toInt() );
        return errNone;
    case QMetaType::Void:
        SetNil( slot );
        return errNone;
    default:
        if( type == qMetaTypeId<VariantList>() ) {
          Slot::setVariantList( slot, val.value<VariantList>() );
        }
        else {
          qcErrorMsg( "the QVariant could not be interpreted!" );
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

QSizeF Slot::toSize( PyrSlot *slot )
{
  if( !isKindOfSlot( slot, symSize->u.classobj ) ) {
    return QSizeF();
  }

  PyrSlot *slots = slotRawObject( slot )->slots;
  float w = 0.f, h = 0.f;
  slotFloatVal( slots+0, &w );
  slotFloatVal( slots+1, &h );

  return QSizeF( w, h );
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

  PyrClass *color_class = s_color->u.classobj;

  for( int i=0; i<8; ++i, ++slots ) {
    QColor c = Slot::toColor(slots);
    if( !c.isValid() ) continue;
    palette.setColor( paletteColorRoles[i], c );
  }

  return palette;
}

VariantList Slot::toVariantList( PyrSlot *slot )
{
  if( isKindOfSlot( slot, class_array ) ) {
    PyrObject *obj = slotRawObject( slot );
    PyrSlot *slots = obj->slots;
    int size = obj->size;
    VariantList list;
    for( int i = 0; i < size; ++i, ++slots )
      list.data << Slot::toVariant( slots );
    return list;
  }
  else if( isKindOfSlot( slot, class_symbolarray ) ) {
    PyrSymbolArray *symarray = slotRawSymbolArray( slot );
    PyrSymbol **symbols = symarray->symbols;
    int size = symarray->size;
    VariantList list;
    for( int i = 0; i < size; ++i, ++symbols )
      list.data << QVariant( QString( (*symbols)->name) );
    return list;
  }

  return VariantList();
}

QObjectProxy* Slot::toObjectProxy( PyrSlot *slot )
{
  if( !isKindOfSlot( slot, getsym("QObject")->u.classobj ) ) return 0;
  QObjectProxy *proxy = 0;
  PyrSlot *proxySlot = slotRawObject( slot )->slots;
  if( IsPtr( proxySlot ) ) proxy = (QObjectProxy*) slotRawPtr( proxySlot );
  return proxy;
}

QVariant Slot::toVariant( PyrSlot *slot )
{
  QObjectProxy *proxy;
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
      else if( isKindOfSlot( slot, symSize->u.classobj ) ) {
        return QVariant( toSize(slot) );
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
        proxy = toObjectProxy(slot);
        return QVariant::fromValue<QObjectProxy*>( proxy );
      }
      else if( isKindOfSlot( slot, class_array ) || isKindOfSlot( slot, class_symbolarray ) ) {
        return QVariant::fromValue<VariantList>( toVariantList(slot) );
      }
      else {
        qcErrorMsg("Could not interpret slot!");
        return QVariant();
      }
    }
    default:
      return QVariant( toFloat( slot ) );
  }
}

using namespace Slot;

void QtCollider::Variant::setData( PyrSlot *slot )
{
  QObjectProxy *proxy;
  switch (GetTag(slot)) {
    case tagChar :
    case tagNil :
      _type = QMetaType::Void;
      _ptr = 0;
      break;
    case tagInt :
      _type = QMetaType::Int;
      _ptr = new int( toInt(slot) );
      break;
    case tagSym :
      _type = QMetaType::QString;
      _ptr = new QString( toString(slot) );
      break;
    case tagFalse :
      _type = QMetaType::Bool;
      _ptr = new bool( false );
      break;
    case tagTrue :
      _type = QMetaType::Bool;
      _ptr = new bool( true );
      break;
    case tagObj :
    {
      if( isKindOfSlot( slot, class_string ) ) {
        _type = QMetaType::QString;
        _ptr = new QString( toString(slot) );
      }
      else if( isKindOfSlot( slot, s_point->u.classobj ) ) {
        _type = QMetaType::QPointF;
        _ptr = new QPointF( toPoint(slot) );
      }
      else if( isKindOfSlot( slot, s_rect->u.classobj ) ) {
        _type = QMetaType::QRectF;
        _ptr = new QRectF( toRect(slot) );
      }
      else if( isKindOfSlot( slot, symSize->u.classobj ) ) {
        _type = QMetaType::QSizeF;
        _ptr = new QSizeF( toSize(slot) );
      }
      else if( isKindOfSlot( slot, s_color->u.classobj ) ) {
        _type = QMetaType::QColor;
        _ptr = new QColor( toColor(slot) );
      }
      else if( isKindOfSlot( slot, class_array ) || isKindOfSlot( slot, class_symbolarray ) ) {
        _type = qMetaTypeId<VariantList>();
        _ptr = new VariantList( toVariantList(slot) );
      }
      else if( isKindOfSlot( slot, getsym("QObject")->u.classobj ) ) {
        proxy = toObjectProxy(slot);
        if( !proxy ) {
          _type = QMetaType::Void;
          _ptr = 0;
        }
        else {
          _type = qMetaTypeId<QObjectProxy*>();
          _ptr = new QObjectProxy*( proxy );
        }
      }
      else {
        qcErrorMsg("Could not interpret slot!");
        _type = QMetaType::Void;
        _ptr = 0;
      }
      break;
    }
    default :
      _type = QMetaType::Float;
      _ptr = new float( toFloat(slot) );
  }
}
