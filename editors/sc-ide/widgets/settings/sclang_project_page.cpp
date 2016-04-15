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
#include "../../core/settings/manager.hpp"
#include "../../core/util/standard_dirs.hpp"

#include "yaml-cpp/yaml.h"


namespace ScIDE { namespace Settings {

SclangProjectPage::SclangProjectPage(QWidget *parent) :
    QWidget(parent),
	ui( new Ui::SclangProjectConfigPage )
{
    ui->setupUi(this);

    ui->sclang_add_include->setIcon( QIcon::fromTheme("list-add") );
    ui->sclang_remove_include->setIcon( QIcon::fromTheme("list-remove") );

    ui->sclang_add_exclude->setIcon( QIcon::fromTheme("list-add") );
    ui->sclang_remove_exclude->setIcon( QIcon::fromTheme("list-remove") );

    ui->runtimeDir->setFileMode(QFileDialog::Directory);

    connect(ui->sclang_add_include, SIGNAL(clicked()), this, SLOT(addIncludePath()));
    connect(ui->sclang_add_exclude, SIGNAL(clicked()), this, SLOT(addExcludePath()));

    connect(ui->sclang_remove_include, SIGNAL(clicked()), this, SLOT(removeIncludePath()));
    connect(ui->sclang_remove_exclude, SIGNAL(clicked()), this, SLOT(removeExcludePath()));

    connect(ui->sclang_post_inline_warnings, SIGNAL(stateChanged(int)), this, SLOT(markSclangConfigDirty()));
    connect(ui->sclang_exclude_default_paths, SIGNAL(stateChanged(int)), this, SLOT(markSclangConfigDirty()));

}

SclangProjectPage::~SclangProjectPage()
{
    delete ui;
}

void SclangProjectPage::load( Manager *s )
{
    s->beginGroup("IDE/interpreter");

    ui->autoStart->setChecked( s->value("autoStart").toBool() );
    ui->runtimeDir->setText( s->value("runtimeDir").toString() );

    selectedLanguageConfigFile = s->value("configFile").toString();
    QString tempLanguageConfigFileDir = QFileInfo(selectedLanguageConfigFile).canonicalPath();
    qSelectedLanguageConfigFileDir = QDir(tempLanguageConfigFileDir);
    selectedLanguageConfigFileDir = tempLanguageConfigFileDir.toStdString();
    project = s->value("project").toBool();

    ui->activeConfigFileLabel->setText(QString("Current project file: ").append(selectedLanguageConfigFile));

    s->endGroup();

    readLanguageConfig(selectedLanguageConfigFile);

}

void SclangProjectPage::store( Manager *s )
{
    s->beginGroup("IDE/interpreter");
    s->setValue("autoStart", ui->autoStart->isChecked());
    s->setValue("runtimeDir", ui->runtimeDir->text());
    s->setValue("project", true);
    QString configSelectedLanguageConfigFile = s->value("configFile").toString();
    s->endGroup();

    writeLanguageConfig(configSelectedLanguageConfigFile);

}

void SclangProjectPage::doRelativeProject(std::function<void(QString)> func, QString path) {
    std::string stdPath = path.toStdString();
    if( project && path.size() >= selectedLanguageConfigFileDir.size() &&
           std::equal( selectedLanguageConfigFileDir.begin(), selectedLanguageConfigFileDir.end(), path.begin() ) ) {
        func(qSelectedLanguageConfigFileDir.relativeFilePath(path));
    } else {
        func(path);
    }
}

void SclangProjectPage::addIncludePath()
{
    QString path = QFileDialog::getExistingDirectory(this, tr("ScLang include directories"));
    if (path.size())
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
    QString path = QFileDialog::getExistingDirectory(this, tr("ScLang exclude directories"));
    if (path.size())
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

void SclangProjectPage::readLanguageConfig(QString configFile)
{
    // LATER: watch for changes

    QFileInfo configFileInfo(configFile);
    const bool configFileExists = configFileInfo.exists();

    if (!configFileExists)
        return;

    using namespace YAML;
    try {
        std::ifstream fin(configFile.toStdString().c_str());
        Parser parser(fin);

        Node doc;
        while(parser.GetNextDocument(doc)) {
            const Node * includePaths = doc.FindValue("includePaths");
            if (includePaths && includePaths->Type() == NodeType::Sequence) {
                ui->sclang_include_directories->clear();
                for (Iterator it = includePaths->begin(); it != includePaths->end(); ++it) {
                    Node const & pathNode = *it;
                    if (pathNode.Type() != NodeType::Scalar)
                        continue;
                    std::string path;
                    pathNode.GetScalar(path);
                    ui->sclang_include_directories->addItem(QString(path.c_str()));
                }
            }

            const Node * excludePaths = doc.FindValue("excludePaths");
            if (excludePaths && excludePaths->Type() == NodeType::Sequence) {
                ui->sclang_exclude_directories->clear();
                for (Iterator it = excludePaths->begin(); it != excludePaths->end(); ++it) {
                    Node const & pathNode = *it;
                    if (pathNode.Type() != NodeType::Scalar)
                        continue;
                    std::string path;
                    pathNode.GetScalar(path);
                    ui->sclang_exclude_directories->addItem(QString(path.c_str()));
                }
            }

            const Node * inlineWarnings = doc.FindValue("postInlineWarnings");
            if (inlineWarnings) {
                try {
                    bool postInlineWarnings = inlineWarnings->to<bool>();
                    ui->sclang_post_inline_warnings->setChecked(postInlineWarnings);
                } catch(...) {
                    qDebug() << "Warning: Cannot parse config file entry \"postInlineWarnings\"";
                }
            }

            const Node * excludeDefaultPahts = doc.FindValue("excludeDefaultPaths");
            if (excludeDefaultPahts) {
                try {
                    bool excludeDefaultPahtsBool = excludeDefaultPahts->to<bool>();
                    ui->sclang_exclude_default_paths->setChecked(excludeDefaultPahtsBool);
                } catch(...) {
                    qDebug() << "Warning: Cannot parse config file entry \"excludeDefaultPaths\"";
                }
            }
        }
    } catch (std::exception & e) {
    }

    sclangConfigDirty = false;
}

void SclangProjectPage::writeLanguageConfig(QString configFile)
{
    if (!sclangConfigDirty)
        return;

    using namespace YAML; using std::ofstream;
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

    out << Key << "excludeDefaultPaths";
    out << Value << (ui->sclang_exclude_default_paths->checkState() == Qt::Checked);

    out << Key << "project";
    out << Value << true;

    out << EndMap;
    ofstream fout(configFile.toStdString().c_str());
    fout << out.c_str();

    QMessageBox::information(this, tr("Sclang configuration file updated"),
                             tr("The SuperCollider language configuration has been updated. "
                                "Reboot the interpreter to apply the changes."));

    sclangConfigDirty = false;
}

}} // namespace ScIDE::Settings
