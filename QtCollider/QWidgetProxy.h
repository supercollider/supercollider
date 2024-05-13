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

#include "QObjectProxy.h"

#include <QWidget>
#include <QAtomicInt>
#include <QMimeData>

namespace QtCollider {
struct SetFocusEvent;
struct SetAlwaysOnTopEvent;
struct StartDragEvent;
}

class QWidgetProxy : public QObjectProxy {
    Q_OBJECT

public:
    enum GlobalEvent { KeyPress = 0x001, KeyRelease = 0x002 };

public:
    static void setGlobalEventEnabled(GlobalEvent ev, bool b) {
        int mask = _globalEventMask.load();
        if (b)
            mask |= ev;
        else
            mask &= ~ev;
        _globalEventMask = mask;
    }

public:
    QWidgetProxy(QWidget* w, PyrObject* po);

    void setKeyEventWidget(QWidget*);

    void setMouseEventWidget(QWidget*);

    bool alwaysOnTop();

    void refresh();

    void setLayout(QObjectProxy* layoutProxy);

    virtual bool setParent(QObjectProxy* parent);

    void setDragData(QMimeData* data, const QString& label);

    inline QWidget* widget() { return static_cast<QWidget*>(object()); }

protected:
    virtual void customEvent(QEvent*);

    virtual bool preProcessEvent(QObject*, QEvent*, EventHandlerData&, QList<QVariant>& args);

    virtual bool postProcessEvent(QObject*, QEvent*, bool handled);

private Q_SLOTS:

    void customPaint(QPainter*);

private:
    bool interpretMouseEvent(QObject*, QEvent*, QList<QVariant>& args);
    bool interpretMouseWheelEvent(QObject*, QEvent*, QList<QVariant>& args);
    bool interpretKeyEvent(QObject*, QEvent*, QList<QVariant>& args);
    bool interpretDragEvent(QObject*, QEvent*, QList<QVariant>& args);

    void bringFrontEvent();
    void setFocusEvent(QtCollider::SetFocusEvent*);
    void setAlwaysOnTopEvent(QtCollider::SetAlwaysOnTopEvent*);
    void performDrag();

    static void sendRefreshEventRecursive(QWidget* w);

    QWidget* _keyEventWidget;
    QWidget* _mouseEventWidget;
    static QAtomicInt _globalEventMask;

    static QMimeData* sDragData;
    static QString sDragLabel;
    bool _performDrag;
};

namespace QtCollider {

struct SetFocusEvent : public QEvent {
    SetFocusEvent(bool b): QEvent((QEvent::Type)QtCollider::Event_Proxy_SetFocus), focus(b) {}
    bool focus;
};

struct SetAlwaysOnTopEvent : public QEvent {
    SetAlwaysOnTopEvent(bool b): QEvent((QEvent::Type)QtCollider::Event_Proxy_SetAlwaysOnTop), alwaysOnTop(b) {}
    bool alwaysOnTop;
};

}
