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

#include <QDialog>

namespace Ui {
class ConfigDialog;
}

namespace ScIDE { namespace Settings {

class Manager;

class Dialog : public QDialog {
    Q_OBJECT

public:
    Dialog(Manager* settings, QWidget* parent = 0);
    ~Dialog();

public Q_SLOTS:

    virtual void accept();
    virtual void reject();
    virtual void reset();
    void apply();

Q_SIGNALS:
    void storeRequest(Manager*);
    void loadRequest(Manager*);

private:
    Manager* mManager;
    Ui::ConfigDialog* ui;
};

}} // namespace ScIDE::Settings
