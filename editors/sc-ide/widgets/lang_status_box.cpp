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

#include "main.hpp"

namespace ScIDE {

LangStatusBox::LangStatusBox(ScProcess* lang, QWidget* parent): StatusBox(parent), mLang(lang) {
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

    // initialize formats from settings:
    auto const main = Main::instance();
    applySettings(main->settings());
    connect(main, &Main::applySettingsRequest, this, &LangStatusBox::applySettings);
}

void LangStatusBox::applySettings(Settings::Manager* settings) {
    backgroundColor = settings->getThemeVal("text").background().color();

    notRunningColor = settings->getThemeVal("text").foreground().color();
    runningColor = settings->getThemeVal("postwindowsuccess").foreground().color();
    startingColor = settings->getThemeVal("postwindowwarning").foreground().color();

    // re-render the box, otherwise we will not update after e.g. a theme switch
    onInterpreterStateChanged(mLang->state());
}

void LangStatusBox::onInterpreterStateChanged(QProcess::ProcessState state) {
    QString text;
    QColor color;

    switch (state) {
    case QProcess::NotRunning:
        text = tr("Inactive");
        color = notRunningColor;
        break;

    case QProcess::Starting:
        text = tr("Booting");
        color = startingColor;
        break;

    case QProcess::Running:
        text = tr("Active");
        color = runningColor;
        break;
    }

    mLabel->setText(text);
    mLabel->setTextColor(color);
    mLabel->setBackground(backgroundColor);
}

} // namespace ScIDE
