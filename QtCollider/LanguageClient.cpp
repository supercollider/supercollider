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
#include "QObjectProxy.h"

#include <PyrKernel.h>
#include <PyrLexer.h>

#include <qmath.h>
#include <QWidget>

extern double elapsedTime();

using namespace QtCollider;

QC_PUBLIC
int QtCollider::run(int argc, char** argv) {
  QtCollider::LangClient app("sclang");
  return app.run(argc, argv);
}

LangClient::LangClient( const char* name )
: SC_TerminalClient( name )
{
}

void LangClient::doInput()
{
  qcDebugMsg(2, "input");
  lockInput();
  interpretInput();
  QApplication::removePostedEvents( this, Event_SCRequest_Input );
  unlockInput();
}

void LangClient::doSchedule()
{
  qcDebugMsg(2, "tick");

  double t;
  bool next;

  lock();
  next = tickLocked( &t );
  QApplication::removePostedEvents( this, Event_SCRequest_Sched );
  unlock();

  flush();

  if( next ) {
    t -= elapsedTime();
    t *= 1000;
    int ti = qMax(0,qCeil(t));
    qcDebugMsg(2, QString("next at %1").arg(ti) );
    appClockTimer.start( ti, this );
  }
  else {
    appClockTimer.stop();
  }
}

void LangClient::commandLoop()
{
  doSchedule();
  QcApplication::instance()->exec();
}

void LangClient::daemonLoop()
{
  commandLoop();
}

void LangClient::sendSignal( Signal sig )
{
  QtCollider::EventType type;
  switch( sig )
  {
    case sig_input:
      type = Event_SCRequest_Input; break;
    case sig_sched:
      type = Event_SCRequest_Sched; break;
    case sig_recompile:
      type = Event_SCRequest_Recompile; break;
    case sig_stop:
      type = Event_SCRequest_Stop; break;
    default:
      return;
  }

  QApplication::postEvent( this, new SCRequestEvent(type) );
}

void LangClient::onQuit( int exitCode )
{
  QApplication::postEvent( this,
    new SCRequestEvent( Event_SCRequest_Quit, exitCode ) );
}

void LangClient::onLibraryShutdown()
{
  QWidgetList windows = QApplication::topLevelWidgets();
  Q_FOREACH( QWidget *w, windows )
    w->hide();
}

void LangClient::customEvent( QEvent *e )
{
  int type = e->type();
  switch( type ) {
    case Event_SCRequest_Input:
      doInput();
      break;
    case Event_SCRequest_Sched:
      doSchedule();
      break;
    case Event_SCRequest_Stop:
      stopMain();
      break;
    case Event_SCRequest_Quit:
    {
      int code = static_cast<SCRequestEvent*>(e)->data.toInt();
      qcDebugMsg( 1, QString("Quit requested with code %1").arg(code) );
      qApp->exit( code );
      break;
    }
    case Event_SCRequest_Recompile:
      recompileLibrary();
      break;
    default: ;
  }
}

void LangClient::timerEvent( QTimerEvent *e )
{
  if( e->timerId() == appClockTimer.timerId() ) doSchedule();
}
