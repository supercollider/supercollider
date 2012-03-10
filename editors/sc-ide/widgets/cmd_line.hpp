/*
    SuperCollider Qt IDE
    Copyright (c) 2010-2012 Jakob Leben
    Copyright (c) 2012 Tim Blechmann
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

#ifndef SCIDE_WIDGETS_CMD_LINE_HPP_INCLUDED
#define SCIDE_WIDGETS_CMD_LINE_HPP_INCLUDED

#include <QString>
#include <QWidget>
#include <QLineEdit>

namespace ScIDE {

class CmdLine : public QWidget
{
Q_OBJECT

public:
    CmdLine( const QString &text, int maxHistory = 30 );
signals:
    void invoked( const QString &, bool silent );
private:
    bool eventFilter( QObject *, QEvent * );

    QLineEdit *expr;
    QStringList history;
    int curHistory;
    int maxHistory;
};

} // namespace ScIDE

#endif // SCIDE_WIDGETS_CMD_LINE_HPP_INCLUDED
