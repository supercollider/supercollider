/************************************************************************
 *
 * Copyright 2010-2012 Jakob Leben (jakob.leben@gmail.com)
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

#include "primitives.h"
#include "../QObjectProxy.h"
#include "../QcObjectFactory.h"
#include "../QcApplication.h"
#include "../Common.h"
#include "../type_codec.hpp"
#include "../metatype.hpp"
#include "../painting.h"

#include <PyrObject.h>
#include <PyrKernel.h>
#include <GC.h>
#include <SCBase.h>

#include <QMetaObject>
#include <QMetaProperty>
#include <QMetaMethod>

#if defined _WIN32
#    include "SC_Win32Utils.h"
#elif defined __FreeBSD__
#    include <stdlib.h>
#else
#    include <alloca.h>
#endif

#define IS_OBJECT_NIL(a) IsNil(slotRawObject(a)->slots)

#define QOBJECT_FROM_SLOT(s) ((QObjectProxy*)slotRawPtr(slotRawObject(s)->slots))

#define CLASS_NAME(slot) slotRawSymbol(&slotRawObject(slot)->classptr->name)->name

namespace QtCollider {

int QObject_Finalize(struct VMGlobals*, struct PyrObject*);

QC_LANG_PRIMITIVE(QObject_New, 2, PyrSlot* r, PyrSlot* a, VMGlobals* g) {
    if (NotSym(a + 0))
        return errWrongType;

    PyrObject* scObject = slotRawObject(r);
    QString qtClassName = QString(slotRawSymbol(a + 0)->name);

    qcSCObjectDebugMsg(1, scObject, QStringLiteral("CREATE: %2").arg(qtClassName));

    if (!QcApplication::compareThread())
        return QtCollider::wrongThreadError();

    QcAbstractFactory* f = QtCollider::factories().value(qtClassName);

    if (!f) {
        qcErrorMsg(QStringLiteral("Factory for class '%1' not found!").arg(qtClassName));
        return errFailed;
    }

    QObjectProxy* proxy = 0;

    MetaValue args[10];

    PyrSlot* argSlot = a + 1;
    int argc;

    if (isKindOfSlot(argSlot, class_array)) {
        PyrObject* array = slotRawObject(argSlot);
        argSlot = array->slots;
        argc = array->size;
    } else {
        argc = 1;
    }

    for (int i = 0; i < argc && i < 10; ++i) {
        PyrSlot* slot = argSlot + i;
        MetaType* type = MetaType::find(slot);
        if (type) {
            void* mem = alloca(type->size());
            Q_ASSERT(mem);
            args[i].read(mem, type, slot);
        }
    }

    proxy = f->newInstance(scObject, args);
    if (!proxy)
        return errFailed;

    SetPtr(scObject->slots, proxy);

    InstallFinalizer(g, scObject, 1, QObject_Finalize);

    return errNone;
}

QC_LANG_PRIMITIVE(QObject_Destroy, 0, PyrSlot* r, PyrSlot* a, VMGlobals* g) {
    qcSCObjectDebugMsg(1, slotRawObject(r), "DESTROY");

    QObjectProxy* proxy = QOBJECT_FROM_SLOT(r);

    if (!proxy->compareThread())
        return QtCollider::wrongThreadError();

    // Post the destruction event asynchronously;
    DestroyEvent* e = new DestroyEvent(QObjectProxy::DestroyObject);
    QApplication::postEvent(proxy, e);

    return errNone;
}

int QObject_Finalize(struct VMGlobals*, struct PyrObject* obj) {
    qcSCObjectDebugMsg(1, obj, "FINALIZE");

    QObjectProxy* proxy = (QObjectProxy*)slotRawPtr(obj->slots);

    // Invalidate proxy's SC object pointer directly.
    // Note that it is protected by language mutex;
    proxy->finalize();

    // Post the destruction event asynchronously;
    DestroyEvent* e = new DestroyEvent(QObjectProxy::DestroyProxyAndObject);
    QApplication::postEvent(proxy, e);

    return errNone;
}

QC_LANG_PRIMITIVE(QObject_SetParent, 1, PyrSlot* r, PyrSlot* a, VMGlobals* g) {
    QObjectProxy* proxy = QOBJECT_FROM_SLOT(r);

    QObjectProxy* parent = QtCollider::get(a);
    if (!parent)
        return errWrongType;

    qcSCObjectDebugMsg(1, slotRawObject(r), "SET PARENT");

    if (!proxy->compareThread())
        return QtCollider::wrongThreadError();

    bool ok = proxy->setParent(parent);

    return ok ? errNone : errFailed;
}

static void qcGetProperties(const QMetaObject* mo, PyrSlot* r, VMGlobals* g) {
    int count = mo->propertyCount();

    PyrObject* array = newPyrArray(g->gc, count, 0, true);
    SetObject(r, array);

    PyrSlot* s = array->slots;
    for (int i = 0; i < count; ++i, ++s) {
        SetSymbol(s, getsym(mo->property(i).name()));
        array->size++;
    }
}

static void qcGetMethods(const QMetaObject* mo, bool getPlain, bool getSignals, bool getSlots, PyrSlot* r,
                         VMGlobals* g) {
    int count = mo->methodCount();

    PyrObject* array = newPyrArray(g->gc, count, 0, true);
    SetObject(r, array);

    PyrSlot* s = array->slots;
    for (int i = 0; i < count; ++i) {
        QMetaMethod method = mo->method(i);
        switch (method.methodType()) {
        case QMetaMethod::Method:
            if (!getPlain || (method.access() != QMetaMethod::Public))
                continue;
            break;
        case QMetaMethod::Signal:
            if (!getSignals)
                continue;
            break;
        case QMetaMethod::Slot:
            if (!getSlots || (method.access() != QMetaMethod::Public))
                continue;
            break;
        default:
            continue;
        }
        QtCollider::set(s, QString::fromLatin1(method.methodSignature()));
        array->size++;
        g->gc->GCWrite(array, s);
        ++s;
    }
}

QC_LANG_PRIMITIVE(QMetaObject_Properties, 0, PyrSlot* r, PyrSlot* a, VMGlobals* g) {
    if (!QcApplication::compareThread())
        return QtCollider::wrongThreadError();

    PyrSlot* sClassName = slotRawObject(r)->slots + 0;
    if (NotSym(sClassName))
        return errWrongType;
    QString className(slotRawSymbol(sClassName)->name);

    QcAbstractFactory* f = QtCollider::factories().value(className);

    if (!f) {
        qcErrorMsg(QStringLiteral("Factory for class '%1' not found!").arg(className));
        return errFailed;
    }

    const QMetaObject* mo = f->metaObject();
    qcGetProperties(mo, r, g);

    return errNone;
}

QC_LANG_PRIMITIVE(QMetaObject_Methods, 3, PyrSlot* r, PyrSlot* a, VMGlobals* g) {
    if (!QcApplication::compareThread())
        return QtCollider::wrongThreadError();

    PyrSlot* sClassName = slotRawObject(r)->slots + 0;
    if (NotSym(sClassName))
        return errWrongType;
    QString className(slotRawSymbol(sClassName)->name);

    QcAbstractFactory* f = QtCollider::factories().value(className);

    if (!f) {
        qcErrorMsg(QStringLiteral("Factory for class '%1' not found!").arg(className));
        return errFailed;
    }

    bool getPlain = IsTrue(a + 0);
    bool getSignals = IsTrue(a + 1);
    bool getSlots = IsTrue(a + 2);

    const QMetaObject* mo = f->metaObject();
    qcGetMethods(mo, getPlain, getSignals, getSlots, r, g);

    return errNone;
}

QC_LANG_PRIMITIVE(QObject_GetProperties, 0, PyrSlot* r, PyrSlot* a, VMGlobals* g) {
    QObjectProxy* proxy = QOBJECT_FROM_SLOT(r);
    if (!proxy->compareThread())
        return QtCollider::wrongThreadError();

    QObject* obj = proxy->object();
    if (!obj) {
        SetNil(r);
        return errNone;
    }

    const QMetaObject* mo = obj->metaObject();
    qcGetProperties(mo, r, g);

    return errNone;
}

QC_LANG_PRIMITIVE(QObject_GetMethods, 3, PyrSlot* r, PyrSlot* a, VMGlobals* g) {
    QObjectProxy* proxy = QOBJECT_FROM_SLOT(r);
    if (!proxy->compareThread())
        return QtCollider::wrongThreadError();

    QObject* obj = proxy->object();
    if (!obj) {
        SetNil(r);
        return errNone;
    }

    bool getPlain = IsTrue(a + 0);
    bool getSignals = IsTrue(a + 1);
    bool getSlots = IsTrue(a + 2);

    const QMetaObject* mo = obj->metaObject();
    qcGetMethods(mo, getPlain, getSignals, getSlots, r, g);

    return errNone;
}

QC_LANG_PRIMITIVE(QObject_SetProperty, 3, PyrSlot* r, PyrSlot* a, VMGlobals* g) {
    QObjectProxy* proxy = QOBJECT_FROM_SLOT(r);

    if (NotSym(a))
        return errWrongType;
    PyrSymbol* property = slotRawSymbol(a);
    bool sync = IsTrue(a + 2);

    qcSCObjectDebugMsg(1, slotRawObject(r), QStringLiteral("SET: %1").arg(property->name));

    if (!proxy->compareThread())
        return QtCollider::wrongThreadError();

    QVariant val = QtCollider::get(a + 1);

    if (sync && !QtCollider::isPaintingObject(proxy)) {
        proxy->setProperty(property->name, val);
    } else {
        SetPropertyEvent* e = new SetPropertyEvent();
        e->property = property;
        e->value = val;
        QApplication::postEvent(proxy, e);
    }

    return errNone;
}

QC_LANG_PRIMITIVE(QObject_GetProperty, 1, PyrSlot* r, PyrSlot* a, VMGlobals* g) {
    QObjectProxy* proxy = QOBJECT_FROM_SLOT(r);

    if (NotSym(a))
        return errWrongType;
    PyrSymbol* symProp = slotRawSymbol(a);

    qcSCObjectDebugMsg(1, slotRawObject(r), QStringLiteral("GET: %1").arg(symProp->name));

    if (!proxy->compareThread())
        return QtCollider::wrongThreadError();

    QVariant val = proxy->property(symProp->name);
    if (!val.isValid()) {
        qcDebugMsg(1, QStringLiteral("WARNING: Invalid property '%1'").arg(symProp->name));
        SetNil(r);
        return errNone;
    }

    if (QtCollider::set(r, val))
        return errNone;
    else
        return errFailed;
}

QC_LANG_PRIMITIVE(QObject_SetEventHandler, 4, PyrSlot* r, PyrSlot* a, VMGlobals* g) {
    QObjectProxy* proxy = QOBJECT_FROM_SLOT(r);

    if (NotInt(a + 0) || NotSym(a + 1))
        return errWrongType;
    int eventType = QtCollider::get(a + 0);
    PyrSymbol* method = 0;
    slotSymbolVal(a + 1, &method);
    Synchronicity sync = IsTrue(a + 2) ? Synchronous : Asynchronous;
    bool enabled = IsTrue(a + 3);

    qcSCObjectDebugMsg(1, slotRawObject(r),
                       QStringLiteral("SET EVENT HANDLER: type %1 -> %2 [%3, %4]")
                           .arg(eventType)
                           .arg(method->name)
                           .arg(sync == Synchronous ? "SYNC" : "ASYNC")
                           .arg(enabled ? "on" : "off"));

    if (!proxy->compareThread())
        return QtCollider::wrongThreadError();

    bool ok = proxy->setEventHandler(eventType, method, sync, enabled);

    return ok ? errNone : errFailed;
}

QC_LANG_PRIMITIVE(QObject_SetEventHandlerEnabled, 2, PyrSlot* r, PyrSlot* a, VMGlobals* g) {
    if (NotInt(a + 0))
        return errWrongType;
    bool enabled = IsTrue(a + 1);
    if (!enabled && !IsFalse(a + 1))
        return errWrongType;
    int type = QtCollider::get(a + 0);

    qcSCObjectDebugMsg(1, slotRawObject(r),
                       QStringLiteral("SET EVENT HANDLER STATE: type %1 = %2").arg(type).arg(enabled ? "on" : "off"));

    QObjectProxy* proxy = QOBJECT_FROM_SLOT(r);

    if (!proxy->compareThread())
        return QtCollider::wrongThreadError();

    bool ok = proxy->setEventHandlerEnabled(type, enabled);

    return ok ? errNone : errFailed;
}

QC_LANG_PRIMITIVE(QObject_ConnectMethod, 3, PyrSlot* r, PyrSlot* a, VMGlobals* g) {
    if (NotSym(a + 0) || NotSym(a + 1))
        return errWrongType;

    PyrSymbol* signal = slotRawSymbol(a + 0);
    PyrSymbol* handler = slotRawSymbol(a + 1);
    Qt::ConnectionType ctype = IsTrue(a + 2) ? Qt::DirectConnection : Qt::QueuedConnection;

    qcSCObjectDebugMsg(1, slotRawObject(r),
                       QStringLiteral("CONNECT METHOD: %1 -> %2 [%3]")
                           .arg(signal->name)
                           .arg(handler->name)
                           .arg(IsTrue(a + 2) ? "SYNC" : "ASYNC"));

    QObjectProxy* proxy = QOBJECT_FROM_SLOT(r);

    if (!proxy->compareThread())
        return QtCollider::wrongThreadError();

    bool ok = proxy->connectMethod(signal->name, handler, ctype);

    return ok ? errNone : errFailed;
}

QC_LANG_PRIMITIVE(QObject_DisconnectMethod, 2, PyrSlot* r, PyrSlot* a, VMGlobals* g) {
    if (NotSym(a + 0) || NotSym(a + 1))
        return errWrongType;

    PyrSymbol* signal = slotRawSymbol(a + 0);
    PyrSymbol* handler = slotRawSymbol(a + 1);

    qcSCObjectDebugMsg(1, slotRawObject(r),
                       QStringLiteral("DISCONNECT METHOD: %1 -> %2").arg(signal->name).arg(handler->name));

    QObjectProxy* proxy = QOBJECT_FROM_SLOT(r);

    if (!proxy->compareThread())
        return QtCollider::wrongThreadError();

    bool ok = proxy->disconnectMethod(signal->name, handler);

    return ok ? errNone : errFailed;
}

QC_LANG_PRIMITIVE(QObject_ConnectObject, 3, PyrSlot* r, PyrSlot* a, VMGlobals* g) {
    if (NotSym(a + 0) || NotObj(a + 1))
        return errWrongType;

    PyrSymbol* signal = slotRawSymbol(a + 0);
    PyrObject* handlerObj = slotRawObject(a + 1);
    Qt::ConnectionType ctype = IsTrue(a + 2) ? Qt::DirectConnection : Qt::QueuedConnection;

    qcSCObjectDebugMsg(1, slotRawObject(r),
                       QStringLiteral("CONNECT OBJECT: %1 -> %2 [%3]")
                           .arg(signal->name)
                           .arg(slotRawSymbol(&handlerObj->classptr->name)->name)
                           .arg(IsTrue(a + 2) ? "SYNC" : "ASYNC"));

    QObjectProxy* proxy = QOBJECT_FROM_SLOT(r);

    if (!proxy->compareThread())
        return QtCollider::wrongThreadError();

    bool ok = proxy->connectObject(signal->name, handlerObj, ctype);

    return ok ? errNone : errFailed;
}

QC_LANG_PRIMITIVE(QObject_DisconnectObject, 2, PyrSlot* r, PyrSlot* a, VMGlobals* g) {
    if (NotSym(a + 0) || NotObj(a + 1))
        return errWrongType;

    PyrSymbol* signal = slotRawSymbol(a + 0);
    PyrObject* handlerObj = slotRawObject(a + 1);

    qcSCObjectDebugMsg(1, slotRawObject(r), QStringLiteral("DISCONNECT OBJECT: %1").arg(signal->name));

    QObjectProxy* proxy = QOBJECT_FROM_SLOT(r);

    if (!proxy->compareThread())
        return QtCollider::wrongThreadError();

    bool ok = proxy->disconnectObject(signal->name, handlerObj);

    return ok ? errNone : errFailed;
}

QC_LANG_PRIMITIVE(QObject_ConnectSlot, 3, PyrSlot* r, PyrSlot* a, VMGlobals* g) {
    // Args: signal, receiver, slot
    if (!isKindOfSlot(a + 1, SC_CLASS(QObject)) || NotSym(a + 0) || NotSym(a + 2))
        return errWrongType;

    PyrSymbol* symSig = slotRawSymbol(a + 0);
    PyrSymbol* symSlot = slotRawSymbol(a + 2);
    QObjectProxy* sndProxy = QOBJECT_FROM_SLOT(r);
    QObjectProxy* rcvProxy = QOBJECT_FROM_SLOT(a + 1);

    qcSCObjectDebugMsg(1, slotRawObject(r),
                       QStringLiteral("CONNECT TO SLOT: %1 -> %2").arg(symSig->name).arg(symSlot->name));

    QString strSig = QStringLiteral("2") + symSig->name;
    QString strSlot = QStringLiteral("1") + symSlot->name;

    sndProxy->lock();
    rcvProxy->lock();
    bool ok;
    if (!sndProxy->object() || !rcvProxy->object()) {
        ok = true;
    } else {
        ok = QObject::connect(sndProxy->object(), strSig.toStdString().c_str(), rcvProxy->object(),
                              strSlot.toStdString().c_str());
    }
    sndProxy->unlock();
    rcvProxy->unlock();

    if (!ok) {
        qcErrorMsg(QStringLiteral("Failed to connect %1::%2 to %3::%4!\n")
                       .arg(sndProxy->scClassName())
                       .arg(symSig->name)
                       .arg(rcvProxy->scClassName())
                       .arg(symSlot->name));
        return errFailed;
    }

    return errNone;
}

QC_LANG_PRIMITIVE(QObject_InvokeMethod, 3, PyrSlot* r, PyrSlot* a, VMGlobals* g) {
    if (NotSym(a + 0))
        return errWrongType;

    PyrSymbol* method = slotRawSymbol(a + 0);
    PyrSlot* methodArgs = a + 1;
    bool sync = !IsFalse(a + 2);

    qcSCObjectDebugMsg(1, slotRawObject(r),
                       QStringLiteral("INVOKE: '%1' [%2]").arg(method->name).arg(sync ? "SYNC" : "ASYNC"));

    QObjectProxy* proxy = QOBJECT_FROM_SLOT(r);

    PyrSlot* retSlot;
    Qt::ConnectionType cType;

    if (sync) {
        if (!proxy->compareThread())
            return QtCollider::wrongThreadError();
        retSlot = r;
        cType = Qt::DirectConnection;
    } else {
        retSlot = 0;
        cType = Qt::QueuedConnection;
    }

    bool ok = proxy->invokeMethod(method->name, retSlot, methodArgs, cType);

    return ok ? errNone : errFailed;
}

QC_LANG_PRIMITIVE(QObject_IsValid, 0, PyrSlot* r, PyrSlot* a, VMGlobals* g) {
    QObjectProxy* proxy = QOBJECT_FROM_SLOT(r);

    bool needLock = !proxy->compareThread();

    if (needLock)
        proxy->lock();

    bool hasObject = proxy->object() != 0;

    if (needLock)
        proxy->unlock();

    SetBool(r, hasObject);

    return errNone;
}

QC_LANG_PRIMITIVE(QObject_GetChildren, 1, PyrSlot* r, PyrSlot* a, VMGlobals* g) {
    if (NotSym(a) && NotNil(a))
        return errWrongType;

    QObjectProxy* proxy = QOBJECT_FROM_SLOT(r);
    PyrSymbol* className = IsSym(a) ? slotRawSymbol(a) : 0;

    qcSCObjectDebugMsg(1, slotRawObject(r),
                       QStringLiteral("GET CHILDREN: of class '%1'").arg(className ? className->name : "QObject"));

    if (!proxy->compareThread())
        return QtCollider::wrongThreadError();

    QList<PyrObject*> children = proxy->children(className);

    int count = children.count();
    PyrObject* array = newPyrArray(g->gc, count, 0, true);
    SetObject(r, array);

    PyrSlot* s = array->slots;
    Q_FOREACH (PyrObject* obj, children) {
        SetObject(s, obj);
        ++array->size;
        g->gc->GCWrite(array, s);
        ++s;
    }

    return errNone;
}

QC_LANG_PRIMITIVE(QObject_GetParent, 1, PyrSlot* r, PyrSlot* a, VMGlobals* g) {
    if (NotSym(a) && NotNil(a))
        return errWrongType;

    QObjectProxy* proxy = QOBJECT_FROM_SLOT(r);
    PyrSymbol* className = IsSym(a) ? slotRawSymbol(a) : 0;

    qcSCObjectDebugMsg(1, slotRawObject(r), QStringLiteral("GET PARENT"));

    if (!proxy->compareThread())
        return QtCollider::wrongThreadError();

    PyrObject* parent = proxy->parent(className);

    if (parent)
        SetObject(r, parent);
    else
        SetNil(r);

    return errNone;
}

void defineQObjectPrimitives() {
    LangPrimitiveDefiner definer;
    definer.define<QObject_New>();
    definer.define<QObject_Destroy>();
    definer.define<QObject_SetParent>();
    definer.define<QMetaObject_Properties>();
    definer.define<QMetaObject_Methods>();
    definer.define<QObject_GetProperties>();
    definer.define<QObject_GetMethods>();
    definer.define<QObject_SetProperty>();
    definer.define<QObject_GetProperty>();
    definer.define<QObject_SetEventHandler>();
    definer.define<QObject_SetEventHandlerEnabled>();
    definer.define<QObject_ConnectMethod>();
    definer.define<QObject_DisconnectMethod>();
    definer.define<QObject_ConnectObject>();
    definer.define<QObject_DisconnectObject>();
    definer.define<QObject_ConnectSlot>();
    definer.define<QObject_InvokeMethod>();
    definer.define<QObject_IsValid>();
    definer.define<QObject_GetChildren>();
    definer.define<QObject_GetParent>();
}

} // namespace QtCollider
