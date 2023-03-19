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

#include "general_page.hpp"
#include "ui_settings_general.h"
#include "../../core/settings/manager.hpp"

Q_DECLARE_METATYPE(QAction*)
Q_DECLARE_METATYPE(QKeySequence)

namespace ScIDE { namespace Settings {

GeneralPage::GeneralPage(QWidget* parent): QWidget(parent), ui(new Ui::GeneralConfigPage) {
    ui->setupUi(this);

    connect(ui->startSessionName, SIGNAL(textChanged(QString)), this, SLOT(onStartSessionNameChanged(QString)));
}

GeneralPage::~GeneralPage() { delete ui; }

void GeneralPage::load(Manager* settings) {
    QString startSessionName = settings->value("IDE/startWithSession").toString();
    if (startSessionName.isEmpty())
        ui->startNewSessionOption->setChecked(true);
    else if (startSessionName == "last")
        ui->startLastSessionOption->setChecked(true);
    else {
        ui->startNamedSessionOption->setChecked(true);
        ui->startSessionName->setText(startSessionName);
    }
}

void GeneralPage::store(Manager* settings) {
    settings->beginGroup("IDE");

    QWidget* checkedOption = ui->startSessionOptions->checkedButton();

    if (checkedOption == ui->startLastSessionOption) {
        settings->setValue("startWithSession", "last");
    } else if (checkedOption == ui->startNamedSessionOption && !ui->startSessionName->text().isEmpty()) {
        settings->setValue("startWithSession", ui->startSessionName->text());
    } else {
        settings->setValue("startWithSession", "");
    }

    settings->endGroup();
}

void GeneralPage::onStartSessionNameChanged(const QString& text) {
    if (!text.isEmpty())
        ui->startNamedSessionOption->setChecked(true);
}

}} // namespace ScIDE::Settings
