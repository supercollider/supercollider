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

#include "../core/doc_manager.hpp"

#include <QDialog>
#include <QTreeWidget>
#include <QLabel>

namespace ScIDE {

class Document;

class DocumentsDialog : public QDialog {
    Q_OBJECT

public:
    enum Mode { ExternalChange, Quit };

    DocumentsDialog(Mode, QWidget* parent = 0);
    DocumentsDialog(const QList<Document*>& docs, Mode, QWidget* parent = 0);
    void setText(const QString& text) { mLabel->setText(text); }
    void addDocument(Document*);
    int count() { return mDocTree->topLevelItemCount(); }

public slots:
    void selectAll();
    void selectNone();

private slots:
    void saveSelected();
    void reloadSelected();
    void ignoreSelected();
    void closeSelected();
    void onDocumentChangedExternally(Document*);

private:
    class Item : public QTreeWidgetItem {
    public:
        Item(Document* doc);
        Document* document() const { return mDoc; }
        bool isChecked() const { return checkState(0) == Qt::Checked; }
        void setChecked(bool checked) { setCheckState(0, checked ? Qt::Checked : Qt::Unchecked); }
        void update();

    private:
        void setTitle(const QString& title) { setText(0, title); }
        void setPath(const QString& path) { setText(1, path); }
        void setStatus(const QString& status) { setText(2, status); }
        Document* mDoc;
    };

    void init(Mode mode, const QList<Document*>& = QList<Document*>());
    Item* item(int idx) { return static_cast<Item*>(mDocTree->topLevelItem(idx)); }

    Mode mMode;
    QLabel* mLabel;
    QTreeWidget* mDocTree;
};

} // namespace ScIDE
