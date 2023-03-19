/************************************************************************
 *
 * Copyright 2011 Jakob Leben (jakob.leben@gmail.com)
 *
 * This file is part of SuperCollider Qt GUI.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 ************************************************************************/

#pragma once

#include "../Common.h"

#include <QFileDialog>
#include <QPointer>

class QcFileDialog : public QObject {
    Q_OBJECT

public:
    /**
     * \param fileMode What the user may select in the dialog
     * \param acceptMode Whether for opening or saving files
     * \param startDir What location the directory will open at
     */
    Q_INVOKABLE QcFileDialog(int fileMode = QFileDialog::ExistingFile, int acceptMode = QFileDialog::AcceptOpen,
                             const QString& startDir = QDir::home().path());

    ~QcFileDialog() {
        if (dialog) {
            dialog->deleteLater();
        };
    }

    QFileDialog* theDialog() { return dialog; }

Q_SIGNALS:

    void accepted(QVariantList result);
    void rejected();

private Q_SLOTS:

    void show() {
        dialog->exec();
        dialog->deleteLater();
    }

    void onFinished(int res) {
        if (res == QDialog::Accepted) {
            QStringList files = dialog->selectedFiles();
            QVariantList varFiles;
            Q_FOREACH (QString f, files) { varFiles << QVariant(f); }
            Q_EMIT(accepted(varFiles));
        } else {
            Q_EMIT(rejected());
        }
    }

private:
    QPointer<QFileDialog> dialog;
};
