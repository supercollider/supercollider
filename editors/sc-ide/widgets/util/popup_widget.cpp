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

PopUpWidget::PopUpWidget( QWidget * parent ):
    QWidget( parent, Qt::ToolTip ),
    mEventLoop(0),
    mResult(0)
{}

PopUpWidget::~PopUpWidget()
{
    quit();
}

int PopUpWidget::exec( const QPoint & pos )
{
    if (mEventLoop) {
        qWarning("ScIDE::PopUpWidget: recursive exec() - suppressed!");
        return Rejected;
    }

    mResult = Rejected;

    QEventLoop eventLoop;
    mEventLoop = &eventLoop;

    move(pos);
    show();

    QPointer<QObject> self(this);
    mEventLoop->exec();
    if (self.isNull())
        return Rejected;

    return mResult;
}

void PopUpWidget::popup( const QPoint & pos )
{
    mResult = Rejected;
    move(pos);
    show();
}

void PopUpWidget::keyPressEvent( QKeyEvent *ke )
{
    switch(ke->key())
    {
    case Qt::Key_Escape:
        reject();
        break;
    default:;
    }
}

void PopUpWidget::showEvent( QShowEvent *e )
{
    QRect r = geometry();
    QRect screen = QApplication::desktop()->availableGeometry(this);
    if (!screen.contains(r))
    {
        if (r.right() > screen.right())
            r.moveRight( screen.right() );
        if (r.bottom() > screen.bottom())
            r.moveBottom( screen.bottom() );
        if (r.left() < screen.left())
            r.moveLeft( screen.left() );
        if (r.top() < screen.top())
            r.moveTop(screen.top());
    }

    move(r.topLeft());
}

} // namespace ScIDE
