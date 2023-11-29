/************************************************************************
 *
 * Copyright 2011 Jonatan Liljedahl <lijon@kymatica.com>
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

#include "../painting.h"
#include "../debug.h"
#include <QPrintDialog>
#include <QPrinter>
#include <QPainter>

class QcPenPrinter : public QObject {
    Q_OBJECT
    Q_PROPERTY(QRect pageRect READ pageRect);
    Q_PROPERTY(QRect paperRect READ paperRect);
    Q_PROPERTY(int fromPage READ fromPage);
    Q_PROPERTY(int toPage READ toPage);

public:
    QcPenPrinter(): dialog(0) {}

    ~QcPenPrinter() { delete dialog; }

    QRect pageRect() const { return printer.pageRect(); }
    QRect paperRect() const { return printer.paperRect(); }
    int fromPage() const { return printer.fromPage(); }
    int toPage() const { return printer.toPage(); }

Q_SIGNALS:

    void dialogDone(int);
    void printFunc();

private Q_SLOTS:

    void show() {
        if (!dialog) {
            dialog = new QPrintDialog(&printer);
            dialog->setWindowTitle(QStringLiteral("Print Document"));
            dialog->setOptions(QAbstractPrintDialog::PrintToFile | QAbstractPrintDialog::PrintPageRange
                               | QAbstractPrintDialog::PrintShowPageSize);
            connect(dialog, SIGNAL(finished(int)), this, SIGNAL(dialogDone(int)));
        }
        if (dialog->isVisible())
            qcWarningMsg("WARNING: Print dialog already open.");
        else
            dialog->exec();
    }

    void print() {
        QPainter painter;
        painter.begin(&printer);
        QtCollider::beginPainting(&painter);
        Q_EMIT(printFunc());
        painter.end();
        QtCollider::endPainting();
    }

    void newPage() { printer.newPage(); }

private:
    QPrinter printer;
    QPrintDialog* dialog;
};
