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

#include "primitives.h"
#include "../QObjectProxy.h"
#include "../QcObjectFactory.h"
#include "../QcApplication.h"
#include "../Common.h"
#include "../Slot.h"

#include <PyrObject.h>
#include <PyrKernel.h>
#include <GC.h>

PyrSymbol *s_QObject;

#define IS_OBJECT_NIL( a ) \
  IsNil( slotRawObject(a)->slots )

#define QOBJECT_FROM_SLOT( s ) \
  ((QObjectProxy*) slotRawPtr( slotRawObject( s )->slots ))

#define CLASS_NAME( slot ) \
  slotRawSymbol( &slotRawObject( slot )->classptr->name )->name

using namespace QtCollider;

int QObject_Finalize( struct VMGlobals *, struct PyrObject * );

struct CreationEvent : public QcSyncEvent
{
  CreationEvent()
  : QcSyncEvent( QcSyncEvent::CreateQObject ) {}
  PyrObject * scObject;
  QString qtClassName;
  QVariant arguments;
  QObjectProxy **proxy;
};

void qcCreateQObject( QcSyncEvent *e )
{
  CreationEvent *ce = static_cast<CreationEvent*>(e);

  QcAbstractFactory *f = QtCollider::factories().value( ce->qtClassName );

  if( !f ) {
    qcErrorMsg( QString("Factory for class '%1' not found!").arg(ce->qtClassName) );
    return;
  }

  QVariant var( ce->arguments );
  if( var.userType() == qMetaTypeId<VariantList>() ) {
    VariantList args = var.value<VariantList>();
    *ce->proxy = f->newInstance( ce->scObject, args.data );
  }
  else {
    QList<QVariant> args;
    if( var.isValid() ) args << var;
    *ce->proxy = f->newInstance( ce->scObject, args );
  }
}

QC_LANG_PRIMITIVE( QObject_New, 2, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  PyrObject *scObject = slotRawObject( r );

  QObjectProxy *proxy = 0;

  CreationEvent *e = new CreationEvent();
  e->scObject = scObject;
  e->qtClassName = Slot::toString( a+0 );
  e->arguments = Slot::toVariant( a+1 );
  e->proxy = &proxy;

  qcSCObjectDebugMsg( 1, scObject, QString("CREATE: %2").arg(e->qtClassName) );

  QcApplication::postSyncEvent( e, &qcCreateQObject );

  if( !proxy ) return errFailed;

  SetPtr( scObject->slots, proxy );

  InstallFinalizer( g, scObject, 1, QObject_Finalize );

  return errNone;
}

QC_LANG_PRIMITIVE( QObject_Destroy, 0, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  qcSCObjectDebugMsg( 1, slotRawObject(r), "DESTROY" );

  QObjectProxy *proxy = QOBJECT_FROM_SLOT( r );

  DestroyEvent *e = new DestroyEvent( QObjectProxy::DestroyObject );
  bool ok = e->send( proxy, Synchronous );
  if( !ok ) qcSCObjectDebugMsg( 1, slotRawObject(r), "Already destroyed\n" );

  return errNone;
}

int QObject_Finalize( struct VMGlobals *, struct PyrObject *obj )
{
  qcSCObjectDebugMsg( 1, obj, "FINALIZE" );

  QObjectProxy *proxy = (QObjectProxy*) slotRawPtr( obj->slots );

  DestroyEvent *e = new DestroyEvent( QObjectProxy::DestroyProxyAndObject );
  e->send( proxy, Synchronous );

  return errNone;
}

QC_LANG_PRIMITIVE( QObject_SetParent, 1, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  QObjectProxy *proxy = QOBJECT_FROM_SLOT( r );

  QObjectProxy *parent = Slot::toObjectProxy( a );
  if( !parent ) return errWrongType;

  qcSCObjectDebugMsg( 1, slotRawObject(r), "SET PARENT" );

  QtCollider::SetParentEvent *e = new QtCollider::SetParentEvent();
  e->parent = parent;
  bool ok = e->send( proxy, Synchronous );

  return ok ? errNone : errFailed;
}

QC_LANG_PRIMITIVE( QObject_SetProperty, 3, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  QObjectProxy *proxy = QOBJECT_FROM_SLOT( r );

  if( NotSym( a ) ) return errWrongType;
  PyrSymbol *property = slotRawSymbol( a );
  bool sync = IsTrue( a+2 );

  qcSCObjectDebugMsg( 1, slotRawObject(r), QString("SET: %1").arg(property->name) );

  SetPropertyEvent *e = new SetPropertyEvent();
  e->property = property;
  e->value = Slot::toVariant( a+1 );

  e->send( proxy, sync ? Synchronous : Asynchronous );

  return errNone;
}

