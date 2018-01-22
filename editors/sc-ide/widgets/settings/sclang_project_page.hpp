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

#ifndef SCIDE_WIDGETS_SETTINGS_SCLANG_PROJECT_PAGE_HPP_INCLUDED
#define SCIDE_WIDGETS_SETTINGS_SCLANG_PROJECT_PAGE_HPP_INCLUDED

#include <QDir>
#include <functional>

#include <yaml-cpp/yaml.h>

#include "sclang_page_generic.hpp"
#include "ui_settings_sclang_project.h"

namespace ScIDE {

struct Session;

namespace Settings {

class Manager;

class SclangProjectPage : public SclangPageGeneric<Ui::SclangProjectConfigPage>
{
    Q_OBJECT

public:
    explicit SclangProjectPage(QWidget *parent = 0);
    void loadLocal( Manager *, Session *, bool);

public Q_SLOTS:
    void load( Manager *, Session *);
    void store( Manager *, Session *, bool);

private Q_SLOTS:
    void addIncludePath();
    void removeIncludePath();
    void addExcludePath();
    void removeExcludePath();
    void markSclangConfigDirty() { sclangConfigDirty = true; }

private:
    void writeAddExtraFields(YAML::Emitter &) override;
    void doRelativeProject(std::function<void(QString)> func, QString path);

    std::string selectedLanguageConfigFileDir;
    QDir qSelectedLanguageConfigFileDir;
};

}} // namespace ScIDE::Settings

#endif // SCIDE_WIDGETS_SETTINGS_SCLANG_PAGE_HPP_INCLUDED
