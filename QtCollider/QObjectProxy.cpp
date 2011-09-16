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

#include "QObjectProxy.h"
#include "QcApplication.h"
#include "Common.h"
#include "Slot.h"
#include "QcSignalSpy.h"

#include <QApplication>
#include <QWidget>
#include <QVarLengthArray>
#include <QThread>

#include <PyrKernel.h>
#include <VMGlobals.h>

using namespace QtCollider;

void interpretMouseEvent( QEvent *e, QList<QVariant> &args );
void interpretKeyEvent( QEvent *e, QList<QVariant> &args );

QObjectProxy::QObjectProxy( QObject *qObject_, PyrObject *scObject_ )
: qObject( qObject_ ),
  _scObject( scObject_ ),
  _scClassName( slotRawSymbol( &scObject_->classptr->name )->name )
{
  ProxyToken *token = new ProxyToken( this, qObject );
  connect( qObject, SIGNAL( destroyed( QObject* ) ), this, SLOT( invalidate() ) );
  qObject->installEventFilter( this );
}

QObjectProxy::~QObjectProxy()
{
  qcProxyDebugMsg( 1, QString("Proxy is being deleted.") );
}

bool QObjectProxy::compareThread() {
  return QThread::currentThread() == this->thread();
}

void QObjectProxy::invalidate() {
  qcProxyDebugMsg( 1, QString("Object has been deleted. Invalidating proxy.") );
  mutex.lock(); qObject = 0; mutex.unlock();
}

