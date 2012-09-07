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

#include <QHBoxLayout>

#include "main_window.hpp"
#include "session_switch_dialog.hpp"

#include "../core/main.hpp"
#include "../core/session_manager.hpp"


using namespace ScIDE;

SessionSwitchDialog::SessionSwitchDialog(QWidget * parent):
    QDialog(parent)
{
    setWindowTitle("Switch Session");

    mSessions = new QListWidget(this);

    QHBoxLayout *contentBox = new QHBoxLayout;
    contentBox->addWidget(mSessions);
    setLayout(contentBox);

    QStringList sessions = Main::sessionManager()->availableSessions();
    mSessions->addItems(sessions);

    connect(mSessions, SIGNAL(itemActivated(QListWidgetItem*)), this, SLOT(onItemActivated(QListWidgetItem*)));
}

void SessionSwitchDialog::onItemActivated(QListWidgetItem* item)
{
    accept();
}
