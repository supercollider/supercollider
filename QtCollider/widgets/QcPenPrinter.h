/************************************************************************
*
* Copyright 2011 Jonatan Liljedahl <lijon@kymatica.com>
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

#ifndef QC_PEN_PRINTER_H
#define QC_PEN_PRINTER_H

#include "../painting.h"
#include <QPrintDialog>
#include <QPrinter>
#include <QPainter>

#include <stdio.h>

class QcPenPrinter : public QObject
{
  Q_OBJECT
  Q_PROPERTY( QRect pageRect READ pageRect );
  Q_PROPERTY( QRect paperRect READ paperRect );
  Q_PROPERTY( int fromPage READ fromPage );
  Q_PROPERTY( int toPage READ toPage );

public:

  QcPenPrinter() {
  }

  ~QcPenPrinter() {
  }

  QRect pageRect() const { return printer.pageRect(); }
  QRect paperRect() const { return printer.paperRect(); }
  int fromPage() const { return printer.fromPage(); }
  int toPage() const { return printer.toPage(); }

Q_SIGNALS:

  void ok();
  void cancel();
  void printFunc();

private Q_SLOTS:

  void show() {
    QPrintDialog dialog(&printer);

    dialog.setWindowTitle( QString("Print Document") );
    dialog.setOptions (
      QAbstractPrintDialog::PrintToFile |
      QAbstractPrintDialog::PrintPageRange |
      QAbstractPrintDialog::PrintShowPageSize
    );
    if (dialog.exec() != QDialog::Accepted) {
      Q_EMIT( cancel() );
    } else {
      Q_EMIT( ok() );
    }
  }

  void print() {
    QPainter painter;
    painter.begin(&printer);
    QtCollider::beginPainting(&painter);
    Q_EMIT ( printFunc() );
    painter.end();
    QtCollider::endPainting();
  }

  void newPage() {
    printer.newPage();
  }

private:

  QPrinter printer;

};

#endif // QC_PEN_PRINTER_H
