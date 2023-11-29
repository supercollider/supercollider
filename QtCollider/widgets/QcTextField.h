/************************************************************************
 *
 * Copyright 2010-2012 Jakob Leben (jakob.leben@gmail.com)
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

#include "../QcWidgetFactory.h"

#include <QLineEdit>
#include <QKeyEvent>

class QcTextField : public QLineEdit {
    Q_OBJECT

public:
    QcTextField() {}

protected:
    virtual void keyPressEvent(QKeyEvent* e) {
        // NOTE: We could use the returnPressed() signal, but that would still
        // propagate the event to parent, which we want to avoid.
        if (e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return)
            Q_EMIT(action());
        else
            QLineEdit::keyPressEvent(e);
    }

Q_SIGNALS:
    void action();
};

QC_DECLARE_QWIDGET_FACTORY(QcTextField);
