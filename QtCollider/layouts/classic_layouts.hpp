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

#ifndef QC_CLASSIC_LAYOUTS_H
#define QC_CLASSIC_LAYOUTS_H

#include <QLayout>
#include <QLayoutItem>
#include <QList>

namespace QtCollider {

enum HSizePolicy {
  StickLeft = 1,
  HStretch,
  StickRight
};

enum VSizePolicy {
  StickTop = 1,
  VStretch,
  StickBottom
};

} // namespace

class QcLayoutItem {
  public:
    QcLayoutItem( QLayoutItem *i )
    : qLayoutItem( i )
    {}

    QtCollider::VSizePolicy vSizePolicy();
    QtCollider::HSizePolicy hSizePolicy();
    QRect geometry ();
    void setGeometry ( const QRect & r );

    QLayoutItem *qLayoutItem;
};

class QcAbstractLayout : public QLayout {

  public:
    QcAbstractLayout( QWidget *parent = 0 ) : QLayout( parent ) {}
    virtual ~QcAbstractLayout();
    int count() const;
    void addItem( QLayoutItem* item );
    QLayoutItem * itemAt ( int index ) const;
    QLayoutItem * takeAt ( int index );
    QSize sizeHint () const;

  protected:
    virtual void setGeometry ( const QRect & r ) = 0;
    QList<QcLayoutItem*> items;
};

/////////////////////////// IMPLEMENTATIONS //////////////////////////////

class QcDefaultLayout : public QcAbstractLayout {
  public:
    QcDefaultLayout() : initialized( false ) {}
  private:
    void setGeometry ( const QRect & r );
    QRect geom;
    bool initialized;
};

class QcHLayout : public QcAbstractLayout
{
  private:
    void setGeometry ( const QRect & r );
};

class QcVLayout : public QcAbstractLayout
{
  private:
    void setGeometry ( const QRect & r );
};

#endif // QC_CLASSIC_LAYOUTS_H
