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

#include <QWidget>
#include <QEventLoop>
#include <QPointer>

namespace ScIDE {

class PopUpWidget : public QWidget {
    Q_OBJECT

public:
    enum Result { Rejected = 0, Accepted };

public:
    explicit PopUpWidget(QWidget* parent = 0);
    virtual ~PopUpWidget();

    int exec(const QRect& targetRect);
    void popup(const QRect& targetRect);
    void setTargetRect(const QRect& targetRect) { mTargetRect = targetRect; }

public slots:
    void accept() { done(Accepted); }

    void reject() { done(Rejected); }

    void done(int result) {
        mResult = result;
        hide();
        quit();
    }

signals:
    void finished(int result);

private:
    void quit() {
        if (mEventLoop) {
            mEventLoop->exit();
            mEventLoop = 0;
        }
    }

protected:
    virtual void hideEvent(QHideEvent* e) {
        quit();
        emit finished(mResult);
    }

    virtual void keyPressEvent(QKeyEvent* ke);
    virtual void showEvent(QShowEvent*);

private:
    QEventLoop* mEventLoop;
    int mResult;
    QRect mTargetRect;
};

} // namespace ScIDE
