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

#ifndef QC_SIGNALSPY_H
#define QC_SIGNALSPY_H

#include "Common.h"
#include "QObjectProxy.h"

#include <PyrSymbol.h>

#include <QByteArray>
#include <QList>
#include <QObject>
#include <QMetaObject>
#include <QMetaMethod>
#include <QVariant>

class QcDynamicSlot {
public:
  QcDynamicSlot( PyrSymbol *scMethod, QObject *obj, int sigId )
  : _scMethod( scMethod ),
    _sigId( sigId )
  {
    QMetaMethod signal = obj->metaObject()->method( sigId );
    QList<QByteArray> params = signal.parameterTypes();
    for( int i = 0; i < params.count(); ++i ) {
      int type = QMetaType::type( params.at(i).constData() );
      if( type == QMetaType::Void )
        qcErrorMsg( QString("QObject:connect: Don't know how to handle '%1', "
                            "use qRegisterMetaType to register it.")
                    .arg(params.at(i).constData()) );
      _argTypes << type;
    }
  }

  inline int signalId() const { return _sigId; }

  inline PyrSymbol *scMethod () const { return _scMethod; }

  void decode( void **argData, QList<QVariant> & args ) {
    for (int i = 0; i < _argTypes.count(); ++i) {
        QMetaType::Type type = static_cast<QMetaType::Type>(_argTypes.at(i));
        args << QVariant( type, argData[i + 1] );
    }
  }

private:
  PyrSymbol *_scMethod;
  int _sigId;
  QList<int> _argTypes;
};

class QcSignalSpy: public QObject
{
public:

  QcSignalSpy( QObjectProxy *proxy )
  : QObject( proxy ), _proxy( proxy )
  {}

  bool connect( const char *sigName, PyrSymbol * scMethod, bool direct = false )
  {
    Q_ASSERT( sigName );

    const QMetaObject *mo = _proxy->object()->metaObject();

    QByteArray signal = QMetaObject::normalizedSignature( sigName );
    int sigId = mo->indexOfSignal( signal );

    if( sigId < 0 ) {
      qcErrorMsg( QString("No such signal: '%1'").arg(signal.constData()) );
      return false;
    }

    Q_FOREACH( QcDynamicSlot* ds, _dynSlots ) {
      if( ds->signalId() == sigId && ds->scMethod() == scMethod ) return true;
    }

    int slotId = QObject::staticMetaObject.methodCount() + _dynSlots.size();

    Qt::ConnectionType conType =
      direct ? Qt::DirectConnection : Qt::QueuedConnection;

    if( !QMetaObject::connect( _proxy->object(), sigId, this, slotId,
                              conType, 0) )
    {
      qcErrorMsg( "QMetaObject::connect returned false. Unable to connect." );
      return false;
    }

    QcDynamicSlot *dynSlot = new QcDynamicSlot( scMethod,
                                                _proxy->object(), sigId );
    _dynSlots << dynSlot;

    return true;
  }

  int qt_metacall( QMetaObject::Call call, int methodId, void **argData )
  {
    methodId = QObject::qt_metacall( call, methodId, argData );

    if( methodId < 0 )
      return methodId;

    if( call == QMetaObject::InvokeMetaMethod ) {
      Q_ASSERT( methodId < _dynSlots.size() );

      QcDynamicSlot *ds = _dynSlots[methodId];
      PyrSymbol *scMethod = ds->scMethod();
      QList<QVariant> args;
      ds->decode( argData, args );

      const QMetaMethod mm =
        _proxy->object()->metaObject()->method( ds->signalId() );
      qcDebugMsg( 1, QString("SIGNAL: '%1' handled by method '%2'")
                      .arg(mm.signature()).arg(scMethod->name) );

      _proxy->invokeScMethod( scMethod, args );

      methodId = -1;
    }

    return methodId;
  }

private:

  QObjectProxy *_proxy;
  QList<QcDynamicSlot*> _dynSlots;
};

#endif //QC_SIGNALSPY_H
