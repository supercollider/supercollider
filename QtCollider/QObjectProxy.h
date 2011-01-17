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

#ifndef QC_QOBJECT_PROXY_H
#define QC_QOBJECT_PROXY_H

#include "Common.h"

#include <QObject>
#include <QString>
#include <QVariant>
#include <QMap>
#include <QEvent>

#include <PyrObject.h>
#include <PyrSlot.h>
#include <PyrSymbol.h>

#define qcProxyDebugMsg( LEVEL, MSG ) qcSCObjectDebugMsg( LEVEL, scObject, MSG )

struct VariantList;
struct ScMethodCallEvent;

class QObjectProxy;
class QcSignalSpy;
class QcMethodSignalHandler;
class QcFunctionSignalHandler;

typedef void (QObjectProxy::*InterpretEventFn) ( QEvent *, QList<QVariant> & );

namespace QtCollider {
  struct SetParentEvent;
  struct SetPropertyEvent;
  struct GetPropertyEvent;
  struct SetEventHandlerEvent;
  struct ConnectEvent;
  struct InvokeMethodEvent;
  class DestroyEvent;
}

class QObjectProxy : public QObject
{
  friend class QcMethodSignalHandler;
  friend class QcFunctionSignalHandler;

  Q_OBJECT

  public:

    enum DestroyAction {
      DestroyProxy,
      DestroyProxyAndObject
    };

    struct EventHandlerData
    {
      EventHandlerData() : type( QEvent::None ) {}
      int type;
      PyrSymbol *method;
      QtCollider::Synchronicity sync;
      InterpretEventFn interpretFn;
    };

    QObjectProxy( QObject *qObject, PyrObject *scObject );

    virtual ~QObjectProxy();

    inline QObject *object() const { return qObject; }

    const char *scClassName() const;

    virtual bool setParentEvent( QtCollider::SetParentEvent * );
    bool setPropertyEvent( QtCollider::SetPropertyEvent * );
    bool getPropertyEvent( QtCollider::GetPropertyEvent * );
    bool setEventHandlerEvent( QtCollider::SetEventHandlerEvent * );
    bool connectEvent( QtCollider::ConnectEvent * );
    bool invokeMethodEvent( QtCollider::InvokeMethodEvent * );
    bool destroyEvent( QtCollider::DestroyEvent * );

    bool invokeMethod( const char *method, PyrSlot *ret, PyrSlot *arg, Qt::ConnectionType );

  private:

    void invokeScMethod
      ( PyrSymbol *method, const QList<QVariant> & args = QList<QVariant>(),
        PyrSlot *result = 0, bool locked = false );

    inline void scMethodCallEvent( ScMethodCallEvent * );

    void interpretMouseEvent( QEvent *e, QList<QVariant> &args );

    void interpretKeyEvent( QEvent *e, QList<QVariant> &args );

    void customEvent( QEvent * );

    bool eventFilter( QObject * watched, QEvent * event );

    QObject *qObject;
    PyrObject *scObject;
    QMap<int,EventHandlerData> eventHandlers;
    QList<QcMethodSignalHandler*> methodSigHandlers;
    QList<QcFunctionSignalHandler*> funcSigHandlers;
};

namespace QtCollider {

class RequestEvent : public QcSyncEvent {
  friend class QObjectProxy;

public:
  bool send( QObjectProxy *, Synchronicity );
  virtual bool execute( QObjectProxy * ) = 0;
protected:
  RequestEvent() : QcSyncEvent( QcSyncEvent::ProxyRequest ), p_done(0) {}
  bool *p_done;
};

template <class T, bool (QObjectProxy::*RESPONSE)( T* )>
class RequestTemplate : public RequestEvent {
protected:
  RequestTemplate() {}
private:
  bool execute( QObjectProxy *obj ) {
    T *data = static_cast<T*>( this );
    bool done = (obj->*RESPONSE)(data);
    if( p_done ) *p_done = done;
    return done;
  }
};

struct SetParentEvent
: public RequestTemplate<SetParentEvent, &QObjectProxy::setParentEvent>
{
  QObject *parent;
};

struct SetPropertyEvent
: public RequestTemplate<SetPropertyEvent, &QObjectProxy::setPropertyEvent>
{
  PyrSymbol *property;
  QVariant value;
};

struct GetPropertyEvent
: public RequestTemplate<GetPropertyEvent, &QObjectProxy::getPropertyEvent>
{
  GetPropertyEvent( PyrSymbol *p, QVariant &v ) : property(p), value(v) {}
  PyrSymbol *property;
  QVariant &value;
};

struct SetEventHandlerEvent
: public RequestTemplate<SetEventHandlerEvent, &QObjectProxy::setEventHandlerEvent>
{
  SetEventHandlerEvent( int t, PyrSymbol *m, Synchronicity s )
  : type(t), method(m), sync(s) {}
  int type;
  PyrSymbol *method;
  Synchronicity sync;
};

struct ConnectEvent
: public RequestTemplate<ConnectEvent, &QObjectProxy::connectEvent>
{
  PyrSymbol *method;
  PyrObject *function;
  // QString is necessary, because signal signature can not be a valid PyrSymbol.
  // Think of brackets and similar...
  QString signal;
  Synchronicity sync;
};

struct InvokeMethodEvent
: public RequestTemplate<InvokeMethodEvent, &QObjectProxy::invokeMethodEvent>
{
  PyrSymbol *method;
  PyrSlot *ret;
  PyrSlot *arg;
};

class DestroyEvent
: public RequestTemplate<DestroyEvent, &QObjectProxy::destroyEvent>
{
public:
  DestroyEvent( QObjectProxy::DestroyAction act ) : _action( act ) {}
  QObjectProxy::DestroyAction action() { return _action; }
private:
  QObjectProxy::DestroyAction _action;
};

} // namespace

Q_DECLARE_METATYPE( QObjectProxy * );

#endif //QC_QOBJECT_PROXY_H
