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

#include <QDebug>
#include <QMessageBox>
#include <QInputDialog>
#include <QFile>

#include <fstream>

#include "sclang_page.hpp"
#include "ui_settings_sclang.h"
#include "../../core/session_manager.hpp"
#include "../../core/settings/manager.hpp"
#include "../../core/util/standard_dirs.hpp"
#include "../core/main.hpp"

#include <yaml-cpp/yaml.h>


namespace ScIDE { namespace Settings {

SclangPage::SclangPage(QWidget *parent) : SclangPageGeneric::SclangPageGeneric(parent)
{
    ui->sclang_add_configfile->setIcon( QIcon::fromTheme("list-add") );
    ui->sclang_remove_configfile->setIcon( QIcon::fromTheme("list-remove") );

    connect(ui->activeConfigFileComboBox, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(changeSelectedLanguageConfig(const QString &)));
    connect(ui->sclang_add_configfile, SIGNAL(clicked()), this, SLOT(dialogCreateNewConfigFile()));
    connect(ui->sclang_remove_configfile, SIGNAL(clicked()), this, SLOT(dialogDeleteCurrentConfigFile()));

    connect(ui->sclang_add_include, SIGNAL(clicked()), this, SLOT(addIncludePath()));
    connect(ui->sclang_add_exclude, SIGNAL(clicked()), this, SLOT(addExcludePath()));

    connect(ui->sclang_remove_include, SIGNAL(clicked()), this, SLOT(removeIncludePath()));
    connect(ui->sclang_remove_exclude, SIGNAL(clicked()), this, SLOT(removeExcludePath()));

    connect(ui->sclang_post_inline_warnings, SIGNAL(stateChanged(int)), this, SLOT(markSclangConfigDirty()));
    connect(ui->sclang_exclude_default_paths, SIGNAL(stateChanged(int)), this, SLOT(markSclangConfigDirty()));

}

void SclangPage::loadLocal(Manager *s, Session *session, bool useLanguageConfigFromSession)
{
    SclangPageGeneric::loadLocal(s);

    QStringList availConfigFiles = availableLanguageConfigFiles();
    QString configSelectedLanguageConfigFile = ((useLanguageConfigFromSession && session != nullptr) ? (SettingsInterface *) session : (SettingsInterface *) s)->getConfigFile();

    ui->activeConfigFileComboBox->clear();
    ui->activeConfigFileComboBox->addItems(availConfigFiles);
    int index = availConfigFiles.indexOf(configSelectedLanguageConfigFile);
    if (index != -1)
        ui->activeConfigFileComboBox->setCurrentIndex(index);
    selectedLanguageConfigFile = configSelectedLanguageConfigFile; // Happens after setting the combobox entries, since the code triggers stateChanged event.

    readLanguageConfig();
}

void SclangPage::load(Manager *s, Session *session)
{
    s->beginGroup("IDE/interpreter");
    ui->autoStart->setChecked( s->value("autoStart").toBool() );
    ui->runtimeDir->setText( s->value("runtimeDir").toString() );
    s->endGroup();

    QStringList availConfigFiles = availableLanguageConfigFiles();
    QString configSelectedLanguageConfigFile = Main::instance()->getConfigFile();

    ui->activeConfigFileComboBox->clear();
    ui->activeConfigFileComboBox->addItems(availConfigFiles);
    int index = availConfigFiles.indexOf(configSelectedLanguageConfigFile);
    if (index != -1)
        ui->activeConfigFileComboBox->setCurrentIndex(index);
    selectedLanguageConfigFile = configSelectedLanguageConfigFile; // Happens after setting the combobox entries, since the code triggers stateChanged event.

    readLanguageConfig();
}

void SclangPage::store( Manager *s, Session *session, bool useLanguageConfigFromSession)
{
    s->beginGroup("IDE/interpreter");
    s->setValue("autoStart", ui->autoStart->isChecked());
    s->setValue("runtimeDir", ui->runtimeDir->text());
    s->setValue("project", false);
    s->endGroup();

    //NOTE: need to know in which settings to save, information which is not yet
    // saved in the settings, so value passed directly via useLanguageConfigFromSession.
    ((useLanguageConfigFromSession && session != nullptr) ? (SettingsInterface *) session : (SettingsInterface *) s)->setConfigFile(ui->activeConfigFileComboBox->currentText());

    writeLanguageConfig();
}

void SclangPage::addIncludePath()
{
    QString path = QFileDialog::getExistingDirectory(this, tr("ScLang include directories"));
    if (path.size())
        ui->sclang_include_directories->addItem(path);
    sclangConfigDirty = true;
}

void SclangPage::removeIncludePath()
{
    foreach (QListWidgetItem * item, ui->sclang_include_directories->selectedItems() ) {
        ui->sclang_include_directories->removeItemWidget(item);
        delete item;
    }
    sclangConfigDirty = true;
}

void SclangPage::addExcludePath()
{
    QString path = QFileDialog::getExistingDirectory(this, tr("ScLang exclude directories"));
    if (path.size())
        ui->sclang_exclude_directories->addItem(path);
    sclangConfigDirty = true;
}

void SclangPage::removeExcludePath()
{
    foreach (QListWidgetItem * item, ui->sclang_exclude_directories->selectedItems() ) {
        ui->sclang_exclude_directories->removeItemWidget(item);
        delete item;
    }
    sclangConfigDirty = true;
}

void SclangPage::changeSelectedLanguageConfig(const QString & configPath) {
    selectedLanguageConfigFile = configPath;
    readLanguageConfig();
}

QStringList SclangPage::availableLanguageConfigFiles()
{
    QDir qdir = QDir(standardDirectory(ScConfigUserDir));
    QStringList fileFilters;
    fileFilters << "sclang_conf*.yaml";
    QFileInfoList configFileList = qdir.entryInfoList(fileFilters);
    QStringList canonicalPaths;
    foreach(QFileInfo aFile, configFileList){
        canonicalPaths.append(aFile.canonicalFilePath());
    }
    return canonicalPaths;
}

void SclangPage::dialogCreateNewConfigFile()
{
    bool ok;
    QString text = QInputDialog::getText
        (this, tr("New Configuration File"),
         tr("Create configuration file 'sclang_conf_*.yaml' with '*' replaced by:"), QLineEdit::Normal,
         QDir::home().dirName(), &ok);

   if (ok && !text.isEmpty())
   {
        QString proposedLanguageConfigFile =
                standardDirectory(ScConfigUserDir)
                + "/sclang_conf_" + text + ".yaml";
        if(QFile(proposedLanguageConfigFile).exists()){
            QMessageBox::information
                (this,
                 tr("File Already Exists"),
                 tr("Configuration file already exists:\n%1")
                 .arg(proposedLanguageConfigFile));
        }else{
            selectedLanguageConfigFile = proposedLanguageConfigFile;
            sclangConfigDirty = true;
            writeLanguageConfig();

            int index = ui->activeConfigFileComboBox->count();
            ui->activeConfigFileComboBox->addItem(selectedLanguageConfigFile);
            ui->activeConfigFileComboBox->setCurrentIndex(index);
       }
   }
}

void SclangPage::dialogDeleteCurrentConfigFile()
{
    int ret = QMessageBox::warning
        (this,
         tr("Delete Configuration File"),
         tr("Are you sure you want to delete the following configuration file?\nThis action is immediate and cannot be undone.\n")
         + selectedLanguageConfigFile,
         QMessageBox::Ok | QMessageBox::Cancel,
         QMessageBox::Cancel);

    if(ret == QMessageBox::Ok){
        QString pathBeingRemoved = selectedLanguageConfigFile;
        QFile::remove(pathBeingRemoved);
        ui->activeConfigFileComboBox->removeItem(ui->activeConfigFileComboBox->findText(pathBeingRemoved));
        if(ui->activeConfigFileComboBox->count() != 0){
          ui->activeConfigFileComboBox->setCurrentIndex(0);
        }
    }
}

}} // namespace ScIDE::Settings
