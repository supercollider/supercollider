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
#include "QcMenu.h"
#include "../style/style.hpp"

#include <QPushButton>

class QcButton : public QPushButton, QcHelper, QtCollider::Style::Client {
    Q_OBJECT
    Q_PROPERTY(QVariantList states READ dummyVariantList WRITE setStates);
    Q_PROPERTY(int value READ getValue WRITE setValue);
    Q_PROPERTY(QColor focusColor READ focusColor WRITE setFocusColor);
    Q_PROPERTY(QcMenu* menu READ qcmenu WRITE setQcMenu);
    Q_PROPERTY(const QtCollider::SharedImage& icon READ icon WRITE setIcon);

public:
    QcButton();
Q_SIGNALS:
    void action(int);

protected:
#ifdef Q_WS_MAC
    bool hitButton(const QPoint&) const;
#endif
    virtual void paintEvent(QPaintEvent*);
private Q_SLOTS:
    void doAction();

private:
    struct State {
        QString text;
        QColor textColor;
        QColor buttonColor;
    };
    void setStates(const QVariantList&);
    void setValue(int val) { setState(val); }
    int getValue() const { return currentState; }

    void setQcMenu(QcMenu* menu);
    QcMenu* qcmenu() const;

    QtCollider::SharedImage icon() { return QtCollider::SharedImage(); }
    void setIcon(const QtCollider::SharedImage& image);

    void setState(int);
    void cycleStates();
    QList<State> states;
    int currentState;
};
