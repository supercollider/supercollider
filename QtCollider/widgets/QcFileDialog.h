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

#ifndef QC_FILE_DIALOG_H
#define QC_FILE_DIALOG_H

#include "../Common.h"

#include <QFileDialog>

class QcFileDialog : public QObject
{
  Q_OBJECT

public:

  QcFileDialog() {
    dialog = new QFileDialog();
    dialog->setDirectory( QDir::home() );
    setParent( dialog );
    connect( dialog, SIGNAL(finished(int)), this, SLOT(onFinished(int)) );
  }

  QFileDialog *theDialog() { return dialog; }

Q_SIGNALS:

  void accepted( VariantList result );
  void rejected();

private Q_SLOTS:

  void show() {
    dialog->exec();
    dialog->deleteLater();
  }

  void onFinished( int res ) {
    if( res == QDialog::Accepted ) {
      QStringList files = dialog->selectedFiles();
      VariantList varFiles;
      Q_FOREACH( QString f, files ) {
        varFiles.data << QVariant( f );
      }
      Q_EMIT( accepted( varFiles ) );
    }
    else {
      Q_EMIT( rejected() );
    }
  }

private:

  QFileDialog *dialog;
};

#endif // QC_FILE_DIALOG_H
