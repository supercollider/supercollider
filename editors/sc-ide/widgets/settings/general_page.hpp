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
class GeneralConfigPage;
}

namespace ScIDE { namespace Settings {

class Manager;

class GeneralPage : public QWidget {
    Q_OBJECT

public:
    GeneralPage(QWidget* parent = 0);
    ~GeneralPage();

public slots:
    void load(Manager*);
    void store(Manager*);

private slots:
    void onStartSessionNameChanged(const QString& text);

private:
    Ui::GeneralConfigPage* ui;
};

}} // namespace ScIDE::Settings
