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
#include "painting.h"
#include "Common.h"

#include <QApplication>
#include <QLayout>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QPainter>
#include <QFontMetrics>
#include <QUrl>

#ifdef Q_WS_X11
#include <QX11Info>
#include "hacks/hacks_x11.hpp"
#endif

using namespace QtCollider;

QAtomicInt QWidgetProxy::_globalEventMask = 0;

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

bool QWidgetProxy::alwaysOnTop()
{
  QWidget *w = widget();
  if(!w) return false;

  Qt::WindowFlags flags = w->windowFlags();
  if( flags & Qt::Window && flags & Qt::WindowStaysOnTopHint ) return true;
  else return false;
}

void QWidgetProxy::refresh() {
  QWidget *w = widget();
  if( w ) sendRefreshEventRecursive( w );
}

void QWidgetProxy::setLayout ( QObjectProxy *layoutProxy ) {

  QWidget *w = widget();
  QLayout *l = qobject_cast<QLayout*>( layoutProxy->object() );
  if( !w || !l ) return;

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
}

bool QWidgetProxy::setParent( QObjectProxy *parentProxy )
{
  QObject *parent = parentProxy->object();
  if( !parent || !widget() ) return true;

  if( parent->isWidgetType() ) {
    QWidget *pw = qobject_cast<QWidget*>(parent);
    bool ok = pw->metaObject()->invokeMethod( pw, "addChild", Q_ARG( QWidget*, widget() ) );
    if( !ok ) widget()->setParent( pw );
    return true;
  }
  return false;
}

void QWidgetProxy::customEvent( QEvent *e )
{
  int type = e->type();
  switch( type ) {
    case QtCollider::Event_Proxy_BringFront:
      bringFrontEvent();
      return;
    case QtCollider::Event_Proxy_SetFocus:
      setFocusEvent( static_cast<SetFocusEvent*>(e) );
      return;
    case QtCollider::Event_Proxy_SetAlwaysOnTop:
      setAlwaysOnTopEvent( static_cast<SetAlwaysOnTopEvent*>(e) );
      return;
    case QtCollider::Event_Proxy_StartDrag:
      startDragEvent( static_cast<StartDragEvent*>(e) );
      return;
    default:
      QObjectProxy::customEvent(e);
  }
}




void QWidgetProxy::bringFrontEvent() {
  QWidget *w = widget();
  if( !w ) return;

  w->setWindowState( w->windowState() & ~Qt::WindowMinimized
                                      | Qt::WindowActive );
  w->show();
  w->raise();

#ifdef Q_WS_X11
  raise_window(QX11Info::display(), w);
#endif
}

void QWidgetProxy::setFocusEvent( QtCollider::SetFocusEvent *e ) {
  if( !widget() ) return;

  if( e->focus )
    widget()->setFocus( Qt::OtherFocusReason );
  else
    widget()->clearFocus();
}

void QWidgetProxy::setAlwaysOnTopEvent( QtCollider::SetAlwaysOnTopEvent *e )
{
  QWidget *w = widget();
  if( !w ) return;

  Qt::WindowFlags flags = w->windowFlags();
  if( flags & Qt::Window ) {
    if( e->alwaysOnTop ) flags |= Qt::WindowStaysOnTopHint;
    else flags &= ~Qt::WindowStaysOnTopHint;

    // record the initial state to restore it later
    QPoint pos = w->pos();
    bool visible = w->isVisible();

    w->setWindowFlags( flags );

    // setting window flags will move the window to (0,0) and hide it,
    // so restore the initial state
    w->move(pos);
    if( visible ) w->show();
  }
}

void QWidgetProxy::startDragEvent( StartDragEvent* e )
{
  QWidget *w = widget();
  if( !w ) return;

  QFont f;
  const QString & label = e->label;
  QFontMetrics fm( f );
  QSize size = fm.size( 0, label ) + QSize(8,4);

  QPixmap pix( size );
  QPainter p( &pix );
  p.setBrush( QColor(255,255,255) );
  QRect r( pix.rect() );
  p.drawRect(r.adjusted(0,0,-1,-1));
  p.drawText( r, Qt::AlignCenter, label );
  p.end();

  QMimeData *mime = e->data;
  e->data = 0; // prevent deleting the data when event destroyed;

  QDrag *drag = new QDrag(w);
  drag->setMimeData( mime );
  drag->setPixmap( pix );
  drag->setHotSpot( QPoint( 0, + r.height() + 2 ) );
  drag->exec();
}

