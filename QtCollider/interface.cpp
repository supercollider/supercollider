/************************************************************************
*
* Copyright 2011 Jakob Leben (jakob.leben@gmail.com)
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

#include "QtCollider.h"
#include "QcApplication.h"
#include "Common.h"
#include "style/ProxyStyle.hpp"

#include <QPlastiqueStyle>
#include <QTimer>
#include <QEventLoop>

#ifdef Q_WS_X11
# include <X11/Xlib.h>
#endif

#include <clocale>

QC_PUBLIC
void QtCollider::init() {
  if( !QApplication::instance() ) {
    qcDebugMsg( 1, "Initializing QtCollider" );
#ifdef Q_WS_X11
    XInitThreads();
#endif
#ifdef Q_OS_MAC
    QApplication::setAttribute( Qt::AA_MacPluginApplication, true );
#endif
    static int qcArgc = 1;
    static char qcArg0[] = "";
    static char *qcArgv[1];
    qcArgv[0] = qcArg0;
    QcApplication *qcApp = new QcApplication( qcArgc, qcArgv );
    qcApp->setQuitOnLastWindowClosed( false );
#ifdef Q_OS_MAC
    qcApp->setStyle( new QtCollider::ProxyStyle( new QPlastiqueStyle ) );
#endif
    // NOTE: Qt may tamper with the C language locale, affecting POSIX number-string conversions.
    // Revert the locale to default:
    setlocale( LC_NUMERIC, "C" );
  }
}

QC_PUBLIC
int QtCollider::exec( int argc, char** argv ) {
  QtCollider::init();
  Q_ASSERT( qApp );
  return qApp->exec();
}
