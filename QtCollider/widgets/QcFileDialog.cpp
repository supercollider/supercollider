/************************************************************************
*
* Copyright 2011 Jakob Leben (jakob.leben@gmail.com)
*
* This file is part of SuperCollider Qt GUI.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
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

class FileDialogFactory : public QcObjectFactory<QcFileDialog>
{
  void initialize( QObjectProxy *, QcFileDialog *dialog, QList<QVariant> & args ) {
    QFileDialog *d = dialog->theDialog();
    if( args.count() > 0 ) {
      int mode = args[0].toInt();
      switch(mode) {
        case QFileDialog::AnyFile:
          d->setFileMode( QFileDialog::AnyFile ); break;
        case QFileDialog::ExistingFile:
          d->setFileMode( QFileDialog::ExistingFile ); break;
        case QFileDialog::Directory:
          d->setFileMode( QFileDialog::Directory ); break;
        case QFileDialog::ExistingFiles:
          d->setFileMode( QFileDialog::ExistingFiles ); break;
        default:
          qcErrorMsg( "File dialog created with invalid file mode!\n");
      }
    }
    if( args.count() > 1 ) {
      int mode = args[1].toInt();
      switch(mode) {
        case QFileDialog::AcceptOpen:
          d->setAcceptMode( QFileDialog::AcceptOpen ); break;
        case QFileDialog::AcceptSave:
          d->setAcceptMode( QFileDialog::AcceptSave ); break;
        default:
          qcErrorMsg( "File dialog created with invalid accept mode!\n");
      }
    }
  }
};

static FileDialogFactory fileDialogFactory;
