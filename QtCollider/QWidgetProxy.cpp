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
#include "Painting.h"

#include <QApplication>
#include <QLayout>
#include <QMouseEvent>
#include <QKeyEvent>

using namespace QtCollider;

QWidgetProxy::QWidgetProxy( QWidget *w, PyrObject *po )
: QObjectProxy( w, po ), _keyEventWidget( w ), _mouseEventWidget( w )
{ }

void QWidgetProxy::setKeyEventWidget( QWidget *w )
{
  if( w == 0 || w == _keyEventWidget ) return;

  QWidget *me = widget();

  if( _keyEventWidget != me )
    _keyEventWidget->removeEventFilter( this );

  _keyEventWidget = w;

  if( _keyEventWidget != me ) {
    _keyEventWidget->installEventFilter( this );
  }
}

void QWidgetProxy::setMouseEventWidget( QWidget *w )
{
  if( w == 0 || w == _mouseEventWidget ) return;

  QWidget *me = widget();

  if( _mouseEventWidget != me )
    _mouseEventWidget->removeEventFilter( this );

  _mouseEventWidget = w;

  if( _mouseEventWidget != me ) {
    _mouseEventWidget->installEventFilter( this );
  }
}

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

bool QWidgetProxy::setLayout ( SetLayoutRequest *r ) {

  QWidget *w = widget();
  QLayout *l = qobject_cast<QLayout*>( r->layoutProxy->object() );
  if( !w || !l ) return true;

  QLayout *exLayout = w->layout();
  if( exLayout != l ) {
    if( exLayout != 0 ) {
      qcDebugMsg( 2, QString("Deleting old layout.") );
      delete exLayout;
    }
    qcDebugMsg( 2, QString("Setting layout.") );
    w->setLayout( l );
    l->activate();
  }
  else {
    qcDebugMsg( 2, QString("Layout same as existing. Will do nothing.") );
  }

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

bool QWidgetProxy::interpretEvent( QObject *o, QEvent *e, QList<QVariant> &args )
{
  switch( e->type() ) {
    case QEvent::MouseButtonPress:
    case QEvent::MouseMove:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseButtonDblClick:
    case QEvent::Enter: {
      if( o == _mouseEventWidget ) {
        interpretMouseEvent( e, args );
        return true;
      }
      else return false;
    }
    case QEvent::KeyPress:
    case QEvent::KeyRelease: {
      if( o == _keyEventWidget ) {
        interpretKeyEvent( e, args );
        return true;
      }
      else return false;
    }
    default: return QObjectProxy::interpretEvent( o, e, args );
  }
}

void QWidgetProxy::customPaint( QPainter *painter )
{
  if( QtCollider::paintingAnnounced() ) {
    qcDebugMsg(1, "WARNING: Custom painting already in progress. Will not paint." );
    return;
  }

  QtCollider::announcePainting();

  QtCollider::lockLang();

  if( QtCollider::beginPainting( painter ) ) {
    invokeScMethod( getsym("doDrawFunc"), QList<QVariant>(), 0, true );
    QtCollider::endPainting();
  }

  QtCollider::unlockLang();
}

void QWidgetProxy::interpretMouseEvent( QEvent *e, QList<QVariant> &args )
{
  // NOTE We assume that qObject need not be checked here, as we wouldn't get events if
  // it wasn't existing

  QWidget *w = widget();

  if( e->type() == QEvent::Enter ) {
    QPoint pos = QCursor::pos();


    if( w ) pos = w->mapFromGlobal( pos );

    args << pos.x();
    args << pos.y();
    return;
  }

  QMouseEvent *mouse = static_cast<QMouseEvent*>( e );
  QPoint pt = ( _mouseEventWidget == w ?
                mouse->pos() :
                _mouseEventWidget->mapTo( w, mouse->pos() ) );
  args << pt.x();
  args << pt.y();

  args << (int) mouse->modifiers();

  if( e->type() == QEvent::MouseMove ) return;

  int button;
  switch( mouse->button() ) {
    case Qt::LeftButton:
      button = 0; break;
    case Qt::RightButton:
      button = 1; break;
    case Qt::MidButton:
      button = 2; break;
    default:
      button = -1;
  }

  args << button;

  switch( e->type() ) {
    case QEvent::MouseButtonPress:
      args << 1; break;
    case QEvent::MouseButtonDblClick:
      args << 2; break;
    default: ;
  }
}

void QWidgetProxy::interpretKeyEvent( QEvent *e, QList<QVariant> &args )
{
  QKeyEvent *ke = static_cast<QKeyEvent*>( e );

  QString text = ke->text();
  int unicode = ( text.count() == 1 ? text[0].unicode() : 0 );

  args << text;
  args << (int) ke->modifiers();
  args << unicode;
  args << ke->key();
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
