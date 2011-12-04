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

#include "Slot.h"
#include "QObjectProxy.h"
#include "Common.h"

#include <PyrObject.h>
#include <PyrKernel.h>
#include <GC.h>
#include <VMGlobals.h>

#include <QPalette>
#include <QWidget>

#include <qmath.h>

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

void Slot::setRect( PyrSlot *slot, const QRectF &r )
{
  PyrObject *obj = instantiateObject( gMainVMGlobals->gc, class_Rect, 0, true, true );
  SetObject( slot, obj );

  PyrSlot *slots = obj->slots;
  SetFloat( slots+0, r.x() );
  SetFloat( slots+1, r.y() );
  SetFloat( slots+2, r.width() );
  SetFloat( slots+3, r.height() );
}

void Slot::setPoint( PyrSlot *slot, const QPointF &pt )
{
  PyrObject *obj = instantiateObject( gMainVMGlobals->gc, class_Point, 0, true, true );
  SetObject( slot, obj );

  PyrSlot *slots = obj->slots;
  SetFloat( slots+0, pt.x() );
  SetFloat( slots+1, pt.y() );
}

void Slot::setSize( PyrSlot *slot, const QSizeF &sz )
{
  PyrObject *obj = instantiateObject( gMainVMGlobals->gc, class_Size, 0, true, true );
  SetObject( slot, obj );

  PyrSlot *slots = obj->slots;
  SetFloat( slots+0, sz.width() );
  SetFloat( slots+1, sz.height() );
}

void Slot::setString( PyrSlot *slot, const QString& arg )
{
  PyrString *str = newPyrString( gMainVMGlobals->gc,
                                 arg.toStdString().c_str(), 0, true );
  SetObject( slot, str );
}

void Slot::setColor( PyrSlot *slot, const QColor &c )
{
  PyrObject *obj = instantiateObject( gMainVMGlobals->gc, class_Color, 0, true, true );
  SetObject( slot, obj );

  PyrSlot *slots = obj->slots;
  SetFloat( slots+0, c.red() / 255.0 );
  SetFloat( slots+1, c.green() / 255.0 );
  SetFloat( slots+2, c.blue() / 255.0 );
  SetFloat( slots+3, c.alpha() / 255.0 );
}

void Slot::setPalette( PyrSlot *slot, const QPalette &plt )
{
  PyrGC *gc = gMainVMGlobals->gc;
  PyrObject *obj = instantiateObject( gc, class_QPalette, 0, true, true );
  SetObject( slot, obj );

  PyrSlot *s = obj->slots;

  for( int i=0; i<8; ++i, ++s ) {
    setColor( s, plt.color( paletteColorRoles[i] ) );
    gc->GCWrite( obj, s );
  }
}

void Slot::setQObject( PyrSlot *s, QObject *o )
{
  if( !o ) {
    SetNil(s);
    return;
  }

  QObjectProxy *proxy = QObjectProxy::fromObject(o);
  if( proxy && proxy->scObject() )
    SetObject( s, proxy->scObject() );
  else
    SetNil( s );
}

void Slot::setTreeWidgetItem( PyrSlot *s, const SafePtr<QcTreeWidget::Item> & itemPtr )
{
  PyrObject *obj = instantiateObject( gMainVMGlobals->gc, class_QTreeViewItem, 0, true, true );
  QcTreeWidget::Item::initialize( gMainVMGlobals, obj, itemPtr );
  SetObject( s, obj );
}

void Slot::setVariantList( PyrSlot *slot, const VariantList& varList )
{
  VMGlobals *g = gMainVMGlobals;

  int count = varList.data.count();

  PyrObject *array = newPyrArray( g->gc, count, 0, true );
  SetObject( slot, array );

  int i;
  PyrSlot *s = array->slots;
  for( i = 0; i < count; ++i, ++s ) {
    if( !Slot::setVariant( s, varList.data[i] ) ) {
      qcDebugMsg(1, "WARNING: Could not set one slot of array" );
    }
    array->size++;
    g->gc->GCWrite( array, s );
  }
}

