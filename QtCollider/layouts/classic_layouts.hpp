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

#include <QWidget>
#include <QResizeEvent>
#include "../Common.h"

namespace QtCollider {

enum HSizePolicy { StickLeft = 1, HStretch, StickRight };

enum VSizePolicy { StickTop = 1, VStretch, StickBottom };

static VSizePolicy vSizePolicy(QWidget* w) {
    QVariant var = w->property("_qc_vSizePolicy");
    if (!var.isValid())
        return StickTop;
    return (VSizePolicy)var.toInt();
}

static HSizePolicy hSizePolicy(QWidget* w) {
    QVariant var = w->property("_qc_hSizePolicy");
    if (!var.isValid())
        return StickLeft;
    return (HSizePolicy)var.toInt();
}


class DefaultLayout {
public:
    DefaultLayout(QWidget* w): parent(w), initialized(false) {}

    void resize(QResizeEvent* e) {
        QRect r;
        r.setSize(e->size());

        if (!initialized) {
            initialized = true;
            geom = r;
            return;
        }

        QPoint dPos = r.topLeft() - geom.topLeft();
        QSize dSize = r.size() - geom.size();

        const QObjectList& children = parent->children();

        Q_FOREACH (QObject* o, children) {
            if (!o->isWidgetType())
                continue;

            QWidget* child = static_cast<QWidget*>(o);

            QRect g = child->geometry();
            int x = g.x();
            int y = g.y();
            int w = g.width();
            int h = g.height();

            if (!dPos.isNull()) {
                x += dPos.x();
                y += dPos.y();
            }

            if (!dSize.isNull()) {
                if (hSizePolicy(child) == QtCollider::StickRight)
                    x += dSize.width();
                if (vSizePolicy(child) == QtCollider::StickBottom)
                    y += dSize.height();
                if (hSizePolicy(child) == QtCollider::HStretch)
                    w += dSize.width();
                if (vSizePolicy(child) == QtCollider::VStretch)
                    h += dSize.height();
            }

            child->setGeometry(QRect(x, y, w, h));
        }

        geom = r;
    }

private:
    QWidget* parent;
    bool initialized;
    QRect geom;
};

class HLayout {
public:
    HLayout(QWidget* w): parent(w) {}

    void resize(QResizeEvent* e) {
        QRect geom;
        geom.setSize(e->size());

        const QObjectList& children = parent->children();
        int varWidth = geom.width();
        int i = 0;

        Q_FOREACH (QObject* o, children) {
            if (!o->isWidgetType())
                continue;
            QWidget* w = static_cast<QWidget*>(o);

            if (hSizePolicy(w) == QtCollider::HStretch) {
                ++i;
            } else {
                QRect r = w->geometry();
                varWidth -= r.width();
                if (varWidth < 0)
                    break;
            }
        }

        int partWidth = i > 0 && varWidth > 0 ? varWidth / i : 0;
        int x = 0;

        Q_FOREACH (QObject* o, children) {
            if (!o->isWidgetType())
                continue;
            QWidget* w = static_cast<QWidget*>(o);

            QRect r = w->geometry();
            r.setHeight(geom.height());
            r.moveTo(x, geom.top());
            if (hSizePolicy(w) == QtCollider::HStretch)
                r.setWidth(partWidth);
            x += r.width();

            w->setGeometry(r);
        }
    }

private:
    QWidget* parent;
};

class VLayout {
public:
    VLayout(QWidget* w): parent(w) {}

    void resize(QResizeEvent* e) {
        QRect geom;
        geom.setSize(e->size());

        const QObjectList& children = parent->children();
        int varHeight = geom.height();
        int i = 0;

        Q_FOREACH (QObject* o, children) {
            if (!o->isWidgetType())
                continue;
            QWidget* w = static_cast<QWidget*>(o);

            if (vSizePolicy(w) == QtCollider::VStretch) {
                ++i;
            } else {
                QRect r = w->geometry();
                varHeight -= r.height();
                if (varHeight < 0)
                    break;
            }
        }

        int partHeight = i > 0 && varHeight > 0 ? varHeight / i : 0;
        int y = 0;

        Q_FOREACH (QObject* o, children) {
            if (!o->isWidgetType())
                continue;
            QWidget* w = static_cast<QWidget*>(o);

            QRect r = w->geometry();
            r.setWidth(geom.width());
            r.moveTo(geom.left(), y);
            if (vSizePolicy(w) == QtCollider::VStretch)
                r.setHeight(partHeight);
            y += r.height();

            w->setGeometry(r);
        }
    }

private:
    QWidget* parent;
};

} // namespace QtCollider
