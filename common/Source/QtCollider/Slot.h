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

#include <PyrSlot.h>

#include "Common.h"

class QObjectProxy;

struct Slot
{
  public:
    Slot() :
      type( QMetaType::Void ),
      ptr(0)
      {}
    Slot( PyrSlot *slot ) :
      type( QMetaType::Void ),
      ptr(0)
      { Q_UNUSED(slot); }
    ~Slot() {
      if( ptr ) {
        QMetaType::destroy( type, ptr );
      }
    }
    void set( PyrSlot * );
    QGenericArgument asGenericArgument() {
      if( type != QMetaType::Void )
        return QGenericArgument( QMetaType::typeName(type), ptr );
      else
        return QGenericArgument();
    }

  private:
    int type;
    void *ptr;

  public:
    static bool toBool( PyrSlot * );
    static int toInt( PyrSlot * );
    static float toFloat( PyrSlot * );
    static QString toString( PyrSlot * );
    static QPointF toPoint( PyrSlot * );
    static QRectF toRect( PyrSlot * );
    static QColor toColor( PyrSlot * );
    static QFont toFont( PyrSlot * );
    static QPalette toPalette( PyrSlot * );
    static VariantList toVariantList( PyrSlot * );
    static QObjectProxy* toObjectProxy( PyrSlot * );
    static QVariant toVariant( PyrSlot * );

    static int setRect( PyrSlot *, const QRectF & );
    static void setString( PyrSlot *, const QString& arg );
    static void setVariantList( PyrSlot *, const VariantList& );
    static int setVariant( PyrSlot *, const QVariant & );
};

#endif
