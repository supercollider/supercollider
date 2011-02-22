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

#include "../Common.h"
#include "../Slot.h"
#include "../QcApplication.h"
#include "QtCollider.h"

#include <PyrKernel.h>

#include <QFontMetrics>
#include <QDesktopWidget>
#include <QFontDatabase>

using namespace QtCollider;

QC_LANG_PRIMITIVE( QtGUI_Start, 0, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  // FIXME is QApplication::instance() thread-safe??
  if( !QApplication::instance() ) {
    QtCollider::init();
  }
  return errNone;
}

QC_LANG_PRIMITIVE( QtGUI_SetDebugLevel, 1, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  QtCollider::setDebugLevel( Slot::toInt(a) );
  return errNone;
}

void qcScreenBounds( QcSyncEvent *e )
{
  QcGenericEvent *ce = static_cast<QcGenericEvent*>(e);
  *ce->_return = QVariant( QApplication::desktop()->screenGeometry() );
}

QC_LANG_PRIMITIVE( QWindow_ScreenBounds, 1, PyrSlot *r, PyrSlot *rectSlot, VMGlobals *g )
{
  if( !isKindOfSlot( rectSlot, s_rect->u.classobj ) ) return errWrongType;

  QVariant var;

  QcGenericEvent *e = new QcGenericEvent(0, QVariant(), &var);
  QcApplication::postSyncEvent( e, &qcScreenBounds );

  QRect rect = var.value<QRect>();

  int err = Slot::setRect( rectSlot, rect );
  if( err ) return err;

  slotCopy( r, rectSlot );

  return errNone;
}

QC_LANG_PRIMITIVE( Qt_StringBounds, 3, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  QString str = Slot::toString( a );

  QFont f = Slot::toFont( a+1 );

  QFontMetrics fm( f );
  QRect bounds = fm.boundingRect( str );

  Slot::setRect( a+2, bounds );
  slotCopy( r, a+2 );

  return errNone;
}

QC_LANG_PRIMITIVE( Qt_AvailableFonts, 0, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  QFontDatabase database;
  VariantList l;
  Q_FOREACH( QString family, database.families() ) {
    l.data << QVariant(family);
  }
  Slot::setVariantList( r, l );
  return errNone;
}

static void qcGlobalPalette( QcSyncEvent *e )
{
  QcGenericEvent *ge = static_cast<QcGenericEvent*>(e);
  *ge->_return = QVariant( QApplication::palette() );
}

QC_LANG_PRIMITIVE( Qt_GlobalPalette, 1, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  QVariant var;

  QcGenericEvent *e = new QcGenericEvent(0, QVariant(), &var);
  QcApplication::postSyncEvent( e, &qcGlobalPalette );

  QPalette p = var.value<QPalette>();
  if( Slot::setPalette( a, p ) ) return errFailed;

  slotCopy( r, a );

  return errNone;
}

static void qcSetGlobalPalette( QcSyncEvent *e )
{
  QcGenericEvent *ge = static_cast<QcGenericEvent*>(e);
  QPalette p = ge->_data.value<QPalette>();
  QApplication::setPalette( p );
}

QC_LANG_PRIMITIVE( Qt_SetGlobalPalette, 1, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  QPalette p = Slot::toPalette( a );
  QcGenericEvent *e = new QcGenericEvent(0, QVariant(p));
  QcApplication::postSyncEvent( e, &qcSetGlobalPalette );
  return errNone;
}