QC_LANG_PRIMITIVE( QObject_GetProperty, 2, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  QObjectProxy *proxy = QOBJECT_FROM_SLOT( r );

  if( NotSym(a) ) return errWrongType;
  PyrSymbol *property = slotRawSymbol( a );
  PyrSlot *slotRetExtra = a+1;
  QVariant val;

  qcSCObjectDebugMsg( 1, slotRawObject(r), QString("GET: %1").arg(property->name) );

  GetPropertyEvent *e = new GetPropertyEvent( property, val );

  bool ok = e->send( proxy, Synchronous );
  if( !ok ) return errFailed;

  bool haveExtra = NotNil( slotRetExtra );
  int err = Slot::setVariant( ( haveExtra ? slotRetExtra : r ), val );
  if( err ) return err;

  if( haveExtra ) slotCopy( r, slotRetExtra );

  return errNone;
}

QC_LANG_PRIMITIVE( QObject_SetEventHandler, 3, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  QObjectProxy *proxy = QOBJECT_FROM_SLOT( r );

  if( NotInt( a+0 ) || NotSym( a+1 ) ) return errWrongType;
  int eventType = Slot::toInt( a+0 );
  PyrSymbol *method = 0; slotSymbolVal( a+1, &method );
  Synchronicity sync = IsTrue( a+2 ) ? Synchronous : Asynchronous;

  qcSCObjectDebugMsg( 1, slotRawObject(r),
                      QString("EVENT HANDLER: type %1 -> %2 [%3]")
                      .arg(eventType).arg(method->name)
                      .arg(sync == Synchronous ? "SYNC" : "ASYNC") );

  SetEventHandlerEvent *e = new SetEventHandlerEvent( eventType, method, sync );

  e->send( proxy, Asynchronous );

  return errNone;
}

QC_LANG_PRIMITIVE( QObject_ConnectMethod, 3, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  QString signal = Slot::toString( a+0 );
  if( signal.isEmpty() || NotSym( a+1 ) ) return errWrongType;
  PyrSymbol *handler = 0; slotSymbolVal( a+1, &handler );
  Synchronicity sync = Slot::toBool( a+2 ) ? Synchronous : Asynchronous;

  qcSCObjectDebugMsg( 1, slotRawObject(r),
                      QString("SET SIGNAL HANDLER: %1 -> %2 [%3]").arg(signal).arg(handler->name)
                      .arg(sync == Synchronous ? "SYNC" : "ASYNC") );

  ConnectEvent *e = new ConnectEvent();
  e->method = handler;
  e->function = 0;
  e->signal = signal;
  e->sync = sync;

  QObjectProxy *proxy = QOBJECT_FROM_SLOT( r );
  e->send( proxy, Asynchronous );

  return errNone;
}

QC_LANG_PRIMITIVE( QObject_DisconnectMethod, 2, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  QString signal = Slot::toString( a+0 );
  if( signal.isEmpty() || NotSym( a+1 ) ) return errWrongType;
  PyrSymbol *handler = 0; slotSymbolVal( a+1, &handler );

  qcSCObjectDebugMsg( 1, slotRawObject(r),
                      QString("DISCONNECT METHOD: %1 -> %2").arg(signal).arg(handler->name) );

  DisconnectEvent *e = new DisconnectEvent();
  e->method = handler;
  e->function = 0;
  e->signal = signal;

  QObjectProxy *proxy = QOBJECT_FROM_SLOT( r );
  e->send( proxy, Asynchronous );

  return errNone;
}

QC_LANG_PRIMITIVE( QObject_ConnectFunction, 3, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  QString signal = Slot::toString( a+0 );
  if( signal.isEmpty() || NotObj( a+1 ) ) return errWrongType;
  PyrObject *handlerObj = slotRawObject( a+1 );
  Synchronicity sync = Slot::toBool( a+2 ) ? Synchronous : Asynchronous;

  qcSCObjectDebugMsg( 1, slotRawObject(r),
                      QString("SET SIGNAL HANDLER: %1 -> a Function [%2]").arg(signal)
                      .arg(sync == Synchronous ? "SYNC" : "ASYNC") );

  ConnectEvent *e = new ConnectEvent();
  e->method = 0;
  e->function = handlerObj;
  e->signal = signal;
  e->sync = sync;

  QObjectProxy *proxy = QOBJECT_FROM_SLOT( r );
  e->send( proxy, Asynchronous );

  return errNone;
}

QC_LANG_PRIMITIVE( QObject_DisconnectFunction, 2, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  QString signal = Slot::toString( a+0 );
  if( signal.isEmpty() || NotObj( a+1 ) ) return errWrongType;
  PyrObject *handlerObj = slotRawObject( a+1 );

  qcSCObjectDebugMsg( 1, slotRawObject(r),
                      QString("DISCONNECT FUNCTION: %1").arg(signal) );

  DisconnectEvent *e = new DisconnectEvent();
  e->method = 0;
  e->function = handlerObj;
  e->signal = signal;

  QObjectProxy *proxy = QOBJECT_FROM_SLOT( r );
  // NOTE Has to be synchronous, because the function might be deleted by the time the signal
  // is sent
  e->send( proxy, Synchronous );

  return errNone;
}

