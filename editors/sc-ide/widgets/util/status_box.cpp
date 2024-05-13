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

#include "status_box.hpp"

namespace ScIDE {

StatusLabel::StatusLabel(QWidget* parent): QLabel(parent) {
    setAutoFillBackground(true);
    setMargin(3);
    setAlignment(Qt::AlignCenter);
    setBackground(Qt::black);
    setTextColor(Qt::white);

    QFont font("Monospace");
    font.setStyleHint(QFont::Monospace);
    font.setBold(true);
    setFont(font);
}

void StatusLabel::setBackground(const QBrush& brush) {
    QPalette plt(palette());
    plt.setBrush(QPalette::Window, brush);
    setPalette(plt);
}

void StatusLabel::setTextColor(const QColor& color) {
    QPalette plt(palette());
    plt.setColor(QPalette::WindowText, color);
    setPalette(plt);
}

StatusBox::StatusBox(QWidget* parent): QWidget(parent), mMenu(0) {}

void StatusBox::showContextMenu() {
    if (!mMenu) {
        QList<QAction*> actions = this->actions();
        if (actions.count()) {
            StatusBoxMenu* menu = new StatusBoxMenu;
            menu->addActions(actions);
            mMenu = menu;
        }
    }

    if (!mMenu->isVisible())
        mMenu->popup(mapToGlobal(QPoint(0, -mMenu->sizeHint().height() - 2)));
}


void StatusBox::mousePressEvent(QMouseEvent*) { showContextMenu(); }

void StatusBox::addActionSeparator() {
    QAction* separator = new QAction(this);
    separator->setSeparator(true);
    addAction(separator);
}

} // namespace ScIDE
