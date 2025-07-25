/************************************************************************
 *
 * Copyright 2011-2012 Jakob Leben (jakob.leben@gmail.com)
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

#include "QcKnob.hpp"
#include "../QcWidgetFactory.h"
#include "../style/routines.hpp"

#include <QMouseEvent>
#include <QPainter>

using namespace QtCollider;

QC_DECLARE_QWIDGET_FACTORY(QcKnob);

QcKnob::QcKnob(): Style::Client(this), _value(0.f), _step(0.01), _mode(0), _centered(false) {
    setFocusPolicy(Qt::StrongFocus);
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    setAttribute(Qt::WA_AcceptTouchEvents);
}

void QcKnob::setValue(double val) {
    _value = qBound(0.0, val, 1.0);
    update();
}

void QcKnob::mousePressEvent(QMouseEvent* e) {
    if (_mode == 0) {
        setValue(value(e->pos()));
        Q_EMIT(action());
    }
    _prevPos = e->pos();
}

void QcKnob::mouseMoveEvent(QMouseEvent* e) {
    if (!e->buttons())
        return;

    if (_mode == 0) {
        double val = value(e->pos());
        if (!(val < 0.0 && _value > 0.5) && !(val > 1.0 && _value < 0.5))
            setValue(value(e->pos()));
        Q_EMIT(action());
    } else {
        int dif;
        dif = _mode == 1 ? e->pos().x() - _prevPos.x() : _prevPos.y() - e->pos().y();
        if (dif != 0) {
            double step = _step;
            modifyStep(&step);
            setValue(_value + dif * step);
            Q_EMIT(action());
        }
    }
    _prevPos = e->pos();
}

void QcKnob::paintEvent(QPaintEvent*) {
    using namespace QtCollider::Style;
    using QtCollider::Style::Ellipse;

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    const QPalette& plt(palette());
    QColor cGroove = plt.color(QPalette::Window).lighter(115);
    QColor cVal = plt.color(QPalette::ButtonText);

    QRect bounds(rect());
    int sz = qMin(bounds.width(), bounds.height());
    float sz_2 = sz * 0.5f;
    QPointF center = QRectF(bounds).center();

    p.translate(center);

    double rad = sz_2 * 0.75;
    Ellipse shKnob(QRectF(-rad, -rad, rad * 2, rad * 2));
    drawRaised(&p, plt, shKnob, plt.color(QPalette::Button).lighter(105), hasFocus() ? focusColor() : QColor());

    p.scale(sz_2, sz_2);

    QPen pen;
    pen.setWidthF(0.1);
    p.setBrush(Qt::NoBrush);

    QRectF r(-0.95, -0.95, 1.9, 1.9);

    p.save(); // groove & value indicator rotation

    double range = 300.0;

    p.rotate(-90 - range * 0.5);

    double valAng = -_value * range;

    if (_centered) {
        double min = qMin(valAng, -range * 0.5);
        double max = qMax(valAng, -range * 0.5);

        pen.setColor(cGroove);
        p.setPen(pen);
        p.drawArc(r, -range * 16.f, (min + range) * 16.f);
        p.drawArc(r, max * 16.f, -max * 16.f);

        pen.setColor(plt.color(QPalette::WindowText));
        p.setPen(pen);
        p.drawArc(r, min * 16.f, (max - min) * 16.f);
    } else {
        pen.setColor(cGroove);
        p.setPen(pen);
        p.drawArc(r, -range * 16.f, (valAng + range) * 16.f);

        pen.setColor(plt.color(QPalette::WindowText));
        p.setPen(pen);
        p.drawArc(r, valAng * 16.f, -valAng * 16.f);
    }

    p.restore(); // groove & value indicator rotation

    p.rotate((360.0 - range) * 0.5 - valAng);

    QLineF line(0, 0.05, 0, 0.55);
    pen.setColor(cVal);
    pen.setWidthF(0.15);
    pen.setCapStyle(Qt::RoundCap);
    p.setPen(pen);
    p.drawLine(line);
}

double QcKnob::value(const QPoint& pt) {
    double angle = QLineF(rect().center(), pt).angle();

    if (angle > 270.0)
        angle -= 270.0;
    else
        angle += 90;
    return (330 - angle) / 300.0;
}
