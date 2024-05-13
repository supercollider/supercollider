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

#include <QCheckBox>

class QcCheckBox : public QCheckBox {
    Q_OBJECT
    Q_PROPERTY(bool value READ value WRITE setValue);

public:
    QcCheckBox() { connect(this, SIGNAL(clicked()), this, SIGNAL(action())); }
Q_SIGNALS:
    void action();

private:
    bool value() { return isChecked(); }
    void setValue(bool val) { setChecked(val); }
};

QC_DECLARE_QWIDGET_FACTORY(QcCheckBox);
