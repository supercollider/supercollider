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

#include <QButtonGroup>

#include "ui_settings_dialog.h"
#include "general_page.hpp"
#include "sclang_page.hpp"
#include "editor_page.hpp"
#include "shortcuts_page.hpp"
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

Dialog::Dialog(Manager* settings, QWidget* parent): QDialog(parent), mManager(settings), ui(new Ui::ConfigDialog) {
    ui->setupUi(this);

    auto* general_page = new GeneralPage;

    ui->configPageStack->addWidget(general_page);
    ui->configPageList->addItem(new QListWidgetItem(tr("General")));
    connect(this, &Dialog::storeRequest, general_page, &GeneralPage::store);
    connect(this, &Dialog::loadRequest, general_page, &GeneralPage::load);

    auto* sclang_page = new SclangPage;
    ui->configPageStack->addWidget(sclang_page);
    ui->configPageList->addItem(new QListWidgetItem(tr("Interpreter")));
    connect(this, &Dialog::storeRequest, sclang_page, &SclangPage::store);
    connect(this, &Dialog::loadRequest, sclang_page, &SclangPage::load);

    auto* editor_page = new EditorPage;
    ui->configPageStack->addWidget(editor_page);
    ui->configPageList->addItem(new QListWidgetItem(tr("Editor")));
    connect(this, &Dialog::storeRequest, editor_page, &EditorPage::store);
    connect(this, &Dialog::loadRequest, editor_page, &EditorPage::load);

    auto* shortcut_page = new ShortcutsPage;
    ui->configPageStack->addWidget(shortcut_page);
    ui->configPageList->addItem(new QListWidgetItem(tr("Shortcuts")));
    connect(this, &Dialog::storeRequest, shortcut_page, &ShortcutsPage::store);
    connect(this, &Dialog::loadRequest, shortcut_page, &ShortcutsPage::load);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &Dialog::accept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &Dialog::reject);
    connect(ui->buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked, this, &Dialog::apply);
    connect(ui->buttonBox->button(QDialogButtonBox::Reset), &QPushButton::clicked, this, &Dialog::reset);


    ui->configPageList->setMinimumWidth(ui->configPageList->sizeHintForColumn(0));

    reset();
}

Dialog::~Dialog() { delete ui; }

void Dialog::accept() {
    Q_EMIT(storeRequest(mManager));

    QDialog::accept();
}

void Dialog::reject() { QDialog::reject(); }

void Dialog::apply() {
    Q_EMIT(storeRequest(mManager));
    Main::instance()->applySettings();
}

void Dialog::reset() { Q_EMIT(loadRequest(mManager)); }

}} // namespace ScIDE::Settings
