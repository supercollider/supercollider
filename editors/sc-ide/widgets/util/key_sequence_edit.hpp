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

#include <QLineEdit>
#include <QKeyEvent>
#include <QBasicTimer>

namespace ScIDE {

class KeySequenceEdit : public QLineEdit {
    Q_OBJECT

private:
    QBasicTimer mEditingTimer;

public:
    KeySequenceEdit(QWidget* parent = 0): QLineEdit(parent), k1(0), k2(0), k3(0), k4(0), mEditing(false) {
        setReadOnly(true);
    }

    QKeySequence sequence() { return QKeySequence(k1, k2, k3, k4); }

    void setSequence(const QKeySequence& seq) {
        k1 = seq[0];
        k2 = seq[1];
        k3 = seq[2];
        k4 = seq[3];

        if (mEditing)
            finishEditing();
        else
            updateText();
    }

public slots:
    void reset() {
        k1 = k2 = k3 = k4 = 0;

        if (mEditing)
            finishEditing();
        else
            updateText();
    }

    void finishEditing() {
        if (mEditing) {
            mEditing = false;
            mEditingTimer.stop();
            updateText();
            emit editingFinished();
        }
    }

signals:
    void editingStarted();

protected:
    //#if 0
    void updateText() {
        QKeySequence seq = sequence();
        if (seq.isEmpty())
            clear();
        else {
            QString text = sequence().toString(QKeySequence::NativeText);
            if (mEditing)
                text.append(", ...");
            setText(text);
        }
    }

    bool event(QEvent* e) {
        if (e->type() != QEvent::KeyPress)
            return QLineEdit::event(e);

        e->accept();

        QKeyEvent* ke = static_cast<QKeyEvent*>(e);
        int key = ke->key();

        if (isModifier(key))
            return true;

        int* k;
        if (!mEditing) {
            k1 = k2 = k3 = k4 = 0;
            mEditing = true;
            k = &k1;
        } else {
            if (k1 == 0)
                k = &k1;
            else if (k2 == 0)
                k = &k2;
            else if (k3 == 0)
                k = &k3;
            else
                k = &k4;
        }

        *k = key | ke->modifiers();

        if (k == &k4)
            finishEditing();
        else {
            mEditingTimer.start(800, this);
            updateText();
            if (k == &k1)
                emit editingStarted();
        }

        return true;
    }

    virtual void timerEvent(QTimerEvent* e) {
        if (e->timerId() == mEditingTimer.timerId())
            finishEditing();
    }

    void focusOutEvent(QFocusEvent* e) { finishEditing(); }

    //#endif
private:
    bool isModifier(int key) {
        switch (key) {
        case Qt::Key_Shift:
        case Qt::Key_Control:
        case Qt::Key_Meta:
        case Qt::Key_Alt:
        case Qt::Key_AltGr:
            return true;
        default:
            return false;
        }
    }

    int k1, k2, k3, k4;
    bool mEditing;
};

} // namespace ScIDE
