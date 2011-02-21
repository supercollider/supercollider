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

#ifndef QC_OBJECT_FACTORY_H
#define QC_OBJECT_FACTORY_H

#include "QObjectProxy.h"
#include "Common.h"

#include <PyrObject.h>

#include <QObject>
#include <QWidget>
#include <QHash>

class QcAbstractFactory;

typedef QHash<QString,QcAbstractFactory*> QcFactoryHash;

namespace QtCollider {
  QcFactoryHash & factories ();
}

class QcAbstractFactory
{
public:
  QcAbstractFactory( const char *className ) {
    qcDebugMsg( 2, QString("Declaring class '%1'").arg(className) );
    QtCollider::factories().insert( className, this );
  }
  virtual QObjectProxy *newInstance( PyrObject *, QList<QVariant> & arguments ) = 0;
};


template <class QOBJECT> class QcObjectFactory : public QcAbstractFactory
{
public:
  QcObjectFactory() : QcAbstractFactory( QOBJECT::staticMetaObject.className() ) {}

  virtual QObjectProxy *newInstance( PyrObject *scObject, QList<QVariant> & arguments ) {
    QOBJECT *qobject = new QOBJECT();
    QObject *qo = qobject; // template parameter type-safety

    QObjectProxy *proxy = new QObjectProxy ( qobject, scObject );

    initialize( proxy, qobject, arguments );

    return proxy;
  }

protected:
  virtual void initialize( QObjectProxy *, QOBJECT *, QList<QVariant> & arguments ) {};
};



#endif //QC_OBJECT_FACTORY_H
