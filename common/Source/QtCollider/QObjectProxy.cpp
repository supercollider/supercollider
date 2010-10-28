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

#include "QObjectProxy.h"
#include "QcApplication.h"
#include "Common.h"
#include "Slot.h"
#include "QcSignalSpy.h"

#include <QApplication>
#include <QWidget>

#include <PyrKernel.h>
#include <VMGlobals.h>

using namespace QtCollider;

void interpretMouseEvent( QEvent *e, QList<QVariant> &args );
void interpretKeyEvent( QEvent *e, QList<QVariant> &args );

QObjectProxy::QObjectProxy( QObject *qObject_, PyrObject *scObject_ )
: QObject( qObject_ ),
  qObject( qObject_ ),
  scObject( scObject_ ),
  sigSpy( new QcSignalSpy( this ) )
{
  qObject->installEventFilter( this );
}

QObjectProxy::~QObjectProxy()
{
  if( scObject ) {
    qscDebugMsg( "~QObjectProxy: invalidating SC object\n" );
    QtCollider::lockLang();
    SetNil( scObject->slots );
    QtCollider::unlockLang();
  }
  else {
    qscDebugMsg( "~QObjectProxy: SC object already detached\n" );
  }
}

bool QObjectProxy::invokeMethod( const char *method, PyrSlot *arg, Qt::ConnectionType ctype )
{
  Slot argSlots[10];

  if ( isKindOfSlot( arg, class_array ) ) {
    PyrSlot *slots = slotRawObject( arg )->slots;
    int size = slotRawObject( arg )->size;
    int i;
    for( i = 0; i<size && i<10; ++i ) {
      argSlots[i].setData( slots );
      ++slots;
    }
  }
  else argSlots[0].setData( arg );

  bool success =
    QMetaObject::invokeMethod( qObject, method, ctype,
                                argSlots[0].asGenericArgument(),
                                argSlots[1].asGenericArgument(),
                                argSlots[2].asGenericArgument(),
                                argSlots[3].asGenericArgument(),
                                argSlots[4].asGenericArgument(),
                                argSlots[5].asGenericArgument(),
                                argSlots[6].asGenericArgument(),
                                argSlots[7].asGenericArgument(),
                                argSlots[8].asGenericArgument(),
                                argSlots[9].asGenericArgument());

  return success;
}

void QObjectProxy::invokeScMethod
( PyrSymbol *method, const QList<QVariant> & args, PyrSlot *result,
  bool locked )
{
  qscDebugMsg("+++ QObjectProxy::invokeScMethod\n");

  if( !locked ) {
    QtCollider::lockLang();
  }

  if( scObject ) {
    VMGlobals *g = gMainVMGlobals;
    g->canCallOS = true;
    ++g->sp;  SetObject(g->sp, scObject);
    Q_FOREACH( QVariant var, args ) {
      ++g->sp;
      if( Slot::setVariant( g->sp, var ) )
        SetNil( g->sp );
    }
    runInterpreter(g, method, args.size() + 1);
    g->canCallOS = false;
    if (result) slotCopy(result, &g->result);
  }
  else {
    SetNil( result );
    qscDebugMsg("QObjectProxy: no SC object\n");
  }

  if( !locked ) QtCollider::unlockLang();

  qscDebugMsg("--- QObjectProxy::invokeScMethod\n");
}

void QObjectProxy::customEvent( QEvent *event )
{
  if( event->type() == (QEvent::Type) QtCollider::Event_ScMethodCall ) {
    scMethodCallEvent( static_cast<ScMethodCallEvent*>( event ) );
    return;
  }

  if( event->type() != (QEvent::Type) QtCollider::Event_Sync ) return;

  QcSyncEvent *se = static_cast<QcSyncEvent*>( event );

  if( se->syncEventType() == QcSyncEvent::ProxyRequest ) {
    QtCollider::RequestEvent *re =  static_cast<QtCollider::RequestEvent*>( event );
    re->execute( this );
    return;
  }
}

bool QObjectProxy::setParentEvent( SetParentEvent *e ) {
  qObject->setParent( e->parent );
  return true;
}

bool QObjectProxy::setPropertyEvent( SetPropertyEvent *e )
{
  if( !qObject->setProperty( e->property->name, e->value ) ) {
    qscDebugMsg("WARNING: setting dynamic property\n");
  }
}

