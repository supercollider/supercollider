/************************************************************************
*
* Copyright 2010-2011 Jakob Leben (jakob.leben@gmail.com)
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

namespace QtCollider {
  struct SetParentEvent;
  struct SetPropertyEvent;
  struct GetPropertyEvent;
  struct SetEventHandlerEvent;
  struct ConnectEvent;
  struct DisconnectEvent;
  struct InvokeMethodEvent;
  class DestroyEvent;
  struct GetChildrenEvent;
  struct GetParentEvent;
  struct GetValidityEvent;

  class ProxyToken : public QObject {
    Q_OBJECT
  public:
    ProxyToken( QObjectProxy *p, QObject *parent )
    : QObject(parent), proxy(p) { }
    QObjectProxy *proxy;
  };
}

class QObjectProxy : public QObject
{
  friend class QcMethodSignalHandler;
  friend class QcFunctionSignalHandler;

  Q_OBJECT

  public:

    enum DestroyAction {
      DestroyProxy,
      DestroyObject,
      DestroyProxyAndObject,
    };

    struct EventHandlerData
    {
      EventHandlerData() : type( QEvent::None ) {}
      int type;
      PyrSymbol *method;
      QtCollider::Synchronicity sync;
    };

    QObjectProxy( QObject *qObject, PyrObject *scObject );

    virtual ~QObjectProxy();

    inline QObject *object() const { return qObject; }

    // Lock for usage of object() outside Qt thread.
    // WARNING Do not post any sync events while locked!
    inline void lock() { mutex.lock(); }
    inline void unlock() { mutex.unlock(); }

    const char *scClassName() const;

    virtual bool setParentEvent( QtCollider::SetParentEvent * );
    bool setPropertyEvent( QtCollider::SetPropertyEvent * );
    bool getPropertyEvent( QtCollider::GetPropertyEvent * );
    bool setEventHandlerEvent( QtCollider::SetEventHandlerEvent * );
    bool connectEvent( QtCollider::ConnectEvent * );
    bool disconnectEvent( QtCollider::DisconnectEvent * );
    bool invokeMethodEvent( QtCollider::InvokeMethodEvent * );
    bool destroyEvent( QtCollider::DestroyEvent * );
    bool getChildrenEvent( QtCollider::GetChildrenEvent * );
    bool getParentEvent( QtCollider::GetParentEvent * );
    bool getValidityEvent( QtCollider::GetValidityEvent * );

    // thread-safe (if connection == queued)
    bool invokeMethod( const char *method, PyrSlot *ret, PyrSlot *arg, Qt::ConnectionType );

  protected:

    void invokeScMethod
      ( PyrSymbol *method, const QList<QVariant> & args = QList<QVariant>(),
        PyrSlot *result = 0, bool locked = false );

    virtual bool eventFilter( QObject * watched, QEvent * event );

    virtual bool interpretEvent( QObject *, QEvent *, QList<QVariant> & ) { return true; }

  private Q_SLOTS:

    void invalidate();

  private:

    inline void scMethodCallEvent( ScMethodCallEvent * );

    void customEvent( QEvent * );

    QObject *qObject;
    PyrObject *scObject;
    QMap<int,EventHandlerData> eventHandlers;
    QList<QcMethodSignalHandler*> methodSigHandlers;
    QList<QcFunctionSignalHandler*> funcSigHandlers;
    QMutex mutex;
};

namespace QtCollider {

class RequestEvent : public QcSyncEvent {
  friend class QObjectProxy;

public:
  bool send( QObjectProxy *, Synchronicity );
  inline bool perform( QObjectProxy *proxy ) {
    bool done = execute( proxy );
    if( p_done ) *p_done = done;
    return done;
  }

protected:
  RequestEvent() : QcSyncEvent( QcSyncEvent::ProxyRequest ), p_done(0) {}
  virtual bool execute( QObjectProxy *proxy ) = 0;

private:
  bool *p_done;
};

template <class T, bool (QObjectProxy::*handler)( T* )>
class RequestTemplate : public RequestEvent {
protected:
  RequestTemplate(){}
private:
  bool execute( QObjectProxy *proxy ) {
    return (proxy->*handler)( static_cast<T*>( this ) );
  }
};

struct SetParentEvent
: public RequestTemplate<SetParentEvent, &QObjectProxy::setParentEvent>
{
  QObjectProxy *parent;
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

struct DisconnectEvent
: public RequestTemplate<DisconnectEvent, &QObjectProxy::disconnectEvent>
{
  PyrSymbol *method;
  PyrObject *function;
  QString signal;
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

struct GetChildrenEvent
: public RequestTemplate<GetChildrenEvent, &QObjectProxy::getChildrenEvent>
{
  GetChildrenEvent( PyrSymbol * cname, QList<PyrObject*> & ch )
  : className( cname ), children( ch ) {}
  PyrSymbol *className;
  QList<PyrObject*> &children;
};

struct GetParentEvent
: public RequestTemplate<GetParentEvent, &QObjectProxy::getParentEvent>
{
  PyrObject ** parent;
};

struct GetValidityEvent
: public RequestTemplate<GetValidityEvent, &QObjectProxy::getValidityEvent>
{};

} // namespace

Q_DECLARE_METATYPE( QObjectProxy * );

#endif //QC_QOBJECT_PROXY_H
