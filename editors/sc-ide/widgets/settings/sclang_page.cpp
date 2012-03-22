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

#include "sclang_page.hpp"
#include "ui_settings_sclang.h"

namespace ScIDE { namespace Settings {

SclangPage::SclangPage(QWidget *parent) :
    QWidget(parent),
    ui( new Ui::SclangConfigPage )
{
    ui->setupUi(this);
    ui->btnAddClassDir->setIcon( QIcon::fromTheme("list-add") );
    ui->btnRemoveClassDir->setIcon( QIcon::fromTheme("list-remove") );
    ui->runtimeDir->setFileMode(QFileDialog::Directory);
    ui->classDirChooser->setFileMode(QFileDialog::Directory);
}

SclangPage::~SclangPage()
{
    delete ui;
}

void SclangPage::load( QSettings *s )
{
    s->beginGroup("IDE/interpreter");
    ui->autoStart->setChecked( s->value("autoStart", true).toBool() );
    ui->command->setText( s->value("command").toString() );
    ui->runtimeDir->setText( s->value("runtimeDir").toString() );
    ui->configFile->setText( s->value("configFile").toString() );
    s->endGroup();
}

void SclangPage::store( QSettings *s )
{
    s->beginGroup("IDE/interpreter");
    s->setValue("autoStart", ui->autoStart->isChecked());
    s->setValue("command", ui->command->text());
    s->setValue("runtimeDir", ui->runtimeDir->text());
    s->setValue("configFile", ui->configFile->text());
    s->endGroup();
}

}} // namespace ScIDE::Settings
