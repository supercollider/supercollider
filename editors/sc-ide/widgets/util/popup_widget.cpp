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

#include "popup_widget.hpp"

#include <QKeyEvent>
#include <QApplication>
#include <QDesktopWidget>
#include <QDebug>

namespace ScIDE {

PopUpWidget::PopUpWidget(QWidget* parent): QWidget(parent, Qt::ToolTip), mEventLoop(0), mResult(0) {}

PopUpWidget::~PopUpWidget() { quit(); }

int PopUpWidget::exec(const QRect& targetRect) {
    if (mEventLoop) {
        qWarning("ScIDE::PopUpWidget: recursive exec() - suppressed!");
        return Rejected;
    }

    mResult = Rejected;
    mTargetRect = targetRect;

    QEventLoop eventLoop;
    mEventLoop = &eventLoop;

    show();

    QPointer<QObject> self(this);
    mEventLoop->exec();
    if (self.isNull())
        return Rejected;

    return mResult;
}

void PopUpWidget::popup(const QRect& targetRect) {
    mResult = Rejected;
    mTargetRect = targetRect;
    show();
}

void PopUpWidget::keyPressEvent(QKeyEvent* ke) {
    switch (ke->key()) {
    case Qt::Key_Escape:
        reject();
        break;
    default:;
    }
}

void PopUpWidget::showEvent(QShowEvent*) {
    QRect rect = geometry();
    rect.moveTopLeft(mTargetRect.bottomLeft());

    QWidget* parentWid = parentWidget();
    QWidget* referenceWidget = parentWid ? parentWid : this;

    QRect screen = QApplication::desktop()->availableGeometry(referenceWidget);
    if (!screen.contains(rect)) {
        if (rect.right() > screen.right())
            rect.moveRight(screen.right());
        if (rect.left() < screen.left())
            rect.moveLeft(screen.left());
        if (rect.bottom() > screen.bottom())
            rect.moveBottom(qMin(mTargetRect.top(), screen.bottom()));
        if (rect.top() < screen.top())
            rect.moveTop(screen.top());
    }

    move(rect.topLeft());
}

} // namespace ScIDE
