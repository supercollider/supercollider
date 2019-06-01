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

#include <QDebug>

#include "QObjectProxy.h"
#include "QcApplication.h"
#include "Common.h"
#include "QcSignalSpy.h"
#include "type_codec.hpp"
#include "metatype.hpp"

#include <QApplication>
#include <QWidget>
#include <QVarLengthArray>
#include <QThread>

#include <PyrKernel.h>
#include <VMGlobals.h>

#if defined _WIN32
#    include "SC_Win32Utils.h"
#elif defined __FreeBSD__
#    include <stdlib.h>
#else
#    include <alloca.h>
#endif

using namespace QtCollider;

void interpretMouseEvent(QEvent* e, QList<QVariant>& args);
void interpretKeyEvent(QEvent* e, QList<QVariant>& args);

QObjectProxy::QObjectProxy(QObject* qObject_, PyrObject* scObject_):
    qObject(qObject_),
    _scObject(scObject_),
    _scClassName(slotRawSymbol(&scObject_->classptr->name)->name) {
    ProxyToken* token = new ProxyToken(this, qObject);
    // WARNING: make sure the signal is already in normalized signature,
    // to avoid triggering very expensive normalization!
    connect(qObject, SIGNAL(destroyed(QObject*)), this, SLOT(invalidate()));
    qObject->installEventFilter(this);
    _eventHandlers.reserve(10);
}

QObjectProxy::~QObjectProxy() { qcProxyDebugMsg(1, QStringLiteral("Proxy is being deleted.")); }

bool QObjectProxy::compareThread() { return gMainVMGlobals->canCallOS; }

void QObjectProxy::invalidate() {
    qcProxyDebugMsg(1, QStringLiteral("Object has been deleted. Invalidating proxy."));
    mutex.lock();
    qObject = 0;
    mutex.unlock();
    QApplication::postEvent(this, new QEvent((QEvent::Type)QtCollider::Event_Proxy_Release));
}

static bool serializeSignature(QVarLengthArray<char, 512>& dst, const char* method, int argc, MetaValue* argv) {
    // serialize method name
    int len = qstrlen(method);
    if (len <= 0) {
        qcErrorMsg("Method name appears to be empty.");
        return false;
    }
    dst.append(method, len);
    dst.append('(');

    // serialize argument types
    int i;
    for (i = 0; i < argc; ++i) {
        int typeId = argv[i].type()->id();
        const char* typeName = QMetaType::typeName(typeId);
        int len = qstrlen(typeName);
        if (len <= 0) {
            qcErrorMsg("Could not get argument type name.");
            return false;
        }
        dst.append(typeName, len);
        dst.append(',');
    }

    // finalize the signature
    if (i == 0)
        dst.append(')');
    else
        dst[dst.size() - 1] = ')';

    dst.append('\0');

    return true;
}

