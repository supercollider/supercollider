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

#include "QcPopUpMenu.h"
#include "../QcWidgetFactory.h"

QC_DECLARE_QWIDGET_FACTORY(QcPopUpMenu);

QcPopUpMenu::QcPopUpMenu(): _changed(false), _reactivation(false) {
    connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(setChanged()));
    connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(clearChanged()), Qt::QueuedConnection);

    connect(this, SIGNAL(activated(int)), this, SLOT(doAction(int)));
}

void QcPopUpMenu::setItems(const QVariantList& items) {
    clear();
    Q_FOREACH (const QVariant& item, items)
        addItem(item.toString());
}

void QcPopUpMenu::doAction(int choice) {
    if (_changed || _reactivation)
        Q_EMIT(action());
}
