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

#include <QApplication>
#include <QWidget>

#include <PyrKernel.h>
#include <VMGlobals.h>

void interpretMouseEvent( QEvent *e, QList<QVariant> &args );
void interpretKeyEvent( QEvent *e, QList<QVariant> &args );

QObjectProxy::QObjectProxy( QObject *qObject_, PyrObject *scObject_ )
: QObject( qObject_ ), qObject( qObject_ ), scObject( scObject_ )
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

void QObjectProxy::destroy()
{
  syncRequest( Destroy );
}

void QObjectProxy::destroyProxyOnly()
{
  syncRequest( DestroyProxy );
}

int QObjectProxy::setProperty( const char *property, PyrSlot *arg, bool direct )
{
  qscDebugMsg("SET: %s\n", property);

  PropertyData p;
  p.name = property;
  p.value = Slot::toVariant( arg );

  QVariant data = QVariant::fromValue<PropertyData>(p);

  if( direct )
    syncRequest( SetProperty, data );
  else
    asyncRequest( SetProperty, data );

  return errNone;
}

int QObjectProxy::getProperty( const char *property,
                               PyrSlot *ret, PyrSlot *retExtra )
{
  qscDebugMsg("GET: %s\n", property);

  bool haveExtra = !IsNil( retExtra );
  PropertyData p;
  p.name = property;
  p.slot = haveExtra ? retExtra : ret;

  QVariant err( errNone );

  syncRequest( GetProperty, QVariant::fromValue<PropertyData>( p ), &err );

  if( haveExtra ) slotCopy( ret, retExtra );

  qscDebugMsg("GOT: %s\n", property);
  return err.toInt();
}

void QObjectProxy::setEventHandler( int eventType, PyrSymbol *method,
                                    bool direct )
{
  qscDebugMsg( "Setting event handler: event: %i / method: %s\n", eventType, method->name );
  EventHandlerData data;
  data.type = eventType;
  data.method = method;
  data.direct = direct;
  switch( eventType ) {
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

  asyncRequest( SetEventHandler,
                QVariant::fromValue<EventHandlerData>( data ) );
}

bool QObjectProxy::invokeMethod( const char *method, PyrSlot *arg )
{
  qscDebugMsg("invoking method '%s'\n", method );

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
    QMetaObject::invokeMethod( qObject, method, Qt::QueuedConnection,
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

  if( !success )
  {
    success =
      QMetaObject::invokeMethod( this, method, Qt::QueuedConnection,
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
  }

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

void QObjectProxy::syncRequest( int type, const QVariant& data, QVariant *ret )
{
  QcGenericEvent *event = new QcGenericEvent( type, data, ret );
  QcApplication::postSyncEvent( event, this );
}

void QObjectProxy::asyncRequest( int type, const QVariant& data, QVariant *ret )
{
  QApplication::postEvent( this, new QcGenericEvent( type, data, ret ) );
}


void QObjectProxy::customEvent( QEvent *event )
{
  if( event->type() == (QEvent::Type) QtCollider::Event_ScMethodCall ) {
    scMethodCallEvent( static_cast<ScMethodCallEvent*>( event ) );
    return;
  }

  if( event->type() != (QEvent::Type) QtCollider::Event_Sync ) return;

  QcSyncEvent *se = static_cast<QcSyncEvent*>( event );
  if( se->syncEventType() != QcSyncEvent::Generic ) return;

  QcGenericEvent *e = static_cast<QcGenericEvent*>( se );
  PropertyData p;
  EventHandlerData eh;

  switch ( e->genericEventType() ) {
    case SetProperty:
      p = e->_data.value<PropertyData>();
      doSetProperty( p.name, p.value );
      break;
    case GetProperty:
      p = e->_data.value<PropertyData>();
      *e->_return = QVariant( doGetProperty( p.name, p.slot ) );
      break;
    case SetEventHandler:
      eh = e->_data.value<EventHandlerData>();
      doSetEventHandler( eh );
      break;
    case Destroy:
      scObject = 0;
      qObject->deleteLater();
      break;
    case DestroyProxy:
      scObject = 0;
      deleteLater();
      break;
    default:
      qscErrorMsg("Unhandled custom event\n");
  }

}

void QObjectProxy::doSetProperty( const QString& property, const QVariant& arg )
{
  if( !qObject->setProperty( property.toStdString().c_str(), arg ) ) {
    qscDebugMsg("WARNING: setting dynamic property\n");
  }
}

int QObjectProxy::doGetProperty( const QString &property, PyrSlot *slot )
{
  int err = errNone;
  QVariant val = qObject->property( property.toStdString().c_str() );
  err = Slot::setVariant( slot, val );

  return err;
}

void QObjectProxy::doSetEventHandler( const EventHandlerData & data )
{
  eventHandlers.insert( data.type, data );
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

      if( eh.direct ) {
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