bool QObjectProxy::invokeMethod(const char* method, PyrSlot* retSlot, PyrSlot* argSlot, Qt::ConnectionType ctype) {
    QMutexLocker locker(&mutex);

    if (!qObject)
        return true;

    MetaValue args[10];

    PyrSlot* argslots;
    int argc;

    if (isKindOfSlot(argSlot, class_array)) {
        argslots = slotRawObject(argSlot)->slots;
        argc = slotRawObject(argSlot)->size;
    } else if (argSlot && NotNil(argSlot)) {
        argslots = argSlot;
        argc = 1;
    } else {
        argslots = 0;
        argc = 0;
    }

    // translate args
    for (int i = 0; i < argc; ++i) {
        MetaType* type = MetaType::find(argslots + i);
        if (!type)
            return false;

        void* mem = alloca(type->size());
        if (!mem) {
            qcErrorMsg("Could not allocate stack space for argument!");
            return false;
        }

        args[i].read(mem, type, argslots + i);
    }

    // serialize signature
    QVarLengthArray<char, 512> sig;
    if (!serializeSignature(sig, method, argc, args))
        return false;

    // get the meta method
    const QMetaObject* mo = qObject->metaObject();

    int mi = mo->indexOfMethod(sig.constData());
    if (mi < 0) {
        QByteArray mnorm = QMetaObject::normalizedSignature(sig.constData());
        mi = mo->indexOfMethod(mnorm.constData());
    }

    if (mi < 0 || mi >= mo->methodCount()) {
        qcProxyDebugMsg(1, QStringLiteral("WARNING: No such method: %1::%2").arg(mo->className()).arg(sig.constData()));
        return false;
    }

    QMetaMethod mm = mo->method(mi);

    // construct the return data object
    QGenericReturnArgument rarg;
    const char* rtype_name = mm.typeName();
    int rtype_id = QMetaType::type(rtype_name);

    MetaValue returnVal;

    if (retSlot && rtype_id != QMetaType::Void) {
        MetaType* type = MetaType::find(rtype_id);
        if (!type) {
            qcErrorMsg(QStringLiteral("No translation for return type '%1'").arg(rtype_name));
            return false;
        }

        void* mem = alloca(type->size());
        if (!mem) {
            qcErrorMsg("Could not allocate stack space for return value");
            return false;
        }

        returnVal.read(mem, type, 0); // default construction
        rarg = returnVal.toGenericReturnArgument();
    }

    // do it!
    bool success = mm.invoke(qObject, ctype, rarg, args[0].toGenericArgument(), args[1].toGenericArgument(),
                             args[2].toGenericArgument(), args[3].toGenericArgument(), args[4].toGenericArgument(),
                             args[5].toGenericArgument(), args[6].toGenericArgument(), args[7].toGenericArgument(),
                             args[8].toGenericArgument(), args[9].toGenericArgument());

    // store the return data into the return slot
    if (success && retSlot)
        returnVal.write(retSlot);

    // done
    return success;
}

void QObjectProxy::invokeScMethod(PyrSymbol* method, const QList<QVariant>& args, PyrSlot* result, bool locked) {
    qcProxyDebugMsg(1, QStringLiteral("SC METHOD CALL [+++]: ") + QString(method->name));

    if (!locked) {
        QtCollider::lockLang();
    }

    if (_scObject) {
        QtCollider::runLang(_scObject, method, args, result);
    } else {
        if (result)
            SetNil(result);
        qcDebugMsg(1, "WARNING: no SC object");
    }

    if (!locked)
        QtCollider::unlockLang();

    qcProxyDebugMsg(1, QStringLiteral("SC METHOD CALL [---]: ") + QString(method->name));
}

void QObjectProxy::customEvent(QEvent* event) {
    switch ((int)event->type()) {
    case QtCollider::Event_ScMethodCall:
        scMethodCallEvent(static_cast<ScMethodCallEvent*>(event));
        return;
    case QtCollider::Event_Proxy_SetProperty:
        setPropertyEvent(static_cast<SetPropertyEvent*>(event));
        return;
    case QtCollider::Event_Proxy_Destroy:
        destroyEvent(static_cast<DestroyEvent*>(event));
        return;
    case QtCollider::Event_Proxy_Release:
        invokeScMethod(SC_SYM(prRelease));
        return;
    default:;
    }
}

bool QObjectProxy::setParent(QObjectProxy* parentProxy) {
    if (qObject && parentProxy->object())
        qObject->setParent(parentProxy->object());

    return true;
}

bool QObjectProxy::setProperty(const char* property, const QVariant& val) {
    if (!qObject)
        return true;
    if (!qObject->setProperty(property, val)) {
        qcProxyDebugMsg(1, QStringLiteral("WARNING: Property '%1' not found. Setting dynamic property.").arg(property));
    }
    return false;
}

bool QObjectProxy::setPropertyEvent(SetPropertyEvent* e) { return setProperty(e->property->name, e->value); }

