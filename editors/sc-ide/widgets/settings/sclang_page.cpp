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

#include <fstream>

#include "sclang_page.hpp"
#include "ui_settings_sclang.h"
#include "../../core/settings/manager.hpp"

#include "common/SC_DirUtils.h"

#include "yaml-cpp/yaml.h"


namespace ScIDE { namespace Settings {

SclangPage::SclangPage(QWidget *parent) :
    QWidget(parent),
    ui( new Ui::SclangConfigPage )
{
    ui->setupUi(this);

    ui->sclang_add_include->setIcon( QIcon::fromTheme("list-add") );
    ui->sclang_add_exclude->setIcon( QIcon::fromTheme("list-add") );
    ui->sclang_remove_include->setIcon( QIcon::fromTheme("list-remove") );
    ui->sclang_remove_exclude->setIcon( QIcon::fromTheme("list-remove") );

    ui->runtimeDir->setFileMode(QFileDialog::Directory);

    connect(ui->sclang_add_include, SIGNAL(clicked()), this, SLOT(addIncludePath()));
    connect(ui->sclang_add_exclude, SIGNAL(clicked()), this, SLOT(addExcludePath()));

    connect(ui->sclang_remove_include, SIGNAL(clicked()), this, SLOT(removeIncludePath()));
    connect(ui->sclang_remove_exclude, SIGNAL(clicked()), this, SLOT(removeExcludePath()));

    connect(ui->sclang_post_inline_warnings, SIGNAL(stateChanged(int)), this, SLOT(markSclangConfigDirty()));
}

SclangPage::~SclangPage()
{
    delete ui;
}

void SclangPage::load( Manager *s )
{
    s->beginGroup("IDE/interpreter");
    ui->autoStart->setChecked( s->value("autoStart").toBool() );
    ui->runtimeDir->setText( s->value("runtimeDir").toString() );
    s->endGroup();

    readLanguageConfig();
}

void SclangPage::store( Manager *s )
{
    s->beginGroup("IDE/interpreter");
    s->setValue("autoStart", ui->autoStart->isChecked());
    s->setValue("runtimeDir", ui->runtimeDir->text());
    s->endGroup();

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

void SclangPage::readLanguageConfig()
{
    // LATER: watch for changes

    QString configFile = languageConfigFile();

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
        }
    } catch (std::exception & e) {
    }

    sclangConfigDirty = false;
}

void SclangPage::writeLanguageConfig()
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

    out << EndMap;
    ofstream fout(languageConfigFile().toStdString().c_str());
    fout << out.c_str();

    QMessageBox::information(this, tr("ScLang configuration file updated"),
                             tr("The ScLang configuration has been updated. "
                                "Recompile the class library to apply changes."));

    sclangConfigDirty = false;
}

QString SclangPage::languageConfigFile()
{
    char configDir[PATH_MAX];
    sc_GetUserConfigDirectory(configDir, PATH_MAX);

    QString configFile = QString(configDir) + "/" + QString("sclang_conf.yaml");
    return configFile;
}

}} // namespace ScIDE::Settings
