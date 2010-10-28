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

  QcAbstractFactory *f = QtCollider::factories().value( ce->_data.scClassName );

  if( !f ) {
    qscErrorMsg( "Factory for class '%s' not found!\n",
                  ce->_data.scClassName.toStdString().c_str() );
    return;
  }

  QVariant var( ce->_data.arguments );
  if( var.userType() == qMetaTypeId<VariantList>() ) {
    VariantList args = var.value<VariantList>();
    *ce->_ret = f->newInstance( ce->_data.scObject, args.data );
  }
  else {
    QList<QVariant> args;
    if( var.isValid() ) args << var;
    *ce->_ret = f->newInstance( ce->_data.scObject, args );
  }
}

QC_LANG_PRIMITIVE( QObject_New, 2, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  PyrObject *scObject = slotRawObject( r );

  QString realClassName = Slot::toString( &scObject->classptr->name );
  qscDebugMsg( "CREATE: %s\n", realClassName.toStdString().c_str() );

  CreationData data;

  data.scObject = scObject;
  data.scClassName = Slot::toString( a+0 );
  data.arguments = Slot::toVariant( a+1 );

  QObjectProxy *proxy = 0;

  CreationEvent *event = new CreationEvent( data, &proxy );
  QcApplication::postSyncEvent( event, &qcCreateQObject );

  if( !proxy ) return errFailed;

  SetPtr( scObject->slots, proxy );

  InstallFinalizer( g, scObject, 1, QObject_Finalize );

  return errNone;
}

QC_LANG_PRIMITIVE( QObject_Destroy, 0, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  QString realClassName =
    Slot::toString( &slotRawObject( r )->classptr->name );
  qscDebugMsg( "DESTROY: %s\n", realClassName.toStdString().c_str() );

  if( IS_OBJECT_NIL( r ) ) return errFailed;

  QObjectProxy *proxy = QOBJECT_FROM_SLOT( r );

  proxy->destroy();

  /* NOTE destroy has set QObjectProxy::scObject to 0, so we have to invalidate
    the SC object ourselves */
  SetNil( slotRawObject( r )->slots );

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

QC_LANG_PRIMITIVE( QObject_SetProperty, 3, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  if( IS_OBJECT_NIL( r ) ) return errFailed;

  QObjectProxy *proxy = QOBJECT_FROM_SLOT( r );

  PyrSymbol *symProp = slotRawSymbol( a+0 );
  PyrSlot *slotVal = a+1;
  bool direct = IsTrue( a+2 );

  return proxy->setProperty( symProp->name, slotVal, direct );
}

QC_LANG_PRIMITIVE( QObject_GetProperty, 2, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  if( IS_OBJECT_NIL( r ) ) return errFailed;

  QObjectProxy *proxy = QOBJECT_FROM_SLOT( r );

  PyrSymbol *symProp = slotRawSymbol( a+0 );
  PyrSlot *slotRetExtra = a+1;

  return proxy->getProperty( symProp->name, r, slotRetExtra );
}

QC_LANG_PRIMITIVE( QObject_SetEventHandler, 3, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  if( IS_OBJECT_NIL( r ) ) return errFailed;

  if( NotInt( a+0 ) || NotSym( a+1 ) ) return errWrongType;
  int eventType = Slot::toInt( a+0 );
  PyrSymbol *method = 0;
  slotSymbolVal( a+1, &method );
  bool direct = IsTrue( a+2 );

  QObjectProxy *proxy = QOBJECT_FROM_SLOT( r );

  proxy->setEventHandler( eventType, method, direct );

  return errNone;
}

QC_LANG_PRIMITIVE( QObject_Connect, 3, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  if( IS_OBJECT_NIL( r ) ) return errFailed;

  QString signal = Slot::toString( a+0 );
  if( signal.isEmpty() || NotSym( a+1 ) ) return errWrongType;
  PyrSymbol *handler = 0; slotSymbolVal( a+1, &handler );
  bool direct = Slot::toBool( a+2 );

  QObjectProxy *proxy = QOBJECT_FROM_SLOT( r );

  proxy->connect( signal, handler );

  return errNone;
}

QC_LANG_PRIMITIVE( QObject_InvokeMethod, 3, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  if( IS_OBJECT_NIL( r ) ) return errFailed;

  if( !IsSym( a+0 ) ) return errWrongType;

  const char *method = slotRawSymbol( a+0 )->name;
  PyrSlot *methodArgs = a+1;
  bool sync = !IsFalse( a+2 );

  qscDebugMsg( "INVOKE: method '%s'; synchronous = %i\n", method, sync );

  QObjectProxy *proxy = QOBJECT_FROM_SLOT( r );
  if( !proxy->invokeMethod( method, methodArgs, sync ) ) return errFailed;

  return errNone;
}
