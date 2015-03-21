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

#ifndef SCIDE_WIDGETS_TOOL_BOX_HPP_INCLUDED
#define SCIDE_WIDGETS_TOOL_BOX_HPP_INCLUDED

#include <QtCollider/layouts/stack_layout.hpp>

#include <QWidget>
#include <QToolButton>
#include <QHBoxLayout>
#include <QStyle>
#include <QStyleOption>
#include <QPainter>

namespace ScIDE {

class ToolBox : public QWidget
{
    Q_OBJECT

public:
    ToolBox( QWidget * parent = 0 ): QWidget(parent)
    {
        setObjectName("toolbox");
        
        mCloseBtn = new QToolButton;
        mCloseBtn->setIcon( style()->standardIcon(QStyle::SP_TitleBarCloseButton) );
        mCloseBtn->setText("X");
        mCloseBtn->setAutoRaise(true);
        mCloseBtn->setMaximumSize(28, 28);

        mStack = new QtCollider::StackLayout;

        QHBoxLayout *layout = new QHBoxLayout;
        layout->setMargin(2);
        layout->setSpacing(0);
        layout->setContentsMargins(2, 2, 2, 2);
        layout->addWidget(mCloseBtn, 0, Qt::AlignTop);
        layout->addLayout(mStack);

        setLayout(layout);
    }

    QAbstractButton *closeButton() { return mCloseBtn; }

    void addWidget ( QWidget *w ) { mStack->addWidget(w); }

    int currentIndex () const { return mStack->currentIndex(); }

    QWidget * currentWidget() const { return mStack->currentWidget(); }

    void setCurrentWidget ( QWidget *w ) { mStack->setCurrentWidget(w); }

    void setCurrentIndex ( int i ) { mStack->setCurrentIndex(i); }

    void paintEvent( QPaintEvent *event )
    {
        QStyleOption opt;
        opt.init(this);
        QPainter p(this);
        style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    }
private:
    QtCollider::StackLayout *mStack;
    QToolButton *mCloseBtn;
};

} // namespace ScIDE

#endif
