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

#include "dialog.hpp"
#include "ui_settings_dialog.h"
#include "general_page.hpp"
#include "sclang_page.hpp"
#include "sclang_project_page.hpp"
#include "editor_page.hpp"
#include "shortcuts_page.hpp"
#include "../../core/session_manager.hpp"
#include "../../core/settings/manager.hpp"
#include "../../core/main.hpp"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDialogButtonBox>
#include <QStackedWidget>
#include <QListWidget>
#include <QFile>
#include <QLineEdit>
#include <QPushButton>

namespace ScIDE { namespace Settings {

Dialog::Dialog(Manager *settings, Session* session, bool projectOpenFromSession, bool projectOpenFromSettings, QWidget * parent):
    QDialog(parent),
    mManager(settings),
    mSession(session),
    projectOpenFromSession(projectOpenFromSession),
    projectOpenFromSettings(projectOpenFromSettings),
    ui( new Ui::ConfigDialog )
{
    ui->setupUi(this);

    QWidget *w;

    generalPage = new GeneralPage;
    ui->configPageStack->addWidget(generalPage);
    ui->configPageList->addItem (
        new QListWidgetItem(QIcon::fromTheme("preferences-system"), tr("General")));
    connect(this, SIGNAL(storeRequest(Manager*, Session*, bool)), generalPage, SLOT(store(Manager*, Session*, bool)));
    connect(this, SIGNAL(loadRequest(Manager*, Session*)), generalPage, SLOT(load(Manager*, Session*)));
    connect(generalPage, SIGNAL(useLanguageConfigFromSessionChanged(bool)), this, SLOT(switchSclangPage(bool)));

    bool projectOpen = Main::instance()->useLanguageConfigFromSession() ? projectOpenFromSession : projectOpenFromSettings;
    sclangPage = projectOpen ? (QWidget *) new SclangProjectPage : (QWidget *) new SclangPage;
    ui->configPageStack->addWidget(sclangPage);
    ui->configPageList->addItem (
        new QListWidgetItem(QIcon::fromTheme("applications-system"), tr("Interpreter")));
    connect(this, SIGNAL(storeRequest(Manager*, Session*, bool)), sclangPage, SLOT(store(Manager*, Session*, bool)));
    connect(this, SIGNAL(loadRequest(Manager*, Session*)), sclangPage, SLOT(load(Manager*, Session*)));

    w = new EditorPage;
    ui->configPageStack->addWidget(w);
    ui->configPageList->addItem (
        new QListWidgetItem(QIcon::fromTheme("accessories-text-editor"), tr("Editor")));
    connect(this, SIGNAL(storeRequest(Manager*, Session*, bool)), w, SLOT(store(Manager*, Session*, bool)));
    connect(this, SIGNAL(loadRequest(Manager*, Session*)), w, SLOT(load(Manager*, Session*)));

    w = new ShortcutsPage;
    ui->configPageStack->addWidget(w);
    ui->configPageList->addItem (
        new QListWidgetItem(QIcon::fromTheme("input-keyboard"), tr("Shortcuts")));
    connect(this, SIGNAL(storeRequest(Manager*, Session*, bool)), w, SLOT(store(Manager*, Session*, bool)));
    connect(this, SIGNAL(loadRequest(Manager*, Session*)), w, SLOT(load(Manager*, Session*)));

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(ui->buttonBox->button(QDialogButtonBox::Apply), SIGNAL(clicked()), this, SLOT(apply()));
    connect(ui->buttonBox->button(QDialogButtonBox::Reset), SIGNAL(clicked()), this, SLOT(reset()));


    ui->configPageList->setMinimumWidth( ui->configPageList->sizeHintForColumn(0) );

    reset();
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::accept()
{
    Q_EMIT( storeRequest(mManager, mSession, generalPage->useLanguageConfigFromSessionChecked()) );

    QDialog::accept();
}

void Dialog::reject()
{
    QDialog::reject();
}

void Dialog::apply()
{
    Q_EMIT( storeRequest(mManager, mSession, generalPage->useLanguageConfigFromSessionChecked()) );
    Main::instance()->applySettings();
}

void Dialog::reset()
{
    Q_EMIT( loadRequest(mManager, mSession) );
}

void Dialog::switchSclangPage(bool useLanguageConfigFromSession)
{
    ui->configPageStack->removeWidget(sclangPage);
    delete sclangPage;

    bool projectOpen = (useLanguageConfigFromSession && mSession != nullptr)? projectOpenFromSession : projectOpenFromSettings;
    if (projectOpen)
    {
        SclangProjectPage* temp = new SclangProjectPage;
        temp->loadLocal(mManager, mSession, useLanguageConfigFromSession);
        sclangPage = temp;
    } else {
        SclangPage* temp = new SclangPage;
        temp->loadLocal(mManager, mSession, useLanguageConfigFromSession);
        sclangPage = temp;
    }
    ui->configPageStack->insertWidget(1, sclangPage);
    connect(this, SIGNAL(storeRequest(Manager*, Session*, bool)), sclangPage, SLOT(store(Manager*, Session*, bool)));
    connect(this, SIGNAL(loadRequest(Manager*, Session*)), sclangPage, SLOT(load(Manager*, Session*)));

}

}} // namespace ScIDE::Settings
