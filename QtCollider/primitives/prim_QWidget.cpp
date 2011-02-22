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

#include "primitives.h"
#include "Slot.h"
#include "../QWidgetProxy.h"

#include <PyrKernel.h>

#include <QWidget>

using namespace QtCollider;

// WARNING these primitives have to always execute asynchronously, or Cocoa language client will
// hang.

QC_LANG_PRIMITIVE( QWidget_SetFocus, 1, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  QWidgetProxy *proxy = qobject_cast<QWidgetProxy*>( Slot::toObjectProxy( r ) );

  SetFocusRequest *req = new SetFocusRequest();
  req->focus = IsTrue( a );

  bool ok = req->send( proxy, Asynchronous );
  return ( ok ? errNone : errFailed );
}

QC_LANG_PRIMITIVE( QWidget_BringFront, 1, PyrSlot *r, PyrSlot *a, VMGlobals *g ) {
  QWidgetProxy *proxy = qobject_cast<QWidgetProxy*>( Slot::toObjectProxy( r ) );

  GenericWidgetRequest *req = new GenericWidgetRequest( &QWidgetProxy::bringFront );

  bool ok = req->send( proxy, Asynchronous );
  return ( ok ? errNone : errFailed );
}

QC_LANG_PRIMITIVE( QWidget_Refresh, 1, PyrSlot *r, PyrSlot *a, VMGlobals *g ) {
  QWidgetProxy *proxy = qobject_cast<QWidgetProxy*>( Slot::toObjectProxy( r ) );

  GenericWidgetRequest *req = new GenericWidgetRequest( &QWidgetProxy::refresh );

  // WARNING The request has to be sent synchronously! Only this will ensure that
  // refreshing within the SC drawing function will not lead into an infinite cycle.

  bool ok = req->send( proxy, Synchronous );
  return ( ok ? errNone : errFailed );
}

QC_LANG_PRIMITIVE( QWidget_MapToGlobal, 2, PyrSlot *r, PyrSlot *a, VMGlobals *g ) {
  QWidgetProxy *proxy = qobject_cast<QWidgetProxy*>( Slot::toObjectProxy( r ) );

  QPoint pt( Slot::toPoint( a ).toPoint() );

  MapToGlobalRequest *req = new MapToGlobalRequest( pt );
  req->send( proxy, Synchronous );

  int err = Slot::setPoint( a+1, pt );
  if( err ) return err;
  slotCopy( r, a+1 );

  return errNone;
}

QC_LANG_PRIMITIVE( QWidget_SetLayout, 1, PyrSlot *r, PyrSlot *a, VMGlobals *g ) {
  if( !isKindOfSlot( a, getsym("QLayout")->u.classobj ) ) return errWrongType;

  QWidgetProxy *wProxy = qobject_cast<QWidgetProxy*>( Slot::toObjectProxy(r) );
  QObjectProxy *lProxy = Slot::toObjectProxy( a );

  SetLayoutRequest *req = new SetLayoutRequest();
  req->layoutProxy = lProxy;

  bool ok = req->send( wProxy, Synchronous );
  return ( ok ? errNone : errFailed );
}
