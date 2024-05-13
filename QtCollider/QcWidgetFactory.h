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

#include "QcObjectFactory.h"
#include "QWidgetProxy.h"

#include <QMetaMethod>
#include <QLayout>

template <class QWIDGET> class QcWidgetFactory : public QcObjectFactory<QWIDGET> {
public:
    virtual QObjectProxy* newInstance(PyrObject* scObject, QtCollider::MetaValue arg[10]) {
        // Omit the first two arguments - parent and bounds

        QWIDGET* w;

        if (!arg[2].type()) {
            w = new QWIDGET;
        } else {
            QObject* obj = QWIDGET::staticMetaObject.newInstance(
                arg[2].toGenericArgument(), arg[3].toGenericArgument(), arg[4].toGenericArgument(),
                arg[5].toGenericArgument(), arg[6].toGenericArgument(), arg[7].toGenericArgument(),
                arg[8].toGenericArgument(), arg[9].toGenericArgument());

            w = qobject_cast<QWIDGET*>(obj);
            if (!w) {
                qcNoConstructorMsg(QcObjectFactory<QWIDGET>::metaObject(), 8, &arg[2]);
                return 0;
            }
        }

        // NOTE: performance: it is completely equal if parent is passed
        // in constructor, or set later, but for some reason it is cheaper
        // if it is set here, before setting other stuff like geometry, etc.

        QWidget* parent = arg[0].value<QWidget*>();

        if (parent) {
            const QMetaObject* mo = parent->metaObject();
            int mi = mo->indexOfMethod("addChild(QWidget*)");
            bool ok;
            if (mi >= 0) {
                QMetaMethod mm = mo->method(mi);
                ok = mm.invoke(parent, Q_ARG(QWidget*, w));
                if (!ok) {
                    qcErrorMsg("Could not set parent!");
                    delete w;
                    return 0;
                }
            } else {
                if (parent->layout())
                    parent->layout()->addWidget(w);
                else
                    w->setParent(parent);
            }
        }

        // set requested geometry

        QRect r(arg[1].value<QRectF>().toRect());
        if (r.size().isEmpty())
            r.setSize(w->sizeHint());

        w->setGeometry(r);

        // automatically support drag and drop

        w->setAcceptDrops(true);

        // ensure visible:

        if (parent && parent->isVisible())
            w->show();

        // create the proxy:

        QObjectProxy* prox(proxy(w, scObject));

        return prox;
    }

protected:
    virtual QObjectProxy* proxy(QWIDGET* w, PyrObject* sc_obj) {
        QWidgetProxy* prox(new QWidgetProxy(w, sc_obj));
        initialize(prox, w);
        return prox;
    }

    // avoid overload of virtual initialize( QObjectProxy *, QWIDGET * )
    using QcObjectFactory<QWIDGET>::initialize;

    virtual void initialize(QWidgetProxy* proxy, QWIDGET* obj) {};
};

#define QC_DECLARE_QWIDGET_FACTORY(QWIDGET) QC_DECLARE_FACTORY(QWIDGET, QcWidgetFactory<QWIDGET>)
