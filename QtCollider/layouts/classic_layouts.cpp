/************************************************************************
*
* Copyright 2010-2011 Jakob Leben (jakob.leben@gmail.com)
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

#include <QVariant>
#include <QWidget>

#include "classic_layouts.hpp"
#include "../Common.h"

using namespace QtCollider;

VSizePolicy QcLayoutItem::vSizePolicy() {
  QWidget *w = qLayoutItem->widget();
  if( !w ) return StickTop;
  QVariant var = w->property( "_qc_vSizePolicy" );
  if( !var.isValid() ) return StickTop;
  return (VSizePolicy) var.toInt();
}

HSizePolicy QcLayoutItem::hSizePolicy() {
  QWidget *w = qLayoutItem->widget();
  if( !w ) return StickLeft;
  QVariant var = w->property( "_qc_hSizePolicy" );
  if( !var.isValid() ) return StickLeft;
  return (HSizePolicy) var.toInt();
}

QRect QcLayoutItem::geometry () {
  QWidget *widget = qLayoutItem->widget();
  if( widget )
    return widget->geometry();
  else
    return qLayoutItem->geometry();
}

void QcLayoutItem::setGeometry( const QRect &r ) {
  QWidget *widget = qLayoutItem->widget();
  if( widget )
    widget->setGeometry( r );
  else
    qLayoutItem->setGeometry( r );
}

QcAbstractLayout::~QcAbstractLayout() {
  QLayoutItem *item;
  while((item = takeAt(0)))
    delete item;
}

int QcAbstractLayout::count() const {
  return items.size();
}

void QcAbstractLayout::addItem( QLayoutItem* item ) {
    qcDebugMsg(2,"QcAbstractLayout::addItem");
    QWidget *w = item->widget();
    items.append( new QcLayoutItem( item ) );
}

QLayoutItem * QcAbstractLayout::itemAt ( int index ) const {
  if( index >= items.size() ) return 0;
  return items[index]->qLayoutItem;
}

QLayoutItem * QcAbstractLayout::takeAt ( int index ) {
  if( index < items.size() ) {
    QcLayoutItem *scItem = items.takeAt(index);
    QLayoutItem *item = scItem->qLayoutItem;
    delete scItem;
    return item;
  }
  else return 0;
}

QSize QcAbstractLayout::sizeHint () const {
  return QSize();
}

/////////////////////////// IMPLEMENTATIONS ////////////////////////////////////

void QcDefaultLayout::setGeometry ( const QRect & r )
{
  if( !initialized ) {
    initialized = true;
    geom = r;
    return;
  }
  qcDebugMsg(2,"QcDefaultLayout::setGeometry");
  QPoint dPos = r.topLeft() - geom.topLeft();
  QSize dSize = r.size() - geom.size();//_geometry.size();
  //qcDebugMsg(2,"dSize: %i,%i", dSize.width(), dSize.height() );
  Q_FOREACH( QcLayoutItem* item, items ) {
    QRect g = item->geometry();
    int x = g.x();
    int y = g.y();
    int w = g.width();
    int h = g.height();
    //qcDebugMsg(2,"geom: %i %i %i %i", x,y,w,h);

    if( !dPos.isNull() ) {
      x += dPos.x();
      y += dPos.y();
    }

    if( !dSize.isNull() ) {
      if( item->hSizePolicy() == QtCollider::StickRight )
        x += dSize.width();
      if( item->vSizePolicy() == QtCollider::StickBottom )
        y += dSize.height();
      if( item->hSizePolicy() == QtCollider::HStretch )
        w += dSize.width();
      if( item->vSizePolicy() == QtCollider::VStretch )
        h += dSize.height();
    }

    item->setGeometry( QRect(x, y, w, h) );
  }
  geom = r;
}

void QcHLayout::setGeometry ( const QRect & geom )
{
  int varWidth = geom.width();
  int i = items.size();
  Q_FOREACH( QcLayoutItem *item, items ) {
    QRect r = item->geometry();
    if( item->hSizePolicy() != QtCollider::HStretch ) {
      varWidth -= r.width();
      i--;
      if( varWidth < 0 ) {
        varWidth = 0;
        break;
      }
    }
  }
  int partWidth = i > 0 && varWidth > 0 ? varWidth / i : 0;
  int x = 0;
  Q_FOREACH( QcLayoutItem *item, items ) {
    QRect r = item->geometry();
    r.setHeight( geom.height() );
    r.moveTo( x, geom.top() );
    if( item->hSizePolicy() == QtCollider::HStretch )
      r.setWidth( partWidth );
    x += r.width();

    item->setGeometry( r );
  }
}

void QcVLayout::setGeometry ( const QRect & geom )
{
  int varHeight = geom.height();
  int i = items.size();
  Q_FOREACH( QcLayoutItem *item, items ) {
    QRect r = item->geometry();
    if( item->vSizePolicy() != QtCollider::VStretch ) {
      varHeight -= r.height();
      i--;
      if( varHeight < 0 ) {
        varHeight = 0;
        break;
      }
    }
  }
  int partHeight = i > 0 && varHeight > 0 ? varHeight / i : 0;
  int y = 0;
  Q_FOREACH( QcLayoutItem *item, items ) {
    QRect r = item->geometry();
    r.setWidth( geom.width() );
    r.moveTo( geom.left(), y );
    if( item->vSizePolicy() == QtCollider::VStretch )
      r.setHeight( partHeight );
    y += r.height();

    item->setGeometry( r );
  }
}
