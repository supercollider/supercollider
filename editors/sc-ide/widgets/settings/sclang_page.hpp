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

#pragma once

#include <QWidget>

namespace Ui {
class SclangConfigPage;
}

namespace ScIDE { namespace Settings {

class Manager;

class SclangPage : public QWidget {
    Q_OBJECT

public:
    SclangPage(QWidget* parent = 0);
    ~SclangPage();

public Q_SLOTS:
    void load(Manager*);
    void store(Manager*);

private Q_SLOTS:
    void addIncludePath();
    void removeIncludePath();

    void addExcludePath();
    void removeExcludePath();
    void markSclangConfigDirty() { sclangConfigDirty = true; }
    void changeSelectedLanguageConfig(const QString& configPath);
    void dialogCreateNewConfigFile();
    void dialogDeleteCurrentConfigFile();

private:
    void readLanguageConfig();
    void writeLanguageConfig();
    QString languageConfigFile();
    QStringList availableLanguageConfigFiles();

    Ui::SclangConfigPage* ui;

    bool sclangConfigDirty;
    QString selectedLanguageConfigFile;
};

}} // namespace ScIDE::Settings
