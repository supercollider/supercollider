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

#include "../QcHelper.h"
#include <QComboBox>

class QcPopUpMenu : public QComboBox, QcHelper {
    Q_OBJECT
    Q_PROPERTY(QVariantList items READ dummyVariantList WRITE setItems);
    Q_PROPERTY(bool reactivationEnabled READ reactivationEnabled WRITE setReactivationEnabled)

public:
    QcPopUpMenu();
    bool reactivationEnabled() const { return _reactivation; }
    void setReactivationEnabled(bool b) { _reactivation = b; }

Q_SIGNALS:
    void action();

private Q_SLOTS:
    void doAction(int);
    void setChanged() { _changed = true; }
    void clearChanged() { _changed = false; }

private:
    void setItems(const QVariantList&);
    bool _changed;
    bool _reactivation;
};
