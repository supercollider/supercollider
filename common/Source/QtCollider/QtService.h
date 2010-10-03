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

#ifndef _QT_BRIDGE_H
#define _QT_BRIDGE_H

#include "ScQt.h"
#include "Common.h"

#include <QVariant>
#include <QThread>

#include <cstdio>

#include <PyrObject.h>

#include <QThread>

class QObjectProxy;

///////////////////////////// Events ///////////////////////////////////////////

struct CreationData {
  QString scClassName;
  PyrObject * scObject;
  QVariant arguments;
};

struct CreationEvent : public QtServiceEvent
{
  CreationEvent( const CreationData& data, QObjectProxy** ret )
  : QtServiceEvent( CreationType ),
    _data ( data ),
    _ret ( ret )
  {}
  const CreationData & _data;
  QObjectProxy **_ret;
};

////////////////////////////// QtService ///////////////////////////////////////

class QtService : public QObject
{
  Q_OBJECT
  public:
    //NOTE: All public functions are thread-safe

    static QtService *instance();
    void stop(); //TODO

    QObjectProxy *create( const CreationData &data );
    void destroy( QObjectProxy * );

    QRect screenBounds();

    void postSyncEvent( QtServiceEvent *e, QObject *rcv = 0, EventHandlerFn fn = 0 );

  private:
    QtService();
    void customEvent( QEvent* );
    void handleRequest( CustomEvent * );
    void handleCreation( CreationEvent * );

    QMutex prMutex;
    friend void QtService_Start();
};

class QcLangThread : public QThread
{
  public:
    QcLangThread( int argc, char **argv );
  private:
    void run();
    int argc;
    char **argv;
};

#endif //_QT_BRIDGE_H