bool QWidgetProxy::filterEvent( QObject *o, QEvent *e, EventHandlerData &eh, QList<QVariant> & args )
{
  // NOTE We assume that qObject need not be checked here, as we wouldn't get events if
  // it wasn't existing

  int type = e->type();

  eh = eventHandlers().value( type );
  if( eh.type != type ) return false;

  switch( type ) {

    case QEvent::KeyPress:
      return ((_globalEventMask & KeyPress) || eh.enabled)
        && interpretKeyEvent( o, e, args );

    case QEvent::KeyRelease:
      return ((_globalEventMask & KeyRelease) || eh.enabled)
        && interpretKeyEvent( o, e, args );

    case QEvent::MouseButtonPress:
    case QEvent::MouseMove:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseButtonDblClick:
    case QEvent::Enter:
    case QEvent::Leave:
      return eh.enabled && interpretMouseEvent( o, e, args );

    case QEvent::Wheel:
      return interpretMouseWheelEvent( o, e, args );

    case QEvent::DragEnter:
    case QEvent::DragMove:
    case QEvent::Drop:
      return eh.enabled && interpretDragEvent( o, e, args );

    default:
      return eh.enabled;

  }
}

bool QWidgetProxy::interpretMouseEvent( QObject *o, QEvent *e, QList<QVariant> &args )
{
  if( o != _mouseEventWidget || !_mouseEventWidget->isEnabled() ) return false;

  QWidget *w = widget();

  if( e->type() == QEvent::Enter || e->type() == QEvent::Leave ) {
    QPoint pos = QCursor::pos();


    if( w ) pos = w->mapFromGlobal( pos );

    args << pos.x();
    args << pos.y();
    return true;
  }

  QMouseEvent *mouse = static_cast<QMouseEvent*>( e );
  QPoint pt = ( _mouseEventWidget == w ?
                mouse->pos() :
                _mouseEventWidget->mapTo( w, mouse->pos() ) );
  args << pt.x();
  args << pt.y();

  args << (int) mouse->modifiers();

  if( e->type() == QEvent::MouseMove ) return true;

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

  return true;
}

bool QWidgetProxy::interpretMouseWheelEvent( QObject *o, QEvent *e, QList<QVariant> &args )
{
  // NOTE: There seems to be a bug in wheel event propagation:
  // the event is propagated to parent twice!
  // Therefore we do not let the propagated events through to SC,
  // (we only let the "spontaneous" ones).

  if( o != _mouseEventWidget || !e->spontaneous() || !_mouseEventWidget->isEnabled() ) return false;

  QWheelEvent *we = static_cast<QWheelEvent*>(e);

  QWidget *w = widget();
  QPoint pt = _mouseEventWidget == w ?
              we->pos() :
              _mouseEventWidget->mapTo( w, we->pos() );
  Qt::Orientation ort = we->orientation();
  // calculate degrees: delta is in 1/8 of a degree.
  int deg = we->delta() / 8;

  args << pt.x();
  args << pt.y();
  args << (int) we->modifiers();
  args << (ort == Qt::Horizontal ? deg : 0);
  args << (ort == Qt::Vertical ? deg : 0);

  return true;
}

bool QWidgetProxy::interpretKeyEvent( QObject *o, QEvent *e, QList<QVariant> &args )
{
  if( o != _keyEventWidget || !_keyEventWidget->isEnabled() ) return false;

  QKeyEvent *ke = static_cast<QKeyEvent*>( e );

  QString text = ke->text();
  int unicode = ( text.count() == 1 ? text[0].unicode() : 0 );

  args << text;
  args << (int) ke->modifiers();
  args << unicode;
  args << ke->key();
  args << ke->spontaneous();

  return true;
}

bool QWidgetProxy::interpretDragEvent( QObject *o, QEvent *e, QList<QVariant> &args )
{
  if( o != _mouseEventWidget ) return false;

  QDropEvent *dnd = static_cast<QDropEvent*>(e);

  const QMimeData *data = dnd->mimeData();
  if ( !data->hasFormat( "application/supercollider" ) )
    return false;

  if( dnd->type() != QEvent::DragEnter ) {
    QPoint pos = dnd->pos();
    args << pos.x() << pos.y();
  }

  return true;
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
    invokeScMethod( s_doDrawFunc, QList<QVariant>(), 0, true );
    QtCollider::endPainting();
  }

  QtCollider::unlockLang();
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
