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

#include "documents_dialog.hpp"
#include "main_window.hpp"
#include "../core/main.hpp"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QHeaderView>
#include <QDir>
#include <QApplication>

namespace ScIDE {

DocumentsDialog::DocumentsDialog(Mode mode, QWidget* parent): QDialog(parent) { init(mode); }

DocumentsDialog::DocumentsDialog(const QList<Document*>& docs, Mode mode, QWidget* parent): QDialog(parent) {
    init(mode, docs);
}

void DocumentsDialog::init(Mode mode, const QList<Document*>& docs) {
    DocumentManager* mng = Main::documentManager();
    connect(mng, SIGNAL(changedExternally(Document*)), this, SLOT(onDocumentChangedExternally(Document*)));

    mMode = mode;

    mLabel = new QLabel;
    mLabel->setWordWrap(true);

    mDocTree = new QTreeWidget;
    mDocTree->setRootIsDecorated(false);

    QStringList headerLabels;
    headerLabels << tr("Document") << tr("Path", "File path.");
    if (mode == ExternalChange)
        headerLabels << tr("Status");
    mDocTree->setHeaderLabels(headerLabels);

    mDocTree->header()->setStretchLastSection(false);
    mDocTree->header()->setSectionResizeMode(1, QHeaderView::Stretch);
    if (mode == ExternalChange)
        mDocTree->header()->setSectionResizeMode(2, QHeaderView::ResizeToContents);

    switch (mode) {
    case ExternalChange:
        setWindowTitle(tr("Externally Changed Documents"));
        mLabel->setText(tr("The following documents have changed externally.\n\n"
                           "Apply the desired actions to selected documents, until the list is empty."));
        break;
    case Quit:
        setWindowTitle(tr("Unsaved Documents"));
        mLabel->setText(tr("The following documents have unsaved changes.\n\n"
                           "Apply desired actions to selected documents, until the list is empty."));
        break;
    }

    foreach (Document* doc, docs)
        addDocument(doc);

    QDialogButtonBox* dialogBtnBox = new QDialogButtonBox();
    QPushButton* btn;
    QPushButton* defaultBtn;
    if (mode == ExternalChange) {
        defaultBtn = btn = dialogBtnBox->addButton(tr("&Reload"), QDialogButtonBox::ActionRole);
        btn->setIcon(QIcon::fromTheme("view-refresh"));
        connect(btn, SIGNAL(clicked()), this, SLOT(reloadSelected()));

        btn = dialogBtnBox->addButton(tr("Over&write"), QDialogButtonBox::ActionRole);
        btn->setIcon(QIcon::fromTheme("document-save"));
        connect(btn, SIGNAL(clicked()), this, SLOT(saveSelected()));

        btn = dialogBtnBox->addButton(tr("&Ignore"), QDialogButtonBox::AcceptRole);
        btn->setIcon(QIcon::fromTheme("window-close"));
        connect(btn, SIGNAL(clicked()), this, SLOT(ignoreSelected()));

        btn = dialogBtnBox->addButton(tr("&Close"), QDialogButtonBox::AcceptRole);
        btn->setIcon(QIcon::fromTheme("window-close"));
        connect(btn, SIGNAL(clicked()), this, SLOT(closeSelected()));
    } else {
        defaultBtn = btn = dialogBtnBox->addButton(tr("&Save"), QDialogButtonBox::ActionRole);
        btn->setIcon(QIcon::fromTheme("document-save"));
        connect(btn, SIGNAL(clicked()), this, SLOT(saveSelected()));

        btn = dialogBtnBox->addButton(tr("&Discard"), QDialogButtonBox::ActionRole);
        btn->setIcon(QIcon::fromTheme("window-close"));
        connect(btn, SIGNAL(clicked()), this, SLOT(ignoreSelected()));

        btn = dialogBtnBox->addButton(tr("&Cancel"), QDialogButtonBox::RejectRole);
        btn->setIcon(QIcon::fromTheme("window-close"));
        connect(btn, SIGNAL(clicked()), this, SLOT(reject()));
    }

    QPushButton* selectAllBtn = new QPushButton(tr("Select &All"));
    connect(selectAllBtn, SIGNAL(clicked()), this, SLOT(selectAll()));

    QPushButton* selectNoneBtn = new QPushButton(tr("Select N&one"));
    connect(selectNoneBtn, SIGNAL(clicked()), this, SLOT(selectNone()));

    QLabel* iconLabel = new QLabel;
    iconLabel->setPixmap(QApplication::style()->standardIcon(QStyle::SP_MessageBoxWarning).pixmap(48, 48));
    iconLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    QHBoxLayout* lblBox = new QHBoxLayout();
    lblBox->addWidget(iconLabel);
    lblBox->addWidget(mLabel);

    QHBoxLayout* selectionBox = new QHBoxLayout();
    selectionBox->addWidget(selectAllBtn);
    selectionBox->addWidget(selectNoneBtn);

    QVBoxLayout* vbox = new QVBoxLayout();
    vbox->addLayout(lblBox);
    vbox->addWidget(mDocTree);
    vbox->addLayout(selectionBox);
    vbox->addWidget(dialogBtnBox);

    setLayout(vbox);

    defaultBtn->setDefault(true);
    defaultBtn->setFocus(Qt::OtherFocusReason);

    resize(500, 300);
}

void DocumentsDialog::addDocument(Document* doc) {
    Item* item = new Item(doc);
    mDocTree->addTopLevelItem(item);
}

void DocumentsDialog::selectAll() {
    int c = count();
    for (int i = 0; i < c; ++i)
        item(i)->setChecked(true);
}

void DocumentsDialog::selectNone() {
    int c = count();
    for (int i = 0; i < c; ++i)
        item(i)->setChecked(false);
}

void DocumentsDialog::saveSelected() {
    int i = 0;
    while (i < count()) {
        Item* itm = item(i);
        if (itm->isChecked()) {
            if (!MainWindow::save(itm->document()))
                return;
            delete itm;
        } else
            ++i;
    }
    if (!count())
        accept();
}

void DocumentsDialog::reloadSelected() {
    DocumentManager* mng = Main::documentManager();

    int i = 0;
    while (i < count()) {
        Item* itm = item(i);
        if (itm->isChecked()) {
            if (!mng->reload(itm->document()))
                return;
            delete itm;
        } else
            ++i;
    }
    if (!count())
        accept();
}

void DocumentsDialog::ignoreSelected() {
    int i = 0;
    while (i < count()) {
        Item* itm = item(i);
        if (itm->isChecked())
            delete itm;
        else
            ++i;
    }
    if (!count())
        accept();
}

void DocumentsDialog::closeSelected() {
    DocumentManager* mng = Main::documentManager();

    int i = 0;
    while (i < count()) {
        Item* itm = item(i);
        if (itm->isChecked()) {
            mng->close(itm->document());
            delete itm;
        } else
            ++i;
    }
    if (!count())
        accept();
}

void DocumentsDialog::onDocumentChangedExternally(Document* doc) {
    int c = count();
    for (int i = 0; i < c; ++i) {
        Item* itm = item(i);
        if (itm->document() == doc) {
            itm->update();
            return;
        }
    }

    addDocument(doc);
}

DocumentsDialog::Item::Item(Document* doc): mDoc(doc) { update(); }

void DocumentsDialog::Item::update() {
    setTitle(mDoc->title());
    setPath(mDoc->filePath());
    if (!mDoc->filePath().isEmpty() && !QFile::exists(mDoc->filePath())) {
        setStatus(tr("Removed"));
        setChecked(false);
    } else {
        setStatus(tr("Modified"));
        setChecked(true);
    }
}

}
