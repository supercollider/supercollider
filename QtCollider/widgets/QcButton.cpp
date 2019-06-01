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

#include "QcButton.h"
#include "../QcWidgetFactory.h"
#include <QStylePainter>
#include <QStyle>

QC_DECLARE_QWIDGET_FACTORY(QcButton);

QcButton::QcButton(): QtCollider::Style::Client(this), currentState(0) {
    setAttribute(Qt::WA_AcceptTouchEvents);
    connect(this, SIGNAL(clicked()), this, SLOT(doAction()));
}

void QcButton::setStates(const QVariantList& statesArg) {
    if (!statesArg.count())
        return;

    states.clear();

    Q_FOREACH (const QVariant& var, statesArg) {
        QVariantList stateArg = var.toList();
        int count = stateArg.size();
        State state;
        if (count >= 1)
            state.text = stateArg[0].toString();
        if (count >= 2)
            state.textColor = stateArg[1].value<QColor>();
        if (count >= 3)
            state.buttonColor = stateArg[2].value<QColor>();
        states.append(state);
    }
    setState(0);
}

void QcButton::setState(int i) {
    int c = states.count();
    if (!c)
        return;

    currentState = qBound(0, i, c - 1);
    setText(states[currentState].text);

    update();
}

void QcButton::cycleStates() {
    if (states.size() < 2)
        return;
    int i = currentState + 1;
    if (i >= states.size())
        i = 0;
    setState(i);
}

void QcButton::doAction() {
    cycleStates();
    Q_EMIT(action((int)QApplication::keyboardModifiers()));
}

void QcButton::paintEvent(QPaintEvent* e) {
    using namespace QtCollider::Style;
    using QtCollider::Style::RoundRect;

    QStylePainter sp(this);
    QPalette plt(palette());

    State state;
    if (states.count())
        state = states[currentState];

    if (state.textColor.isValid())
        plt.setColor(QPalette::ButtonText, state.textColor);
    if (state.buttonColor.isValid())
        plt.setColor(QPalette::Button, state.buttonColor);

    QStyleOptionButton option;
    initStyleOption(&option);

    QStyleOptionButton mbiOption = option;

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    bool sunken = isDown();
    int radius = 2;
    bool hasMenu = (option.features & QStyleOptionButton::HasMenu);
    int mbiSize = hasMenu ? sp.style()->pixelMetric(QStyle::PM_MenuButtonIndicator, &option, this) : 0;

    if (sunken) {
        plt.setColor(QPalette::ButtonText, plt.color(QPalette::ButtonText).darker(120));
        option.rect.translate(1, 1);
    }

    option.palette = plt;

    QColor focusClr(hasFocus() ? focusColor() : QColor());

    QRect r(rect());

    RoundRect frame(r, radius);
    if (sunken)
        drawSunken(&p, plt, frame, plt.color(QPalette::Button), focusClr);
    else
        drawRaised(&p, plt, frame, plt.color(QPalette::Button), focusClr);

    p.setPen(plt.color(QPalette::ButtonText));

    option.rect.adjust(0, 0, -mbiSize, 0);
    sp.drawControl(QStyle::CE_PushButtonLabel, option);

    if (hasMenu) {
        mbiOption.rect = QRect(r.right() - mbiSize + 2, r.height() / 2 - mbiSize / 2 + 3, mbiSize - 6, mbiSize - 6);
        sp.style()->drawPrimitive(QStyle::PE_IndicatorArrowDown, &mbiOption, &p, this);
    }
}

void QcButton::setQcMenu(QcMenu* menu) { QPushButton::setMenu(menu); }

QcMenu* QcButton::qcmenu() const {
    QMenu* menu = QPushButton::menu();
    QcMenu* qcmenu = qobject_cast<QcMenu*>(menu);
    return qcmenu;
}

void QcButton::setIcon(const QtCollider::SharedImage& image) {
    if (image) {
        QIcon icon = QIcon(image->pixmap());
        QAbstractButton::setIcon(icon);
    } else {
        QAbstractButton::setIcon(QIcon());
    }
}
