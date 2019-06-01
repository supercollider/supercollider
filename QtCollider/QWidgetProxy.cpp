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

#include "QWidgetProxy.h"
#include "painting.h"
#include "Common.h"
#include "hacks/hacks_qt.hpp"

#include <QApplication>
#include <QLayout>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QPainter>
#include <QFontMetrics>
#include <QUrl>
#include <QMimeData>
#include <QDrag>

#ifdef Q_WS_X11
#    include "hacks/hacks_x11.hpp"
#    include <QX11Info>
#    include <X11/Xlib.h>
// X11 defines the following, clashing with QEvent::Type enum
#    undef KeyPress
#    undef KeyRelease
#endif

#ifdef Q_OS_MAC
#    include "./hacks/hacks_mac.hpp"
#endif

using namespace QtCollider;

QAtomicInt QWidgetProxy::_globalEventMask = 0;
QMimeData* QWidgetProxy::sDragData = 0;
QString QWidgetProxy::sDragLabel;

QWidgetProxy::QWidgetProxy(QWidget* w, PyrObject* po):
    QObjectProxy(w, po),
    _keyEventWidget(w),
    _mouseEventWidget(w),
    _performDrag(false) {}

void QWidgetProxy::setKeyEventWidget(QWidget* w) {
    if (w == 0 || w == _keyEventWidget)
        return;

    QWidget* me = widget();

    if (_keyEventWidget != me)
        _keyEventWidget->removeEventFilter(this);

    _keyEventWidget = w;

    if (_keyEventWidget != me) {
        _keyEventWidget->installEventFilter(this);
    }
}

void QWidgetProxy::setMouseEventWidget(QWidget* w) {
    if (w == 0 || w == _mouseEventWidget)
        return;

    QWidget* me = widget();

    if (_mouseEventWidget != me)
        _mouseEventWidget->removeEventFilter(this);

    _mouseEventWidget = w;

    if (_mouseEventWidget != me) {
        _mouseEventWidget->installEventFilter(this);
    }
}

bool QWidgetProxy::alwaysOnTop() {
    QWidget* w = widget();
    if (!w)
        return false;

    Qt::WindowFlags flags = w->windowFlags();
    if (flags & Qt::Window && flags & Qt::WindowStaysOnTopHint)
        return true;
    else
        return false;
}

void QWidgetProxy::refresh() {
    QWidget* w = widget();
    if (w)
        sendRefreshEventRecursive(w);
}

void QWidgetProxy::setLayout(QObjectProxy* layoutProxy) {
    QWidget* w = widget();
    QLayout* l = qobject_cast<QLayout*>(layoutProxy->object());
    if (!w || !l)
        return;

    QLayout* exLayout = w->layout();
    if (exLayout != l) {
        if (exLayout != 0) {
            qcDebugMsg(2, QStringLiteral("Deleting old layout."));
            delete exLayout;
        }
        qcDebugMsg(2, QStringLiteral("Setting layout."));
        w->setLayout(l);
        l->activate();
    } else {
        qcDebugMsg(2, QStringLiteral("Layout same as existing. Will do nothing."));
    }
}

bool QWidgetProxy::setParent(QObjectProxy* parentProxy) {
    QObject* parent = parentProxy->object();
    if (!parent || !widget())
        return true;

    if (parent->isWidgetType()) {
        QWidget* pw = qobject_cast<QWidget*>(parent);
        bool ok = pw->metaObject()->invokeMethod(pw, "addChild", Q_ARG(QWidget*, widget()));
        if (!ok)
            widget()->setParent(pw);
        return true;
    }
    return false;
}

void QWidgetProxy::setDragData(QMimeData* data, const QString& label) {
    if (data == 0)
        return;

    if (sDragData == 0) {
        sDragData = data;
        sDragLabel = label;
        _performDrag = true;
    } else {
        delete data;
        qcErrorMsg("QWidgetProxy: attempt at starting a drag while another one is in progress.");
    }
}

void QWidgetProxy::customEvent(QEvent* e) {
    int type = e->type();
    switch (type) {
    case QtCollider::Event_Proxy_BringFront:
        bringFrontEvent();
        return;
    case QtCollider::Event_Proxy_SetFocus:
        setFocusEvent(static_cast<SetFocusEvent*>(e));
        return;
    case QtCollider::Event_Proxy_SetAlwaysOnTop:
        setAlwaysOnTopEvent(static_cast<SetAlwaysOnTopEvent*>(e));
        return;
    default:
        QObjectProxy::customEvent(e);
    }
}


void QWidgetProxy::bringFrontEvent() {
    QWidget* w = widget();
    if (!w)
        return;

    w->setWindowState(w->windowState() & ~Qt::WindowMinimized | Qt::WindowActive);
    w->show();
    w->raise();

#ifdef Q_WS_X11
    raise_window(QX11Info::display(), w);
#endif

#ifdef Q_OS_MAC
    QtCollider::Mac::activateApp();
#endif
}

