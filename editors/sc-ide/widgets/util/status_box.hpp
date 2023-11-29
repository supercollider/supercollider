/*
    SuperCollider Qt IDE
    Copyright (c) 2012 - 2013 Jakob Leben & Tim Blechmann
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

#include <QLabel>
#include <QMenu>

namespace ScIDE {

class StatusBoxMenu : public QMenu {
public:
    StatusBoxMenu(QWidget* parent = 0): QMenu(parent) {}

protected:
    virtual void mouseReleaseEvent(QMouseEvent* e) {
        QAction* action = activeAction();
        if (action && action->isEnabled() && !action->menu() && action->property("keep_menu_open").toBool()) {
            action->setEnabled(false);
            QMenu::mouseReleaseEvent(e);
            action->setEnabled(true);
            action->trigger();
            return;
        }

        QMenu::mouseReleaseEvent(e);
    }
};

class StatusLabel : public QLabel {
public:
    StatusLabel(QWidget* parent = 0);
    void setBackground(const QBrush&);
    void setTextColor(const QColor&);
};

class StatusBox : public QWidget {
public:
    StatusBox(QWidget* parent = 0);

protected:
    void addActionSeparator();
    void showContextMenu();
    virtual void mousePressEvent(QMouseEvent*);
    StatusBoxMenu* mMenu;
};

} // namespace ScIDE
