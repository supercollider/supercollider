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
#include <QGridLayout>

template<class BOXLAYOUT> class QcBoxLayoutFactory : public QcObjectFactory<BOXLAYOUT>
{
protected:
  virtual void initialize( QObjectProxy *, BOXLAYOUT *l, QList<QVariant> & items ) {
    Q_FOREACH( QVariant v, items ) {
      VariantList item = v.value<VariantList>();
      l->addItem( item );
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
};

template <class BOXLAYOUT> class QcBoxLayout : public QcLayout<BOXLAYOUT>
{
public:
  void addItem( const VariantList & item ) {
    if( item.data.size() < 3 ) return;

    int stretch = item.data[1].toInt();
    Qt::Alignment alignment = (Qt::Alignment) item.data[2].toInt();

    QVariant varObject = item.data[0];

    if( varObject.canConvert<int>() ) {
      int size = varObject.toInt();
      if( size > 0 ) BOXLAYOUT::addSpacing( size );
      else BOXLAYOUT::addStretch( stretch );
      return;
    }

    QObjectProxy *p = varObject.value<QObjectProxy*>();
    if( !p || !p->object() ) return;

    QWidget *w = qobject_cast<QWidget*>( p->object() );
    if( w ) {
      BOXLAYOUT::addWidget( w, stretch, alignment );
      return;
    }

    QLayout *l2 = qobject_cast<QLayout*>( p->object() );
    if(l2) {
      BOXLAYOUT::addLayout( l2, stretch );
      return;
    }
  }
};

class QcHBoxLayout : public QcBoxLayout<QHBoxLayout>
{
  Q_OBJECT
  Q_PROPERTY( VariantList margins READ margins WRITE setMargins )
public:
  Q_INVOKABLE void addItem( const VariantList &data ) { QcBoxLayout<QHBoxLayout>::addItem(data); }
};

class QcVBoxLayout : public QcBoxLayout<QVBoxLayout>
{
  Q_OBJECT
  Q_PROPERTY( VariantList margins READ margins WRITE setMargins )
public:
  Q_INVOKABLE void addItem( const VariantList &data ) { QcBoxLayout<QVBoxLayout>::addItem(data); }
};

class QcGridLayout : public QcLayout<QGridLayout>
{
  Q_OBJECT
  Q_PROPERTY( VariantList margins READ margins WRITE setMargins )
  Q_PROPERTY( int verticalSpacing READ verticalSpacing WRITE setVerticalSpacing )
  Q_PROPERTY( int horizontalSpacing READ horizontalSpacing WRITE setHorizontalSpacing )
public:
  Q_INVOKABLE void addItem( const VariantList &dataList );
  Q_INVOKABLE void setRowStretch( int row, int factor ) {
    QcLayout<QGridLayout>::setRowStretch( row, factor );
  }
  Q_INVOKABLE void setColumnStretch( int column, int factor ) {
    QcLayout<QGridLayout>::setColumnStretch( column, factor );
  }
};

#endif
