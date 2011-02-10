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

#ifndef _SLOT_H
#define _SLOT_H

#include <QString>
#include <QRect>
#include <QFont>
#include <QColor>
#include <QList>
#include <QVariant>
#include <QPalette>

#include <PyrSlot.h>

#include "Common.h"

class QObjectProxy;

namespace QtCollider {

namespace Slot
{
  bool toBool( PyrSlot * );
  int toInt( PyrSlot * );
  float toFloat( PyrSlot * );
  QString toString( PyrSlot * );
  QPointF toPoint( PyrSlot * );
  QRectF toRect( PyrSlot * );
  QSizeF toSize( PyrSlot * );
  QColor toColor( PyrSlot * );
  QFont toFont( PyrSlot * );
  QPalette toPalette( PyrSlot * );
  VariantList toVariantList( PyrSlot * );
  QObjectProxy * toObjectProxy( PyrSlot * );
  QVariant toVariant( PyrSlot * );

  int setRect( PyrSlot *, const QRectF & );
  int setPoint( PyrSlot *, const QPointF & );
  void setString( PyrSlot *, const QString & );
  int setColor( PyrSlot *, const QColor & );
  int setPalette( PyrSlot *, const QPalette & );
  void setVariantList( PyrSlot *, const VariantList& );
  int setVariant( PyrSlot *, const QVariant & );
};

class Variant
{
public:

  Variant() :
    _type( QMetaType::Void ),
    _ptr(0)
    {}

  Variant( PyrSlot *slot )
    { setData( slot ); }

  ~Variant() {
    if( _ptr ) {
      QMetaType::destroy( _type, _ptr );
    }
  }

  void setData( PyrSlot * );

  QGenericArgument asGenericArgument() {
    if( _type != QMetaType::Void )
      return QGenericArgument( QMetaType::typeName(_type), _ptr );
    else
      return QGenericArgument();
  }

  int type() { return _type; }

private:

  int _type;
  void *_ptr;
};

} // namespace QtCollider

#endif
