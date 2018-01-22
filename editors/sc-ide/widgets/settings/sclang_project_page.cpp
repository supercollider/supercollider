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
#include <QDir>

#include <fstream>

#include "sclang_project_page.hpp"
#include "ui_settings_sclang_project.h"
#include "../../core/session_manager.hpp"
#include "../../core/settings/manager.hpp"
#include "../../core/util/standard_dirs.hpp"
#include "../core/main.hpp"

#include "yaml-cpp/yaml.h"


namespace ScIDE { namespace Settings {

SclangProjectPage::SclangProjectPage(QWidget *parent) : SclangPageGeneric(parent)
{
    connect(ui->sclang_add_include, SIGNAL(clicked()), this, SLOT(addIncludePath()));
    connect(ui->sclang_add_exclude, SIGNAL(clicked()), this, SLOT(addExcludePath()));

    connect(ui->sclang_remove_include, SIGNAL(clicked()), this, SLOT(removeIncludePath()));
    connect(ui->sclang_remove_exclude, SIGNAL(clicked()), this, SLOT(removeExcludePath()));

    connect(ui->sclang_post_inline_warnings, SIGNAL(stateChanged(int)), this, SLOT(markSclangConfigDirty()));
    connect(ui->sclang_exclude_default_paths, SIGNAL(stateChanged(int)), this, SLOT(markSclangConfigDirty()));

}

//NOTE: need to know in which settings to save, information which is not yet
// saved in the settings, so value passed directly via useLanguageConfigFromSession.
void SclangProjectPage::loadLocal( Manager *s, Session *session, bool useLanguageConfigFromSession)
{
    SclangPageGeneric::loadLocal(s);

    selectedLanguageConfigFile = ((useLanguageConfigFromSession && session != nullptr) ?
                                      static_cast<SettingsInterface *>(session)
                                    : static_cast<SettingsInterface *>(s))->getConfigFile();
    QString tempLanguageConfigFileDir = QFileInfo(selectedLanguageConfigFile).canonicalPath();
    qSelectedLanguageConfigFileDir = QDir(tempLanguageConfigFileDir);
    selectedLanguageConfigFileDir = tempLanguageConfigFileDir.toStdString();

    ui->activeConfigFileLabel->setText(QString("Current project file: ").append(selectedLanguageConfigFile));

    readLanguageConfig();

}

void SclangProjectPage::load( Manager *s, Session *session)
{
    SclangPageGeneric::load(s);

    selectedLanguageConfigFile = Main::instance()->getConfigFile();
    QString tempLanguageConfigFileDir = QFileInfo(selectedLanguageConfigFile).canonicalPath();
    qSelectedLanguageConfigFileDir = QDir(tempLanguageConfigFileDir);
    selectedLanguageConfigFileDir = tempLanguageConfigFileDir.toStdString();
    ui->activeConfigFileLabel->setText(QString("Current project file: ").append(selectedLanguageConfigFile));

    readLanguageConfig();

}

void SclangProjectPage::store( Manager *s, Session *session, bool useLanguageConfigFromSession)
{
    s->beginGroup("IDE/interpreter");
    s->setValue("autoStart", ui->autoStart->isChecked());
    s->setValue("runtimeDir", ui->runtimeDir->text());
    s->endGroup();

    writeLanguageConfig();
}

void SclangProjectPage::doRelativeProject(std::function<void(QString)> func, QString path) {
    if (path.size()) {
        if ( path.size() >= selectedLanguageConfigFileDir.size() &&
             std::equal( selectedLanguageConfigFileDir.begin(), selectedLanguageConfigFileDir.end(), path.begin() ) ) {
            func(qSelectedLanguageConfigFileDir.relativeFilePath(path));
        } else {
            func(path);
        }
    }
}

void SclangProjectPage::addIncludePath()
{
    QString projectDir = QFileInfo(Main::instance()->getConfigFile()).dir().absolutePath();
    QString path = QFileDialog::getExistingDirectory(this, tr("ScLang include directories"), projectDir);
    doRelativeProject([this](QString p){ ui->sclang_include_directories->addItem(p); }, path);
    sclangConfigDirty = true;
}

void SclangProjectPage::removeIncludePath()
{
    foreach (QListWidgetItem * item, ui->sclang_include_directories->selectedItems() ) {
        ui->sclang_include_directories->removeItemWidget(item);
        delete item;
    }
    sclangConfigDirty = true;
}

void SclangProjectPage::addExcludePath()
{
    QString projectDir = QFileInfo(Main::instance()->getConfigFile()).dir().absolutePath();
    QString path = QFileDialog::getExistingDirectory(this, tr("ScLang exclude directories"), projectDir);
    doRelativeProject([this](QString p){ ui->sclang_exclude_directories->addItem(p); }, path);
    sclangConfigDirty = true;
}

void SclangProjectPage::removeExcludePath()
{
    foreach (QListWidgetItem * item, ui->sclang_exclude_directories->selectedItems() ) {
        ui->sclang_exclude_directories->removeItemWidget(item);
        delete item;
    }
    sclangConfigDirty = true;
}

void SclangProjectPage::writeAddExtraFields(YAML::Emitter &out)
{
    out << YAML::Key << "project";
    out << YAML::Value << true;
}

}} // namespace ScIDE::Settings
