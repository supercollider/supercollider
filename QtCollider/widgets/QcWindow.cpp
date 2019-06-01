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

#include "QcWindow.h"
#include "../QcWidgetFactory.h"
#include "../QWidgetProxy.h"

#include <QShortcut>
#include <QApplication>
#include <QDesktopWidget>

class QcWindowFactory : public QcObjectFactory<QcWindow> {
    // NOTE: use basic object contruction, but return widget proxy
public:
    virtual QObjectProxy* proxy(QcWindow* obj, PyrObject* sc_obj) {
        QObjectProxy* proxy = new QWidgetProxy(obj, sc_obj);
        QObject::connect(obj, SIGNAL(painting(QPainter*)), proxy, SLOT(customPaint(QPainter*)));
        return proxy;
    }
};

class QcScrollWindowFactory : public QcObjectFactory<QcScrollWindow> {
    // NOTE: use basic object contruction, but return widget proxy
    // NOTE: painting will be performed by QcScrollWidget and its factory
public:
    virtual QObjectProxy* proxy(QcScrollWindow* obj, PyrObject* sc_obj) { return new QWidgetProxy(obj, sc_obj); }
};

QC_DECLARE_FACTORY(QcWindow, QcWindowFactory);
QC_DECLARE_FACTORY(QcScrollWindow, QcScrollWindowFactory);

static void qcInitWindow(QWidget* window, const QString& title, const QRectF& geom_, bool resizable, bool frame) {
    // window title

    window->setWindowTitle(title);

    // size, resizability

    QRect geom(geom_.toRect());

    if (geom.isEmpty()) {
        geom = QApplication::desktop()->availableGeometry();
        geom.setSize(window->sizeHint());
    }

    if (resizable) {
        window->setGeometry(geom);
    } else {
        window->move(geom.topLeft());
        window->setFixedSize(geom.size());
    }

    // frameless?

    if (!frame)
        window->setWindowFlags(window->windowFlags() | Qt::FramelessWindowHint);

    // Ctrl+W shortcut: close the window

    QShortcut* closeShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_W), window);
    QObject::connect(closeShortcut, SIGNAL(activated()), window, SLOT(close()));
}

QcWindow::QcWindow(const QString& title, const QRectF& geom, bool resizable, bool frame) {
    qcInitWindow(this, title, geom, resizable, frame);
}

QcScrollWindow::QcScrollWindow(const QString& title, const QRectF& geom, bool resizable, bool frame) {
    qcInitWindow(this, title, geom, resizable, frame);
}