QC_LANG_PRIMITIVE( QObject_ConnectSlot, 3, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  // Args: signal, receiver, slot
  if( !isKindOfSlot( a+1, getsym("QObject")->u.classobj )
      || NotSym( a+0 ) || NotSym( a+2 ) ) return errWrongType;

  PyrSymbol *symSig = slotRawSymbol( a+0 );
  PyrSymbol *symSlot = slotRawSymbol( a+2 );
  QObjectProxy *sndProxy = QOBJECT_FROM_SLOT( r );
  QObjectProxy *rcvProxy = QOBJECT_FROM_SLOT( a+1 );

  qcSCObjectDebugMsg( 1, slotRawObject(r),
                      QString("CONNECT TO SLOT: %1 -> %2").arg(symSig->name).arg(symSlot->name) );

  QString strSig = QString("2") + symSig->name;
  QString strSlot = QString("1") + symSlot->name;

  sndProxy->lock();
  rcvProxy->lock();
  bool ok;
  if( !sndProxy->object() || !rcvProxy->object() ) {
    ok = true;
  }
  else {
    ok = QObject::connect( sndProxy->object(), strSig.toStdString().c_str(),
                           rcvProxy->object(), strSlot.toStdString().c_str() );
  }
  sndProxy->unlock();
  rcvProxy->unlock();

  if (!ok)  {
    qcErrorMsg( QString("Failed to connect %1::%2 to %3::%4!\n")
                .arg(sndProxy->scClassName()).arg(symSig->name)
                .arg(rcvProxy->scClassName()).arg(symSlot->name)
              );
    return errFailed;
  }

  return errNone;
}

QC_LANG_PRIMITIVE( QObject_InvokeMethod, 3, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  if( NotSym( a+0 ) ) return errWrongType;

  PyrSymbol *method = slotRawSymbol( a+0 );
  PyrSlot *methodArgs = a+1;
  bool sync = !IsFalse( a+2 );

  qcSCObjectDebugMsg( 1, slotRawObject(r),
                      QString("INVOKE: '%1' [%2]").arg(method->name).arg(sync ? "SYNC" : "ASYNC") );

  QObjectProxy *proxy = QOBJECT_FROM_SLOT( r );

  if( sync ) {
    InvokeMethodEvent *e = new InvokeMethodEvent();
    e->method = method;
    e->ret = r;
    e->arg = methodArgs;

    bool ok = e->send( proxy, Synchronous );
    return ok ? errNone : errFailed;
  }
  else {
    proxy->invokeMethod( method->name, 0, methodArgs, Qt::QueuedConnection );
  }

  return errNone;
}

QC_LANG_PRIMITIVE( QObject_IsValid, 0, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  // NOTE Though we could check validity without posting an event (by locking the proxy),
  // we post a sync event to maximize the chance that a DeferredDelete event issued earlier
  // can get processed by the object in question before it is checked for validity.

  QObjectProxy *proxy = Slot::toObjectProxy( r );

  GetValidityEvent *e = new GetValidityEvent();
  bool valid = e->send( proxy, Synchronous );

  SetBool( r, valid );

  return errNone;
}

QC_LANG_PRIMITIVE( QObject_GetChildren, 1, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  if( NotSym(a) && NotNil(a) ) return errWrongType;

  QObjectProxy *proxy = Slot::toObjectProxy( r );
  PyrSymbol *className = IsSym( a ) ? slotRawSymbol( a ) : 0;

  qcSCObjectDebugMsg( 1, slotRawObject(r),
                      QString("GET CHILDREN: of class '%1'")
                      .arg( className ? className->name : "QObject" ) );

  QList<PyrObject*> children;

  GetChildrenEvent *e = new GetChildrenEvent( className, children );

  bool ok = e->send( proxy, Synchronous );

  if( ok ) {

    int count = children.count();
    PyrObject *array = newPyrArray( g->gc, count, 0, true );
    PyrSlot *s = array->slots;

    Q_FOREACH( PyrObject *obj, children ) {
      SetObject( s, obj );
      ++array->size;
      g->gc->GCWrite( array, s );
      ++s;
    }

    SetObject( r, array );

    return errNone;
  }

  return errFailed;
}

QC_LANG_PRIMITIVE( QObject_GetParent, 0, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  QObjectProxy *proxy = Slot::toObjectProxy( r );

  qcSCObjectDebugMsg( 1, slotRawObject(r), QString("GET PARENT") );

  GetParentEvent *e = new GetParentEvent();
  PyrObject *parent = 0;
  e->parent = &parent;

  bool ok = e->send( proxy, Synchronous );
  if( !ok ) return errFailed;

  if( parent ) SetObject( r, parent );
  else SetNil( r );

  return errNone;
}
