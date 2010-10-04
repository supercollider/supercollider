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

#include "MainLoop.h"
#include "ScQt.h"
#include "QcApplication.h"

#include <SC_TerminalClient.h>

QcLangThread::QcLangThread( int c, char *v[] )
{
  argc = c;
  argv = v;
}

void QcLangThread::run()
{
  SC_TerminalClient app("sclang");
  app.run( argc, argv );
}

int QtCollider::run( int & argc, char **argv )
{
  // create language thread
  QcLangThread langThread( argc, argv );

  #ifdef Q_OS_MAC
    QApplication::setAttribute( Qt::AA_MacPluginApplication, true );
  #endif

  // create QcApplication
  int qcArgc = 1;
  char qcArg = '\0';
  char *qcArgv[1];
  qcArgv[0] = &qcArg;
  QcApplication qcApp( qcArgc, qcArgv  );

  qcApp.setQuitOnLastWindowClosed( false );

  // quit QApplication when language thread finishes
  QObject::connect( &langThread, SIGNAL(finished()),
                    &qcApp, SLOT(quit()) );

  // start them both
  langThread.start();
  qcApp.exec();

  // after QApplication quits, thread should have terminated, or we wait for it
  langThread.wait();

  return 0;
}
