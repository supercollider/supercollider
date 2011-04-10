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

#define qcProxyDebugMsg( LEVEL, MSG ) \
  qcDebugMsg( LEVEL, QString("[%1]: ").arg(_scClassName) + QString(MSG) )

struct VariantList;
struct ScMethodCallEvent;

class QObjectProxy;
class QcSignalSpy;
class QcMethodSignalHandler;
class QcFunctionSignalHandler;

namespace QtCollider {
  struct SetPropertyEvent;
  struct SetEventHandlerEvent;
  class DestroyEvent;

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

    // compare current thread to proxy's thread
    bool compareThread();

    // WARNING: must be called with language locked!
    void finalize() { _scObject = 0; }

    inline QObject *object() const { return qObject; }
    inline PyrObject *scObject() const { return _scObject; }

    // Lock for usage of object() outside Qt thread.
    // WARNING Do not post any sync events while locked!
    inline void lock() { mutex.lock(); }
    inline void unlock() { mutex.unlock(); }

    QString scClassName() const { return _scClassName; }

    virtual bool setParent( QObjectProxy *parent );

    bool setProperty( const char *property, const QVariant & val );
    bool setPropertyEvent( QtCollider::SetPropertyEvent * );
    QVariant property( const char *property );

    QList<PyrObject*> children( PyrSymbol *className );
    PyrObject *parent( PyrSymbol *className );

    bool connectObject( const char *signal, PyrObject *object, Qt::ConnectionType );
    bool connectMethod( const char *signal, PyrSymbol *method, Qt::ConnectionType );
    bool disconnectObject( const char *signal, PyrObject *object );
    bool disconnectMethod( const char *signal, PyrSymbol *method);

    bool setEventHandlerEvent( QtCollider::SetEventHandlerEvent * );
    bool destroyEvent( QtCollider::DestroyEvent * );

    // thread-safe (if connection == queued)
    bool invokeMethod( const char *method, PyrSlot *ret, PyrSlot *arg, Qt::ConnectionType );

    static QObjectProxy *fromObject( QObject * );

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
    // NOTE: scObject is protected by the language lock. Should not use it without it!
    PyrObject *_scObject;
    // NOTE: for the reason above we extract SC class name at construction
    QString _scClassName;

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

struct SetPropertyEvent
: public RequestTemplate<SetPropertyEvent, &QObjectProxy::setPropertyEvent>
{
  PyrSymbol *property;
  QVariant value;
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
