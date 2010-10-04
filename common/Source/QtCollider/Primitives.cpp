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

#include <cstdio>

#include "QcApplication.h"
#include "QcObjectFactory.h"
#include "Common.h"
#include "QObjectProxy.h"
#include "Slot.h"

#include <PyrPrimitive.h>
#include <PyrObject.h>
#include <PyrKernel.h>
#include <VMGlobals.h>

#include <QFontMetrics>
#include <QDesktopWidget>

extern pthread_mutex_t gLangMutex;

#define IS_OBJECT_NIL( a ) \
  IsNil( slotRawObject(a)->slots )

#define QOBJECT_FROM_SLOT( s ) \
  ((QObjectProxy*) slotRawPtr( slotRawObject( s )->slots ))

PyrSymbol *s_QView, *s_QLayout, *s_QHLayout, *s_QFont, *s_QObject;
PyrSymbol *s_font, *sym_string, *s_states;

int QtGui_Start(struct VMGlobals *, int)
{
  // FIXME is QApplication::instance() thread-safe??
  if( !QApplication::instance() ) {
    #ifdef Q_OS_MAC
      QApplication::setAttribute( Qt::AA_MacPluginApplication, true );
    #endif
    int qcArgc = 0;
    char **qcArgv = 0;
    QcApplication *qcApp = new QcApplication( qcArgc, qcArgv  );
    qcApp->setQuitOnLastWindowClosed( false );
  }
  return errNone;
}

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

  if( !isKindOfSlot( args, s_QObject->u.classobj ) ) return errFailed;

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

void qcDestroyQObject( QcSyncEvent *e )
{
  QcGenericEvent *ce = static_cast<QcGenericEvent*>(e);
  delete ( ce->_data.value<QObjectProxy*>() );
}

int QObject_Destroy (struct VMGlobals *g, int)
{
  /* NOTE we post a synchronous event (waiting for object to be deleted),
  because the SC object pointer should not be accessed after this call */

  if( !isKindOfSlot( g->sp, s_QObject->u.classobj ) ) return errFailed;
  if( IS_OBJECT_NIL( g->sp ) ) return errFailed;

  QString realClassName =
    Slot::toString( &slotRawObject( g->sp )->classptr->name );
  qscDebugMsg( "DESTROY: %s\n", realClassName.toStdString().c_str() );

  QObjectProxy *proxy = QOBJECT_FROM_SLOT( g->sp );

  QVariant data = QVariant::fromValue<QObjectProxy*>( proxy );

  QcGenericEvent *event = new QcGenericEvent( 0, data );
  QcApplication::postSyncEvent( event, &qcDestroyQObject );

  return errNone;
}

int QObject_Finalize( struct VMGlobals *, struct PyrObject *obj )
{
  qscDebugMsg("Finalizing a QObject\n");
  if( IsNil( obj->slots ) ) return errNone;
  QObjectProxy *proxy = (QObjectProxy*) slotRawPtr( obj->slots );

  QVariant data = QVariant::fromValue<QObjectProxy*>( proxy );

  QcGenericEvent *event = new QcGenericEvent( 0, data );
  QcApplication::postSyncEvent( event, &qcDestroyQObject );

  return errNone;
}

int QObject_SetProperty(struct VMGlobals *g, int)
{
  PyrSlot *args = g->sp - 3;

  if( !isKindOfSlot( args, s_QObject->u.classobj ) ) return errFailed;

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

  if( !isKindOfSlot( args, s_QObject->u.classobj ) ) return errFailed;

  if( IS_OBJECT_NIL( args ) ) return errFailed;

  QObjectProxy *proxy = QOBJECT_FROM_SLOT( args );

  PyrSymbol *symProp = slotRawSymbol( args+1 );
  PyrSlot *slotRetExtra = args+2;

  return proxy->getProperty( symProp->name, args, slotRetExtra );
}

int QObject_SetEventHandler (struct VMGlobals *g, int)
{
  PyrSlot *args = g->sp - 3;

  if( !isKindOfSlot( args, s_QObject->u.classobj ) ) return errFailed;
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

  if( !isKindOfSlot( args, s_QObject->u.classobj ) ) return errFailed;
  if( IS_OBJECT_NIL( args ) ) return errFailed;

  if( !IsSym( args+1 ) ) return errWrongType;

  const char *method = slotRawSymbol( args+1 )->name;

  QObjectProxy *proxy = QOBJECT_FROM_SLOT( args );
  if( !proxy->invokeMethod( method, args+2 ) ) return errFailed;

  return errNone;
}

