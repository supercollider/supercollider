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

#pragma once

#include "QObjectProxy.h"
#include "Common.h"
#include "metatype.hpp"

#include <PyrObject.h>

#include <QMetaObject>
#include <QObject>
#include <QWidget>
#include <QHash>

class QcAbstractFactory;

typedef QHash<QString, QcAbstractFactory*> QcFactoryHash;

namespace QtCollider {
QcFactoryHash& factories();
}

class QcAbstractFactory {
public:
    virtual const QMetaObject* metaObject() = 0;
    virtual QObjectProxy* newInstance(PyrObject*, QtCollider::MetaValue arg[10]) = 0;
};

static void qcNoConstructorMsg(const QMetaObject* metaObject, int argc, QtCollider::MetaValue* argv) {
    QString str = QStringLiteral("No appropriate constructor found for %1 (").arg(metaObject->className());

    for (int i = 0; i < argc; ++i) {
        MetaType* type = argv[i].type();
        if (type) {
            if (i > 0)
                str += ", ";
            str += QMetaType::typeName(type->id());
        } else
            break;
    }

    str += ")";

    qcErrorMsg(str);
}

template <class QOBJECT> class QcObjectFactory : public QcAbstractFactory {
public:
    const QMetaObject* metaObject() { return &QOBJECT::staticMetaObject; }

    virtual QObjectProxy* newInstance(PyrObject* scObject, QtCollider::MetaValue arg[10]) {
        QOBJECT* qObject;

        if (!arg[0].type()) {
            qObject = new QOBJECT;
        } else {
            QObject* obj = QOBJECT::staticMetaObject.newInstance(
                arg[0].toGenericArgument(), arg[1].toGenericArgument(), arg[2].toGenericArgument(),
                arg[3].toGenericArgument(), arg[4].toGenericArgument(), arg[5].toGenericArgument(),
                arg[6].toGenericArgument(), arg[7].toGenericArgument(), arg[8].toGenericArgument(),
                arg[9].toGenericArgument());

            qObject = qobject_cast<QOBJECT*>(obj);
            if (!qObject) {
                qcNoConstructorMsg(metaObject(), 10, arg);
                return 0;
            }
        }

        return proxy(qObject, scObject);
    }

protected:
    virtual QObjectProxy* proxy(QOBJECT* obj, PyrObject* sc_obj) {
        QObjectProxy* prox(new QObjectProxy(obj, sc_obj));
        initialize(prox, obj);
        return prox;
    }

    virtual void initialize(QObjectProxy* proxy, QOBJECT* obj) {};
};

#define QC_DECLARE_FACTORY(QOBJECT, FACTORY)                                                                           \
    void add_factory_##QOBJECT() {                                                                                     \
        QcAbstractFactory* factory = new FACTORY;                                                                      \
        factories().insert(factory->metaObject()->className(), factory);                                               \
    }

#define QC_DECLARE_QOBJECT_FACTORY(QOBJECT) QC_DECLARE_FACTORY(QOBJECT, QcObjectFactory<QOBJECT>)

#define QC_ADD_FACTORY(QOBJECT)                                                                                        \
    void add_factory_##QOBJECT();                                                                                      \
    add_factory_##QOBJECT()