QVariant QObjectProxy::property(const char* property) { return qObject ? qObject->property(property) : QVariant(); }

bool QObjectProxy::setEventHandler(int type, PyrSymbol* method, QtCollider::Synchronicity sync, bool enable) {
    EventHandlerData data;
    data.type = type;
    data.method = method;
    data.sync = sync;
    data.enabled = enable;

    EventHandlerData* d = _eventHandlers.data();
    int n = _eventHandlers.size();
    while (n--) {
        if (d->type == type) {
            *d = data;
            break;
        }
        ++d;
    }
    if (n < 0)
        _eventHandlers.append(data);

    return true;
}

bool QObjectProxy::setEventHandlerEnabled(int type, bool enabled) {
    EventHandlerData* d = _eventHandlers.data();
    int n = _eventHandlers.size();
    while (n--) {
        if (d->type == type) {
            d->enabled = enabled;
            break;
        }
        ++d;
    }

    return n >= 0;
}

bool QObjectProxy::connectObject(const char* signal, PyrObject* object, Qt::ConnectionType ctype) {
    if (!qObject)
        return true;

    QcFunctionSignalHandler* handler = new QcFunctionSignalHandler(this, signal, object, ctype);

    if (!handler->isValid()) {
        delete handler;
        return false;
    }

    funcSigHandlers.append(handler);

    return true;
}

bool QObjectProxy::connectMethod(const char* signal, PyrSymbol* method, Qt::ConnectionType ctype) {
    if (!qObject)
        return true;

    QcMethodSignalHandler* handler = new QcMethodSignalHandler(this, signal, method, ctype);

    if (handler->isValid()) {
        methodSigHandlers.append(handler);
        return true;
    } else {
        delete handler;
        return false;
    }
}

bool QObjectProxy::disconnectObject(const char* sig, PyrObject* object) {
    if (!qObject)
        return true;

    const QMetaObject* mo = qObject->metaObject();
    QByteArray signal = QMetaObject::normalizedSignature(sig);
    int sigId = mo->indexOfSignal(signal);
    if (sigId < 0) {
        qcDebugMsg(1, QStringLiteral("WARNING: No such signal: '%1'").arg(signal.constData()));
        return false;
    }

    for (int i = 0; i < funcSigHandlers.size(); ++i) {
        QcFunctionSignalHandler* h = funcSigHandlers[i];
        if (h->indexOfSignal() == sigId && h->function() == object) {
            funcSigHandlers.removeAt(i);
            delete h;
            return true;
        }
    }

    return false;
}

bool QObjectProxy::disconnectMethod(const char* sig, PyrSymbol* method) {
    if (!qObject)
        return true;

    const QMetaObject* mo = qObject->metaObject();
    QByteArray signal = QMetaObject::normalizedSignature(sig);
    int sigId = mo->indexOfSignal(signal);
    if (sigId < 0) {
        qcDebugMsg(1, QStringLiteral("WARNING: No such signal: '%1'").arg(signal.constData()));
        return false;
    }

    for (int i = 0; i < methodSigHandlers.size(); ++i) {
        QcMethodSignalHandler* h = methodSigHandlers[i];
        if (h->indexOfSignal() == sigId && h->method() == method) {
            methodSigHandlers.removeAt(i);
            delete h;
            return true;
        }
    }

    return false;
}

void QObjectProxy::destroy(DestroyAction action) {
    switch (action) {
    case DestroyObject:
        delete qObject;
        return;
    case DestroyProxy:
        delete this;
        return;
    case DestroyProxyAndObject:
        delete qObject;
        delete this;
        return;
    }
}

bool QObjectProxy::destroyEvent(DestroyEvent* e) {
    destroy(e->action());
    return true;
}

