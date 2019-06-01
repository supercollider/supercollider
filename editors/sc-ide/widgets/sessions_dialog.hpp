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

#include "../core/session_manager.hpp"

#include <QDialog>
#include <QListWidget>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QInputDialog>
#include <QMessageBox>

namespace ScIDE {

class SessionsDialog : public QDialog {
    Q_OBJECT

public:
    SessionsDialog(SessionManager* mng, QWidget* parent = 0): QDialog(parent), mSessionManager(mng) {
        setWindowTitle("Sessions");

        mList = new QListWidget;

        QPushButton* removeBtn = new QPushButton(tr("Delete"));
        QPushButton* renameBtn = new QPushButton(tr("Rename..."));

        QDialogButtonBox* dialogBtns = new QDialogButtonBox(QDialogButtonBox::Ok);


        QHBoxLayout* contentBox = new QHBoxLayout;
        QVBoxLayout* btnBox = new QVBoxLayout;
        btnBox->addWidget(renameBtn);
        btnBox->addWidget(removeBtn);
        btnBox->addStretch();
        contentBox->addWidget(mList);
        contentBox->addLayout(btnBox);

        QVBoxLayout* layout = new QVBoxLayout;
        layout->addLayout(contentBox);
        layout->addWidget(dialogBtns);

        setLayout(layout);

        connect(removeBtn, SIGNAL(clicked()), this, SLOT(removeCurrent()));
        connect(renameBtn, SIGNAL(clicked()), this, SLOT(renameCurrent()));
        connect(dialogBtns, SIGNAL(accepted()), this, SLOT(accept()));

        updateSessions();
    }

private slots:
    void removeCurrent() {
        QListWidgetItem* item = mList->currentItem();
        if (!item)
            return;

        QString name = item->text();

        if (QMessageBox::warning(this, tr("Delete Session"),
                                 tr("Are you sure you want to delete session '%1'?").arg(name),
                                 QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes)
            == QMessageBox::Yes) {
            mSessionManager->removeSession(name);
            updateSessions();
        }
    }

    void renameCurrent() {
        QListWidgetItem* item = mList->currentItem();
        if (!item)
            return;

        QString oldName = item->text();
        QString newName = QInputDialog::getText(this, tr("Rename Session"), tr("Enter a new name for the session:"),
                                                QLineEdit::Normal, oldName);
        if (newName.isEmpty() || newName == oldName)
            return;

        mSessionManager->renameSession(oldName, newName);
        updateSessions();
    }

private:
    void updateSessions() {
        mList->clear();
        QStringList sessions = mSessionManager->availableSessions();
        mList->addItems(sessions);

        const Session* currentSession = mSessionManager->currentSession();
        if (!currentSession)
            return;

        const int currentSessionIndex = sessions.indexOf(currentSession->name());
        if (currentSessionIndex != -1)
            mList->setCurrentRow(currentSessionIndex);
    }

    QListWidget* mList;
    SessionManager* mSessionManager;
};

} // namespace ScIDE
