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
#include "widgets/QcTreeWidget.h"

class QObjectProxy;

namespace QtCollider {

namespace Slot
{
  bool toBool( PyrSlot * );
  int toInt( PyrSlot * );
  float toFloat( PyrSlot * );
  double toDouble( PyrSlot * );
  QString toString( PyrSlot * );
  QPointF toPoint( PyrSlot * );
  QRectF toRect( PyrSlot * );
  QSizeF toSize( PyrSlot * );
  QColor toColor( PyrSlot * );
  QFont toFont( PyrSlot * );
  QPalette toPalette( PyrSlot * );
  VariantList toVariantList( PyrSlot * );
  QObjectProxy * toObjectProxy( PyrSlot * );
  QcTreeWidget::ItemPtr toTreeWidgetItem( PyrSlot * );
  QVariant toVariant( PyrSlot * );

  void setRect( PyrSlot *, const QRectF & );
  void setPoint( PyrSlot *, const QPointF & );
  void setSize( PyrSlot *, const QSizeF & );
  void setString( PyrSlot *, const QString & );
  void setColor( PyrSlot *, const QColor & );
  void setPalette( PyrSlot *, const QPalette & );
  void setQObject( PyrSlot *, QObject * );
  void setTreeWidgetItem( PyrSlot *, const QcTreeWidget::ItemPtr & );
  void setVariantList( PyrSlot *, const VariantList& );
  bool setVariant( PyrSlot *, const QVariant & );
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

  template<typename T> T value() {
    if( qMetaTypeId<T>() == _type )
      return *reinterpret_cast<T*>(_ptr);
    else
      return T();
  }

private:

  int _type;
  void *_ptr;
};

} // namespace QtCollider

#endif