void qcScreenBounds( QcSyncEvent *e )
{
  QcGenericEvent *ce = static_cast<QcGenericEvent*>(e);
  *ce->_return = QVariant( QApplication::desktop()->screenGeometry() );
}

int QWindow_ScreenBounds (struct VMGlobals *g, int)
{
  if( !isKindOfSlot( g->sp, s_rect->u.classobj ) ) return errWrongType;

  QVariant var;

  QcGenericEvent *e = new QcGenericEvent(0, QVariant(), &var);
  QcApplication::postSyncEvent( e, &qcScreenBounds );

  QRect r = var.value<QRect>();

  int err = Slot::setRect( g->sp, r );
  if( err ) return err;

  slotCopy( g->sp - 1, g->sp );

  return errNone;
}

int Qt_StringBounds (struct VMGlobals *g, int)
{
  PyrSlot *args = g->sp - 3;

  QString str = Slot::toString( args+1 );

  QFont f = Slot::toFont( args+2 );

  QFontMetrics fm( f );
  QRect bounds = fm.boundingRect( str );

  Slot::setRect( args+3, bounds );
  slotCopy( args, args+3 );

  return errNone;
}

int Crash_Test(struct VMGlobals *, int) {
  printf("Crash!\n");
  char *c = 0;
  c[100] = 'a';
  return errNone;
}

int Test_Sleep(struct VMGlobals *, int)
{
  sleep(1);
  return errNone;
}

int Test_Finalize( struct VMGlobals*, struct PyrObject* )
{
  printf("finalize!!\n");
  return errNone;
}

int Test_Install_Finalizer(struct VMGlobals *g, int)
{
  printf("installing finalizer\n");
  InstallFinalizer(g, slotRawObject(g->sp), 0, Test_Finalize);
  return errNone;
}

int Test_Lang_Lock(struct VMGlobals *, int)
{
  printf("locking\n");
  pthread_mutex_lock (&gLangMutex);
  printf("locked\n");
  pthread_mutex_unlock (&gLangMutex);
  printf("unlocked\n");
  return errNone;
}

void initQtPenPrimitives();

void initQtGUIPrimitives () {
  qscDebugMsg("initializing QtGUI primitives\n");

  int base, index;

  base = nextPrimitiveIndex();

  index = 0;

  definePrimitive(base, index++, "_QtGUI_Start",
                  QtGui_Start, 1, 0);

  definePrimitive(base, index++, "_QObject_New",
                  QObject_New, 3, 0);
  definePrimitive(base, index++, "_QObject_Destroy",
                  QObject_Destroy, 1, 0);
  definePrimitive(base, index++, "_QObject_SetProperty",
                  QObject_SetProperty, 4, 0);
  definePrimitive(base, index++, "_QObject_GetProperty",
                  QObject_GetProperty, 3, 0);
  definePrimitive(base, index++, "_QObject_SetEventHandler",
                  QObject_SetEventHandler, 4, 0);
  definePrimitive(base, index++, "_QObject_InvokeMethod",
                  QObject_InvokeMethod, 3, 0);

  definePrimitive(base, index++, "_QWindow_ScreenBounds",
                  QWindow_ScreenBounds, 2, 0);
  definePrimitive(base, index++, "_Qt_StringBounds",
                  Qt_StringBounds, 4, 0);

  definePrimitive(base, index++, "_Crash_Test",
                  Crash_Test, 1, 0);
  definePrimitive(base, index++, "_Test_Sleep",
                  Test_Sleep, 1, 0);
  definePrimitive(base, index++, "_Test_Install_Finalizer",
                  Test_Install_Finalizer, 1, 0);
  definePrimitive(base, index++, "_Test_Lang_Lock",
                  Test_Lang_Lock, 1, 0);

  s_QObject = getsym("QObject");
  s_QView = getsym("QView");
  s_QLayout = getsym("QLayout");
  s_QHLayout = getsym("QHLayout");
  s_QFont = getsym("QFont");
  s_font = getsym("font");
  sym_string = getsym("string");
  s_states = getsym("states");

  initQtPenPrimitives();
}

