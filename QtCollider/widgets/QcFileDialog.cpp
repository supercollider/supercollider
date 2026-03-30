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

#include "QcFileDialog.h"
#include "../QcObjectFactory.h"

QC_DECLARE_QOBJECT_FACTORY(QcFileDialog);

QcFileDialog::QcFileDialog(int fileMode, int acceptMode, const QString& startDir)
    : _fileMode(fileMode)
    , _acceptMode(acceptMode)
    , _startDir(startDir) {
    createDialog();
}

void QcFileDialog::createDialog() {
    if (dialog)
        return;

    dialog = new QFileDialog();
    dialog->setDirectory(QDir{_startDir});

    switch (_fileMode) {
    case QFileDialog::AnyFile:
        dialog->setFileMode(QFileDialog::AnyFile);
        break;
    case QFileDialog::ExistingFile:
        dialog->setFileMode(QFileDialog::ExistingFile);
        break;
    case QFileDialog::Directory:
        dialog->setFileMode(QFileDialog::Directory);
        break;
    case QFileDialog::ExistingFiles:
        dialog->setFileMode(QFileDialog::ExistingFiles);
        break;
    default:
        qcErrorMsg("File dialog created with invalid file mode!\n");
    }

    switch (_acceptMode) {
    case QFileDialog::AcceptOpen:
        dialog->setAcceptMode(QFileDialog::AcceptOpen);
        break;
    case QFileDialog::AcceptSave:
        dialog->setAcceptMode(QFileDialog::AcceptSave);
        break;
    default:
        qcErrorMsg("File dialog created with invalid accept mode!\n");
    }

    // Keep the wrapper object alive while the dialog is open.
    // Do not make QcFileDialog a child of the dialog to avoid accidental self-deletion on dialog close.
    dialog->setParent(this);

    connect(dialog, &QFileDialog::finished, this, &QcFileDialog::onFinished);
}
