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
        qscErrorMsg( "QObject:connect: Don't know how to handle '%s', "
                     "use qRegisterMetaType to register it.\n",
                    params.at(i).constData() );
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

  bool connect( const char *sigName, PyrSymbol * scMethod )
  {
    Q_ASSERT( sigName );

    const QMetaObject *mo = _proxy->object()->metaObject();

    QByteArray signal = QMetaObject::normalizedSignature( sigName );
    int sigId = mo->indexOfSignal( signal );

    if( sigId < 0 ) {
      qscErrorMsg( "No such signal: '%s'\n", signal.constData() );
      return false;
    }

    Q_FOREACH( QcDynamicSlot* ds, _dynSlots ) {
      if( ds->signalId() == sigId && ds->scMethod() == scMethod ) return true;
    }

    int slotId = QObject::staticMetaObject.methodCount() + _dynSlots.size();

    if( !QMetaObject::connect( _proxy->object(), sigId, this, slotId,
                              Qt::DirectConnection, 0) )
    {
      qscErrorMsg( "QMetaObject::connect returned false."
                   " Unable to connect.\n");
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
      qscDebugMsg( "SIGNAL: '%s' handled by method '%s'\n",
                  mm.signature(), scMethod->name );

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
