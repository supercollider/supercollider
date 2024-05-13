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
#include <QStandardItemModel>
#include <QSortFilterProxyModel>

class QTreeWidgetItem;

namespace Ui {
class ShortcutConfigPage;
}

namespace ScIDE { namespace Settings {

class Manager;

class ShortcutsPage : public QWidget {
    Q_OBJECT

public:
    ShortcutsPage(QWidget* parent = 0);
    ~ShortcutsPage();

public Q_SLOTS:
    void load(Manager*);
    void store(Manager*);
    void filterBy(const QString&);

private Q_SLOTS:
    void showItem(QTreeWidgetItem*, QTreeWidgetItem*);
    void apply();

private:
    enum ItemDataRole {
        ActionRole = Qt::UserRole,
        SettingsKeyRole,
        DefaultSequenceRole,
        CustomSequenceRole,
        EditedRole
    };

    void addAction(QAction*, Manager*);
    void applyTo(QTreeWidgetItem*);
    void updateItem(QTreeWidgetItem*);
    QKeySequence activeItemSequence(QTreeWidgetItem*);
    bool confirmOverride(const QKeySequence& duplicateSequence, QTreeWidgetItem* duplicateItem);

    Ui::ShortcutConfigPage* ui;
    QStandardItemModel* mActionModel;
    QSortFilterProxyModel* mFilter;
};

}} // namespace ScIDE::Settings
