/************************************************************************
*
* Copyright 2010-2012 Jakob Leben (jakob.leben@gmail.com)
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

#ifndef QC_WIDGET_FACTORY_H
#define QC_WIDGET_FACTORY_H

#include "QcObjectFactory.h"
#include "QWidgetProxy.h"

#include <QLayout>

template <class QWIDGET>
class QcWidgetFactory : public QcObjectFactory<QWIDGET>
{
public:

  virtual QObjectProxy *newInstance( PyrObject *scObject, QtCollider::Variant arg[10] ) {

    // Omit the first two arguments - parent and bounds

    QWIDGET *w;

    if( arg[2].type() == QMetaType::Void ) {
      w = new QWIDGET;
    }
    else {
      QObject *obj = QWIDGET::staticMetaObject.newInstance(
        arg[2].asGenericArgument(),
        arg[3].asGenericArgument(),
        arg[4].asGenericArgument(),
        arg[5].asGenericArgument(),
        arg[6].asGenericArgument(),
        arg[7].asGenericArgument(),
        arg[8].asGenericArgument(),
        arg[9].asGenericArgument()
      );

      w = qobject_cast<QWIDGET*>(obj);
      if( !w ) {
        qcErrorMsg( QString("No appropriate constructor found for '%1'.")
          .arg( QWIDGET::staticMetaObject.className() ) );
        return 0;
      }
    }

    QObjectProxy *prox( proxy(w, scObject) );

    // check if parent arg is valid;

    QObjectProxy *parentProxy( arg[0].value<QObjectProxy*>() );


    // set requested geometry

    QRect r( arg[1].value<QRectF>().toRect() );
    if( r.size().isEmpty() ) r.setSize( w->sizeHint() );

    w->setGeometry( r );

    // automatically support drag and drop

    w->setAcceptDrops( true );

    // set parent

    QWidget *parent = parentProxy ? qobject_cast<QWidget*>( parentProxy->object() ) : 0;

    if( parent ) {

      // Ok, we have the parent, so stuff the child in

      const QMetaObject *mo = parent->metaObject();
      bool ok = mo->invokeMethod( parent, "addChild", Q_ARG( QWidget*, w ) );

      if( !ok ) w->setParent( parent );

      w->show();
    }

    return prox;
  }

protected:

  virtual QObjectProxy * proxy( QWIDGET *w, PyrObject *sc_obj )
  {
    QWidgetProxy *prox( new QWidgetProxy( w, sc_obj ) );
    initialize( prox, w );
    return prox;
  }

  virtual void initialize( QWidgetProxy *proxy, QWIDGET *obj ) {};
};

#define QC_DECLARE_QWIDGET_FACTORY( QWIDGET ) QC_DECLARE_FACTORY( QWIDGET, QcWidgetFactory<QWIDGET> )

#endif
