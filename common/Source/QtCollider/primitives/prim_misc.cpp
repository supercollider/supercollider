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

#include "../Common.h"
#include "../Slot.h"
#include "../QcApplication.h"

#include <PyrKernel.h>

#include <QFontMetrics>
#include <QDesktopWidget>

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

void defineMiscPrimitives()
{
  qscDebugMsg( "defining miscellaneous primitives\n" );

  QtCollider::PrimitiveDefiner d;
  d.define( "_QtGUI_Start", QtGui_Start, 1, 0 );
  d.define( "_QWindow_ScreenBounds", QWindow_ScreenBounds, 2, 0 );
  d.define( "_Qt_StringBounds", Qt_StringBounds, 4, 0);
}
