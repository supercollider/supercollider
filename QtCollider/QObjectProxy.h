/************************************************************************
 *
 * Copyright 2010-2011 Jakob Leben (jakob.leben@gmail.com)
 *
 * This file is part of SuperCollider Qt GUI.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
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

#pragma once

#include "Common.h"

#include <QObject>
#include <QString>
#include <QVariant>
#include <QVector>
#include <QEvent>

#include <PyrObject.h>
#include <PyrSlot.h>
#include <PyrSymbol.h>

#define qcProxyDebugMsg(LEVEL, MSG) qcDebugMsg(LEVEL, QStringLiteral("[%1]: ").arg(_scClassName) + QString(MSG))

class QObjectProxy;
class QcSignalSpy;
class QcMethodSignalHandler;
class QcFunctionSignalHandler;

namespace QtCollider {
struct SetPropertyEvent;
class DestroyEvent;
struct ScMethodCallEvent;

class ProxyToken : public QObject {
    Q_OBJECT
public:
    ProxyToken(QObjectProxy* p, QObject* parent): QObject(parent), proxy(p) {}
    QObjectProxy* proxy;
};
}

class QObjectProxy : public QObject {
    friend class QcMethodSignalHandler;
    friend class QcFunctionSignalHandler;

    Q_OBJECT

public:
    enum DestroyAction {
        DestroyProxy,
        DestroyObject,
        DestroyProxyAndObject,
    };

    struct EventHandlerData {
        EventHandlerData(): type(QEvent::None) {}
        int type;
        PyrSymbol* method;
        QtCollider::Synchronicity sync;
        bool enabled;
    };

    QObjectProxy(QObject* qObject, PyrObject* scObject);

    virtual ~QObjectProxy();

    // Check if this is the right thread.
    // WARNING: must be called with language locked!
    bool compareThread();

    // WARNING: must be called with language locked!
    void finalize() { _scObject = 0; }

    inline QObject* object() const { return qObject; }
    inline PyrObject* scObject() const { return _scObject; }

    // Lock for usage of object() outside Qt thread.
    inline void lock() { mutex.lock(); }
    inline void unlock() { mutex.unlock(); }

    QString scClassName() const { return _scClassName; }

    QList<PyrObject*> children(PyrSymbol* className);
    PyrObject* parent(PyrSymbol* className);
    virtual bool setParent(QObjectProxy* parent);

    bool setProperty(const char* property, const QVariant& val);
    QVariant property(const char* property);

    bool connectObject(const char* signal, PyrObject* object, Qt::ConnectionType);
    bool connectMethod(const char* signal, PyrSymbol* method, Qt::ConnectionType);
    bool disconnectObject(const char* signal, PyrObject* object);
    bool disconnectMethod(const char* signal, PyrSymbol* method);
    bool setEventHandler(int eventType, PyrSymbol* method, QtCollider::Synchronicity, bool enabled = true);
    bool setEventHandlerEnabled(int eventType, bool enabled);

    // thread-safe if connection == queued
    bool invokeMethod(const char* method, PyrSlot* ret, PyrSlot* arg, Qt::ConnectionType);

    void destroy(DestroyAction);

    static QObjectProxy* fromObject(QObject*);

protected:
    void invokeScMethod(PyrSymbol* method, const QList<QVariant>& args = QList<QVariant>(), PyrSlot* result = 0,
                        bool locked = false);

    virtual bool eventFilter(QObject* watched, QEvent* event);

    virtual void customEvent(QEvent*);

    virtual bool preProcessEvent(QObject*, QEvent*, EventHandlerData&, QList<QVariant>& args);

    virtual bool postProcessEvent(QObject*, QEvent*, bool handled) { return handled; }

    bool invokeEventHandler(QEvent* e, EventHandlerData&, QList<QVariant>& args);

    const QVector<EventHandlerData>& eventHandlers() { return _eventHandlers; }

private Q_SLOTS:

    void invalidate();

private:
    void scMethodCallEvent(QtCollider::ScMethodCallEvent*);
    bool setPropertyEvent(QtCollider::SetPropertyEvent*);
    bool destroyEvent(QtCollider::DestroyEvent*);

    QObject* qObject;
    // NOTE: scObject is protected by the language lock. Should not use it without it!
    PyrObject* _scObject;
    // NOTE: for the reason above we extract SC class name at construction
    QString _scClassName;

    QVector<EventHandlerData> _eventHandlers;
    QList<QcMethodSignalHandler*> methodSigHandlers;
    QList<QcFunctionSignalHandler*> funcSigHandlers;
    QMutex mutex;
};

namespace QtCollider {

struct SetPropertyEvent : public QEvent {
    SetPropertyEvent(): QEvent((QEvent::Type)QtCollider::Event_Proxy_SetProperty) {}
    PyrSymbol* property;
    QVariant value;
};

class DestroyEvent : public QEvent {
public:
    DestroyEvent(QObjectProxy::DestroyAction act):
        QEvent((QEvent::Type)QtCollider::Event_Proxy_Destroy),
        _action(act) {}
    QObjectProxy::DestroyAction action() { return _action; }

private:
    QObjectProxy::DestroyAction _action;
};

struct ScMethodCallEvent : public QEvent {
    ScMethodCallEvent(PyrSymbol* m, const QList<QVariant>& l = QList<QVariant>(), bool b_locked = false):
        QEvent((QEvent::Type)QtCollider::Event_ScMethodCall),
        method(m),
        args(l),
        locked(b_locked) {}

    PyrSymbol* method;
    QList<QVariant> args;
    bool locked;
};

} // namespace

Q_DECLARE_METATYPE(QObjectProxy*);
