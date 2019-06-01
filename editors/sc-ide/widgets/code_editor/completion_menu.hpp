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

#include "../util/popup_widget.hpp"
#include "../../core/sc_introspection.hpp"

#include <QObject>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QListView>
#include <QHBoxLayout>
#include <QSortFilterProxyModel>
#include <QTextBrowser>

namespace ScIDE {

namespace ScLanguage {
struct Method;
struct Class;
}

class CompletionTextBrowser : public QTextBrowser {
    // FIXME: Workaround for bug #1452 - crash when dragging/copying text from completion help window
    // QT seems to delete the QMimeData before it's finished with it, causing a crash.
    // The only way to avoid is to return no MIME data at all.
    QMimeData* createMimeDataFromSelection() const { return NULL; };
};

class CompletionMenu : public PopUpWidget {
    Q_OBJECT

public:
    enum DataRole { CompletionRole = Qt::UserRole, MethodRole };

    CompletionMenu(QWidget* parent = 0);
    void addItem(QStandardItem* item);
    void addInfo(QString info);
    void adapt();
    void setCompletionRole(int role);
    QString currentText();
    const ScLanguage::Method* currentMethod();
    QString exec(const QRect& rect);
    QSortFilterProxyModel* model();
    QListView* view();

protected:
    virtual bool eventFilter(QObject* obj, QEvent* ev);

private:
    QListView* mListView;
    QStandardItemModel* mModel;
    QSortFilterProxyModel* mFilterModel;
    CompletionTextBrowser* mTextBrowser;
    QHBoxLayout* mLayout;
    int mCompletionRole;

private slots:
    void onAnchorClicked(const QUrl url);

signals:
    void itemChanged(int item);
    void infoClicked(QString info);
};

} // namespace ScIDE
