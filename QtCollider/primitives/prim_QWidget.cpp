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
#include "Slot.h"

#include <QWidget>

// WARNING these primitives have to always execute asynchronously, or Cocoa language client will
// hang.

QC_LANG_PRIMITIVE( QWidget_SetFocus, 1, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  PyrObject *scObj = slotRawObject( r );

  QObject *proxy = static_cast<QObject*>( slotRawPtr( scObj->slots ) );
  bool b = IsTrue( a );

  int ok = QMetaObject::invokeMethod( proxy, "setFocus", Qt::QueuedConnection, Q_ARG( bool, b ) );

  if( !ok ) return errFailed;
  return errNone;
}

QC_LANG_PRIMITIVE( QWidget_BringFront, 1, PyrSlot *r, PyrSlot *a, VMGlobals *g ) {
  PyrObject *scObj = slotRawObject( r );

  QObject *proxy = static_cast<QObject*>( slotRawPtr( scObj->slots ) );

  int ok = QMetaObject::invokeMethod( proxy, "bringFront", Qt::QueuedConnection );

  if( !ok ) return errFailed;
  return errNone;
}
