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
#include "../../core/settings/manager.hpp"
#include "../../core/util/standard_dirs.hpp"

#include <yaml-cpp/yaml.h>


namespace ScIDE { namespace Settings {

SclangPage::SclangPage(QWidget* parent): QWidget(parent), ui(new Ui::SclangConfigPage) {
    ui->setupUi(this);

    ui->sclang_add_configfile->setIcon(QIcon::fromTheme("list-add"));
    ui->sclang_remove_configfile->setIcon(QIcon::fromTheme("list-remove"));

    ui->sclang_add_include->setIcon(QIcon::fromTheme("list-add"));
    ui->sclang_remove_include->setIcon(QIcon::fromTheme("list-remove"));

    ui->sclang_add_exclude->setIcon(QIcon::fromTheme("list-add"));
    ui->sclang_remove_exclude->setIcon(QIcon::fromTheme("list-remove"));

    ui->runtimeDir->setFileMode(QFileDialog::Directory);

    connect(ui->activeConfigFileComboBox, SIGNAL(currentIndexChanged(const QString&)), this,
            SLOT(changeSelectedLanguageConfig(const QString&)));
    connect(ui->sclang_add_configfile, SIGNAL(clicked()), this, SLOT(dialogCreateNewConfigFile()));
    connect(ui->sclang_remove_configfile, SIGNAL(clicked()), this, SLOT(dialogDeleteCurrentConfigFile()));

    connect(ui->sclang_add_include, SIGNAL(clicked()), this, SLOT(addIncludePath()));
    connect(ui->sclang_add_exclude, SIGNAL(clicked()), this, SLOT(addExcludePath()));

    connect(ui->sclang_remove_include, SIGNAL(clicked()), this, SLOT(removeIncludePath()));
    connect(ui->sclang_remove_exclude, SIGNAL(clicked()), this, SLOT(removeExcludePath()));

    connect(ui->sclang_post_inline_warnings, SIGNAL(stateChanged(int)), this, SLOT(markSclangConfigDirty()));
}

SclangPage::~SclangPage() { delete ui; }

void SclangPage::load(Manager* s) {
    s->beginGroup("IDE/interpreter");

    ui->autoStart->setChecked(s->value("autoStart").toBool());
    ui->runtimeDir->setText(s->value("runtimeDir").toString());
    ui->sclang_standalone_mode->setChecked(s->value("standalone").toBool());

    QStringList availConfigFiles = availableLanguageConfigFiles();
    QString configSelectedLanguageConfigFile = s->value("configFile").toString();

    ui->activeConfigFileComboBox->clear();
    ui->activeConfigFileComboBox->addItems(availConfigFiles);
    int index = availConfigFiles.indexOf(configSelectedLanguageConfigFile);
    if (index != -1)
        ui->activeConfigFileComboBox->setCurrentIndex(index);
    selectedLanguageConfigFile = configSelectedLanguageConfigFile; // Happens after setting the combobox entries, since
                                                                   // the code triggers stateChanged event.

    s->endGroup();

    readLanguageConfig();
}

void SclangPage::store(Manager* s) {
    s->beginGroup("IDE/interpreter");
    s->setValue("autoStart", ui->autoStart->isChecked());
    s->setValue("runtimeDir", ui->runtimeDir->text());
    s->setValue("configFile", ui->activeConfigFileComboBox->currentText());
    s->setValue("standalone", ui->sclang_standalone_mode->isChecked());
    s->endGroup();

    writeLanguageConfig();
}

void SclangPage::addIncludePath() {
    QString path = QFileDialog::getExistingDirectory(this, tr("ScLang include directories"));
    if (path.size())
        ui->sclang_include_directories->addItem(path);
    sclangConfigDirty = true;
}

void SclangPage::removeIncludePath() {
    foreach (QListWidgetItem* item, ui->sclang_include_directories->selectedItems()) {
        ui->sclang_include_directories->removeItemWidget(item);
        delete item;
    }
    sclangConfigDirty = true;
}

void SclangPage::addExcludePath() {
    QString path = QFileDialog::getExistingDirectory(this, tr("ScLang exclude directories"));
    if (path.size())
        ui->sclang_exclude_directories->addItem(path);
    sclangConfigDirty = true;
}

void SclangPage::removeExcludePath() {
    foreach (QListWidgetItem* item, ui->sclang_exclude_directories->selectedItems()) {
        ui->sclang_exclude_directories->removeItemWidget(item);
        delete item;
    }
    sclangConfigDirty = true;
}

void SclangPage::changeSelectedLanguageConfig(const QString& configPath) {
    selectedLanguageConfigFile = configPath;
    readLanguageConfig();
}

void SclangPage::readLanguageConfig() {
    // LATER: watch for changes

    QString configFile = languageConfigFile();

    QFileInfo configFileInfo(configFile);
    const bool configFileExists = configFileInfo.exists();

    if (!configFileExists)
        return;

    using namespace YAML;
    try {
        std::ifstream fin(configFile.toStdString());
        Node doc = YAML::Load(fin);
        if (doc) {
            const Node& includePaths = doc["includePaths"];
            if (includePaths && includePaths.IsSequence()) {
                ui->sclang_include_directories->clear();
                for (Node const& pathNode : includePaths) {
                    if (!pathNode.IsScalar())
                        continue;
                    std::string path = pathNode.as<std::string>();
                    if (!path.empty())
                        ui->sclang_include_directories->addItem(QString(path.c_str()));
                }
            }

            const Node& excludePaths = doc["excludePaths"];
            if (excludePaths && excludePaths.IsSequence()) {
                ui->sclang_exclude_directories->clear();
                for (Node const& pathNode : excludePaths) {
                    if (!pathNode.IsScalar())
                        continue;
                    std::string path = pathNode.as<std::string>();
                    if (!path.empty())
                        ui->sclang_exclude_directories->addItem(QString(path.c_str()));
                }
            }

            const Node& inlineWarnings = doc["postInlineWarnings"];
            if (inlineWarnings) {
                try {
                    bool postInlineWarnings = inlineWarnings.as<bool>();
                    ui->sclang_post_inline_warnings->setChecked(postInlineWarnings);
                } catch (...) {
                    qDebug() << "Warning: Cannot parse config file entry \"postInlineWarnings\"";
                }
            }
        }
    } catch (std::exception&) {
    }

    sclangConfigDirty = false;
}

void SclangPage::writeLanguageConfig() {
    if (!sclangConfigDirty)
        return;

    using namespace YAML;
    using std::ofstream;
    Emitter out;
    out.SetIndent(4);
    out.SetMapFormat(Block);
    out.SetSeqFormat(Block);
    out.SetBoolFormat(TrueFalseBool);

    out << BeginMap;

    out << Key << "includePaths";
    out << Value << BeginSeq;

    for (int i = 0; i != ui->sclang_include_directories->count(); ++i)
        out << ui->sclang_include_directories->item(i)->text().toStdString();
    out << EndSeq;

    out << Key << "excludePaths";
    out << Value << BeginSeq;
    for (int i = 0; i != ui->sclang_exclude_directories->count(); ++i)
        out << ui->sclang_exclude_directories->item(i)->text().toStdString();
    out << EndSeq;

    out << Key << "postInlineWarnings";
    out << Value << (ui->sclang_post_inline_warnings->checkState() == Qt::Checked);

    out << EndMap;
    ofstream fout(languageConfigFile().toStdString().c_str());
    fout << out.c_str();

    QMessageBox::information(this, tr("Sclang configuration file updated"),
                             tr("The SuperCollider language configuration has been updated. "
                                "Reboot the interpreter to apply the changes."));

    sclangConfigDirty = false;
}

QString SclangPage::languageConfigFile() {
    if (selectedLanguageConfigFile.isEmpty()) {
        selectedLanguageConfigFile = standardDirectory(ScConfigUserDir) + "/" + QStringLiteral("sclang_conf.yaml");
    }
    return selectedLanguageConfigFile;
}

QStringList SclangPage::availableLanguageConfigFiles() {
    QDir qdir = QDir(standardDirectory(ScConfigUserDir));
    QStringList fileFilters;
    fileFilters << "sclang_conf*.yaml";
    QFileInfoList configFileList = qdir.entryInfoList(fileFilters);
    QStringList canonicalPaths;
    foreach (QFileInfo aFile, configFileList) { canonicalPaths.append(aFile.canonicalFilePath()); }
    return canonicalPaths;
}

void SclangPage::dialogCreateNewConfigFile() {
    bool ok;
    QString text = QInputDialog::getText(this, tr("New Configuration File"),
                                         tr("Create configuration file 'sclang_conf_*.yaml' with '*' replaced by:"),
                                         QLineEdit::Normal, QDir::home().dirName(), &ok);

    if (ok && !text.isEmpty()) {
        QString proposedLanguageConfigFile = standardDirectory(ScConfigUserDir) + "/sclang_conf_" + text + ".yaml";
        if (QFile(proposedLanguageConfigFile).exists()) {
            QMessageBox::information(this, tr("File Already Exists"),
                                     tr("Configuration file already exists:\n%1").arg(proposedLanguageConfigFile));
        } else {
            selectedLanguageConfigFile = proposedLanguageConfigFile;
            sclangConfigDirty = true;
            writeLanguageConfig();

            int index = ui->activeConfigFileComboBox->count();
            ui->activeConfigFileComboBox->addItem(selectedLanguageConfigFile);
            ui->activeConfigFileComboBox->setCurrentIndex(index);
        }
    }
}

void SclangPage::dialogDeleteCurrentConfigFile() {
    int ret = QMessageBox::warning(this, tr("Delete Configuration File"),
                                   tr("Are you sure you want to delete the following configuration file?\nThis action "
                                      "is immediate and cannot be undone.\n")
                                       + selectedLanguageConfigFile,
                                   QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel);

    if (ret == QMessageBox::Ok) {
        QString pathBeingRemoved = selectedLanguageConfigFile;
        QFile::remove(pathBeingRemoved);
        ui->activeConfigFileComboBox->removeItem(ui->activeConfigFileComboBox->findText(pathBeingRemoved));
        if (ui->activeConfigFileComboBox->count() != 0) {
            ui->activeConfigFileComboBox->setCurrentIndex(0);
        }
    }
}
}} // namespace ScIDE::Settings