bool QObjectProxy::getPropertyEvent( GetPropertyEvent *e )
{
  e->value = qObject->property( e->property->name );
  return true;
}

bool QObjectProxy::setEventHandlerEvent( SetEventHandlerEvent *e )
{
  EventHandlerData data;
  data.type = e->type;
  data.method = e->method;
  data.sync = e->sync;
  switch( e->type ) {
    case QEvent::MouseButtonPress:
    case QEvent::MouseMove:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseButtonDblClick:
    case QEvent::Enter:
      data.interpretFn = &QObjectProxy::interpretMouseEvent; break;
    case QEvent::KeyPress:
    case QEvent::KeyRelease:
      data.interpretFn = &QObjectProxy::interpretKeyEvent; break;
    default:
      data.interpretFn = 0;
  }
  eventHandlers.insert( data.type, data );
  return true;
}

bool QObjectProxy::connectEvent( ConnectEvent *e )
{
  bool direct = e->sync == Synchronous;
  return sigSpy->connect( e->signal.toStdString().c_str(), e->handler, direct );
}

bool QObjectProxy::invokeMethodEvent( InvokeMethodEvent *e )
{
  return invokeMethod( e->method->name, e->arg, Qt::DirectConnection );
}

bool QObjectProxy::destroyEvent( DestroyEvent *e )
{
  scObject = 0;

  if( e->action() == DestroyProxyAndObject )
    qObject->deleteLater();
  else
    deleteLater();

  return errNone;
}

bool QObjectProxy::eventFilter( QObject * watched, QEvent * event )
{
  int type = event->type();

  if( type == QtCollider::Event_ScMethodCall ) {
    ScMethodCallEvent* mce = static_cast<ScMethodCallEvent*>( event );
    qscDebugMsg("executing SC method: %s\n", mce->method->name );
    scMethodCallEvent( mce );
    return true;
  }
  else {
    EventHandlerData eh = eventHandlers.value( type, EventHandlerData() );
    if( eh.type == type ) {
      PyrSymbol *symMethod = eh.method;
      qscDebugMsg("catching event %i with handler '%s'\n",
                  type, symMethod->name );

      InterpretEventFn interpreter = eh.interpretFn;

      QList<QVariant> args;

      if( interpreter ) {
        qscDebugMsg("got interpreter\n");
        (this->*interpreter) ( event, args );
      }

      if( eh.sync == Synchronous ) {
        qscDebugMsg("direct!\n");
        PyrSlot result;
        invokeScMethod( symMethod, args, &result );
        if( IsNil( &result ) ) return false;
        else {
          if( IsFalse( &result ) ) event->ignore();
          return true;
        }
      }
      else {
        qscDebugMsg("indirect\n");
        ScMethodCallEvent *e = new ScMethodCallEvent( symMethod, args );
        QApplication::postEvent( this, e );
      }
    }
    return false;
  }
}


void QObjectProxy::scMethodCallEvent( ScMethodCallEvent *e )
{
  invokeScMethod( e->method, e->args, 0, e->locked );
}

#include <QMouseEvent>

void QObjectProxy::interpretMouseEvent( QEvent *e, QList<QVariant> &args )
{
  if( e->type() == QEvent::Enter ) {
    QPoint pos = QCursor::pos();

    QWidget *w = qobject_cast<QWidget*>( qObject );
    if( w ) pos = w->mapFromGlobal( pos );

    args << pos.x();
    args << pos.y();
    return;
  }

  QMouseEvent *mouse = static_cast<QMouseEvent*>( e );
  args << mouse->x();
  args << mouse->y();

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

void QObjectProxy::interpretKeyEvent( QEvent *e, QList<QVariant> &args )
{
  QKeyEvent *ke = static_cast<QKeyEvent*>( e );

  QString text = ke->text();
  int unicode = ( text.count() == 1 ? text[0].unicode() : 0 );

  args << text;
  args << (int) ke->modifiers();
  args << unicode;
  args << ke->key();
}

bool QtCollider::RequestEvent::send( QObjectProxy *proxy, Synchronicity sync )
{
  if( sync == Synchronous ) {
    bool done = false;
    p_done = &done;
    QcApplication::postSyncEvent( this, proxy );
    return done;
  }
  else {
    QApplication::postEvent( proxy, this );
  }

  // WARNING at this point, the event has been deleted, so "this" pointer and data members are
  // not valid anymore!

  return true;
}