bool Slot::setVariant( PyrSlot *slot, const QVariant &val )
{
  bool b_val;
  int type = val.userType();

  switch( type ) {
    case QMetaType::Bool:
        b_val = val.toBool();
        if( b_val ) SetTrue( slot );
        else SetFalse( slot );
        break;

    case QMetaType::QPoint:
    case QMetaType::QPointF:
        Slot::setPoint( slot, val.toPointF() );
        break;

    case QMetaType::QSize:
    case QMetaType::QSizeF:
        Slot::setSize( slot, val.toSizeF() );
        break;

    case QMetaType::QRect:
    case QMetaType::QRectF:
        Slot::setRect( slot, val.toRectF() );
        break;

    case QMetaType::QString:
        Slot::setString( slot, val.toString() );
        break;

    case QMetaType::QColor:
         Slot::setColor( slot, val.value<QColor>() );
         break;

    case QMetaType::QPalette:
        Slot::setPalette( slot, val.value<QPalette>() );
        break;

    case QMetaType::Float:
    case QMetaType::Double:
        SetFloat( slot, val.value<double>() );
        break;

    case QMetaType::Int:
        SetInt( slot, val.toInt() );
        break;

    case QMetaType::QObjectStar:
        Slot::setQObject( slot, val.value<QObject*>() );
        break;

    case QMetaType::QWidgetStar:
        Slot::setQObject( slot, val.value<QWidget*>() );
        break;

    case QMetaType::Void:
        SetNil( slot );
        break;

    default:
        if( type == qMetaTypeId<PyrObject*>() ) {
          SetObject( slot, val.value<PyrObject*>() );
        }
        else if( type == qMetaTypeId<VariantList>() ) {
          Slot::setVariantList( slot, val.value<VariantList>() );
        }
        else if( type == qMetaTypeId<QcTreeWidget::ItemPtr>() ) {
          Slot::setTreeWidgetItem( slot, val.value< QtCollider::SafePtr<QcTreeWidget::Item> >() );
        }
        else {
          qcErrorMsg( "the QVariant could not be interpreted!" );
          return false;
        }
  }

  return true;
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

double Slot::toDouble( PyrSlot *slot )
{
  double d;
  if( slotDoubleVal( slot, &d ) ) return 0.0;
  return d;
}

QString Slot::toString( PyrSlot *slot )
{
  if( IsSym(slot) ) {
    return QString( slotRawSymbol(slot)->name );
  }
  else if( isKindOfSlot( slot, class_String ) ) {
    int len = slotRawObject( slot )->size;
    return QString::fromAscii( slotRawString(slot)->s, len );
  }
  return QString();
}

QPointF Slot::toPoint( PyrSlot *slot )
{
  if( !isKindOfSlot( slot, class_Point ) ) {
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
  if( !isKindOfSlot( slot, class_Rect ) ) {
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
  if( !isKindOfSlot( slot, class_Size ) ) {
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
  if( !isKindOfSlot( slot, class_Color) )
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
  if( !isKindOfSlot( slot, class_QFont ) )
    return QFont();

  PyrSlot *slots = slotRawObject(slot)->slots;

  QString family = Slot::toString( slots+0 );
  float fSize = Slot::toFloat( slots+1 );
  bool bold = IsTrue( slots+2 );
  bool italic = IsTrue( slots+3 );
  bool isPtSize = IsTrue( slots+4 );

  QFont f;

  if( !family.isEmpty() ) f.setFamily( family );

  if( fSize > 0.f ) {
    if( isPtSize ) {
      f.setPointSizeF( fSize );
    }
    else {
      int pixSize = ( fSize > 1.f ? qRound(fSize) : 1 );
      f.setPixelSize( pixSize );
    }
  }

  f.setBold( bold );

  f.setItalic( italic );

  return f;
}

QPalette Slot::toPalette( PyrSlot *slot )
{
  if( !isKindOfSlot( slot, class_QPalette ) )
    return QPalette();

  PyrSlot *slots = slotRawObject( slot )->slots;
  QPalette palette;

  for( int i=0; i<8; ++i, ++slots ) {
    QColor c = Slot::toColor(slots);
    if( !c.isValid() ) continue;
    palette.setColor( paletteColorRoles[i], c );
  }

  return palette;
}

VariantList Slot::toVariantList( PyrSlot *slot )
{
  if( isKindOfSlot( slot, class_Array ) ) {
    PyrObject *obj = slotRawObject( slot );
    PyrSlot *slots = obj->slots;
    int size = obj->size;
    VariantList list;
    for( int i = 0; i < size; ++i, ++slots )
      list.data << Slot::toVariant( slots );
    return list;
  }
  else if( isKindOfSlot( slot, class_SymbolArray ) ) {
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
  if( !isKindOfSlot( slot, class_QObject ) ) return 0;
  QObjectProxy *proxy = 0;
  PyrSlot *proxySlot = slotRawObject( slot )->slots;
  if( IsPtr( proxySlot ) ) proxy = (QObjectProxy*) slotRawPtr( proxySlot );
  return proxy;
}

QcTreeWidget::ItemPtr Slot::toTreeWidgetItem( PyrSlot *slot )
{
  if( !isKindOfSlot( slot, class_QTreeViewItem ) ) return QcTreeWidget::ItemPtr();
  PyrSlot *ptrSlot = slotRawObject(slot)->slots+0;
  if( IsPtr( ptrSlot ) ) {
    QcTreeWidget::ItemPtr *safePtr = static_cast<QcTreeWidget::ItemPtr*>( slotRawPtr(ptrSlot) );
    return *safePtr;
  }
  else {
    return QcTreeWidget::ItemPtr();
  }
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
      if( isKindOfSlot( slot, class_String ) ) {
        return QVariant( toString(slot) );
      }
      else if( isKindOfSlot( slot, class_Point ) ) {
        return QVariant( toPoint( slot ) );
      }
      else if( isKindOfSlot( slot, class_Rect ) ) {
        return QVariant( toRect(slot) );
      }
      else if( isKindOfSlot( slot, class_Size ) ) {
        return QVariant( toSize(slot) );
      }
      else if( isKindOfSlot( slot, class_Color) ) {
        return QVariant::fromValue<QColor>( toColor(slot) );
      }
      else if( isKindOfSlot( slot, class_QFont ) ) {
        return QVariant::fromValue<QFont>( toFont(slot) );
      }
      else if( isKindOfSlot( slot, class_QPalette ) ) {
        return QVariant::fromValue<QPalette>( toPalette(slot) );
      }
      else if( isKindOfSlot( slot, class_QObject ) ) {
        proxy = toObjectProxy(slot);
        return QVariant::fromValue<QObjectProxy*>( proxy );
      }
      else if( isKindOfSlot( slot, class_Array ) || isKindOfSlot( slot, class_SymbolArray ) ) {
        return QVariant::fromValue<VariantList>( toVariantList(slot) );
      }
      else if( isKindOfSlot( slot, class_QTreeViewItem ) ) {
        return QVariant::fromValue<QcTreeWidget::ItemPtr>( toTreeWidgetItem(slot) );
      }
      else {
        qcErrorMsg("Could not interpret slot!");
        return QVariant();
      }
    }
    default:
      return QVariant( toDouble( slot ) );
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
      if( isKindOfSlot( slot, class_String ) ) {
        _type = QMetaType::QString;
        _ptr = new QString( toString(slot) );
      }
      else if( isKindOfSlot( slot, class_Point ) ) {
        _type = QMetaType::QPointF;
        _ptr = new QPointF( toPoint(slot) );
      }
      else if( isKindOfSlot( slot, class_Rect ) ) {
        _type = QMetaType::QRectF;
        _ptr = new QRectF( toRect(slot) );
      }
      else if( isKindOfSlot( slot, class_Size ) ) {
        _type = QMetaType::QSizeF;
        _ptr = new QSizeF( toSize(slot) );
      }
      else if( isKindOfSlot( slot, class_Color) ) {
        _type = QMetaType::QColor;
        _ptr = new QColor( toColor(slot) );
      }
      else if( isKindOfSlot( slot, class_Array ) || isKindOfSlot( slot, class_SymbolArray ) ) {
        _type = qMetaTypeId<VariantList>();
        _ptr = new VariantList( toVariantList(slot) );
      }
      else if( isKindOfSlot( slot, class_QObject ) ) {
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
      else if( isKindOfSlot( slot, class_QTreeViewItem ) ) {
        _type = qMetaTypeId<QcTreeWidget::ItemPtr>();
        _ptr = new QcTreeWidget::ItemPtr( toTreeWidgetItem(slot) );
      }
      else {
        qcErrorMsg("Could not interpret slot!");
        _type = QMetaType::Void;
        _ptr = 0;
      }
      break;
    }
    default :
      _type = QMetaType::Double;
      _ptr = new double( toDouble(slot) );
  }
}
