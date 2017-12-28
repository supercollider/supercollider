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
#include "../../core/session_manager.hpp"
#include "../../core/settings/manager.hpp"

Q_DECLARE_METATYPE(QAction*)
Q_DECLARE_METATYPE(QKeySequence)

namespace ScIDE { namespace Settings {

GeneralPage::GeneralPage(QWidget *parent) :
    QWidget(parent),
    ui( new Ui::GeneralConfigPage )
{
    ui->setupUi(this);

    ui->fileTreeRootDir->setFileMode(QFileDialog::Directory);

    connect( ui->startSessionName, SIGNAL(textChanged(QString)),
             this, SLOT(onStartSessionNameChanged(QString)) );

    connect( ui->useLanguageConfigFromSession, SIGNAL(clicked(bool)),
             this, SLOT(emitUseLanguageConfigFromSessionChanged(bool)) );

}

GeneralPage::~GeneralPage()
{
    delete ui;
}

void GeneralPage::load( Manager *settings, Session *session)
{
    QString startSessionName = settings->value("IDE/startWithSession").toString();
    if (startSessionName.isEmpty())
        ui->startNewSessionOption->setChecked(true);
    else if (startSessionName == "last")
        ui->startLastSessionOption->setChecked(true);
    else {
        ui->startNamedSessionOption->setChecked(true);
        ui->startSessionName->setText(startSessionName);
    }
    ui->useLanguageConfigFromSession->setChecked(settings->value("IDE/useLanguageConfigFromSession").toBool());
    ui->fileTreeRootDir->setText(settings->value("IDE/fileTreeRoot").toString());
}

void GeneralPage::store( Manager *settings, Session *session, bool useLanguageConfigFromSession)
{
    settings->beginGroup("IDE");

    QWidget *checkedOption = ui->startSessionOptions->checkedButton();

    if (checkedOption == ui->startLastSessionOption) {
        settings->setValue("startWithSession", "last");
    }
    else if (checkedOption == ui->startNamedSessionOption &&
             !ui->startSessionName->text().isEmpty())
    {
        settings->setValue("startWithSession", ui->startSessionName->text());
    }
    else {
        settings->setValue("startWithSession", "");
    }

    settings->setValue("useLanguageConfigFromSession", ui->useLanguageConfigFromSession->isChecked() );

    settings->setValue("fileTreeRoot", ui->fileTreeRootDir->text());

    settings->endGroup();
}

void GeneralPage::onStartSessionNameChanged( const QString & text )
{
    if (!text.isEmpty())
        ui->startNamedSessionOption->setChecked(true);
}

void GeneralPage::emitUseLanguageConfigFromSessionChanged(bool selected)
{
    Q_EMIT( useLanguageConfigFromSessionChanged(selected) );
}

bool GeneralPage::useLanguageConfigFromSessionChecked()
{
    return ui->useLanguageConfigFromSession->isChecked();
}


}} // namespace ScIDE::Settings
