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

#include <QtCollider/layouts/stack_layout.hpp>

#include <QWidget>
#include <QToolButton>
#include <QHBoxLayout>
#include <QStyle>

namespace ScIDE {

class ToolBox : public QWidget {
    Q_OBJECT

public:
    ToolBox(QWidget* parent = 0): QWidget(parent) {
        mCloseBtn = new QToolButton;
        mCloseBtn->setIcon(style()->standardIcon(QStyle::SP_TitleBarCloseButton));
        mCloseBtn->setText("X");
        mCloseBtn->setAutoRaise(true);

        mStack = new QtCollider::StackLayout;

        QHBoxLayout* layout = new QHBoxLayout;
        layout->setContentsMargins(2, 2, 2, 2);
        layout->addWidget(mCloseBtn, 0, Qt::AlignTop | Qt::AlignLeft);
        layout->addLayout(mStack);

        setLayout(layout);
    }

    QAbstractButton* closeButton() { return mCloseBtn; }

    void addWidget(QWidget* w) { mStack->addWidget(w); }

    int currentIndex() const { return mStack->currentIndex(); }

    QWidget* currentWidget() const { return mStack->currentWidget(); }

    void setCurrentWidget(QWidget* w) { mStack->setCurrentWidget(w); }

    void setCurrentIndex(int i) { mStack->setCurrentIndex(i); }

private:
    QtCollider::StackLayout* mStack;
    QToolButton* mCloseBtn;
};

} // namespace ScIDE
