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


#ifndef _SC_QT_COMMON_H
#define _SC_QT_COMMON_H

#include <cstdarg>
#include <cstdio>

#ifdef QC_DEBUG
  #define qscDebugMsg( ... ) printf( "Qt: " __VA_ARGS__)
#else
  #define qscDebugMsg( ... )
#endif

#define qscErrorMsg( ... ) printf( "Qt: ERROR: " __VA_ARGS__)

#include <QList>
#include <QVariant>
#include <QEvent>
#include <QMutex>
#include <QWaitCondition>

#include <PyrSymbol.h>
#include <PyrObject.h>

extern pthread_mutex_t gLangMutex;

struct VariantList {
  QList<QVariant> data;
};

Q_DECLARE_METATYPE( VariantList );


struct QtServiceEvent;

typedef void (*EventHandlerFn) ( QtServiceEvent * );

namespace QtCollider {
  enum Type {
    Type_VariantList,
    Type_Count
  };
}


//namespace QtCollider {

enum QtEventType {
  CustomType = QEvent::User,
  SCMethodType,
  CreationType,
  ScMethodCallType
};

enum QcCustomEventType {
  ScreenBoundsRequest,
  DeleteObjectRequest
};


//}

struct QtServiceEvent : public QEvent
{
  QtServiceEvent( QtEventType type )
    : QEvent( (QEvent::Type) (type) ),
    _cond( 0 ),
    _mutex( 0 ),
    _handler( 0 )
  { }
  ~QtServiceEvent()
  {
    if( _cond && _mutex ) {
      _mutex->lock();
      _cond->wakeAll();
      _mutex->unlock();
    }
  }
  private:
    friend class QtService;
    QWaitCondition *_cond;
    QMutex *_mutex;
    EventHandlerFn _handler;
};

struct CustomEvent : public QtServiceEvent
{
  CustomEvent( int type, const QVariant& data = QVariant(), QVariant *ret = 0 )
    : QtServiceEvent( CustomType ),
    _data( data ),
    _return( ret ),
    _type( type )
  {}
  template <class T> void returnThis( T arg ) {
    *_return = QVariant::fromValue<T>( arg );
  }
  QVariant _data;
  QVariant *_return;
  int _type;
};

struct ScMethodCallEvent : public QEvent
{
  ScMethodCallEvent( const QString &s,
                     const QList<QVariant> &l = QList<QVariant>(),
                     bool b_locked = false )
  : QEvent( (QEvent::Type) ScMethodCallType ),
    method( getsym( s.toStdString().c_str() ) ),
    args( l ),
    locked( b_locked )
  {}

  ScMethodCallEvent( PyrSymbol *m,
                     const QList<QVariant> &l = QList<QVariant>(),
                     bool b_locked = false )
  : QEvent( (QEvent::Type) ScMethodCallType ),
    method( m ),
    args( l ),
    locked( b_locked )
  {}

  PyrSymbol *method;
  QList<QVariant> args;
  bool locked;
};

namespace QtCollider {

  void lockLang();

  inline static void unlockLang()
  {
    pthread_mutex_unlock(&gLangMutex);
    //printf("UNLOCKED\n");
  }

}

#endif //_SC_QT_COMMON_H
