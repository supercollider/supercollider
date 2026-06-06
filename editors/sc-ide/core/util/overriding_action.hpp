/*
    SuperCollider Qt IDE
    Copyright (c) 2012 Jakob Leben & Tim Blechmann
    http://www.audiosynth.com

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
*/

#pragma once

#include <QAction>
#include <QWidget>
#include <QKeyEvent>
#include <QKeySequence>

namespace ScIDE {

class OverridingAction : public QAction {
public:
    OverridingAction(QObject* parent): QAction(parent) { setShortcutContext(Qt::WidgetWithChildrenShortcut); }

    OverridingAction(const QString& text, QObject* parent): QAction(text, parent) {
        setShortcutContext(Qt::WidgetWithChildrenShortcut);
    }

    OverridingAction(const QIcon& icon, const QString& text, QObject* parent): QAction(icon, text, parent) {
        setShortcutContext(Qt::WidgetWithChildrenShortcut);
    }

    void addToWidget(QWidget* widget) {
        widget->addAction(this);
        widget->installEventFilter(this);
    }

    static QKeySequence keySequence(QKeyEvent* event) {
        if (event->key() >= Qt::Key_Shift && event->key() <= Qt::Key_Alt)
            return QKeySequence();

        return QKeySequence(event->modifiers() | event->key());
    }

protected:
    virtual bool eventFilter(QObject* object, QEvent* event) {
        Q_UNUSED(object);
        switch (event->type()) {
        case QEvent::ShortcutOverride: {
            QKeySequence sequence = keySequence(static_cast<QKeyEvent*>(event));
            if (sequence.isEmpty())
                break;
            foreach (const QKeySequence& shortcut, shortcuts()) {
                if (shortcut == sequence) {
                    event->accept();
                    return true;
                }
            }
            break;
        }
        case QEvent::KeyPress: {
            QKeySequence sequence = keySequence(static_cast<QKeyEvent*>(event));
            if (sequence.isEmpty())
                break;
            foreach (const QKeySequence& shortcut, shortcuts()) {
                if (shortcut == sequence) {
                    event->accept();
                    activate(QAction::Trigger);
                    return true;
                }
            }
            break;
        }
        default:
            break;
        }

        return QAction::eventFilter(object, event);
    }
};

} // namespace ScIDE