void QWidgetProxy::setFocusEvent(QtCollider::SetFocusEvent* e) {
    if (!widget())
        return;

    if (e->focus)
        widget()->setFocus(Qt::OtherFocusReason);
    else
        widget()->clearFocus();
}

void QWidgetProxy::setAlwaysOnTopEvent(QtCollider::SetAlwaysOnTopEvent* e) {
    QWidget* w = widget();
    if (!w)
        return;

    Qt::WindowFlags flags = w->windowFlags();
    if (flags & Qt::Window) {
        if (e->alwaysOnTop)
            flags |= Qt::WindowStaysOnTopHint;
        else
            flags &= ~Qt::WindowStaysOnTopHint;

        // record the initial state to restore it later
        QPoint pos = w->pos();
        bool visible = w->isVisible();

        w->setWindowFlags(flags);

        // setting window flags will move the window to (0,0) and hide it,
        // so restore the initial state
        w->move(pos);
        if (visible)
            w->show();
    }
}

void QWidgetProxy::performDrag() {
    Q_ASSERT(sDragData);

    QFont f;
    const QString& label = sDragLabel;
    QFontMetrics fm(f);
    QSize size = fm.size(0, label) + QSize(8, 4);

    QPixmap pix(size);
    QPainter p(&pix);
    p.setBrush(QColor(255, 255, 255));
    QRect r(pix.rect());
    p.drawRect(r.adjusted(0, 0, -1, -1));
    p.drawText(r, Qt::AlignCenter, label);
    p.end();

    QDrag* drag = new QDrag(widget());
    drag->setMimeData(sDragData);
    drag->setPixmap(pix);
    drag->setHotSpot(QPoint(0, +r.height() + 2));
    drag->exec();

    sDragData = 0;
    sDragLabel.clear();
}

bool QWidgetProxy::preProcessEvent(QObject* o, QEvent* e, EventHandlerData& eh, QList<QVariant>& args) {
    // NOTE We assume that qObject need not be checked here, as we wouldn't get events if
    // it wasn't existing
    int acquired_globalEventMask = _globalEventMask.load();

    switch (eh.type) {
    case QEvent::KeyPress:
        return ((acquired_globalEventMask & KeyPress) || eh.enabled) && interpretKeyEvent(o, e, args);

    case QEvent::KeyRelease:
        return ((acquired_globalEventMask & KeyRelease) || eh.enabled) && interpretKeyEvent(o, e, args);

    case QEvent::MouseButtonPress:
    case QEvent::MouseMove:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseButtonDblClick:
    case QEvent::Enter:
    case QEvent::Leave:
        return eh.enabled && interpretMouseEvent(o, e, args);

    case QEvent::Wheel:
        return eh.enabled && interpretMouseWheelEvent(o, e, args);

    case QEvent::DragEnter:
    case QEvent::DragMove:
    case QEvent::Drop:
        return eh.enabled && interpretDragEvent(o, e, args);

    default:
        return eh.enabled;
    }
}

bool QWidgetProxy::interpretMouseEvent(QObject* o, QEvent* e, QList<QVariant>& args) {
    if (o != _mouseEventWidget || !_mouseEventWidget->isEnabled())
        return false;

    QWidget* w = widget();

    QEvent::Type etype = e->type();

    if (etype == QEvent::Enter || etype == QEvent::Leave)
        return true;

    QMouseEvent* mouse = static_cast<QMouseEvent*>(e);
    QPoint pt = (_mouseEventWidget == w ? mouse->pos() : _mouseEventWidget->mapTo(w, mouse->pos()));
    args << pt.x();
    args << pt.y();

    args << (int)mouse->modifiers();

    if (etype == QEvent::MouseMove) {
        int buttons = mouse->buttons();

        if (buttons == 0) {
            // Special treatment of mouse-tracking events.

            QWidget* win = w->window();

            // Only accept if window has a special property enabled.
            if (!(win && win->property("_qc_win_mouse_tracking").toBool()))
                return false;

            // Reject the events when mouse pointer leaves the window,
            // resulting in out-of-bounds coordinates
            if (win == w) {
                if (pt.x() < 0 || pt.x() >= w->width() || pt.y() < 0 || pt.y() >= w->height())
                    return false;
            }
        }

        args << (int)mouse->buttons();
    } else {
        // MouseButtonPress, MouseButtonDblClick, MouseButtonRelease

        int button;

        switch (mouse->button()) {
        case Qt::LeftButton:
            button = 0;
            break;
        case Qt::RightButton:
            button = 1;
            break;
        case Qt::MidButton:
            button = 2;
            break;
        default:
            button = -1;
        }

        args << button;

        if (etype == QEvent::MouseButtonPress)
            args << 1;
        else if (etype == QEvent::MouseButtonDblClick)
            args << 2;
    }

    return true;
}

