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

  if( IS_OBJECT_NIL( r ) ) return errFailed;

  QObjectProxy *proxy = QOBJECT_FROM_SLOT( r );

  DestroyEvent *e = new DestroyEvent( QObjectProxy::DestroyProxyAndObject );
  e->send( proxy, Synchronous );

  /* NOTE destroy has set QObjectProxy::scObject to 0, so we have to invalidate
    the SC object ourselves */
  SetNil( slotRawObject( r )->slots );

  return errNone;
}

int QObject_Finalize( struct VMGlobals *, struct PyrObject *obj )
{
  qcSCObjectDebugMsg( 1, obj, "FINALIZE" );

  if( IsNil( obj->slots ) ) return errNone;

  QObjectProxy *proxy = (QObjectProxy*) slotRawPtr( obj->slots );

  DestroyEvent *e = new DestroyEvent( QObjectProxy::DestroyProxyAndObject );
  e->send( proxy, Synchronous );

  return errNone;
}

QC_LANG_PRIMITIVE( QObject_SetParent, 1, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  if( IS_OBJECT_NIL( r ) ) return errFailed;
  QObjectProxy *proxy = QOBJECT_FROM_SLOT( r );

  QObject *parent = Slot::toObject( a );
  if( !parent ) return errWrongType;

  qcSCObjectDebugMsg( 1, slotRawObject(r), "SET PARENT" );

  QtCollider::SetParentEvent *e = new QtCollider::SetParentEvent();
  e->parent = parent;
  bool ok = e->send( proxy, Synchronous );

  return ok ? errNone : errFailed;
}

QC_LANG_PRIMITIVE( QObject_SetProperty, 3, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  if( IS_OBJECT_NIL( r ) ) return errFailed;
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
  if( IS_OBJECT_NIL( r ) ) return errFailed;
  QObjectProxy *proxy = QOBJECT_FROM_SLOT( r );

  if( NotSym(a) ) return errWrongType;
  PyrSymbol *property = slotRawSymbol( a );
  PyrSlot *slotRetExtra = a+1;
  QVariant val;

  qcSCObjectDebugMsg( 1, slotRawObject(r), QString("GET: %1").arg(property->name) );

  GetPropertyEvent *e = new GetPropertyEvent( property, val );

  e->send( proxy, Synchronous );

  bool haveExtra = !IsNil( slotRetExtra );
  int err = Slot::setVariant( ( haveExtra ? slotRetExtra : r ), val );
  if( err ) return err;

  if( haveExtra ) slotCopy( r, slotRetExtra );

  return errNone;
}

QC_LANG_PRIMITIVE( QObject_SetEventHandler, 3, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  if( IS_OBJECT_NIL( r ) ) return errFailed;
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

QC_LANG_PRIMITIVE( QObject_Connect, 3, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  if( IS_OBJECT_NIL( r ) ) return errFailed;

  QString signal = Slot::toString( a+0 );
  if( signal.isEmpty() || NotSym( a+1 ) ) return errWrongType;
  PyrSymbol *handler = 0; slotSymbolVal( a+1, &handler );
  Synchronicity sync = Slot::toBool( a+2 ) ? Synchronous : Asynchronous;

  qcDebugMsg( 1, QString("CONNECT: %1 -> %2 [%3]").arg(signal).arg(handler->name)
                  .arg(sync == Synchronous ? "SYNC" : "ASYNC") );

  ConnectEvent *e = new ConnectEvent();
  e->handler = handler;
  e->signal = signal;
  e->sync = sync;

  QObjectProxy *proxy = QOBJECT_FROM_SLOT( r );
  e->send( proxy, Asynchronous );

  return errNone;
}

QC_LANG_PRIMITIVE( QObject_InvokeMethod, 3, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  if( IS_OBJECT_NIL( r ) ) return errFailed;

  if( NotSym( a+0 ) ) return errWrongType;

  PyrSymbol *method = slotRawSymbol( a+0 );
  PyrSlot *methodArgs = a+1;
  bool sync = !IsFalse( a+2 );

  qcDebugMsg( 1, QString("INVOKE: '%1' [%2]").arg(method->name).arg(sync ? "SYNC" : "ASYNC") );

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
