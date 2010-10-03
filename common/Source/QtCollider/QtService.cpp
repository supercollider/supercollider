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

#include "QtService.h"
#include "QObjectProxy.h"
#include "Common.h"
#include "QcObjectFactory.h"

#include <PyrKernel.h>
#include <VMGlobals.h>
#include <SC_TerminalClient.h>

#include <QApplication>
#include <QDesktopWidget>
#include <QMouseEvent>

///////////////////// QtService startup ///////////////////////////////////////

pthread_t qt_thread;

static QtService *qtServiceInstance = 0;
QMutex qtServiceMutex;

// Actually just creates a QApplication and a QtService
void QtService_Start( )
{
  qtServiceMutex.lock();

  if( qtServiceInstance ) {
    qtServiceMutex.unlock();
    return;
  }

  if( !QApplication::instance() ) {
    int argc = 0;
    char **argv = 0;
#ifdef Q_OS_MAC
    QApplication::setAttribute( Qt::AA_MacPluginApplication, true );
#endif
    QApplication *app = new QApplication(argc,argv);
    app->setQuitOnLastWindowClosed( false );
  }

  qtServiceInstance = new QtService();

  qtServiceMutex.unlock();
}

void QtService_MainLoop( int argc, char **argv )
{
  // create language thread
  QcLangThread langThread( argc, argv );

  // create QApplication
  QtService_Start();

  // quit QApplication when language thread finishes
  QObject::connect( &langThread, SIGNAL(finished()),
                    QApplication::instance(), SLOT(quit()) );

  // start them both
  langThread.start();
  QApplication::instance()->exec();

  // after QApplication quits, thread should have terminated, or we wait for it
  langThread.wait();
}

///////////////////////////////////////////////////////////////////////////////

QtService * QtService::instance()
{
  QtService *ret = 0;
  qtServiceMutex.lock();
  ret = qtServiceInstance;
  qtServiceMutex.unlock();
  return ret;
}

QtService::QtService()
{
  qRegisterMetaType<VariantList>();
}

void QtService::stop()
{
    //TODO
}

QObjectProxy *QtService::create( const CreationData &data )
{
  qscDebugMsg( "CREATE: %s\n", data.scClassName.toStdString().c_str() );
  QObjectProxy *object = 0;
  CreationEvent *event = new CreationEvent( data, &object );
  postSyncEvent( event );
  return object;
}

void QtService::destroy( QObjectProxy *object )
{
  qscDebugMsg( "QtService::destroy\n" );

  /* NOTE we post a synchronous event (waiting for object to be deleted),
  because the SC object pointer should not be accessed after this call */

  QVariant data = QVariant::fromValue<QObjectProxy*>( object );
  postSyncEvent( new CustomEvent( DeleteObjectRequest, data ) );
}

QRect QtService::screenBounds()
{
  QVariant ret;
  QtServiceEvent *event = new CustomEvent( ScreenBoundsRequest, QVariant(), &ret );
  postSyncEvent( event );
  return ret.value<QRect>();
}

void QtService::postSyncEvent( QtServiceEvent *e, QObject *rcv, EventHandlerFn fn )
{
  QObject *receiver;
  if( rcv ) receiver = rcv;
  else {
    receiver = this;
    e->_handler = fn;
  }

  if( QThread::currentThread() == receiver->thread() ) {
    QApplication::sendEvent( receiver, e );
    delete e;
  }
  else {
    prMutex.lock();
    QWaitCondition cond;
    e->_cond = &cond;
    e->_mutex = &prMutex;

    QApplication::postEvent( receiver, e );

    cond.wait( &prMutex );
    prMutex.unlock();
  }
}

void QtService::customEvent( QEvent* ev ) {
  if( ev->type() < QEvent::User && ev->type() > QEvent::MaxUser ) return;

  QtServiceEvent *e = (QtServiceEvent*) ev;

  if( e->_handler ) {
    (*e->_handler) ( e );
    return;
  }

  int type = ev->type();
  switch( type ) {
    case CreationType:
      handleCreation( (CreationEvent*) ev );
      break;
    case CustomType:
      handleRequest( (CustomEvent*) ev );
      break;
  }
}

void QtService::handleRequest( CustomEvent *e )
{
  if( e->_type == ScreenBoundsRequest )
  {
    *e->_return = QVariant( QApplication::desktop()->screenGeometry() );
  }
  else if( e->_type == DeleteObjectRequest )
  {
    delete ( e->_data.value<QObjectProxy*>() );
  }
  else
    qscErrorMsg( "QtService: Unknown request\n");
}

void QtService::handleCreation( CreationEvent *e )
{

  CreateFn createFn = factoryFunction( e->_data.scClassName );

  if( !createFn ) {
    qscErrorMsg( "QObject Factory for %s not found!\n",
                          e->_data.scClassName.toStdString().c_str() );
    return;
  }

  *e->_ret = (*createFn)( e->_data.scObject, e->_data.arguments );
}


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