QList<PyrObject*> QObjectProxy::children(PyrSymbol* className) {
    QList<PyrObject*> scChildren;

    if (!qObject)
        return scChildren;

    const QObjectList& children = qObject->children();

    Q_FOREACH (QObject* child, children) {
        QObjectProxy* proxy = QObjectProxy::fromObject(child);
        if (!proxy)
            continue;

        PyrObject* obj = proxy->_scObject;

        if (obj) {
            if (className && !isKindOf(obj, className->u.classobj))
                continue;
            scChildren.append(obj);
        }
    }

    return scChildren;
}

PyrObject* QObjectProxy::parent(PyrSymbol* className) {
    if (!qObject)
        return 0;

    QObject* parent = qObject->parent();

    while (parent) {
        // see if this parent has a corresponding proxy
        QObjectProxy* proxy = QObjectProxy::fromObject(parent);
        if (proxy) {
            // if parent does not have a corresponding SC object (it is just
            // being deleted) return no parent;
            PyrObject* scobj = proxy->_scObject;
            if (!scobj)
                return 0;

            // if parent SC object is of desired class (or no class specified)
            // return it, else continue
            if (!className || isKindOf(scobj, className->u.classobj)) {
                return scobj;
            }
        }

        // if this parent was not appropriate continue to consider the parent's parent
        parent = parent->parent();
    }

    return 0;
}

bool QObjectProxy::eventFilter(QObject* watched, QEvent* event) {
    int type = event->type();

    EventHandlerData* d = _eventHandlers.data();
    int n = _eventHandlers.size();
    while (n--) {
        if (d->type == type)
            break;
        ++d;
    }
    if (n < 0) {
        qcProxyDebugMsg(3, QStringLiteral("No handler for event (%1), forwarding to the widget").arg(type));
        return false;
    }

    QList<QVariant> args;

    if (!preProcessEvent(watched, event, *d, args)) {
        qcProxyDebugMsg(3,
                        QStringLiteral("Event (%1, %2) not handled, forwarding to the widget")
                            .arg(type)
                            .arg(event->spontaneous() ? "spontaneous" : "inspontaneous"));
        return false;
    }

    qcProxyDebugMsg(1,
                    QStringLiteral("Will handle event (%1, %2) -> (%3, %4)")
                        .arg(type)
                        .arg(event->spontaneous() ? "spontaneous" : "inspontaneous")
                        .arg(d->method->name)
                        .arg(d->sync == Synchronous ? "sync" : "async"));

    bool eventHandled = invokeEventHandler(event, *d, args);

    eventHandled = postProcessEvent(watched, event, eventHandled);

    return eventHandled;
}

bool QObjectProxy::invokeEventHandler(QEvent* event, EventHandlerData& eh, QList<QVariant>& args) {
    PyrSymbol* method = eh.method;

    if (eh.sync == Synchronous) {
        PyrSlot result;
        invokeScMethod(method, args, &result);
        if (IsTrue(&result)) {
            qcProxyDebugMsg(2, "Event accepted");
            event->accept();
            return true;
        } else if (IsFalse(&result)) {
            qcProxyDebugMsg(2, "Event ignored");
            event->ignore();
            return true;
        }
    } else {
        ScMethodCallEvent* e = new ScMethodCallEvent(method, args);
        QApplication::postEvent(this, e);
    }

    qcProxyDebugMsg(2, "Forwarding event to the system");
    return false;
}

bool QObjectProxy::preProcessEvent(QObject*, QEvent* e, EventHandlerData& eh, QList<QVariant>& args) {
    return eh.enabled;
}

inline void QObjectProxy::scMethodCallEvent(ScMethodCallEvent* e) { invokeScMethod(e->method, e->args, 0, e->locked); }

QObjectProxy* QObjectProxy::fromObject(QObject* object) {
    if (!object)
        return 0;

    const QObjectList& children = object->children();
    Q_FOREACH (QObject* child, children) {
        ProxyToken* token = qobject_cast<QtCollider::ProxyToken*>(child);
        if (token)
            return token->proxy;
    }
    return 0;
}
