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

#ifndef QC_LAYOUTS_H
#define QC_LAYOUTS_H

#include "../QcObjectFactory.h"

#include <QHBoxLayout>
#include <QVBoxLayout>

template<class BOXLAYOUT> class QcBoxLayoutFactory : public QcObjectFactory<BOXLAYOUT>
{
protected:
  virtual void initialize( BOXLAYOUT *l, QList<QVariant> & args ) {
    printf("initializing\n");

    int s = args.size();
    if( s > 0 ) {
      VariantList items = args[0].value<VariantList>();
      Q_FOREACH( QVariant v, items.data ) {
        VariantList item = v.value<VariantList>();
        l->addItem( item );
      }
    }

    if( s > 1 ) {
      int spacing = args[1].value<int>();
      l->setSpacing( spacing );
    }

    if( s > 2 ) {
      VariantList margins = args[2].value<VariantList>();
      l->setMargins( margins );
    }
  }
};

template <class LAYOUT> struct QcLayout : public LAYOUT
{
public:
  VariantList margins() const { return VariantList(); }

  void setMargins( const VariantList &list ) {
    if( list.data.size() < 4 ) return;
    int m[4];
    for( int i=0; i<4; ++i ) {
      m[i] = list.data[i].value<int>();
    }
    LAYOUT::setContentsMargins( m[0], m[1], m[2], m[3] );
  }

  void addItem( const VariantList & item ) {
    if( item.data.size() < 3 ) {
      printf("item data array empty or too short\n");
      return;
    }

    int stretch = item.data[1].toInt();
    Qt::Alignment alignment = (Qt::Alignment) item.data[2].toInt();

    QVariant varObject = item.data[0];

    if( varObject.canConvert<int>() ) {
      printf( "a spacer\n");
      int size = varObject.toInt();
      if( size > 0 ) LAYOUT::addSpacing( size );
      else LAYOUT::addStretch( stretch );
      return;
    }

    QObjectProxy *p = varObject.value<QObjectProxy*>();

    if( !p ) {
      printf( "not a proxy\n");
      return;
    }

    if( !p->object() ) {
      printf("invalid proxy\n");
      return;
    }

    QWidget *w = qobject_cast<QWidget*>( p->object() );
    if( w ) {
      printf("a widget\n");
      LAYOUT::addWidget( w, stretch, alignment );
      return;
    }

    QLayout *l2 = qobject_cast<QLayout*>( p->object() );
    if(l2) {
      printf("a layout\n");
      LAYOUT::addLayout( l2, stretch );
      return;
    }

    printf("not a widget or a layout\n");
  }
};

class QcHBoxLayout : public QcLayout<QHBoxLayout>
{
  Q_OBJECT
  Q_PROPERTY( VariantList margins READ margins WRITE setMargins )
public:
  Q_INVOKABLE void addItem( const VariantList &data ) { QcLayout<QHBoxLayout>::addItem(data); }
};

class QcVBoxLayout : public QcLayout<QVBoxLayout>
{
  Q_OBJECT
  Q_PROPERTY( VariantList margins READ margins WRITE setMargins )
public:
  Q_INVOKABLE void addItem( const VariantList &data ) { QcLayout<QVBoxLayout>::addItem(data); }
};

#endif
