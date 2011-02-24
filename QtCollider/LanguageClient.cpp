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

#include "LanguageClient.h"
#include "QcApplication.h"
#include "QtCollider.h"

using namespace QtCollider;

int QtCollider::run(int argc, char** argv) {
  QtCollider::LangClient app("sclang");
  return app.run(argc, argv);
}

LangClient::LangClient( const char* name )
: SC_TerminalClient( name )
{
}

void LangClient::commandLoop()
{
  langTimer = new QTimer(this);
  QObject::connect( langTimer, SIGNAL(timeout()), this, SLOT(cmdLineTick()) );
  langTimer->start(10);
  QcApplication::instance()->exec();
}

void LangClient::daemonLoop()
{
  langTimer = new QTimer(this);
  QObject::connect( langTimer, SIGNAL(timeout()), this, SLOT(daemonTick()) );
  langTimer->start(10);
  QcApplication::instance()->exec();
}

void LangClient::cmdLineTick()
{
  if( shouldBeRunning() ) {
    tick();
    readCmdLine();
  }
  else {
    QcApplication::instance()->quit();
  }
}

void LangClient::daemonTick()
{
  if( shouldBeRunning() ) tick();
  else QcApplication::instance()->quit();
}
