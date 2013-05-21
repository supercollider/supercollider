/************************************************************************
*
* Copyright 2010-2012 Jakob Leben (jakob.leben@gmail.com)
*
* This file is part of SuperCollider Qt GUI.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 2 of the License, or
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

#include "layouts.hpp"

QC_DECLARE_QOBJECT_FACTORY(QcHBoxLayout);
QC_DECLARE_QOBJECT_FACTORY(QcVBoxLayout);
QC_DECLARE_QOBJECT_FACTORY(QcGridLayout);
QC_DECLARE_QOBJECT_FACTORY(QcStackLayout);

void QcGridLayout::addItem( const QVariantList &data )
{
  if( data.count() < 6 ) return;

  int row = data[1].toInt();
  int column = data[2].toInt();
  int rSpan = data[3].toInt();
  int cSpan = data[4].toInt();
  Qt::Alignment alignment = (Qt::Alignment) data[5].toInt();

  QVariant varObject = data[0];

  QObjectProxy *p = varObject.value<QObjectProxy*>();

  if( !p || !p->object() ) return;

  QWidget *w = qobject_cast<QWidget*>( p->object() );
  if( w ) {
    addWidget( w, row, column, rSpan, cSpan, alignment );
    return;
  }

  QLayout *l = qobject_cast<QLayout*>( p->object() );
  if(l) {
    addLayout( l, row, column, rSpan, cSpan, alignment );
    return;
  }
}
