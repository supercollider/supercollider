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
#include "sclang_page.hpp"
#include "editor_page.hpp"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDialogButtonBox>
#include <QStackedWidget>
#include <QListWidget>
#include <QFile>
#include <QLineEdit>
#include <QPushButton>

namespace ScIDE { namespace Settings {

Dialog::Dialog( QSettings *settings, QWidget * parent ):
    QDialog(parent),
    mSettings(settings),
    ui( new Ui::ConfigDialog )
{
    ui->setupUi(this);

    QWidget *w = new SclangPage;
    ui->configPageStack->addWidget(w);
    ui->configPageList->addItem (
        new QListWidgetItem(QIcon::fromTheme("applications-system"), "Interpreter"));
    connect(this, SIGNAL(storeRequest(QSettings*)), w, SLOT(store(QSettings*)));
    connect(this, SIGNAL(loadRequest(QSettings*)), w, SLOT(load(QSettings*)));

    w = new EditorPage;
    ui->configPageStack->addWidget(w);
    ui->configPageList->addItem (
        new QListWidgetItem(QIcon::fromTheme("accessories-text-editor"), "Editor"));
    connect(this, SIGNAL(storeRequest(QSettings*)), w, SLOT(store(QSettings*)));
    connect(this, SIGNAL(loadRequest(QSettings*)), w, SLOT(load(QSettings*)));

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(ui->buttonBox->button(QDialogButtonBox::Reset), SIGNAL(clicked()),
            this, SLOT(reset()));

    reset();
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::accept()
{
    Q_EMIT( storeRequest(mSettings) );

    QDialog::accept();
}

void Dialog::reject()
{
    QDialog::reject();
}

void Dialog::reset()
{
    Q_EMIT( loadRequest(mSettings) );
}

}} // namespace ScIDE::Settings
