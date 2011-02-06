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

#include "QWidgetProxy.h"

#include <QApplication>

using namespace QtCollider;

QWidgetProxy::QWidgetProxy( QWidget *w, PyrObject *po ) : QObjectProxy( w, po )
{ }

bool QWidgetProxy::setFocus( QtCollider::SetFocusRequest *r ) {
  if( !widget() ) return true;

  if( r->focus )
    widget()->setFocus( Qt::OtherFocusReason );
  else
    widget()->clearFocus();

  return true;
}

bool QWidgetProxy::bringFront() {
  QWidget *w = widget();
  if( !w ) return true;
  w->setWindowState( w->windowState() & ~Qt::WindowMinimized
                                      | Qt::WindowActive );
  w->show();
  w->raise();

  return true;
}

bool QWidgetProxy::refresh() {
  QWidget *w = widget();
  if( w ) sendRefreshEventRecursive( w );
  return true;
}

bool QWidgetProxy::mapToGlobal( QtCollider::MapToGlobalRequest *r )
{
  QWidget *w = widget();
  if( w ) r->point = w->mapToGlobal( r->point );
  return true;
}

bool QWidgetProxy::setParentEvent( QtCollider::SetParentEvent *e ) {

  QObject *parent = e->parent->object();
  if( !parent || !widget() ) return true;

  if( parent->isWidgetType() ) {
    QWidget *pw = qobject_cast<QWidget*>(parent);
    bool ok = pw->metaObject()->invokeMethod( pw, "addChild", Q_ARG( QWidget*, widget() ) );
    if( !ok ) widget()->setParent( pw );
    return true;
  }
  return false;
}

void QWidgetProxy::sendRefreshEventRecursive( QWidget *w ) {
  QEvent event( static_cast<QEvent::Type>( QtCollider::Event_Refresh ) );
  QApplication::sendEvent( w, &event );
  const QObjectList &children = w->children();
  Q_FOREACH( QObject *child, children ) {
    if( child->isWidgetType() )
        sendRefreshEventRecursive( static_cast<QWidget*>( child ) );
  }
}