bool QObjectProxy::invokeMethod( const char *method, PyrSlot *retSlot, PyrSlot *argSlot,
                                 Qt::ConnectionType ctype )
{
  mutex.lock();
  if( !qObject ) {
    mutex.unlock();
    return true;
  }

  // the signature char array
  QVarLengthArray<char, 512> sig;

  // serialize method name
  int len = qstrlen( method );
  if( len <= 0 ) {
    mutex.unlock();
    return false;
  }
  sig.append( method, len );
  sig.append( '(' );

  // get data from argument slots
  QtCollider::Variant argSlots[10];

  if( isKindOfSlot( argSlot, class_Array ) ) {
    PyrSlot *slots = slotRawObject( argSlot )->slots;
    int size = slotRawObject( argSlot )->size;
    int i;
    for( i = 0; i<size && i<10; ++i ) {
      argSlots[i].setData( slots );
      ++slots;
    }
  }
  else argSlots[0].setData( argSlot );

  // serialize argument types
  int i;
  for( i = 0; i < 10; ++i ) {
    int type = argSlots[i].type();
    if( type == QMetaType::Void ) break;
    const char *typeName = QMetaType::typeName( type );
    int len = qstrlen( typeName );
    if( len <= 0 ) break;
    sig.append( typeName, len );
    sig.append( ',' );
  }

  // finalize the signature
  if( i==0 ) sig.append( ')' );
  else sig[sig.size() - 1] = ')';

  sig.append('\0');

  // get the meta method
  const QMetaObject *mo = qObject->metaObject();

  int mi = mo->indexOfMethod( sig.constData() );
  if( mi < 0 ) {
    QByteArray mnorm = QMetaObject::normalizedSignature( sig.constData() );
    mi = mo->indexOfMethod( mnorm.constData() );
  }

  if( mi < 0 || mi >= mo->methodCount()  ) {
    qcProxyDebugMsg( 1, QString("WARNING: No such method: %1::%2").arg( mo->className() )
                        .arg( sig.constData() ) );
    mutex.unlock();
    return false;
  }

  QMetaMethod mm = mo->method( mi );

  // construct the return data object
  QGenericReturnArgument retGArg;
  const char *retTypeName = mm.typeName();
  int retType = QMetaType::type( retTypeName );
  void *retPtr = 0;
  if( retSlot ) {
    retPtr = QMetaType::construct( retType );
    retGArg = QGenericReturnArgument( retTypeName, retPtr );
  }

  //do it!
  bool success =
    mm.invoke( qObject, ctype, retGArg,
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

  // store the return data into the return slot
  if( success && retPtr ) {
    QVariant retVar( retType, retPtr );
    Slot::setVariant( retSlot, retVar );
  };

  if( retPtr )
    QMetaType::destroy( retType, retPtr );

  mutex.unlock();
  return success;
}

void QObjectProxy::invokeScMethod
( PyrSymbol *method, const QList<QVariant> & args, PyrSlot *result,
  bool locked )
{
  qcProxyDebugMsg(1, QString("SC METHOD CALL [+++]: ") + QString(method->name) );

  if( !locked ) {
    QtCollider::lockLang();
  }

  if( _scObject ) {
    QtCollider::runLang( _scObject, method, args, result );
  }
  else {
    SetNil( result );
    qcDebugMsg(1, "WARNING: no SC object");
  }

  if( !locked ) QtCollider::unlockLang();

  qcProxyDebugMsg(1, QString("SC METHOD CALL [---]: ") + QString(method->name) );
}

void QObjectProxy::customEvent( QEvent *event )
{
  switch ( event->type() ) {
    case (QEvent::Type) QtCollider::Event_ScMethodCall:
      scMethodCallEvent( static_cast<ScMethodCallEvent*>(event) );
      return;
    case (QEvent::Type) QtCollider::Event_Proxy_SetProperty:
      setPropertyEvent( static_cast<SetPropertyEvent*>(event) );
      return;
    case (QEvent::Type) QtCollider::Event_Proxy_Destroy:
      destroyEvent( static_cast<DestroyEvent*>(event) );
      return;
    default: ;
  }
}

bool QObjectProxy::setParent( QObjectProxy *parentProxy ) {
  if( qObject && parentProxy->object() )
    qObject->setParent( parentProxy->object() );

  return true;
}

bool QObjectProxy::setProperty( const char *property, const QVariant & val )
{
  if( !qObject ) return true;
  if( !qObject->setProperty( property, val ) ) {
    qcProxyDebugMsg(1, QString("WARNING: Property '%1' not found. Setting dynamic property.")
                        .arg( property ) );
  }
  return false;
}

bool QObjectProxy::setPropertyEvent( SetPropertyEvent *e )
{
  return setProperty( e->property->name, e->value );
}

QVariant QObjectProxy::property( const char *property )
{
  return qObject ? qObject->property( property ) : QVariant();
}

bool QObjectProxy::setEventHandler( int type, PyrSymbol *method,
                                    QtCollider::Synchronicity sync, bool enable )
{
  EventHandlerData data;
  data.type = type;
  data.method = method;
  data.sync = sync;

  // if 'enable' is true, insert the new event handler enabled,
  // otherwise copy current state, or set disabled if none.
  if( enable ) {
    data.enabled = true;
  }
  else {
    EventHandlerData d = _eventHandlers.value( type );
    if( d.type != QEvent::None ) data.enabled = d.enabled;
    else data.enabled = false;
  }

  // NOTE: will replace if same key
  _eventHandlers.insert( type, data );

  return true;
}

bool QObjectProxy::setEventHandlerEnabled( int type, bool enabled )
{
  EventHandlerData d = _eventHandlers.value( type );
  if( d.type != type ) return false;

  if( d.enabled != enabled ) {
    d.enabled = enabled;
    _eventHandlers.insert( type, d );
  }

  return true;
}

bool QObjectProxy::connectObject( const char *signal, PyrObject *object,
                                    Qt::ConnectionType ctype )
{
  if( !qObject ) return true;

  QcFunctionSignalHandler *handler =
    new QcFunctionSignalHandler( this, signal, object, ctype );

  if( !handler->isValid() ) { delete handler; return false; }

  funcSigHandlers.append( handler );

  return true;
}

bool QObjectProxy::connectMethod( const char *signal, PyrSymbol *method,
                                  Qt::ConnectionType ctype )
{
  if( !qObject ) return true;

  QcMethodSignalHandler *handler =
    new QcMethodSignalHandler( this, signal, method, ctype );

  if( handler->isValid() ) {
    methodSigHandlers.append( handler );
    return true;
  }
  else {
    delete handler;
    return false;
  }
}

bool QObjectProxy::disconnectObject( const char *sig, PyrObject *object )
{
  if( !qObject ) return true;

  const QMetaObject *mo = qObject->metaObject();
  QByteArray signal = QMetaObject::normalizedSignature( sig );
  int sigId = mo->indexOfSignal( signal );
  if( sigId < 0 ) {
    qcDebugMsg( 1, QString("WARNING: No such signal: '%1'").arg(signal.constData()) );
    return false;
  }

  for( int i = 0; i < funcSigHandlers.size(); ++i ) {
    QcFunctionSignalHandler *h = funcSigHandlers[i];
    if( h->indexOfSignal() == sigId && h->function() == object ) {
      funcSigHandlers.removeAt(i);
      delete h;
      return true;
    }
  }

  return false;
}

bool QObjectProxy::disconnectMethod( const char *sig, PyrSymbol *method)
{
  if( !qObject ) return true;

  const QMetaObject *mo = qObject->metaObject();
  QByteArray signal = QMetaObject::normalizedSignature( sig );
  int sigId = mo->indexOfSignal( signal );
  if( sigId < 0 ) {
    qcDebugMsg( 1, QString("WARNING: No such signal: '%1'").arg(signal.constData()) );
    return false;
  }

  for( int i = 0; i < methodSigHandlers.size(); ++i ) {
    QcMethodSignalHandler *h = methodSigHandlers[i];
    if( h->indexOfSignal() == sigId && h->method() == method ) {
      methodSigHandlers.removeAt(i);
      delete h;
      return true;
    }
  }

  return false;
}

void QObjectProxy::destroy( DestroyAction action )
{
  switch( action ) {
    case DestroyObject:
      delete qObject;
      return;
    case  DestroyProxy:
      delete this;
      return;
    case DestroyProxyAndObject:
      delete qObject;
      delete this;
      return;
  }
}

bool QObjectProxy::destroyEvent( DestroyEvent *e )
{
  destroy( e->action() );
  return true;
}

QList<PyrObject*> QObjectProxy::children( PyrSymbol *className )
{
  QList<PyrObject*> scChildren;

  if( !qObject ) return scChildren;

  const QObjectList &children = qObject->children();

  Q_FOREACH( QObject *child, children ) {

    QObjectProxy *proxy = QObjectProxy::fromObject( child );
    if( !proxy ) continue;

    PyrObject * obj = proxy->_scObject;

    if( obj ) {
        if( className && !isKindOf( obj, className->u.classobj ) )
            continue;
        scChildren.append( obj );
    }
  }

  return scChildren;
}

PyrObject *QObjectProxy::parent( PyrSymbol *className )
{
  if( !qObject ) return 0;

  QObject *parent = qObject->parent();

  while( parent ) {
      // see if this parent has a corresponding proxy
      QObjectProxy *proxy = QObjectProxy::fromObject( parent );
      if( proxy ) {
          // if parent does not have a corresponding SC object (it is just
          // being deleted) return no parent;
          PyrObject *scobj = proxy->_scObject;
          if( !scobj ) return 0;

          // if parent SC object is of desired class (or no class specified)
          // return it, else continue
          if( !className || isKindOf( scobj, className->u.classobj ) ) {
              return scobj;
          }
      }

      // if this parent was not appropriate continue to consider the parent's parent
      parent = parent->parent();
  }

  return 0;
}

bool QObjectProxy::eventFilter( QObject * watched, QEvent * event )
{
  int type = event->type();

  EventHandlerData eh;
  QList<QVariant> args;

  if( !filterEvent( watched, event, eh, args ) ) {
    qcProxyDebugMsg(3,QString("Event (%1, %2) not handled, forwarding to the widget")
      .arg(type)
      .arg(event->spontaneous() ? "spontaneous" : "inspontaneous") );
    return false;
  }

  qcProxyDebugMsg(1,QString("Will handle event (%1, %2) -> (%3, %4)")
    .arg(type)
    .arg(event->spontaneous() ? "spontaneous" : "inspontaneous")
    .arg(eh.method->name)
    .arg(eh.sync == Synchronous ? "sync" : "async") );

  return invokeEventHandler( event, eh, args );
}

bool QObjectProxy::invokeEventHandler( QEvent *event, EventHandlerData &eh, QList<QVariant> & args )
{
  PyrSymbol *method = eh.method;

  if( eh.sync == Synchronous ) {
    PyrSlot result;
    invokeScMethod( method, args, &result );
    if( IsTrue( &result ) ) {
      qcProxyDebugMsg(2,"Event accepted");
      event->accept();
      return true;
    }
    else if( IsFalse( &result ) ) {
      qcProxyDebugMsg(2,"Event ignored");
      event->ignore();
      return true;
    }
  }
  else {
    ScMethodCallEvent *e = new ScMethodCallEvent( method, args );
    QApplication::postEvent( this, e );
  }

  qcProxyDebugMsg(2,"Forwarding event to the system");
  return false;
}

bool QObjectProxy::filterEvent( QObject *, QEvent *e, EventHandlerData & eh, QList<QVariant> & args )
{
  int type = e->type();
  eh = _eventHandlers.value( type, EventHandlerData() );
  return ( eh.type == type ) && eh.enabled;
}

inline void QObjectProxy::scMethodCallEvent( ScMethodCallEvent *e )
{
  invokeScMethod( e->method, e->args, 0, e->locked );
}

QObjectProxy * QObjectProxy::fromObject( QObject *object )
{
  const QObjectList &children = object->children();
  Q_FOREACH( QObject *child, children ) {
    ProxyToken *token = qobject_cast<QtCollider::ProxyToken*>( child );
    if( token ) return token->proxy;
  }
  return 0;
}
