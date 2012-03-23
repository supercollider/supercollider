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

#ifndef SCIDE_WIDGETS_UTIL_COLOR_WIDGET_HPP_INCLUDED
#define SCIDE_WIDGETS_UTIL_COLOR_WIDGET_HPP_INCLUDED

#include <QLabel>
#include <QColorDialog>

namespace ScIDE {

class ColorWidget : public QLabel
{
public:
    ColorWidget(QWidget *parent = 0) : QLabel(parent)
    {
        setAutoFillBackground(true);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    }

    void setColor ( const QColor & color )
    {
        QPalette plt;
        plt.setColor(QPalette::Window, color);
        setPalette(plt);
    }

    QColor color () const
    {
        return palette().color(QPalette::Window);
    }

protected:
    virtual void mouseDoubleClickEvent (QMouseEvent *)
    {
        QColor c = QColorDialog::getColor(color(), this);
        if (c.isValid())
            setColor(c);
    }
};

} // namespace ScIDE

#endif // SCIDE_WIDGETS_UTIL_COLOR_WIDGET_HPP_INCLUDED
