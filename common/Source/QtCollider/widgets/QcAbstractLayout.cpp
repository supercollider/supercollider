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

#include <QVariant>
#include <QWidget>

#include "QcAbstractLayout.h"
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

QcAbstractLayout::~QcAbstractLayout() {
  QLayoutItem *item;
  while((item = takeAt(0)))
    delete item;
}

int QcAbstractLayout::count() const {
  return items.size();
}

void QcAbstractLayout::addItem( QLayoutItem* item ) {
    qscDebugMsg("addItem\n");
    QWidget *w = item->widget();
    if( w ) {
      w->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );
    }
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
