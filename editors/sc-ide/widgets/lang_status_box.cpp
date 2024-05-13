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

#include "lang_status_box.hpp"
#include "../core/sc_process.hpp"

#include <QHBoxLayout>

namespace ScIDE {

LangStatusBox::LangStatusBox(ScProcess* lang, QWidget* parent): StatusBox(parent) {
    mLabel = new StatusLabel;
    QHBoxLayout* layout = new QHBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(mLabel);
    setLayout(layout);

    addAction(lang->action(ScProcess::ToggleRunning));
    addAction(lang->action(ScProcess::Restart));
    addAction(lang->action(ScProcess::RecompileClassLibrary));

    connect(lang, SIGNAL(stateChanged(QProcess::ProcessState)), this,
            SLOT(onInterpreterStateChanged(QProcess::ProcessState)));

    onInterpreterStateChanged(lang->state());
}

void LangStatusBox::onInterpreterStateChanged(QProcess::ProcessState state) {
    QString text;
    QColor color;

    switch (state) {
    case QProcess::NotRunning:
        text = tr("Inactive");
        color = Qt::white;
        break;

    case QProcess::Starting:
        text = tr("Booting");
        color = QColor(255, 255, 0);
        break;

    case QProcess::Running:
        text = tr("Active");
        color = Qt::green;
        break;
    }

    mLabel->setText(text);
    mLabel->setTextColor(color);
}

} // namespace ScIDE
