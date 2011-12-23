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

#include "QcApplication.h"
#include "widgets/QcTreeWidget.h"

#include <PyrLexer.h>
#include <VMGlobals.h>
#include <PyrKernel.h>
#include <PyrSlot.h>
#include <GC.h>

#include <QThread>
#include <QFileOpenEvent>
#include <QIcon>

extern bool compiledOK;

QcApplication * QcApplication::_instance = 0;
QMutex QcApplication::_mutex;

#ifdef Q_WS_X11
#include <X11/Xlib.h>
#endif


/* on x11, we need to check, if we can actually connect to the X server */
static bool QtColliderUseGui(void)
{
#ifdef Q_WS_X11
  Display *dpy = XOpenDisplay(NULL);
  if (!dpy)
    return false;
  XCloseDisplay(dpy);
  return true;
#else
  return true;
#endif
}

QcApplication::QcApplication( int & argc, char ** argv )
: QApplication( argc, argv, QtColliderUseGui() )
{
  _mutex.lock();
  _instance = this;
  _mutex.unlock();
  qRegisterMetaType<VariantList>();
  qRegisterMetaType<QcTreeWidget::ItemPtr>();

  if (QtColliderUseGui()) { // avoid a crash on linux, if x is not available
    QIcon icon;
    icon.addFile(":icons/sc-cube-128");
    icon.addFile(":icons/sc-cube-48");
    icon.addFile(":icons/sc-cube-32");
    icon.addFile(":icons/sc-cube-16");
    setWindowIcon(icon);
  }
}

QcApplication::~QcApplication()
{
  _mutex.lock();
  _instance = 0;
  _mutex.unlock();
}

bool QcApplication::compareThread()
{
  bool same;

  _mutex.lock();

  if( _instance )
    same = QThread::currentThread() == _instance->thread();
  else
    same = false;

  _mutex.unlock();

  return same;
}

void QcApplication::interpret( const QString &str, bool print )
{
  QtCollider::lockLang();
  if( compiledOK ) {
      VMGlobals *g = gMainVMGlobals;

      PyrString *strObj = newPyrString( g->gc, str.toStdString().c_str(), 0, true );

      SetObject(&slotRawInterpreter(&g->process->interpreter)->cmdLine, strObj);
      g->gc->GCWrite(slotRawObject(&g->process->interpreter), strObj);

      runLibrary( print ? QtCollider::s_interpretPrintCmdLine : QtCollider::s_interpretCmdLine );
  }
  QtCollider::unlockLang();
}

bool QcApplication::event( QEvent *e )
{
  if( e->type() == QEvent::FileOpen ) {
    // open the file dragged onto the application icon on Mac
    QFileOpenEvent *fe = static_cast<QFileOpenEvent*>(e);
    interpret( QString("Document.open(\"%1\")").arg(fe->file()), false );

    return true;
  }

  return QApplication::event( e );
}
