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

#ifndef QC_WIDGET_PROXY_H
#define QC_WIDGET_PROXY_H

#include "QObjectProxy.h"

#include <QWidget>

namespace QtCollider {
  struct SetFocusRequest;
  struct MapToGlobalRequest;
}

class QWidgetProxy : public QObjectProxy
{
  Q_OBJECT

public:

  QWidgetProxy( QWidget *w, PyrObject *po );

  bool setFocus( QtCollider::SetFocusRequest *r );

  bool bringFront();

  bool refresh();

  bool mapToGlobal( QtCollider::MapToGlobalRequest * );

  virtual bool setParentEvent( QtCollider::SetParentEvent *e );

protected:

  inline QWidget *widget() { return static_cast<QWidget*>( object() ); }

private:

  static void sendRefreshEventRecursive( QWidget *w );
};

namespace QtCollider {

class GenericWidgetRequest : public RequestEvent
{
public:
  GenericWidgetRequest( bool (QWidgetProxy::*h)() ) : handler(h) {}
protected:
  virtual bool execute( QObjectProxy *proxy ) {
    QWidgetProxy *wproxy = qobject_cast<QWidgetProxy*>( proxy );
    return (wproxy->*handler)();
  }
private:
  bool (QWidgetProxy::*handler)();
};

template <class T, bool (QWidgetProxy::*handler)( T* )>
class WidgetRequestTemplate : public RequestEvent
{
protected:
  WidgetRequestTemplate(){}
private:
  bool execute( QObjectProxy *proxy ) {
    QWidgetProxy *wproxy = qobject_cast<QWidgetProxy*>( proxy );
    return (wproxy->*handler)( static_cast<T*>( this ) );
  }
};

struct SetFocusRequest
: public WidgetRequestTemplate<SetFocusRequest, &QWidgetProxy::setFocus>
{
  bool focus;
};

struct MapToGlobalRequest
: public WidgetRequestTemplate<MapToGlobalRequest, &QWidgetProxy::mapToGlobal>
{
  MapToGlobalRequest( QPoint &pt ) : point(pt) {}
  QPoint &point;
};

}

#endif //QC_WIDGET_PROXY_H