bool QWidgetProxy::interpretMouseWheelEvent(QObject* o, QEvent* e, QList<QVariant>& args) {
    // NOTE: There seems to be a bug in wheel event propagation:
    // the event is propagated to parent twice!
    // Therefore we do not let the propagated events through to SC,
    // (we only let the "spontaneous" ones).

    if (o != _mouseEventWidget || !e->spontaneous() || !_mouseEventWidget->isEnabled())
        return false;

    QWheelEvent* we = static_cast<QWheelEvent*>(e);

    QWidget* w = widget();
    QPoint pt = _mouseEventWidget == w ? we->pos() : _mouseEventWidget->mapTo(w, we->pos());

    QPointF delta = we->pixelDelta();
    delta *= 0.25f; // This matches old scaling of delta

    args << pt.x();
    args << pt.y();
    args << (int)we->modifiers();
    args << delta.x();
    args << delta.y();

    return true;
}

bool QWidgetProxy::interpretKeyEvent(QObject* o, QEvent* e, QList<QVariant>& args) {
    if (o != _keyEventWidget || !_keyEventWidget->isEnabled())
        return false;

    QKeyEvent* ke = static_cast<QKeyEvent*>(e);

    int key = ke->key();

    int mods = ke->modifiers();

    QChar character;

#ifdef Q_OS_MAC
    bool isLetter = key >= Qt::Key_A && key <= Qt::Key_Z;
    if (mods & Qt::MetaModifier && isLetter) {
        character = QChar(key - Qt::Key_A + 1);
    } else if (mods & Qt::AltModifier && isLetter) {
        character = (mods & Qt::ShiftModifier) ? QChar(key) : QChar(key - Qt::Key_A + 97);
    } else
#endif
    {
        QString text(ke->text());
        if (text.count())
            character = text[0];
    }

    int unicode = character.unicode();

#ifdef Q_WS_X11
    KeySym sym = ke->nativeVirtualKey();
    int keycode = XKeysymToKeycode(QX11Info::display(), sym);
#else
    // FIXME: On Mac OS X, this does not work for modifier keys
    int keycode = ke->nativeVirtualKey();
#endif

    args << character;
    args << mods;
    args << unicode;
    args << keycode;
    args << key;
    args << ke->spontaneous();

    return true;
}

static QString urlAsString(const QUrl& url) {
    if (QURL_IS_LOCAL_FILE(url))
        return url.toLocalFile();
    else
        return url.toString();
}

static bool interpretMimeData(const QMimeData* data, QList<QVariant>& args) {
    if (data->hasUrls()) {
        QList<QUrl> urls = data->urls();
        if (urls.count() > 1) {
            QVariantList list;
            Q_FOREACH (QUrl url, urls)
                list << urlAsString(url);
            args << QVariant(list);
        } else {
            args << urlAsString(urls[0]);
        }
    } else if (data->hasText()) {
        args << data->text();
    } else {
        return false;
    }

    return true;
}

bool QWidgetProxy::interpretDragEvent(QObject* o, QEvent* e, QList<QVariant>& args) {
    if (o != _mouseEventWidget)
        return false;

    QDropEvent* dnd = static_cast<QDropEvent*>(e);

    const QMimeData* data = dnd->mimeData();

    if (dnd->type() == QEvent::DragEnter) {
        bool internal = data->hasFormat("application/supercollider");
        args << internal;
        if (!internal)
            interpretMimeData(data, args);
    } else {
        QPoint pos = dnd->pos();
        args << pos.x() << pos.y();
    }

    return true;
}

bool QWidgetProxy::postProcessEvent(QObject* object, QEvent* event, bool handled) {
    if (_performDrag) {
        _performDrag = false;
        performDrag();
        return true;
    }

    return handled;
}

void QWidgetProxy::customPaint(QPainter* painter) {
    if (QtCollider::paintingAnnounced()) {
        qcDebugMsg(1, "WARNING: Custom painting already in progress. Will not paint.");
        return;
    }

    QtCollider::announcePainting();

    QtCollider::lockLang();

    if (QtCollider::beginPainting(painter, this)) {
        invokeScMethod(SC_SYM(doDrawFunc), QList<QVariant>(), 0, true);
        QtCollider::endPainting();
    }

    QtCollider::unlockLang();
}

void QWidgetProxy::sendRefreshEventRecursive(QWidget* w) {
    QEvent event(static_cast<QEvent::Type>(QtCollider::Event_Refresh));
    QApplication::sendEvent(w, &event);

    const QObjectList& children = w->children();
    Q_FOREACH (QObject* child, children) {
        if (child->isWidgetType())
            sendRefreshEventRecursive(static_cast<QWidget*>(child));
    }
}
