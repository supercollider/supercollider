/************************************************************************
 *
 * Copyright 2011-2012 Jakob Leben (jakob.leben@gmail.com)
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

#include "QcMenu.h"
#include "../QcWidgetFactory.h"

#include <PyrKernel.h>

#include <QKeyEvent>

QC_DECLARE_QWIDGET_FACTORY(QcMenu);
QC_DECLARE_QWIDGET_FACTORY(QcToolBar);
QC_DECLARE_QOBJECT_FACTORY(QcAction);
QC_DECLARE_QOBJECT_FACTORY(QcWidgetAction);

#ifdef Q_OS_MAC
#    include <QTimer>
#    include <QGuiApplication>
#    include <QCursor>
#endif

QcMenu::QcMenu(): QMenu(nullptr) {
    setAttribute(Qt::WA_DeleteOnClose, false);
#ifdef Q_OS_MAC
    connect(this, &QMenu::triggered, this, [this]() { m_actionTriggered = true; });
#endif
}

void QcMenu::popup(QPointF pos, QAction* action) { QMenu::popup(QPoint(pos.x(), pos.y()), action); }

void QcMenu::addAction(QAction* action) {
    if (action) {
        QMenu::addAction(action);
    } else {
        QMenu::addSeparator();
    }
}

void QcMenu::insertAction(QAction* beforeAction, QAction* action) {
    if (action) {
        QMenu::insertAction(beforeAction, action);
    } else {
        QMenu::insertSeparator(beforeAction);
    }
}

void QcMenu::removeAction(QAction* action) {
    if (action) {
        QMenu::removeAction(action);
    }
}

#ifdef Q_OS_MAC
// workaround to trigger menu actions on macOS

void QcMenu::showEvent(QShowEvent* event) {
    m_actionTriggered = false; // reset the flag when the menu is shown
    QMenu::showEvent(event);
}

bool QcMenu::event(QEvent* event) {
    // if the event close arrive first
    if (event->type() == QEvent::Close || event->type() == QEvent::Hide) {
        if (!m_actionTriggered && (QGuiApplication::mouseButtons() & Qt::LeftButton)) {
            QPoint localPos = mapFromGlobal(QCursor::pos());
            if (QAction* act = actionAt(localPos)) {
                m_actionTriggered = true; // indicate handled action
                QTimer::singleShot(0, act, &QAction::trigger); // fire the action
            }
        }
    }

    // if the mousepress arrives first
    if (event->type() == QEvent::MouseButtonPress) {
        auto* mouseEvent = static_cast<QMouseEvent*>(event);
        if (QAction* act = actionAt(mouseEvent->pos())) {
            if (!m_actionTriggered) {
                m_actionTriggered = true; // indicate handled action
                QTimer::singleShot(0, act, &QAction::trigger); // fire the action
            }
            return true; // consume event
        }
    }

    // avoid double-triggering
    if (event->type() == QEvent::MouseButtonRelease) {
        if (m_actionTriggered) {
            return true; // consume event
        }
    }

    // handle keyboard
    if (event->type() == QEvent::KeyPress) {
        auto* keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Space) {
            if (QAction* act = activeAction()) {
                if (!m_actionTriggered) {
                    m_actionTriggered = true; // indicate handled action
                    QTimer::singleShot(0, act, &QAction::trigger);
                }
                return true; // consume event
            }
        }
    }

    return QMenu::event(event);
}
#endif

void QcToolBar::addAction(QAction* action) {
    if (action) {
        QToolBar::addAction(action);
    } else {
        QToolBar::addSeparator();
    }
}

void QcToolBar::insertAction(QAction* beforeAction, QAction* action) {
    if (action) {
        QToolBar::insertAction(beforeAction, action);
    } else {
        QToolBar::insertSeparator(beforeAction);
    }
}

void QcToolBar::removeAction(QAction* action) {
    if (action) {
        QToolBar::removeAction(action);
    }
}

QcAction::QcAction(): QAction(NULL) {}

QcWidgetAction::QcWidgetAction(): QWidgetAction(NULL) {}

void QcAction::setIcon(const QtCollider::SharedImage& image) {
    if (image) {
        QIcon icon(image->pixmap());
        QAction::setIcon(icon);
    } else {
        QAction::setIcon(QIcon());
    }
}

QcToolBar::QcToolBar(): QToolBar() {}
