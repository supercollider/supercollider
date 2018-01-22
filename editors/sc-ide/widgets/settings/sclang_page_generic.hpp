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

#ifndef SCIDE_WIDGETS_SETTINGS_SCLANG_PAGE_GENERIC_HPP_INCLUDED
#define SCIDE_WIDGETS_SETTINGS_SCLANG_PAGE_GENERIC_HPP_INCLUDED

#include <QWidget>
#include <QMessageBox>
#include <QFileDialog>

#include <fstream>

#include <yaml-cpp/yaml.h>

#include "sclang_page_generic.hpp"
#include "../../core/session_manager.hpp"
#include "../../core/settings/manager.hpp"
#include "../../core/util/standard_dirs.hpp"
#include "../core/main.hpp"

namespace ScIDE {

struct Session;

namespace Settings {

class Manager;

template <class ui_type>
class SclangPageGeneric : public QWidget
{

public:

    explicit SclangPageGeneric(QWidget *parent = 0) :
        QWidget(parent),
        ui( new ui_type )
    {
        ui->setupUi(this);

        ui->sclang_add_include->setIcon( QIcon::fromTheme("list-add") );
        ui->sclang_remove_include->setIcon( QIcon::fromTheme("list-remove") );

        ui->sclang_add_exclude->setIcon( QIcon::fromTheme("list-add") );
        ui->sclang_remove_exclude->setIcon( QIcon::fromTheme("list-remove") );

        ui->runtimeDir->setFileMode(QFileDialog::Directory);
    }

    ~SclangPageGeneric()
    {
        delete ui;
    }

protected:

    void loadLocal(Manager *s) {
        s->beginGroup("IDE/interpreter");
        ui->autoStart->setChecked( s->value("autoStart").toBool() );
        ui->runtimeDir->setText( s->value("runtimeDir").toString() );
        s->endGroup();
    }

    void load(Manager *s)
    {
        s->beginGroup("IDE/interpreter");
        ui->autoStart->setChecked( s->value("autoStart").toBool() );
        ui->runtimeDir->setText( s->value("runtimeDir").toString() );
        s->endGroup();
    }

    void readLanguageConfig()
    {
        // LATER: watch for changes

        QString configFile = languageConfigFile();

        QFileInfo configFileInfo(configFile);
        const bool configFileExists = configFileInfo.exists();

        if (!configFileExists)
            return;

        using namespace YAML;
        try {
            std::ifstream fin(configFile.toStdString());
            Node doc = YAML::Load( fin );
            if ( doc ) {
                const Node & includePaths = doc[ "includePaths" ];
                if (includePaths && includePaths.IsSequence()) {
                    ui->sclang_include_directories->clear();
                    for( Node const & pathNode : includePaths ) {
                        if (!pathNode.IsScalar())
                            continue;
                        std::string path = pathNode.as<std::string>();
                        if ( !path.empty() )
                            ui->sclang_include_directories->addItem(QString(path.c_str()));
                    }
                }

                const Node & excludePaths = doc[ "excludePaths" ];
                if (excludePaths && excludePaths.IsSequence()) {
                    ui->sclang_exclude_directories->clear();
                    for( Node const & pathNode : excludePaths ) {
                        if (!pathNode.IsScalar())
                            continue;
                        std::string path = pathNode.as<std::string>();
                        if ( !path.empty() )
                            ui->sclang_exclude_directories->addItem(QString(path.c_str()));
                    }
                }

                const Node & inlineWarnings = doc[ "postInlineWarnings" ];
                if (inlineWarnings) {
                    try {
                        bool postInlineWarnings = inlineWarnings.as<bool>();
                        ui->sclang_post_inline_warnings->setChecked(postInlineWarnings);
                    } catch(...) {
                        qDebug() << "Warning: Cannot parse config file entry \"postInlineWarnings\"";
                    }
                }

                const Node & excludeDefaultPaths = doc["excludeDefaultPaths" ];
                if (excludeDefaultPaths) {
                    try {
                        bool excludeDefaultPathsBool = excludeDefaultPaths.as<bool>();
                        ui->sclang_exclude_default_paths->setChecked(excludeDefaultPathsBool);
                    } catch(...) {
                        qDebug() << "Warning: Cannot parse config file entry \"excludeDefaultPaths\"";
                    }
                }
            }
        } catch (std::exception &) {
        }

        sclangConfigDirty = false;
    }

    void writeLanguageConfig()
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

        writeAddExtraFields(out);

        out << EndMap;
        ofstream fout(languageConfigFile().toStdString().c_str());
        fout << out.c_str();

        QMessageBox::information(this, tr("Sclang configuration file updated"),
                                 tr("The SuperCollider language configuration has been updated. "
                                    "Reboot the interpreter to apply the changes."));

        sclangConfigDirty = false;
    }

    virtual void writeAddExtraFields(YAML::Emitter &out) = 0;

    QString languageConfigFile()
    {
        if (selectedLanguageConfigFile.isEmpty())
        {
            selectedLanguageConfigFile =
                    standardDirectory(ScConfigUserDir)
                    + "/" + QStringLiteral("sclang_conf.yaml");
        }
        return selectedLanguageConfigFile;
    }

    ui_type *ui;

    bool sclangConfigDirty;
    QString selectedLanguageConfigFile;
};

}} // namespace ScIDE::Settings

#endif // SCIDE_WIDGETS_SETTINGS_SCLANG_PAGE_GENERIC_HPP_INCLUDED
