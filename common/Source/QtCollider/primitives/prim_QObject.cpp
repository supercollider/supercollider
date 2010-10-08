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

#include "PrimitiveDefiner.h"
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

int QObject_Finalize( struct VMGlobals *, struct PyrObject * );

struct CreationData {
  QString scClassName;
  PyrObject * scObject;
  QVariant arguments;
};

struct CreationEvent : public QcSyncEvent
{
  CreationEvent( const CreationData& data, QObjectProxy** ret )
  : QcSyncEvent( QcSyncEvent::CreateQObject ),
    _data ( data ),
    _ret ( ret )
  {}
  const CreationData & _data;
  QObjectProxy **_ret;
};

void qcCreateQObject( QcSyncEvent *e )
{
  CreationEvent *ce = static_cast<CreationEvent*>(e);

  CreateFn createFn = factoryFunction( ce->_data.scClassName );

  if( !createFn ) {
    qscErrorMsg( "QObject Factory for %s not found!\n",
                  ce->_data.scClassName.toStdString().c_str() );
    return;
  }

  *ce->_ret = (*createFn)( ce->_data.scObject, ce->_data.arguments );
}

int QObject_New (struct VMGlobals *g, int)
{
  PyrSlot *args = g->sp - 2;

  QString realClassName =
    Slot::toString( &slotRawObject( args )->classptr->name );
  qscDebugMsg( "CREATE: %s\n", realClassName.toStdString().c_str() );

  CreationData data;

  data.scObject = slotRawObject( args );
  data.scClassName = Slot::toString( args+1 );
  data.arguments = Slot::toVariant( args+2 );

  QObjectProxy *proxy = 0;

  CreationEvent *event = new CreationEvent( data, &proxy );
  QcApplication::postSyncEvent( event, &qcCreateQObject );

  if( !proxy ) return errFailed;

  SetPtr( slotRawObject(args)->slots, proxy );

  InstallFinalizer( g, slotRawObject( args ), 1, QObject_Finalize );

  return errNone;
}

int QObject_Destroy (struct VMGlobals *g, int)
{
  QString realClassName =
    Slot::toString( &slotRawObject( g->sp )->classptr->name );
  qscDebugMsg( "DESTROY: %s\n", realClassName.toStdString().c_str() );

  if( IS_OBJECT_NIL( g->sp ) ) return errFailed;

  QObjectProxy *proxy = QOBJECT_FROM_SLOT( g->sp );

  proxy->destroy();

  /* NOTE destroy has set QObjectProxy::scObject to 0, so we have to invalidate
    the SC object ourselves */
  SetNil( slotRawObject( g->sp )->slots );

  return errNone;
}

int QObject_Finalize( struct VMGlobals *, struct PyrObject *obj )
{
  QString realClassName =
    Slot::toString( &obj->classptr->name );
  qscDebugMsg("FINALIZE: %s\n", realClassName.toStdString().c_str() );

  if( IsNil( obj->slots ) ) return errNone;

  QObjectProxy *proxy = (QObjectProxy*) slotRawPtr( obj->slots );

  proxy->destroy();

  return errNone;
}

int QObject_SetProperty(struct VMGlobals *g, int)
{
  PyrSlot *args = g->sp - 3;

  if( IS_OBJECT_NIL( args ) ) return errFailed;

  QObjectProxy *proxy = QOBJECT_FROM_SLOT( args );

  PyrSymbol *symProp = slotRawSymbol( args+1 );
  PyrSlot *slotVal = args+2;
  bool direct = IsTrue( args+3 );

  return proxy->setProperty( symProp->name, slotVal, direct );
}

int QObject_GetProperty(struct VMGlobals *g, int)
{
  PyrSlot *args = g->sp - 2;

  if( IS_OBJECT_NIL( args ) ) return errFailed;

  QObjectProxy *proxy = QOBJECT_FROM_SLOT( args );

  PyrSymbol *symProp = slotRawSymbol( args+1 );
  PyrSlot *slotRetExtra = args+2;

  return proxy->getProperty( symProp->name, args, slotRetExtra );
}

int QObject_SetEventHandler (struct VMGlobals *g, int)
{
  PyrSlot *args = g->sp - 3;

  if( IS_OBJECT_NIL( args ) ) return errFailed;

  if( NotInt( args+1 ) || NotSym( args+2 ) ) return errWrongType;
  int eventType = Slot::toInt( args+1 );
  PyrSymbol *method = 0;
  slotSymbolVal( args+2, &method );
  bool direct = IsTrue( args+3 );

  QObjectProxy *proxy = QOBJECT_FROM_SLOT( args );

  proxy->setEventHandler( eventType, method, direct );

  return errNone;
}

int QObject_InvokeMethod (struct VMGlobals *g, int)
{
  PyrSlot *args = g->sp - 2;

  if( IS_OBJECT_NIL( args ) ) return errFailed;

  if( !IsSym( args+1 ) ) return errWrongType;

  const char *method = slotRawSymbol( args+1 )->name;

  QObjectProxy *proxy = QOBJECT_FROM_SLOT( args );
  if( !proxy->invokeMethod( method, args+2 ) ) return errFailed;

  return errNone;
}

void defineQObjectPrimitives()
{
  qscDebugMsg( "defining QObject primitives\n" );

  s_QObject = getsym("QObject");

  QtCollider::PrimitiveDefiner d;
  d.define( "_QObject_New", QObject_New, 3, 0 );
  d.define( "_QObject_Destroy", QObject_Destroy, 1, 0 );
  d.define( "_QObject_SetProperty",QObject_SetProperty, 4, 0 );
  d.define( "_QObject_GetProperty", QObject_GetProperty, 3, 0 );
  d.define( "_QObject_SetEventHandler", QObject_SetEventHandler, 4, 0 );
  d.define( "_QObject_InvokeMethod", QObject_InvokeMethod, 3, 0);
}
